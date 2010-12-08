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
#include "display.h"
#include "langutil.h"

using namespace eIDMW;

CK_BYTE AllowCardReading(void)
{
	CK_BYTE bDispResp = P11_DISPLAY_NO;
	DlgRet retVal = DlgDisplayModal(DLG_ICON_WARN,DLG_MESSAGE_USER_WARNING,L"",DLG_BUTTON_YES + DLG_BUTTON_NO, DLG_BUTTON_YES, DLG_BUTTON_NO);

	if( ( retVal == DLG_YES) || ( retVal == DLG_OK) )
	{
		bDispResp = P11_DISPLAY_YES;
	}
	else if (retVal == DLG_CANCEL)
	{
		bDispResp = P11_DISPLAY_CANCEL;
	}		
	return bDispResp;
}
