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
/********************************************************************************
*
*	dlgs.cpp (GTK+ version)
*
********************************************************************************/
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <cstdlib>
#include "errno.h"

#include "mw_util.h"
#include "dialogs.h"
#include "langutil.h"

#include "log.h"
#include "util.h"
#include "mwexception.h"
#include "eiderrors.h"
#include "config.h"

#include "pindialog.c"

int drop_suid(void)
{
	uid_t uid=getuid();

	if(setuid(uid)<0)
		return errno;

	if(getuid()!=uid||geteuid()!=uid)
		return errno;

	return 0;
}

static char *askpin(const char *msg)
{
    pid_t pid;
    size_t len;
    char *pass;
    int p[2], status, ret;
    char buf[1024];

    if(fflush(stdout) != 0)
	{
        perror("askpin/fflush");
		return NULL;
	}

    if(pipe(p)<0)
	{
		perror("askpin/pipe");
        return NULL;
    }

    if((pid=fork())<0)
	{
		perror("askpin/fork");
        return NULL;
    }

    if(pid == 0)
	{
        if(drop_suid()<0)
		{
			perror("askpin/child/drop_suid");
			exit(1);
		}
			
        close(p[0]);

        if(dup2(p[1],STDOUT_FILENO)<0)
		{
			perror("askpin/child/dup2");
			exit(1);
		}

		get_pin("beID PIN",msg);

    }
    close(p[1]);

    len = ret = 0;
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

    buf[len] = '\0';
    close(p[0]);

    while(waitpid(pid,&status,0)<0)
        if (errno != EINTR)
            break;

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
	{
        memset(buf,0,sizeof(buf));
        return NULL;
    }

    buf[strcspn(buf, "\r\n")]='\0';
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

