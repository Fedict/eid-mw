
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

#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <cstdlib>
#include "errno.h"
#include "mw_util.h"
#include "dialogs.h"
#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <stdarg.h>
#include "../../common/log.h"
#include "../../common/util.h"
#include "../../common/mwexception.h"
#include "../../common/eiderrors.h"
#include "config.h"

#include "gtk_dialog_names.h"

using namespace eIDMW;

extern "C"
{
	pid_t sdialog_call(const char *path, const char *msg);
	char *sdialog_call_modal(const char *path, const char *msg);
	void dlg_log_printf(const char *format, ...);
	void dlg_log_error(const char *label);
}

bool MW_PERROR(tLevel level, tModule mod, const char *comment)
{
	char err_txt[256], log_txt[1024];
	wchar_t wide_log_txt[1024];

	strerror_r(errno, err_txt, sizeof(err_txt));

	snprintf(log_txt, sizeof(log_txt), "%s:%s", comment, err_txt);
	mbstowcs(wide_log_txt, log_txt,
		 sizeof(wide_log_txt) / sizeof(wchar_t));
	return MWLOG(level, mod, wide_log_txt);
}

void dlg_log_printf(const char *format, ...)
{
	char buf[1024];
	wchar_t wbuf[1024];
	va_list ap;

	va_start(ap, format);
	vsnprintf(buf, sizeof(buf), format, ap);
	va_end(ap);
	mbstowcs(wbuf, buf, sizeof(wbuf) / sizeof(wchar_t));
	MWLOG(LEV_DEBUG, MOD_DLG, wbuf);
}

void dlg_log_error(const char *label)
{
	MW_PERROR(LEV_ERROR, MOD_DLG, label);
}

DLGS_EXPORT DlgRet eIDMW::DlgAskPin(DlgPinOperation operation,
				    DlgPinUsage usage,
				    const wchar_t * wsPinName,
				    DlgPinInfo pinInfo, wchar_t * wsPin,
				    unsigned long ulPinBufferLen)
{
	MWLOG(LEV_DEBUG, MOD_DLG, L"eIDMW::DlgAskPin called");

	char *response = sdialog_call_modal(BEID_ASKPIN_DIALOG, "");

	if (response == NULL)
		return DLG_CANCEL;
	else
	{
		mbstowcs(wsPin, response, ulPinBufferLen);
		free(response);
		return DLG_OK;
	}
}

DLGS_EXPORT DlgRet eIDMW::DlgAskPins(DlgPinOperation operation,
				     DlgPinUsage usage,
				     const wchar_t * wsPinName,
				     DlgPinInfo pin1Info, wchar_t * wsPin1,
				     unsigned long ulPin1BufferLen,
				     DlgPinInfo pin2Info, wchar_t * wsPin2,
				     unsigned long ulPin2BufferLen)
{
	DlgRet result = DLG_ERR;

	MWLOG(LEV_DEBUG, MOD_DLG, L"eIDMW::DlgAskPins called");

	char *response = sdialog_call_modal(BEID_CHANGEPIN_DIALOG, "");

	if (response == NULL)
		result = DLG_CANCEL;
	else
	{
		char *sep_pos = strchr(response, ':');

		if (sep_pos != NULL)
		{
			*sep_pos = '\0';
			mbstowcs(wsPin1, response, ulPin1BufferLen);
			mbstowcs(wsPin2, sep_pos + 1, ulPin2BufferLen);
			result = DLG_OK;
		}
		free(response);
	}

	return result;
}


DLGS_EXPORT DlgRet eIDMW::DlgBadPin(DlgPinUsage usage,
				    const wchar_t * wsPinName,
				    unsigned long ulRemainingTries)
{
	char count[4];

	MWLOG(LEV_DEBUG, MOD_DLG, L"eIDMW::DlgBadPin called");

	snprintf(count, sizeof(count) - 2, "%1lu", ulRemainingTries);
	char *response = sdialog_call_modal(BEID_BADPIN_DIALOG, count);

	free(response);
	return DLG_OK;
}


DLGS_EXPORT DlgRet eIDMW::DlgDisplayPinpadInfo(DlgPinOperation operation,
					       const wchar_t * wsReader,
					       DlgPinUsage usage,
					       const wchar_t * wsPinName,
					       const wchar_t * wsMessage,
					       unsigned long *pulHandle)
{
	char message[1024];
	pid_t dialog_pid;

	MWLOG(LEV_DEBUG, MOD_DLG, L"eIDMW::DlgDisplayPinPadInfo called");

	wcstombs(message, wsReader, 1024);

	if (operation == DLG_PIN_OP_VERIFY)
		dialog_pid = sdialog_call(BEID_SPR_ASKPIN_DIALOG, message);
	else
		dialog_pid = sdialog_call(BEID_SPR_CHANGEPIN_DIALOG, message);

	if (dialog_pid < 0)
		return DLG_ERR;

	*pulHandle = (unsigned long) dialog_pid;
	return DLG_OK;
}


DLGS_EXPORT void eIDMW::DlgClosePinpadInfo(unsigned long ulHandle)
{
	MWLOG(LEV_DEBUG, MOD_DLG, L"eIDMW::DlgClosePinPadInfo called");

	if (ulHandle == 0L)
	{
		MW_PERROR(LEV_ERROR, MOD_DLG, "no dialog pid to kill");
		return;
	}

	pid_t dialog_pid = (pid_t) ulHandle;

	if (kill(dialog_pid, SIGTERM) != 0)
	{
		MW_PERROR(LEV_ERROR, MOD_DLG, "kill");
		return;
	}
}


DLGS_EXPORT DlgRet eIDMW::DlgAskAccess(const wchar_t * wsAppPath,
				       const wchar_t * wsReaderName,
				       DlgPFOperation ulOperation,
				       int *piForAllOperations)
{
	char message[1024];
	DlgRet result = DLG_CANCEL;

	MWLOG(LEV_DEBUG, MOD_DLG, L"eIDMW::DlgAskAccess called");


	wcstombs(message, wsAppPath, 1024);
	char *response = sdialog_call_modal(BEID_ASKACCESS_DIALOG, message);

	if (response != NULL)
	{
		if (strcmp(response, "OK") == 0)
			result = DLG_OK;
		free(response);
	}

	return result;
}
