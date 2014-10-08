/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2013 FedICT.
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
#include "display.h"

#if defined(WIN32) || defined (__APPLE__)
#include "langutil.h"
#else
#include "dialogs.h"
#endif

using namespace eIDMW;

CK_BYTE AllowCardReading(void)
{
#ifndef NO_DIALOGS
	CK_BYTE bDispResp = P11_DISPLAY_NO;
#if defined(WIN32) || defined (__APPLE__)
	//DlgRet retVal = DlgDisplayModal(DLG_ICON_WARN,DLG_MESSAGE_USER_WARNING,L"",DLG_BUTTON_YES + DLG_BUTTON_NO, DLG_BUTTON_YES, DLG_BUTTON_NO);
	DlgRet retVal = DlgDisplayModal(DLG_ICON_WARN,DLG_MESSAGE_USER_WARNING,L"",DLG_BUTTON_YES + DLG_BUTTON_NO+DLG_BUTTON_ALWAYS, DLG_BUTTON_NO, DLG_BUTTON_NO);
#else
	const wchar_t *wsAppPath = L"An application";
	const wchar_t *wsReaderName = L"your reader";
	int iForAllOperations = 0;
	DlgRet retVal = DlgAskAccess( wsAppPath, wsReaderName, DLG_PF_OP_READ_ID, &iForAllOperations);
#endif
	if( ( retVal == DLG_YES) || ( retVal == DLG_OK) )
	{
		bDispResp = P11_DISPLAY_YES;
	}
	else if (retVal == DLG_CANCEL)
	{
		bDispResp = P11_DISPLAY_CANCEL;
	}
	else if (retVal == DLG_ALWAYS)
	{
		bDispResp = P11_DISPLAY_ALWAYS;
	}
	return bDispResp;
#else
  	return P11_DISPLAY_YES;
#endif
}
