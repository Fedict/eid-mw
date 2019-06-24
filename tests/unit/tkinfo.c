/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2014 FedICT.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */
#ifdef WIN32
#include <win32.h>
#pragma pack(push, cryptoki, 1)
#include "pkcs11.h"
#pragma pack(pop, cryptoki)
#else
#include <unix.h>
#include <pkcs11.h>
#endif
#include <stdio.h>

#include "testlib.h"

TEST_FUNC(tkinfo) {
	CK_SLOT_ID slot = 0;
	CK_TOKEN_INFO info;
	int ret;

	check_rv_long(C_GetTokenInfo(slot, &info), m_p11_noinit);

	check_rv(C_Initialize(NULL_PTR));

	check_rv_long(C_GetTokenInfo(slot, NULL_PTR), m_p11_badarg);

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		check_rv(C_Finalize(NULL_PTR));
		return ret;
	}

	check_rv(C_GetTokenInfo(slot, &info));

	verify_null(info.label, 32, 0, "Label:\t'%s'\n");
	verify_null(info.manufacturerID, 32, 0, "Manufacturer ID:\t'%s'\n");
	verify_null(info.model, 16, 0, "Model:\t'%s'\n");
	//verify_null(info.serialNumber, 16, 0, "Serial number:\t'%s'\n");
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
#ifndef NO_DIALOGS
	verbose_assert(!(info.flags & CKF_LOGIN_REQUIRED));
#else
	verbose_assert(info.flags & CKF_LOGIN_REQUIRED);
#endif
	verbose_assert(!(info.flags & CKF_RESTORE_KEY_NOT_NEEDED));
	verbose_assert(!(info.flags & CKF_CLOCK_ON_TOKEN));
	verbose_assert(!(info.flags & CKF_DUAL_CRYPTO_OPERATIONS));
	verbose_assert(info.flags & CKF_TOKEN_INITIALIZED);
	verbose_assert(!(info.flags & CKF_SECONDARY_AUTHENTICATION));

	printf("Hardware version: %d.%d\n", info.hardwareVersion.major, info.hardwareVersion.minor);
	printf("Firmware version: %d.%d\n", info.firmwareVersion.major, info.firmwareVersion.minor);

	check_rv_long(C_GetTokenInfo(slot+30, &info), m_p11_badslot);

	check_rv(C_Finalize(NULL_PTR));

	return TEST_RV_OK;
}
