/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2009-2012 FedICT.
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
#ifndef basetest_h
#define basetest_h

#include <stdio.h>
#ifdef WIN32
//allign at 1 byte
#pragma pack(push, cryptoki, 1)
#include <win32.h>
#include <pkcs11.h>
#pragma pack(pop, cryptoki)
//back to default allignment

#include <windows.h>
#include <conio.h>
#include <tchar.h>
#include <strsafe.h>

#define dlopen(lib,h) LoadLibrary(lib)
#define dlsym(h, function) GetProcAddress(h, function)
#define dlclose(h) FreeLibrary(h)
#ifdef WIN64
#define PKCS11_LIB L"..\\..\\VS_2010\\Binaries\\x64_Release\\beidpkcs11.dll"
#else
#define PKCS11_LIB "beidpkcs11.dll"//"F:\\trunk\\VS_2010\\Binaries\\Win32_Debug\\beidpkcs11D.dll"//L"beid_ff_pkcs11.dll"//
#endif
#define RTLD_LAZY	1
#define RTLD_NOW	2
#define RTLD_GLOBAL 4

#else

#include "include/rsaref220/unix.h"
#include "include/rsaref220/pkcs11.h"

//#include "pkcs11.h"
#include <dlfcn.h>
#include <unistd.h>
#define PKCS11_LIB "/usr/local/lib/libbeidpkcs11.dylib" 
#endif
#include <stdlib.h>

typedef enum{
	TEST_PASSED = 0,
	TEST_SKIPPED,
	TEST_WARNING,
	TEST_ERROR,
	TEST_FAILED
}BASETEST_RV;

typedef struct{
	CK_RV pkcs11rv;
	BASETEST_RV basetestrv;
} testRet;

/****************************************************************************
 * Base Test Functions prototypes
 ***************************************************************************/
int GetPKCS11FunctionList(CK_FUNCTION_LIST_PTR *pFunctions,void *handle);
CK_BBOOL ReturnedSuccesfull(CK_RV frv, CK_RV *ptrv, char* pkcs11function, char* test_name );
CK_BBOOL ReturnedSucces(CK_RV frv, CK_RV *ptrv, char* pkcs11function);

//fills in the c_functionlist and gives back a handle to the pkcs11 library
CK_BBOOL InitializeTest(void **phandle,CK_FUNCTION_LIST_PTR *pfunctions);

//fills in the c_functionlist and gives back a handle to the pkcs11 library
//uses the pkcs11 library to retrieve the slotlist and slotcount
testRet PrepareSlotListTest(void **phandle,CK_FUNCTION_LIST_PTR *pfunctions, CK_SLOT_ID_PTR* pslotIds, CK_ULONG_PTR pulCount,CK_BBOOL tokenPresent );
void EndSlotListTest(void *handle, CK_SLOT_ID_PTR slotIds );

//function to print the slotdescription of a certain slot, specified by its slotId
testRet bt_logslotdescription(CK_FUNCTION_LIST_PTR *pfunctions, CK_ULONG slotId);
#endif
