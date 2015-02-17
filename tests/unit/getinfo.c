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

#include "testlib.h"

TEST_FUNC(getinfo) {
	CK_INFO info;
	ckrv_mod m[] = { { CKR_ARGUMENTS_BAD, TEST_RV_OK }, { CKR_OK, TEST_RV_FAIL } };

	check_rv(C_Initialize(NULL_PTR));
	check_rv_long(C_GetInfo(NULL_PTR), m);
	check_rv(C_GetInfo(&info));
	verbose_assert(info.cryptokiVersion.major == 2);
	verify_null(info.manufacturerID, 32, 0, "Cryptoki manufacturer ID:\t'%s'\n");
	printf("Cryptoki version: %d.%d\n", info.cryptokiVersion.major, info.cryptokiVersion.minor);
	verify_null(info.libraryDescription, 32, 0, "Library description:\t'%s'\n");
	printf("Library version: %d.%d\n", info.libraryVersion.major, info.libraryVersion.minor);
	check_rv(C_Finalize(NULL_PTR));

	return TEST_RV_OK;
}
