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
#include <stdlib.h>

#include "testlib.h"

TEST_FUNC(nonsensible) {
	CK_SLOT_ID slot = 0;
	CK_OBJECT_HANDLE object;
	CK_SESSION_HANDLE session = 0;
	CK_ULONG data;
	CK_MECHANISM mech;
	CK_ATTRIBUTE attr;

	ckrv_mod m[] = {
		{ CKR_OK, TEST_RV_FAIL },
		{ CKR_FUNCTION_NOT_SUPPORTED, TEST_RV_OK },
	};

	check_rv(C_Initialize(NULL_PTR));

	check_rv_long(C_InitToken(slot, (CK_UTF8CHAR_PTR)"1111", 4, (CK_UTF8CHAR_PTR)""),m);
	check_rv_long(C_InitPIN(slot, (CK_UTF8CHAR_PTR)"1111", 4),m);
	check_rv_long(C_CreateObject(session, NULL_PTR, 0, &object),m);
	check_rv_long(C_CopyObject(session, object, NULL_PTR, 0, &object),m);
	check_rv_long(C_DestroyObject(session, object),m);
	check_rv_long(C_GetObjectSize(session, object, &data),m);
	check_rv_long(C_EncryptInit(session, &mech, object),m);
	check_rv_long(C_Encrypt(session, NULL_PTR, 0, NULL_PTR, &data),m);
	check_rv_long(C_EncryptUpdate(session, NULL_PTR, 0, NULL_PTR, &data),m);
	check_rv_long(C_EncryptFinal(session, NULL_PTR, 0),m);
	check_rv_long(C_DecryptInit(session, &mech, object),m);
	check_rv_long(C_Decrypt(session, NULL_PTR, 0, NULL_PTR, &data),m);
	check_rv_long(C_DecryptUpdate(session, NULL_PTR, 0, NULL_PTR, &data),m);
	check_rv_long(C_DecryptFinal(session, NULL_PTR, 0),m);
	check_rv_long(C_DigestEncryptUpdate(session, NULL_PTR, 0, NULL_PTR, &data),m);
	check_rv_long(C_DecryptDigestUpdate(session, NULL_PTR, 0, NULL_PTR, &data),m);
	check_rv_long(C_SignEncryptUpdate(session, NULL_PTR, 0, NULL_PTR, &data),m);
	check_rv_long(C_DecryptVerifyUpdate(session, NULL_PTR, 0, NULL_PTR, &data),m);
	check_rv_long(C_GetOperationState(session, NULL_PTR, &data), m);
	check_rv_long(C_SetOperationState(session, NULL_PTR, data, 0, 0), m);
	check_rv_long(C_SetAttributeValue(session, object, &attr, 1), m);
	check_rv_long(C_GenerateKey(session, &mech, &attr, 1, &object), m);
	check_rv_long(C_GenerateKeyPair(session, &mech, &attr, 1, &attr, 1, &object, &object), m);
	check_rv_long(C_WrapKey(session, &mech, object, object, NULL_PTR, &data), m);
	check_rv_long(C_UnwrapKey(session, &mech, object, NULL_PTR, data, &attr, data, &object), m);
	check_rv_long(C_DeriveKey(session, &mech, object, &attr, data, &object), m);
	check_rv_long(C_SeedRandom(session, NULL_PTR, data), m);
	check_rv_long(C_GenerateRandom(session, NULL_PTR, data), m);
	check_rv_long(C_GetFunctionStatus(session), m);
	check_rv_long(C_CancelFunction(session), m);
	check_rv_long(C_DigestKey(session, object), m);
	check_rv_long(C_SignRecoverInit(session, &mech, object), m);
	check_rv_long(C_SignRecover(session, NULL_PTR, data, NULL_PTR, &data), m);
	check_rv_long(C_VerifyInit(session, &mech, object), m);
	check_rv_long(C_Verify(session, NULL_PTR, data, NULL_PTR, data), m);
	check_rv_long(C_VerifyUpdate(session, NULL_PTR, data), m);
	check_rv_long(C_VerifyFinal(session, NULL_PTR, data), m);
	check_rv_long(C_VerifyRecoverInit(session, &mech, object), m);
	check_rv_long(C_VerifyRecover(session, NULL_PTR, data, NULL_PTR, &data), m);

	check_rv(C_Finalize(NULL_PTR));

	return TEST_RV_OK;
}
