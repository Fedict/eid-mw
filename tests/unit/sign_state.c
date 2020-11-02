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
#include <string.h>
#include <stdlib.h>

#include "testlib.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

TEST_FUNC(sign_state) {
	int ret;
	CK_SESSION_HANDLE session;
	CK_MECHANISM mech;
	CK_BYTE data[] = { 'f', 'o', 'o' };
	CK_SLOT_ID slot;
	CK_ULONG sig_len, type, count;
	CK_OBJECT_HANDLE privatekey;
	CK_ATTRIBUTE attr[2];
	CK_KEY_TYPE keytype;
	ckrv_mod m_is_rmvd[] = {
		{ CKR_OK, TEST_RV_FAIL },
		{ CKR_TOKEN_NOT_PRESENT, TEST_RV_OK },
		{ CKR_DEVICE_REMOVED, TEST_RV_OK },
	};
	ckrv_mod m_inv[] = {
		{ CKR_OK, TEST_RV_FAIL },
		{ CKR_DEVICE_REMOVED, TEST_RV_OK },
		{ CKR_SESSION_HANDLE_INVALID, TEST_RV_OK },
	};
	ckrv_mod m_objinv[] = {
		{ CKR_OK, TEST_RV_FAIL },
		{ CKR_KEY_HANDLE_INVALID, TEST_RV_OK },
	};
	ckrv_mod m_n_init[] = {
		{ CKR_OK, TEST_RV_FAIL },
		{ CKR_OPERATION_NOT_INITIALIZED, TEST_RV_OK },
		
	};


	if(!have_robot()) {
		printf("Robot not present, can't do this test...\n");
		return TEST_RV_SKIP;
	}
	check_rv_long(C_Sign(NULL, data, sizeof(data), NULL, &sig_len), m_p11_noinit);

	check_rv(C_Initialize(NULL_PTR));

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		check_rv(C_Finalize(NULL_PTR));
		return ret;
	}

	check_rv(C_OpenSession(slot, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session));

	attr[0].type = CKA_CLASS;
	attr[0].pValue = &type;
	type = CKO_PRIVATE_KEY;
	attr[0].ulValueLen = sizeof(CK_ULONG);

	attr[1].type = CKA_LABEL;
	attr[1].pValue = "Signature";
	attr[1].ulValueLen = strlen("Signature");

	check_rv(C_FindObjectsInit(session, attr, 2));
	check_rv(C_FindObjects(session, &privatekey, 1, &count));
	verbose_assert(count == 1 || count == 0);
	check_rv(C_FindObjectsFinal(session));

	if(count == 0) {
		fprintf(stderr, "Cannot test signature state on a card without a signature key\n");
		return TEST_RV_SKIP;
	}

	attr[0].type = CKA_KEY_TYPE;
	attr[0].pValue = &keytype;
	attr[0].ulValueLen = sizeof(keytype);

	check_rv(C_GetAttributeValue(session, privatekey, attr, 1));
	verbose_assert(keytype == CKK_RSA || keytype == CKK_EC);

	if(keytype == CKK_RSA) {
		mech.mechanism = CKM_SHA256_RSA_PKCS;
	} else {
		mech.mechanism = CKM_ECDSA_SHA256;
	}
	
	check_rv_long(C_Sign(session, data, sizeof(data), NULL, &sig_len), m_n_init);
	
	check_rv(C_SignInit(session, &mech, privatekey));

	robot_remove_card();

	check_rv_long(C_Sign(session, data, sizeof(data), NULL, &sig_len), m_is_rmvd);

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		check_rv(C_Finalize(NULL_PTR));
		return ret;
	}

	check_rv_long(C_SignInit(session, &mech, privatekey), m_inv);

	check_rv(C_CloseSession(session));

	check_rv(C_OpenSession(slot, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session));

	check_rv_long(C_SignInit(session, &mech, privatekey), m_objinv);

	attr[0].type = CKA_CLASS;
	attr[0].pValue = &type;
	type = CKO_PRIVATE_KEY;
	attr[0].ulValueLen = sizeof(CK_ULONG);

	attr[1].type = CKA_LABEL;
	attr[1].pValue = "Signature";
	attr[1].ulValueLen = strlen("Signature");

	check_rv(C_FindObjectsInit(session, attr, 2));
	check_rv(C_FindObjects(session, &privatekey, 1, &count));
	verbose_assert(count == 1 || count == 0);

	check_rv(C_SignInit(session, &mech, privatekey));

	check_rv(C_Sign(session, data, sizeof(data), NULL, &sig_len));

	robot_remove_card();
	robot_insert_card();

	check_rv_long(C_Sign(session, data, sizeof(data), NULL, &sig_len), m_is_rmvd);

	if (C_FindObjectsFinal(session)!=CKR_OK && C_FindObjectsFinal(session)!=CKR_DEVICE_REMOVED){
		printf("not missing device or not ok\n");
		return TEST_RV_FAIL; 
	}
	
	check_rv(C_Finalize(NULL_PTR));

	return TEST_RV_OK;
}
