/* ****************************************************************************

* eID Middleware Project.
* Copyright (C) 2012 FedICT.
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

#include "common.h"

HMODULE g_handle = NULL;						//handle to the pkcs11 library

CK_RV loadpkcs11(CK_FUNCTION_LIST_PTR * pFunctions) {
	CK_RV retval = CKR_OK;
	CK_C_GetFunctionList pC_GetFunctionList;
	int err = GetLastError();

	if(g_handle == NULL){
		g_handle = LoadLibrary(TEXT("F:\\Windows\\System32\\beidpkcs11.dll"));//PKCS11_LIB);//"F:\\Windows\\System32\\beidpkcs11.dll");
		if (g_handle == NULL) {
			err = GetLastError();
			printf("err = %d\n",err);
			return CKR_GENERAL_ERROR;
		}
	}
	// get function pointer to C_GetFunctionList
	pC_GetFunctionList = (CK_C_GetFunctionList) GetProcAddress(g_handle, "C_GetFunctionList");
	if (pC_GetFunctionList == NULL) {
		FreeLibrary(g_handle);
		return CKR_GENERAL_ERROR;
	}
	// invoke C_GetFunctionList
	retval = (*pC_GetFunctionList) (pFunctions);

	return retval;
}

CK_RV unloadpkcs11(void) {
	if(g_handle != NULL)
	{
		FreeLibrary(g_handle);
		g_handle = NULL;
	}
	return CKR_OK;
}
