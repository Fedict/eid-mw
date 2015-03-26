#include <p11.h>
#include <unix.h>
#include <pkcs11.h>
#include <backend.h>
#include <state.h>
#include <labels.h>
#include <cache.h>

typedef struct {
	CK_RV rv;
	int res;
} ckrv_mod;

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
		be_log(EID_VWR_LOG_DETAIL, #call " returned %d", rv); \
		return retval; \
	} \
}
#define check_rv(call) check_rv_long(call, defmod)
#define check_rv_late(rv) { \
	int retval = ckrv_decode_vwr(rv, 1, defmod); \
	if(retval != EIDV_RV_OK) { \
		be_log(EID_VWR_LOG_DETAIL, "found return value of %d", rv); \
		return retval; \
	} \
}

int eid_vwr_p11_init() {
	check_rv(C_Initialize(NULL_PTR));

	return 0;
}

static CK_SESSION_HANDLE session;
static CK_SLOT_ID slot;

int eid_vwr_p11_open_session(void* slot_) {
	slot = *(CK_SLOT_ID_PTR)slot_;
	check_rv(C_OpenSession(slot, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session));

	return 0;
}

int eid_vwr_p11_close_session() {
	check_rv(C_CloseSession(session));

	cache_clear();

	return 0;
}

int eid_vwr_p11_find_first_slot(CK_SLOT_ID_PTR loc) {
	CK_SLOT_ID_PTR slotlist = malloc(sizeof(CK_SLOT_ID));
	CK_ULONG count = 0;
	CK_RV ret;

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

static int perform_find(CK_BBOOL do_objid) {
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

		be_log(EID_VWR_LOG_DETAIL, "found data for label %s", label_str);
		if(can_convert(label_str)) {
			be_log(EID_VWR_LOG_DETAIL, "converting %s", label_str);
			char* str = converted_string(label_str, value_str);
			be_newstringdata(label_str, str);
			free(str);
		} else if(objid_str != NULL && is_string(objid_str, label_str)) {
			be_newstringdata(label_str, value_str);
		} else {
			be_newbindata(label_str, value_str, data[1].ulValueLen);
		}

		free(label_str);
		free(value_str);
		free(objid_str);
	} while(count);
	sm_handle_event(EVENT_READ_READY, NULL, NULL, NULL);
	return 0;
}

int eid_vwr_p11_finalize_find(void* data) {
	check_rv(C_FindObjectsFinal(session));
	return 0;
}

int eid_vwr_p11_read_id(void* data) {
	CK_ATTRIBUTE attr;
	CK_ULONG type;

	attr.type = CKA_CLASS;
	attr.pValue = &type;
	type = CKO_DATA;
	attr.ulValueLen = sizeof(CK_ULONG);

	check_rv(C_FindObjectsInit(session, &attr, 1));

	return perform_find(1);
}

int eid_vwr_p11_read_certs(void* data) {
	CK_ATTRIBUTE attr;
	CK_ULONG type;
	
	attr.type = CKA_CLASS;
	attr.pValue = &type;
	type = CKO_CERTIFICATE;
	attr.ulValueLen = sizeof(CK_ULONG);

	check_rv(C_FindObjectsInit(session, &attr, 1));

	return perform_find(0);
}

int eid_vwr_p11_do_pinop(void* data) {
	enum eid_vwr_pinops p = (enum eid_vwr_pinops) data;
	check_rv(C_Login(session, CKU_USER, NULL_PTR, 0));
	if(p >= EID_VWR_PINOP_CHG) {
		check_rv(C_SetPIN(session, NULL_PTR, 0, NULL_PTR, 0));
	}
	sm_handle_event(EVENT_READ_READY, NULL, NULL, NULL);

	return 0;
}

int eid_vwr_p11_leave_pinop() {
	check_rv(C_Logout(session));

	return 0;
}
