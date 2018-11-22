#include <unix.h>
#include <pkcs11.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>
#include <malloc.h>

#include <testlib.h>

#include <beid_fuzz.h>

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
	static bool initialized = false;
	CK_SLOT_ID slot;
	CK_ULONG count = 0, type;
	CK_SLOT_ID_PTR slotlist = NULL;
	CK_RV rv;
	CK_SESSION_HANDLE session;
	CK_ATTRIBUTE attrs[2];
	CK_OBJECT_HANDLE object;

	beid_set_fuzz_data(data, size, "3F00DF014031");
	check_rv(C_Initialize(NULL));
	do {
		slotlist = realloc(slotlist, sizeof(CK_SLOT_ID) * count);
	} while((rv = C_GetSlotList(CK_TRUE, slotlist, &count)) == CKR_BUFFER_TOO_SMALL);

	check_rv_late("C_GetSlotList");
	check_rv(C_OpenSession(slotlist[0], CKF_SERIAL_SESSION, NULL, NULL, &session));
	free(slotlist);
	slotlist = NULL;

	attrs[0].type = CKA_CLASS;
	attrs[0].pValue = &type;
	type = CKO_DATA;
	attrs[0].ulValueLen = sizeof(CK_ULONG);
	attrs[1].type = CKA_OBJECT_ID;
	attrs[1].pValue = "id";
	attrs[1].ulValueLen = strlen("id");

	check_rv(C_FindObjectsInit(session, attrs, 2));

	char *label_str = NULL;
	char *value_str = NULL;
	char *objid_str = NULL;

	do {
		char junk[1024];

		CK_ATTRIBUTE data[3] = {
			{CKA_LABEL, NULL_PTR, 0},
			{CKA_VALUE, NULL_PTR, 0},
			{CKA_OBJECT_ID, NULL_PTR, 0},
		};

		check_rv(C_FindObjects(session, &object, 1, &count));
		if(!count) continue;

		free(label_str);
		free(value_str);
		free(objid_str);

		check_rv(C_GetAttributeValue(session, object, data, 3));

		label_str = malloc(data[0].ulValueLen + 1);
		data[0].pValue = label_str;

		value_str = malloc(data[1].ulValueLen + 1);
		data[1].pValue = value_str;

		objid_str = malloc(data[2].ulValueLen + 1);
		data[2].pValue = objid_str;

		check_rv(C_GetAttributeValue(session, object, data, 3));

		snprintf(junk, sizeof(junk), "%s%s%s", label_str, value_str, objid_str);
	} while(count);

	printf("label: %s, objid: %s, value: %s\n", label_str, objid_str, value_str);

	free(label_str);
	free(value_str);
	free(objid_str);

	check_rv(C_CloseSession(session));
	check_rv(C_Finalize(NULL));

	return 0;
}
