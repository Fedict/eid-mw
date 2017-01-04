
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

#include "stdafx.h"
#include "../dialogs.h"

#define DLGWIN_PINRES_LEN 128
using namespace eIDMW;


class dlgWndAskPIN:public Win32Dialog
{
	void GetPinResult();
	HBITMAP ImagePIN;
	HBITMAP ImagePIN_Mask;

	HBITMAP ImageKP_BTN[12];
	HBITMAP ImageKP_BTN_Mask;
	HFONT TextFont;

	unsigned int m_ulPinMinLen;
	unsigned int m_ulPinMaxLen;
	bool m_UseKeypad;
	const wchar_t *szHeader;
	const wchar_t *szPIN;

	LONG m_KeypadHeight;

public:
	     dlgWndAskPIN(DlgPinInfo pinInfo, DlgPinUsage PinPusage,
			  std::wstring & Header, std::wstring & PINName,
			  bool UseKeypad, HWND Parent = NULL);
	     virtual ~ dlgWndAskPIN();

	wchar_t PinResult[DLGWIN_PINRES_LEN];

	virtual LRESULT ProcecEvent(UINT uMsg,	// Message For This Window
				    WPARAM wParam,	// Additional Message Information
				    LPARAM lParam);	// Additional Message Information
};
