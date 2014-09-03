#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>
#include <stdlib.h>

#include "testlib.h"

int mechinfo(void) {
	CK_SLOT_ID slot;
	CK_ULONG count=0;
	CK_MECHANISM_INFO info;
	CK_MECHANISM_TYPE_PTR mechlist;
	int i, ret;
	int found = 0;
	int card_2k = 0;
	ckrv_mod m[] = { CKR_BUFFER_TOO_SMALL, TEST_RV_OK };

	check_rv(C_Initialize(NULL_PTR));

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		return ret;
	}

	check_rv_long(C_GetMechanismList(slot, NULL_PTR, &count), m);
	mechlist = malloc(sizeof(CK_MECHANISM_TYPE) * count);
	printf("number of mechanisms supported: %lu\n", count);

	check_rv(C_GetMechanismList(slot, mechlist, &count));

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

	check_rv(C_Finalize(NULL_PTR));

	free(mechlist);

	return TEST_RV_OK;
}
