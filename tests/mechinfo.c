#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>
#include <stdlib.h>

#include "testlib.h"

int mechinfo(void) {
	CK_RV rv;
	CK_SLOT_ID slot;
	CK_ULONG count=0;
	CK_MECHANISM_INFO info;
	CK_MECHANISM_TYPE_PTR mechlist;
	int i, ret;
	int found = 0;

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

	for(i=0; i<count; i++) {
		if(mechlist[i] == CKM_RSA_PKCS) {
			found=1;
		}
	}
	if(!found) {
		printf("Couldn't find PKCS RSA mechanism on this token\n");
		return TEST_RV_SKIP;
	}

	rv = C_GetMechanismInfo(slot, CKM_RSA_PKCS, &info);
	check_rv;

	printf("RSA minimum key length: %lu\n", info.ulMinKeySize);
	printf("RSA maximum key length: %lu\n", info.ulMaxKeySize);
	verbose_assert(info.ulMinKeySize == info.ulMaxKeySize);
	verbose_assert(info.ulMinKeySize == 1024 || info.ulMaxKeySize == 2048);
	printf("RSA flags: %#08lx\n", info.flags);
	verbose_assert(info.flags & CKF_HW);
	verbose_assert(info.flags & CKF_SIGN);
	verbose_assert((info.flags & ~CKF_HW & ~CKF_SIGN) == 0);

	rv = C_Finalize(NULL_PTR);
	check_rv;

	return TEST_RV_OK;
}
