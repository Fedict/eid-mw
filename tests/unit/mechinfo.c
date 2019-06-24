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

TEST_FUNC(mechinfo) {
	CK_SLOT_ID slot = -1;
	CK_ULONG count=0;
	CK_MECHANISM_INFO info;
	CK_MECHANISM_TYPE_PTR mechlist;
	unsigned int i;
	int ret;
	int found = 0;
	int card_2k = 0;
	ckrv_mod m[] = { { CKR_BUFFER_TOO_SMALL, TEST_RV_OK } };
	ckrv_mod m_mech_inv[] = {
		{ CKR_OK, TEST_RV_FAIL },
		{ CKR_MECHANISM_INVALID, TEST_RV_OK },
	};

	check_rv_long(C_GetMechanismInfo(slot, NULL_PTR, &info), m_p11_noinit);

	check_rv(C_Initialize(NULL_PTR));

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		check_rv(C_Finalize(NULL_PTR));
		return ret;
	}

	check_rv_long(C_GetMechanismList(slot, NULL_PTR, &count), m);
	mechlist = malloc(sizeof(CK_MECHANISM_TYPE) * count);
	printf("number of mechanisms supported: %lu\n", count);

	check_rv(C_GetMechanismList(slot, mechlist, &count));

	check_rv_long(C_GetMechanismInfo(slot, mechlist[1], NULL_PTR), m_p11_badarg);
	check_rv_long(C_GetMechanismInfo(slot, NULL_PTR, &info), m_mech_inv);
	check_rv_long(C_GetMechanismInfo(slot, 0xdeadbeef, &info), m_mech_inv);

	switch(count) {
		case 8:
		case 13:
			printf("Found 1K card\n");
			break;
		case 10:
		case 15:
			printf("Found 2K card\n");
			card_2k = 1;
			break;
		default:
			printf("Dunno what kind of card this is!\n");
			return TEST_RV_FAIL;
	}

	for(i=0; i<count; i++) {
		printf("Mechanism %#08lx (%s):\n", mechlist[i], ckm_to_charp(mechlist[i]));

		info.ulMinKeySize = 0xdeadbeef;
		info.ulMaxKeySize = 0xdeadbeef;
		info.flags = 0xdeadbeef;

		check_rv(C_GetMechanismInfo(slot, mechlist[i], &info));

		printf("minimum key length: %lu\n", info.ulMinKeySize);
		printf("maximum key length: %lu\n", info.ulMaxKeySize);
		printf("flags: %#08lx\n", info.flags);

		verbose_assert(info.ulMinKeySize != 0xdeadbeef);
		verbose_assert(info.ulMaxKeySize != 0xdeadbeef);
		verbose_assert(info.flags != 0xdeadbeef);

		if(mechlist[i] != CKM_SHA1_RSA_PKCS_PSS && mechlist[i] != CKM_SHA256_RSA_PKCS_PSS && mechlist[i] != CKM_ECDSA_SHA256 && mechlist[i] != CKM_ECDSA_SHA384 && mechlist[i] != CKM_ECDSA_SHA512 && mechlist[i] != CKM_ECDSA) {
			verbose_assert(info.ulMinKeySize == info.ulMaxKeySize);
		}
		if(mechlist[i] == CKM_RSA_PKCS) {
			found=1;
			if(card_2k) {
				verbose_assert(info.ulMaxKeySize == 2048);
			} else {
				verbose_assert(info.ulMaxKeySize == 1024);
			}
			verbose_assert(info.flags & CKF_HW);
			verbose_assert(info.flags & CKF_SIGN);
			verbose_assert((info.flags & ~CKF_HW & ~CKF_SIGN) == 0);
		}
		if(mechlist[i] == CKM_ECDSA) {
			found=1;
			verbose_assert(info.flags & CKF_HW);
			verbose_assert(info.flags & CKF_SIGN);
			verbose_assert((info.flags & ~CKF_HW & ~CKF_SIGN) == 0);
		}
	}
	if(!found) {
		printf("Couldn't find PKCS RSA or ECDSA mechanism on this token\n");
		return TEST_RV_FAIL;
	}

	check_rv(C_Finalize(NULL_PTR));

	free(mechlist);

	return TEST_RV_OK;
}
