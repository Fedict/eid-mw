#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>

#include "testlib.h"

int getinfo(int argc, char**argv) {
	CK_RV rv;
	CK_INFO info;

	rv = C_Initialize(NULL_PTR);
	check_rv;
	rv = C_GetInfo(&info);
	check_rv;
	verbose_assert(info.cryptokiVersion.major == 2);
	verify_null(info.manufacturerID, 32, 0, "Cryptoki manufacturer ID:\t'%s'\n");
	printf("Cryptoki version: %d.%d\n", info.cryptokiVersion.major, info.cryptokiVersion.minor);
	verify_null(info.libraryDescription, 32, 0, "Library description:\t'%s'\n");
	printf("Library version: %d.%d\n", info.libraryVersion.major, info.libraryVersion.minor);
	rv = C_Finalize(NULL_PTR);
	check_rv;

	return TEST_RV_OK;
}
