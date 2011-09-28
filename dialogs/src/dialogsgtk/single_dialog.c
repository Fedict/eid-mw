/* ****************************************************************************
 * eID Middleware Project.
 * Copyright (C) 2008-2011 FedICT.
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
#include <libgen.h>
#include <signal.h>
#include "single_dialog.h"

#define	MIN_CMDLINE_PATH_BYTES 14


#ifdef DEBUG
#define DPRINTF(format,args...) fprintf(stderr, format , ## args)
#define DERROR(label) perror(label)
#else
#define DPRINTF
#define DERROR
#endif


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

// execute dialog in a child process, leave it running
///////////////////////////////////////////////////////////////////////////////////////////
pid_t sdialog_call(const char* path,const char* msg)
{
    pid_t pid;

	DPRINTF("sdialog_call/fork\n");
    if((pid=fork())<0)
	{
		DERROR("sdialog_call/fork");
        return 0L;
    }

    if(pid==0)
	{
		DPRINTF("sdialog_call: in child\n");
		DPRINTF("sdialog_call: DISPLAY=%s\n",getenv("DISPLAY"));
       	umask(0);
		chdir("/");
		DPRINTF("call_dialog: about to exec %s\n",path);
		execlp(path,path,msg,(char*)0);
		DERROR("sdialog_call/execlp");
		exit(1);
    }

	DPRINTF("sdialog_call: child PID=%d\n",pid);
    return pid;
}

// execute dialog in a child process, read back it's stdout via a pipe, wait for it to exit
///////////////////////////////////////////////////////////////////////////////////////////
char* sdialog_call_modal(const char* path,const char* msg)
{
    pid_t pid;
    size_t len;
    char *response;
    int p[2], status, ret;
    char buf[1024];

	DPRINTF("... fflush\n");
    if(fflush(stdout)!=0)
	{
		DERROR("sdialog_call_modal/fflush");
		return NULL;
	}

	DPRINTF("... pipe\n");
    if(pipe(p)<0)
	{
		DERROR("sdialog_call_modal/pipe");
        return NULL;
    }

	DPRINTF("... fork\n");
    if((pid=fork())<0)
	{
		DERROR("sdialog_call_modal/fork");
        return NULL;
    }

    if(pid==0)
	{
		DPRINTF("sdialog_call_modal: in child\n");
		DPRINTF("sdialog_call_modal: DISPLAY=%s\n",getenv("DISPLAY"));
       	umask(0);
		chdir("/");
        close(p[0]);

        if(dup2(p[1],STDOUT_FILENO)<0)
		{
			DPRINTF("sdialog_call_modal/child/dup2");
			exit(1);
		}

		DPRINTF("call_dialog_result: about to exec %s\n",path);
		execlp(path,path,msg,(char*)0);

		DERROR("sdialog_call_modal/execlp");
		exit(1);
    }

	DPRINTF("sdialog_call_modal: child PID=%d\n",pid);
	DPRINTF("sdialog_call_modal: reading result\n");

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

	DPRINTF("sdialog_call_modal: waiting for child to die\n");
    while(waitpid(pid,&status,0)<0)
        if(errno!=EINTR)
            break;

    if(!WIFEXITED(status) || WEXITSTATUS(status)>1)
	{
		DPRINTF("sdialog_call_modal: child died badly\n");
        memset(buf,0,sizeof(buf));
        return NULL;
    }

	DPRINTF("sdialog_call_modal: child died normally\n");
    buf[strcspn(buf,"\r\n")]='\0';
    response=strdup(buf);
    memset(buf,0,sizeof(buf));
    return response;
}
