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


testRet test_getslotlist_2calls(CK_BBOOL tokenPresent, long* pslot_count) {
	void *handle;						//handle to the pkcs11 library
	CK_FUNCTION_LIST_PTR functions;		// list of the pkcs11 function pointers

	testRet retVal = {CKR_OK,TEST_PASSED};	//return values of this test
	CK_RV frv = CKR_OK;						//return value of last pkcs11 function called

	CK_SLOT_ID_PTR slotIds;

	testlog(LVL_INFO, "test_getslotlist_2calls enter\n");
	if (InitializeTest(&handle,&functions))
	{
		frv = (*functions->C_Initialize) (NULL);
		if (ReturnedSucces(frv,&(retVal.pkcs11rv), "C_Initialize" ))
		{	
			frv = (*functions->C_GetSlotList) (0, 0, pslot_count);
			if (ReturnedSucces(frv,&(retVal.pkcs11rv), "C_GetSlotList" ))
			{
				testlog(LVL_INFO,"slot count: %i\n", *pslot_count);
				slotIds = malloc(*pslot_count * sizeof(CK_SLOT_INFO));
				if(slotIds != NULL)
				{
					frv = (*functions->C_GetSlotList) (CK_FALSE, slotIds, pslot_count);
					if (ReturnedSucces(frv,&(retVal.pkcs11rv), "C_GetSlotList (X2)" ))
					{
						testlog(LVL_INFO,"slot count: %i\n", *pslot_count);
						if(*pslot_count == 0)
						{
							retVal.basetestrv = TEST_SKIPPED;
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
			// C_Finalize
			frv = (*functions->C_Finalize) (NULL_PTR);
			ReturnedSucces(frv,&(retVal.pkcs11rv), "C_Finalize" );
		}
		dlclose(handle);
	}
	else
	{
		retVal.basetestrv = TEST_ERROR;
	}

	testlog(LVL_INFO, "test_getslotlist_2calls leave\n");
	return retVal;
}

testRet test_getslotlist_1call(CK_BBOOL tokenPresent, long* pslot_count) {
	void *handle;						//handle to the pkcs11 library
	CK_FUNCTION_LIST_PTR functions;		// list of the pkcs11 function pointers

	testRet retVal = {CKR_OK,TEST_PASSED};	//return values of this test
	CK_RV frv = CKR_OK;						//return value of last pkcs11 function called

	CK_SLOT_ID_PTR slotIds;

	testlog(LVL_INFO, "test_getslotlist_1call enter\n");
	if (InitializeTest(&handle,&functions))
	{
		frv = (*functions->C_Initialize) (NULL);
		if (ReturnedSucces(frv,&(retVal.pkcs11rv), "C_Initialize"))
		{	
			testlog(LVL_INFO,"slot count: %i\n", *pslot_count);
			slotIds = malloc(*pslot_count * sizeof(CK_SLOT_INFO));
			if(slotIds != NULL)
			{
				frv = (*functions->C_GetSlotList) (tokenPresent, slotIds, pslot_count);
				if (ReturnedSucces(frv,&(retVal.pkcs11rv), "C_GetSlotList (X1)" ))
				{
					testlog(LVL_INFO,"slot count: %i\n", *pslot_count);
					if(*pslot_count == 0)
					{
						retVal.basetestrv = TEST_SKIPPED;
					}
				}
				free(slotIds);
			}
			else //malloc failed
			{
				testlog(LVL_INFO,"malloc failed");
				retVal.basetestrv = TEST_ERROR;
			}
			// C_Finalize
			frv = (*functions->C_Finalize) (NULL_PTR);
			ReturnedSucces(frv,&(retVal.pkcs11rv), "C_Finalize" );
		}
		dlclose(handle);
	}
	else
	{
		retVal.basetestrv = TEST_ERROR;
	}

	testlog(LVL_INFO, "test_getslotlist_1call leave\n");
	return retVal;
}

testRet test_getslotlist()
{
	testRet retVal = {CKR_OK,TEST_PASSED};	//return values of this test
	long slot_cnt_all = 0;					//slot count of all slots (by using 2 calls to C_getSlotList)
	long slot_cnt_tpresent = 0;				//slot count of slots with a token present (by using 2 calls to C_getSlotList)
	long slot_cnt_all_onecall = 0;			//slot count of all slots (by using 1 call to C_getSlotList)
	long slot_cnt_tpresent_onecall = 0;		//slot count of slots with a token present (by using 1 call to C_getSlotList)

	retVal = test_getslotlist_2calls(CK_TRUE,  &slot_cnt_tpresent);
	if((retVal.pkcs11rv == CKR_OK) && (retVal.basetestrv == TEST_PASSED))
	{
		retVal = test_getslotlist_2calls(CK_FALSE,  &slot_cnt_all);
		if(slot_cnt_all < slot_cnt_tpresent)
		{
			retVal.basetestrv = TEST_ERROR;
		}
		else if((retVal.pkcs11rv == CKR_OK) && (retVal.basetestrv == TEST_PASSED))
		{
			slot_cnt_all_onecall = slot_cnt_all;
			test_getslotlist_1call(CK_TRUE,  &slot_cnt_all_onecall);
			if(slot_cnt_all_onecall != slot_cnt_all)
			{
				retVal.basetestrv = TEST_ERROR;
			}
			else if((retVal.pkcs11rv == CKR_OK) && (retVal.basetestrv == TEST_PASSED))
			{
				slot_cnt_tpresent_onecall = slot_cnt_tpresent;
				test_getslotlist_1call(CK_TRUE,  &slot_cnt_tpresent_onecall);
				if(slot_cnt_tpresent_onecall != slot_cnt_tpresent)
				{
					retVal.basetestrv = TEST_ERROR;
				}
			}
		}
	}
	return retVal;
}

testRet test_getslotlist_multiple_slots()
{
	void *handle;						//handle to the pkcs11 library
	CK_FUNCTION_LIST_PTR functions;		// list of the pkcs11 function pointers

	testRet retVal = {CKR_OK,TEST_PASSED};	//return values of this test
	CK_RV frv = CKR_OK;						//return value of last pkcs11 function called

	CK_SLOT_ID_PTR slotIds;
	long slot_count=0;

	testlog(LVL_INFO, "test_getslotlist_multiple_slots enter\n");
	if (InitializeTest(&handle,&functions))
	{
		frv = (*functions->C_Initialize) (NULL);
		if (ReturnedSucces(frv,&(retVal.pkcs11rv), "C_Initialize" ))
		{	
			frv = (*functions->C_GetSlotList) (0, 0, &slot_count);
			if (ReturnedSucces(frv,&(retVal.pkcs11rv), "C_GetSlotList" ))
			{
				if(slot_count < 2)
				{
					retVal.basetestrv = TEST_SKIPPED;
				}
				else
				{
					slotIds = malloc(1 * sizeof(CK_SLOT_INFO));
					slot_count = 1;
					if(slotIds != NULL)
					{
						frv = (*functions->C_GetSlotList) (CK_FALSE, slotIds, &slot_count);
						if(frv != CKR_BUFFER_TOO_SMALL)
						{
							retVal.basetestrv = TEST_ERROR;
							ReturnedSucces(frv,&(retVal.pkcs11rv), "C_GetSlotList (X2)" );
						}
						free(slotIds);
					}
					else //malloc failed
					{
						retVal.basetestrv = TEST_ERROR;
					}
				}
			}
			// C_Finalize
			frv = (*functions->C_Finalize) (NULL_PTR);
			ReturnedSucces(frv,&(retVal.pkcs11rv), "C_Finalize" );
		}
		dlclose(handle);
	}
	else
	{
		retVal.basetestrv = TEST_ERROR;
	}

	testlog(LVL_INFO, "test_getslotlist_multiple_slots leave\n");
	return retVal;
}
