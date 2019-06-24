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

#include "testlib.h"

TEST_FUNC(wrong_init) {
	ckrv_mod m_duplicate[] = { { CKR_CRYPTOKI_NOT_INITIALIZED, TEST_RV_OK }, { CKR_OK, TEST_RV_FAIL } };

	CK_C_INITIALIZE_ARGS args = {
		.pReserved = (CK_VOID_PTR)0xdeadbeaf
	};

	check_rv_long(C_Initialize(&args), m_p11_badarg);

	args.pReserved = NULL;
	args.CreateMutex = (CK_VOID_PTR)0xdeadbeaf;

	check_rv_long(C_Initialize(&args), m_p11_badarg);

	check_rv_long(C_Finalize(NULL_PTR), m_p11_noinit);

	check_rv(C_Initialize(NULL_PTR));
	check_rv_long(C_Finalize((CK_VOID_PTR)0xdeadbeaf), m_p11_badarg);
	check_rv(C_Finalize(NULL_PTR));
	check_rv_long(C_Finalize(NULL_PTR), m_duplicate);

	return TEST_RV_OK;
}
