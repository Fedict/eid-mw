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

testRet test_open_close_session() {
	void *handle;						//handle to the pkcs11 library
	CK_FUNCTION_LIST_PTR functions;		// list of the pkcs11 function pointers

	testRet retVal = {CKR_OK,TEST_PASSED};	//return values of this test
	CK_RV frv = CKR_OK;						//return value of last pkcs11 function called

	CK_INFO info;
	CK_SESSION_HANDLE session_handle;
	long slot_count;
	CK_SLOT_ID_PTR slotIds;
	int slotIdx;

	testlog(LVL_INFO, "test_open_close_session enter\n");
	if (InitializeTest(&handle,&functions))
	{
		frv = (*functions->C_Initialize) (NULL);
		if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Initialize", "test_open_close_session enter" ))
		{
			frv = (*functions->C_GetInfo) (&info);
			if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetInfo", "test_open_close_session enter" ))
			{
				testlog(LVL_INFO,"library version: %d.%d\n", info.libraryVersion.major, info.libraryVersion.minor);
				testlog(LVL_INFO,"PKCS#11 version: %d.%d\n", info.cryptokiVersion.major, info.cryptokiVersion.minor);

				frv = (*functions->C_GetSlotList) (0, 0, &slot_count);
				if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetSlotList", "test_open_close_session enter" ))
				{
					testlog(LVL_INFO,"slot count: %i\n", slot_count);
					slotIds = malloc(slot_count * sizeof(CK_SLOT_INFO));
					if(slotIds != NULL)
					{
						frv = (*functions->C_GetSlotList) (CK_FALSE, slotIds, &slot_count);
						if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetSlotList X2", "test_open_close_session enter" ))
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
								frv = (*functions->C_GetSlotInfo) (slotId, &slotInfo);
								if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetSlotInfo", "test_open_close_session enter" ))
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

									frv = (*functions->C_OpenSession)(slotId, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session_handle);
									if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_OpenSession", "test_open_close_session enter" ))
									{
										frv = (*functions->C_CloseSession) (session_handle);
										ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_CloseSession", "test_open_close_session enter" );
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
			frv = (*functions->C_Finalize) (NULL_PTR);
			ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Finalize", "test_open_close_session enter" );
		}
		dlclose(handle);
	}
	else
	{
		retVal.basetestrv = TEST_ERROR;
	}

	testlog(LVL_INFO, "test_open_close_session enter leave\n");
	return retVal;
}

