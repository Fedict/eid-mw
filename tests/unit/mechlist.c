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

#define HAS_CKM(ckm, crit_rsa, crit_ecdsa) case ckm: { printf("Found " #ckm "\n"); known_mechs++; if(crit_rsa) rsa_mechs++; if(crit_ecdsa) ecdsa_mechs++; } break;

TEST_FUNC(mechlist) {
	CK_SLOT_ID slot;
	CK_ULONG count=0;
	CK_MECHANISM_TYPE_PTR mechlist;
	CK_ULONG known_mechs = 0;
	int rsa_mechs = 0;
	int ecdsa_mechs = 0;
	unsigned int i;
	int ret;
	int retval = TEST_RV_OK;
	ckrv_mod m_small[] = {
		{ CKR_BUFFER_TOO_SMALL, TEST_RV_OK },
		{ CKR_OK, TEST_RV_FAIL },
	};
	ckrv_mod m_p11_ntoken[] = {
		{ CKR_TOKEN_NOT_PRESENT, TEST_RV_OK },
		{ CKR_FUNCTION_FAILED, TEST_RV_OK },
		{ CKR_OK, TEST_RV_FAIL },
	};

	check_rv_long(C_GetMechanismList(0, NULL_PTR, &count), m_p11_noinit);

	check_rv(C_Initialize(NULL_PTR));

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		check_rv(C_Finalize(NULL_PTR));
		return ret;
	}

	check_rv(C_GetMechanismList(slot, NULL_PTR, &count));
	mechlist = malloc(sizeof(CK_MECHANISM_TYPE) * count);
	printf("number of mechanisms supported: %lu\n", count);

	for(i=0; i<count; i++) {
		mechlist[i] = 0xdeadbeef;
	}

	check_rv(C_GetMechanismList(slot, mechlist, &count));
	if(count == 0) {
		printf("Token supports no mechanisms!\n");
		retval = TEST_RV_FAIL;
		goto done;
	}

	for(i=0; i<count; i++) {
		unsigned long temp = i+1;

		switch(mechlist[i]) {
		HAS_CKM(CKM_RSA_PKCS, 1, 0);
		HAS_CKM(CKM_RIPEMD160, 0, 0);
		HAS_CKM(CKM_MD5, 0, 0);
		HAS_CKM(CKM_SHA_1, 0, 0);
		HAS_CKM(CKM_SHA256, 0, 0);
		HAS_CKM(CKM_SHA384, 0, 0);
		HAS_CKM(CKM_SHA512, 0, 0);
		HAS_CKM(CKM_RIPEMD160_RSA_PKCS, 1, 0);
		HAS_CKM(CKM_MD5_RSA_PKCS, 1, 0);
		HAS_CKM(CKM_SHA1_RSA_PKCS, 1, 0);
		HAS_CKM(CKM_SHA1_RSA_PKCS_PSS, 0, 0);
		HAS_CKM(CKM_SHA256_RSA_PKCS, 1, 0);
		HAS_CKM(CKM_SHA256_RSA_PKCS_PSS, 1, 0);
		HAS_CKM(CKM_SHA384_RSA_PKCS, 1, 0);
		HAS_CKM(CKM_SHA512_RSA_PKCS, 1, 0);
		HAS_CKM(CKM_ECDSA, 0, 1);
		HAS_CKM(CKM_ECDSA_SHA1, 0, 0);
		HAS_CKM(CKM_ECDSA_SHA256, 0, 1);
		HAS_CKM(CKM_ECDSA_SHA384, 0, 1);
		HAS_CKM(CKM_ECDSA_SHA512, 0, 1);
		case 0xdeadbeef:
			printf("E: found uninitialized data\n");
			retval = TEST_RV_FAIL;
			goto done;
		default:
			printf("Found unknown mechanism %#08lx\n", mechlist[i]);
			break;
		}
		if(i<(count-1)) {
			check_rv_long(C_GetMechanismList(slot, mechlist, &temp), m_small);
		} else {
			check_rv(C_GetMechanismList(slot, mechlist, &temp));
		}
	}

	if(count == known_mechs) {
		printf("INFO: no unknown mechanisms found\n");
	} else {
		if(count > known_mechs) {
			printf("INFO: %lu unknown mechanisms found\n", count - known_mechs);
		} else {
			fprintf(stderr, "E: something weird happened");
			retval = TEST_RV_FAIL;
			goto done;
		}
	}

	verbose_assert(count == known_mechs);
	verbose_assert(rsa_mechs == 7 || rsa_mechs == 8 || ecdsa_mechs == 4);

	check_rv_long(C_GetMechanismList(slot+30, mechlist, &count), m_p11_badslot);

	if(have_robot()) {
		robot_remove_card();
		check_rv_long(C_GetMechanismList(slot, mechlist, &count), m_p11_ntoken);
	}

	check_rv(C_Finalize(NULL_PTR));

done:
	free(mechlist);

	return retval;
}
