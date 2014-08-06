#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>

#include "verbose_assert.h"
#include "testlib.h"

/*
 * Initializing the library when it is already initialized should return
 * CKR_CRYPTOKI_ALREADY_INITIALIZED
 */

int main(void) {
	CK_RV rv;

	rv = C_Initialize(NULL_PTR);
	check_ok;
	rv = C_Initialize(NULL_PTR);
	verbose_assert(rv == CKR_CRYPTOKI_ALREADY_INITIALIZED);
	rv = C_Finalize(NULL_PTR);
	check_ok;

	return TEST_OK;
}
