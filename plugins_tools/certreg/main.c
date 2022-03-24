// certreg.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "main.h"
#include "certreg.h"
#include "carddata.h"

#define MAX_LOADSTRING 100
#define IDB_REG 1
#define IDB_CANCEL 2
#define IDC_EDIT 3

#define PKCS11THREAD_RUN		  0
#define STOP_PKCS11THREAD			1
#define PKCS11THREAD_STOPPED  2

#define REG_BUTTON_HEIGHT 24
#define REG_BUTTON_HEIGHT_MARGIN 12
#define REG_BUTTON_WIDTH 152
#define REG_BUTTON_FROM_RIGHT 240
#define QUIT_BUTTON_WIDTH 72
#define QUIT_BUTTON_FROM_RIGHT 80

// Global Variables:
HINSTANCE hInst;								// current instance
HWND hTextEdit;									// the edit control
DWORD gStopThreads;
DWORD gAutoFlags;
CK_FUNCTION_LIST_PTR gfunctions; // the pkcs11 functions
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

DWORD WINAPI pkcs11EventsThread( LPVOID ThreadVars ); //the thread used to detect card insertion/removed events

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
BOOL InitPKCS11(void *handle);

int APIENTRY _tWinMain(HINSTANCE hInstance,
											 HINSTANCE hPrevInstance,
											 LPTSTR    lpCmdLine,
											 int       nCmdShow)
{
	//UNREFERENCED_PARAMETER(hPrevInstance);
	//UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;
	void *pkcs11_handle = NULL;
	CK_RV retval = CKR_OK;
	DWORD   dwThreadId;
	HANDLE  hThreadHandle; 
	gStopThreads = PKCS11THREAD_RUN;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CERTREG, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}
	if (!InitPKCS11(pkcs11_handle))
	{
		SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"Exiting application in 5 seconds\r\n");
		Sleep(5000);
		return FALSE;
	}

	retval = (gfunctions->C_Initialize) (NULL);
	if (retval != CKR_OK)
	{	
		SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"ERROR: C_Initialize failed :\r\n");
	}
	else
	{
		// Create pkcs11Events thread
		hThreadHandle = CreateThread( 
			NULL,             // default security attributes
			0,                // use default stack size  
			pkcs11EventsThread,			// thread function name
			NULL,			// argument to thread function 
			0,                // use default creation flags 
			&dwThreadId);			// returns the thread identifier 

		if (hThreadHandle == NULL)
		{
			SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"ERROR: Failed to start CardEventThread\r\n");
			SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"Automated functionality is inactive\r\n");
		}

		hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CERTREG));

		// Main message loop:
		while (GetMessage(&msg, NULL, 0, 0))
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			if(gStopThreads == PKCS11THREAD_STOPPED)
			{
				if(msg.hwnd != NULL)
					DestroyWindow(msg.hwnd);
				break;
			}
		}
		//retval = (gfunctions->C_Finalize) (NULL_PTR);
	}

	if (hThreadHandle != NULL)
	{
		gStopThreads = 1;
		// Wait for max 5 seconds until pkcs11Events thread is terminated.
		if (WaitForSingleObject(hThreadHandle, 5000) == WAIT_OBJECT_0)
		{
			CloseHandle(hThreadHandle);
		}
	}


	dlclose(pkcs11_handle);

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CERTREG));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_CERTREG);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	RECT clientRect;
	HWND hOkButton;
	HWND hCancelButton;

	hInst = hInstance; // Store instance handle in our global variable

	gAutoFlags = AUTO_REGISTER | AUTO_REMOVE;
	//hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	//   CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		0, 0, 480, 240, NULL, NULL, hInstance, NULL);

	GetClientRect( hWnd, &clientRect );


	//hTextEdit = CreateWindowEx( WS_EX_CLIENTEDGE,
	//	L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, 
	//	10, 10, 240, 120,
	//	hWnd, (HMENU)IDC_EDIT, hInstance, NULL );
	//| WS_VSCROLL | WS_HSCROLL
	hTextEdit = CreateWindow("EDIT", "",  WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_LEFT | WS_VSCROLL,
		10, 10, 440, 100, hWnd, (HMENU)IDC_EDIT, hInstance, NULL);

	if (!hTextEdit)
	{
		return FALSE;
	}
	//SendMessage( hTextEdit, EM_LIMITTEXT, m_ulPinMaxLen, 0 );
	//SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)L"Please verify if :\r\n");
	//SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)L"   - a cardreader is connected;\r\n");
	//SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)L"   - an eID card is present in the cardreader.\r\n");

	//Register Button
	hOkButton = CreateWindow(
		"BUTTON", "Register Certificates", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON, 
		clientRect.right - REG_BUTTON_FROM_RIGHT, clientRect.bottom - (REG_BUTTON_HEIGHT+REG_BUTTON_HEIGHT_MARGIN),
		REG_BUTTON_WIDTH, REG_BUTTON_HEIGHT, 
		hWnd, (HMENU)IDB_REG, hInstance, NULL );
	//EnableWindow( hOkButton, false );

	//Quit Button
	hCancelButton = CreateWindow(
		"BUTTON", "Quit", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_TEXT, 
		clientRect.right - QUIT_BUTTON_FROM_RIGHT, clientRect.bottom - (REG_BUTTON_HEIGHT+REG_BUTTON_HEIGHT_MARGIN),
		QUIT_BUTTON_WIDTH, REG_BUTTON_HEIGHT, 
		hWnd, (HMENU)IDB_CANCEL, hInstance, NULL );

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

BOOL InitPKCS11(void *handle)
{
	CK_RV retval = CKR_OK; 
	CK_C_GetFunctionList pC_GetFunctionList;

	handle = dlopen("beid_ff_pkcs11.dll", RTLD_LAZY); // RTLD_NOW is slower
	if (NULL == handle) {
		SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"beid_ff_pkcs11.dll not found\r\n");
		handle = dlopen("beidpkcs11.dll", RTLD_LAZY); // RTLD_NOW is slower
		if (NULL == handle) {
			SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"ERROR: beidpkcs11.dll not found\r\n");
			return FALSE;
		}
		else{
			SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"beidpkcs11.dll found\r\n");
		}
	}

	// get function pointer to C_GetFunctionList
	pC_GetFunctionList = (CK_C_GetFunctionList) dlsym(handle, "C_GetFunctionList");
	if (pC_GetFunctionList == NULL) {
		dlclose(handle);
		SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"ERROR: getting C_GetFunctionList failed :\r\n");
		return FALSE;
	}

	// invoke C_GetFunctionList
	retval = (*pC_GetFunctionList) (&gfunctions);
	if (retval != CKR_OK) {
		SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"ERROR: C_GetFunctionList failed :\r\n");
		return FALSE;  
	}
	return TRUE;
}


BOOL CALLBACK ChildWindowResize(HWND hWndChild, LPARAM lParam)
{
	LPRECT rcPar = (LPRECT)lParam;
	LONG childId = GetWindowLong(hWndChild,GWL_ID);

	switch(childId)
	{
	case IDB_REG:
		MoveWindow(hWndChild,
			rcPar->right-REG_BUTTON_FROM_RIGHT,
			rcPar->bottom - REG_BUTTON_HEIGHT,
			REG_BUTTON_WIDTH,
			REG_BUTTON_HEIGHT,
			TRUE);
		break;
	case IDB_CANCEL:
		MoveWindow(hWndChild,
			rcPar->right-QUIT_BUTTON_FROM_RIGHT,
			rcPar->bottom - REG_BUTTON_HEIGHT,
			QUIT_BUTTON_WIDTH,
			REG_BUTTON_HEIGHT,
			TRUE);
		break;
	case IDC_EDIT:
		MoveWindow(hWndChild,
			rcPar->left+10,
			rcPar->top+10,
			(rcPar->right - rcPar->left) - 20,
			rcPar->bottom - (REG_BUTTON_HEIGHT + REG_BUTTON_HEIGHT_MARGIN),
			TRUE);
		break;
	default:
		break;
	};
	return TRUE;
}
//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	DWORD flags = 0;
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rcClient;
	HMENU hmenu;            // top-level menu 
DWORD retval = 0;
	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_AUTOREGISTER:
			flags=AUTO_REGISTER;
		case IDM_AUTOREMOVE:	
			if(flags==0)
				flags=AUTO_REMOVE;

			// Get the menu.
			if ((	hmenu = GetMenu(hWnd)) == NULL) 
				return 0;

			if( (gAutoFlags & flags) != 0)
			{
				//flag was on, turn it off
				CheckMenuItem(hmenu,wmId,MF_UNCHECKED|MF_BYCOMMAND);
				gAutoFlags -= flags;
			}
			else
			{
				CheckMenuItem(hmenu,wmId,MF_CHECKED|MF_BYCOMMAND);
				gAutoFlags |= flags;
			}
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDB_REG:
			SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"Searching for eID card..\r\n");
			getcertificates(hTextEdit,gfunctions);
			break;
		case IDB_CANCEL:
		case IDM_EXIT:
			if(gStopThreads != PKCS11THREAD_STOPPED)
				gStopThreads = STOP_PKCS11THREAD;
			gfunctions->C_Finalize(NULL_PTR);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_SIZE:
		GetClientRect(hWnd,&rcClient);
		EnumChildWindows(hWnd,ChildWindowResize, (LPARAM) &rcClient);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

DWORD WINAPI pkcs11EventsThread( LPVOID ThreadVars ) 
{ 
	//threadVars->threadRetVal = readslots(functions);
	while (gStopThreads == PKCS11THREAD_RUN)
	{
		WaitForCardEvent(hTextEdit, gfunctions, &gAutoFlags);
		//Sleep(2000);
		SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)".");
	}

	gStopThreads = PKCS11THREAD_STOPPED;
  return 0; 
} 
