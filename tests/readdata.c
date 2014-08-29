#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "testlib.h"

#define ADD_CKO(cko) ckos[cko] = #cko

void hex_dump(char* data, CK_ULONG length) {
	CK_ULONG i;
	int j;

	for(i=0, j=0; i<length; i++) {
		int8_t d = (int8_t)(data[i]);
		printf("%02hhx ", d);
		j+=3;
		if(!((i + 1) % 5)) {
			printf(" ");
			j += 1;
		}
		if(j >= 80) {
			printf("\n");
			j = 0;
		}
	}
	if(j) {
		printf("\n");
	}
}

#define ADD_LABEL(l, is_string) if(!strcmp(label, l)) return is_string

CK_BBOOL want_string(char* id, char* label) {
	if(!strcmp(id, "carddata")) {
		ADD_LABEL("CARD_DATA", CK_FALSE);
		ADD_LABEL("ATR", CK_FALSE);
		ADD_LABEL("carddata_serialnumber", CK_FALSE);
		ADD_LABEL("carddata_comp_code", CK_FALSE);
		ADD_LABEL("carddata_os_number", CK_FALSE);
		ADD_LABEL("carddata_os_version", CK_FALSE);
		ADD_LABEL("carddata_soft_mask_number", CK_FALSE);
		ADD_LABEL("carddata_soft_mask_version", CK_FALSE);
		ADD_LABEL("carddata_appl_version", CK_FALSE);
		ADD_LABEL("carddata_appl_int_version", CK_FALSE);
		ADD_LABEL("carddata_pkcs15_version", CK_FALSE);
		ADD_LABEL("carddata_appl_lifecycle", CK_FALSE);
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

int readdata() {
	CK_RV rv;
	CK_SESSION_HANDLE session;
	CK_OBJECT_HANDLE object;
	CK_ULONG count;
	CK_ULONG type;
	CK_SLOT_ID slot;
	CK_ATTRIBUTE attr;
	CK_OBJECT_CLASS klass;
	int ret;

	if(!can_confirm()) {
		printf("Need the ability to read privacy-sensitive data from the card for this test...\n");
		return TEST_RV_SKIP;
	}

	rv = C_Initialize(NULL_PTR);
	check_rv;

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		return ret;
	}

	rv = C_OpenSession(slot, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session);
	check_rv;

	attr.type = CKA_CLASS;
	attr.pValue = &type;
	type = CKO_DATA;
	attr.ulValueLen = sizeof(CK_ULONG);

	rv = C_FindObjectsInit(session, &attr, 1);
	check_rv;

	do {
		char* label_str;
		char* value_str;
		char* objid_str;
		CK_ATTRIBUTE data[3] = {
			{CKA_LABEL, NULL_PTR, 0},
			{CKA_VALUE, NULL_PTR, 0},
			{CKA_OBJECT_ID, NULL_PTR, 0},
		};

		rv = C_FindObjects(session, &object, 1, &count);
		check_rv;
		if(!count) continue;

		rv = C_GetAttributeValue(session, object, data, 3);
		check_rv;

		label_str = malloc(data[0].ulValueLen + 1);
		data[0].pValue = label_str;

		value_str = malloc(data[1].ulValueLen + 1);
		data[1].pValue = value_str;

		objid_str = malloc(data[2].ulValueLen + 1);
		data[2].pValue = objid_str;

		rv = C_GetAttributeValue(session, object, data, 3);
		check_rv;

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
	} while(count);

	rv = C_CloseSession(session);
	check_rv;

	rv = C_Finalize(NULL_PTR);
	check_rv;

	return TEST_RV_OK;
}
