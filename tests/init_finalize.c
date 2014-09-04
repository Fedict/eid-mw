#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>

#include "testlib.h"

TEST_FUNC(init_finalize) {
	check_rv(C_Initialize(NULL_PTR));
	check_rv(rv = C_Finalize(NULL_PTR));

	return TEST_RV_OK;
}
