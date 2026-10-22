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
#include "dlgwndaskpin.h"
#include <commctrl.h>
#include "resource.h"
#include "../langutil.h"
#include "common/log.h"
#include "mw_util.h"

#define IDC_STATIC 0
#define IDB_OK 1
#define IDB_CANCEL 2
#define IMG_SIZE 128
#define IDC_EDIT 3
#define IDB_KeypadStart 10
#define IDB_KeypadEnd   21
#define KP_BTN_SIZE 48
#define KP_LBL_SIZE 24

dlgWndAskPIN::dlgWndAskPIN( DlgPinInfo pinInfo, DlgPinUsage PinPusage, std::wstring & Header, std::wstring & PINName, bool UseKeypad, HWND Parent )
:Win32Dialog(L"WndAskPIN")
{
	static const int points_per_inch = 96;
	HMONITOR h_monitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFOEX mInfo;
	mInfo.cbSize = sizeof(mInfo);
	GetMonitorInfo(h_monitor, &mInfo);
	int pixels_per_inch = GetDeviceCaps(CreateDCW(mInfo.szDevice, NULL, NULL, NULL), LOGPIXELSY);
	int scalingValue = pixels_per_inch / points_per_inch;
	int pixels_height = (24 * scalingValue); // was 20

	m_UseKeypad = UseKeypad;

	PinResult[0] = ' ';
	PinResult[1] = (char)0;

	std::wstring tmpTitle = L"";

	if( PinPusage == DLG_PIN_SIGN )
		tmpTitle += GETSTRING_DLG(SigningWith);
	else
		tmpTitle += GETSTRING_DLG(Asking);

	tmpTitle += L" ";
	tmpTitle += PINName;

	m_ulPinMinLen = pinInfo.ulMinLen;
	m_ulPinMaxLen = pinInfo.ulMaxLen;

	szHeader = Header.c_str();
	szPIN = PINName.c_str();

	int Height = 280 * scalingValue;
	if( m_UseKeypad )
		Height = 480 * scalingValue;

	if( CreateWnd( tmpTitle.c_str() , 480 * scalingValue, Height, 0, Parent ) )
	{
		RECT clientRect;
		GetClientRect( m_hWnd, &clientRect );

		TextFont = CreateFont(pixels_height, 0, 0, 0, FW_DONTCARE, 0, 0, 0,
				DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"MS Shell Dlg" );

		//OK Button
		HWND hOkButton = CreateWindow(
			L"BUTTON", GETSTRING_DLG(Ok), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON, 
			clientRect.right - 220 * scalingValue, clientRect.bottom - (44 * scalingValue), 
			100 * scalingValue, 36 * scalingValue, m_hWnd, (HMENU)IDB_OK, m_hInstance, NULL );
		EnableWindow( hOkButton, false );

		//Cancel Button
		HWND hCancelButton = CreateWindow(
			L"BUTTON", GETSTRING_DLG(Cancel), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_TEXT, 
			clientRect.right - 110 * scalingValue, clientRect.bottom - 44 * scalingValue, 
			100 * scalingValue, 36 * scalingValue, m_hWnd, (HMENU)IDB_CANCEL, m_hInstance, NULL );

		m_KeypadHeight=0;

		//Virtual Keypad
		if( m_UseKeypad )
		{
			int top = 60 * scalingValue;
			int hMargin = 16 * scalingValue;
			int vMargin = 16 * scalingValue;
			int btnwidth  = 64 * scalingValue;  // was 48
			int btnheight = 64 * scalingValue;  // was 48
			int totwidth  = btnwidth * 3 + 2 * hMargin;
			int totheight = btnheight * 4 + 3 * vMargin;
			int left = (clientRect.right - clientRect.left - totwidth) / 2;
			m_KeypadHeight = top + totheight + 8;

			for( int i = 0; i < 4; i++ )
			{
				for( int  j = 0; j < 3; j++ )
				{
					if( i == 3 && j == 0 )
						continue;
					HWND hOkButton = CreateWindow(
						L"BUTTON", L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW, //BS_FLAT, 
						left + ( btnwidth + hMargin )* j, top + ( btnheight + vMargin ) * i, btnwidth, btnheight, 
						m_hWnd, (HMENU)(long long)(IDB_KeypadStart + 3*i + j ), m_hInstance, NULL );
				}
			}

			ImageKP_BTN[0]  = reinterpret_cast<HBITMAP>(LoadImageW(m_hInstance, MAKEINTRESOURCEW(IDB_KP_0),   IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION));
			ImageKP_BTN[1]  = reinterpret_cast<HBITMAP>(LoadImageW(m_hInstance, MAKEINTRESOURCEW(IDB_KP_1),   IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION));
			ImageKP_BTN[2]  = reinterpret_cast<HBITMAP>(LoadImageW(m_hInstance, MAKEINTRESOURCEW(IDB_KP_2),   IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION));
			ImageKP_BTN[3]  = reinterpret_cast<HBITMAP>(LoadImageW(m_hInstance, MAKEINTRESOURCEW(IDB_KP_3),   IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION));
			ImageKP_BTN[4]  = reinterpret_cast<HBITMAP>(LoadImageW(m_hInstance, MAKEINTRESOURCEW(IDB_KP_4),   IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION));
			ImageKP_BTN[5]  = reinterpret_cast<HBITMAP>(LoadImageW(m_hInstance, MAKEINTRESOURCEW(IDB_KP_5),   IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION));
			ImageKP_BTN[6]  = reinterpret_cast<HBITMAP>(LoadImageW(m_hInstance, MAKEINTRESOURCEW(IDB_KP_6),   IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION));
			ImageKP_BTN[7]  = reinterpret_cast<HBITMAP>(LoadImageW(m_hInstance, MAKEINTRESOURCEW(IDB_KP_7),   IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION));
			ImageKP_BTN[8]  = reinterpret_cast<HBITMAP>(LoadImageW(m_hInstance, MAKEINTRESOURCEW(IDB_KP_8),   IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION));
			ImageKP_BTN[9]  = reinterpret_cast<HBITMAP>(LoadImageW(m_hInstance, MAKEINTRESOURCEW(IDB_KP_9),   IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION));
			ImageKP_BTN[10] = reinterpret_cast<HBITMAP>(LoadImageW(m_hInstance, MAKEINTRESOURCEW(IDB_KP_CE),  IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION));
			ImageKP_BTN[11] = reinterpret_cast<HBITMAP>(LoadImageW(m_hInstance, MAKEINTRESOURCEW(IDB_KP_BTN), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION));

			CreateBitapMask(ImageKP_BTN[11], ImageKP_BTN_Mask );
			//MWLOG(LEV_DEBUG, MOD_DLG, L" dlgWndAskPIN : Virtual pinpad - LoadBitmap");
		}

		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_PASSWORD;
		if( pinInfo.ulFlags & PIN_FLAG_DIGITS )
			dwStyle |= ES_NUMBER;

		LONG pinTop=0;
		LONG pinLeft=clientRect.right/2 - 120 * scalingValue + 40;

		if( m_UseKeypad )
			pinTop = clientRect.top + 20 * scalingValue;
		else
			pinTop = clientRect.bottom - 100 * scalingValue;

		HWND hTextEdit = CreateWindowEx( WS_EX_CLIENTEDGE,
			L"EDIT", L"", dwStyle, 
			pinLeft, pinTop, 220 * scalingValue, 34 * scalingValue, // wider and taller
			m_hWnd, (HMENU)IDC_EDIT, m_hInstance, NULL );
		SendMessage( hTextEdit, EM_LIMITTEXT, m_ulPinMaxLen, 0 );

		HWND hStaticText = CreateWindow( 
			L"STATIC", szPIN, WS_CHILD | WS_VISIBLE | SS_RIGHT, 
			pinLeft-110 * scalingValue, pinTop + 4, 106 * scalingValue, 26 * scalingValue, // a bit taller label
			m_hWnd, (HMENU)IDC_STATIC, m_hInstance, NULL );

		SendMessage( hStaticText, WM_SETFONT, (WPARAM)TextFont, 0 );

		SendMessage( hOkButton, WM_SETFONT, (WPARAM)TextFont, 0 );
		SendMessage( hCancelButton, WM_SETFONT, (WPARAM)TextFont, 0 );

		if( PinPusage == DLG_PIN_SIGN )
			ImagePIN = LoadBitmap( m_hInstance, MAKEINTRESOURCE(IDB_PINSIGN) );
		else
			ImagePIN = LoadBitmap( m_hInstance, MAKEINTRESOURCE(IDB_PIN) );
		CreateBitapMask( ImagePIN, ImagePIN_Mask );

		SetFocus(GetDlgItem( m_hWnd, IDC_EDIT ));
	}
}

dlgWndAskPIN::~dlgWndAskPIN()
{
	KillWindow();

	for (int i = 0; i < 12; ++i)
	{
		if (ImageKP_BTN[i]) { DeleteObject(ImageKP_BTN[i]); ImageKP_BTN[i] = NULL; }
	}
	if (ImageKP_BTN_Mask) { DeleteObject(ImageKP_BTN_Mask); ImageKP_BTN_Mask = NULL; }
	if (ImagePIN) { DeleteObject(ImagePIN); ImagePIN = NULL; }
	if (ImagePIN_Mask) { DeleteObject(ImagePIN_Mask); ImagePIN_Mask = NULL; }
	if (TextFont) { DeleteObject(TextFont); TextFont = NULL; }
}

void dlgWndAskPIN::GetPinResult()
{
	wchar_t nameBuf[128];
	long len = (long)SendMessage( GetDlgItem( m_hWnd, IDC_EDIT ), WM_GETTEXTLENGTH, 0, 0 );
	if( len < 128 )
	{
		SendMessage( GetDlgItem( m_hWnd, IDC_EDIT ), WM_GETTEXT, (WPARAM)(sizeof(nameBuf)), (LPARAM)nameBuf );
		wcscpy_s( PinResult, DLGWIN_PINRES_LEN, nameBuf );
	}
}

LRESULT dlgWndAskPIN::ProcecEvent
			(	UINT		uMsg,			// Message For This Window
				WPARAM		wParam,			// Additional Message Information
				LPARAM		lParam )		// Additional Message Information
{
	PAINTSTRUCT ps;
	RECT rect;

	switch( uMsg )
	{ 
		case WM_COMMAND:
		{
			switch( LOWORD(wParam) )
			{
				case IDC_EDIT:
				{
					if( EN_CHANGE == HIWORD(wParam) )
					{
						long len = (long)SendMessage( GetDlgItem( m_hWnd, IDC_EDIT ), WM_GETTEXTLENGTH, 0, 0 );
						EnableWindow( GetDlgItem( m_hWnd, IDOK ), ( (unsigned int)len >= m_ulPinMinLen ) );
					}
					return TRUE;
				}

				case IDB_OK:
					GetPinResult();
					dlgResult = eIDMW::DLG_OK;
					close();
					return TRUE;

				case IDB_CANCEL:
					dlgResult = eIDMW::DLG_CANCEL;
					close();
					return TRUE;

				default:
					unsigned short tmp = LOWORD(wParam);
					if( tmp >= IDB_KeypadStart && tmp < IDB_KeypadEnd ) // Keypad Buttons
					{
						wchar_t nameBuf[128];
						SendMessage( GetDlgItem( m_hWnd, IDC_EDIT ), WM_GETTEXT, (WPARAM)(sizeof(nameBuf)), (LPARAM)nameBuf );
						size_t iPos = wcslen( nameBuf );
						if( iPos >= m_ulPinMaxLen )
							return TRUE;
						if( tmp == IDB_KeypadEnd - 1 ) // Keypad Button 0
						{
							nameBuf[ iPos++ ] = L'0';
						}
						else // Keypad Button 1 to 9
						{
							nameBuf[ iPos++ ] = 49 + tmp - IDB_KeypadStart;
						}
						nameBuf[ iPos++ ] = NULL;
						SendMessage( GetDlgItem( m_hWnd, IDC_EDIT ), WM_SETTEXT, 0, (LPARAM)nameBuf );
						return TRUE;
					}
					if( tmp == IDB_KeypadEnd ) // Keypad Button CE
					{
						SendMessage( GetDlgItem( m_hWnd, IDC_EDIT ), WM_SETTEXT, 0, (LPARAM)"" );
						//clear
					}
					return DefWindowProc( m_hWnd, uMsg, wParam, lParam );
			}
		}

		case WM_DRAWITEM:
		{
			LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT)lParam;
			if( lpDrawItem->CtlType & ODT_BUTTON )
			{
				HBRUSH hbrFace = GetSysColorBrush(COLOR_3DFACE);
				FillRect(lpDrawItem->hDC, &lpDrawItem->rcItem, hbrFace);

				// Base face image (optional)
				if (ImageKP_BTN[11] && ImageKP_BTN_Mask)
				{
					HDC hdcMem = CreateCompatibleDC(lpDrawItem->hDC);
					HGDIOBJ hOld = SelectObject(hdcMem, ImageKP_BTN[11]);
					MaskBlt(lpDrawItem->hDC,
						(lpDrawItem->rcItem.right  - KP_BTN_SIZE) / 2,
						(lpDrawItem->rcItem.bottom - KP_BTN_SIZE) / 2,
						KP_BTN_SIZE, KP_BTN_SIZE,
						hdcMem, 0, 0,
						ImageKP_BTN_Mask, 0, 0, MAKEROP4(SRCCOPY, 0x00AA0029));
					SelectObject(hdcMem, hOld);
					DeleteDC(hdcMem);
				}
				else
				{
					RECT r = lpDrawItem->rcItem;
					DrawEdge(lpDrawItem->hDC, &r, EDGE_RAISED, BF_RECT);
				}

				// Determine label index and text
				unsigned int iNum = 0;
				wchar_t label[3] = L"";
				if (lpDrawItem->CtlID == IDB_KeypadEnd)         { iNum = 10; wcscpy_s(label, L"CE"); }
				else if (lpDrawItem->CtlID == IDB_KeypadEnd - 1) { iNum = 0;  wcscpy_s(label, L"0");  }
				else if (lpDrawItem->CtlID >= IDB_KeypadStart && lpDrawItem->CtlID < IDB_KeypadEnd - 2)
				{
					iNum = (lpDrawItem->CtlID - IDB_KeypadStart) + 1; // 1..9
					label[0] = L'0' + (wchar_t)iNum; label[1] = L'\0';
				}

				// Draw label bitmap if available, else text
				if (iNum <= 10 && ImageKP_BTN[iNum])
				{
					HDC hdcMem = CreateCompatibleDC(lpDrawItem->hDC);
					HGDIOBJ hOld = SelectObject(hdcMem, ImageKP_BTN[iNum]);
					BitBlt(lpDrawItem->hDC,
						(lpDrawItem->rcItem.right  - KP_LBL_SIZE) / 2,
						(lpDrawItem->rcItem.bottom - KP_LBL_SIZE) / 2,
						KP_LBL_SIZE, KP_LBL_SIZE, hdcMem, 0, 0, SRCCOPY);
					SelectObject(hdcMem, hOld);
					DeleteDC(hdcMem);
				}
				else
				{
					SetBkMode(lpDrawItem->hDC, TRANSPARENT);
					SetTextColor(lpDrawItem->hDC, GetSysColor(COLOR_BTNTEXT));
					HFONT hOldFont = (HFONT)SelectObject(lpDrawItem->hDC, TextFont);
					RECT tr = lpDrawItem->rcItem;
					tr.left += 2; tr.right -= 2; tr.top += 2; tr.bottom -= 2;
					DrawText(lpDrawItem->hDC, label, -1, &tr, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
					SelectObject(lpDrawItem->hDC, hOldFont);
				}

				if (lpDrawItem->itemState & ODS_SELECTED)
					DrawEdge(lpDrawItem->hDC, &lpDrawItem->rcItem, EDGE_RAISED, BF_RECT);
				if (lpDrawItem->itemState & ODS_HOTLIGHT)
					DrawEdge(lpDrawItem->hDC, &lpDrawItem->rcItem, EDGE_SUNKEN, BF_RECT);
				if (lpDrawItem->itemState & ODS_FOCUS)
				{
					RECT r; GetClientRect(lpDrawItem->hwndItem, &r);
					r.left += 2; r.right -= 2; r.top += 2; r.bottom -= 2;
					DrawFocusRect(lpDrawItem->hDC, &r);
				}
				return TRUE;
			}
			break;
		}

		case WM_SIZE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskPIN::ProcecEvent WM_SIZE (wParam=%X, lParam=%X)",wParam,lParam);

			if( IsIconic( m_hWnd ) )
				return 0;
			break;
		}

		case WM_PAINT:
		{
			//MWLOG(LEV_DEBUG, MOD_DLG, L" dlgWndAskPIN : WM_PAINT");
			m_hDC = BeginPaint( m_hWnd, &ps );

			HDC hdcMem;

			hdcMem = CreateCompatibleDC( m_hDC );

			HGDIOBJ oldObj = SelectObject( hdcMem , ImagePIN );

			GetClientRect( m_hWnd, &rect );
			MaskBlt( m_hDC, 4, m_KeypadHeight + 8,
				IMG_SIZE, IMG_SIZE,	hdcMem, 0, 0,
				ImagePIN_Mask, 0, 0, MAKEROP4( SRCCOPY, 0x00AA0029 ) );
		
			
			SelectObject( hdcMem, oldObj );
			DeleteDC(hdcMem);

			if( m_UseKeypad )
			{
				GetClientRect( m_hWnd, &rect );
				rect.left += 8;
				rect.right -= 8;
				rect.top += 8;
				rect.bottom = m_KeypadHeight;

				DrawEdge( m_hDC, &rect, EDGE_RAISED, BF_RECT );
			}

			GetClientRect( m_hWnd, &rect );
			rect.left   += 8;
			rect.top     = m_KeypadHeight + 8;
			rect.right  -= 8;
			rect.bottom  = rect.bottom - 40;
			SetBkColor( m_hDC, GetSysColor( COLOR_3DFACE ) );
			SelectObject( m_hDC, TextFont );
			DrawText( m_hDC, szHeader, -1, &rect, DT_WORDBREAK | DT_CENTER);

			EndPaint( m_hWnd, &ps );

			SetForegroundWindow( m_hWnd );

			return 0;
		}

		case WM_ACTIVATE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskPIN::ProcecEvent WM_ACTIVATE (wParam=%X, lParam=%X)",wParam,lParam);
			break;
		}

		case WM_NCACTIVATE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskPIN::ProcecEvent WM_NCACTIVATE (wParam=%X, lParam=%X)",wParam,lParam);

			//if( !IsIconic( m_hWnd ) && m_ModalHold && Active_hWnd == m_hWnd )
			//{
			//	ShowWindow( m_hWnd, SW_SHOW );
			//	SetFocus( m_hWnd );
			//	return 0;
			//}
			if(!wParam)
			{
				SetFocus( m_hWnd );
				return 0;
			}
			break;
		}

		case WM_SETFOCUS:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskPIN::ProcecEvent WM_SETFOCUS (wParam=%X, lParam=%X)",wParam,lParam);
			break;
		}

		case WM_KILLFOCUS:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskPIN::ProcecEvent WM_KILLFOCUS (wParam=%X, lParam=%X)",wParam,lParam);

			//if( !IsIconic( m_hWnd ) && m_ModalHold && Active_hWnd == m_hWnd )
			//{
			//	if( GetParent((HWND)wParam ) != m_hWnd )
			//	{
			//		SetFocus( m_hWnd );
			//		return 0;
			//	}
			//}
			break;
		}

		case WM_CREATE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskPIN::ProcecEvent WM_CREATE (wParam=%X, lParam=%X)",wParam,lParam);

			HMENU hSysMenu;

			hSysMenu = GetSystemMenu( m_hWnd, FALSE );
			EnableMenuItem( hSysMenu, 3, MF_BYPOSITION | MF_GRAYED );
			SendMessage( m_hWnd, DM_SETDEFID, (WPARAM) IDC_EDIT, (LPARAM) 0); 

			return DefWindowProc( (HWND)((CREATESTRUCT *)lParam)->lpCreateParams, uMsg, wParam, lParam );
		}

		case WM_CLOSE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskPIN::ProcecEvent WM_CLOSE (wParam=%X, lParam=%X)",wParam,lParam);

			if( IsIconic( m_hWnd ) )
				return DefWindowProc( m_hWnd, uMsg, wParam, lParam );

			ShowWindow( m_hWnd, SW_MINIMIZE );
			return 0;
		}

		case WM_DESTROY: 
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskPIN::ProcecEvent WM_DESTROY (wParam=%X, lParam=%X)",wParam,lParam);
			break;
		}

		default:
		{
			return DefWindowProc( m_hWnd, uMsg, wParam, lParam );
		}
	}
	return DefWindowProc( m_hWnd, uMsg, wParam, lParam );
}
