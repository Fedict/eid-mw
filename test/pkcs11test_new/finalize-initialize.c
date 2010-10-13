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
/*
* Integration test for the PKCS#11 library.
* Tests the finalize/initialize sequence in multiple threads
* Required interaction: none.
*/

int readslots(CK_FUNCTION_LIST_PTR functions, CK_FLAGS flags) {

	testRet retVal = {CKR_OK,TEST_PASSED};	//return values of this test
	CK_RV frv = CKR_OK;						//return value of last pkcs11 function called

	CK_C_INITIALIZE_ARGS init_args ={NULL,NULL,NULL,NULL,flags,NULL};

	long slot_count;
	CK_SLOT_ID_PTR slotIds;
	int slotIdx;
	CK_INFO info;
	CK_SESSION_HANDLE session_handle;

	testlog(LVL_INFO,"readslots enter\n");

	frv = (*functions->C_Initialize) ( (CK_VOID_PTR)&init_args );
	if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Initialize", "readslots" ))
	{	
		frv = (*functions->C_GetInfo) (&info);
		if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetInfo", "readslots" ))
		{	
			testlog(LVL_INFO,"library version: %d.%d\n", info.libraryVersion.major, info.libraryVersion.minor);
			testlog(LVL_INFO,"PKCS#11 version: %d.%d\n", info.cryptokiVersion.major, info.cryptokiVersion.minor);

			// C_GetSlotList
			frv = (*functions->C_GetSlotList) (0, 0, &slot_count);
			if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetSlotList", "readslots" ))
			{	
				testlog(LVL_DEBUG,"slot count: %i\n", slot_count);
				slotIds = malloc(slot_count * sizeof(CK_SLOT_INFO));
				if(slotIds != NULL)
				{
					frv = (*functions->C_GetSlotList) (CK_FALSE, slotIds, &slot_count);
					if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetSlotList", "readslots" ))
					{	

						for (slotIdx = 0; slotIdx < slot_count; slotIdx++) 
						{
							CK_SLOT_INFO slotInfo;		
							CK_SLOT_ID slotId = slotIds[slotIdx];
							int idx;
							frv = (*functions->C_GetSlotInfo) (slotId, &slotInfo);
							if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_GetSlotList", "readslots" ))
							{
								for (idx = 64 - 1; idx > 0; idx--) 
								{
									if (slotInfo.slotDescription[idx] == ' ') {
										slotInfo.slotDescription[idx] = '\0';			
									} else {
										break;
									}		
								}
								testlog(LVL_DEBUG,"slot Id: %d\n", slotId);
								testlog(LVL_DEBUG,"slot description: %s\n", slotInfo.slotDescription);
								// C_OpenSession
								frv = (*functions->C_OpenSession)(slotId, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session_handle);
								if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_OpenSession", "readslots" ))
								{
									// C_CloseSession
									frv = (*functions->C_CloseSession) (session_handle);
									ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_CloseSession", "readslots" );
								}
							}
						}//end of for
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
		ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Finalize", "readslots" );	
	}
	testlog(LVL_INFO,"readslots leave\n");
	return 0;
}

typedef struct test_finalize_initialize_threadvars
{
	int threadRetVal;
	CK_FUNCTION_LIST_PTR functions;
	CK_FLAGS initflags;
} *LP_TEST_FIN_INI_VARS;

DWORD WINAPI pkcs11Thread( LPVOID testThreadVars ) 
{ 
	LP_TEST_FIN_INI_VARS threadVars = (LP_TEST_FIN_INI_VARS)testThreadVars;
	CK_FUNCTION_LIST_PTR functions;

	functions = threadVars->functions;

	threadVars->threadRetVal = readslots(functions,threadVars->initflags);

	return 0; 
} 

testRet test_finalize_initialize_flag(CK_FLAGS flags) {
	void *handle;						//handle to the pkcs11 library
	CK_FUNCTION_LIST_PTR functions;		// list of the pkcs11 function pointers

	testRet retVal = {CKR_OK,TEST_PASSED};	//return values of this test
	CK_RV frv = CKR_OK;	

	DWORD   dwThreadId;
	HANDLE  hThreadHandle; 
	struct test_finalize_initialize_threadvars threadVars;

	testlog(LVL_INFO, "test_finalize_initialize enter\n");
	if (InitializeTest(&handle,&functions))
	{
		readslots(functions,flags);	// run the sequence a first time 

		threadVars.functions = functions;
		threadVars.threadRetVal = 0;
		threadVars.initflags = flags;

		// Create pkcs11 thread
		hThreadHandle = CreateThread( 
			NULL,                   // default security attributes
			0,                      // use default stack size  
			pkcs11Thread,			// thread function name
			&threadVars,				// argument to thread function 
			0,                      // use default creation flags 
			&dwThreadId);			// returns the thread identifier 

		if (hThreadHandle == NULL)
		{
			testlog(LVL_ERROR, "CreateThread failed\n");
			retVal.basetestrv = TEST_ERROR;
		}
		else
		{
			// Wait until pkcs11 thread is terminated.
			if (WAIT_OBJECT_0 != WaitForSingleObject(hThreadHandle, INFINITE))
			{
				testlog(LVL_ERROR, "WaitForSingleObject failed\n");
				retVal.basetestrv = TEST_ERROR;
			}	
		}
		dlclose(handle);

		testlog(LVL_DEBUG, "second thread returned %d\n",threadVars.threadRetVal);
		testlog(LVL_INFO, "test_finalize_initialize leave\n");
	}
	return retVal;
}

testRet test_finalize_initialize(void ) {
	testRet retVal = {CKR_OK,TEST_PASSED};
	// use pkcs11 locking mechanisms
	// pkcs11 is allowed to use threads
	retVal = test_finalize_initialize_flag((CK_FLAGS)CKF_OS_LOCKING_OK);
	if ((retVal.basetestrv == TEST_PASSED) && (retVal.pkcs11rv == CKR_OK) )
	{
		// use no locking mechanisms (so we shouldn't access pkcs11 simultanious in multiple threads)
		// also use no threads within pkcs11
		retVal = test_finalize_initialize_flag((CK_FLAGS)CKF_LIBRARY_CANT_CREATE_OS_THREADS);
		if ((retVal.basetestrv == TEST_PASSED) && (retVal.pkcs11rv == CKR_OK || retVal.pkcs11rv == CKR_NEED_TO_CREATE_THREADS) )
		{
			// use pkcs11 locking mechanisms (so we shouldn't access pkcs11 simultanious in multiple threads)
			// also use no threads within pkcs11
			retVal = test_finalize_initialize_flag((CK_FLAGS)(CKF_LIBRARY_CANT_CREATE_OS_THREADS & CKF_OS_LOCKING_OK) );
			if ((retVal.basetestrv == TEST_PASSED) && (retVal.pkcs11rv == CKR_OK || retVal.pkcs11rv == CKR_NEED_TO_CREATE_THREADS) )
			{
				// use no locking mechanisms (so we shouldn't access pkcs11 simultanious in multiple threads)
				// pkcs11 is allowed to use threads
				retVal = test_finalize_initialize_flag((CK_FLAGS)0);
			}
		}
	}
	return retVal;
}




testRet test_finalize_initialize_st() 
{
	void *handle;						//handle to the pkcs11 library
	CK_FUNCTION_LIST_PTR functions;		// list of the pkcs11 function pointers

	testRet retVal = {CKR_OK,TEST_PASSED};	//return values of this test
	CK_RV frv = CKR_OK;						//return value of last pkcs11 function called
	int counter = 200;

	testlog(LVL_INFO, "test_finalize_initialize_st enter\n");
	if (InitializeTest(&handle,&functions))
	{
		while ((counter > 0) && (frv = CKR_OK) )
		{
			frv = (*functions->C_Initialize) (NULL);
			if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Initialize", "test_finalize_initialize_st" ))
			{	
				frv = (*functions->C_Finalize) (NULL_PTR);
				ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Finalize", "test_finalize_initialize_st" );
			}		
		}
		dlclose(handle);
	}
	else
	{
		retVal.basetestrv = TEST_ERROR;
	}
	testlog(LVL_INFO, "test_finalize_initialize_st leave\n");
	return retVal;
}

HANDLE ghMutex;
unsigned long CreateaMutex(CK_VOID_PTR_PTR ppMutex)
{
	ghMutex = CreateMutex( 
        NULL,              // default security attributes
        FALSE,             // initially not owned
        NULL); 

	*ppMutex = (CK_VOID_PTR)ghMutex;
	return 0;
}

testRet test_initialize_bad_args(CK_C_INITIALIZE_ARGS* pinit_args) 
{
	void *handle;						//handle to the pkcs11 library
	CK_FUNCTION_LIST_PTR functions;		// list of the pkcs11 function pointers

	testRet retVal = {CKR_OK,TEST_PASSED};	//return values of this test
	CK_RV frv = CKR_OK;						//return value of last pkcs11 function called


	CK_C_INITIALIZE_ARGS init_args ={CreateaMutex,NULL,NULL,NULL,0,NULL};

	testlog(LVL_INFO, "test_initialize_bad_args enter\n");
	if (InitializeTest(&handle,&functions))
	{
		frv = (*functions->C_Initialize) ((CK_VOID_PTR)pinit_args);
		//retVal.pkcs11rv should be CKR_ARGUMENTS_BAD as some, but not all, 
		//of the supplied function pointers to C_Initialize are non-NULL_PTR
		if (retVal.pkcs11rv != CKR_ARGUMENTS_BAD)
		{
			ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Initialize", "test_initialize_bad_args" );
			retVal.basetestrv = TEST_ERROR;			
		}
		else
		{	
			frv = (*functions->C_Finalize) (NULL_PTR);
			ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Finalize", "test_initialize_bad_args" );
		}	
		dlclose(handle);
	}
	else
	{
		retVal.basetestrv = TEST_ERROR;
	}
	testlog(LVL_INFO, "test_initialize_bad_args leave\n");
	return retVal;
}

testRet test_initialize_ownmutex() 
{
	CK_C_INITIALIZE_ARGS init_args ={CreateaMutex,NULL,NULL,NULL,0,NULL};
	return test_initialize_bad_args(&init_args);
}

testRet test_initialize_preserved() 
{
	testRet retVal = {CKR_OK,TEST_PASSED};	//return values of this test
	int wrongvalue = 12;

	CK_C_INITIALIZE_ARGS init_args ={NULL,NULL,NULL,NULL,0,(CK_VOID_PTR)&wrongvalue};//pReserved should be zero
	CK_C_INITIALIZE_ARGS init_args2 ={NULL,NULL,NULL,NULL,CKF_OS_LOCKING_OK,(CK_VOID_PTR)&wrongvalue};//pReserved should be zero
	CK_C_INITIALIZE_ARGS init_args3 ={NULL,NULL,NULL,NULL,CKF_LIBRARY_CANT_CREATE_OS_THREADS,(CK_VOID_PTR)&wrongvalue};//pReserved should be zero
	CK_C_INITIALIZE_ARGS init_args4 ={NULL,NULL,NULL,NULL,CKF_OS_LOCKING_OK & CKF_LIBRARY_CANT_CREATE_OS_THREADS,(CK_VOID_PTR)&wrongvalue};//pReserved should be zero
	retVal = test_initialize_bad_args(&init_args);
	if ((retVal.basetestrv == TEST_PASSED) && (retVal.pkcs11rv == CKR_OK) )
	{
		retVal = test_initialize_bad_args(&init_args2);
		if ((retVal.basetestrv == TEST_PASSED) && (retVal.pkcs11rv == CKR_OK) )
		{
			retVal = test_initialize_bad_args(&init_args3);
			if ((retVal.basetestrv == TEST_PASSED) && (retVal.pkcs11rv == CKR_OK) )
			{
				retVal = test_initialize_bad_args(&init_args4);
			}
		}
	}
	return retVal;
}


testRet test_finalize_preserved() 
{
	void *handle;						//handle to the pkcs11 library
	CK_FUNCTION_LIST_PTR functions;		// list of the pkcs11 function pointers

	testRet retVal = {CKR_OK,TEST_PASSED};	//return values of this test
	CK_RV frv = CKR_OK;						//return value of last pkcs11 function called
	int	wrongvalue = 0;

	testlog(LVL_INFO, "test_finalize_preserved enter\n");
	if (InitializeTest(&handle,&functions))
	{
		frv = (*functions->C_Initialize) (NULL);
		if (ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Initialize", "test_finalize_preserved" ))
		{	
			frv = (*functions->C_Finalize) (&wrongvalue);
			if (frv != CKR_ARGUMENTS_BAD)
			{
				ReturnedSuccesfull(frv,&(retVal.pkcs11rv), "C_Initialize", "test_finalize_preserved" );
				retVal.basetestrv = TEST_ERROR;			
			}
		}		
		dlclose(handle);
	}
	else
	{
		retVal.basetestrv = TEST_ERROR;
	}
	testlog(LVL_INFO, "test_finalize_preserved leave\n");
	return retVal;
}

