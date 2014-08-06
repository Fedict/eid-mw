#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>
#include <stdlib.h>

#include "testlib.h"

int slotevent(void) {
	CK_RV rv;
	CK_SLOT_ID_PTR list = NULL;
	CK_ULONG count=0;
	CK_TOKEN_INFO info;
	int i;

	if(!have_robot()) {
		printf("Need ability to remove token to perform this test\n");
		return TEST_RV_SKIP;
	}

	robot_insert_card();

	rv = C_Initialize(NULL_PTR);
	check_rv;

	rv = C_GetSlotList(CK_TRUE, NULL_PTR, &count);
	verbose_assert((rv == CKR_OK) || (rv == CKR_BUFFER_TOO_SMALL));
	printf("slots with token found: %lu\n", count);
	if(count == 0) {
		printf("Need at least one token to call C_WaitForSlotEvent\n");
		return TEST_RV_FAIL;
	}

	while(rv == CKR_BUFFER_TOO_SMALL) {
		list = realloc(list, sizeof(CK_SLOT_ID) * count);

		rv = C_GetSlotList(CK_TRUE, list, &count);
	}
	check_rv;

	if(count > 1) {
		printf("INFO: multiple slots found, using slot %lu\n", list[0]);
	}

	rv = C_WaitForSlotEvent(0, list, NULL_PTR);
	check_rv;

	rv = C_Finalize(NULL_PTR);
	check_rv;

	return TEST_RV_OK;
}
