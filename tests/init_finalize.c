#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>

#include "verbose_assert.h"
#include "testlib.h"

int main(void) {
	CK_RV rv;

	rv = C_Initialize(NULL_PTR);
	check_ok;
	rv = C_Finalize(NULL_PTR);
	check_ok;

	return TEST_OK;
}
