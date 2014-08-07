#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>

#include "testlib.h"

int slotinfo(void) {
	CK_RV rv;
	CK_SLOT_ID slot;
	CK_ULONG count=0;
	CK_SLOT_INFO info;
	int i, ret;

	rv = C_Initialize(NULL_PTR);
	check_rv;

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		return ret;
	}

	rv = C_GetSlotInfo(slot, &info);
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
