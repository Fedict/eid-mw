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

/*
 * This complete example shows how to read identity data from the card.
 */

CK_RV Beidsdk_GetObjectValue(CK_FUNCTION_LIST_PTR pFunctions, CK_SESSION_HANDLE session_handle,CK_CHAR_PTR pName,CK_VOID_PTR *ppValue, CK_ULONG_PTR pvalueLen);
void Beidsdk_PrintCharArray(CK_CHAR_PTR pName, CK_BYTE_PTR pValue, CK_ULONG valueLen);
void Beidsdk_PrintUTF8String(CK_CHAR_PTR pName, CK_BYTE_PTR pValue, CK_ULONG valueLen);
CK_ULONG beidsdk_GetData(void);

int main() {
	CK_ULONG retval = CKR_OK;
#ifdef WIN32
	//we will write in utf-8 to the windows command prompt
	SetConsoleOutputCP(CP_UTF8);
#endif
	retval = beidsdk_GetData();

	printf("Done. Press a key to continue...\n");
	_getch();
}

CK_ULONG beidsdk_GetData() 
{
	void *pkcs11Handle;				//handle to the pkcs11 library		  
	CK_FUNCTION_LIST_PTR pFunctions;		//list of the pkcs11 function pointers
	CK_C_GetFunctionList pC_GetFunctionList;
	CK_RV retVal = CKR_OK;

	//open the pkcs11 library
	pkcs11Handle = dlopen(PKCS11_LIB, RTLD_LAZY);   // RTLD_NOW is slower
	if (pkcs11Handle == NULL) {
		printf("%s not found\n",PKCS11_LIB);
#ifdef WIN32
		DWORD err;
		err = GetLastError();
		printf("err is 0x%.8x\n",err);
		//14001 is "MSVCR80.DLL not found"
#else
   		printf("err is %s", dlerror());
#endif
		return CKR_GENERAL_ERROR;
	} 

	// get function pointer to C_GetFunctionList
	pC_GetFunctionList = (CK_C_GetFunctionList)dlsym(pkcs11Handle, "C_GetFunctionList");
	if (pC_GetFunctionList == NULL) { retVal = CKR_GENERAL_ERROR; goto out; }

	// invoke C_GetFunctionList to get the list of pkcs11 function pointers
	retVal = (*pC_GetFunctionList) (&pFunctions);
	if (retVal != CKR_OK) { goto out; }

	// initialize Cryptoki
	retVal = (pFunctions->C_Initialize) (NULL);
	if (retVal != CKR_OK) { goto out; }

	CK_ULONG slot_count = 0;
	// retrieve the number of slots (cardreaders) found
	//set first parameter to CK_FALSE if you also want to find the slots without a card inserted
	retVal = (pFunctions->C_GetSlotList) (CK_TRUE, 0, &slot_count);
	if (slot_count == 0) { retVal = CKR_GENERAL_ERROR; printf("no slots found\n"); }
	if (retVal != CKR_OK) { goto finalize; }

	CK_SLOT_ID_PTR slotIds = (CK_SLOT_ID_PTR)malloc(slot_count * sizeof(CK_SLOT_ID));
	if(slotIds == NULL) { printf("malloc failed\n"); retVal = CKR_GENERAL_ERROR; goto finalize; }

	// Now retrieve the list of slots (cardreaders).
	retVal = (pFunctions->C_GetSlotList) (CK_TRUE, slotIds, &slot_count);
	if (retVal != CKR_OK) { goto freeslots; }

	/* Note: the above should ideally be done in a loop, since the
	 * number of slots reported by C_GetSlotList might increase if
	 * the user inserts a card (or card reader) at exactly the right
	 * moment. See PKCS#11 (pkcs-11v2-11r1.pdf) for details.
	 */

	/* Loop over the reported slots and read data from any eID card found */
	CK_ULONG slotIdx;
	for (slotIdx = 0; slotIdx < slot_count; slotIdx++) 
	{
		CK_SESSION_HANDLE session_handle;
		//open a session
		retVal = (pFunctions->C_OpenSession)(slotIds[slotIdx], CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session_handle);
		if (retVal != CKR_OK) { continue; }

		/* Prepare some labels for particular bits of data that we want
		 * to read from the card. See sdk/pdf/beid_card_data.pdf (or
		 * .../beid_card_data.odt) for other options (e.g., address,
		 * given name, issuing municipality, ...).
		 */
		CK_CHAR_PTR pFilename = (CK_CHAR_PTR)TEXT("carddata_glob_os_version");
		CK_CHAR_PTR pSignatureFilename = (CK_CHAR_PTR)TEXT("CARD_DATA");
		CK_CHAR_PTR pLastname = (CK_CHAR_PTR)TEXT("surname");
		/*
		 * Variables in which to store the actual data
		 */
		CK_VOID_PTR pFileValue = NULL;
		CK_VOID_PTR pSignatureValue = NULL;
		CK_VOID_PTR pLastnameValue = NULL;
		CK_ULONG    FileValueLen = 0;
		CK_ULONG    SignatureValueLen = 0;
		CK_ULONG    LastnameValueLen = 0;

		//retrieve the card OS version
		retVal = Beidsdk_GetObjectValue(pFunctions, session_handle, pFilename, &pFileValue, &FileValueLen);
		if(retVal == CKR_OK)
			Beidsdk_PrintCharArray(pFilename,(CK_BYTE_PTR)pFileValue, FileValueLen);
		else
			printf("error 0x%.8x Beidsdk_GetObjectValue\n",retVal);

		//retrieve the data of the signature file
		retVal = Beidsdk_GetObjectValue(pFunctions, session_handle, pSignatureFilename, &pSignatureValue, &SignatureValueLen);
		if(retVal == CKR_OK)
			Beidsdk_PrintCharArray(pSignatureFilename,(CK_BYTE_PTR)pSignatureValue, SignatureValueLen);
		else
			printf("error 0x%.8x Beidsdk_GetObjectValue\n",retVal);

		//retrieve the lastname
		retVal = Beidsdk_GetObjectValue(pFunctions, session_handle, pLastname, &pLastnameValue, &LastnameValueLen);
		if(retVal == CKR_OK)
			Beidsdk_PrintUTF8String(pLastname,(CK_BYTE_PTR)pLastnameValue, LastnameValueLen);
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
freeslots:
	free(slotIds);
finalize:
	if (retVal == CKR_OK)
		retVal = (pFunctions->C_Finalize) (NULL_PTR);
	else
		(pFunctions->C_Finalize) (NULL_PTR);
out:
	dlclose(pkcs11Handle);
	return retVal;
} 

/* Read exactly one value from the card. */
CK_RV Beidsdk_GetObjectValue(CK_FUNCTION_LIST_PTR pFunctions, CK_SESSION_HANDLE session_handle,CK_CHAR_PTR pName,CK_VOID_PTR *ppValue, CK_ULONG_PTR pvalueLen)
{
	CK_RV retVal = CKR_OK;
	CK_ULONG data = CKO_DATA;
	CK_ATTRIBUTE searchtemplate[2] = {{CKA_CLASS,&data,sizeof(CK_ULONG)},{CKA_LABEL,(CK_VOID_PTR)pName,(CK_ULONG)strlen(pName)}};
	*pvalueLen = 0;

	//initialize the search for the objects with label <filename>
	retVal = (pFunctions->C_FindObjectsInit)(session_handle, searchtemplate, 2); 
	if (retVal != CKR_OK) { return retVal; }

	CK_OBJECT_HANDLE hObject = 0;
	CK_ULONG ulObjectCount = 0;
	//find the first object with label <filename>
	retVal = (pFunctions->C_FindObjects)(session_handle, &hObject,1,&ulObjectCount); 
	if (ulObjectCount != 1) { retVal = CKR_GENERAL_ERROR; }
	if (retVal != CKR_OK) { goto finalize; }

	/* NULL_PTR as second argument, so the length of value is filled in to
	 * retValueLen. See the definition of C_GetAttributeValue in the PKCS#11
	 * standard for more details.
	 */
	CK_ATTRIBUTE attr_templ[1] = {CKA_VALUE,NULL_PTR,0};

	//retrieve the length of the data from the object
	retVal = (pFunctions->C_GetAttributeValue)(session_handle,hObject,attr_templ,1);
	if(retVal != CKR_OK || ((CK_LONG)(attr_templ[0].ulValueLen) == -1)) { goto finalize; }

	*ppValue = malloc (attr_templ[0].ulValueLen);
	if (*ppValue == NULL) { retVal = CKR_GENERAL_ERROR; goto finalize; }

	attr_templ[0].pValue = *ppValue;
	/* now run C_GetAttributeValue a second time to actually retrieve the
	 * data from the object
	 */
	retVal = (pFunctions->C_GetAttributeValue)(session_handle,hObject,attr_templ,1);
	*pvalueLen = attr_templ[0].ulValueLen;

finalize:
	//finalize the search
	retVal = (pFunctions->C_FindObjectsFinal)(session_handle); 
	return retVal;
}

void Beidsdk_PrintCharArray(CK_CHAR_PTR pName, CK_BYTE_PTR pValue, CK_ULONG valueLen)
{
	unsigned long counter = 0;
	printf("%s:\n\n",pName);
	if(pValue != NULL)
	{
		while (counter < valueLen)
		{
			if( isprint(*(pValue+counter)) )
				printf("%c", *(pValue+counter));
			else
				printf(".");
			counter++;
		}
	}
	printf("\n");
}

void Beidsdk_PrintUTF8String(CK_CHAR_PTR pName, CK_BYTE_PTR pValue, CK_ULONG valueLen)
{
	CK_BYTE_PTR pValueNullTerminated = malloc(valueLen + 1);
	memcpy_s(pValueNullTerminated, sizeof(CK_BYTE)*(valueLen+1), pValue, sizeof(CK_BYTE)*valueLen);
	pValueNullTerminated[valueLen] = 0;

	unsigned long counter = 0;
	printf("%s:\n", pName);
	printf("%s\n", pValueNullTerminated);
	printf("\n");
}