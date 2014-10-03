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

CK_RV notify_session(CK_SESSION_HANDLE handle, CK_NOTIFICATION event, CK_VOID_PTR ptr) {
	return CKR_OK;
}

TEST_FUNC(sessions) {
	CK_SLOT_ID slot;
	int ret;
	CK_SESSION_HANDLE handle;
	ckrv_mod m[] = { 
		{ CKR_OK, TEST_RV_FAIL },
		{ CKR_SESSION_PARALLEL_NOT_SUPPORTED, TEST_RV_OK },
	};

	check_rv(C_Initialize(NULL_PTR));

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		return ret;
	}

	check_rv_long(C_OpenSession(slot, 0, NULL_PTR, notify_session, &handle), m);

	check_rv(C_OpenSession(slot, CKF_SERIAL_SESSION, NULL_PTR, notify_session, &handle));

	check_rv(C_CloseSession(handle));

	check_rv(C_OpenSession(slot, CKF_SERIAL_SESSION | CKF_RW_SESSION, NULL_PTR, notify_session, &handle));

	check_rv(C_CloseAllSessions(slot));

	check_rv(C_Finalize(NULL_PTR));

	return TEST_RV_OK;
}
