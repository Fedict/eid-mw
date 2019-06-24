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

TEST_FUNC(slotlist) {
	CK_SLOT_ID_PTR list=(CK_SLOT_ID_PTR)0xdeadbeef;
	CK_ULONG count=0;
	unsigned int i;
	ckrv_mod m[] = { { CKR_BUFFER_TOO_SMALL, TEST_RV_OK } };
	ckrv_mod m_small[] = { { CKR_BUFFER_TOO_SMALL, TEST_RV_OK }, { CKR_OK, TEST_RV_FAIL } };

	check_rv_long(C_GetSlotList(CK_FALSE, NULL_PTR, &count), m_p11_noinit);

	check_rv(C_Initialize(NULL_PTR));

	check_rv_long(C_GetSlotList(CK_FALSE, NULL_PTR, NULL_PTR), m_p11_badarg);

    check_rv_long(C_GetSlotList(CK_FALSE, NULL_PTR, &count), m);
    printf("slots found: %lu\n", count);

	if(count > 0) {
		count=0;
		check_rv_long(C_GetSlotList(CK_FALSE, list, &count), m_small);
	} else {
		printf("No slots found, can't test for CKR_BUFFER_TOO_SMALL...\n");
	}

	list = (CK_SLOT_ID_PTR)malloc(sizeof(CK_SLOT_ID) * count);
	for(i=0; i<count; i++) {
		list[i] = 0xdeadbeef;
	}

	check_rv_long(C_GetSlotList(CK_FALSE, list, &count), m);
	printf("slots found: %lu\n", count);
	for(i=0; i<count; i++) {
		printf("slot %d: id %lu\n", i, list[i]);
		verbose_assert(list[i] != 0xdeadbeef);
	}

	check_rv(C_Finalize(NULL_PTR));

	free(list);

	return TEST_RV_OK;
}
