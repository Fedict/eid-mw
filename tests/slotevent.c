#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>
#include <stdlib.h>

#include "testlib.h"

int slotevent(void) {
	CK_SLOT_ID slot;
	int ret;

	if(!have_robot()) {
		printf("Need ability to remove token to perform this test\n");
		return TEST_RV_SKIP;
	}

	check_rv(C_Initialize(NULL_PTR));

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		return ret;
	}

	check_rv(C_WaitForSlotEvent(0, &slot, NULL_PTR));

	check_rv(C_Finalize(NULL_PTR));

	return TEST_RV_OK;
}
