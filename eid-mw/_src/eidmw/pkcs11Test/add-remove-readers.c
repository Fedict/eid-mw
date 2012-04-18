/* ****************************************************************************

* eID Middleware Project.
* Copyright (C) 2012 FedICT.
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

void show_slot_info(CK_SLOT_INFO_PTR slotInfo);

CK_RV test_add_remove_readers() {
	void *handle;
	CK_FUNCTION_LIST_PTR functions;
	int retVal = 0;
	CK_RV frv;
	CK_RV	trv = CKR_OK; 
	long slot_count;
	CK_SLOT_ID_PTR slotIds;
	int slotIdx;
	CK_ULONG ulObjectCount=1;
	int keypress = 0;

	testlog(LVL_INFO, "test_getallobjects enter\n");
	if (!InitializeTest(&handle,&functions))
	{
		testlog(LVL_INFO, "test_getallobjects leave\n");
		return 1;
	}
	frv = (*functions->C_Initialize) (NULL);
	if (ReturnedSuccesfull(frv,&trv, "C_Initialize", "test_getallobjects" ))
	{		
		printf("press 'q' to end this test\n");
		printf("re-arrange cardreaders and press enter\n");
		keypress = _getch();
		while ( keypress != 'q')
		{

			frv = (*functions->C_GetSlotList) (CK_FALSE, 0, &slot_count);
			if (ReturnedSuccesfull(frv,&trv, "C_GetSlotList", "test_getallobjects" ))
			{

				printf("slot count: %i\n", slot_count);
				slotIds = malloc(slot_count * sizeof(CK_SLOT_INFO));
				frv = (*functions->C_GetSlotList) (CK_FALSE, slotIds, &slot_count);
				if (ReturnedSuccesfull(frv,&trv, "C_GetSlotList (X2)", "test_getallobjects" ))
				{
					for (slotIdx = 0; slotIdx < slot_count; slotIdx++) 
					{
						CK_SLOT_INFO slotInfo;		
						CK_SLOT_ID slotId = slotIds[slotIdx];
						int idx;
						frv = (*functions->C_GetSlotInfo) (slotId, &slotInfo);
						if (CKR_OK != frv) {
							printf("ERROR C_GetSlotInfo returned 0x%x\n",frv);
						}
						else
						{
							for (idx = 64 - 1; idx > 0; idx--) {
								if (slotInfo.slotDescription[idx] == ' ') {
									slotInfo.slotDescription[idx] = '\0';			
								} else {
									break;
								}		
							}
						}
						printf("slot Id: %d\n", slotId);
						printf("slot description: %s\n", slotInfo.slotDescription);


					}// end for

				}
			}
			printf("press 'q' to end this test\n");
			printf("re-arrange cardreaders and press enter\n");
			keypress = _getch();
		}//end while
		frv = (*functions->C_Finalize) (NULL_PTR);
		ReturnedSuccesfull(frv,&trv, "C_Finalize", "test_getallobjects" );
	}
	dlclose(handle);
	testlog(LVL_INFO, "test_getallobjects leave\n");
	return trv;
} 


CK_RV test_add_remove_readerevents() {
	void *handle;
	CK_FUNCTION_LIST_PTR functions;
	int retVal = 0;
	CK_RV frv;
	CK_RV	trv = CKR_OK; 
	long slot_count;
	CK_SLOT_ID_PTR slotIds;
	CK_SLOT_ID slotId;
	CK_SLOT_INFO slotInfo;
	int slotIdx;
	CK_ULONG ulObjectCount=1;
	int keypress = 0;

	testlog(LVL_INFO, "test_getallobjects enter\n");
	if (!InitializeTest(&handle,&functions))
	{
		testlog(LVL_INFO, "test_getallobjects leave\n");
		return 1;
	}
	frv = (*functions->C_Initialize) (NULL);
	if (ReturnedSuccesfull(frv,&trv, "C_Initialize", "test_getallobjects" ))
	{		
		printf("press 'q' to end this test\n");
		printf("press enter to start waiting for card and reader events\n");
		keypress = _getch();
		while ( keypress != 'q')
		{
			printf("waiting for card and reader events...\n");
			frv = (*functions->C_WaitForSlotEvent)(0,&slotId,NULL_PTR);
			if (!ReturnedSuccesfull(frv,&trv, "C_WaitForSlotEvent", "test_getallobjects" ))
			{
				printf("ERROR: C_WaitForSlotEvent returned 0x%x\n",frv);
			}
			else
			{
				printf("event received on slotID %d\n",slotId);
				frv = (*functions->C_GetSlotInfo) (slotId, &slotInfo);
				if (CKR_OK != frv) 
				{
					printf("ERROR C_GetSlotInfo returned 0x%x\n",frv);
				}
				else
				{
					printf("slot Id: %d\n", slotId);
					show_slot_info(&slotInfo);
				}
			}

			frv = (*functions->C_GetSlotList) (CK_FALSE, 0, &slot_count);
			if (ReturnedSuccesfull(frv,&trv, "C_GetSlotList", "test_getallobjects" ))
			{

				printf("slot count: %i\n", slot_count);
				slotIds = malloc(slot_count * sizeof(CK_SLOT_INFO));
				frv = (*functions->C_GetSlotList) (CK_FALSE, slotIds, &slot_count);
				if (ReturnedSuccesfull(frv,&trv, "C_GetSlotList (X2)", "test_getallobjects" ))
				{
					for (slotIdx = 0; slotIdx < slot_count; slotIdx++) 
					{		
						CK_SLOT_ID slotId = slotIds[slotIdx];
						frv = (*functions->C_GetSlotInfo) (slotId, &slotInfo);
						if (CKR_OK != frv) 
						{
							printf("ERROR C_GetSlotInfo returned 0x%x\n",frv);
						}
						else
						{
							printf("slot Id: %d\n", slotId);
							show_slot_info(&slotInfo);
						}
					}// end for
				}
			}
			printf("press 'q' to end this test\n");
			printf("press enter to start waiting for card and reader events\n");
			keypress = _getch();
		}//end while
		frv = (*functions->C_Finalize) (NULL_PTR);
		ReturnedSuccesfull(frv,&trv, "C_Finalize", "test_getallobjects" );
	}
	dlclose(handle);
	testlog(LVL_INFO, "test_getallobjects leave\n");
	return trv;
} 

void show_slot_info(CK_SLOT_INFO_PTR slotInfo)
{
	int idx;

	for (idx = 64 - 1; idx > 0; idx--) {
		if (slotInfo->slotDescription[idx] == ' ') {
			slotInfo->slotDescription[idx] = '\0';			
		} else {
			break;
		}		
	}
	printf("slot description: %s\n", slotInfo->slotDescription);
}
