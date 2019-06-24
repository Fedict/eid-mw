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

CK_BYTE digest_results[4][512] = {
	{
		0x2c, 0x26, 0xb4, 0x6b,
		0x68, 0xff, 0xc6, 0x8f,
		0xf9, 0x9b, 0x45, 0x3c,
		0x1d, 0x30, 0x41, 0x34,
		0x13, 0x42, 0x2d, 0x70,
		0x64, 0x83, 0xbf, 0xa0,
		0xf9, 0x8a, 0x5e, 0x88,
		0x62, 0x66, 0xe7, 0xae
	},
	{
		0x98, 0xc1, 0x1f, 0xfd,
		0xfd, 0xd5, 0x40, 0x67,
		0x6b, 0x1a, 0x13, 0x7c,
		0xb1, 0xa2, 0x2b, 0x2a,
		0x70, 0x35, 0x0c, 0x9a,
		0x44, 0x17, 0x1d, 0x6b,
		0x11, 0x80, 0xc6, 0xbe,
		0x5c, 0xbb, 0x2e, 0xe3,
		0xf7, 0x9d, 0x53, 0x2c,
		0x8a, 0x1d, 0xd9, 0xef,
		0x2e, 0x8e, 0x08, 0xe7,
		0x52, 0xa3, 0xba, 0xbb
	},
	{
		0xf7, 0xfb, 0xba, 0x6e,
		0x06, 0x36, 0xf8, 0x90,
		0xe5, 0x6f, 0xbb, 0xf3,
		0x28, 0x3e, 0x52, 0x4c,
		0x6f, 0xa3, 0x20, 0x4a,
		0xe2, 0x98, 0x38, 0x2d,
		0x62, 0x47, 0x41, 0xd0,
		0xdc, 0x66, 0x38, 0x32,
		0x6e, 0x28, 0x2c, 0x41,
		0xbe, 0x5e, 0x42, 0x54,
		0xd8, 0x82, 0x07, 0x72,
		0xc5, 0x51, 0x8a, 0x2c,
		0x5a, 0x8c, 0x0c, 0x7f,
		0x7e, 0xda, 0x19, 0x59,
		0x4a, 0x7e, 0xb5, 0x39,
		0x45, 0x3e, 0x1e, 0xd7
	},
	{
		0x42, 0xcf, 0xa2, 0x11,
		0x01, 0x8e, 0xa4, 0x92,
		0xfd, 0xee, 0x45, 0xac,
		0x63, 0x7b, 0x79, 0x72,
		0xa0, 0xad, 0x68, 0x73
	},
};

CK_MECHANISM_TYPE digest_mechs[4] = {
	CKM_SHA256,
	CKM_SHA384,
	CKM_SHA512,
	CKM_RIPEMD160,
};

TEST_FUNC(digest) {
	int ret;
	CK_SESSION_HANDLE session;
	CK_MECHANISM mech;
	CK_BYTE data[] = { 'f', 'o', 'o' };
	CK_SLOT_ID slot;
	int i;
	CK_ULONG len = 0;
	CK_BYTE_PTR digest;

	check_rv(C_Initialize(NULL_PTR));

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		check_rv(C_Finalize(NULL_PTR));
		return ret;
	}

	check_rv(C_OpenSession(slot, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session));

	for(i=0; i<4; i++) {
		memset(&mech, 0, sizeof(mech));
		mech.mechanism = digest_mechs[i];

		printf("Testing mechanism %s\n", ckm_to_charp(digest_mechs[i]));

		check_rv(C_DigestInit(session, &mech));

		check_rv(C_DigestUpdate(session, data, sizeof(data)));

		check_rv(C_DigestFinal(session, NULL_PTR, &len));

		digest = malloc(len);

		check_rv(C_DigestFinal(session, digest, &len));

		verbose_assert(memcmp(digest, digest_results[i], len) == 0);

		check_rv(C_DigestInit(session, &mech));

		check_rv(C_Digest(session, data, sizeof(data), digest, &len));

		verbose_assert(memcmp(digest, digest_results[i], len) == 0);
	}

	if(have_robot()) {
		CK_ULONG len = 0;
		ckrv_mod m_maybe_rmvd[] = {
			{ CKR_TOKEN_NOT_PRESENT, TEST_RV_OK },
			{ CKR_DEVICE_REMOVED, TEST_RV_OK },
		};
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

		check_rv(C_DigestInit(session, &mech));

		robot_remove_card();

		check_rv_long(C_DigestUpdate(session, data, sizeof(data)), m_maybe_rmvd);

		check_rv_long(C_DigestFinal(session, NULL_PTR, &len), m_is_rmvd);

		if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
			check_rv(C_Finalize(NULL_PTR));
			return ret;
		}

		check_rv_long(C_DigestInit(session, &mech), m_inv);

		check_rv(C_CloseSession(session));
		
		check_rv(C_OpenSession(slot, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session));

		check_rv(C_DigestInit(session, &mech));
	} else {
		printf("Robot not present, skipping manual removal/insertion part of test...\n");
	}

	check_rv(C_Finalize(NULL_PTR));

	return TEST_RV_OK;
}
