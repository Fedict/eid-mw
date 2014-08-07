#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>
#include <stdlib.h>

#include "testlib.h"

CK_RV notify(CK_SESSION_HANDLE handle, CK_NOTIFICATION event, CK_VOID_PTR ptr) {
	return CKR_OK;
}

int sessions(void) {
	CK_RV rv;
	CK_SLOT_ID_PTR list;
	CK_ULONG count=0;
	int i;
	CK_SESSION_HANDLE handle;

	rv = C_Initialize(NULL_PTR);
	check_rv;

	rv = C_GetSlotList(CK_TRUE, NULL_PTR, &count);
	assert(ckrv_decode(rv, 1, CKR_BUFFER_TOO_SMALL, TEST_RV_OK) == TEST_RV_OK);
	printf("slots with token found: %lu\n", count);
	if(count == 0) {
		printf("Need at least one token to call C_OpenSession\n");
		return TEST_RV_SKIP;
	}

	list = malloc(sizeof(CK_SLOT_ID) * count);

	rv = C_GetSlotList(CK_TRUE, list, &count);
	assert(ckrv_decode(rv, 1, CKR_BUFFER_TOO_SMALL, TEST_RV_OK) == TEST_RV_OK);

	rv = C_OpenSession(list[0], 0, NULL_PTR, notify, &handle);
	assert(ckrv_decode(rv, 2, CKR_OK, TEST_RV_FAIL, CKR_SESSION_PARALLEL_NOT_SUPPORTED, TEST_RV_OK) == TEST_RV_OK);

	rv = C_OpenSession(list[0], CKF_SERIAL_SESSION, NULL_PTR, notify, &handle);
	check_rv;

	rv = C_CloseSession(handle);
	check_rv;

	rv = C_Finalize(NULL_PTR);
	check_rv;

	return TEST_RV_OK;
}
