#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>

#include "testlib.h"

#define ADD_CKO(cko) ckos[cko] = #cko

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
		CK_ATTRIBUTE data[2] = {
			{CKA_LABEL, NULL_PTR, 0},
			{CKA_VALUE, NULL_PTR, 0},
		};

		rv = C_FindObjects(session, &object, 1, &count);
		check_rv;
		if(!count) continue;

		rv = C_GetAttributeValue(session, object, data, 2);
		check_rv;

		label_str = malloc(data[0].ulValueLen + 1);
		data[0].pValue = label_str;

		value_str = malloc(data[1].ulValueLen + 1);
		data[1].pValue = value_str;

		rv = C_GetAttributeValue(session, object, data, 2);
		check_rv;

		label_str[data[0].ulValueLen] = '\0';
		value_str[data[1].ulValueLen] = '\0';

		printf("Found data object with label %s and value (length: %lu) %s\n", label_str, data[1].ulValueLen, value_str);

		free(label_str);
		free(value_str);
	} while(count);

	rv = C_CloseSession(session);
	check_rv;

	rv = C_Finalize(NULL_PTR);
	check_rv;

	return TEST_RV_OK;
}
