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


testRet test_waitforslotevent_noblock() {
	void *handle = NULL;				//handle to the pkcs11 library
	CK_FUNCTION_LIST_PTR functions;		// list of the pkcs11 function pointers

	testRet retVal = {CKR_OK,TEST_PASSED};	//return values of this test
	CK_RV frv = CKR_OK;						//return value of last pkcs11 function called

	CK_ULONG ulCount = 0;
	CK_ULONG slotIdx = 0;
	CK_SLOT_ID_PTR slotIds = NULL;
	CK_SLOT_ID slotId = 0;

	testlog(LVL_INFO, "test_waitforslotevent enter\n");

	retVal = PrepareSlotListTest(&handle,&functions, &slotIds, &ulCount,CK_TRUE );
	if((retVal.pkcs11rv == CKR_OK) && (retVal.basetestrv == TEST_PASSED))
	{
		frv = (*functions->C_WaitForSlotEvent)(CKF_DONT_BLOCK, &slotId,NULL_PTR);
		if (frv != CKR_NO_EVENT)
		{
			ReturnedSucces(frv,&(retVal.pkcs11rv), "C_WaitForSlotEvent");
			retVal.basetestrv = TEST_ERROR;
		}
		frv = (*functions->C_Finalize) (NULL_PTR);
		ReturnedSucces(frv,&(retVal.pkcs11rv), "C_Finalize");
	}
	EndSlotListTest(handle,slotIds );

	testlog(LVL_INFO, "test_waitforslotevent leave\n");
	return retVal;
}

testRet test_waitforslotevent_userinteraction_flags(CK_FLAGS flags) {
	void *handle = NULL;				//handle to the pkcs11 library
	CK_FUNCTION_LIST_PTR functions;		// list of the pkcs11 function pointers

	testRet retVal = {CKR_OK,TEST_PASSED};	//return values of this test
	CK_RV frv = CKR_OK;						//return value of last pkcs11 function called

	CK_ULONG ulCount = 0;
	CK_ULONG slotIdx = 0;
	CK_SLOT_ID_PTR slotIds = NULL;
	CK_SLOT_ID slotId = 0;

	testlog(LVL_INFO, "test_waitforslotevent_block enter\n");

	retVal = PrepareSlotListTest(&handle,&functions, &slotIds, &ulCount,CK_TRUE );
	if((retVal.pkcs11rv == CKR_OK) && (retVal.basetestrv == TEST_PASSED))
	{
		if(ulCount > 0){
			testlog(LVL_NOLEVEL, "please remove a card from a slot and press a key\n");
			testlog(LVL_NOLEVEL, "if the test does not continue afterwards, consider it failed and enter card again\n");
			getchar();
			if((retVal.pkcs11rv == CKR_OK) && (retVal.basetestrv == TEST_PASSED)){
				frv = (*functions->C_WaitForSlotEvent)(flags, &slotId,NULL_PTR);
				if (ReturnedSucces(frv,&(retVal.pkcs11rv), "C_WaitForSlotEvent")){
					testlog(LVL_INFO, "card removed from slotId %d\n",slotId);
					testlog(LVL_NOLEVEL, "please enter a card into a slot and press a key\n");
					testlog(LVL_NOLEVEL, "if the test does not continue afterwards, consider it failed\n");
					getchar();
					frv = (*functions->C_WaitForSlotEvent)(flags, &slotId,NULL_PTR);
					if (ReturnedSucces(frv,&(retVal.pkcs11rv), "C_WaitForSlotEvent")){
						testlog(LVL_INFO, "card inserted into slotId %d\n",slotId);
					}
				}
				else
				{
					testlog(LVL_NOLEVEL, "please enter a card into a slot and press a key\n");
					testlog(LVL_NOLEVEL, "if the test does not continue afterwards, consider it failed\n");
					getchar();
				}
			}
		}
		frv = (*functions->C_Finalize) (NULL_PTR);
		ReturnedSucces(frv,&(retVal.pkcs11rv), "C_Finalize");
	}
	EndSlotListTest(handle,slotIds );

	testlog(LVL_INFO, "test_waitforslotevent_block leave\n");
	return retVal;
}

testRet test_waitforslotevent_userinteraction()
{
	testRet retVal = {CKR_OK,TEST_PASSED};	//return values of this test
	retVal = test_waitforslotevent_userinteraction_flags(CKF_DONT_BLOCK);
	if((retVal.pkcs11rv == CKR_OK) && (retVal.basetestrv == TEST_PASSED)){
		retVal = test_waitforslotevent_userinteraction_flags(0);
	}
	return retVal;
}

DWORD WINAPI test_waitforslotevent_finalize( LPVOID thefunctions ) 
{
	CK_FUNCTION_LIST_PTR functions = (CK_FUNCTION_LIST_PTR)thefunctions;	

	Sleep(500);
	functions->C_Finalize(NULL_PTR);

	return 0;
}

testRet test_waitforslotevent_whilefinalize() {
	void *handle = NULL;				//handle to the pkcs11 library
	CK_FUNCTION_LIST_PTR functions;		//list of the pkcs11 function pointers

	testRet retVal = {CKR_OK,TEST_PASSED};	//return values of this test
	CK_RV frv = CKR_OK;						//return value of last pkcs11 function called

	CK_ULONG ulCount = 0;
	CK_ULONG slotIdx = 0;
	CK_SLOT_ID_PTR slotIds = NULL;
	CK_SLOT_ID slotId = 0;

	testlog(LVL_INFO, "test_waitforslotevent_whilefinalize enter\n");

	retVal = PrepareSlotListTest(&handle,&functions, &slotIds, &ulCount,CK_TRUE );
	if((retVal.pkcs11rv == CKR_OK) && (retVal.basetestrv == TEST_PASSED))
	{
		DWORD   dwThreadId;
		HANDLE  hThreadHandle; 
		// Create pkcs11 thread
		hThreadHandle = CreateThread( 
			NULL,                   // default security attributes
			0,                      // use default stack size  
			test_waitforslotevent_finalize,			// thread function name
			(LPVOID)functions,		// argument to thread function 
			0,                      // use default creation flags 
			&dwThreadId);			// returns the thread identifier 

		if (hThreadHandle == NULL)
		{
			testlog(LVL_ERROR, "CreateThread failed\n");
			retVal.basetestrv = TEST_ERROR;
		}
		else
		{
			frv = (*functions->C_WaitForSlotEvent)(0, &slotId,NULL_PTR);
			if (frv != CKR_CRYPTOKI_NOT_INITIALIZED)
			{
				ReturnedSucces(frv,&(retVal.pkcs11rv), "C_WaitForSlotEvent");
				retVal.basetestrv = TEST_ERROR;
			}
			// Wait until thread is terminated.
			if (WAIT_OBJECT_0 != WaitForSingleObject(hThreadHandle, INFINITE))
			{
				testlog(LVL_ERROR, "WaitForSingleObject failed\n");
				retVal.basetestrv = TEST_ERROR;
			}	
		}
	}
	EndSlotListTest(handle,slotIds );

	testlog(LVL_INFO, "test_waitforslotevent_whilefinalize leave\n");
	return retVal;
}
