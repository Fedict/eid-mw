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


testRet test_getmechanisms() {
	void *handle;						//handle to the pkcs11 library
	CK_FUNCTION_LIST_PTR functions;		// list of the pkcs11 function pointers

	testRet retVal = {CKR_OK,TEST_PASSED};	//return values of this test
	CK_RV frv = CKR_OK;						//return value of last pkcs11 function called

	CK_INFO info;
	CK_TOKEN_INFO tokenInfo;
	CK_SESSION_HANDLE session_handle;
	long slot_count;
	CK_SLOT_ID_PTR slotIds;
	int slotIdx;
	CK_ULONG ulMechCount;
	unsigned long ulCount;
	CK_MECHANISM_TYPE_PTR pMechanismList;

	testlog(LVL_INFO, "test_show_mechanismsinfo enter\n");
	if (InitializeTest(&handle,&functions))
	{
		frv = (*functions->C_Initialize) (NULL);
		if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Initialize", "test_show_mechanismsinfo" ))
		{	
			frv = (*functions->C_GetInfo) (&info);
			if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetInfo", "test_show_mechanismsinfo" ))
			{
				testlog(LVL_INFO,"library version: %d.%d\n", info.libraryVersion.major, info.libraryVersion.minor);
				testlog(LVL_INFO,"PKCS#11 version: %d.%d\n", info.cryptokiVersion.major, info.cryptokiVersion.minor);
				frv = (*functions->C_GetSlotList) (0, 0, &slot_count);
				if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetSlotList", "test_show_mechanismsinfo" ))
				{
					testlog(LVL_INFO,"slot count: %i\n", slot_count);
					slotIds = malloc(slot_count * sizeof(CK_SLOT_INFO));
					if(slotIds != NULL)
					{
						frv = (*functions->C_GetSlotList) (CK_FALSE, slotIds, &slot_count);
						if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetSlotList (X2)", "test_show_mechanismsinfo" ))
						{
							if(slot_count == 0)
							{
								retVal.basetestrv = TEST_SKIPPED;
							}
							for (slotIdx = 0; slotIdx < slot_count; slotIdx++) 
							{
								CK_SLOT_INFO slotInfo;		
								CK_SLOT_ID slotId = slotIds[slotIdx];
								int idx;
								// C_GetSlotInfo
								frv = (*functions->C_GetSlotInfo) (slotId, &slotInfo);
								if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetSlotInfo", "test_show_mechanismsinfo" ))
								{
									for (idx = 64 - 1; idx > 0; idx--) {
										if (slotInfo.slotDescription[idx] == ' ') {
											slotInfo.slotDescription[idx] = '\0';			
										} else {
											break;
										}		
									}
									testlog(LVL_INFO,"slot Id: %d\n", slotId);
									testlog(LVL_INFO,"slot description: %s\n", slotInfo.slotDescription);

									// C_OpenSession
									frv = (*functions->C_OpenSession)(slotId, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session_handle);
									if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_OpenSession", "test_show_mechanismsinfo" ))
									{

										//C_GetTokenInfo
										/* Get token information for first slot */
										frv = (*functions->C_GetTokenInfo)(slotId, &tokenInfo);
										if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetTokenInfo", "test_show_mechanismsinfo" ))
										{
											testlog(LVL_INFO,"C_GetTokenInfo :\n ");
											testlog(LVL_DEBUG,"flags : %x \n ", tokenInfo.flags);
											testlog(LVL_DEBUG,"label : %s \n ", tokenInfo.label);
											testlog(LVL_DEBUG,"serialNumber : %s \n ", tokenInfo.serialNumber);

											// C_GetMechanismList
											frv = (*functions->C_GetMechanismList)(slotId, NULL_PTR, &ulMechCount);
											if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetMechanismList", "test_show_mechanismsinfo" ))
											{
												if (ulMechCount > 0) 
												{
													pMechanismList = (CK_MECHANISM_TYPE_PTR)malloc(ulMechCount*sizeof(CK_MECHANISM_TYPE));
													if(pMechanismList != NULL)
													{
														frv = (*functions->C_GetMechanismList)(slotId, pMechanismList,&ulMechCount);
														if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetMechanismList X2", "test_show_mechanismsinfo" ))
														{
															testlog(LVL_INFO,"C_GetMechanismList :\n ");
															if(ulMechCount == 0)
															{
																testlog(LVL_ERROR,"No mechanisms found\n ");
															}
															ulCount = 0;
															for ( ;ulCount < ulMechCount;ulCount++)
															{
																testlog(LVL_DEBUG,"Mechanism%d : 0x%.8x \n ",ulCount, pMechanismList[ulCount]);
															}
														}
													}
													else //malloc failed
													{
														testlog(LVL_INFO,"malloc failed");
														retVal.basetestrv = TEST_ERROR;
													}
												}
											}
										}

										// C_CloseSession
										frv = (*functions->C_CloseSession) (session_handle);
										ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_CloseSession", "test_show_mechanismsinfo" );
									}
								}
							}
						}
					}
					else //malloc failed
					{
						testlog(LVL_INFO,"malloc failed");
						retVal.basetestrv = TEST_ERROR;
					}
				}
			}
			// C_Finalize
			frv = (*functions->C_Finalize) (NULL_PTR);
			ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Finalize", "test_show_mechanismsinfo" );
		}
		dlclose(handle);
	}
	else
	{
		retVal.basetestrv = TEST_ERROR;
	}
	testlog(LVL_INFO, "test_show_mechanismsinfo leave\n");
	return retVal;
}

