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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "testlib.h"

#define ADD_LABEL(l, is_string) if(!strcmp(label, l)) return is_string

CK_BBOOL want_string(char* id, char* label) {
	if(!strcmp(id, "carddata")) {
		ADD_LABEL("CARD_DATA", CK_FALSE);
		ADD_LABEL("ATR", CK_FALSE);
		ADD_LABEL("carddata_serialnumber", CK_FALSE);
		ADD_LABEL("carddata_comp_code", CK_FALSE);
		ADD_LABEL("carddata_os_number", CK_FALSE);
		ADD_LABEL("carddata_os_version", CK_FALSE);
		ADD_LABEL("carddata_glob_os_version", CK_FALSE);
		ADD_LABEL("carddata_soft_mask_number", CK_FALSE);
		ADD_LABEL("carddata_soft_mask_version", CK_FALSE);
		ADD_LABEL("carddata_appl_version", CK_FALSE);
		ADD_LABEL("carddata_appl_int_version", CK_FALSE);
		ADD_LABEL("carddata_pkcs15_version", CK_FALSE);
		ADD_LABEL("carddata_appl_lifecycle", CK_FALSE);
		ADD_LABEL("carddata_pkcs1_support", CK_FALSE);
		ADD_LABEL("carddata_key_exchange_version", CK_FALSE);
	}
	if(!strcmp(id, "id")) {
		ADD_LABEL("DATA_FILE", CK_FALSE);
		ADD_LABEL("card_number", CK_TRUE);
		ADD_LABEL("chip_number", CK_FALSE);
		ADD_LABEL("validity_begin_date", CK_TRUE);
		ADD_LABEL("validity_end_date", CK_TRUE);
		ADD_LABEL("issuing_municipality", CK_TRUE);
		ADD_LABEL("national_number", CK_TRUE);
		ADD_LABEL("surname", CK_TRUE);
		ADD_LABEL("firstnames", CK_TRUE);
		ADD_LABEL("first_letter_of_third_given_name", CK_TRUE);
		ADD_LABEL("nationality", CK_TRUE);
		ADD_LABEL("location_of_birth", CK_TRUE);
		ADD_LABEL("date_of_birth", CK_TRUE);
		ADD_LABEL("gender", CK_TRUE);
		ADD_LABEL("nobility", CK_TRUE);
		ADD_LABEL("document_type", CK_TRUE);
		ADD_LABEL("special_status", CK_TRUE);
		ADD_LABEL("photo_hash", CK_FALSE);
		ADD_LABEL("duplicata", CK_FALSE);
		ADD_LABEL("special_organization", CK_FALSE);
		ADD_LABEL("member_of_family", CK_FALSE);
		ADD_LABEL("date_and_country_of_protection", CK_TRUE);
	}
	if(!strcmp(id, "address")) {
		ADD_LABEL("ADDRESS_FILE", CK_FALSE);
		ADD_LABEL("address_street_and_number", CK_TRUE);
		ADD_LABEL("address_zip", CK_TRUE);
		ADD_LABEL("address_municipality", CK_TRUE);
	}
	if(!strcmp(id, "photo")) {
		ADD_LABEL("PHOTO_FILE", CK_FALSE);
	}
	if(!strcmp(id, "rncert")) {
		ADD_LABEL("CERT_RN_FILE", CK_FALSE);
	}
	if(!strcmp(id, "sign_data_file")) {
		ADD_LABEL("SIGN_DATA_FILE", CK_FALSE);
	}
	if(!strcmp(id, "sign_address_file")) {
		ADD_LABEL("SIGN_ADDRESS_FILE", CK_FALSE);
	}
	printf("E: unknown label %s/%s found\n", id, label);
	exit(TEST_RV_FAIL);
}

TEST_FUNC(readdata) {
	CK_SESSION_HANDLE session;
	CK_OBJECT_HANDLE object;
	CK_ULONG count;
	CK_ULONG type;
	CK_SLOT_ID slot;
	CK_ATTRIBUTE attr;
	int ret;

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

	attr.type = CKA_CLASS;
	attr.pValue = &type;
	type = CKO_DATA;
	attr.ulValueLen = sizeof(CK_ULONG);

	check_rv(C_FindObjectsInit(session, &attr, 1));

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

		if(want_string(objid_str, label_str)) {
			printf("Data object with object ID: %s; label: %s; length: %lu\nContents(string):\n%s\n", objid_str, label_str, data[1].ulValueLen, value_str);
		} else {
			printf("Data object with object ID: %s; label: %s; length: %lu\nContents(hexdump):\n", objid_str, label_str, data[1].ulValueLen);
			hex_dump(value_str, data[1].ulValueLen);
		}

		free(label_str);
		free(value_str);
		free(objid_str);
	} while(count);

	check_rv(C_CloseSession(session));

	check_rv(C_Finalize(NULL_PTR));

	return TEST_RV_OK;
}
