#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>
#include <stdlib.h>

#include "testlib.h"

int slotlist(void) {
	CK_RV rv;
	CK_SLOT_ID_PTR list;
	CK_ULONG count=0;
	int i;

	rv = C_Initialize(NULL_PTR);
	check_rv;

	rv = C_GetSlotList(CK_FALSE, NULL_PTR, &count);
	assert(ckrv_decode(rv, 1, (CK_RV)CKR_BUFFER_TOO_SMALL, (int)TEST_RV_OK) == TEST_RV_OK);
	printf("slots found: %lu\n", count);

	list = malloc(sizeof(CK_SLOT_ID) * count);

	rv = C_GetSlotList(CK_FALSE, list, &count);
	assert(ckrv_decode(rv, 1, (CK_RV)CKR_BUFFER_TOO_SMALL, (int)TEST_RV_OK) == TEST_RV_OK);
	printf("slots found: %lu\n", count);
	for(i=0; i<count; i++) {
		printf("slot %d: id %lu\n", i, list[i]);
	}

	rv = C_Finalize(NULL_PTR);
	check_rv;

	free(list);

	return TEST_RV_OK;
}
