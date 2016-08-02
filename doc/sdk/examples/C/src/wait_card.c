
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

#include "base.h"					

CK_ULONG beidsdk_waitcard(void);

int main() {
	CK_ULONG retval = CKR_OK;
	retval = beidsdk_waitcard();

	_getch();
}

CK_ULONG beidsdk_waitcard() 
{
	void *pkcs11Handle;									//pkcs11Handle to the pkcs11 library		  
	CK_FUNCTION_LIST_PTR pFunctions;		//list of the pkcs11 function pointers
	CK_C_GetFunctionList pC_GetFunctionList;
	CK_RV retVal = CKR_OK;
	CK_BBOOL cardInserted = CK_FALSE;

	pkcs11Handle = dlopen(PKCS11_LIB, RTLD_LAZY); // RTLD_NOW is slower
	if (pkcs11Handle != NULL)
	{
		// get function pointer to C_GetFunctionList
		pC_GetFunctionList = (CK_C_GetFunctionList) dlsym(pkcs11Handle, "C_GetFunctionList");
		if (pC_GetFunctionList != NULL) 
		{
			// invoke C_GetFunctionList to get the list of pkcs11 function pointers
			retVal = (*pC_GetFunctionList) (&pFunctions);
			if (retVal == CKR_OK) 
			{
				// initialize Cryptoki
				retVal = (pFunctions->C_Initialize) (NULL);
				if (retVal == CKR_OK)
				{		
					CK_ULONG slot_count;
					// retrieve the number of slots (cardreaders)
					// to find also the slots without tokens inserted, set the first parameter to CK_FALSE
					retVal = (pFunctions->C_GetSlotList) (CK_FALSE, 0, &slot_count);
					if ((retVal == CKR_OK)&& (slot_count > 0))
					{
						CK_SLOT_ID_PTR slotIds = (CK_SLOT_ID_PTR)malloc(slot_count * sizeof(CK_SLOT_INFO));
						if(slotIds != NULL)
						{
							// retrieve the list of slots (cardreaders)
							retVal = (pFunctions->C_GetSlotList) (CK_FALSE, slotIds, &slot_count);
							if (retVal == CKR_OK)
							{
								CK_ULONG slotIdx;
								CK_SLOT_INFO slotinfo;
								CK_UTF8CHAR slotDescription[65];
								//check if a card is already present in one of the readers
								for (slotIdx = 0; slotIdx < slot_count; slotIdx++) 
								{
									retVal = (pFunctions->C_GetSlotInfo) (slotIds[slotIdx], &slotinfo);
									if ( (retVal == CKR_OK) && (slotinfo.flags & CKF_TOKEN_PRESENT) )
									{
										memcpy(slotDescription,slotinfo.slotDescription,64);
										slotDescription[64] = '\0'; //make the string null terminated
										printf("Card found in reader %s \n",slotDescription);

										//a card is found in the slot
										cardInserted = CK_TRUE;
									}
								}

								if(cardInserted == CK_FALSE)
								{
									CK_FLAGS flags = 0; //use CKF_DONT_BLOCK if you don't want C_WaitForSlotEvent to block
									CK_SLOT_ID slotId;  //will receive the ID of the slot that the event occurred in
									printf("Please insert a beid card\n");
									retVal = (pFunctions->C_WaitForSlotEvent) (flags,&slotId, NULL_PTR);
									if(retVal == CKR_OK)
									{
										printf("Card inserted \n");
										for (slotIdx = 0; slotIdx < slot_count; slotIdx++) 
										{
											if(slotId == slotIds[slotIdx])
											{
												retVal = (pFunctions->C_GetSlotInfo) (slotId, &slotinfo);
												if(retVal == CKR_OK)
												{
													memcpy(slotDescription,slotinfo.slotDescription,64);
													slotDescription[64] = '\0'; //make the string null terminated
													printf("into reader %s \n",slotDescription);
												}
											}
										}
									}
								}
							}
							free(slotIds);
						}
						else //malloc failed
						{
							printf("malloc failed\n");
							retVal = CKR_GENERAL_ERROR;
						}
					}//no slots found
					else if (slot_count == 0)
					{
						printf("no slots found\n");
					}
					if (retVal == CKR_OK)
						retVal = (pFunctions->C_Finalize) (NULL_PTR);
					else
						(pFunctions->C_Finalize) (NULL_PTR);
				}
			}
		}
		dlclose(pkcs11Handle);
	}
	return retVal;
} 

