#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>

#include "testlib.h"

TEST_FUNC(slotinfo) {
	CK_SLOT_ID slot;
	CK_SLOT_INFO info;
	int ret;

	check_rv(C_Initialize(NULL_PTR));

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		return ret;
	}

	check_rv(C_GetSlotInfo(slot, &info));

	verify_null(info.slotDescription, 64, 0, "Slot description:\t'%s'\n");
	verify_null(info.manufacturerID, 32, 0, "Manufacturer ID:\t'%s'\n");

	printf("Hardware version: %d.%d\n", info.hardwareVersion.major, info.hardwareVersion.minor);
	printf("Firmware version: %d.%d\n", info.hardwareVersion.major, info.hardwareVersion.minor);
	printf("Token present: %c; Removable: %c; Hardware slot: %c\n", info.flags & CKF_TOKEN_PRESENT ? 'y' : 'n', info.flags & CKF_REMOVABLE_DEVICE ? 'y' : 'n', info.flags & CKF_HW_SLOT ? 'y' : 'n');

	check_rv(C_Finalize(NULL_PTR));

	return TEST_RV_OK;
}
