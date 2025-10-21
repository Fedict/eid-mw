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
#include "dlgwndaskpins.h"
#include "resource.h"
#include "../langutil.h"
#include "common/log.h"
#include "mw_util.h"

#define IDC_STATIC 0
#define IDB_OK 1
#define IDB_CANCEL 2
#define IDC_EDIT_PIN1 3
#define IDC_EDIT_PIN2 4
#define IDC_EDIT_PIN3 5
#define IMG_SIZE 128
#define IDC_EDIT 3
#define IDB_KeypadStart 10
#define IDB_KeypadEnd   21
#define KP_BTN_SIZE 48
#define KP_LBL_SIZE 24

#define INPUTFIELD_OLD 0
#define INPUTFIELD_NEW 1
#define INPUTFIELD_CONFIRM 2

dlgWndAskPINs::dlgWndAskPINs( DlgPinInfo pinInfo1, DlgPinInfo pinInfo2, std::wstring & Header, std::wstring & PINName, bool UseKeypad, HWND Parent )
:Win32Dialog(L"WndAskPINs")
{
	static const int points_per_inch = 96;
	HMONITOR h_monitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFOEX mInfo;
	mInfo.cbSize = sizeof(mInfo);
	GetMonitorInfo(h_monitor, &mInfo);
	int pixels_per_inch = GetDeviceCaps(CreateDCW(mInfo.szDevice, NULL, NULL, NULL), LOGPIXELSY);
	int scalingValue = pixels_per_inch / points_per_inch;
	int pixels_height = (16 * scalingValue);

	m_UseKeypad = UseKeypad;

	InputField1_OK = InputField2_OK = InputField3_OK = false;
	Pin1Result[0] = ' ';
	Pin1Result[1] = (char)0;
	Pin2Result[0] = ' ';
	Pin2Result[1] = (char)0;
	DrawError = false;

	std::wstring tmpTitle = L"";
	tmpTitle += GETSTRING_DLG(RenewingPinCode);

	m_ulPinMaxLen = pinInfo1.ulMaxLen;
	m_ulPin1MinLen = pinInfo1.ulMinLen;
	m_ulPin1MaxLen = pinInfo1.ulMaxLen;
	m_ulPin2MinLen = pinInfo2.ulMinLen;
	m_ulPin2MaxLen = pinInfo2.ulMaxLen;
	
	szHeader = const_cast<wchar_t *>(Header.c_str());
	szPIN = PINName.c_str();

	int Height = 300 * scalingValue;
	if( m_UseKeypad )
		Height = 430 * scalingValue;

	if( CreateWnd( tmpTitle.c_str() , 280 * scalingValue, Height, 0, Parent ) )
	{
		RECT clientRect;
		GetClientRect( m_hWnd, &clientRect );

		TextFont = CreateFont(pixels_height, 0, 0, 0, FW_DONTCARE, 0, 0, 0,
				DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"MS Shell Dlg" );

		HWND hOkButton = CreateWindow(
			L"BUTTON", GETSTRING_DLG(Ok), WS_CHILD | WS_VISIBLE | WS_TABSTOP, 
			clientRect.right - 175 * scalingValue, clientRect.bottom - 36 * scalingValue, 
			72 * scalingValue, 24 * scalingValue, m_hWnd, (HMENU)IDB_OK, m_hInstance, NULL );
		EnableWindow( hOkButton, false );

		HWND hCancelButton = CreateWindow(
			L"BUTTON", GETSTRING_DLG(Cancel), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON, 
			clientRect.right - 95 * scalingValue, clientRect.bottom - 36 * scalingValue, 
			85 * scalingValue, 24 * scalingValue, m_hWnd, (HMENU)IDB_CANCEL, m_hInstance, NULL );

		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_PASSWORD;
		if( pinInfo1.ulFlags & PIN_FLAG_DIGITS )
			dwStyle |= ES_NUMBER;

		if( m_UseKeypad )
		{
			m_UK_InputField = 0;

			HWND hTextEdit = CreateWindowEx( WS_EX_CLIENTEDGE,
				L"EDIT", L"", dwStyle, 
				62 * scalingValue, clientRect.top + 20 * scalingValue, 
				clientRect.right - 94 * scalingValue, 26 * scalingValue,
				m_hWnd, (HMENU)IDC_EDIT, m_hInstance, NULL );
			SendMessage( hTextEdit, EM_LIMITTEXT, m_ulPin1MaxLen, 0 );

			HWND hStaticText = CreateWindow( 
				L"STATIC", szPIN, WS_CHILD | WS_VISIBLE | SS_RIGHT, 
				16 * scalingValue, clientRect.top + 24 * scalingValue, 
				36 * scalingValue, 22 * scalingValue,
				m_hWnd, (HMENU)IDC_STATIC, m_hInstance, NULL );

			int top = 60 * scalingValue;
			int bottom = clientRect.bottom - 70 * scalingValue - IMG_SIZE;
			int left = 46 * scalingValue;
			int right = clientRect.right - 46 * scalingValue;
			int hMargin = 12 * scalingValue;
			int vMargin = 12 * scalingValue;
			int totwidth = right - left;
			int totheight = bottom - top;
			int btnwidth = ( totwidth - 2*hMargin ) / 3;
			int btnheight = ( totheight - 3*vMargin ) /4;
			if( btnheight < btnwidth )
			{
				btnwidth = btnheight;
				hMargin = (totwidth - 3*btnwidth)/2;
			}
			else if( btnheight > btnwidth )
			{
				btnheight = btnwidth;
				vMargin = (totheight - 3*btnheight)/2;
			}
			for( int i = 0; i < 4; i++ )
			{
				for( int j = 0; j < 3; j++ )
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
			
			SendMessage( hStaticText, WM_SETFONT, (WPARAM)TextFont, 0 );
		}
		else
		{

			HWND hTextEdit1 = CreateWindowEx( WS_EX_CLIENTEDGE,
				L"EDIT", L"", dwStyle, 
				clientRect.right/2 - 30 * scalingValue, clientRect.bottom - 150 * scalingValue, 
				160 * scalingValue, 26 * scalingValue, m_hWnd, (HMENU)IDC_EDIT_PIN1, m_hInstance, NULL );
			SendMessage( hTextEdit1, EM_LIMITTEXT, m_ulPin1MaxLen, 0 );


			dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_PASSWORD;
			if( pinInfo2.ulFlags & PIN_FLAG_DIGITS )
				dwStyle |= ES_NUMBER;

			HWND hTextEdit2 = CreateWindowEx( WS_EX_CLIENTEDGE,
				L"EDIT", L"", dwStyle, 
				clientRect.right/2 - 30 * scalingValue, clientRect.bottom - 120 * scalingValue, 
				160 * scalingValue, 26 * scalingValue, m_hWnd, (HMENU)IDC_EDIT_PIN2, m_hInstance, NULL );
			SendMessage( hTextEdit2, EM_LIMITTEXT, m_ulPin2MaxLen, 0 );

			HWND hTextEdit3 = CreateWindowEx( WS_EX_CLIENTEDGE,
				L"EDIT", L"", dwStyle, 
				clientRect.right/2 - 30 * scalingValue, clientRect.bottom - 90 * scalingValue, 
				160 * scalingValue, 26 * scalingValue, m_hWnd, (HMENU)IDC_EDIT_PIN3, m_hInstance, NULL );
			SendMessage( hTextEdit3, EM_LIMITTEXT, m_ulPin2MaxLen, 0 );


			HWND hStaticText1 = CreateWindow( 
				L"STATIC", GETSTRING_DLG(CurrentPin), WS_CHILD | WS_VISIBLE | SS_RIGHT,
				0, clientRect.bottom - 146 * scalingValue, clientRect.right/2 - 35 * scalingValue, 
				22 * scalingValue, m_hWnd, (HMENU)IDC_STATIC, m_hInstance, NULL );

			HWND hStaticText2 = CreateWindow( 
				L"STATIC", GETSTRING_DLG(NewPin), WS_CHILD | WS_VISIBLE | SS_RIGHT,
				0, clientRect.bottom - 116 * scalingValue, clientRect.right/2 - 35 * scalingValue, 
				22 * scalingValue, m_hWnd, (HMENU)IDC_STATIC, m_hInstance, NULL );

			HWND hStaticText3 = CreateWindow( 
				L"STATIC", GETSTRING_DLG(ConfirmNewPin), WS_CHILD | WS_VISIBLE | SS_RIGHT,
				0, clientRect.bottom - 86 * scalingValue, clientRect.right/2 - 35 * scalingValue, 
				38 * scalingValue, m_hWnd, (HMENU)IDC_STATIC, m_hInstance, NULL );


			//ImageKP_BTN = ImageKP_BTN_Mask = NULL;

			SendMessage( hStaticText1, WM_SETFONT, (WPARAM)TextFont, 0 );
			SendMessage( hStaticText2, WM_SETFONT, (WPARAM)TextFont, 0 );
			SendMessage( hStaticText3, WM_SETFONT, (WPARAM)TextFont, 0 );
		}

		SendMessage( hOkButton, WM_SETFONT, (WPARAM)TextFont, 0 );
		SendMessage( hCancelButton, WM_SETFONT, (WPARAM)TextFont, 0 );

		ImagePIN = LoadBitmap( m_hInstance, MAKEINTRESOURCE(IDB_PIN) );
		CreateBitapMask( ImagePIN, ImagePIN_Mask );

		SetFocus( GetDlgItem( m_hWnd, IDC_EDIT ) );

	}
}

dlgWndAskPINs::~dlgWndAskPINs()
{
	KillWindow( );

	for (int i = 0; i < 12; ++i)
	{
		if (ImageKP_BTN[i]) { DeleteObject(ImageKP_BTN[i]); ImageKP_BTN[i] = NULL; }
	}
	if (ImageKP_BTN_Mask) { DeleteObject(ImageKP_BTN_Mask); ImageKP_BTN_Mask = NULL; }

	if (ImagePIN) { DeleteObject(ImagePIN); ImagePIN = NULL; }
	if (ImagePIN_Mask) { DeleteObject(ImagePIN_Mask); ImagePIN_Mask = NULL; }

	if (TextFont) { DeleteObject(TextFont); TextFont = NULL; }
}

void dlgWndAskPINs::GetPinResult()
{
	wchar_t nameBuf[128];
	long len = (long)SendMessage( GetDlgItem( m_hWnd, IDC_EDIT_PIN1 ), WM_GETTEXTLENGTH, 0, 0 );
	if( len < 128 )
	{
		SendMessage( GetDlgItem( m_hWnd, IDC_EDIT_PIN1 ), WM_GETTEXT, (WPARAM)(sizeof(nameBuf)), (LPARAM)nameBuf );
		wcscpy_s( Pin1Result, nameBuf );

		SendMessage( GetDlgItem( m_hWnd, IDC_EDIT_PIN2 ), WM_GETTEXT, (WPARAM)(sizeof(nameBuf)), (LPARAM)nameBuf );
		wcscpy_s( Pin2Result, nameBuf );
	}
}

bool dlgWndAskPINs::CheckPin2Result()
{
	wchar_t PINBuf[128];
	wchar_t PINBuf2[128];
	long len = (long)SendMessage(GetDlgItem(m_hWnd, IDC_EDIT_PIN2), WM_GETTEXTLENGTH, 0, 0);
	if (len < 128)
	{
		SendMessage( GetDlgItem( m_hWnd, IDC_EDIT_PIN2 ), WM_GETTEXT, (WPARAM)(sizeof(PINBuf)), (LPARAM)PINBuf );
		wcscpy_s( PINBuf2, PINBuf );
	}
	else {
		return false;
	}
	len = (long)SendMessage(GetDlgItem(m_hWnd, IDC_EDIT_PIN3), WM_GETTEXTLENGTH, 0, 0);
	if (len < 128)
	{
		SendMessage(GetDlgItem(m_hWnd, IDC_EDIT_PIN3), WM_GETTEXT, (WPARAM)(sizeof(PINBuf)), (LPARAM)PINBuf);
	}
	else {
		return false;
	}
	if (wcscoll(PINBuf, PINBuf2) == 0)
		return true;
	return false;
}
void dlgWndAskPINs::SetHeaderText(const wchar_t * txt)
{
	RECT rect;
	szHeader = txt;
	GetClientRect( m_hWnd, &rect );
	rect.bottom -= 40;
	rect.top = rect.bottom - IMG_SIZE + 32;
	rect.left += 136;
	rect.right -= 8;
	InvalidateRect( m_hWnd, &rect, TRUE );
	UpdateWindow( m_hWnd );
}

LRESULT dlgWndAskPINs::ProcecEvent
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
				case IDC_EDIT_PIN1: // == IDC_EDIT
				{
					if( EN_CHANGE == HIWORD(wParam) )
					{
						if( m_UseKeypad )
						{
							unsigned int len = (unsigned int)SendMessage( GetDlgItem( m_hWnd, IDC_EDIT_PIN1 ), WM_GETTEXTLENGTH, 0, 0 );
							unsigned int iTmp = m_UK_InputField ? m_ulPin2MinLen : m_ulPin1MinLen;
							EnableWindow( GetDlgItem( m_hWnd, IDOK ), ( iTmp <= len ) );
						}
						else
						{
							unsigned int len = (unsigned int)SendMessage( GetDlgItem( m_hWnd, IDC_EDIT_PIN1 ), WM_GETTEXTLENGTH, 0, 0 );
							InputField1_OK = len >= m_ulPin1MinLen;
							EnableWindow( GetDlgItem( m_hWnd, IDOK ), ( InputField1_OK && InputField2_OK && InputField3_OK ) );
						}
					}
					return TRUE;
				}
				case IDC_EDIT_PIN2:
				{
					if( EN_CHANGE == HIWORD(wParam) )
					{
						unsigned int len = (unsigned int)SendMessage( GetDlgItem( m_hWnd, IDC_EDIT_PIN2 ), WM_GETTEXTLENGTH, 0, 0 );
						InputField2_OK = len >= m_ulPin2MinLen;
						EnableWindow( GetDlgItem( m_hWnd, IDOK ), ( InputField1_OK && InputField2_OK && InputField3_OK ) );
					}
					return TRUE;
				}
				case IDC_EDIT_PIN3:
				{
					if( EN_CHANGE == HIWORD(wParam) )
					{
						unsigned int len = (unsigned int)SendMessage( GetDlgItem( m_hWnd, IDC_EDIT_PIN3 ), WM_GETTEXTLENGTH, 0, 0 );
						InputField3_OK = len >= m_ulPin2MinLen;
						EnableWindow( GetDlgItem( m_hWnd, IDOK ), ( InputField1_OK && InputField2_OK && InputField3_OK ) );
					}
					return TRUE;
				}

				case IDB_OK:
					if( m_UseKeypad )
					{
						if( m_UK_InputField == INPUTFIELD_OLD )
						{
							// Store the Input @ Pin1
							wchar_t nameBuf[128];
							long len = (long)SendMessage( GetDlgItem( m_hWnd, IDC_EDIT ), WM_GETTEXTLENGTH, 0, 0 );
							if( len < 128 )
							{
								SendMessage( GetDlgItem( m_hWnd, IDC_EDIT ), WM_GETTEXT, (WPARAM)(sizeof(nameBuf)), (LPARAM)nameBuf );
								wcscpy_s( Pin1Result, nameBuf );
							}
							SetHeaderText( GETSTRING_DLG(EnterYourNewPinCode));
						}
						else if( m_UK_InputField == INPUTFIELD_NEW )
						{
							// Store the Input @ Pin2
							wchar_t nameBuf[128];
							long len = (long)SendMessage( GetDlgItem( m_hWnd, IDC_EDIT ), WM_GETTEXTLENGTH, 0, 0 );
							if( len < 128 )
							{
								SendMessage( GetDlgItem( m_hWnd, IDC_EDIT ), WM_GETTEXT, (WPARAM)(sizeof(nameBuf)), (LPARAM)nameBuf );
								wcscpy_s( Pin2Result, nameBuf );
							}
							DrawError = false;
							GetClientRect( m_hWnd, &rect );
							rect.bottom -= 36;
							rect.top = rect.bottom - 30;
							InvalidateRect( m_hWnd, &rect, TRUE );
							UpdateWindow( m_hWnd );
							SetHeaderText( GETSTRING_DLG(EnterYourNewPinCodeAgainToConfirm) );
						}
						else // INPUTFIELD_CONFIRM
						{
							// Check if the Input is equal to Pin2
							wchar_t nameBuf[128];
							SendMessage( GetDlgItem( m_hWnd, IDC_EDIT ), WM_GETTEXT, (WPARAM)(sizeof(nameBuf)), (LPARAM)nameBuf );
							if( wcscoll( nameBuf, Pin2Result ) == 0 )
							{
								dlgResult = eIDMW::DLG_OK;
								close();
							}
							else // Not OK
							{
								m_UK_InputField = INPUTFIELD_NEW;
								SendMessage( GetDlgItem( m_hWnd, IDC_EDIT ), WM_SETTEXT, 0, (LPARAM)"" );

								SetHeaderText( GETSTRING_DLG(RetryEnterYourNewPinCode) );
								DrawError = true;
								GetClientRect( m_hWnd, &rect );
								rect.bottom -= 36;
								rect.top = rect.bottom - 30;
								InvalidateRect( m_hWnd, &rect, TRUE );
								UpdateWindow( m_hWnd );
								return TRUE;
							}
						}
						m_UK_InputField++;
						SendMessage( GetDlgItem( m_hWnd, IDC_EDIT ), WM_SETTEXT, 0, (LPARAM)"" );
					}
					else if( !CheckPin2Result() ) // !m_UseKeypad
					{
						DrawError = true;
						GetClientRect( m_hWnd, &rect );
						rect.bottom -= 36;
						rect.top = rect.bottom - 30;
						InvalidateRect( m_hWnd, &rect, TRUE );
						UpdateWindow( m_hWnd );
					}
					else
					{
						GetPinResult();
						dlgResult = eIDMW::DLG_OK;
						close();
					}
					return TRUE;

				case IDB_CANCEL:
					//strcpy( PinResult, "" );
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
					}
					return DefWindowProc( m_hWnd, uMsg, wParam, lParam );
			}
		}

		case WM_DRAWITEM:
		{
			LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT)lParam;
			if( lpDrawItem->CtlType & ODT_BUTTON )
			{
				// Background without leaking a brush
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

				// Determine label and text
				unsigned int iNum = 0;
				wchar_t label[3] = L"";
				if (lpDrawItem->CtlID == IDB_KeypadEnd)         { iNum = 10; wcscpy_s(label, L"CE"); }
				else if (lpDrawItem->CtlID == IDB_KeypadEnd - 1) { iNum = 0;  wcscpy_s(label, L"0");  }
				else if (lpDrawItem->CtlID >= IDB_KeypadStart && lpDrawItem->CtlID < IDB_KeypadEnd - 2)
				{
					iNum = (lpDrawItem->CtlID - IDB_KeypadStart) + 1; // 1..9
					label[0] = L'0' + (wchar_t)iNum; label[1] = L'\0';
				}

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
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskPINs::ProcecEvent WM_SIZE (wParam=%X, lParam=%X)",wParam,lParam);

			if( IsIconic( m_hWnd ) )
				return 0;
			break;
		}

		case WM_PAINT:
		{
			m_hDC = BeginPaint( m_hWnd, &ps );

				HDC hdcMem;
				GetClientRect( m_hWnd, &rect );

				hdcMem = CreateCompatibleDC( m_hDC );
				SelectObject( hdcMem , ImagePIN );

				if( m_UseKeypad )
				{
					MaskBlt( m_hDC, 4, rect.bottom - 40 - IMG_SIZE,
						IMG_SIZE, IMG_SIZE,	hdcMem, 0, 0,
						ImagePIN_Mask, 0, 0, MAKEROP4( SRCCOPY, 0x00AA0029 ) );
					rect.bottom -= 40;
					rect.top = rect.bottom - IMG_SIZE + 32;
				}
				else
				{
					MaskBlt( m_hDC, 4, 4, IMG_SIZE, IMG_SIZE,
						hdcMem, 0, 0, ImagePIN_Mask, 0, 0,
						MAKEROP4( SRCCOPY, 0x00AA0029 ) );
					rect.bottom = IMG_SIZE + 32;
					rect.top += 32;
				}

				DeleteDC(hdcMem);

				rect.left += 136;
				rect.right -= 8;
				SetBkColor( m_hDC, GetSysColor( COLOR_3DFACE ) );
				SelectObject( m_hDC, TextFont );
				DrawText( m_hDC, szHeader, -1, &rect, DT_WORDBREAK );

				if( DrawError )
				{
					SetTextColor( m_hDC, RGB(255, 0, 0) );
					
					GetClientRect( m_hWnd, &rect );
					rect.left += 10;
					rect.bottom -= 36;
					rect.top = rect.bottom - 30;

					DrawText( m_hDC, GETSTRING_DLG(ErrorTheNewPinCodesAreNotIdentical), -1, &rect, DT_SINGLELINE | DT_VCENTER );
				}

				if( m_UseKeypad )
				{
					GetClientRect( m_hWnd, &rect );
					rect.left += 8;
					rect.right -= 8;
					rect.top += 8;
					rect.bottom -= 48 + IMG_SIZE;

					DrawEdge( m_hDC, &rect, EDGE_RAISED, BF_RECT );
				}

			EndPaint( m_hWnd, &ps );

			SetForegroundWindow( m_hWnd );

			return 0;
		}

		case WM_ACTIVATE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskPINs::ProcecEvent WM_ACTIVATE (wParam=%X, lParam=%X)",wParam,lParam);
			break;
		}

		case WM_NCACTIVATE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskPINs::ProcecEvent WM_NCACTIVATE (wParam=%X, lParam=%X)",wParam,lParam);

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
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskPINs::ProcecEvent WM_SETFOCUS (wParam=%X, lParam=%X)",wParam,lParam);
			break;
		}

		case WM_KILLFOCUS:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskPINs::ProcecEvent WM_KILLFOCUS (wParam=%X, lParam=%X)",wParam,lParam);

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
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskPINs::ProcecEvent WM_CREATE (wParam=%X, lParam=%X)",wParam,lParam);

			HMENU hSysMenu;

			hSysMenu = GetSystemMenu( m_hWnd, FALSE );
			EnableMenuItem( hSysMenu, 2, MF_BYPOSITION | MF_GRAYED );
			SendMessage( m_hWnd, DM_SETDEFID, (WPARAM)IDC_EDIT_PIN1, (LPARAM) 0); 

			return 1;
		}


		case WM_CLOSE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskPINs::ProcecEvent WM_CLOSE (wParam=%X, lParam=%X)",wParam,lParam);

			if( IsIconic( m_hWnd ) )
				return DefWindowProc( m_hWnd, uMsg, wParam, lParam );

			ShowWindow( m_hWnd, SW_MINIMIZE );
			return 0;
		}

		case WM_DESTROY: 
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskPINs::ProcecEvent WM_DESTROY (wParam=%X, lParam=%X)",wParam,lParam);
			break;
		}

		default:
		{
			return DefWindowProc( m_hWnd, uMsg, wParam, lParam );
		}
	}
	return DefWindowProc( m_hWnd, uMsg, wParam, lParam );
}
