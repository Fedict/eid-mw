
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
#include <stdlib.h>
#include "dlgwndpinpadinfo.h"
#include "resource.h"
#include "../langutil.h"
#include "common/log.h"

#define IDB_OK 1
#define IDB_CANCEL 2
#define IMG_SIZE 128

dlgWndPinpadInfo::dlgWndPinpadInfo(unsigned long ulHandle,
				   DlgPinUsage PinPusage,
				   DlgPinOperation operation,
				   const std::wstring & csReader,
				   const std::wstring & PinName,
				   const std::wstring & Message,
				   HWND Parent):Win32Dialog(L"WndPinpadInfo")
{
	m_szHeader = NULL;
	m_szMessage = NULL;

	m_ModalHold = false;
	m_szMessage = _wcsdup(Message.c_str());

	m_ulHandle = ulHandle;

	std::wstring tmpTitle = L"";

	tmpTitle += GETSTRING_DLG(PinpadInfo);

	if (!csReader.empty())
	{
		tmpTitle += L" - ";
		tmpTitle += csReader;
	}

	m_szHeader = _wcsdup(PinName.c_str());

	if (CreateWnd(tmpTitle.c_str(), 480, 420, 0, Parent))
	{
		if (PinPusage == DLG_PIN_SIGN)
			ImagePIN =
				LoadBitmap(m_hInstance,
					   MAKEINTRESOURCE(IDB_PINSIGN));
		else
			ImagePIN =
				LoadBitmap(m_hInstance,
					   MAKEINTRESOURCE(IDB_PIN));
	}


}

dlgWndPinpadInfo::~dlgWndPinpadInfo()
{
	KillWindow();

	if (m_szHeader)
	{
		free(m_szHeader);
		m_szHeader = NULL;
	}
	if (m_szMessage)
	{
		free(m_szMessage);
		m_szMessage = NULL;
	}
}

LRESULT dlgWndPinpadInfo::ProcecEvent(UINT uMsg,	// Message For This Window
				      WPARAM wParam,	// Additional Message Information
				      LPARAM lParam)	// Additional Message Information
{
	PAINTSTRUCT ps;
	RECT rect;

	switch (uMsg)
	{

		case WM_PAINT:
		{
			m_hDC = BeginPaint(m_hWnd, &ps);

			HDC hdcMem;

			GetClientRect(m_hWnd, &rect);
			rect.bottom = rect.top + IMG_SIZE + 8;	//rect.bottom / 2;
			FillRect(m_hDC, &rect,
				 CreateSolidBrush(RGB(255, 255, 255)));

			hdcMem = CreateCompatibleDC(m_hDC);
			SelectObject(hdcMem, ImagePIN);
			BitBlt(m_hDC, 4, 4, IMG_SIZE, IMG_SIZE, hdcMem,
			       0, 0, SRCCOPY);

			DeleteDC(hdcMem);

			rect.left += 136;
			rect.top += 32;
			rect.right -= 8;
			rect.bottom = 136 - 8;
			//SetBkColor( m_hDC, GetSysColor( COLOR_3DFACE ) );
			DrawText(m_hDC, m_szHeader, -1, &rect, DT_WORDBREAK);

			GetClientRect(m_hWnd, &rect);
			rect.top = rect.top + IMG_SIZE + 8;

			rect.top = rect.top + 8;
			rect.bottom = rect.bottom - 8;
			rect.left = rect.left + 8;
			rect.right = rect.right - 8;
			FillRect(m_hDC, &rect,
				 CreateSolidBrush(RGB(255, 255, 255)));

			rect.top = rect.top + 8;
			rect.bottom = rect.bottom - 8;
			rect.left = rect.left + 8;
			rect.right = rect.right - 8;
			DrawText(m_hDC, m_szMessage, -1, &rect, DT_WORDBREAK);

			EndPaint(m_hWnd, &ps);

			SetForegroundWindow(m_hWnd);

			return 0;
		}

		case WM_CREATE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG,
			      L"  --> dlgWndPinpadInfo::ProcecEvent WM_CLOSE (wParam=%X, lParam=%X)",
			      wParam, lParam);

			HMENU hSysMenu;

			hSysMenu = GetSystemMenu(m_hWnd, FALSE);
			EnableMenuItem(hSysMenu, 2,
				       MF_BYPOSITION | MF_GRAYED);

			return 1;
		}

		case WM_CLOSE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG,
			      L"  --> dlgWndPinpadInfo::ProcecEvent WM_CLOSE (wParam=%X, lParam=%X)",
			      wParam, lParam);

			if (m_ulHandle)
			{
				unsigned long tmp = m_ulHandle;

				m_ulHandle = 0;
				DlgClosePinpadInfo(tmp);
			}
			return 0;
		}

		case WM_DESTROY:
		{
			MWLOG(LEV_DEBUG, MOD_DLG,
			      L"  --> dlgWndModal::ProcecEvent WM_DESTROY (wParam=%X, lParam=%X)",
			      wParam, lParam);
			break;
		}

		default:
		{
			return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
		}
	}
	return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
}
