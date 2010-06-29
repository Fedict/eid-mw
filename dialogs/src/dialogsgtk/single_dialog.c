/* ****************************************************************************
 * eID Middleware Project.
 * Copyright (C) 2008-2010 FedICT.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.
**************************************************************************** */

#include <sys/wait.h>
#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <glib/gi18n.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <glib/gi18n.h>
#include <sys/stat.h>
#include <stdio.h>
#include "single_dialog.h"

#define	MIN_CMDLINE_PATH_BYTES 14

void sdialog_init(SingleDialog* sdialog, char* did)
{
	char default_tempdir[]="/tmp";
    char* tmpdir=getenv("TMPDIR");
    if(tmpdir==NULL)
        tmpdir=default_tempdir;
    snprintf(sdialog->pid_path,sizeof(sdialog->pid_path)-2,"%s/.%s-%d.pid",tmpdir,did,getuid());
	strncpy(sdialog->unique_id,did,sizeof(sdialog->unique_id)-2);
}

int sdialog_write_pid(SingleDialog* sdialog)
{
	int 	pid_fd,result=-1;
	char 	tmp_str[32];

	fprintf(stderr,"I'm %d\n",getpid());

	do { pid_fd=open(sdialog->pid_path,O_WRONLY|O_EXCL|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH); } while (pid_fd==-1 && errno==EINTR);
	if(pid_fd>0)
	{
		int nwritten=0;
		snprintf(tmp_str,sizeof(tmp_str),"%d",getpid());
		do { nwritten=write(pid_fd,tmp_str,strlen(tmp_str)); } while (nwritten==-1 && errno==EINTR);
		if(nwritten==strlen(tmp_str))
			result=0;
	}

	fprintf(stderr,"sdialog_write_pid about to return %d\n",result);
	return result;
}

pid_t sdialog_read_pid(SingleDialog* sdialog)
{
	char	tmp_str[16];
	int 	pid_fd;
	pid_t	pid;

	fprintf(stderr,"sdialog_read_pid\n");
	do { pid_fd=open(sdialog->pid_path,O_RDONLY); } while (pid_fd==-1 && errno==EINTR);

	fprintf(stderr,"open returns %d\n",pid_fd);
	if(pid_fd>0)
	{
		size_t nread=0;
		do { nread=read(pid_fd,tmp_str,sizeof(tmp_str)-2); } while (pid_fd==-1 && errno==EINTR);
		if(nread>0)						// if something read
		{
			tmp_str[nread]='\0';			// 0-sdialog_terminate to make valid C string
			pid=strtoul(tmp_str,NULL,10);	// convert to pid_t
		}

		close(pid_fd);
		return pid;
	}
	return -1;
}
	

int read_proc_cmdline(char* cmdline, size_t max_cmd_size, pid_t pid)
{
	char 	tmp_str[32];
	ssize_t nread=0;

	fprintf(stderr,"read_proc_cmdline[%d]\n",pid);
	
	if(snprintf(tmp_str,sizeof(tmp_str)-2,"/proc/%d/cmdline",pid) >= MIN_CMDLINE_PATH_BYTES)
	{
		fprintf(stderr,"opening [%s]\n",tmp_str);

		int proc_fd;
		do { proc_fd=open(tmp_str,O_RDONLY); } while (proc_fd==-1 && errno==EINTR);

		if(proc_fd>0)
		{
			fprintf(stderr,"reading from [%s]\n",tmp_str);
			do { nread=read(proc_fd,cmdline,max_cmd_size); } while (proc_fd==-1 && errno==EINTR);

			
			fprintf(stderr,"read [%d] bytes from %s\n",nread,tmp_str);
			if(nread>0)
				cmdline[nread]='\0';			// but make sure the whole is properly sdialog_terminated
			close(proc_fd);
			fprintf(stderr,"commandline=[%s]\n",cmdline);
			return 0;
		}
	}
	return -1;
}

inline int running(pid_t pid)
{
	return kill(pid,0);
}

int sdialog_terminate(SingleDialog* sdialog, pid_t target)
{
	char 	tmp_str[PATH_MAX];
	int 	attempts;
	
	fprintf(stderr,"sdialog_terminate %d\n",target);

	if(running(target)==0 && (read_proc_cmdline(tmp_str,sizeof(tmp_str),target)==0) && (strstr(tmp_str,sdialog->unique_id)!=NULL))			// if it contains our command-line, it's a former instance
	{
		fprintf(stderr,"ours to kill\n");
		for(attempts=0;attempts<3 && running(target)==0;attempts++)
		{
			kill(target,SIGTERM);									// tell it to make room for the living
			usleep(100000);											// give it 100ms to do so
		}

		for(attempts=0;attempts<3 && running(target)==0;attempts++)
		{
			kill(target,SIGKILL);									// insist
			usleep(100000);											// give it 100ms to do so
		}
	}
	else
	{
		fprintf(stderr,"not ours to kill\n");
	}

	if(running(target)==-1 && errno==ESRCH)
	{
		fprintf(stderr,"Kill succeeds\n");
		return 0;
	}
	else
	{
		fprintf(stderr,"Kill failed\n");
	}

	return -1;
}

int sdialog_terminate_active(SingleDialog* sdialog)
{
	pid_t active=sdialog_read_pid(sdialog);
	fprintf(stderr,"sdialog_read_pid returns %d\n",active);
	if(active>0)
	{
		fprintf(stderr,"active dialog found - terminating\n");
		if(sdialog_terminate(sdialog,active)==0)	
		{
			fprintf(stderr,"removing active pid file\n");
			if(unlink(sdialog->pid_path)==0)	
				fprintf(stderr,"active pid file removed\n");
			else
				fprintf(stderr,"can't unlink interloper pid file\n");
		}
	}
}

int sdialog_lock(SingleDialog* sdialog)
{

	int	pid_fd,attempts=3;

	while((pid_fd=sdialog_write_pid(sdialog))<0 && attempts-->0)								// while we can't acquire exclusive lock
	{
		fprintf(stderr,"sdialog_write_pid fails (pid_fd=%d) - getting interloper\n",pid_fd);	
		sdialog_terminate_active(sdialog);									// attempts to destroy interlopers
	}

	if(pid_fd>=0)
	{
		fprintf(stderr,"lock acquired\n");
		close(pid_fd);
		return 0;
	}

	fprintf(stderr,"lock failed\n");
	return -1;
}

int sdialog_unlock(SingleDialog* sdialog)
{
	return unlink(sdialog->pid_path);
}

/* the sdialog_call_modal function borrows from readpass.c in the OpenSSH distribution, whose Copyright is as follows: 
 *
 * START OF extra (C) NOTICE FOR sdialog_call_modal() 

/* $OpenBSD: readpass.c,v 1.47 2006/08/03 03:34:42 deraadt Exp $ */
/*
 * Copyright (c) 2001 Markus Friedl.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * END OF (C) NOTICE for sdialog_call_modal()
 */

#define POSIX_SOURCE 1

// execute dialog in a child process, and leave it running
///////////////////////////////////////////////////////////
void sdialog_call(char* path, char* msg)
{
    pid_t pid=fork();
    if(pid<0)
	{
		perror("call_dialog/fork");
        return;
    }

    if(pid==0)
	{
       	umask(0);
		chdir("/");
		execlp(path,path,msg,(char*)0);
		perror("call_dialog/child/execlp");
		exit(1);
    }
}

// execute dialog in a child process, read back it's stdout via a pipe, wait for it to exit
///////////////////////////////////////////////////////////////////////////////////////////
char* sdialog_call_modal(char* path,const char* msg)
{
    pid_t pid;
    size_t len;
    char *response;
    int p[2], status, ret;
    char buf[1024];

	fprintf(stderr,"... fflush\n");
    if(fflush(stdout)!=0)
	{
        perror("call_dialog_result/fflush");
		return NULL;
	}

	fprintf(stderr,"... pipe\n");
    if(pipe(p)<0)
	{
		perror("call_dialog_result/pipe");
        return NULL;
    }

	fprintf(stderr,"... fork\n");
    if((pid=fork())<0)
	{
		perror("call_dialog_result/fork");
        return NULL;
    }

    if(pid==0)
	{
		fprintf(stderr,"*** in child\n");
		fprintf(stderr,"*** DISPLAY=%s\n",getenv("DISPLAY"));
       	umask(0);
		chdir("/");
        close(p[0]);

        if(dup2(p[1],STDOUT_FILENO)<0)
		{
			perror("call_dialog_result/child/dup2");
			exit(1);
		}

		fprintf(stderr,"*** exec %s\n",path);
		execlp(path,path,msg,(char*)0);
		perror("call_dialog_result/child/execlp");
		exit(1);
    }

	fprintf(stderr,"... child PID=%d\n",pid);

	fprintf(stderr,"... reading result\n");
    close(p[1]);
    len=ret=0;
    do
	{
        ret=read(p[0],buf+len,sizeof(buf)-1-len);
        if(ret==-1 && errno==EINTR)
            continue;
        if(ret<=0)
            break;
        len+=ret;
    }
	while(sizeof(buf)-1-len>0);

    buf[len]='\0';
    close(p[0]);

	fprintf(stderr,"... waiting for child to die\n");
    while(waitpid(pid,&status,0)<0)
        if(errno!=EINTR)
            break;

    if(!WIFEXITED(status) || WEXITSTATUS(status)!=0)
	{
		fprintf(stderr,"... child died badly\n");
        memset(buf,0,sizeof(buf));
        return NULL;
    }

	fprintf(stderr,"... child died normally\n");
    buf[strcspn(buf,"\r\n")]='\0';
    response=strdup(buf);
    memset(buf,0,sizeof(buf));
    return response;
}
