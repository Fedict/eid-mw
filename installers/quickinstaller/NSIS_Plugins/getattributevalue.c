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

testRet test_getattributevalue(CK_ATTRIBUTE_PTR psearchtemplate, CK_ULONG templateLen) {
  void *handle;						//handle to the pkcs11 library
  CK_FUNCTION_LIST_PTR functions;		// list of the pkcs11 function pointers

  testRet retVal = {CKR_OK,TEST_PASSED};	//return values of this test
  CK_RV frv = CKR_OK;						//return value of last pkcs11 function called

  CK_SESSION_HANDLE session_handle;
  long slot_count;
  CK_SLOT_ID_PTR slotIds;
  int slotIdx;
  CK_ULONG ulObjectCount=1;

  testlog(LVL_INFO, "test_getattributevalue enter\n");
  if (InitializeTest(&handle,&functions))
  {
	frv = (*functions->C_Initialize) (NULL);
	if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Initialize", "test_getattributevalue" ))
	{		
	  frv = (*functions->C_GetSlotList) (0, 0, &slot_count);
	  if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetSlotList", "test_getattributevalue" ))
	  {
		testlog(LVL_INFO,"slot count: %i\n", slot_count);
		slotIds = malloc(slot_count * sizeof(CK_SLOT_INFO));
		if(slotIds != NULL)
		{
		  frv = (*functions->C_GetSlotList) (CK_FALSE, slotIds, &slot_count);
		  if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetSlotList (X2)", "test_getattributevalue" ))
		  {
			if(slot_count == 0)
			{
			  retVal.basetestrv = TEST_SKIPPED;
			}
			for (slotIdx = 0; slotIdx < slot_count; slotIdx++) 
			{
			  CK_OBJECT_HANDLE hObject;

			  frv = (*functions->C_OpenSession)(slotIds[slotIdx], CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session_handle);
			  if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_OpenSession", "test_getattributevalue" ))
			  {
				//CK_ATTRIBUTE attributes[1] = {CKA_CLASS,&data,sizeof(CK_ULONG)};
				CK_ATTRIBUTE_PTR attributes = psearchtemplate;
				frv = (*functions->C_FindObjectsInit)(session_handle, attributes, templateLen); 
				if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_FindObjectsInit", "test_getattributevalue" ))
				{
				  frv = (*functions->C_FindObjects)(session_handle, &hObject,1,&ulObjectCount); 
				  ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_FindObjects", "test_getattributevalue" );

				  while (ulObjectCount > 0)
				  {
					CK_ULONG retValueLen = 0;
					CK_ATTRIBUTE attr_templ[] = {{CKA_LABEL,NULL_PTR,0},
					{CKA_VALUE,NULL_PTR,0},
					{CKA_VALUE_LEN,&retValueLen,sizeof(CK_ULONG)}};
					testlog(LVL_INFO, "found object, CK_OBJECT_HANDLE = %d\n",hObject);

					frv = (*functions->C_GetAttributeValue)(session_handle,hObject,attr_templ,3);
					if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetAttributeValue", "test_getattributevalue" ))
					{
					  CK_VOID_PTR pLabel = malloc (attr_templ[0].ulValueLen);
					  if(pLabel != NULL)
					  {
						CK_VOID_PTR pValue = malloc (attr_templ[1].ulValueLen);
						attr_templ[0].pValue = pLabel;						
						if (pValue != NULL )
						{
						  attr_templ[1].pValue = pValue;
						  frv = (*functions->C_GetAttributeValue)(session_handle,hObject,attr_templ,3);
						  if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetAttributeValue X1", "test_getattributevalue" ))
						  {
							frv = (*functions->C_GetAttributeValue)(session_handle,hObject,attr_templ,3);
							if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetAttributeValue X2", "test_getattributevalue" ))
							{
							  CK_ULONG_PTR pretValueLen = attr_templ[2].pValue;
							  testlog(LVL_INFO,"key CKA_LABEL = ");
							  testlogbytes(LVL_INFO,(CK_BYTE_PTR)(attr_templ[0].pValue), attr_templ[0].ulValueLen);
							  testlog(LVL_INFO,"key CKA_VALUE = ");
							  testlogbytes(LVL_INFO,(CK_BYTE_PTR)(attr_templ[1].pValue), attr_templ[1].ulValueLen);
							  testlog(LVL_INFO,"key CKA_VALUE_LEN = %d",*pretValueLen);
							  if(*pretValueLen != attr_templ[1].ulValueLen)
							  {
								testlog(LVL_ERROR,"C_GetAttributeValue : key CKA_VALUE_LEN doesn't match CKA_VALUE .ulValueLen\n");
							  }
							}
						  }						  
						  else
						  {
							testlog(LVL_ERROR,"test_getattributevalue malloc failed\n");
							retVal.basetestrv = TEST_ERROR;
						  }
						  free (pLabel);						  
						}
						else
						{
						  testlog(LVL_ERROR,"test_getattributevalue malloc failed\n");
						  retVal.basetestrv = TEST_ERROR;
						}	
						free (pValue);
					  }
					}
					frv = (*functions->C_FindObjects)(session_handle, &hObject,1,&ulObjectCount); 
					ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_FindObjects", "test_getattributevalue" );
				  }
				  frv = (*functions->C_FindObjectsFinal)(session_handle); 
				  ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_FindObjectsFinal", "test_getattributevalue" );
				}
			  }
			  frv = (*functions->C_CloseSession) (session_handle);
			  ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_CloseSession", "test_getattributevalue" );
			}
		  }
		  free(slotIds);
		}
		else //malloc failed
		{
		  testlog(LVL_INFO,"malloc failed");
		  retVal.basetestrv = TEST_ERROR;
		}
	  }
	  frv = (*functions->C_Finalize) (NULL_PTR);
	  ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Finalize", "test_getattributevalue" );
	}
	dlclose(handle);
  }
  else
  {
	retVal.basetestrv = TEST_ERROR;
  }
  testlog(LVL_INFO, "test_getattributevalue leave\n");
  return retVal;
} 


testRet test_getattributevalue_all() {
  testRet retVal = {CKR_OK,TEST_PASSED};	//return values of this test
  CK_ULONG data = CKO_DATA;
  CK_ATTRIBUTE attributes[1] = {CKA_CLASS,&data,sizeof(CK_ULONG)};
  test_getattributevalue(attributes, 1);

  return retVal;
}

testRet test_getattributevalue_lastname() {
  testRet retVal = {CKR_OK,TEST_PASSED};	//return values of this test
  CK_ULONG data = CKO_DATA;
  CK_UTF8CHAR label[] = "Surname";
  CK_ATTRIBUTE attributes[] = {{CKA_CLASS,&data,sizeof(CK_ULONG)},
								{CKA_LABEL,label,sizeof(label)-1}};
  test_getattributevalue(attributes, 2);

  return retVal;
}