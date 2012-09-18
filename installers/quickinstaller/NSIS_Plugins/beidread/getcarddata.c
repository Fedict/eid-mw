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
#include "getcarddata.h"


char* g_firstNames = NULL;
char* g_firstLetterThirdName = NULL;
char* g_surName = NULL;
char* g_address_Street_Number = NULL;
char* g_address_Zip = NULL;
char* g_address_Municipality = NULL;


/*int _main(int argv, char* args[])
{
	CK_RV retval;	
	retval = ReadTheCardData();
	return 0;
}*/

CK_RV ReadTheCardData(void) {

	CK_RV retval = CKR_OK;				//return value of last pkcs11 function called
	CK_FUNCTION_LIST_PTR functions=NULL;		// list of the pkcs11 function pointers

	CK_SESSION_HANDLE session_handle;
	CK_ULONG slot_count;
	CK_SLOT_ID_PTR slotIds;
	CK_ULONG slotIdx;

	int err = GetLastError();
	printf("err = %d\n",err);

	SetLastError(0);

	retval=loadpkcs11(&functions);
	if( retval != CKR_OK )
		return retval;

	if( (retval=(functions->C_Initialize) (NULL)) == CKR_OK)
	{		
		if( (retval=(functions->C_GetSlotList) (CK_TRUE, 0, &slot_count)) == CKR_OK)
		{
			if(slot_count == 0)
			{
				retval = CKR_TOKEN_NOT_PRESENT; 				//no card found -> log this
			}
			slotIds = (CK_SLOT_ID_PTR)malloc(slot_count * sizeof(CK_SLOT_INFO));
			if(slotIds != NULL)
			{
				if( (retval = (functions->C_GetSlotList) (CK_TRUE, slotIds, &slot_count)) == CKR_OK)
				{
					if(slot_count == 0)
					{
						//no card found -> log this
						retval = CKR_TOKEN_NOT_PRESENT;
					}
					for (slotIdx = 0; slotIdx < slot_count; slotIdx++) 
					{
						retval = (functions->C_OpenSession)(slotIds[slotIdx], CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session_handle);
						if(retval == CKR_OK)
						{
							if ((retval = FindAndStore(functions, session_handle, "firstnames",&g_firstNames)) == CKR_OK)
							if ((retval = FindAndStore(functions, session_handle, "first_letter_of_third_given_name",&g_firstLetterThirdName)) == CKR_OK)
							if ((retval = FindAndStore(functions, session_handle, "surname",&g_surName)) == CKR_OK)
							if ((retval = FindAndStore(functions, session_handle, "address_street_and_number",&g_address_Street_Number)) == CKR_OK)
							if ((retval = FindAndStore(functions, session_handle, "address_zip",&g_address_Zip)) == CKR_OK)
							if ((retval = FindAndStore(functions, session_handle, "address_municipality",&g_address_Municipality)) == CKR_OK)
							(functions->C_CloseSession) (session_handle);
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
		(functions->C_Finalize) (NULL_PTR);
	}

	unloadpkcs11();

	return retval;
} 

CK_RV FindAndStore(CK_FUNCTION_LIST_PTR functions, CK_SESSION_HANDLE session_handle, const char* label, char** data_storage)
{
	CK_RV retval = CKR_OK;
	CK_ULONG ulObjectCount=1;
	CK_OBJECT_HANDLE hObject;
	CK_ULONG data = CKO_DATA;
	CK_ATTRIBUTE attributes[] = {{CKA_CLASS,&data,sizeof(CK_ULONG)},{CKA_LABEL,(void*)label,strlen(label)}};

	retval = (functions->C_FindObjectsInit)(session_handle, attributes, 2); 
	if(retval == CKR_OK)
	{
		retval = (functions->C_FindObjects)(session_handle, &hObject,1,&ulObjectCount); 
		if(retval == CKR_OK)
		{
			CK_ATTRIBUTE attr_templ[] = {{CKA_VALUE,NULL_PTR,0}};
			retval = (functions->C_GetAttributeValue)(session_handle,hObject,attr_templ,1);
			if(retval == CKR_OK)
			{
				*data_storage = (char*)malloc (attr_templ[0].ulValueLen+1);//one extra for terminating null					
				if (*data_storage != NULL )
				{
					attr_templ[0].pValue = *data_storage;
					retval = (functions->C_GetAttributeValue)(session_handle,hObject,attr_templ,1);
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
		(functions->C_FindObjectsFinal)(session_handle); 
	}	
	return retval;
}
