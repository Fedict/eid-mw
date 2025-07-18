

#include <windows.h>
#include <pluginapi.h> // nsis plugin
#include "common.h"
#include "getcarddata.h"
#include "getreadercount.h"

HINSTANCE g_hInstance;
HWND g_hwndParent;
extern char* g_firstNames;
extern char* g_firstLetterThirdName;
extern char* g_surName;
//extern char* g_address_Street_Number;
//extern char* g_address_Zip;
//extern char* g_address_Municipality;

void testandpushstring(char* thestring);
// Safe string push function that handles UTF-8 to local codepage conversion
void pushstring_safe(const char* str) {
	if(str == NULL || strlen(str) == 0) {
		pushstring("");
		return;
	}
	
	// Convert UTF-8 to current Windows ANSI codepage
	// This works perfectly with ANSI NSIS
	int wlen = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	if(wlen <= 0) {
		pushstring(str);
		return;
	}
	
	wchar_t* wbuf = (wchar_t*)malloc(wlen * sizeof(wchar_t));
	if(wbuf == NULL) {
		pushstring(str);
		return;
	}
	
	if (MultiByteToWideChar(CP_UTF8, 0, str, -1, wbuf, wlen) <= 0) {
		free(wbuf);
		pushstring(str);
		return;
	}
	
	// Convert to local ANSI codepage for ANSI NSIS
	int alen = WideCharToMultiByte(CP_ACP, 0, wbuf, -1, NULL, 0, NULL, NULL);
	if(alen <= 0) {
		free(wbuf);
		pushstring(str);
		return;
	}
	
	char* abuf = (char*)malloc(alen);
	if(abuf == NULL) {
		free(wbuf);
		pushstring(str);
		return;
	}
	
	if(WideCharToMultiByte(CP_ACP, 0, wbuf, -1, abuf, alen, NULL, NULL) > 0) {
		pushstring(abuf);
	} else {
		pushstring(str);
	}
	
	free(wbuf);
	free(abuf);
}

void __declspec(dllexport) ReadCardData(HWND hwndParent, int string_size, 
	char *variables, stack_t **stacktop,
	extra_parameters *extra)
{
	CK_RV retval;	

	g_hwndParent=hwndParent;
	EXDLL_INIT();
	{
		retval = ReadTheCardData();
		if(retval == CKR_OK)
		{
			testandpushstring(g_firstNames);
			testandpushstring(g_firstLetterThirdName);
			testandpushstring(g_surName);
//			testandpushstring(g_address_Street_Number);
//			testandpushstring(g_address_Zip);
//			testandpushstring(g_address_Municipality);
		}
		pushint(retval);
	}
}

void __declspec(dllexport) GetReaderCount(HWND hwndParent, int string_size, 
	char *variables, stack_t **stacktop, extra_parameters *extra)
{
	CK_RV retval;	
	int nrofCardReaders = 0;

	g_hwndParent=hwndParent;
	EXDLL_INIT();
	{
		int cardsInserted = popint();
		retval = GetTheReaderCount(&nrofCardReaders, cardsInserted);
		if(retval == CKR_OK)
		{
			pushint(nrofCardReaders);
		}
		pushint(retval);
	}
}

void testandpushstring(char* thestring)
{
	if(thestring != NULL){
		pushstring_safe(thestring);
	}
	else{
		pushstring_safe(" ");
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

