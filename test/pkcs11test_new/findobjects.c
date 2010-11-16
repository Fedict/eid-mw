/* ****************************************************************************

* eID Middleware Project.
* Copyright (C) 2009-2010 FedICT.
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


testRet test_findPrivateKeyWithoutLoginShouldFail() 
{
	void *handle = NULL;				//handle to the pkcs11 library
	CK_FUNCTION_LIST_PTR functions;		// list of the pkcs11 function pointers

	testRet retVal = {CKR_OK,TEST_PASSED};	//return values of this test
	CK_RV frv = CKR_OK;						//return value of last pkcs11 function called

	CK_ULONG ulCount = 0;
	CK_ULONG slotIdx = 0;
	CK_ULONG ulCounter = 10;
	CK_SLOT_ID_PTR slotIds = NULL;
	CK_SESSION_HANDLE session;

	testlog(LVL_INFO, "test_findPrivateKeyWithoutLoginShouldFail enter\n");

	retVal = PrepareSlotListTest(&handle,&functions, &slotIds, &ulCount,CK_TRUE );
	if((retVal.pkcs11rv == CKR_OK) && (retVal.basetestrv == TEST_PASSED))
	{
		for (slotIdx = 0; slotIdx < ulCount; slotIdx++) 
		{
			if (frv == CKR_OK)
			{
				frv = (*functions->C_OpenSession)(slotIds[slotIdx], CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session);
				if (ReturnedSucces(frv,&(retVal.pkcs11rv), "C_OpenSession"))
				{
					CK_ULONG type = CKA_CLASS;
					CK_ULONG value = CKO_PRIVATE_KEY;
					CK_ULONG valueLen = sizeof(CK_ULONG);
					CK_ATTRIBUTE attributes[1] = {{type,(CK_VOID_PTR)&value, valueLen}};

					frv = (*functions->C_FindObjectsInit)(session, attributes, 1);
					if (ReturnedSucces(frv,&(retVal.pkcs11rv), "C_FindObjectsInit"))
					{
						CK_OBJECT_HANDLE hObject = 0;
						CK_ULONG maxObjCount = 1;
						CK_ULONG objCount = 0;
						frv = (*functions->C_FindObjects)(session, &hObject,maxObjCount, &objCount);
						if (ReturnedSucces(frv,&(retVal.pkcs11rv), "C_FindObjects"))
						{
							if(objCount > 0)
							{
								testlog(LVL_ERROR, "a CKO_PRIVATE_KEY object was found without logging in\n");
								retVal.basetestrv = TEST_FAILED;
							}
						}
						if (frv == CKR_OK)
						{
							frv = (*functions->C_FindObjectsFinal)(session);
							ReturnedSucces(frv,&(retVal.pkcs11rv), "C_FindObjectsFinal");
						}
						else
							(*functions->C_FindObjectsFinal)(session);
					}
					if (frv == CKR_OK)
					{
						frv = (*functions->C_CloseSession) (session);
						ReturnedSucces(frv,&(retVal.pkcs11rv), "C_FindObjectsFinal");
					}
					else
					{
						ReturnedSucces(frv,&(retVal.pkcs11rv), "C_CloseSession");
					}
				}
			}
		}//end of for loop
	}

	frv = (*functions->C_Finalize) (NULL_PTR);
	ReturnedSucces(frv,&(retVal.pkcs11rv), "C_Finalize");

	EndSlotListTest(handle,slotIds );

	testlog(LVL_INFO, "test_findPrivateKeyWithoutLoginShouldFail leave\n");
	return retVal;
}


/*
testRet test_findobjects() {

CK_ULONG data = CKO_DATA;
CK_ATTRIBUTE attributes[1] = {CKA_CLASS,&data,sizeof(CK_ULONG)};
CK_VOID_PTR pLabel = malloc (256);
CK_ULONG ullabelLen = 255;//last one is for the string termination
//CK_BYTE_PTR pByte;
CK_OBJECT_CLASS value;
CK_ULONG ulvalueLen = sizeof(CK_ULONG);//last one is for the string termination
CK_ATTRIBUTE attr_templ[2] = {{CKA_LABEL,pLabel,ullabelLen},{CKA_CLASS,&value,ulvalueLen}};

}

testRet test_findobjectwithTemplate(CK_ATTRIBUTE *pobjectTempl,CK_ATTRIBUTE *pattrTempl, ) {
void *handle = NULL;				//handle to the pkcs11 library
CK_FUNCTION_LIST_PTR functions;		// list of the pkcs11 function pointers

testRet retVal = {CKR_OK,TEST_PASSED};	//return values of this test
CK_RV frv = CKR_OK;						//return value of last pkcs11 function called

CK_ULONG ulCount = 0;
CK_ULONG slotIdx = 0;
CK_SLOT_ID_PTR slotIds = NULL;
CK_SESSION_HANDLE session_handle;

testlog(LVL_INFO, "test_open_close_session enter\n");

retVal = PrepareSlotListTest(&handle,&functions, &slotIds, &ulCount,CK_TRUE );
if((retVal.pkcs11rv == CKR_OK) && (retVal.basetestrv == TEST_PASSED))
{
for (slotIdx = 0; slotIdx < ulCount; slotIdx++) 
{
frv = (*functions->C_OpenSession)(slotIds[slotIdx], CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session_handle);
if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_OpenSession", "test_open_close_session enter" ))
{
CK_OBJECT_HANDLE hObject;

frv = (*functions->C_FindObjectsInit)(session_handle, pobjectTempl, 0); 
if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_FindObjectsInit", "test_getallobjects" ))
{
frv = (*functions->C_FindObjects)(session_handle, &hObject,1,&ulObjectCount); 
ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_FindObjects", "test_getallobjects" );

while (ulObjectCount > 0)
{
testlog(LVL_INFO, "found object, CK_OBJECT_HANDLE = %d\n",hObject);
frv = (*functions->C_GetAttributeValue)(session_handle,hObject,attr_templ,2);
if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetAttributeValue", "test_getallobjects" ))
{
testlog(LVL_INFO,"key LABEL value = ");
testlogbytes(LVL_INFO,(CK_BYTE_PTR)(attr_templ[0].pValue), attr_templ[0].ulValueLen);
testlog(LVL_INFO,"key CKA_CLASS value = %d\n", value);
attr_templ[0].ulValueLen = ullabelLen;
attr_templ[1].ulValueLen = ulvalueLen;
}
frv = (*functions->C_FindObjects)(session_handle, &hObject,1,&ulObjectCount); 
ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_FindObjects", "test_getallobjects" );
}
frv = (*functions->C_FindObjectsFinal)(session_handle); 
ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_FindObjectsFinal", "test_getallobjects" );
}
}
frv = (*functions->C_CloseSession) (session_handle);
ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_CloseSession", "test_getallobjects" );
}

frv = (*functions->C_Finalize) (NULL_PTR);
ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Finalize", "test_open_close_session enter" );
}
EndSlotListTest(handle,slotIds );

testlog(LVL_INFO, "test_open_close_session leave\n");
return retVal;
}
*/

