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

PrintSlotInfo(CK_SLOT_INFO *slotInfo);

testRet test_get_token_info() 
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

	testlog(LVL_INFO, "test_get_token_info enter\n");

	retVal = PrepareSlotListTest(&handle,&functions, &slotIds, &ulCount,CK_TRUE );
	if((retVal.pkcs11rv == CKR_OK) && (retVal.basetestrv == TEST_PASSED))
	{
		for (slotIdx = 0; slotIdx < ulCount; slotIdx++) 
		{
			CK_SLOT_INFO slotInfo;
			CK_TOKEN_INFO tokenInfo;
			// Get slot information for slot
			frv = (*functions->C_GetSlotInfo)(slotIds[slotIdx], &slotInfo);
			if (ReturnedSucces(frv,&(retVal.pkcs11rv), "C_GetSlotInfo"))
			{
				PrintSlotInfo(&slotInfo);
				// Get token information for slot
				frv = (*functions->C_GetTokenInfo)(slotIds[slotIdx], &tokenInfo);
				if (ReturnedSucces(frv,&(retVal.pkcs11rv), "C_GetTokenInfo"))
				{
					PrintTokenInfo(&slotInfo);
				}
			}
		}

		frv = (*functions->C_Finalize) (NULL_PTR);
		ReturnedSucces(frv,&(retVal.pkcs11rv), "C_Finalize" );
	}
	EndSlotListTest(handle,slotIds );

	testlog(LVL_INFO, "test_get_token_info leave\n");
	return retVal;
}

PrintSlotInfo(CK_SLOT_INFO *slotInfo)
{
	CK_UTF8CHAR printbuffer[65];

	strncpy(printbuffer,slotInfo->slotDescription,64);
	printbuffer[64] = '\0';
	testlog(LVL_DEBUG, "slotDescription %s\n",printbuffer);
	strncpy(printbuffer,slotInfo->manufacturerID,32);
	printbuffer[32] = '\0';
	testlog(LVL_DEBUG, "manufacturerID %s\n",printbuffer);

	testlog(LVL_DEBUG, "flags 0x%x\n",slotInfo->flags);
	testlog(LVL_DEBUG, "hardwareVersion %d.%d \n",slotInfo->hardwareVersion.major,slotInfo->hardwareVersion.minor);
	testlog(LVL_DEBUG, "firmwareVersion %d.%d \n",slotInfo->firmwareVersion.major,slotInfo->firmwareVersion.minor);
}

PrintTokenInfo(CK_TOKEN_INFO *tokenInfo)
{
	CK_UTF8CHAR printbuffer[33];

	strncpy(printbuffer,tokenInfo->label,32);
	printbuffer[32] = '\0';
	testlog(LVL_DEBUG, "label %s\n",printbuffer);
	strncpy(printbuffer,tokenInfo->manufacturerID,32);
	printbuffer[32] = '\0';
	testlog(LVL_DEBUG, "manufacturerID %s\n",printbuffer);
	strncpy(printbuffer,tokenInfo->model,16);
	printbuffer[16] = '\0';
	testlog(LVL_DEBUG, "model %s\n",printbuffer);
	strncpy(printbuffer,tokenInfo->serialNumber,16);
	printbuffer[16] = '\0';
	testlog(LVL_DEBUG, "serialNumber %s\n",printbuffer);
	testlog(LVL_DEBUG, "flags 0x%x\n",tokenInfo->flags);

	/* ulMaxSessionCount, ulSessionCount, ulMaxRwSessionCount,
	* ulRwSessionCount, ulMaxPinLen, and ulMinPinLen have all been
	* changed from CK_USHORT to CK_ULONG for v2.0 */
	testlog(LVL_DEBUG, "ulMaxSessionCount %u\n",tokenInfo->ulMaxSessionCount);
	testlog(LVL_DEBUG, "ulSessionCount %u\n",tokenInfo->ulSessionCount);
	testlog(LVL_DEBUG, "ulMaxRwSessionCount %u\n",tokenInfo->ulMaxRwSessionCount);
	testlog(LVL_DEBUG, "ulRwSessionCount %u\n",tokenInfo->ulRwSessionCount);
	testlog(LVL_DEBUG, "ulMaxPinLen %u\n",tokenInfo->ulMaxPinLen);
	testlog(LVL_DEBUG, "ulMinPinLen %u\n",tokenInfo->ulMinPinLen);
	testlog(LVL_DEBUG, "ulTotalPublicMemory %u\n",tokenInfo->ulTotalPublicMemory);
	testlog(LVL_DEBUG, "ulFreePublicMemory %u\n",tokenInfo->ulFreePublicMemory);
	testlog(LVL_DEBUG, "ulTotalPrivateMemory %u\n",tokenInfo->ulTotalPrivateMemory);
	testlog(LVL_DEBUG, "ulFreePrivateMemory %u\n",tokenInfo->ulFreePrivateMemory);

	/* hardwareVersion, firmwareVersion, and time are new for
	* v2.0 */
	testlog(LVL_DEBUG, "hardwareVersion %d.%d \n",tokenInfo->hardwareVersion.major,tokenInfo->hardwareVersion.minor);
	testlog(LVL_DEBUG, "firmwareVersion %d.%d \n",tokenInfo->firmwareVersion.major,tokenInfo->firmwareVersion.minor);
	testlog(LVL_DEBUG, "model %s\n",printbuffer);
	strncpy(printbuffer,tokenInfo->utcTime,16);
	printbuffer[16] = '\0';
	testlog(LVL_DEBUG, "utcTime %s\n",printbuffer);
}