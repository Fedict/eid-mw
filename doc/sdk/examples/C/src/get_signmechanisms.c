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

CK_ULONG beidsdk_getsignmechanisms(void);

int main() {
	CK_ULONG retval = CKR_OK;
	retval = beidsdk_getsignmechanisms();

	_getch();
}

CK_ULONG beidsdk_getsignmechanisms() 
{
	void *pkcs11Handle;									//pkcs11Handle to the pkcs11 library		  
	CK_FUNCTION_LIST_PTR pFunctions;		//list of the pkcs11 function pointers
	CK_C_GetFunctionList pC_GetFunctionList;
	CK_RV retVal = CKR_OK;

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
					// retrieve the number of slots (cardreaders) found that hold a token (card)
					// to find also the slots without tokens inserted, set the first parameter to CK_FALSE
					retVal = (pFunctions->C_GetSlotList) (CK_TRUE, 0, &slot_count);
					if ((retVal == CKR_OK)&& (slot_count > 0))
					{
						CK_SLOT_ID_PTR slotIds = (CK_SLOT_ID_PTR)malloc(slot_count * sizeof(CK_SLOT_INFO));
						if(slotIds != NULL)
						{
							// retrieve the list of slots (cardreaders) found that hold a token (card)
							retVal = (pFunctions->C_GetSlotList) (CK_TRUE, slotIds, &slot_count);
							if (retVal == CKR_OK)
							{
								CK_ULONG slotIdx;

								for (slotIdx = 0; slotIdx < slot_count; slotIdx++) 
								{
									CK_ULONG ulMechCount = 0;
									CK_MECHANISM_TYPE_PTR pMechanismList = NULL;
									// C_GetMechanismList
									retVal = (pFunctions->C_GetMechanismList)(slotIds[slotIdx], NULL_PTR, &ulMechCount);
									if( (retVal == CKR_OK) && (ulMechCount > 0) )
									{
										pMechanismList = (CK_MECHANISM_TYPE_PTR)malloc(ulMechCount*sizeof(CK_MECHANISM_TYPE));
										if(pMechanismList != NULL)
										{//CK_DEFINE_FUNCTION(CK_RV, C_GetMechanismInfo)(
											retVal = (pFunctions->C_GetMechanismList)(slotIds[slotIdx], pMechanismList,&ulMechCount);
											if (retVal == CKR_OK)
											{ 
												CK_MECHANISM_INFO mechanismInfo;
												CK_ULONG ulCount = 0;
												printf("Card Mechanisms found :\n",pMechanismList[ulCount]);
												for (ulCount = 0 ;ulCount < ulMechCount;ulCount++)
												{
													retVal = (pFunctions->C_GetMechanismInfo)(slotIds[slotIdx], pMechanismList[ulCount],&mechanismInfo);
													if(retVal == CKR_OK)
													{
														if (mechanismInfo.flags & CKF_SIGN)
															printf("Mechanism 0x%.8x, which supports signing\n",pMechanismList[ulCount]);
														else
															printf("Mechanism 0x%.8x, which doesn't support signing\n",pMechanismList[ulCount]);
													}
												}
											}
										}
										else //malloc pMechanismList failed
										{
											printf("malloc pMechanismList failed\n");
											retVal = CKR_GENERAL_ERROR;
										}
									}
								}//end of for loop
							}
							free(slotIds);
						}
						else //malloc slotIds failed
						{
							printf("malloc failed\n");
							retVal = CKR_GENERAL_ERROR;
						}
					}
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