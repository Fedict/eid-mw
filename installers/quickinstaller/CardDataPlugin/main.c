

#include <windows.h>
#include <nsis/pluginapi.h> // nsis plugin
#include "getcarddata.h"

HINSTANCE g_hInstance;
HWND g_hwndParent;
extern char* g_firstNames;
extern char* g_firstLetterThirdName;
extern char* g_surName;
extern char* g_address_Street_Number;
extern char* g_address_Zip;
extern char* g_address_Municipality;

void testandpushstring(const char* thestring);

void __declspec(dllexport) ReadCardData(HWND hwndParent, int string_size, 
	char *variables, stack_t **stacktop,
	extra_parameters *extra)
{
	CK_RV retval;	

	g_hwndParent=hwndParent;
	EXDLL_INIT();

	retval = ReadTheCardData();
	if(retval == CKR_OK)
	{
		testandpushstring(g_firstNames);
		testandpushstring(g_firstLetterThirdName);
		testandpushstring(g_surName);
		testandpushstring(g_address_Street_Number);
		testandpushstring(g_address_Zip);
		testandpushstring(g_address_Municipality);
	}
	pushint(retval);
}

void testandpushstring(char* thestring)
{
	if( (thestring != NULL) && (thestring != "") ){
		pushstring(thestring);
	}
	else{
		pushstring(" ");
	}
	if( thestring != NULL)
	{
		free(thestring);
		thestring = NULL;
	}
}

BOOL WINAPI DllMain(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
	g_hInstance=(HINSTANCE)hInst;
	switch(ul_reason_for_call)
	{
	case DLL_PROCESS_DETACH:
		//unloadpkcs11();
		break;
	default:
		break;
	}
	return TRUE;
}

