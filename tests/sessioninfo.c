#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>
#include <stdlib.h>

#include "testlib.h"

#define ADD_STATE(state) statenames[state] = #state

CK_RV notify(CK_SESSION_HANDLE handle, CK_NOTIFICATION event, CK_VOID_PTR ptr) {
	return CKR_OK;
}

int sessioninfo(void) {
	CK_RV rv;
	CK_SLOT_ID_PTR list;
	CK_ULONG count=0;
	CK_SESSION_HANDLE handle;
	CK_SESSION_INFO info;
	int i;
	char* statenames[5];

	ADD_STATE(CKS_RO_PUBLIC_SESSION);
	ADD_STATE(CKS_RO_USER_FUNCTIONS);
	ADD_STATE(CKS_RW_PUBLIC_SESSION);
	ADD_STATE(CKS_RW_USER_FUNCTIONS);
	ADD_STATE(CKS_RW_SO_FUNCTIONS);

	rv = C_Initialize(NULL_PTR);
	check_rv;

	rv = C_GetSlotList(CK_TRUE, NULL_PTR, &count);
	assert(ckrv_decode(rv, 1, (CK_RV)CKR_BUFFER_TOO_SMALL, (int)TEST_RV_OK) == TEST_RV_OK);
	printf("slots with token found: %lu\n", count);
	if(count == 0) {
		printf("Need at least one token to call C_OpenSession\n");
		return TEST_RV_SKIP;
	}

	list = malloc(sizeof(CK_SLOT_ID) * count);

	rv = C_GetSlotList(CK_TRUE, list, &count);
	assert(ckrv_decode(rv, 1, (CK_RV)CKR_BUFFER_TOO_SMALL, (int)TEST_RV_OK) == TEST_RV_OK);

	rv = C_OpenSession(list[0], CKF_SERIAL_SESSION, NULL_PTR, notify, &handle);
	check_rv;

	rv = C_GetSessionInfo(handle, &info);
	check_rv;

	printf("Slot: %lu\n", info.slotID);
	verbose_assert(info.slotID == list[0]);
	printf("State: %s\n", statenames[info.state]);
	printf("Flags: 0x%08x\n", info.flags);
	verbose_assert(!(info.flags & CKF_RW_SESSION));

	rv = C_CloseSession(handle);
	check_rv;

	rv = C_OpenSession(list[0], CKF_SERIAL_SESSION | CKF_RW_SESSION, NULL_PTR, notify, &handle);
	check_rv;

	rv = C_GetSessionInfo(handle, &info);
	check_rv;

	printf("Slot: %lu\n", info.slotID);
	verbose_assert(info.slotID == list[0]);
	printf("State: %s\n", statenames[info.state]);
	printf("Flags: 0x%08x\n", info.flags);
	verbose_assert(info.flags & CKF_RW_SESSION);

	rv = C_CloseSession(handle);
	check_rv;

	rv = C_Finalize(NULL_PTR);
	check_rv;

	return TEST_RV_OK;
}
