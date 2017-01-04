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
#else
#include <unix.h>
#endif
#include <pkcs11.h>
#include <stdio.h>
#include <stdlib.h>

#include "testlib.h"

CK_RV notify_login(CK_SESSION_HANDLE handle, CK_NOTIFICATION event, CK_VOID_PTR ptr) {
	printf("INFO: notification called\n");
	return CKR_OK;
}

TEST_FUNC(slogin) {
	CK_SLOT_ID slot;
	CK_SESSION_HANDLE handle = 0;
	CK_SESSION_INFO sinfo;
	int ret;
	ckrv_mod m[] = {
		{ CKR_PIN_INCORRECT, TEST_RV_SKIP },
		{ CKR_FUNCTION_CANCELED, TEST_RV_SKIP },
	};
	ckrv_mod m_nlogin[] = {
		{ CKR_USER_NOT_LOGGED_IN, TEST_RV_OK },
		{ CKR_OK, TEST_RV_FAIL },
	};
	ckrv_mod m_nsession[] = {
		{ CKR_SESSION_HANDLE_INVALID, TEST_RV_OK },
		{ CKR_OK, TEST_RV_FAIL },
	};

	check_rv_long(C_Login(handle, CKU_USER, NULL_PTR, 0), m_p11_noinit);
	check_rv_long(C_Logout(handle), m_p11_noinit);

	check_rv(C_Initialize(NULL_PTR));

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		check_rv(C_Finalize(NULL_PTR));
		return ret;
	}

	check_rv_long(C_Logout(handle), m_nsession);

	check_rv(C_OpenSession(slot, CKF_SERIAL_SESSION, NULL_PTR, notify_login, &handle));

	check_rv(C_GetSessionInfo(handle, &sinfo));
	printf("State: %lu\n", sinfo.state);
	printf("Flags: %#08lx\n", sinfo.flags);

	if(!have_pin() || !can_enter_pin(slot)) {
		fprintf(stderr, "cannot test login without the ability to enter a pin code\n");
        check_rv(C_Finalize(NULL_PTR));
		return TEST_RV_SKIP;
	}

	check_rv_long(C_Logout(handle), m_nlogin);

	check_rv_long(C_Login(handle, CKU_USER, NULL_PTR, 0), m);

	check_rv(C_GetSessionInfo(handle, &sinfo));

	printf("State: %lu\n", sinfo.state);
	printf("Flags: %#08lx\n", sinfo.flags);

	check_rv(C_Logout(handle));
	check_rv_long(C_Logout(handle), m_nlogin);
	check_rv(C_CloseSession(handle));

	if(have_robot()) {
		check_rv(C_OpenSession(slot, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &handle));
		check_rv_long(C_Login(handle, CKU_USER, NULL_PTR, 0), m);
		robot_remove_card();
		check_rv_long(C_Logout(handle), m_p11_nocard);
		C_CloseSession(handle);
	}

	check_rv(C_Finalize(NULL_PTR));

	return TEST_RV_OK;
}
