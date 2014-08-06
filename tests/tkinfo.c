#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>

#include "verbose_assert.h"
#include "testlib.h"

int main(void) {
	CK_RV rv;
	CK_SLOT_ID_PTR list;
	CK_ULONG count=0;
	CK_TOKEN_INFO info;
	int i;

	rv = C_Initialize(NULL_PTR);
	check_ok;

	rv = C_GetSlotList(CK_TRUE, NULL_PTR, &count);
	verbose_assert((rv == CKR_OK) || (rv == CKR_BUFFER_TOO_SMALL));
	printf("slots with token found: %d\n", count);
	if(count == 0) {
		printf("Need at least one token to call C_GetTokenInfo\n");
		return TEST_SKIP;
	}

	list = malloc(sizeof(CK_SLOT_ID) * count);

	rv = C_GetSlotList(CK_TRUE, list, &count);
	verbose_assert((rv == CKR_OK) || (rv == CKR_BUFFER_TOO_SMALL));

	rv = C_GetTokenInfo(list[0], &info);
	check_ok;

	verify_null(info.label, 32, 0, "Label:\t'%s'\n");
	verify_null(info.manufacturerID, 32, 0, "Manufacturer ID:\t'%s'\n");
	verify_null(info.model, 16, 0, "Model:\t'%s'\n");
	verify_null(info.serialNumber, 16, 0, "Serial number:\t'%s'\n");
	//verify_null(info.utcTime, 16, 0, "UTC time on token:\t'%s'\n");

	printf("Max session count: %d; session count: %d\n", info.ulMaxSessionCount, info.ulSessionCount);
	printf("Max RW session count: %d; rw session count: %d\n", info.ulMaxRwSessionCount, info.ulRwSessionCount);
	printf("Max PIN length: %d; min PIN length: %d\n", info.ulMaxPinLen, info.ulMinPinLen);
	printf("Total public memory: %d; free public memory: %d\n", info.ulTotalPublicMemory, info.ulFreePublicMemory);
	printf("Total private memory: %d; free private memory: %d\n", info.ulTotalPrivateMemory, info.ulFreePrivateMemory);

	printf("flags field: %x\n", info.flags);
	
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
	check_ok;

	return TEST_OK;
}
