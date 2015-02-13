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

TEST_FUNC(slotlist) {
	CK_SLOT_ID_PTR list;
	CK_ULONG count=0;
	int i;
	ckrv_mod m[] = { { CKR_BUFFER_TOO_SMALL, TEST_RV_OK } };

	check_rv(C_Initialize(NULL_PTR));

	check_rv_long(C_GetSlotList(CK_FALSE, NULL_PTR, &count), m);
	printf("slots found: %lu\n", count);

	list = malloc(sizeof(CK_SLOT_ID) * count);
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
