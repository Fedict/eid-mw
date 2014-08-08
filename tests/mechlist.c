#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>
#include <stdlib.h>

#include "testlib.h"

#define HAS_CKM(ckm, crit) case ckm: { printf("Found " #ckm "\n"); known_mechs++; if(crit) crit_mechs++; } break;

int mechlist(void) {
	CK_RV rv;
	CK_SLOT_ID slot;
	CK_ULONG count=0;
	CK_MECHANISM_TYPE_PTR mechlist;
	int known_mechs = 0;
	int crit_mechs = 0;
	int i, ret;

	rv = C_Initialize(NULL_PTR);
	check_rv;

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		return ret;
	}

	rv = C_GetMechanismList(slot, NULL_PTR, &count);
	mechlist = malloc(sizeof(CK_MECHANISM_TYPE) * count);
	printf("number of mechanisms supported: %lu\n", count);

	rv = C_GetMechanismList(slot, mechlist, &count);
	check_rv;
	if(count == 0) {
		printf("Token supports no mechanisms!\n");
		return TEST_RV_FAIL;
	}

	for(i=0; i<count; i++) {
		switch(mechlist[i]) {
		HAS_CKM(CKM_RSA_PKCS, 1);
		HAS_CKM(CKM_RIPEMD160, 0);
		HAS_CKM(CKM_MD5, 0);
		HAS_CKM(CKM_SHA_1, 1);
		HAS_CKM(CKM_SHA256, 1);
		HAS_CKM(CKM_SHA384, 0);
		HAS_CKM(CKM_SHA512, 0);
		HAS_CKM(CKM_RIPEMD160_RSA_PKCS, 0);
		HAS_CKM(CKM_MD5_RSA_PKCS, 0);
		HAS_CKM(CKM_SHA1_RSA_PKCS, 1);
		HAS_CKM(CKM_SHA256_RSA_PKCS, 1);
		HAS_CKM(CKM_SHA384_RSA_PKCS, 0);
		HAS_CKM(CKM_SHA512_RSA_PKCS, 0);
		default:
			printf("Found unknown mechanism %#08lx\n", mechlist[i]);
			break;
		}
	}

	if(count == known_mechs) {
		printf("INFO: no unknown mechanisms found\n");
	} else {
		if(count > known_mechs) {
			printf("INFO: %lu unknown mechanisms found\n", count - known_mechs);
		} else {
			fprintf(stderr, "E: something weird happened");
			return TEST_RV_FAIL;
		}
	}

	verbose_assert(crit_mechs == 5);

	rv = C_Finalize(NULL_PTR);
	check_rv;

	return TEST_RV_OK;
}
