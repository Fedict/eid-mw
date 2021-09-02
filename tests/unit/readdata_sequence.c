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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "testlib.h"

int ReadFirstDataObject(CK_SESSION_HANDLE session);
int ReadAuthCert(CK_SESSION_HANDLE session);

TEST_FUNC(readdata_sequence) {
	CK_SESSION_HANDLE session;
	CK_SLOT_ID slot;
	int ret = TEST_RV_OK;

#ifndef WIN32
	if(!can_confirm()) {
		printf("Need the ability to read privacy-sensitive data from the card for this test...\n");
		return TEST_RV_SKIP;
	}
#endif

	check_rv(C_Initialize(NULL_PTR));

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		check_rv(C_Finalize(NULL_PTR));
		return ret;
	}

	check_rv(C_OpenSession(slot, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session));

//read the CKO_DATA objects
	ret = ReadFirstDataObject(session);
	if(ret != 0)
		return ret;


//read the authentication certificate
	ret = ReadAuthCert(session);
	if(ret != 0)
		return ret;

	check_rv(C_CloseSession(session));


//now try it in reverse order, in a new session
	check_rv(C_OpenSession(slot, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session));


	ret = ReadAuthCert(session);
	if(ret != 0)
		return ret;

	ret = ReadFirstDataObject(session);
	if(ret != 0)
		return ret;

	check_rv(C_CloseSession(session));


	check_rv(C_Finalize(NULL_PTR));

	return TEST_RV_OK;
}

int ReadFirstDataObject(CK_SESSION_HANDLE session)
{
	CK_ULONG count = 0;
	CK_ULONG value = CKO_DATA;
	CK_OBJECT_HANDLE object;
	CK_ATTRIBUTE attr = {CKA_CLASS,&value,sizeof(CK_ULONG)};

	check_rv(C_FindObjectsInit(session, &attr, 1));

	check_rv(C_FindObjects(session, &object, 1, &count));
	verbose_assert(count != 0);

	check_rv(C_FindObjectsFinal(session));
	return TEST_RV_OK;
}

int ReadAuthCert(CK_SESSION_HANDLE session)
{
	CK_ULONG count = 0;
	CK_ULONG value = CKO_CERTIFICATE;
	CK_OBJECT_HANDLE object;
	const char *pAuthentication = "Authentication";
	CK_ATTRIBUTE attr_list[2] = {
		{CKA_CLASS,&value,sizeof(CK_ULONG)},
		{CKA_LABEL,(CK_VOID_PTR)pAuthentication,(CK_ULONG)strlen(pAuthentication)}
	};

	check_rv(C_FindObjectsInit(session, attr_list, 2));

	check_rv(C_FindObjects(session, &object, 1, &count));
	verbose_assert(count != 0);

	check_rv(C_FindObjectsFinal(session));

	return TEST_RV_OK;
}
