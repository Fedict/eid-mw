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

CK_RV notify_nocard(CK_SESSION_HANDLE handle EIDT_UNUSED, CK_NOTIFICATION event EIDT_UNUSED, CK_VOID_PTR ptr EIDT_UNUSED) {
	return CKR_OK;
}

TEST_FUNC(sessions_nocard) {
	CK_SLOT_ID slot;
	int ret;
	CK_SESSION_HANDLE handle;
	ckrv_mod m_no_p[] = {
		{ CKR_OK, TEST_RV_FAIL },
		{ CKR_SESSION_PARALLEL_NOT_SUPPORTED, TEST_RV_OK },
	};
	/* FIXME: this should not be such a long list... */
	ckrv_mod m_no_card[] = {
		{ CKR_OK, TEST_RV_FAIL },
		{ CKR_SLOT_ID_INVALID, TEST_RV_OK },
		{ CKR_DEVICE_ERROR, TEST_RV_OK },
		{ CKR_DEVICE_REMOVED, TEST_RV_OK },
		{ CKR_TOKEN_NOT_PRESENT, TEST_RV_OK },
	};

	if(!have_robot()) {
		printf("Need ability to remove token to perform this test\n");
		return TEST_RV_SKIP;
	}

	check_rv(C_Initialize(NULL_PTR));

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		check_rv(C_Finalize(NULL_PTR));
		return ret;
	}

	check_rv_long(C_OpenSession(slot, 0, NULL_PTR, notify_nocard, &handle), m_no_p);

	check_rv(C_OpenSession(slot, CKF_SERIAL_SESSION, NULL_PTR, notify_nocard, &handle));

	robot_remove_card();

	check_rv(C_CloseSession(handle));

	check_rv_long(C_OpenSession(slot, CKF_SERIAL_SESSION | CKF_RW_SESSION, NULL_PTR, notify_nocard, &handle), m_no_card);

	check_rv(C_Finalize(NULL_PTR));

	return TEST_RV_OK;
}
