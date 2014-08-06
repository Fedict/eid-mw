#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>
#include <stdlib.h>

#include "testlib.h"

int mechlist(void) {
	CK_RV rv;
	CK_SLOT_ID_PTR slotlist;
	CK_ULONG count=0;
	CK_TOKEN_INFO info;
	CK_MECHANISM_TYPE_PTR mechlist;
	int i;

	rv = C_Initialize(NULL_PTR);
	check_rv;

	rv = C_GetSlotList(CK_TRUE, NULL_PTR, &count);
	verbose_assert((rv == CKR_OK) || (rv == CKR_BUFFER_TOO_SMALL));
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
		printf("mechanism %2d: 0x%08lx\n", i, mechlist[i]);
	}

	rv = C_Finalize(NULL_PTR);
	check_rv;

	return TEST_RV_OK;
}
