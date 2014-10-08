/* ****************************************************************************

* eID Middleware Project.
* Copyright (C) 2011-2012 FedICT.
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

#include "base.h"



CK_RV Beidsdk_GetObjectValue(CK_FUNCTION_LIST_PTR pFunctions, CK_SESSION_HANDLE session_handle,CK_CHAR_PTR pName,CK_VOID_PTR *ppValue, CK_ULONG_PTR pvalueLen);
void Beidsdk_PrintValue(CK_CHAR_PTR pName, CK_BYTE_PTR pValue, CK_ULONG valueLen);
CK_ULONG beidsdk_GetData(void);

int main() {
	CK_ULONG retval = CKR_OK;
	retval = beidsdk_GetData();

	_getch();
}

CK_ULONG beidsdk_GetData() 
{
	void *pkcs11Handle;							//handle to the pkcs11 library		  
	CK_FUNCTION_LIST_PTR pFunctions;		//list of the pkcs11 function pointers
	CK_C_GetFunctionList pC_GetFunctionList;
	CK_RV retVal = CKR_OK;
	DWORD err;

	//open the pkcs11 library
	pkcs11Handle = dlopen(PKCS11_LIB, RTLD_LAZY); // RTLD_NOW is slower
	if (pkcs11Handle != NULL) 
	{
		// get function pointer to C_GetFunctionList
		pC_GetFunctionList = (CK_C_GetFunctionList)dlsym(pkcs11Handle, "C_GetFunctionList");
		if (pC_GetFunctionList != NULL) 
		{
			// invoke C_GetFunctionList to get the list of pkcs11 function pointers
			retVal = (*pC_GetFunctionList) (&pFunctions);
			if (retVal == CKR_OK) 
			{
				// initialize Cryptoki
				retVal = (pFunctions->C_Initialize) (NULL);
				if (retVal == CKR_OK)
				{		
					CK_ULONG slot_count = 0;
					// retrieve the number of slots (cardreaders) found
						//set first parameter to CK_FALSE if you also want to find the slots without a card inserted
						retVal = (pFunctions->C_GetSlotList) (CK_TRUE, 0, &slot_count);
						if ((retVal == CKR_OK) && (slot_count > 0) )
						{
							CK_SLOT_ID_PTR slotIds = (CK_SLOT_ID_PTR)malloc(slot_count * sizeof(CK_SLOT_ID));
							if(slotIds != NULL)
						{
								// retrieve the list of slots (cardreaders)
								retVal = (pFunctions->C_GetSlotList) (CK_TRUE, slotIds, &slot_count);
								if (retVal == CKR_OK)
								{
									CK_ULONG slotIdx;
									for (slotIdx = 0; slotIdx < slot_count; slotIdx++) 
									{
											CK_SESSION_HANDLE session_handle;
											//open a session
											retVal = (pFunctions->C_OpenSession)(slotIds[slotIdx], CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session_handle);
											if (retVal == CKR_OK)
											{
										CK_CHAR_PTR pFilename = (CK_CHAR_PTR)TEXT("carddata_glob_os_version");
										CK_CHAR_PTR pSignatureFilename = (CK_CHAR_PTR)TEXT("CARD_DATA");
										CK_CHAR_PTR pLastname = (CK_CHAR_PTR)TEXT("surname");
										CK_VOID_PTR pFileValue = NULL;
										CK_VOID_PTR pSignatureValue = NULL;
										CK_VOID_PTR pLastnameValue = NULL;
										CK_ULONG		FileValueLen = 0;
										CK_ULONG		SignatureValueLen = 0;
										CK_ULONG		LastnameValueLen = 0;
										//retrieve the data of the file
										retVal = Beidsdk_GetObjectValue(pFunctions, session_handle, pFilename, &pFileValue, &FileValueLen);
										if(retVal == CKR_OK)
											Beidsdk_PrintValue(pFilename,(CK_BYTE_PTR)pFileValue, FileValueLen);
										else
											printf("error 0x%.8x Beidsdk_GetObjectValue\n",retVal);

										//retrieve the data of the signature file
										retVal = Beidsdk_GetObjectValue(pFunctions, session_handle, pSignatureFilename, &pSignatureValue, &SignatureValueLen);
										if(retVal == CKR_OK)
											Beidsdk_PrintValue(pSignatureFilename,(CK_BYTE_PTR)pSignatureValue, SignatureValueLen);
										else
											printf("error 0x%.8x Beidsdk_GetObjectValue\n",retVal);

												//retrieve the lastname
												retVal = Beidsdk_GetObjectValue(pFunctions, session_handle, pLastname, &pLastnameValue, &LastnameValueLen);
												if(retVal == CKR_OK)
													Beidsdk_PrintValue(pLastname,(CK_BYTE_PTR)pLastnameValue, LastnameValueLen);
												else
													printf("error 0x%.8x Beidsdk_GetObjectValue\n",retVal);

										if (pFileValue != NULL)
											free (pFileValue);
										if (pSignatureValue != NULL)
											free (pSignatureValue);
										if (pLastnameValue != NULL)
											free (pLastnameValue);
												//close the session
												if (retVal == CKR_OK)
													retVal = (pFunctions->C_CloseSession) (session_handle);
												else
													(pFunctions->C_CloseSession) (session_handle);
											}
									}//end of for loop
								}
								free(slotIds);
							}
							else //malloc failed
							{
								printf("malloc failed\n");
								retVal = CKR_GENERAL_ERROR;
							}
						}//no slots found
						else if (slot_count == 0)
						{
							printf("no slots found\n");
						}
					if (retVal == CKR_OK)
						retVal = (pFunctions->C_Finalize) (NULL_PTR);
					else
						(pFunctions->C_Finalize) (NULL_PTR);
				}//C_Initialize failed	
			}
			else //CK_C_GetFunctionList failed
			{
				retVal = CKR_GENERAL_ERROR;
				printf("error 0x%.8x C_GetFunctionList\n",retVal);
			}
		}
		else //dlsym failed
		{
			retVal = CKR_GENERAL_ERROR;
		}
		dlclose(pkcs11Handle);
	}
	else //dlopen failed
	{
		retVal = CKR_GENERAL_ERROR;
		printf("%s not found\n",PKCS11_LIB);
		err = GetLastError();
		printf("err is 0x%.8x\n",err);
		//14001 MSVCR80.DLL not found
	}
	return retVal;
} 

CK_RV Beidsdk_GetObjectValue(CK_FUNCTION_LIST_PTR pFunctions, CK_SESSION_HANDLE session_handle,CK_CHAR_PTR pName,CK_VOID_PTR *ppValue, CK_ULONG_PTR pvalueLen)
{
	CK_RV retVal = CKR_OK;
	CK_ULONG data = CKO_DATA;
	CK_ATTRIBUTE searchtemplate[2] = {{CKA_CLASS,&data,sizeof(CK_ULONG)},{CKA_LABEL,(CK_VOID_PTR)pName,(CK_ULONG)strlen(pName)}};
	*pvalueLen = 0;

	//initialize the search for the objects with label <filename>
	retVal = (pFunctions->C_FindObjectsInit)(session_handle, searchtemplate, 2); 
	if (retVal == CKR_OK)
	{
		CK_OBJECT_HANDLE hObject = 0;
		CK_ULONG ulObjectCount = 0;
		//find the first object with label <filename>
		retVal = (pFunctions->C_FindObjects)(session_handle, &hObject,1,&ulObjectCount); 

		if ( (ulObjectCount == 1) && (retVal == CKR_OK) )
		{
			//NULL_PTR as second argument, so the length of value is filled in to retValueLen
			CK_ATTRIBUTE attr_templ[1] = {CKA_VALUE,NULL_PTR,0};

			//retrieve the length of the data from the object
			retVal = (pFunctions->C_GetAttributeValue)(session_handle,hObject,attr_templ,1);
			if ((retVal == CKR_OK) && ((CK_LONG)(attr_templ[0].ulValueLen) != -1))
			{
				*ppValue = malloc (attr_templ[0].ulValueLen);												
				if (*ppValue != NULL )
				{
					attr_templ[0].pValue = *ppValue;
					//retrieve the data from the object
					retVal = (pFunctions->C_GetAttributeValue)(session_handle,hObject,attr_templ,1);
					*pvalueLen = attr_templ[0].ulValueLen;
				}						  
				else
				{
					//error allocating memory for pValue
					retVal = CKR_GENERAL_ERROR;
				}					  
			}
		}
		//finalize the search
		retVal = (pFunctions->C_FindObjectsFinal)(session_handle); 
	}
	return retVal;
}

void Beidsdk_PrintValue(CK_CHAR_PTR pName, CK_BYTE_PTR pValue, CK_ULONG valueLen)
{
	unsigned long counter = 0;
	printf("\n%s:\n\n",pName);
	if(pValue != NULL)
	{
		while (counter < valueLen)
		{
			if( (0x29 < *(pValue+counter)) && (*(pValue+counter) < 0x81) )
				printf("%c", *(pValue+counter));
			else
				printf(".", *(pValue+counter));
			counter++;
		}
	}
}
