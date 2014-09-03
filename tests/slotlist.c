#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>
#include <stdlib.h>

#include "testlib.h"

int slotlist(void) {
	CK_SLOT_ID_PTR list;
	CK_ULONG count=0;
	int i;
	ckrv_mod m[] = { CKR_BUFFER_TOO_SMALL, TEST_RV_OK };

	check_rv(C_Initialize(NULL_PTR));

	check_rv_long(C_GetSlotList(CK_FALSE, NULL_PTR, &count), m);
	printf("slots found: %lu\n", count);

	list = malloc(sizeof(CK_SLOT_ID) * count);
	for(i=0; i<count; i++) {
		list[i] = 0xdeadbeef;
	}

	check_rv_long(C_GetSlotList(CK_FALSE, list, &count), m);
	printf("slots found: %lu\n", count);
	for(i=0; i<count; i++) {
		printf("slot %d: id %lu\n", i, list[i]);
		verbose_assert(list[i] != 0xdeadbeef);
	}

	check_rv(C_Finalize(NULL_PTR));

	free(list);

	return TEST_RV_OK;
}
