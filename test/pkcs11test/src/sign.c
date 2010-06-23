/*
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

/*
 * Integration test for the PKCS#11 library.
 * Tests the opening and closing of a PKCS#11 session.
 * Required interaction: none.
 */

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

#define dlopen(lib,h) LoadLibrary(lib)
#define dlsym(h, function) GetProcAddress(h, function)
#define dlclose(h) FreeLibrary(h)
#define PKCS11_LIB "libpkcs11.dll"
#define RTLD_LAZY	1
#define RTLD_NOW	2
#define RTLD_GLOBAL 4

#else
#include <opensc/pkcs11.h>
#include <dlfcn.h>
#include <unistd.h>
#define PKCS11_LIB "/usr/local/lib/libbeidpkcs11.so" 
#endif
#include <stdlib.h>

void check_return_value(CK_RV rv, const char *message)
{
        if (rv != CKR_OK) {
                fprintf(stderr, "Error at %s: %x\n",
                        message, (unsigned int)rv);
                exit(EXIT_FAILURE);
        }
}


void read_private_keys(CK_FUNCTION_LIST_PTR functions, CK_SESSION_HANDLE session)
{
     CK_RV rv;
     CK_OBJECT_CLASS 	keyClass 	= CKO_CERTIFICATE;
     CK_BBOOL		trueClass 	= CK_TRUE;
     CK_ATTRIBUTE	attr[2];

     attr[0].type 		= CKA_CLASS;
     attr[0].pValue 		= &keyClass;
     attr[0].ulValueLen 	= sizeof(keyClass);

     attr[1].type 		= CKA_TOKEN;
     attr[1].pValue 		= &trueClass;
     attr[1].ulValueLen 	= sizeof(trueClass);

     CK_ULONG objectCount;
     CK_OBJECT_HANDLE object[16];

     rv = (*functions->C_FindObjectsInit)(session, attr,2 );
     check_return_value(rv, "Find objects init");

     rv = (*functions->C_FindObjects)(session, object,16, &objectCount);
     check_return_value(rv, "Find first object");

	printf("objectcount: %d\n",objectCount);

     rv = (*functions->C_FindObjectsFinal)(session);
     check_return_value(rv, "Find objects final");
}

void printTokenInfo(CK_TOKEN_INFO_PTR tokenInfo)
{
	tokenInfo->label[31]='\0';		printf("label       : %32s\n", tokenInfo->label);
	tokenInfo->manufacturerID[31]='\0';	printf("manufacturer: %32s\n", tokenInfo->manufacturerID);
	tokenInfo->model[15]='\0';		printf("model       : %16s\n", tokenInfo->model);
	tokenInfo->serialNumber[15]='\0';	printf("serial      : %16s\n", tokenInfo->serialNumber);
}


int main()
{
   	void *handle;
   	CK_C_GetFunctionList pC_GetFunctionList;
   	CK_RV rv;
   	CK_FUNCTION_LIST_PTR functions;
	CK_INFO info;
   	CK_SESSION_HANDLE session_handle;
   	CK_SLOT_ID_PTR slot_list;
   	long slot_count;
   	CK_SLOT_ID_PTR slotIds;
	CK_TOKEN_INFO tokenInfo;
	int slotIdx;

   	printf("PKCS11 test\n");

	printf("dlopen %s\n",PKCS11_LIB);
   	handle = dlopen(PKCS11_LIB, RTLD_LAZY); // RTLD_NOW is slower
   	if (NULL == handle)
	{
   	    fprintf(stderr, "dlopen error\n");
		exit(1);    
   	}

   	// get function pointer to C_GetFunctionList 
	printf("get C_GetFunctionList\n");
   	pC_GetFunctionList = (CK_C_GetFunctionList) dlsym(handle, "C_GetFunctionList");
   	if (pC_GetFunctionList == NULL)
	{
   	    dlclose(handle);
   	    printf("failure\n");
   	    exit(1);
   	}

   	// invoke C_GetFunctionList
	printf("invoke C_GetFunctionList\n");
   	rv = (*pC_GetFunctionList) (&functions);
   	if (rv != CKR_OK)
	{
   	    fprintf(stderr, "C_GetFunctionList failed\n");
   	    exit(1);    
   	}

   	// C_Initialize
	printf("invoke C_Initialize\n");
   	rv = (*functions->C_Initialize) (NULL);
     	check_return_value(rv, "initialize");

	// C_GetInfo
	printf("invoke C_GetInfo\n");
	rv = (*functions->C_GetInfo) (&info);
     	check_return_value(rv, "getinfo");

	printf("library version: %d.%d\n", info.libraryVersion.major, info.libraryVersion.minor);
	printf("PKCS#11 version: %d.%d\n", info.cryptokiVersion.major, info.cryptokiVersion.minor);

	// C_GetSlotList
	printf("invoke C_GetSlotList\n");
    	rv = (*functions->C_GetSlotList) (0, 0, &slot_count);
     	check_return_value(rv, "getslotlist (to get count)");

    	printf("slot count: %i\n", slot_count);
    	slotIds = malloc(slot_count * sizeof(CK_SLOT_INFO));

	printf("invoke C_GetSlotList\n");
    	rv = (*functions->C_GetSlotList)(CK_FALSE, slotIds, &slot_count);
     	check_return_value(rv, "getslotlist (to get info)");

	CK_SLOT_INFO slotInfo;		
	CK_SLOT_ID slotId = slotIds[0];
	int idx;

	printf("invoke C_GetSlotInfo\n");
	rv = (*functions->C_GetSlotInfo)(slotId,&slotInfo);
     	check_return_value(rv, "getslotinfo");

	for(idx = 64 - 1; idx > 0; idx--)
	{
		if (slotInfo.slotDescription[idx] == ' ')
		{
			slotInfo.slotDescription[idx] = '\0';			
		}
		else
		{
			break;
		}		
	}

	printf("slot Id: %d\n", slotId);
	printf("slot description: %s\n", slotInfo.slotDescription);

	rv = (*functions->C_GetTokenInfo)(slotId, &tokenInfo);
     	check_return_value(rv, "opensession");
	printTokenInfo(&tokenInfo);

	// C_OpenSession
	printf("invoke C_OpenSession\n");
   	rv = (*functions->C_OpenSession)(slotId, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session_handle);
     	check_return_value(rv, "opensession");

	char pin[4]="1234";
	printf("invoke C_Login\n");
       	rv = (*functions->C_Login)(session_handle, CKU_USER, NULL, 0);
     	check_return_value(rv, "login");

	read_private_keys(functions,session_handle);

	printf("invoke C_Logout\n");
	rv=(*functions->C_Logout)(session_handle);
     	check_return_value(rv, "logout");

	// C_CloseSession
	printf("invoke C_CloseSession\n");
	rv = (*functions->C_CloseSession) (session_handle);
     	check_return_value(rv, "closesession");


	// C_Finalize
finalize:
	printf("invoke C_Finalize\n");
	rv = (*functions->C_Finalize) (NULL_PTR);
	if (CKR_OK != rv) {
		fprintf(stderr, "C_Finalize error\n");
		exit(1);
	}
    dlclose(handle);
}



