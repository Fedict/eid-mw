#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>
#include <stdlib.h>

#include "testlib.h"

int nonsensible(void) {
	CK_SLOT_ID slot = 0;
	CK_OBJECT_HANDLE object;
	CK_SESSION_HANDLE session;
	CK_ULONG data;
	CK_MECHANISM mech;
	int ret;
	ckrv_mod m[] = {
		{ CKR_OK, TEST_RV_FAIL },
		{ CKR_FUNCTION_NOT_SUPPORTED, TEST_RV_OK },
	};

	check_rv(C_Initialize(NULL_PTR));

	check_rv_long(C_InitToken(slot, "1111", 4, ""),m);

	check_rv_long(C_InitPIN(slot, "1111", 4),m);

	check_rv_long(C_CreateObject(session, NULL_PTR, 0, &object),m);

	check_rv_long(C_CopyObject(session, object, NULL_PTR, 0, &object),m);

	check_rv_long(C_DestroyObject(session, object),m);

	check_rv_long(C_GetObjectSize(session, object, &data),m);

	check_rv_long(C_EncryptInit(session, &mech, object),m);

	check_rv_long(C_Encrypt(session, NULL_PTR, 0, NULL_PTR, &data),m);

	check_rv_long(C_EncryptUpdate(session, NULL_PTR, 0, NULL_PTR, &data),m);

	check_rv_long(C_EncryptFinal(session, NULL_PTR, 0),m);

	check_rv_long(C_DecryptInit(session, &mech, object),m);

	check_rv_long(C_Decrypt(session, NULL_PTR, 0, NULL_PTR, &data),m);

	check_rv_long(C_DecryptUpdate(session, NULL_PTR, 0, NULL_PTR, &data),m);

	check_rv_long(C_DecryptFinal(session, NULL_PTR, 0),m);

	check_rv(C_Finalize(NULL_PTR));

	return TEST_RV_OK;
}
