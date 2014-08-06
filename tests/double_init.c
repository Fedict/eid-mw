#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>

#include "testlib.h"

/*
 * Initializing the library when it is already initialized should return
 * CKR_CRYPTOKI_ALREADY_INITIALIZED
 */

int double_init(void) {
	CK_RV rv;

	rv = C_Initialize(NULL_PTR);
	check_rv;
	rv = C_Initialize(NULL_PTR);
	verbose_assert(rv == CKR_CRYPTOKI_ALREADY_INITIALIZED);
	rv = C_Finalize(NULL_PTR);
	check_rv;

	return TEST_RV_OK;
}
