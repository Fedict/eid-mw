/* ****************************************************************************

* eID Middleware Project.
* Copyright (C) 2009-2012 FedICT.
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
#include "getmechanisms.h"

testRet test_sign_mech(CK_MECHANISM_TYPE mechanismType) {
	void *handle;						//handle to the pkcs11 library
	CK_FUNCTION_LIST_PTR functions;		// list of the pkcs11 function pointers

	testRet retVal = {CKR_OK,TEST_PASSED};	//return values of this test
	CK_RV frv = CKR_OK;						//return value of last pkcs11 function called

	CK_SESSION_HANDLE session_handle;
	CK_ULONG slot_count;
	CK_SLOT_ID_PTR slotIds;
	int slotIdx;
	CK_ULONG ulObjectCount;

	testlog(LVL_INFO, "test_sign enter\n");
	if (InitializeTest(&handle,&functions))
	{
		frv = (*functions->C_Initialize) (NULL);
		if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Initialize", "test_sign" ))
		{		
			frv = (*functions->C_GetSlotList) (CK_TRUE, 0, &slot_count);
			if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetSlotList", "test_sign" ))
			{
				testlog(LVL_INFO,"slot count: %i\n", slot_count);
				slotIds = malloc(slot_count * sizeof(CK_SLOT_INFO));
				if(slotIds != NULL)
				{
					frv = (*functions->C_GetSlotList) (CK_TRUE, slotIds, &slot_count);
					if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetSlotList (X2)", "test_sign" ))
					{
						if(slot_count == 0)
						{
							retVal.basetestrv = TEST_SKIPPED;
						}
						for (slotIdx = 0; slotIdx < slot_count; slotIdx++) 
						{
							CK_SLOT_ID slotId = slotIds[slotIdx];
							CK_ULONG private_key = CKO_PRIVATE_KEY;
							CK_ATTRIBUTE attributes[2] = {	{CKA_CLASS,&private_key,sizeof(CK_ULONG)},
							{CKA_LABEL,"Signature",(CK_ULONG) strlen("Signature")}};
							CK_OBJECT_HANDLE hKey;
							CK_MECHANISM mechanism = {mechanismType, NULL_PTR, 0};
							CK_BYTE_PTR data = "testsignthis"; 
							CK_BYTE signature[256];
							CK_ULONG signLength = 256;
							CK_BYTE signatureMultiPart[256];
							CK_ULONG signMultiPartLength = 256;
							CK_ULONG lengthNeeded = 0;

							frv = (*functions->C_OpenSession)(slotId, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session_handle);
							if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_OpenSession", "test_sign" ))
							{
								frv = (*functions->C_FindObjectsInit)(session_handle, attributes, 2); 
								if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_FindObjectsInit", "test_sign" ))
								{
									frv = (*functions->C_FindObjects)(session_handle, &hKey,1,&ulObjectCount); 
									if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_FindObjects", "test_sign" ))
									{
										frv = (*functions->C_FindObjectsFinal)(session_handle); 
										if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_FindObjectsFinal", "test_sign" ))
										{
											//sign in a single part
											frv = (*functions->C_SignInit)(session_handle, &mechanism, hKey); 
											if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_SignInit", "test_sign" ))
											{
												frv = (*functions->C_Sign)(session_handle,data,(CK_ULONG) strlen(data),signature,&signLength);
												ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Sign", "test_sign" );
											}
											//sign in multiple parts
											frv = (*functions->C_SignInit)(session_handle, &mechanism, hKey); 
											if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_SignInit", "test_sign" ))
											{
												frv = (*functions->C_SignUpdate)(session_handle,data,4);
												if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Sign", "test_sign" ))
												{
													data += 4;
													frv = (*functions->C_SignUpdate)(session_handle,data,4);
													if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Sign", "test_sign" ))
													{
														data += 4;
														frv = (*functions->C_SignUpdate)(session_handle,data,4);
														if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Sign", "test_sign" ))
														{
															frv = (*functions->C_SignFinal)(session_handle,NULL,&lengthNeeded);
															if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Sign", "test_sign" ))
															{
																frv = (*functions->C_SignFinal)(session_handle,signatureMultiPart,&signMultiPartLength);
																ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Sign", "test_sign" );
																if(memcmp(signature,signatureMultiPart,signLength) != 0)
																{
																	testlog(LVL_ERROR, "test_sign single_part and multi_part give different results\n");
																	testlog(LVL_ERROR, "signature length single_part = %d\n signature = ",signLength);
																	testlogbytes(LVL_ERROR,signature, signLength);
																	testlog(LVL_ERROR, "\nsignature length multi_part = %d\n signature = ",signLength);
																	testlogbytes(LVL_ERROR,signatureMultiPart, signMultiPartLength);
																	testlog(LVL_ERROR, "\n");
																}
															}
														}
													}
												}
											}
										}
									}
									else
									{
										frv = (*functions->C_FindObjectsFinal)(session_handle); 
										ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_FindObjectsFinal", "test_sign");
									}
								}
								frv = (*functions->C_CloseSession) (session_handle);
								ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_CloseSession", "test_sign" );
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
			ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Finalize", "test_sign" );
		}
		dlclose(handle);
	}
	else
	{
		retVal.basetestrv = TEST_ERROR;
	}

	testlog(LVL_INFO, "test_sign leave\n");
	return retVal;
} 

testRet test_sign(void)
{
	testRet testret;
	
	//testret = test_sign_mech(CKM_SHA384);//invalid

	testret = test_sign_mech(CKM_RSA_PKCS);//CKM_SHA384_RSA_PKCS);
	
	testlog(LVL_ERROR, "test_sign single_part and multi_part give different results\n");

	//testret = test_sign_mech(CKM_SHA1_RSA_PKCS_PSS);
	

	return testret;
}

testRet test_sign_allmechs(void)
{
	testRet testret;
	CK_ULONG ulRetMechCount = 0;
	CK_MECHANISM_TYPE_PTR pRetMechanismList = NULL;

	testret = test_returnmechanisms(&ulRetMechCount,pRetMechanismList);

	pRetMechanismList = (CK_MECHANISM_TYPE_PTR) malloc (ulRetMechCount * sizeof(CK_MECHANISM_TYPE));
	if(pRetMechanismList == NULL)
	{
			testlog(LVL_ERROR, "test_sign_allmechs: malloc failed \n");
			testret.basetestrv = TEST_SKIPPED;
			return testret;
	}
	testret = test_returnmechanisms(&ulRetMechCount,pRetMechanismList);

	if(testret.pkcs11rv == CKR_OK)
	{
		CK_ULONG ulCounter = 0;
		while (ulCounter < ulRetMechCount)
		{
			testlog(LVL_INFO, "test_sign with mechanism 0x%.8x\n",pRetMechanismList[ulCounter]);
			testret = test_sign_mech(pRetMechanismList[ulCounter]);
			ulCounter++;
		}
	}

	return testret;
}
