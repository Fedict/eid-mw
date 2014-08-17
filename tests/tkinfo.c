#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>

#include "testlib.h"

int tkinfo(void) {
	CK_RV rv;
	CK_SLOT_ID slot;
	CK_TOKEN_INFO info;
	int ret;

	rv = C_Initialize(NULL_PTR);
	check_rv;

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		return ret;
	}

	rv = C_GetTokenInfo(slot, &info);
	check_rv;

	verify_null(info.label, 32, 0, "Label:\t'%s'\n");
	verify_null(info.manufacturerID, 32, 0, "Manufacturer ID:\t'%s'\n");
	verify_null(info.model, 16, 0, "Model:\t'%s'\n");
	verify_null(info.serialNumber, 16, 0, "Serial number:\t'%s'\n");
	//verify_null(info.utcTime, 16, 0, "UTC time on token:\t'%s'\n");

	printf("Max session count: %lu; session count: %lu\n", info.ulMaxSessionCount, info.ulSessionCount);
	printf("Max RW session count: %lu; rw session count: %lu\n", info.ulMaxRwSessionCount, info.ulRwSessionCount);
	printf("Max PIN length: %lu; min PIN length: %lu\n", info.ulMaxPinLen, info.ulMinPinLen);

	verbose_assert(info.ulTotalPublicMemory == CK_UNAVAILABLE_INFORMATION);
	verbose_assert(info.ulFreePublicMemory == CK_UNAVAILABLE_INFORMATION);
	verbose_assert(info.ulTotalPrivateMemory == CK_UNAVAILABLE_INFORMATION);
	verbose_assert(info.ulFreePrivateMemory == CK_UNAVAILABLE_INFORMATION);

	printf("flags field: %#08lx\n", info.flags);
	
	verbose_assert(!(info.flags & CKF_RNG));
	verbose_assert(info.flags & CKF_WRITE_PROTECTED);
	verbose_assert(!(info.flags & CKF_LOGIN_REQUIRED));
	verbose_assert(!(info.flags & CKF_RESTORE_KEY_NOT_NEEDED));
	verbose_assert(!(info.flags & CKF_CLOCK_ON_TOKEN));
	verbose_assert(!(info.flags & CKF_DUAL_CRYPTO_OPERATIONS));
	verbose_assert(info.flags & CKF_TOKEN_INITIALIZED);
	verbose_assert(!(info.flags & CKF_SECONDARY_AUTHENTICATION));

	printf("Hardware version: %d.%d\n", info.hardwareVersion.major, info.hardwareVersion.minor);
	printf("Firmware version: %d.%d\n", info.hardwareVersion.major, info.hardwareVersion.minor);

	rv = C_Finalize(NULL_PTR);
	check_rv;

	return TEST_RV_OK;
}
