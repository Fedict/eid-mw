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
#include <stdlib.h>

#include "testlib.h"

#define ADD_STATE(state) statenames[state] = #state

CK_RV notify(CK_SESSION_HANDLE handle EIDT_UNUSED, CK_NOTIFICATION event EIDT_UNUSED, CK_VOID_PTR ptr EIDT_UNUSED) {
	return CKR_OK;
}

TEST_FUNC(sessioninfo) {
	CK_SLOT_ID slot;
	CK_SESSION_HANDLE handle;
	CK_SESSION_INFO info;
	char* statenames[5];
	int ret;

	ADD_STATE(CKS_RO_PUBLIC_SESSION);
	ADD_STATE(CKS_RO_USER_FUNCTIONS);
	ADD_STATE(CKS_RW_PUBLIC_SESSION);
	ADD_STATE(CKS_RW_USER_FUNCTIONS);
	ADD_STATE(CKS_RW_SO_FUNCTIONS);

	check_rv(C_Initialize(NULL_PTR));

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		check_rv(C_Finalize(NULL_PTR));
		return ret;
	}

	check_rv(C_OpenSession(slot, CKF_SERIAL_SESSION, NULL_PTR, notify, &handle));

	check_rv(C_GetSessionInfo(handle, &info));

	printf("Slot: %lu\n", info.slotID);
	verbose_assert(info.slotID == slot);
	printf("State: %s\n", statenames[info.state]);
	printf("Flags: %#08lx\n", info.flags);
	verbose_assert(!(info.flags & CKF_RW_SESSION));

	check_rv(C_CloseSession(handle));

	check_rv(C_OpenSession(slot, CKF_SERIAL_SESSION | CKF_RW_SESSION, NULL_PTR, notify, &handle));

	check_rv(C_GetSessionInfo(handle, &info));

	printf("Slot: %lu\n", info.slotID);
	verbose_assert(info.slotID == slot);
	printf("State: %s\n", statenames[info.state]);
	printf("Flags: %#08lx\n", info.flags);
	verbose_assert(info.flags & CKF_RW_SESSION);

	check_rv(C_CloseSession(handle));

	check_rv(C_Finalize(NULL_PTR));

	return TEST_RV_OK;
}
