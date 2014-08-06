#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>

#include "verbose_assert.h"
#include "testlib.h"

int main(int argc, char**argv) {
	CK_RV rv;
	CK_INFO info;
	char buf[33];
	int i;
	int nullCount=0;

	rv = C_Initialize(NULL_PTR);
	check_ok;
	rv = C_GetInfo(&info);
	check_ok;
	verbose_assert(info.cryptokiVersion.major == 2);
	verify_null(info.manufacturerID, 32, 0, "Cryptoki manufacturer ID:\t'%s'\n");
	printf("Cryptoki version: %d.%d\n", info.cryptokiVersion.major, info.cryptokiVersion.minor);
	verify_null(info.libraryDescription, 32, 0, "Library description:\t'%s'\n");
	printf("Library version: %d.%d\n", info.libraryVersion.major, info.libraryVersion.minor);
	rv = C_Finalize(NULL_PTR);
	check_ok;

	return TEST_OK;
}
