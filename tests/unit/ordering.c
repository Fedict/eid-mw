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
#include <stdint.h>
#include <string.h>

#include "testlib.h"

/* Tests that operations fail if run in the wrong order */

/* The actual functions we want to check */
#define CHECK_FUNCS(all, mod) { \
	/* Digest */ \
	if(all) { \
		check_rv_long(C_DigestInit(session, &mech), mod); \
	} \
	check_rv_long(C_Digest(session, data, sizeof(data), result, &len), mod); \
	check_rv_long(C_DigestUpdate(session, data, sizeof(data)), mod); \
	check_rv_long(C_DigestFinal(session, result, &len), mod); \
	/* Sign */ \
	if(all) { \
		check_rv_long(C_SignInit(session, &mech, obj), mod); \
	} \
	check_rv_long(C_Sign(session, data, sizeof(data), result, &len), mod); \
	check_rv_long(C_SignUpdate(session, data, sizeof(data)), mod); \
	check_rv_long(C_SignFinal(session, result, &len), mod); \
	/* Find */ \
	if(all) { \
		check_rv_long(C_FindObjectsInit(session, &attr, 1), mod); \
	} \
	check_rv_long(C_FindObjects(session, &obj, 1, &len), mod); \
	check_rv_long(C_GetAttributeValue(session, obj, &attr, 1), mod); \
	check_rv_long(C_FindObjectsFinal(session), mod); \
}

TEST_FUNC(ordering) {
	CK_SESSION_HANDLE session = 0;
	CK_BYTE data[] = { 'f', 'o', 'o' };
	CK_SLOT_ID slot = UINT32_MAX;
	CK_BYTE_PTR result = NULL;
	CK_ULONG len = 0;
	CK_MECHANISM mech;
	CK_OBJECT_HANDLE obj;
	CK_ATTRIBUTE attr;
	int ret;
	ckrv_mod m_sess_noinit[] = {
		{ CKR_OK, TEST_RV_FAIL },
		{ CKR_SESSION_HANDLE_INVALID, TEST_RV_OK },
	};
	ckrv_mod m_op_noinit[] = {
		{ CKR_OK, TEST_RV_FAIL },
		{ CKR_OPERATION_NOT_INITIALIZED, TEST_RV_OK },
		{ CKR_OBJECT_HANDLE_INVALID, TEST_RV_OK },
	};

	memset(&attr, 0, sizeof attr);
	memset(&obj, 0, sizeof obj);
	memset(&mech, 0, sizeof mech);

	/* Before C_Initialize: everything should return
	 * CKR_CRYPTOKI_NOT_INITIALIZED */

	CHECK_FUNCS(1, m_p11_noinit);

	check_rv(C_Initialize(NULL_PTR));

	/* Before C_OpenSession: everything should return
	 * CKR_SESSION_HANDLE_INVALID */

	CHECK_FUNCS(1, m_sess_noinit);

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		check_rv(C_Finalize(NULL_PTR));
		return ret;
	}

	check_rv(C_OpenSession(slot, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session));

	/* After session initialization: everything (except for *Init
	 * functions) should return CKR_OPERATION_NOT_INITIALIZED */

	CHECK_FUNCS(0, m_op_noinit);

	check_rv(C_Finalize(NULL_PTR));

	return TEST_RV_OK;
}
