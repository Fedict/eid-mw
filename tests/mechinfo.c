#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>
#include <stdlib.h>

#include "testlib.h"

int mechinfo(void) {
	CK_RV rv;
	CK_SLOT_ID_PTR slotlist;
	CK_ULONG count=0;
	CK_MECHANISM_INFO info;
	CK_MECHANISM_TYPE_PTR mechlist;
	int i;
	int found = 0;

	rv = C_Initialize(NULL_PTR);
	check_rv;

	rv = C_GetSlotList(CK_TRUE, NULL_PTR, &count);
	assert(ckrv_decode(rv, 1, CKR_BUFFER_TOO_SMALL, TEST_RV_OK) == TEST_RV_OK);
	printf("slots with token found: %lu\n", count);
	if(count == 0) {
		printf("Need at least one token to call C_GetMechanismList\n");
		return TEST_RV_SKIP;
	}

	slotlist = malloc(sizeof(CK_SLOT_ID) * count);

	rv = C_GetSlotList(CK_TRUE, slotlist, &count);
	check_rv;

	rv = C_GetMechanismList(slotlist[0], NULL_PTR, &count);
	mechlist = malloc(sizeof(CK_MECHANISM_TYPE) * count);
	printf("number of mechanisms supported: %lu\n", count);

	rv = C_GetMechanismList(slotlist[0], mechlist, &count);
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

	rv = C_GetMechanismInfo(slotlist[0], CKM_RSA_PKCS, &info);
	check_rv;

	printf("RSA minimum key length: %lu\n", info.ulMinKeySize);
	printf("RSA maximum key length: %lu\n", info.ulMaxKeySize);
	verbose_assert(info.ulMinKeySize == info.ulMaxKeySize);
	verbose_assert(info.ulMinKeySize == 1024 || info.ulMaxKeySize == 2048);
	printf("RSA flags: 0x%08x\n", info.flags);
	verbose_assert(info.flags & CKF_HW);
	verbose_assert(info.flags & CKF_SIGN);
	verbose_assert((info.flags & ~CKF_HW & ~CKF_SIGN) == 0);

	rv = C_Finalize(NULL_PTR);
	check_rv;

	return TEST_RV_OK;
}
