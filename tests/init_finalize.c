#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>

#include "testlib.h"

int main(void) {
	CK_RV rv;

	rv = C_Initialize(NULL_PTR);
	check_rv;
	rv = C_Finalize(NULL_PTR);
	check_rv;

	return TEST_OK;
}
