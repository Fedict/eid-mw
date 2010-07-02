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
#include <libintl.h>

#define _(String) gettext (String)

#include "log.h"
#include "util.h"
#include "mwexception.h"
#include "eiderrors.h"
#include "config.h"

extern "C"
{
#include "single_dialog.h"
#include "default-beid-askaccess.cpp"
#include "default-beid-askpin.cpp"
#include "default-beid-badpin.cpp"
#include "default-beid-changepin.cpp"
#include "default-beid-spr-askpin.cpp"
#include "default-beid-spr-changepin.cpp"
#include "default-beid-spr-close.cpp"

int sdialog_raise(char* path, size_t path_max, char* name, unsigned char* start, size_t size)
{
    snprintf(path,path_max,"/tmp/.raised-%s",name);
    int fd=open(path,O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR);
    if(fd>0)
    {
        int nwritten=write(fd,start,size);
        fprintf(stderr,"wrote %d/%d bytes\n",nwritten,size);
        close(fd);
		return 0;
    }
	return -1;
}

int sdialog_rest(char* name)
{
	char path[PATH_MAX];
    snprintf(path,sizeof(path)-2,"/tmp/.raised-%s",name);
	unlink(path);
}

}

using namespace eIDMW;

DLGS_EXPORT DlgRet eIDMW::DlgAskPin(DlgPinOperation operation, DlgPinUsage usage, const wchar_t *wsPinName, DlgPinInfo pinInfo, wchar_t *wsPin, unsigned long ulPinBufferLen)
{
	printf("DlgAskPin called\n");

	char path[PATH_MAX];
	if(sdialog_raise(path,sizeof(path)-2,"beid-askpin",default_beid_askpin,sizeof(default_beid_askpin))==-1)
	{
		perror("sdialog_raise");
		return DLG_ERR;
	}

	char* response=sdialog_call_modal(path,_("Please enter your PIN code."));	
	if(response==NULL)
	{
		sdialog_rest("beid-askpin");
		return DLG_CANCEL;
	}
	else
	{
		sdialog_rest("beid-askpin");
		mbstowcs(wsPin,response,ulPinBufferLen);
		free(response);
		return DLG_OK;
	}
}

DLGS_EXPORT DlgRet eIDMW::DlgAskPins(DlgPinOperation operation, DlgPinUsage usage, const wchar_t *wsPinName, DlgPinInfo pin1Info, wchar_t *wsPin1, unsigned long ulPin1BufferLen, DlgPinInfo pin2Info, wchar_t *wsPin2, unsigned long ulPin2BufferLen)
{
	DlgRet result=DLG_ERR;

	printf("DlgAskPins called\n");

	char path[PATH_MAX];
	if(sdialog_raise(path,sizeof(path)-2,"beid-changepin",default_beid_changepin,sizeof(default_beid_changepin))==-1)
	{
		perror("sdialog_raise");
		return DLG_ERR;
	}

	char* response=sdialog_call_modal(path,"");
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

	sdialog_rest("beid-changepin");
	return result;
}

DLGS_EXPORT DlgRet eIDMW::DlgBadPin( DlgPinUsage usage, const wchar_t *wsPinName, unsigned long ulRemainingTries)
{
	char count[4];
	printf("DlgBadPin called\n");

	char path[PATH_MAX];
	if(sdialog_raise(path,sizeof(path)-2,"beid-badpin",default_beid_badpin,sizeof(default_beid_badpin))==-1)
	{
		perror("sdialog_raise");
		return DLG_ERR;
	}

	snprintf(count,sizeof(count)-2,"%1d",ulRemainingTries);
	char* response=sdialog_call_modal(path,count);	
	free(response);
	sdialog_rest("beid-badpin");
    return DLG_OK;
}

DLGS_EXPORT DlgRet eIDMW::DlgDisplayPinpadInfo(DlgPinOperation operation, const wchar_t *wsReader, DlgPinUsage usage, const wchar_t *wsPinName, const wchar_t *wsMessage, unsigned long *pulHandle)
{
	char message[1024];
	char path[PATH_MAX];

	printf("DlgDisplayPinPadInfo called\n");

	if(operation==DLG_PIN_OP_VERIFY)
	{
		if(sdialog_raise(path,sizeof(path)-2,"beid-spr-askpin",default_beid_spr_askpin,sizeof(default_beid_spr_askpin))==-1)
		{
			perror("sdialog_raise");
			return DLG_ERR;
		}
	}
	else
	{
		if(sdialog_raise(path,sizeof(path)-2,"beid-spr-changepin",default_beid_spr_changepin,sizeof(default_beid_spr_changepin))==-1)
		{
			perror("sdialog_raise");
			return DLG_ERR;
		}
	}

	wcstombs(message,wsReader,1024);
	printf("operation=%d\n",operation);
	sdialog_call(path,message);
	sdialog_rest("beid-spr-changepin");
    return DLG_OK;
}


DLGS_EXPORT void eIDMW::DlgClosePinpadInfo( unsigned long ulHandle )
{
	printf("DlgClosePinpadInfo called\n");

	char path[PATH_MAX];
	if(sdialog_raise(path,sizeof(path)-2,"beid-spr-close",default_beid_spr_close,sizeof(default_beid_spr_close))==-1)
		perror("sdialog_raise");
	else
	{
		sdialog_call(path,"");
		sdialog_rest("beid-spr-close");
	}
}


DLGS_EXPORT DlgRet eIDMW::DlgAskAccess( const wchar_t *wsAppPath, const wchar_t *wsReaderName, DlgPFOperation ulOperation, int *piForAllOperations)
{
	char 	message[1024];
	DlgRet 	result=DLG_CANCEL;

	printf("DlgAskAccess called\n");

	char path[PATH_MAX];
	if(sdialog_raise(path,sizeof(path)-2,"beid-askaccess",default_beid_askaccess,sizeof(default_beid_askaccess))==-1)
	{
		perror("sdialog_raise");
		return DLG_ERR;
	}

	wcstombs(message,wsAppPath,1024);
	char* response=sdialog_call_modal(path,message);
	if(response!=NULL)
	{
		if(strcmp(response,"OK")==0)
			result=DLG_OK;
		free(response);
	}

	sdialog_rest("beid-askaccess");
	return result;
}
