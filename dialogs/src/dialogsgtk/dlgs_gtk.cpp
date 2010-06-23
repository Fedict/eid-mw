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

/* the askpin function borrows from readpass.c in the OpenSSH distribution, whose Copyright is as follows: 
 *
 * START OF (C) NOTICE FOR askpin() 

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
 * END OF (C) NOTICE for askpin()
 */


#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <cstdlib>
#include "errno.h"

#include "mw_util.h"
#include "dialogs.h"
#include "langutil.h"

#include <stdio.h>
#include <limits.h>
#include <unistd.h>

#include "log.h"
#include "util.h"
#include "mwexception.h"
#include "eiderrors.h"
#include "config.h"

#include "pindialog.c"

#define POSIX_SOURCE 1

void close_all_fd_except(int exempt)
{
  int i,max;
	return;

#if defined(OPEN_MAX)
  max=OPEN_MAX;
#else
  max=sysconf(_SC_OPEN_MAX);
#endif

  for(i=3;i<max;i++)
	if(i!=exempt)
    	close(i);
}

char* askpin(const char* msg)
{
    pid_t pid;
    size_t len;
    char *pass;
    int p[2], status, ret;
    char buf[1024];

	fprintf(stderr,"... fflush\n");
    if(fflush(stdout)!=0)
	{
        perror("askpin/fflush");
		return NULL;
	}

	fprintf(stderr,"... pipe\n");
    if(pipe(p)<0)
	{
		perror("askpin/pipe");
        return NULL;
    }

	fprintf(stderr,"... fork\n");
    if((pid=fork())<0)
	{
		perror("askpin/fork");
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
			perror("askpin/child/dup2");
			exit(1);
		}

		fprintf(stderr,"*** exec beid-askpin\n");
		execlp("/usr/local/bin/beid-askpin","/usr/local/bin/beid-askpin", msg, (char *) 0);
		perror("askpin/child/execlp");
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

	fprintf(stderr,"... child died peacefully\n");
    buf[strcspn(buf,"\r\n")]='\0';
    pass=strdup(buf);
    memset(buf,0,sizeof(buf));
    return pass;
}


using namespace eIDMW;

/**
 * Ask for 1 PIN (e.g. to verify or unblock without changing the old PIN)
 * This dialog is modal (returns after the dlg is closed)
 * - operation: DLG_PIN_OP_VERIFY or DLG_PIN_OP_UNBLOCK_NO_CHANGE
 * - pinName: should only be used if 'usage' == DLG_PIN_UNKNOWN
 * - pinInfo: should be used to verify the format of the PIN
 *     (valid length, valid characters, ...); as long as this
 *     isn't the case, the OK button should be disabled.
 * Returns: DLG_OK if the OK button was pressed,
 *          DLG_CANCEL if the Cancel button was pressed,
 *          DLG_BAD_PARAM or DLG_ERR otherwise
 */

DLGS_EXPORT DlgRet eIDMW::DlgAskPin(DlgPinOperation operation, DlgPinUsage usage, const wchar_t *wsPinName, DlgPinInfo pinInfo, wchar_t *wsPin, unsigned long ulPinBufferLen)
{
	printf("DlgAskPin called\n");
	
	char* pin=askpin("please enter your PIN code");

	if(pin!=NULL)
	{
		mbstowcs(wsPin,pin,ulPinBufferLen);
		return DLG_OK;
	}
	else
	{
		return DLG_CANCEL;
	}
}

DLGS_EXPORT DlgRet eIDMW::DlgAskPins(DlgPinOperation operation, DlgPinUsage usage, const wchar_t *wsPinName, DlgPinInfo pin1Info, wchar_t *wsPin1, unsigned long ulPin1BufferLen, DlgPinInfo pin2Info, wchar_t *wsPin2, unsigned long ulPin2BufferLen)
{
	printf("DlgAskPins called\n");
    return DLG_ERR;
}



DLGS_EXPORT DlgRet eIDMW::DlgBadPin( DlgPinUsage usage, const wchar_t *wsPinName, unsigned long ulRemainingTries)
{
	printf("DlgBadPin called\n");
    return DLG_ERR;
}

DLGS_EXPORT DlgRet eIDMW::DlgDisplayPinpadInfo(DlgPinOperation operation, const wchar_t *wsReader, DlgPinUsage usage, const wchar_t *wsPinName, const wchar_t *wsMessage, unsigned long *pulHandle)
{
	printf("DlgDisplayPinPadInfo called\n");
    return DLG_ERR;
}



DLGS_EXPORT void eIDMW::DlgClosePinpadInfo( unsigned long ulHandle )
{
	printf("DlgClosePinpadInfo called\n");
}



DLGS_EXPORT DlgRet eIDMW::DlgAskAccess( const wchar_t *wsAppPath, const wchar_t *wsReaderName, DlgPFOperation ulOperation, int *piForAllOperations)
{
  return DLG_ERR;
}



/////////////////////////////////// NOT CALLED /////////////////////////////////////////////////////////////

DLGS_EXPORT void eIDMW::DlgCloseAllPinpadInfo()
{
}

DLGS_EXPORT DlgRet eIDMW::DlgDisplayModal(DlgIcon icon, DlgMessageID messageID, const wchar_t *csMesg, unsigned char ulButtons, unsigned char ulEnterButton, unsigned char ulCancelButton)
{
    return DLG_ERR;
}

