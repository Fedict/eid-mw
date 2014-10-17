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

TEST_FUNC(slotevent) {
	CK_SLOT_ID slot;
	int ret;
	CK_RV rv;

	if(!have_robot()) {
		printf("Need ability to remove token to perform this test\n");
		return TEST_RV_SKIP;
	}

	check_rv(C_Initialize(NULL_PTR));

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		return ret;
	}

	printf("Waiting for slot event with CKF_DONT_BLOCK...\n");
	robot_remove_card_delayed();
	do {
		rv = C_WaitForSlotEvent(CKF_DONT_BLOCK, &slot, NULL_PTR);
	} while(rv == CKR_NO_EVENT);
	check_rv_late("C_WaitForSlotEvent");

	printf("Waiting for slot event without flags...\n");
	robot_insert_card_delayed();
	check_rv(C_WaitForSlotEvent(0, &slot, NULL_PTR));

	robot_remove_card();

	printf("Checking for slot for already-removed card\n");
	check_rv(C_WaitForSlotEvent(0, &slot, NULL_PTR));

	robot_insert_card_delayed();
	check_rv(C_WaitForSlotEvent(0, &slot, NULL_PTR));

	check_rv(C_Finalize(NULL_PTR));

	return TEST_RV_OK;
}
