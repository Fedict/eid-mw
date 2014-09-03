#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>
#include <stdlib.h>

#include "testlib.h"

CK_RV notify(CK_SESSION_HANDLE handle, CK_NOTIFICATION event, CK_VOID_PTR ptr) {
	return CKR_OK;
}

int sessions_nocard(void) {
	CK_SLOT_ID slot;
	int ret;
	CK_SESSION_HANDLE handle;
	ckrv_mod m_no_p[] = {
		{ CKR_OK, TEST_RV_FAIL },
		{ CKR_SESSION_PARALLEL_NOT_SUPPORTED, TEST_RV_OK },
	};
	/* FIXME: this should not be such a long list... */
	ckrv_mod m_no_card[] = {
		{ CKR_OK, TEST_RV_FAIL },
		{ CKR_SLOT_ID_INVALID, TEST_RV_OK },
		{ CKR_DEVICE_ERROR, TEST_RV_OK },
		{ CKR_DEVICE_REMOVED, TEST_RV_OK },
		{ CKR_TOKEN_NOT_PRESENT, TEST_RV_OK },
	};

	if(!have_robot()) {
		printf("Need ability to remove token to perform this test\n");
		return TEST_RV_SKIP;
	}

	check_rv(C_Initialize(NULL_PTR));

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		return ret;
	}

	check_rv_long(C_OpenSession(slot, 0, NULL_PTR, notify, &handle), m_no_p);

	check_rv(C_OpenSession(slot, CKF_SERIAL_SESSION, NULL_PTR, notify, &handle));

	robot_remove_card();

	check_rv(C_CloseSession(handle));

	check_rv_long(C_OpenSession(slot, CKF_SERIAL_SESSION | CKF_RW_SESSION, NULL_PTR, notify, &handle), m_no_card);

	check_rv(C_Finalize(NULL_PTR));

	return TEST_RV_OK;
}
