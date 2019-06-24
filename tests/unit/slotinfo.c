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

TEST_FUNC(slotinfo) {
	CK_SLOT_ID slot = 0;
	CK_SLOT_INFO info;
	int ret;

	check_rv_long(C_GetSlotInfo(slot, &info), m_p11_noinit);

	check_rv(C_Initialize(NULL_PTR));

	check_rv_long(C_GetSlotInfo(slot, NULL_PTR), m_p11_badarg);

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		check_rv(C_Finalize(NULL_PTR));
		return ret;
	}

	check_rv_long(C_GetSlotInfo(slot+30, &info), m_p11_badslot);
	check_rv(C_GetSlotInfo(slot, &info));

	verify_null(info.slotDescription, 64, 0, "Slot description:\t'%s'\n");
	verify_null(info.manufacturerID, 32, 0, "Manufacturer ID:\t'%s'\n");

	printf("Hardware version: %d.%d\n", info.hardwareVersion.major, info.hardwareVersion.minor);
	printf("Firmware version: %d.%d\n", info.hardwareVersion.major, info.hardwareVersion.minor);
	printf("Token present: %c; Removable: %c; Hardware slot: %c\n", info.flags & CKF_TOKEN_PRESENT ? 'y' : 'n', info.flags & CKF_REMOVABLE_DEVICE ? 'y' : 'n', info.flags & CKF_HW_SLOT ? 'y' : 'n');

	check_rv(C_Finalize(NULL_PTR));

	return TEST_RV_OK;
}
