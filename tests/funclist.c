#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>

#include "verbose_assert.h"
#include "testlib.h"

/*
 * Check that C_GetFunctionList returns a table of function pointers that point to the right location
 */

int main(int argc, char**argv) {
	CK_RV rv;
	CK_FUNCTION_LIST_PTR ptr;

	rv = C_GetFunctionList(&ptr);
	check_ok;
	verbose_assert(ptr->C_Initialize == C_Initialize);

	return TEST_OK;
}
