#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>
#include <stdlib.h>

#include "testlib.h"

CK_RV notify(CK_SESSION_HANDLE handle, CK_NOTIFICATION event, CK_VOID_PTR ptr) {
	return CKR_OK;
}

TEST_FUNC(sessions) {
	CK_SLOT_ID slot;
	int ret;
	CK_SESSION_HANDLE handle;
	ckrv_mod m[] = { 
		{ CKR_OK, TEST_RV_FAIL },
		{ CKR_SESSION_PARALLEL_NOT_SUPPORTED, TEST_RV_OK },
	};

	check_rv(C_Initialize(NULL_PTR));

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		return ret;
	}

	check_rv_long(C_OpenSession(slot, 0, NULL_PTR, notify, &handle), m);

	check_rv(C_OpenSession(slot, CKF_SERIAL_SESSION, NULL_PTR, notify, &handle));

	check_rv(C_CloseSession(handle));

	check_rv(C_OpenSession(slot, CKF_SERIAL_SESSION | CKF_RW_SESSION, NULL_PTR, notify, &handle));

	check_rv(C_CloseAllSessions(slot));

	check_rv(C_Finalize(NULL_PTR));

	return TEST_RV_OK;
}
