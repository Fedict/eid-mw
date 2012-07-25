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

testRet test_open_close_session_info() {
	void *handle;						//handle to the pkcs11 library
	CK_FUNCTION_LIST_PTR functions;		// list of the pkcs11 function pointers

	testRet retVal = {CKR_OK,TEST_PASSED};	//return values of this test
	CK_RV frv = CKR_OK;						//return value of last pkcs11 function called

	CK_INFO info;
	CK_SESSION_HANDLE session_handle;
	long slot_count;
	CK_SLOT_ID_PTR slotIds;
	int slotIdx;

	testlog(LVL_INFO, "test_open_close_session_info enter\n");
	if (InitializeTest(&handle,&functions))
	{
		frv = (*functions->C_Initialize) (NULL);
		if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Initialize", "test_open_close_session enter" ))
		{
			frv = (*functions->C_GetInfo) (&info);
			if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetInfo", "test_open_close_session enter" ))
			{
				testlog(LVL_DEBUG,"library version: %d.%d\n", info.libraryVersion.major, info.libraryVersion.minor);
				testlog(LVL_DEBUG,"PKCS#11 version: %d.%d\n", info.cryptokiVersion.major, info.cryptokiVersion.minor);

				frv = (*functions->C_GetSlotList) (0, 0, &slot_count);
				if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetSlotList", "test_open_close_session enter" ))
				{
					testlog(LVL_DEBUG,"slot count: %i\n", slot_count);
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
									testlog(LVL_DEBUG,"slot Id: %d\n", slotId);
									testlog(LVL_DEBUG,"slot description: %s\n", slotInfo.slotDescription);

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

	testlog(LVL_INFO, "test_open_close_session_info leave\n");
	return retVal;
}

testRet test_open_close_session() 
{
	void *handle = NULL;				//handle to the pkcs11 library
	CK_FUNCTION_LIST_PTR functions;		// list of the pkcs11 function pointers

	testRet retVal = {CKR_OK,TEST_PASSED};	//return values of this test
	CK_RV frv = CKR_OK;						//return value of last pkcs11 function called

	CK_ULONG ulCount = 0;
	CK_ULONG slotIdx = 0;
	CK_ULONG ulCounter = 10;
	CK_SLOT_ID_PTR slotIds = NULL;
	CK_SESSION_HANDLE session_handle;

	testlog(LVL_INFO, "test_open_close_session enter\n");

	retVal = PrepareSlotListTest(&handle,&functions, &slotIds, &ulCount,CK_TRUE );
	if((retVal.pkcs11rv == CKR_OK) && (retVal.basetestrv == TEST_PASSED))
	{
		for (slotIdx = 0; slotIdx < ulCount; slotIdx++) 
		{
			while(ulCounter > 0)
			{
				ulCounter--;
				if (frv == CKR_OK)
				{
					frv = (*functions->C_OpenSession)(slotIdx, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session_handle);
					if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_OpenSession", "test_open_close_session enter" ))
					{
						frv = (*functions->C_CloseSession) (session_handle);
						ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_CloseSession", "test_open_close_session enter" );
					}
				}
			}
		}

		frv = (*functions->C_Finalize) (NULL_PTR);
		ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Finalize", "test_open_close_session enter" );
	}
	EndSlotListTest(handle,slotIds );

	testlog(LVL_INFO, "test_open_close_session leave\n");
	return retVal;
}

testRet test_open_close_session_bad_param(CK_FLAGS flags) 
{
	void *handle = NULL;				//handle to the pkcs11 library
	CK_FUNCTION_LIST_PTR functions;		// list of the pkcs11 function pointers

	testRet retVal = {CKR_OK,TEST_PASSED};	//return values of this test
	CK_RV frv = CKR_OK;						//return value of last pkcs11 function called

	CK_ULONG ulCount = 0;
	CK_ULONG slotIdx = 0;
	CK_SLOT_ID_PTR slotIds = NULL;
	CK_SESSION_HANDLE session_handle;

	testlog(LVL_INFO, "test_open_close_session_bad_param enter\n");

	retVal = PrepareSlotListTest(&handle,&functions, &slotIds, &ulCount,CK_TRUE );
	if((retVal.pkcs11rv == CKR_OK) && (retVal.basetestrv == TEST_PASSED))
	{
		for (slotIdx = 0; slotIdx < ulCount; slotIdx++) 
		{
			frv = (*functions->C_OpenSession)(slotIdx, flags, NULL_PTR, NULL_PTR, &session_handle);
			if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_OpenSession", "test_open_close_session_bad_param enter" ))
			{
				frv = (*functions->C_CloseSession) (session_handle);
				ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_CloseSession", "test_open_close_session_bad_param enter" );
			}
			else
			{
				slotIdx = ulCount;//end the loop
			}
		}
		frv = (*functions->C_Finalize) (NULL_PTR);
		ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Finalize", "test_open_close_session enter" );
	}
	EndSlotListTest(handle,slotIds );

	testlog(LVL_INFO, "test_open_close_session_bad_param leave\n");
	return retVal;
}

testRet test_open_close_session_bad_params(void) 
{
	testRet retVal = {CKR_OK,TEST_PASSED};	//return values of this test

	retVal = test_open_close_session_bad_param(0); //CKF_SERIAL_SESSION not set
	if ((retVal.pkcs11rv == CKR_SESSION_PARALLEL_NOT_SUPPORTED) && (retVal.basetestrv == TEST_PASSED) )
	{
		retVal = test_open_close_session_bad_param(CKF_RW_SESSION | CKF_SERIAL_SESSION); //CKF_SERIAL_SESSION not set
		if ((retVal.pkcs11rv != CKR_TOKEN_WRITE_PROTECTED) && (retVal.basetestrv == TEST_PASSED) )
		{
			retVal.basetestrv = TEST_WARNING;
		}
	}
	else if ((retVal.pkcs11rv == CKR_OK))
	{
		retVal.basetestrv = TEST_ERROR;
	}
	return retVal;
}

//this function returns the session limit for the first card found
//do not return CKR_SESSION_COUNT as error, return CKR_OK instead

testRet test_open_close_session_limit(CK_FLAGS flags, CK_ULONG *pcounter) 
{
	void *handle = NULL;				//handle to the pkcs11 library
	CK_FUNCTION_LIST_PTR functions;		// list of the pkcs11 function pointers

	testRet retVal = {CKR_OK,TEST_PASSED};	//return values of this test
	CK_RV frv = CKR_OK;						//return value of last pkcs11 function called

	CK_ULONG ulCount = 0;
	CK_ULONG slotIdx = 0;
	CK_ULONG lcounter = 0;
	CK_ULONG maxcount = *pcounter;
	CK_SLOT_ID_PTR slotIds = NULL;
	CK_SESSION_HANDLE session_handle;

	testlog(LVL_INFO, "test_open_close_session_limit enter\n");
	*pcounter = 0;

	retVal = PrepareSlotListTest(&handle,&functions, &slotIds, &ulCount,CK_TRUE );
	if((retVal.pkcs11rv == CKR_OK) && (retVal.basetestrv == TEST_PASSED))
	{
		slotIdx = 0;
		if ( slotIdx < ulCount) 
		{
			//see how many sessions we can open
			while( (lcounter < maxcount) && ( frv == CKR_OK) )
			{
				lcounter++;
				frv = (*functions->C_OpenSession)(slotIdx, flags, NULL_PTR, NULL_PTR, &session_handle);
			}
			if(frv == CKR_SESSION_COUNT)
			{
				*pcounter = lcounter;
				//do not return CKR_SESSION_COUNT as error
			}
			else
			{
				ReturnedSucces(frv,&(retVal.pkcs11rv), "C_OpenSession");
			}
			if(( lcounter > 1)||(retVal.pkcs11rv == CKR_OK) ) // at least one session was opened
			{
				frv = (*functions->C_CloseAllSessions)(slotIdx);
				ReturnedSucces(frv,&(retVal.pkcs11rv), "C_CloseAllSessions");
			}
		}
		frv = (*functions->C_Finalize) (NULL_PTR);
		ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Finalize", "test_open_close_session_limit enter" );
	}
	EndSlotListTest(handle,slotIds );

	testlog(LVL_INFO, "test_open_close_session_limit leave\n");
	return retVal;
}

testRet test_open_close_session_limits() 
{
	testRet retVal = {CKR_OK,TEST_PASSED};	//return values of this test
	CK_ULONG counter = 200;

	retVal = test_open_close_session_limit(CKF_SERIAL_SESSION, &counter); 
	if ((retVal.pkcs11rv == CKR_OK) && (retVal.basetestrv == TEST_PASSED) )
	{
		testlog(LVL_INFO, "session_limits read-only tried 200, able to open %d\n",counter);
		counter = 50;
		retVal = test_open_close_session_limit(CKF_RW_SESSION | CKF_SERIAL_SESSION, &counter); //CKF_SERIAL_SESSION not set
		if ((retVal.pkcs11rv == CKR_OK) && (retVal.basetestrv == TEST_PASSED) )
		{
			testlog(LVL_INFO, "session_limits read-write tried 50, able to open %d\n",counter);
		}
	}
	return retVal;
}
