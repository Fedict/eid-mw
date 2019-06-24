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

static int notify_called = 0;

static CK_RV notify_login(CK_SESSION_HANDLE handle EIDT_UNUSED, CK_NOTIFICATION event EIDT_UNUSED, CK_VOID_PTR ptr EIDT_UNUSED) {
	printf("INFO: notify called\n");
	notify_called = 1;
	return CKR_OK;
}

TEST_FUNC(login_state) {
	CK_SLOT_ID slot;
	CK_SESSION_HANDLE handle = 0;
	int ret;
	ckrv_mod m_nlogin[] = {
		{ CKR_USER_NOT_LOGGED_IN, TEST_RV_OK },
		{ CKR_OK, TEST_RV_FAIL },
	};
	ckrv_mod m_nsession[] = {
		{ CKR_SESSION_HANDLE_INVALID, TEST_RV_OK },
		{ CKR_OK, TEST_RV_FAIL },
	};

	if(!have_robot()) {
		fprintf(stderr, "cannot run this test without the ability to remove the card\n");
		return TEST_RV_SKIP;
	}

	check_rv(C_Initialize(NULL_PTR));

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		check_rv(C_Finalize(NULL_PTR));
		return ret;
	}

	check_rv_long(C_Logout(handle), m_nsession);

	check_rv(C_OpenSession(slot, CKF_SERIAL_SESSION, NULL_PTR, notify_login, &handle));

	check_rv_long(C_Logout(handle), m_nlogin);

	robot_remove_card();

	check_rv_long(C_Login(handle, CKU_USER, NULL_PTR, 0), m_p11_nocard);

	check_rv_long(C_Logout(handle), m_p11_nocard);

	check_rv(C_CloseSession(handle));

	check_rv(C_Finalize(NULL_PTR));

	return TEST_RV_OK;
}
