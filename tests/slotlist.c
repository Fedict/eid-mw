#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>

#include "verbose_assert.h"
#include "testlib.h"

int main(void) {
	CK_RV rv;
	CK_SLOT_ID_PTR list;
	CK_ULONG count=0;
	int i;

	rv = C_Initialize(NULL_PTR);
	check_ok;

	rv = C_GetSlotList(CK_FALSE, NULL_PTR, &count);
	verbose_assert((rv == CKR_OK) || (rv == CKR_BUFFER_TOO_SMALL));
	printf("slots found: %d\n", count);

	list = malloc(sizeof(CK_SLOT_ID) * count);

	rv = C_GetSlotList(CK_FALSE, list, &count);
	verbose_assert((rv == CKR_OK) || (rv == CKR_BUFFER_TOO_SMALL));
	for(i=0; i<count; i++) {
		printf("slot %d: id %d\n", i, list[i]);
	}

	rv = C_Finalize(NULL_PTR);
	check_ok;

	return TEST_OK;
}
