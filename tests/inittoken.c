#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>
#include <stdlib.h>

#include "testlib.h"

int inittoken(void) {
	CK_RV rv;
	CK_SLOT_ID slot;
	int ret;

	rv = C_Initialize(NULL_PTR);
	check_rv;

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		return ret;
	}

	rv = C_InitToken(slot, "1111", 4, "");
	assert(ckrv_decode(rv, 2, CKR_OK, TEST_RV_FAIL, CKR_FUNCTION_NOT_SUPPORTED, TEST_RV_OK) == TEST_RV_OK);

	return TEST_RV_OK;
}
