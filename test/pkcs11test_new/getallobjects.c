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

testRet test_getallobjects() {
	void *handle;						//handle to the pkcs11 library
	CK_FUNCTION_LIST_PTR functions;		// list of the pkcs11 function pointers

	testRet retVal = {CKR_OK,TEST_PASSED};	//return values of this test
	CK_RV frv = CKR_OK;						//return value of last pkcs11 function called

	CK_SESSION_HANDLE session_handle;
	long slot_count;
	CK_SLOT_ID_PTR slotIds;
	int slotIdx;
	CK_ULONG ulObjectCount=1;

	testlog(LVL_INFO, "test_getallobjects enter\n");
	if (InitializeTest(&handle,&functions))
	{
		frv = (*functions->C_Initialize) (NULL);
		if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Initialize", "test_getallobjects" ))
		{		
			frv = (*functions->C_GetSlotList) (0, 0, &slot_count);
			if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetSlotList", "test_getallobjects" ))
			{
				testlog(LVL_INFO,"slot count: %i\n", slot_count);
				slotIds = malloc(slot_count * sizeof(CK_SLOT_INFO));
				if(slotIds != NULL)
				{
					frv = (*functions->C_GetSlotList) (CK_FALSE, slotIds, &slot_count);
					if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetSlotList (X2)", "test_getallobjects" ))
					{
						if(slot_count == 0)
						{
							retVal.basetestrv = TEST_SKIPPED;
						}
						for (slotIdx = 0; slotIdx < slot_count; slotIdx++) 
						{
							CK_SLOT_ID slotId = slotIds[slotIdx];
							CK_ULONG data = CKO_DATA;
							CK_ATTRIBUTE attributes[1] = {CKA_CLASS,&data,sizeof(CK_ULONG)};
							CK_VOID_PTR pLabel = malloc (256);
							CK_ULONG ullabelLen = 255;//last one is for the string termination
							//CK_BYTE_PTR pByte;
							CK_OBJECT_CLASS value;
							CK_ULONG ulvalueLen = sizeof(CK_ULONG);//last one is for the string termination
							CK_ATTRIBUTE attr_templ[2] = {{CKA_LABEL,pLabel,ullabelLen},{CKA_CLASS,&value,ulvalueLen}};
							CK_OBJECT_HANDLE hObject;

							if(pLabel != NULL)
							{
								frv = (*functions->C_OpenSession)(slotId, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session_handle);
								if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_OpenSession", "test_getallobjects" ))
								{
									frv = (*functions->C_FindObjectsInit)(session_handle, attributes, 0); 
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
							else //malloc failed
							{
								testlog(LVL_INFO,"malloc failed");
								retVal.basetestrv = TEST_ERROR;
							}
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
			ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Finalize", "test_getallobjects" );
		}
		dlclose(handle);
	}
	else
	{
		retVal.basetestrv = TEST_ERROR;
	}
	testlog(LVL_INFO, "test_getallobjects leave\n");
	return retVal;
} 
