#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>

#include "testlib.h"

/*
 * Initializing the library when it is already initialized should return
 * CKR_CRYPTOKI_ALREADY_INITIALIZED
 */

int double_init(void) {
	ckrv_mod m[] = { CKR_CRYPTOKI_ALREADY_INITIALIZED, TEST_RV_OK, CKR_OK, TEST_RV_FAIL };

	check_rv(C_Initialize(NULL_PTR));
	check_rv_long(C_Initialize(NULL_PTR), m);
	check_rv(C_Finalize(NULL_PTR));

	return TEST_RV_OK;
}
