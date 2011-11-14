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
#include "getcarddata.h"

HMODULE g_handle = NULL;						//handle to the pkcs11 library
CK_FUNCTION_LIST_PTR g_functions=NULL;		// list of the pkcs11 function pointers

char* g_firstNames = NULL;
char* g_firstLetterThirdName = NULL;
char* g_surName = NULL;
char* g_address_Street_Number = NULL;
char* g_address_Zip = NULL;
char* g_address_Municipality = NULL;

CK_RV loadpkcs11(void);//private function declaration 

/*int _main(int argv, char* args[])
{
	CK_RV retval;	
	retval = ReadTheCardData();
	return 0;
}*/

CK_RV ReadTheCardData(void) {

	CK_RV retval = CKR_OK;				//return value of last pkcs11 function called

	CK_SESSION_HANDLE session_handle;
	CK_ULONG slot_count;
	CK_SLOT_ID_PTR slotIds;
	CK_ULONG slotIdx;

	int err = GetLastError();
	printf("err = %d\n",err);

	SetLastError(0);
	if(g_handle == NULL)
	{
		retval=loadpkcs11();
		if( retval != CKR_OK )
			return retval;
	}

	if( (retval=(g_functions->C_Initialize) (NULL)) == CKR_OK)
	{		
		if( (retval=(g_functions->C_GetSlotList) (0, 0, &slot_count)) == CKR_OK)
		{
			slotIds = (CK_SLOT_ID_PTR)malloc(slot_count * sizeof(CK_SLOT_INFO));
			if(slotIds != NULL)
			{
				if( (retval = (g_functions->C_GetSlotList) (CK_TRUE, slotIds, &slot_count)) == CKR_OK)
				{
					if(slot_count == 0)
					{
						//no card found -> log this
						retval = CKR_TOKEN_NOT_PRESENT;
					}
					for (slotIdx = 0; slotIdx < slot_count; slotIdx++) 
					{
						retval = (g_functions->C_OpenSession)(slotIds[slotIdx], CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session_handle);
						if(retval == CKR_OK)
						{
							if ((retval = FindAndStore(g_functions, session_handle, "FirstNames",&g_firstNames)) == CKR_OK)
							if ((retval = FindAndStore(g_functions, session_handle, "First_Letter_Of_Third_Given_Name",&g_firstLetterThirdName)) == CKR_OK)
							if ((retval = FindAndStore(g_functions, session_handle, "Surname",&g_surName)) == CKR_OK)
							if ((retval = FindAndStore(g_functions, session_handle, "Address_Street_Number",&g_address_Street_Number)) == CKR_OK)
							if ((retval = FindAndStore(g_functions, session_handle, "Address_Zip",&g_address_Zip)) == CKR_OK)
							if ((retval = FindAndStore(g_functions, session_handle, "Address_Municipality",&g_address_Municipality)) == CKR_OK)
							(g_functions->C_CloseSession) (session_handle);
						}			
					}
				}
				free(slotIds);
			}
			else 
			{
				retval = CKR_HOST_MEMORY;
			}
		}
		(g_functions->C_Finalize) (NULL_PTR);
	}

	unloadpkcs11();

	return retval;
} 

CK_RV loadpkcs11(void) {
	CK_RV retval = CKR_OK;
	CK_C_GetFunctionList pC_GetFunctionList;
	int err = GetLastError();

	g_handle = LoadLibrary(PKCS11_LIB);//"F:\\Windows\\System32\\beidpkcs11.dll");
	if (g_handle == NULL) {
		err = GetLastError();
		printf("err = %d\n",err);
		return CKR_GENERAL_ERROR;
	}
	// get function pointer to C_GetFunctionList
	pC_GetFunctionList = (CK_C_GetFunctionList) GetProcAddress(g_handle, "C_GetFunctionList");
	if (pC_GetFunctionList == NULL) {
		FreeLibrary(g_handle);
		return CKR_GENERAL_ERROR;
	}
	// invoke C_GetFunctionList
	retval = (*pC_GetFunctionList) (&g_functions);

	return retval;
}

CK_RV unloadpkcs11(void) {
	if(g_handle == NULL)
	{
		FreeLibrary(g_handle);
	}
	return CKR_OK;
}

CK_RV FindAndStore(CK_FUNCTION_LIST_PTR g_functions, CK_SESSION_HANDLE session_handle, const char* label, char** data_storage)
{
	CK_RV retval = CKR_OK;
	CK_ULONG ulObjectCount=1;
	CK_OBJECT_HANDLE hObject;
	CK_ULONG data = CKO_DATA;
	CK_ATTRIBUTE attributes[] = {{CKA_CLASS,&data,sizeof(CK_ULONG)},{CKA_LABEL,(void*)label,strlen(label)}};

	retval = (g_functions->C_FindObjectsInit)(session_handle, attributes, 2); 
	if(retval == CKR_OK)
	{
		retval = (g_functions->C_FindObjects)(session_handle, &hObject,1,&ulObjectCount); 
		if(retval == CKR_OK)
		{
			CK_ATTRIBUTE attr_templ[] = {{CKA_VALUE,NULL_PTR,0}};
			retval = (g_functions->C_GetAttributeValue)(session_handle,hObject,attr_templ,1);
			if(retval == CKR_OK)
			{
				*data_storage = (char*)malloc (attr_templ[0].ulValueLen+1);//one extra for terminating null					
				if (*data_storage != NULL )
				{
					attr_templ[0].pValue = *data_storage;
					retval = (g_functions->C_GetAttributeValue)(session_handle,hObject,attr_templ,1);
					if(retval == CKR_OK)
					{
						(*data_storage)[attr_templ[0].ulValueLen] = '\0';
					}
				}		
				else
				{
					retval = CKR_HOST_MEMORY;
				}	
			}									
		}
		(g_functions->C_FindObjectsFinal)(session_handle); 
	}	
	return retval;
}
