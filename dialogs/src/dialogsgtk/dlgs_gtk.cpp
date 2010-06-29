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

extern "C"
{
#include "single_dialog.h"
}

using namespace eIDMW;

DLGS_EXPORT DlgRet eIDMW::DlgAskPin(DlgPinOperation operation, DlgPinUsage usage, const wchar_t *wsPinName, DlgPinInfo pinInfo, wchar_t *wsPin, unsigned long ulPinBufferLen)
{
	printf("DlgAskPin called\n");
	char* response=sdialog_call_modal("/usr/local/bin/beid-askpin","Please enter your PIN code.");	
	if(response==NULL)
	{
		return DLG_CANCEL;
	}
	else
	{
		mbstowcs(wsPin,response,ulPinBufferLen);
		free(response);
		return DLG_OK;
	}
}

DLGS_EXPORT DlgRet eIDMW::DlgAskPins(DlgPinOperation operation, DlgPinUsage usage, const wchar_t *wsPinName, DlgPinInfo pin1Info, wchar_t *wsPin1, unsigned long ulPin1BufferLen, DlgPinInfo pin2Info, wchar_t *wsPin2, unsigned long ulPin2BufferLen)
{
	DlgRet result=DLG_ERR;

	printf("DlgAskPins called\n");

	char* response=sdialog_call_modal("/usr/local/bin/beid-changepin","Please enter your current PIN, followed by your new PIN (twice)");
	if(response==NULL)
	{
		result=DLG_CANCEL;
	}
	else
	{
		char* sep_pos=strchr(response,':');
		if(sep_pos!=NULL)
		{
			*sep_pos='\0';
			mbstowcs(wsPin1,response,ulPin1BufferLen);
			mbstowcs(wsPin2,sep_pos+1,ulPin2BufferLen);
			result=DLG_OK;
		}
		free(response);
	}
	
	return result;
}

DLGS_EXPORT DlgRet eIDMW::DlgBadPin( DlgPinUsage usage, const wchar_t *wsPinName, unsigned long ulRemainingTries)
{
	char message[1024];
	printf("DlgBadPin called\n");
	wcstombs(message,wsPinName,1024);
	char* response=sdialog_call_modal("/usr/local/bin/beid-badpin",message);	
	free(response);
    return DLG_OK;
}

DLGS_EXPORT DlgRet eIDMW::DlgDisplayPinpadInfo(DlgPinOperation operation, const wchar_t *wsReader, DlgPinUsage usage, const wchar_t *wsPinName, const wchar_t *wsMessage, unsigned long *pulHandle)
{
	char message[1024];
	printf("DlgDisplayPinPadInfo called\n");
	wcstombs(message,wsReader,1024);
	sdialog_call("/usr/local/bin/beid-secure-askpin",message);
    return DLG_OK;
}


DLGS_EXPORT void eIDMW::DlgClosePinpadInfo( unsigned long ulHandle )
{
	printf("DlgClosePinpadInfo called\n");
	sdialog_call("/usr/local/bin/beid-close-secure-askpin","close");
}


DLGS_EXPORT DlgRet eIDMW::DlgAskAccess( const wchar_t *wsAppPath, const wchar_t *wsReaderName, DlgPFOperation ulOperation, int *piForAllOperations)
{
	char 	message[1024];
	DlgRet 	result=DLG_CANCEL;

	printf("DlgAskAccess called\n");

	wcstombs(message,wsAppPath,1024);
	char* response=sdialog_call_modal("/usr/local/bin/beid-ask-access",message);
	if(response!=NULL)
	{
		if(strcmp(response,"OK")==0)
			result=DLG_OK;
		free(response);
	}
	
	return result;
}



/////////////////////////////////// NOT CALLED /////////////////////////////////////////////////////////////

DLGS_EXPORT void eIDMW::DlgCloseAllPinpadInfo()
{
}

DLGS_EXPORT DlgRet eIDMW::DlgDisplayModal(DlgIcon icon, DlgMessageID messageID, const wchar_t *csMesg, unsigned char ulButtons, unsigned char ulEnterButton, unsigned char ulCancelButton)
{
    return DLG_ERR;
}

