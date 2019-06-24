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

#include <eid-util/utftranslate.h>
#include <eid-util/labels.h>

#include "testlib.h"

#define READ_OBJECT "carddata"

TEST_FUNC(readdata_limited) {
	CK_SESSION_HANDLE session;
	CK_OBJECT_HANDLE object;
	CK_ULONG count;
	CK_ULONG type;
	CK_SLOT_ID slot;
	CK_ATTRIBUTE attr[2];
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

	attr[0].type = CKA_CLASS;
	attr[0].pValue = &type;
	type = CKO_DATA;
	attr[0].ulValueLen = sizeof(CK_ULONG);
	attr[1].type = CKA_OBJECT_ID;
	attr[1].pValue = READ_OBJECT;
	attr[1].ulValueLen = strlen(READ_OBJECT);

	check_rv(C_FindObjectsInit(session, attr, 2));

	do {
		char* label_str;
		char* value_str;
		char* objid_str;
		CK_ATTRIBUTE data[3] = {
			{CKA_LABEL, NULL_PTR, 0},
			{CKA_VALUE, NULL_PTR, 0},
			{CKA_OBJECT_ID, NULL_PTR, 0},
		};

		check_rv(C_FindObjects(session, &object, 1, &count));
		if(!count) continue;

		check_rv(C_GetAttributeValue(session, object, data, 3));

		label_str = malloc(data[0].ulValueLen + 1);
		data[0].pValue = label_str;

		value_str = malloc(data[1].ulValueLen + 1);
		data[1].pValue = value_str;

		objid_str = malloc(data[2].ulValueLen + 1);
		data[2].pValue = objid_str;

		check_rv(C_GetAttributeValue(session, object, data, 3));

		label_str[data[0].ulValueLen] = '\0';
		value_str[data[1].ulValueLen] = '\0';
		objid_str[data[2].ulValueLen] = '\0';

		if(is_string(label_str)) {
			printf("Data object with object ID: %s; label: %s; length: %lu\nContents(string):\n%s\n", objid_str, label_str, data[1].ulValueLen, value_str);
		} else {
			printf("Data object with object ID: %s; label: %s; length: %lu\nContents(hexdump):\n", objid_str, label_str, data[1].ulValueLen);
			hex_dump(value_str, data[1].ulValueLen);
		}
		verbose_assert(strcmp(objid_str, READ_OBJECT) == 0);

		free(label_str);
		free(value_str);
		free(objid_str);
	} while(count);

	check_rv(C_CloseSession(session));

	check_rv(C_Finalize(NULL_PTR));

	return TEST_RV_OK;
}
