#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>
#include <stdlib.h>

#include "testlib.h"

#define ADD_STATE(state) statenames[state] = #state

CK_RV notify(CK_SESSION_HANDLE handle, CK_NOTIFICATION event, CK_VOID_PTR ptr) {
	return CKR_OK;
}

TEST_FUNC(sessioninfo) {
	CK_SLOT_ID slot;
	CK_SESSION_HANDLE handle;
	CK_SESSION_INFO info;
	char* statenames[5];
	int ret;

	ADD_STATE(CKS_RO_PUBLIC_SESSION);
	ADD_STATE(CKS_RO_USER_FUNCTIONS);
	ADD_STATE(CKS_RW_PUBLIC_SESSION);
	ADD_STATE(CKS_RW_USER_FUNCTIONS);
	ADD_STATE(CKS_RW_SO_FUNCTIONS);

	check_rv(C_Initialize(NULL_PTR));

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		return ret;
	}

	check_rv(C_OpenSession(slot, CKF_SERIAL_SESSION, NULL_PTR, notify, &handle));

	check_rv(C_GetSessionInfo(handle, &info));

	printf("Slot: %lu\n", info.slotID);
	verbose_assert(info.slotID == slot);
	printf("State: %s\n", statenames[info.state]);
	printf("Flags: %#08lx\n", info.flags);
	verbose_assert(!(info.flags & CKF_RW_SESSION));

	check_rv(C_CloseSession(handle));

	check_rv(C_OpenSession(slot, CKF_SERIAL_SESSION | CKF_RW_SESSION, NULL_PTR, notify, &handle));

	check_rv(C_GetSessionInfo(handle, &info));

	printf("Slot: %lu\n", info.slotID);
	verbose_assert(info.slotID == slot);
	printf("State: %s\n", statenames[info.state]);
	printf("Flags: %#08lx\n", info.flags);
	verbose_assert(info.flags & CKF_RW_SESSION);

	check_rv(C_CloseSession(handle));

	check_rv(C_Finalize(NULL_PTR));

	return TEST_RV_OK;
}
