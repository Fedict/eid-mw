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

/*
 * Integration test for the PKCS#11 library.
 * Required interaction: none.
 */

#include <stdio.h>
#include "basetest.h"
#include "logtest.h"

int GetPKCS11FunctionList(CK_FUNCTION_LIST_PTR *pFunctions, void *handle){
	CK_C_GetFunctionList pC_GetFunctionList;
	CK_RV frv;

	testlog(LVL_INFO,"GetPKCS11FunctionList enter\n");

	// get function pointer to C_GetFunctionList
	pC_GetFunctionList = (CK_C_GetFunctionList) dlsym(handle, "C_GetFunctionList");
	if (pC_GetFunctionList == NULL) {
	    dlclose(handle);
	    testlog(LVL_ERROR,"dlsym failed\n");
		testlog(LVL_INFO,"GetPKCS11FunctionList leave\n");
		return 1;
	}

	// invoke C_GetFunctionList
	frv = (*pC_GetFunctionList) (pFunctions);
	if (frv != CKR_OK) {
	    testlog(LVL_ERROR,"C_GetFunctionList failed\n");
	    testlog(LVL_INFO,"GetPKCS11FunctionList leave\n");
		return 1;  
	}

	testlog(LVL_INFO,"GetPKCS11FunctionList leave\n");
	return 0;
}

CK_BBOOL ReturnedSuccesfull(CK_RV frv, CK_RV *ptrv, char* pkcs11function, char* test_name )
{
	if (CKR_OK != frv) {
	  testlog(LVL_ERROR, "%s error frv = 0x%.8x \n",pkcs11function,frv);
		testlog(LVL_INFO, "%s leave\n",test_name);
		if(*ptrv == CKR_OK)
		{
			*ptrv = frv;
		}
	  return CK_FALSE;  
	}
	return CK_TRUE;
}

CK_BBOOL ReturnedSucces(CK_RV frv, CK_RV *ptrv, char* pkcs11function)
{
	if (CKR_OK != frv) {
	  testlog(LVL_ERROR, "%s error frv = 0x%.8x \n",pkcs11function,frv);
		if(*ptrv == CKR_OK)
		{
			*ptrv = frv;
		}
	  return CK_FALSE;  
	}
	return CK_TRUE;
}

CK_BBOOL InitializeTest(void **phandle,CK_FUNCTION_LIST_PTR *pfunctions)
{
	testlog(LVL_INFO, "InitializeTest enter\n");
	*phandle = dlopen(PKCS11_LIB, RTLD_LAZY); // RTLD_NOW is slower
	if (NULL == *phandle) {
		testlog(LVL_ERROR, "dlopen error, couldn't open pkcs11 lib\n");
		testlog(LVL_INFO, "InitializeTest leave\n");
		return CK_FALSE;
	}
	GetPKCS11FunctionList(pfunctions, *phandle);

	testlog(LVL_INFO, "InitializeTest leave\n");
	return CK_TRUE;
}

testRet PrepareSlotListTest(void **phandle,CK_FUNCTION_LIST_PTR *pfunctions, CK_SLOT_ID_PTR* pslotIds, CK_ULONG_PTR pulCount,CK_BBOOL tokenPresent )
{
	testRet retVal = {CKR_OK,TEST_PASSED};	//return values of this test
	CK_RV frv = CKR_OK;						//return value of last pkcs11 function called
	*phandle = NULL;
	if (InitializeTest(phandle,pfunctions))
	{
		frv = ((*pfunctions)->C_Initialize) (NULL);
		if (ReturnedSucces(frv,&(retVal.pkcs11rv), "C_Initialize" ))
		{	
			frv = ((*pfunctions)->C_GetSlotList) (0, 0, pulCount);
			if (ReturnedSucces(frv,&(retVal.pkcs11rv), "C_GetSlotList" ))
			{
				*pslotIds = malloc(*pulCount * sizeof(CK_SLOT_INFO));
				if(*pslotIds != NULL)
				{
					frv = ((*pfunctions)->C_GetSlotList) (tokenPresent, *pslotIds, pulCount);
					if (ReturnedSucces(frv,&(retVal.pkcs11rv), "C_GetSlotList (X2)" ))
					{
						if(*pulCount == 0)
						{
							retVal.basetestrv = TEST_SKIPPED;
						}
					}
				}
				else //malloc failed
				{
					testlog(LVL_INFO,"malloc failed");
					retVal.basetestrv = TEST_ERROR;
				}
			}
			// C_Finalize
			if((retVal.basetestrv != TEST_PASSED) || (retVal.pkcs11rv != CKR_OK) )
			{
				frv = ((*pfunctions)->C_Finalize) (NULL_PTR);
			}
		}	
	}
	else
	{
		retVal.basetestrv = TEST_ERROR;
	}
	return retVal;
}

void EndSlotListTest(void *handle,CK_SLOT_ID_PTR slotIds )
{
	if(handle != NULL)
	{
		if(slotIds != NULL)
		{
			free(slotIds);
		}
		dlclose(handle);
	}
	return;
}

testRet bt_logslotdescription(CK_FUNCTION_LIST_PTR *pfunctions, CK_ULONG slotId)
{
	testRet retVal = {CKR_OK,TEST_PASSED};
	CK_RV frv = CKR_OK;						//return value of last pkcs11 function called

	CK_SLOT_INFO slotInfo;	

	frv = ((*pfunctions)->C_GetSlotInfo) (slotId, &slotInfo);
	if (ReturnedSucces(frv,&(retVal.pkcs11rv), "C_GetSlotInfo" ))
	{
		CK_UTF8CHAR   slotDescription[65];
		int idx;
		memcpy(slotDescription,slotInfo.slotDescription,64);
		slotDescription[64] = '\0';
		//remove padded spaces
		for (idx = 64 - 1; idx > 0; idx--) {
			if (slotDescription[idx] == ' ') {
				slotDescription[idx] = '\0';			
			} else {
				break;
			}		
		}
		testlog(LVL_NOLEVEL,"slot description: %s", slotDescription);
	}
	return retVal;
}
