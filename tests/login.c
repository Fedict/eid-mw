#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>
#include <stdlib.h>

#include "testlib.h"

CK_RV notify(CK_SESSION_HANDLE handle, CK_NOTIFICATION event, CK_VOID_PTR ptr) {
	return CKR_OK;
}

int login(void) {
	CK_RV rv;
	CK_SLOT_ID slot;
	CK_ULONG count=0;
	CK_SESSION_HANDLE handle;
	CK_TOKEN_INFO info;
	CK_SESSION_INFO sinfo;
	int i, ret;
	char* statenames[5];

	if(!have_pin()) {
		fprintf(stderr, "cannot test login without a pin code\n");
		return TEST_RV_SKIP;
	}

	rv = C_Initialize(NULL_PTR);
	check_rv;

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		return ret;
	}

	rv = C_OpenSession(slot, CKF_SERIAL_SESSION, NULL_PTR, notify, &handle);
	check_rv;

	rv = C_GetTokenInfo(slot, &info);
	check_rv;

	rv = C_GetSessionInfo(handle, &sinfo);
	printf("State: %lu\n", sinfo.state);
	printf("Flags: 0x%08x\n", sinfo.flags);

	if(info.flags & CKF_PROTECTED_AUTHENTICATION_PATH) {
		if(!is_manual_robot()) {
			fprintf(stderr, "robot cannot enter a pin code on a protected auth path SC reader\n");
			return TEST_RV_SKIP;
		}
	}

	rv = C_Login(handle, CKU_USER, NULL_PTR, 0);
	check_rv;

	rv = C_GetSessionInfo(handle, &sinfo);
	check_rv;

	printf("State: %lu\n", sinfo.state);
	printf("Flags: 0x%08x\n", sinfo.flags);

	rv = C_Logout(handle);
	check_rv;

	rv = C_CloseSession(handle);
	check_rv;

	rv = C_Finalize(NULL_PTR);
	check_rv;

	return TEST_RV_OK;
}
