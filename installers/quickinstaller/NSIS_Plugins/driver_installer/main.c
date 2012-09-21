

#include <windows.h>
#include <pluginapi.h> // nsis plugin
#include "updatedriver.h"

HINSTANCE g_hInstance;
HWND g_hwndParent;

void __declspec(dllexport) UpdateDriver(HWND hwndParent, int string_size, 
	char *variables, stack_t **stacktop,
	extra_parameters *extra)
{
	int retval;	

	g_hwndParent=hwndParent;
	EXDLL_INIT();
	{
		BOOL restart = 0;
		DWORD flags = 0;//INSTALLFLAG_NONINTERACTIVE
		DWORD lastErrorCode;
		char hardwareID[256];
		char infFilePath[256];
		popstring(hardwareID);
		popstring(infFilePath);

		retval = eIDUpdateDriver(hwndParent,hardwareID,infFilePath,flags,&restart, &lastErrorCode);

		if(retval != 0)
		{
			//on succes, let the caller know if a system reboot was requested
			pushint((int)restart);
		}
		else
		{
			//on failure, send the error code
			pushint((int)lastErrorCode);
		}

		pushint(retval);
	}
}


void __declspec(dllexport) getOSVersion(HWND hwndParent, int string_size, 
	char *variables, stack_t **stacktop,
	extra_parameters *extra)
{
	g_hwndParent=hwndParent;
	EXDLL_INIT();

	{
		OSVERSIONINFO versionInfo;

		ZeroMemory(&versionInfo, sizeof(OSVERSIONINFO));
		versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&versionInfo);

		pushint(versionInfo.dwMinorVersion);
		pushint(versionInfo.dwMajorVersion);
	}
}


BOOL WINAPI DllMain(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
	g_hInstance=(HINSTANCE)hInst;
	switch(ul_reason_for_call)
	{
	case DLL_PROCESS_DETACH:
		break;
	default:
		break;
	}
	return TRUE;
}

