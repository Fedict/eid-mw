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

CK_ULONG beidsdk_sign(CK_CHAR_PTR textToSign);

int main() {
	CK_ULONG retval = CKR_OK;
	CK_CHAR_PTR copyrightText = TEXT("* eID Middleware Project.							\
																	 * Copyright (C) 2009-2010 FedICT.																		\
																	 *																																		\
																	 * This is free software; you can redistribute it and/or modify it		\
																	 * under the terms of the GNU Lesser General Public License version	\
																	 * 3.0 as published by the Free Software Foundation.									\
																	 *																																		\
																	 * This software is distributed in the hope that it will be useful,	\
																	 * but WITHOUT ANY WARRANTY; without even the implied warranty of		\
																	 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU	\
																	 * Lesser General Public License for more details.										\
																	 *																																		\
																	 * You should have received a copy of the GNU Lesser General Public	\
																	 * License along with this software; if not, see											\
																	 * http://www.gnu.org/licenses/.");

	retval = beidsdk_sign( copyrightText );

	_getch();
}

CK_ULONG beidsdk_sign(CK_CHAR_PTR textToSign) 
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
						CK_SLOT_ID_PTR slotIds = malloc(slot_count * sizeof(CK_SLOT_INFO));
						if(slotIds != NULL)
						{
							// retrieve the list of slots (cardreaders)
							retVal = (pFunctions->C_GetSlotList) (CK_TRUE, slotIds, &slot_count);
							if (retVal == CKR_OK)
							{
								CK_ULONG slotIdx;
								for (slotIdx = 0; slotIdx < slot_count; slotIdx++) 
								{
									CK_SESSION_HANDLE session_handle;
									// open a session
									retVal = (pFunctions->C_OpenSession)(slotIds[slotIdx], CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session_handle);
									if (retVal == CKR_OK)
									{
										CK_ULONG private_key = CKO_PRIVATE_KEY;
										CK_ULONG attribute_len = 2; //the number of attributes in the search template below
										//the searchtemplate that will be used to initialize the search
										CK_ATTRIBUTE attributes[2] = {	{CKA_CLASS,&private_key,sizeof(CK_ULONG)},
										{CKA_LABEL,"Signature",(CK_ULONG) strlen("Signature")}};
										//prepare the findobjects function to find all objects with attributes 
										//CKA_CLASS set to CKO_PRIVATE_KEY and with CKA_LABEL set to Signature
										retVal = (pFunctions->C_FindObjectsInit)(session_handle, attributes, attribute_len); 
										if (retVal == CKR_OK)
										{
											CK_ULONG ulMaxObjectCount = 1;//we want max one object returned
											CK_ULONG ulObjectCount;	//returns the number of objects found
											CK_OBJECT_HANDLE hKey;
											//retrieve the private key with label "signature" 
											retVal = (pFunctions->C_FindObjects)(session_handle, &hKey,ulMaxObjectCount,&ulObjectCount); 
											if (retVal == CKR_OK)
											{
												//terminate the search
												retVal = (pFunctions->C_FindObjectsFinal)(session_handle); 
												if (retVal == CKR_OK)
												{
													//use the CKM_SHA1_RSA_PKCS mechanism for signing
													CK_MECHANISM mechanism = {CKM_SHA1_RSA_PKCS, NULL_PTR, 0};
													CK_BYTE signature[128];
													CK_ULONG signLength = 128;
													//initialize the signature operation
													retVal = (pFunctions->C_SignInit)(session_handle, &mechanism, hKey); 
													if (retVal == CKR_OK)
													{
														retVal = (pFunctions->C_Sign)(session_handle,textToSign,(CK_ULONG) strlen(textToSign),signature,&signLength);
														if(retVal == CKR_OK)
														{
															unsigned long counter = 0;
															printf("The Signature:\n");
															while (counter < signLength)
															{																
																printf("%c", *(signature+counter));
																counter++;
															}
														}
													}
												}
											}
											if (retVal == CKR_OK)
												retVal = (pFunctions->C_FindObjectsFinal)(session_handle); 
											else
												(pFunctions->C_FindObjectsFinal)(session_handle);
										}
										if (retVal == CKR_OK)
											retVal = (pFunctions->C_CloseSession) (session_handle);
										else
											(pFunctions->C_CloseSession) (session_handle);
									}
								}//end of for loop
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

