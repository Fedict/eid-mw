/* ****************************************************************************

* eID Middleware Project.
* Copyright (C) 2020 BOSA.
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
CK_ULONG beidsdk_challenge(CK_BYTE_PTR textToSign, CK_BYTE len);

int main() {
	CK_ULONG retval = CKR_OK;
	CK_CHAR_PTR copyrightText = TEXT("* eID Middleware Project.							\
																	 * Copyright (C) 2020 BOSA.																		\
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

	//retval = beidsdk_challenge( copyrightText );

	CK_BYTE bytes[48] ={	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
							0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f, 
							0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f};

	CK_BYTE bytesLen = 48;

	retval = beidsdk_challenge(bytes, bytesLen);

	_getch();
}

CK_ULONG beidsdk_challenge(CK_BYTE_PTR textToSign, CK_BYTE len)
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
										CK_ULONG data_class = CKO_PRIVATE_KEY;
										CK_ULONG attribute_len = 2; //the number of attributes in the search template below
										//the searchtemplate that will be used to initialize the search
										CK_ATTRIBUTE attributes[2] = {	{CKA_CLASS,&data_class,sizeof(CK_ULONG)},
										{CKA_LABEL,"Card",(CK_ULONG) strlen("Card")}};
										//prepare the findobjects function to find all objects with attributes 
										//CKA_CLASS set to CKO_DATA and with CKA_LABEL set to "Card"
										retVal = (pFunctions->C_FindObjectsInit)(session_handle, attributes, attribute_len); 
										if (retVal == CKR_OK)
										{
											CK_ULONG ulMaxObjectCount = 1;//we want max one object returned
											CK_ULONG ulObjectCount = 0;	//returns the number of objects found
											CK_OBJECT_HANDLE hKey;
											//retrieve the private key object with label "Card" 
											retVal = (pFunctions->C_FindObjects)(session_handle, &hKey,ulMaxObjectCount,&ulObjectCount); 
											if ( (retVal == CKR_OK) && (ulObjectCount > 0) )
											{
												//terminate the search
												retVal = (pFunctions->C_FindObjectsFinal)(session_handle); 
												if (retVal == CKR_OK)
												{
													//use the CKM_ECDSA mechanism for the challenge
													CK_MECHANISM mechanism = { CKM_ECDSA, NULL_PTR, 0};
													CK_BYTE signature[96];
													CK_ULONG signLength = 96;
													//initialize the signature operation
													retVal = (pFunctions->C_SignInit)(session_handle, &mechanism, hKey); 
													if (retVal == CKR_OK)
													{
														retVal = (pFunctions->C_Sign)(session_handle,textToSign,(CK_ULONG)len,signature,&signLength);
														if(retVal == CKR_OK)
														{
															unsigned long counter = 0;
															printf("The Signature:\n");
															printf("r: ");
															while (counter < signLength)
															{																
																printf("0x%x ", *(signature+counter));
																counter++;
																if (counter == 0x30)
																{
																	printf("\ns: ");
																}
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

