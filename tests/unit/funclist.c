/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2014 FedICT.
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
#ifdef WIN32
#include <win32.h>
#pragma pack(push, cryptoki, 1)
#include "pkcs11.h"
#pragma pack(pop, cryptoki)
#else
#include <unix.h>
#include <pkcs11.h>
#endif
#include <stdio.h>

#include "testlib.h"

/*
 * Check that C_GetFunctionList returns a table of function pointers that point to the right location
 */

TEST_FUNC(funclist) {
	CK_FUNCTION_LIST_PTR ptr;

	check_rv_long(C_GetFunctionList(NULL_PTR), m_p11_badarg);
	check_rv(C_GetFunctionList(&ptr));
	verbose_assert(ptr->C_Initialize == C_Initialize);
	verbose_assert(ptr->C_Finalize == C_Finalize);
	verbose_assert(ptr->C_GetInfo == C_GetInfo);
	verbose_assert(ptr->C_GetFunctionList == C_GetFunctionList);
	verbose_assert(ptr->C_GetSlotList == C_GetSlotList);
	verbose_assert(ptr->C_GetSlotInfo == C_GetSlotInfo);
	verbose_assert(ptr->C_GetTokenInfo == C_GetTokenInfo);
	verbose_assert(ptr->C_GetMechanismList == C_GetMechanismList);
	verbose_assert(ptr->C_GetMechanismInfo == C_GetMechanismInfo);
	verbose_assert(ptr->C_InitToken == C_InitToken);
	verbose_assert(ptr->C_InitPIN == C_InitPIN);
	verbose_assert(ptr->C_SetPIN == C_SetPIN);
	verbose_assert(ptr->C_OpenSession == C_OpenSession);
	verbose_assert(ptr->C_CloseSession == C_CloseSession);
	verbose_assert(ptr->C_CloseAllSessions == C_CloseAllSessions);
	verbose_assert(ptr->C_GetSessionInfo == C_GetSessionInfo);
	verbose_assert(ptr->C_GetOperationState == C_GetOperationState);
	verbose_assert(ptr->C_SetOperationState == C_SetOperationState);
	verbose_assert(ptr->C_Login == C_Login);
	verbose_assert(ptr->C_Logout == C_Logout);
	verbose_assert(ptr->C_CreateObject == C_CreateObject);
	verbose_assert(ptr->C_CopyObject == C_CopyObject);
	verbose_assert(ptr->C_DestroyObject == C_DestroyObject);
	verbose_assert(ptr->C_GetObjectSize == C_GetObjectSize);
	verbose_assert(ptr->C_GetAttributeValue == C_GetAttributeValue);
	verbose_assert(ptr->C_SetAttributeValue == C_SetAttributeValue);
	verbose_assert(ptr->C_FindObjectsInit == C_FindObjectsInit);
	verbose_assert(ptr->C_FindObjects == C_FindObjects);
	verbose_assert(ptr->C_FindObjectsFinal == C_FindObjectsFinal);
	verbose_assert(ptr->C_EncryptInit == C_EncryptInit);
	verbose_assert(ptr->C_Encrypt == C_Encrypt);
	verbose_assert(ptr->C_EncryptUpdate == C_EncryptUpdate);
	verbose_assert(ptr->C_EncryptFinal == C_EncryptFinal);
	verbose_assert(ptr->C_DecryptInit == C_DecryptInit);
	verbose_assert(ptr->C_Decrypt == C_Decrypt);
	verbose_assert(ptr->C_DecryptUpdate == C_DecryptUpdate);
	verbose_assert(ptr->C_DecryptFinal == C_DecryptFinal);
	verbose_assert(ptr->C_DigestInit == C_DigestInit);
	verbose_assert(ptr->C_Digest == C_Digest);
	verbose_assert(ptr->C_DigestUpdate == C_DigestUpdate);
	verbose_assert(ptr->C_DigestKey == C_DigestKey);
	verbose_assert(ptr->C_DigestFinal == C_DigestFinal);
	verbose_assert(ptr->C_SignInit == C_SignInit);
	verbose_assert(ptr->C_Sign == C_Sign);
	verbose_assert(ptr->C_SignUpdate == C_SignUpdate);
	verbose_assert(ptr->C_SignFinal == C_SignFinal);
	verbose_assert(ptr->C_SignRecoverInit == C_SignRecoverInit);
	verbose_assert(ptr->C_SignRecover == C_SignRecover);
	verbose_assert(ptr->C_VerifyInit == C_VerifyInit);
	verbose_assert(ptr->C_Verify == C_Verify);
	verbose_assert(ptr->C_VerifyUpdate == C_VerifyUpdate);
	verbose_assert(ptr->C_VerifyFinal == C_VerifyFinal);
	verbose_assert(ptr->C_VerifyRecoverInit == C_VerifyRecoverInit);
	verbose_assert(ptr->C_VerifyRecover == C_VerifyRecover);
	verbose_assert(ptr->C_DigestEncryptUpdate == C_DigestEncryptUpdate);
	verbose_assert(ptr->C_DecryptDigestUpdate == C_DecryptDigestUpdate);
	verbose_assert(ptr->C_SignEncryptUpdate == C_SignEncryptUpdate);
	verbose_assert(ptr->C_DecryptVerifyUpdate == C_DecryptVerifyUpdate);
	verbose_assert(ptr->C_GenerateKey == C_GenerateKey);
	verbose_assert(ptr->C_GenerateKeyPair == C_GenerateKeyPair);
	verbose_assert(ptr->C_WrapKey == C_WrapKey);
	verbose_assert(ptr->C_UnwrapKey == C_UnwrapKey);
	verbose_assert(ptr->C_DeriveKey == C_DeriveKey);
	verbose_assert(ptr->C_SeedRandom == C_SeedRandom);
	verbose_assert(ptr->C_GenerateRandom == C_GenerateRandom);
	verbose_assert(ptr->C_GetFunctionStatus == C_GetFunctionStatus);
	verbose_assert(ptr->C_CancelFunction == C_CancelFunction);
	verbose_assert(ptr->C_WaitForSlotEvent == C_WaitForSlotEvent);

	return TEST_RV_OK;
}
