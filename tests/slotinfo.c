#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>

#include "testlib.h"

int slotinfo(void) {
	CK_RV rv;
	CK_SLOT_ID_PTR list;
	CK_ULONG count=0;
	CK_SLOT_INFO info;
	int i;

	rv = C_Initialize(NULL_PTR);
	check_rv;

	rv = C_GetSlotList(CK_FALSE, NULL_PTR, &count);
	assert(ckrv_decode(rv, 1, (CK_RV)CKR_BUFFER_TOO_SMALL, (int)TEST_RV_OK) == TEST_RV_OK);
	printf("slots found: %d\n", count);
	if(count == 0) {
		printf("Need at least one slot to call C_GetSlotInfo");
		return TEST_RV_SKIP;
	}

	list = malloc(sizeof(CK_SLOT_ID) * count);

	rv = C_GetSlotList(CK_FALSE, list, &count);
	assert(ckrv_decode(rv, 1, (CK_RV)CKR_BUFFER_TOO_SMALL, (int)TEST_RV_OK) == TEST_RV_OK);

	rv = C_GetSlotInfo(list[0], &info);
	check_rv;

	verify_null(info.slotDescription, 64, 0, "Slot description:\t'%s'\n");
	verify_null(info.manufacturerID, 32, 0, "Manufacturer ID:\t'%s'\n");

	printf("Hardware version: %d.%d\n", info.hardwareVersion.major, info.hardwareVersion.minor);
	printf("Firmware version: %d.%d\n", info.hardwareVersion.major, info.hardwareVersion.minor);
	printf("Token present: %c; Removable: %c; Hardware slot: %c\n", info.flags & CKF_TOKEN_PRESENT ? 'y' : 'n', info.flags & CKF_REMOVABLE_DEVICE ? 'y' : 'n', info.flags & CKF_HW_SLOT ? 'y' : 'n');

	rv = C_Finalize(NULL_PTR);
	check_rv;

	return TEST_RV_OK;
}
