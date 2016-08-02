/* ****************************************************************************

* eID Middleware Project.
* Copyright (C) 2009-2011 FedICT.
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
#include "cert_registration.h"

/* forward definitions */
CK_RV Beidsdk_GetObjectValue(CK_FUNCTION_LIST_PTR pFunctions, CK_SESSION_HANDLE session_handle,CK_CHAR_PTR filename,CK_VOID_PTR* ppValue, CK_ULONG_PTR pvalueLen);
CK_RV Beidsdk_RegisterCertificates(CK_FUNCTION_LIST_PTR pFunctions, CK_SESSION_HANDLE session_handle, CK_BYTE_PTR cardSerialNumber, CK_ULONG cardSerialNumberLen);
void Beidsdk_PrintValue(CK_CHAR_PTR pName, CK_BYTE_PTR pValue, CK_ULONG valueLen);
CK_ULONG beidsdk_Main(void);

int main() {
	CK_ULONG retval = CKR_OK;
	retval = beidsdk_Main();

	printf("Done. Return value: %ld (%s)\npress a key to exit...", retval, retval == CKR_OK ? "ok" : "NOT ok");

	_getch();
}


CK_ULONG beidsdk_Main() 
{
	HMODULE pkcs11Handle;				//handle to the pkcs11 library
	CK_FUNCTION_LIST_PTR pFunctions;		//list of the pkcs11 function pointers
	CK_C_GetFunctionList pC_GetFunctionList;
	CK_RV retVal = CKR_OK;

	//open the pkcs11 library
	pkcs11Handle = dlopen(PKCS11_LIB, RTLD_LAZY); // RTLD_NOW is slower
	if (pkcs11Handle == NULL) { retVal = CKR_GENERAL_ERROR; return retVal; }

	// get function pointer to C_GetFunctionList
	pC_GetFunctionList = (CK_C_GetFunctionList)dlsym(pkcs11Handle, "C_GetFunctionList");
	if (pC_GetFunctionList == NULL) { retVal = CKR_GENERAL_ERROR; goto close; }

	// invoke C_GetFunctionList to get the list of pkcs11 function pointers
	retVal = (*pC_GetFunctionList) (&pFunctions);
	if (retVal != CKR_OK) { goto close; }

	// initialize Cryptoki
	retVal = (pFunctions->C_Initialize) (NULL);
	if (retVal != CKR_OK) {	goto close; }

	CK_ULONG slot_count = 0;
	// retrieve the number of slots (cardreaders) found
	retVal = (pFunctions->C_GetSlotList) (CK_FALSE, 0, &slot_count);
	if (slot_count == 0) { retVal = CKR_GENERAL_ERROR; printf("no slots found\n"); }
	if (retVal != CKR_OK) { goto finalize; }

	CK_SLOT_ID_PTR slotIds = (CK_SLOT_ID_PTR)malloc(slot_count * sizeof(CK_SLOT_INFO));
	if(slotIds == NULL) { printf("malloc failed\n"); retVal = CKR_GENERAL_ERROR; goto finalize; }

	// retrieve the list of slots (cardreaders)
	retVal = (pFunctions->C_GetSlotList) (CK_FALSE, slotIds, &slot_count);
	if (retVal != CKR_OK) { goto freeslots; }

	CK_ULONG slotIdx;
	for (slotIdx = 0; slotIdx < slot_count; slotIdx++) 
	{
		CK_SESSION_HANDLE session_handle;
		//open a session
		retVal = (pFunctions->C_OpenSession)(slotIds[slotIdx], CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session_handle);
		if (retVal != CKR_OK) { continue; }

		CK_CHAR_PTR pSerialNumber = (CK_CHAR_PTR)TEXT("carddata_serialnumber");
		CK_VOID_PTR pSerialNumberValue = NULL;
		CK_ULONG	SerialNumberValueLen = 0;

		//retrieve the data of the file
		retVal = Beidsdk_GetObjectValue(pFunctions, session_handle, pSerialNumber, &pSerialNumberValue, &SerialNumberValueLen);
		if(retVal == CKR_OK)
		{
			Beidsdk_RegisterCertificates(pFunctions, session_handle,(CK_BYTE_PTR)pSerialNumberValue,SerialNumberValueLen);
			Beidsdk_PrintValue(pSerialNumber,(CK_BYTE_PTR)pSerialNumberValue, SerialNumberValueLen);
		}
		else
			printf("error 0x%.8x Beidsdk_GetObjectValue\n",retVal);

		if (pSerialNumberValue != NULL)
			free (pSerialNumberValue);

		//close the session
		if (retVal == CKR_OK)
			retVal = (pFunctions->C_CloseSession) (session_handle);
		else
			(pFunctions->C_CloseSession) (session_handle);
	}
freeslots:
	free(slotIds);
finalize:
	if (retVal == CKR_OK)
		retVal = (pFunctions->C_Finalize) (NULL_PTR);
	else
		(pFunctions->C_Finalize) (NULL_PTR);
close:
	dlclose(pkcs11Handle);
	return retVal;
} 


CK_RV Beidsdk_GetObjectValue(CK_FUNCTION_LIST_PTR pFunctions, CK_SESSION_HANDLE session_handle,CK_CHAR_PTR pName,CK_VOID_PTR *ppValue, CK_ULONG_PTR pvalueLen)
{
	CK_RV retVal = CKR_OK;
	CK_ULONG classType = CKO_DATA;
	CK_ATTRIBUTE searchtemplate[2] = {{CKA_LABEL,(CK_VOID_PTR)pName,(CK_ULONG)strlen(pName)},{CKA_CLASS,&classType,sizeof(CK_ULONG)}};
	*pvalueLen = 0;

	//initialize the search for the objects with label <filename>
	retVal = (pFunctions->C_FindObjectsInit)(session_handle, searchtemplate, 2); 
	if (retVal != CKR_OK) { return retVal; }

	CK_OBJECT_HANDLE hObject = 0;
	CK_ULONG ulObjectCount = 0;
	//find the first object with label <filename>
	retVal = (pFunctions->C_FindObjects)(session_handle, &hObject,1,&ulObjectCount); 
	if ( (ulObjectCount != 1) || (retVal != CKR_OK) ) { goto finalize; }

	//NULL_PTR as second argument, so the length of value is filled in to retValueLen
	CK_ATTRIBUTE attr_templ[2] = {{CKA_VALUE,NULL_PTR,0},{CKA_CLASS,CKO_DATA,sizeof(CK_ULONG)}};

	//retrieve the length of the data from the object
	retVal = (pFunctions->C_GetAttributeValue)(session_handle,hObject,attr_templ,1);
	if ((retVal != CKR_OK) || ((CK_LONG)(attr_templ[0].ulValueLen) == -1)) { goto finalize; }

	*ppValue = malloc (attr_templ[0].ulValueLen);
	if (*ppValue == NULL ) { retVal = CKR_GENERAL_ERROR; goto finalize; }

	attr_templ[0].pValue = *ppValue;
	//retrieve the data from the object
	retVal = (pFunctions->C_GetAttributeValue)(session_handle,hObject,attr_templ,1);
	*pvalueLen = attr_templ[0].ulValueLen;
finalize:
	//finalize the search
	retVal = (pFunctions->C_FindObjectsFinal)(session_handle); 
	return retVal;
}

CK_RV Beidsdk_RegisterCertificates(CK_FUNCTION_LIST_PTR pFunctions, CK_SESSION_HANDLE session_handle, CK_BYTE_PTR cardSerialNumber, CK_ULONG SerialNumberValueLen)
{
	CK_RV retVal = CKR_OK;
	CK_ULONG classType = CKO_CERTIFICATE;
	CK_ATTRIBUTE searchtemplate[1] = {CKA_CLASS,&classType,sizeof(CK_ULONG)};

	//initialize the search for the objects with label <filename>
	retVal = (pFunctions->C_FindObjectsInit)(session_handle, searchtemplate, 1); 
	if (retVal != CKR_OK) { return retVal; }

	CK_OBJECT_HANDLE hObject = 0;
	CK_ULONG ulObjectCount = 0;
	//find the first object with class CKO_CERTIFICATE
	retVal = (pFunctions->C_FindObjects)(session_handle, &hObject,1,&ulObjectCount); 
	while( (ulObjectCount == 1) && (retVal == CKR_OK) )
	{
		//NULL_PTR as second argument, so the length of value is filled in to retValueLen
		CK_ATTRIBUTE attr_templ[1] = {CKA_VALUE,NULL_PTR,0};

		//retrieve the length of the data from the object
		retVal = (pFunctions->C_GetAttributeValue)(session_handle,hObject,attr_templ,1);
		if ((retVal == CKR_OK) && ((CK_LONG)(attr_templ[0].ulValueLen) != -1))
		{
			CK_BYTE* pValue = (CK_BYTE*)malloc (attr_templ[0].ulValueLen);												
			if (pValue != NULL )
			{
				attr_templ[0].pValue = pValue;
				//retrieve the data from the object
				retVal = (pFunctions->C_GetAttributeValue)(session_handle,hObject,attr_templ,1);
				if (retVal == CKR_OK)
				{
					ImportCertificate(pValue,attr_templ[0].ulValueLen,cardSerialNumber,SerialNumberValueLen);
				}
				free(pValue);
			}						  
			else
			{
				//error allocating memory for pValue
				retVal = CKR_GENERAL_ERROR;
			}					  
		}
		retVal = (pFunctions->C_FindObjects)(session_handle, &hObject,1,&ulObjectCount); 
	}
	//finalize the search
	retVal = (pFunctions->C_FindObjectsFinal)(session_handle); 
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
	printf("\n");
}
