/* ****************************************************************************

* eID Middleware Project.
* Copyright (C) 2008-2012 FedICT.
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

testRet test_getallobjects() 
{
	void *handle;						//handle to the pkcs11 library
	CK_FUNCTION_LIST_PTR functions;		// list of the pkcs11 function pointers

	testRet retVal = {CKR_OK,TEST_PASSED};	//return values of this test
	CK_RV frv = CKR_OK;						//return value of last pkcs11 function called

	CK_SESSION_HANDLE session_handle;
	CK_ULONG slot_count;
	CK_SLOT_ID_PTR slotIds;
	CK_ULONG slotIdx;
	CK_ULONG ulObjectCount=1;

	testlog(LVL_INFO, "test_getallobjects enter\n");
	if (InitializeTest(&handle,&functions))
	{
		frv = (*functions->C_Initialize) (NULL);
		if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Initialize", "test_getallobjects" ))
		{		
			frv = (*functions->C_GetSlotList) (CK_TRUE, 0, &slot_count);
			if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetSlotList", "test_getallobjects" ))
			{
				testlog(LVL_INFO,"slot count: %i\n", slot_count);
				slotIds = (CK_SLOT_ID_PTR) malloc(slot_count * sizeof(CK_SLOT_INFO));
				if(slotIds != NULL)
				{
					frv = (*functions->C_GetSlotList) (CK_TRUE, slotIds, &slot_count);
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
							CK_BYTE_PTR pByte;
							CK_OBJECT_CLASS value;
							CK_ULONG ulvalueLen = sizeof(CK_ULONG);//last one is for the string termination
							CK_ATTRIBUTE attr_templ[2] = {{CKA_LABEL,pLabel,ullabelLen},{CKA_CLASS,&value,ulvalueLen}};
							CK_OBJECT_HANDLE hObject;

							frv = (*functions->C_OpenSession)(slotId, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session_handle);
							if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_OpenSession", "test_getallobjects" ))
							{
								frv = (*functions->C_FindObjectsInit)(session_handle, attributes, 0); 
								if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_FindObjectsInit", "test_getallobjects" ))
								{
									while (ulObjectCount > 0)
									{
										frv = (*functions->C_FindObjects)(session_handle, &hObject,1,&ulObjectCount); 
										ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_FindObjects", "test_getallobjects" );
										if (ulObjectCount > 0)
										{
											testlog(LVL_INFO, "found object, CK_OBJECT_HANDLE = %d\n",hObject);
											frv = (*functions->C_GetAttributeValue)(session_handle,hObject,attr_templ,2);
											if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetAttributeValue", "test_getallobjects" ))
											{
												pByte = attr_templ[0].pValue;
												pByte[attr_templ[0].ulValueLen] = 0;
												testlog(LVL_INFO,"key LABEL value = %s\n", pByte);
												testlog(LVL_INFO,"key CKA_CLASS value = %d\n", value);
												//reset buffer length to allocated memory value
												attr_templ[0].ulValueLen = 255;
											}
										}
									}
									frv = (*functions->C_FindObjectsFinal)(session_handle); 
									ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_FindObjectsFinal", "test_getallobjects" );
								}
								frv = (*functions->C_CloseSession) (session_handle);
								ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_CloseSession", "test_getallobjects" );
							}
						}
					}
					free(slotIds);
				}
				else
				{
					testlog(LVL_ERROR,"malloc failed\n");
				}
			}//getslotlist
			frv = (*functions->C_Finalize) (NULL_PTR);
			ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Finalize", "test_getallobjects" );
		}
		dlclose(handle);
		testlog(LVL_INFO, "test_getallobjects leave\n");
	}
	return retVal;
} 


testRet test_getallidobjects() {
	void *handle = NULL;				//handle to the pkcs11 library
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
			frv = (*functions->C_GetSlotList) (CK_TRUE, 0, &slot_count);
			if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetSlotList", "test_getallobjects" ))
			{
				testlog(LVL_INFO,"slot count: %i\n", slot_count);
				slotIds = (CK_SLOT_ID_PTR) malloc(slot_count * sizeof(CK_SLOT_INFO));
				frv = (*functions->C_GetSlotList) (CK_TRUE, slotIds, &slot_count);
				if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetSlotList (X2)", "test_getallobjects" ))
				{
					for (slotIdx = 0; slotIdx < slot_count; slotIdx++) 
					{
						CK_SLOT_ID slotId = slotIds[slotIdx];
						CK_ULONG data = CKO_DATA;
						CK_UTF8CHAR* ptheLabel = (CK_UTF8CHAR*)"surname";//id
						CK_ATTRIBUTE attributes[2] = {{CKA_CLASS,&data,sizeof(CK_ULONG)},{CKA_LABEL,ptheLabel,strlen((const char*)ptheLabel)}};//surname CKA_OBJECT_ID
						CK_VOID_PTR pLabel = malloc (256);
						CK_ULONG ullabelLen = 255;//last one is for the string termination
						CK_BYTE_PTR pByte;
						CK_OBJECT_CLASS value;
						CK_ULONG ulvalueLen = sizeof(CK_ULONG);//last one is for the string termination
						CK_VOID_PTR pObjectID = malloc (256);
						CK_ULONG ulpObjectID = 255;//last one is for the string termination
						CK_VOID_PTR pObjectValue = malloc (256);
						CK_ULONG ulObjectValueLen = 255;//last one is for the string termination

						CK_ATTRIBUTE attr_templ[4] = {{CKA_LABEL,pLabel,ullabelLen},{CKA_CLASS,&value,ulvalueLen},
						{CKA_OBJECT_ID,pObjectID,ulpObjectID},{CKA_VALUE,pObjectValue,ulObjectValueLen} };
						CK_OBJECT_HANDLE hObject;

						frv = (*functions->C_OpenSession)(slotId, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session_handle);
						if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_OpenSession", "test_getallobjects" ))
						{
							frv = (*functions->C_FindObjectsInit)(session_handle, attributes, 2); 
							if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_FindObjectsInit", "test_getallobjects" ))
							{
								while (ulObjectCount > 0)
								{
									frv = (*functions->C_FindObjects)(session_handle, &hObject,1,&ulObjectCount); 
									ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_FindObjects", "test_getallobjects" );
									if (ulObjectCount > 0)
									{
										testlog(LVL_INFO, "found object, CK_OBJECT_HANDLE = %d\n",hObject);
										frv = (*functions->C_GetAttributeValue)(session_handle,hObject,attr_templ,4);
										if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetAttributeValue", "test_getallobjects" ))
										{
											pByte = (CK_BYTE_PTR) attr_templ[0].pValue;
											pByte[attr_templ[0].ulValueLen] = 0;
											testlog(LVL_INFO,"key LABEL value = %s\n", pByte);
											testlog(LVL_INFO,"key CKA_CLASS value = %d\n", value);
											pByte = (CK_BYTE_PTR) attr_templ[2].pValue;
											pByte[attr_templ[2].ulValueLen] = 0;
											testlog(LVL_INFO,"key LABEL value = %s\n", pByte);
											pByte = (CK_BYTE_PTR) attr_templ[3].pValue;
											pByte[attr_templ[3].ulValueLen] = 0;
											testlog(LVL_INFO,"key LABEL value = %s\n", pByte);
											//reset buffer length to allocated memory value
											attr_templ[0].ulValueLen = 255;
											attr_templ[2].ulValueLen = 255;
											attr_templ[3].ulValueLen = 255;
										}
									}
								}
								frv = (*functions->C_FindObjectsFinal)(session_handle); 
								ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_FindObjectsFinal", "test_getallobjects" );
							}

							ulObjectCount=1;
							attributes[1].pValue = "address_zip";
							attributes[1].ulValueLen = strlen("address_zip");
							frv = (*functions->C_FindObjectsInit)(session_handle, attributes, 2); 
							if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_FindObjectsInit", "test_getallobjects" ))
							{
								while (ulObjectCount > 0)
								{
									frv = (*functions->C_FindObjects)(session_handle, &hObject,1,&ulObjectCount); 
									ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_FindObjects", "test_getallobjects" );
									if (ulObjectCount > 0)
									{
										testlog(LVL_INFO, "found object, CK_OBJECT_HANDLE = %d\n",hObject);
										frv = (*functions->C_GetAttributeValue)(session_handle,hObject,attr_templ,4);
										if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetAttributeValue", "test_getallobjects" ))
										{
											pByte = (CK_BYTE_PTR)attr_templ[0].pValue;
											pByte[attr_templ[0].ulValueLen] = 0;
											testlog(LVL_INFO,"key LABEL value = %s\n", pByte);
											testlog(LVL_INFO,"key CKA_CLASS value = %d\n", value);
											pByte = (CK_BYTE_PTR)attr_templ[2].pValue;
											pByte[attr_templ[2].ulValueLen] = 0;
											testlog(LVL_INFO,"key LABEL value = %s\n", pByte);
											pByte = (CK_BYTE_PTR)attr_templ[3].pValue;
											pByte[attr_templ[3].ulValueLen] = 0;
											testlog(LVL_INFO,"key LABEL value = %s\n", pByte);
											//reset buffer length to allocated memory value
											attr_templ[0].ulValueLen = 255;
										}
									}
								}
								frv = (*functions->C_FindObjectsFinal)(session_handle); 
								ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_FindObjectsFinal", "test_getallobjects" );
							}
						}
						frv = (*functions->C_CloseSession) (session_handle);
						ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_CloseSession", "test_getallobjects" );

						if(pLabel != NULL)
							free(pLabel);
						if(pObjectID != NULL)
							free(pObjectID);
						if(pObjectValue != NULL)
							free(pObjectValue);
					}
				}
			}
			frv = (*functions->C_Finalize) (NULL_PTR);
			ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Finalize", "test_getallobjects" );
		}
		dlclose(handle);
		testlog(LVL_INFO, "test_getallobjects leave\n");
	}
	return retVal;
} 
