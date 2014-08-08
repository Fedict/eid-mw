#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>
#include <stdlib.h>

#include "testlib.h"

#define ADD_CKM(ckm) ckms[ckm] = #ckm

int mechinfo(void) {
	CK_RV rv;
	CK_SLOT_ID slot;
	CK_ULONG count=0;
	CK_MECHANISM_INFO info;
	CK_MECHANISM_TYPE_PTR mechlist;
	int i, ret;
	int found = 0;
	int card_2k = 0;
	char* ckms[0x2002];

	ADD_CKM(CKM_RSA_PKCS);
	ADD_CKM(CKM_RIPEMD160);
	ADD_CKM(CKM_MD5);
	ADD_CKM(CKM_SHA_1);
	ADD_CKM(CKM_SHA256);
	ADD_CKM(CKM_SHA384);
	ADD_CKM(CKM_SHA512);
        ADD_CKM(CKM_RIPEMD160_RSA_PKCS);
        ADD_CKM(CKM_MD5_RSA_PKCS);
        ADD_CKM(CKM_SHA1_RSA_PKCS);
        ADD_CKM(CKM_SHA1_RSA_PKCS_PSS);
        ADD_CKM(CKM_SHA256_RSA_PKCS);
        ADD_CKM(CKM_SHA256_RSA_PKCS_PSS);
        ADD_CKM(CKM_SHA384_RSA_PKCS);
        ADD_CKM(CKM_SHA512_RSA_PKCS);

	rv = C_Initialize(NULL_PTR);
	check_rv;

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		return ret;
	}

	rv = C_GetMechanismList(slot, NULL_PTR, &count);
	assert(ckrv_decode(rv, 1, CKR_BUFFER_TOO_SMALL, TEST_RV_OK) == TEST_RV_OK);
	mechlist = malloc(sizeof(CK_MECHANISM_TYPE) * count);
	printf("number of mechanisms supported: %lu\n", count);

	rv = C_GetMechanismList(slot, mechlist, &count);
	check_rv;

	switch(count) {
		case 13:
			printf("Found 1K card\n");
			break;
		case 15:
			printf("Found 2K card\n");
			card_2k = 1;
			break;
	}

	for(i=0; i<count; i++) {
		printf("Mechanism %#08lx (%s):\n", mechlist[i], ckms[mechlist[i]]);

		rv = C_GetMechanismInfo(slot, mechlist[i], &info);
		check_rv;

		printf("minimum key length: %lu\n", info.ulMinKeySize);
		printf("maximum key length: %lu\n", info.ulMaxKeySize);
		printf("flags: %#08lx\n", info.flags);
		if(mechlist[i] != CKM_SHA1_RSA_PKCS_PSS && mechlist[i] != CKM_SHA256_RSA_PKCS_PSS) {
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
	}
	if(!found) {
		printf("Couldn't find PKCS RSA mechanism on this token\n");
		return TEST_RV_FAIL;
	}

	rv = C_Finalize(NULL_PTR);
	check_rv;

	free(mechlist);

	return TEST_RV_OK;
}
