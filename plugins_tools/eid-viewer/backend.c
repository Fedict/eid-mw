#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <unix.h>
#include <pkcs11.h>

#include "oslayer.h"
#include "labels.h"
#include "cache.h"
#include "conversions.h"

typedef struct {
	CK_RV rv;
	int res;
} ckrv_mod;

#define EIDV_RV_OK 0
#define EIDV_RV_FAIL -1

ckrv_mod defmod[] = { { CKR_OK, EIDV_RV_OK } };

int ckrv_decode_vwr(CK_RV rv, int count, ckrv_mod* mods) {
	int i;
	for(i=0; i<count; i++) {
		if(mods[i].rv == rv) {
			return mods[i].res;
		}
	}
	return EIDV_RV_FAIL;
}

#define check_rv_long(call, mods) { \
	CK_RV rv = call; \
	int retval = ckrv_decode_vwr(rv, sizeof(mods) / sizeof(ckrv_mod), mods); \
	if(retval != EIDV_RV_OK) { \
		cb->log(EID_VWR_LOG_DETAIL, #call " returned %d", retval); \
		return retval; \
	} \
}
#define check_rv(call) check_rv_long(call, defmod)
#define check_rv_late(rv) { \
	int retval = ckrv_decode_vwr(rv, 1, defmod); \
	if(retval != EIDV_RV_OK) { \
		cb->log(EID_VWR_LOG_DETAIL, "found return value of %d", retval); \
		return retval; \
	} \
}

struct eid_vwr_ui_callbacks* cb;

struct eid_vwr_ui_callbacks* eid_vwr_cbstruct() {
	struct eid_vwr_ui_callbacks* retval = calloc(sizeof(struct eid_vwr_ui_callbacks), 1);
	return retval;
}

int eid_vwr_serialize(void** data, int* len) {
	return 0;
}

int eid_vwr_deserialize(void* data, int len) {
	assert(cb);

	if(!len) {
		cb->newsrc(EID_VWR_SRC_NONE);
	}
	// TODO: parse the XML file, set the source
	return 0;
}

int eid_vwr_pinop(enum eid_vwr_pinops op) {
}

int eid_vwr_createcallbacks(struct eid_vwr_ui_callbacks* cb_) {
	assert(cb == NULL);
	cb = cb_;
	cb->newsrc(EID_VWR_SRC_NONE);
	check_rv(C_Initialize(NULL_PTR));
	return 0;
}

int find_first_slot(CK_SLOT_ID_PTR loc) {
	CK_SLOT_ID_PTR slotlist = malloc(sizeof(CK_SLOT_ID));
	CK_ULONG count = 0;
	CK_RV ret;
	ckrv_mod m[] = {
		{ CKR_BUFFER_TOO_SMALL, EIDV_RV_OK },
		{ CKR_OK, EIDV_RV_OK },
	};

	while((ret = C_GetSlotList(CK_TRUE, slotlist, &count)) == CKR_BUFFER_TOO_SMALL) {
		free(slotlist);
		slotlist = calloc(sizeof(CK_SLOT_ID), count);
	}
	check_rv_late(ret);
	if(count > 0) {
		*loc = slotlist[0];
		return EIDV_RV_OK;
	}
	return EIDV_RV_FAIL;
}

static int perform_find(CK_SESSION_HANDLE session, CK_BBOOL do_objid) {
	CK_OBJECT_HANDLE object;
	CK_ULONG count;
	do {
		char* label_str;
		char* value_str;
		char* objid_str = NULL;

		CK_ATTRIBUTE data[3] = {
			{ CKA_LABEL, NULL_PTR, 0 },
			{ CKA_VALUE, NULL_PTR, 0 },
			{ CKA_OBJECT_ID, NULL_PTR, 0 },
		};

		check_rv(C_FindObjects(session, &object, 1, &count));
		if(!count) continue;

		if(do_objid) {
			check_rv(C_GetAttributeValue(session, object, data, 3));
		} else {
			check_rv(C_GetAttributeValue(session, object, data, 2));
		}

		label_str = malloc(data[0].ulValueLen + 1);
		data[0].pValue = label_str;

		value_str = malloc(data[1].ulValueLen + 1);
		data[1].pValue = value_str;

		if(do_objid) {
			objid_str = malloc(data[2].ulValueLen + 1);
			data[2].pValue = objid_str;

			check_rv(C_GetAttributeValue(session, object, data, 3));

			objid_str[data[2].ulValueLen] = '\0';
		} else {
			check_rv(C_GetAttributeValue(session, object, data, 2));
		}

		label_str[data[0].ulValueLen] = '\0';
		value_str[data[1].ulValueLen] = '\0';

		cache_add(label_str, value_str, data[1].ulValueLen);

		cb->log(EID_VWR_LOG_DETAIL, "found data for label %s", label_str);
		if(can_convert(label_str)) {
			cb->log(EID_VWR_LOG_DETAIL, "converting %s", label_str);
			char* str = converted_string(label_str, value_str);
			cb->newstringdata(label_str, str);
			free(str);
		} else if(objid_str != NULL && is_string(objid_str, label_str)) {
			cb->newstringdata(label_str, value_str);
		} else {
			cb->newbindata(label_str, value_str, data[1].ulValueLen);
		}

		free(label_str);
		free(value_str);
		free(objid_str);
	} while(count);
}

int read_card(CK_SLOT_ID which) {
	CK_SESSION_HANDLE session;
	CK_ATTRIBUTE attr;
	CK_ULONG type;

	check_rv(C_OpenSession(which, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session));

	attr.type = CKA_CLASS;
	attr.pValue = &type;
	type = CKO_DATA;
	attr.ulValueLen = sizeof(CK_ULONG);

	cb->status("Reading identity data from card");

	check_rv(C_FindObjectsInit(session, &attr, 1));

	perform_find(session, 1);

	check_rv(C_FindObjectsFinal(session));

	type = CKO_CERTIFICATE;

	check_rv(C_FindObjectsInit(session, &attr, 1));

	cb->status("Reading certificates");

	perform_find(session, 0);

	check_rv(C_FindObjectsFinal(session));

	check_rv(C_CloseSession(session));
}

void eid_vwr_be_mainloop() {
	for(;;) {
		eid_vwr_poll();
		sleep(1);
	}
}

void eid_vwr_poll() {
	static CK_SLOT_ID prev_slot;
	static int had_slot = 0;
	CK_SLOT_ID tmp;

	if(find_first_slot(&tmp) == EIDV_RV_OK) {
		if(!had_slot || (tmp != prev_slot)) {
			cb->log(EID_VWR_LOG_NORMAL, "found a card in slot %lu", tmp);
			prev_slot = tmp;
			cb->newsrc(EID_VWR_SRC_CARD);
			had_slot = (read_card(prev_slot) == EIDV_RV_OK);
		}
	} else {
		if(had_slot) {
			cb->log(EID_VWR_LOG_NORMAL, "card gone, clearing data");
			cb->newsrc(EID_VWR_SRC_NONE);
		}
		had_slot = 0;
	}
}

struct eid_vwr_preview* eid_vwr_get_preview(char* filename) {
	// TODO: open the file, deserialize until we find a photo, pass back the photo.
	// For now, show the same photo in all cases...
	FILE* f;
	struct eid_vwr_preview* p;
	p = calloc(sizeof(struct eid_vwr_preview), 1);
	if(!filename) {
		return p;
	}
	if(strstr(filename, ".eid") != filename + (strlen(filename) - 4)) {
		return p;
	}
	f = fopen("../../tests/unit/foto.jpg", "r");
	if(!f) {
		perror("fopen");
		return p;
	}
	fseek(f, 0, SEEK_END);
	p->imagelen = ftell(f);
	fseek(f, 0, SEEK_SET);
	p->imagedata = malloc(p->imagelen);
	fread(p->imagedata, p->imagelen, 1, f);
	p->have_data = 1;
	return p;
}
