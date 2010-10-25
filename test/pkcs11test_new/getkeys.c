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
#include "basetest.h"
#include "logtest.h"


testRet test_getkeystype(CK_ULONG keytype,CK_ULONG attributetype, CK_VOID_PTR pValue, CK_ULONG ulvalueLen) {
  void *handle;						//handle to the pkcs11 library
  CK_FUNCTION_LIST_PTR functions;		// list of the pkcs11 function pointers

  testRet retVal = {CKR_OK,TEST_PASSED};	//return values of this test
  CK_RV frv = CKR_OK;						//return value of last pkcs11 function called

  CK_SESSION_HANDLE session_handle;
  long slot_count;
  CK_SLOT_ID_PTR slotIds;
  int slotIdx;
  CK_ULONG ulObjectCount = 1;

  testlog(LVL_INFO, "test_getkeystype enter\n");
  if (InitializeTest(&handle,&functions))
  {
	frv = (*functions->C_Initialize) (NULL);
	if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Initialize", "test_getkeystype" ))
	{		
	  frv = (*functions->C_GetSlotList) (0, 0, &slot_count);
	  if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetSlotList", "test_getkeystype" ))
	  {
		testlog(LVL_INFO,"slot count: %i\n", slot_count);
		slotIds = malloc(slot_count * sizeof(CK_SLOT_INFO));
		if(slotIds != NULL)
		{
		  frv = (*functions->C_GetSlotList) (CK_FALSE, slotIds, &slot_count);
		  if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetSlotList (X2)", "test_getkeystype" ))
		  {
			if(slot_count == 0)
			{
			  retVal.basetestrv = TEST_SKIPPED;
			}
			for (slotIdx = 0; slotIdx < slot_count; slotIdx++) 
			{
			  CK_ULONG key = keytype;
			  CK_ATTRIBUTE attributes[] = {CKA_CLASS,&key,sizeof(CK_ULONG)};
			  CK_OBJECT_HANDLE hKey;

			  frv = (*functions->C_OpenSession)(slotIds[slotIdx], CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session_handle);
			  if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_OpenSession", "test_getkeystype" ))
			  {
				frv = (*functions->C_FindObjectsInit)(session_handle, attributes, 1); 
				if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_FindObjectsInit", "test_getkeystype" ))
				{
				  frv = (*functions->C_FindObjects)(session_handle, &hKey,1,&ulObjectCount); 
				  ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_FindObjects", "test_getkeystype" ); 
				  while (ulObjectCount > 0)
				  {
					CK_ATTRIBUTE attr_label_templ[] = {	attributetype,pValue,ulvalueLen};
					testlog(LVL_INFO, "found key, CK_OBJECT_HANDLE = %d\n",hKey);
					frv = (*functions->C_GetAttributeValue)(session_handle,hKey,attr_label_templ,1);
					if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetAttributeValue", "test_getkeystype" ))
					{
					  if( (attr_label_templ[0].pValue == NULL) && (attr_label_templ[0].ulValueLen != 0) )
					  {
						attr_label_templ[0].pValue = malloc(attr_label_templ[0].ulValueLen);
						if(attr_label_templ[0].pValue != NULL)
						{
						  frv = (*functions->C_GetAttributeValue)(session_handle,hKey,attr_label_templ,1);
						  if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetAttributeValue", "test_getkeystype" ))
						  {
							testlogbytes(LVL_INFO,(CK_BYTE_PTR)(attr_label_templ[0].pValue), attr_label_templ[0].ulValueLen);
						  }
						  free(attr_label_templ[0].pValue);
						}
						else
						{
						  testlog(LVL_INFO,"test_getkeystype malloc failed\n");
						  retVal.basetestrv = TEST_FAILED;
						}
					  }
					  else
					  {
						testlogbytes(LVL_INFO,(CK_BYTE_PTR)(attr_label_templ[0].pValue), attr_label_templ[0].ulValueLen);
					  }
					}
					else if (attr_label_templ[0].ulValueLen != -1)
					{//in case of an error, attr_label_templ[0].ulValueLen shoulod be set to -1
					  retVal.basetestrv = TEST_ERROR;
					  testlog(LVL_ERROR,"C_GetAttributeValue returned an error, but ulValueLen was %d (not -1)\n",attr_label_templ[0].ulValueLen);
					}
					frv = (*functions->C_FindObjects)(session_handle, &hKey,1,&ulObjectCount); 
					ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_FindObjects", "test_getkeystype" );
				  }
				  frv = (*functions->C_FindObjectsFinal)(session_handle); 
				  ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_FindObjectsFinal", "test_getkeystype" );
				}
			  }
			  frv = (*functions->C_CloseSession) (session_handle);
			  ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_CloseSession", "test_getkeystype" );
			}
		  }
		  free (slotIds);
		}
		else //malloc failed
		{
		  testlog(LVL_INFO,"malloc failed");
		  retVal.basetestrv = TEST_ERROR;
		}
	  }
	  frv = (*functions->C_Finalize) (NULL_PTR);
	  ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Finalize", "test_getkeystype" );
	}
	dlclose(handle);
  }
  else
  {
	retVal.basetestrv = TEST_ERROR;
  }
  testlog(LVL_INFO, "test_getkeystype leave\n");
  return retVal;
} 


testRet test_getkeys_retvals() {
  testRet retVal = {CKR_OK,TEST_PASSED};
  CK_VOID_PTR pValue = malloc(14);
  if(pValue != NULL)
  {
	CK_ULONG ulvalueLen = 14;
	retVal = test_getkeystype(CKO_PRIVATE_KEY, CKA_LABEL, pValue, ulvalueLen);
	//key names are "authentication" and "signature", so 14 bytes should be sufficient
	if(( retVal.pkcs11rv == CKR_OK) && (retVal.basetestrv == TEST_PASSED))
	{
	  CK_BYTE shortvalue[5];
	  CK_ULONG ulshortvalueLen = 5;
	  retVal = test_getkeystype(CKO_PUBLIC_KEY, CKA_LABEL, shortvalue, ulshortvalueLen);
	  //our buffer of 5 is too small to hold the public key names, so we expect a CKR_BUFFER_TOO_SMALL
	  //as response here
	  if( retVal.pkcs11rv == CKR_BUFFER_TOO_SMALL)
	  {
		testlog(LVL_ERROR, "error 0x%.8x is wanted\n",CKR_BUFFER_TOO_SMALL);
		retVal.pkcs11rv = CKR_OK;
		retVal = test_getkeystype(CKO_CERTIFICATE, CKA_APPLICATION, NULL_PTR, 0);
		if(( retVal.pkcs11rv == CKR_ATTRIBUTE_TYPE_INVALID) && (retVal.basetestrv == TEST_PASSED))
		{
		  testlog(LVL_ERROR, "error 0x%.8x is wanted\n",CKR_ATTRIBUTE_TYPE_INVALID);
		  retVal.pkcs11rv = CKR_OK;
		}
	  }
	  else
	  {
		retVal.basetestrv = TEST_ERROR;
	  }
	}
	free(pValue);
  }
  else
  {
	testlog(LVL_ERROR, "test_getkeys_retvals malloc failed\n");
	retVal.basetestrv = TEST_ERROR;
  }
  return retVal;
}

testRet test_getkeys() {
  testRet retVal = {CKR_OK,TEST_PASSED};

  retVal = test_getkeystype(CKO_PRIVATE_KEY, CKA_LABEL, NULL_PTR, 0);
  if(( retVal.pkcs11rv == CKR_OK) && (retVal.basetestrv == TEST_PASSED))
  {
	retVal = test_getkeystype(CKO_PUBLIC_KEY, CKA_LABEL, NULL_PTR, 0);
	if(( retVal.pkcs11rv == CKR_OK) && (retVal.basetestrv == TEST_PASSED))
	{
	  retVal = test_getkeystype(CKO_CERTIFICATE, CKA_LABEL, NULL_PTR, 0);
	  if(( retVal.pkcs11rv == CKR_OK) && (retVal.basetestrv == TEST_PASSED))
	  {
	  }
	}
  }
  return retVal;
}
