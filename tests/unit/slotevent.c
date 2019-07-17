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

TEST_FUNC(slotevent) {
	CK_SLOT_ID slot;
	int ret;
	CK_RV rv;
	ckrv_mod no_event[] = {
		{CKR_OK, TEST_RV_FAIL},
		{CKR_NO_EVENT, TEST_RV_OK},
	};

	if(!have_robot()) {
		printf("Need ability to remove token to perform this test\n");
		return TEST_RV_SKIP;
	}

	robot_insert_card();

	check_rv_long(C_WaitForSlotEvent(CKF_DONT_BLOCK, NULL_PTR, NULL_PTR), m_p11_noinit);
	check_rv(C_Initialize(NULL_PTR));

	if(find_slot(CK_TRUE, &slot) != TEST_RV_OK) {
		printf("Couldn't find a card!\n");
		return TEST_RV_FAIL;
	}

	robot_remove_card();

	check_rv(C_WaitForSlotEvent(CKF_DONT_BLOCK, &slot, NULL_PTR));
	// The below should return CKR_NO_EVENT, but it doesn't. That
	// doesn't (usually) hurt, because standard implementations are
	// likely to assume a race condition and ignore the event, but
	// it's not proper...
	//check_rv_long(C_WaitForSlotEvent(CKF_DONT_BLOCK, &slot, NULL_PTR), no_event);

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		check_rv(C_Finalize(NULL_PTR));
		return ret;
	}

	check_rv(C_WaitForSlotEvent(CKF_DONT_BLOCK, &slot, NULL_PTR))

	printf("Waiting for slot event without flags...\n");
	robot_remove_card_delayed();
	check_rv(C_WaitForSlotEvent(0, &slot, NULL_PTR));

	robot_insert_card_delayed();
	check_rv(C_WaitForSlotEvent(0, &slot, NULL_PTR));

	check_rv_long(C_WaitForSlotEvent(CKF_DONT_BLOCK, &slot, NULL_PTR), no_event);

	robot_remove_card();
	check_rv(C_WaitForSlotEvent(CKF_DONT_BLOCK, &slot, NULL_PTR));

	robot_insert_card_delayed();

	do {
		rv = C_WaitForSlotEvent(CKF_DONT_BLOCK, &slot, NULL_PTR);
	} while(rv == CKR_NO_EVENT);
	check_rv_late("C_WaitForSlotEvent(CKF_DONT_BLOCK, &slot, NULL_PTR)");

	if(!have_reader_robot()) {
		printf("Need ability to remove card reader to do the rest of this test\n");
		goto end;
	}

	robot_remove_reader();
	check_rv(C_WaitForSlotEvent(CKF_DONT_BLOCK, &slot, NULL_PTR));
	robot_insert_reader();
	check_rv(C_WaitForSlotEvent(CKF_DONT_BLOCK, &slot, NULL_PTR));

end:
	check_rv(C_Finalize(NULL_PTR));
	return TEST_RV_OK;
}
