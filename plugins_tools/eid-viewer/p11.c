#include <p11.h>

#ifdef WIN32
#include <win32.h>
#else
#include <unix.h>
#endif
#include <pkcs11.h>
#include <backend.h>
#include <state.h>
#include "labels.h"
#include <cache.h>
#include <string.h>

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
		be_log(EID_VWR_LOG_DETAIL, TEXT( "%s returned %d"),#call, rv); \
		return retval; \
	} \
}
#define check_rv(call) check_rv_long(call, defmod)
#define check_rv_late(rv) { \
	int retval = ckrv_decode_vwr(rv, 1, defmod); \
	if(retval != EIDV_RV_OK) { \
		be_log(EID_VWR_LOG_DETAIL, TEXT("found return value of %d"), rv); \
		return retval; \
	} \
}

/* Called by state machine to initialize p11 subsystem */
int eid_vwr_p11_init() {
	check_rv(C_Initialize(NULL_PTR));

	return 0;
}

static CK_SESSION_HANDLE session;
static CK_SLOT_ID slot;
static CK_SLOT_ID slot_manual;
static CK_BBOOL is_auto = CK_TRUE;

/* Called by UI when user selects a slot (or selects the "automatic" option again */
int eid_vwr_p11_select_slot(CK_BBOOL automatic, CK_SLOT_ID manualslot) {
	is_auto = automatic;
	if(!is_auto) {
		slot_manual = manualslot;
	}

	return 0;
}

/* Called by state machine when a card is inserted */
int eid_vwr_p11_open_session(void* slot_) {
	slot = *(CK_SLOT_ID_PTR)slot_;
	check_rv(C_OpenSession(slot, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session));

	be_newsource(EID_VWR_SRC_CARD);

	return 0;
}

/* Called by state machine when a card is removed */
int eid_vwr_p11_close_session() {
	check_rv(C_CloseSession(session));

	cache_clear();

	return 0;
}

/* Called by eid_vwr_poll(). */
int eid_vwr_p11_find_first_slot(CK_BBOOL with_token, CK_SLOT_ID_PTR loc, CK_ULONG_PTR count) {
	CK_RV ret;

	*count = 0;
	if(is_auto) {
		CK_SLOT_ID_PTR slotlist = NULL;
		C_GetSlotList(with_token, slotlist, count);

		slotlist = (CK_SLOT_ID_PTR)calloc(sizeof(CK_SLOT_ID), *count);
		if (slotlist == NULL)
		{
			return EIDV_RV_FAIL;
		}
		ret = C_GetSlotList(with_token, slotlist, count);
		check_rv_late(ret);
		if (*count > 0) {
			*loc = slotlist[0];
			free(slotlist);
			return EIDV_RV_OK;
		}

	} else {
		CK_SLOT_INFO info;
		ret = C_GetSlotInfo(slot_manual, &info);
		if(with_token) {
			if((ret == CKR_OK) && ((info.flags & CKF_TOKEN_PRESENT) == CKF_TOKEN_PRESENT)) {
				*loc = slot_manual;
				return EIDV_RV_OK;
			}
		} else {
			/* Figure out how many slots there are so that the caller can update its 
			   state if necessary, but return FAIL if GetSlotInfo told us the reader wasn't found */
			C_GetSlotList(CK_FALSE, NULL, count);
			if(ret == CKR_OK) {
				return EIDV_RV_OK;
			}
		}
	}
	return EIDV_RV_FAIL;
}

/* Called by UI to get list of slots */
int eid_vwr_p11_name_slots(struct _slotdesc* slots, CK_ULONG_PTR len) {
	CK_SLOT_ID_PTR slotlist = (CK_SLOT_ID_PTR)calloc(sizeof(CK_SLOT_ID), 1);
	CK_ULONG count = 1;
	CK_RV ret;
	int rv = EIDV_RV_FAIL;
	int i;
	int counter;
	char description[65];
	description[64] = '\0';
	unsigned int description_len = 65;

	while((ret = C_GetSlotList(CK_FALSE, slotlist, &count)) == CKR_BUFFER_TOO_SMALL) {
		free(slotlist);
		slotlist = (CK_SLOT_ID_PTR)calloc(sizeof(CK_SLOT_ID), count);
	}
	if(count > *len) {
		*len = count;
		goto end;
	}
	for(i=0; i<count; i++) {
		CK_SLOT_INFO info;
		slots[i].slot = slotlist[i];

		ret = C_GetSlotInfo(slotlist[i], &info);
		if(ret != CKR_OK) {
			goto end;
		}
		//null-terminate the description (and remove padding spaces)
		memcpy(description, info.slotDescription, sizeof(info.slotDescription) > description_len ? description_len : sizeof(info.slotDescription));
		for (counter = description_len-1; (description[counter] == ' ' || description[counter] == '\0') && (counter > 0); counter--) {
			description[counter] = '\0';
		}

		//transform it into a wchar if needed
		unsigned long len;
		slots[i].description = UTF8TOEID(description, &len);
	}

	rv = EIDV_RV_OK;
end:
	free(slotlist);
	return rv;
}

/* Called by the backend when something needs to be passed on to the UI.
 * Will abstract the conversion between on-card data and presentable
 * data */
void eid_vwr_p11_to_ui(const EID_CHAR* label, const void* value, int len) {
	EID_CHAR* str;
	if(can_convert(label)) {
		be_log(EID_VWR_LOG_DETAIL, TEXT("converting %s"), label);
		str = converted_string(label, (const EID_CHAR*)value);
		be_newstringdata(label, str);
		free(str);
	} else if(is_string(label)) {
		be_newstringdata(label, (const EID_CHAR*)value);
	} else {
		be_newbindata(label, value, len);
	}
}

/* Performs a previously-initialized find operation. */
static int perform_find(CK_BBOOL do_objid) {
	CK_OBJECT_HANDLE object;
	CK_ULONG count;
	do {
		unsigned char* label_str;
		unsigned char* value_str;
		unsigned char* objid_str = NULL;

		CK_ATTRIBUTE data[3] = {
			{ CKA_LABEL, NULL_PTR, 0 },
			{ CKA_VALUE, NULL_PTR, 0 },
			{ CKA_OBJECT_ID, NULL_PTR, 0 },
		};

		check_rv(C_FindObjects(session, &object, 1, &count));
		if (!count) continue;

		if (do_objid) {
			check_rv(C_GetAttributeValue(session, object, data, 3));
		}
		else {
			check_rv(C_GetAttributeValue(session, object, data, 2));
		}

		label_str = (unsigned char*)malloc(data[0].ulValueLen + 1);
		data[0].pValue = label_str;

		value_str = (unsigned char*)malloc(data[1].ulValueLen + 1);
		data[1].pValue = value_str;

		if (do_objid) {
			objid_str = (unsigned char*)malloc(data[2].ulValueLen + 1);
			data[2].pValue = objid_str;

			check_rv(C_GetAttributeValue(session, object, data, 3));

			objid_str[data[2].ulValueLen] = '\0';
		}
		else {
			check_rv(C_GetAttributeValue(session, object, data, 2));
		}

		label_str[data[0].ulValueLen] = '\0';
		value_str[data[1].ulValueLen] = '\0';


		EID_CHAR* label_eidstr = UTF8TOEID((const char*)label_str, &(data[0].ulValueLen));
		if (is_string(label_eidstr))
		{
			EID_CHAR* value_eidstr = UTF8TOEID((const char*)value_str, &(data[1].ulValueLen));
			cache_add(label_eidstr, value_eidstr, data[1].ulValueLen / sizeof(EID_CHAR));
			be_log(EID_VWR_LOG_DETAIL, TEXT("found data for label %s"), label_eidstr);
			eid_vwr_p11_to_ui(label_eidstr, value_eidstr, (int)data[1].ulValueLen);
			EID_SAFE_FREE(value_eidstr);
		}
		else
		{
			cache_add_bin(label_eidstr, value_str, data[1].ulValueLen);
			be_log(EID_VWR_LOG_DETAIL, TEXT("found data for label %s"), label_eidstr);
			eid_vwr_p11_to_ui(label_eidstr, value_str, (int)data[1].ulValueLen);
		}
		
		EID_SAFE_FREE(label_eidstr);		
		EID_SAFE_FREE(label_str);
		EID_SAFE_FREE(value_str);
		EID_SAFE_FREE(objid_str);
	} while(count);
	/* Inform state machine that we're done reading, which will
	 * cause the state machine to enter the next state */
	sm_handle_event(EVENT_READ_READY, NULL, NULL, NULL);
	return 0;
}

/* Called by state machine at end of TOKEN_CERTS and TOKEN_ID states */
int eid_vwr_p11_finalize_find() {
	check_rv(C_FindObjectsFinal(session));
	return 0;
}

/* Called by state machine at start of TOKEN_ID state */
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

/* Called by state machine at start of TOKEN_CERTS state */
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

/* Do the actual PIN operation. Separate helper function for the below,
 * because PKCS#11 "failed" message does not match state machine
 * "failed" message, and otherwise we can't use our check_rv() macro
 */
static int eid_vwr_p11_do_pinop_real(enum eid_vwr_pinops p) {
	/* Need to do a C_Login in both cases (whether we're doing a "test pin"
	 * or a "change pin", since you can't change your PIN code unless
	 * you're logged in. */
	if (p == EID_VWR_PINOP_TEST) {
		check_rv(C_Login(session, CKU_USER, NULL, 0));
	}
	else {
		check_rv(C_SetPIN(session, NULL_PTR, 0, NULL_PTR, 0));
	}
	sm_handle_event(EVENT_READ_READY, NULL, NULL, NULL);
	
	return 0;
}

/* Called by state machine when a "perform PIN operation" action was
 * requested */
int eid_vwr_p11_do_pinop(void* data) {
	int retval;
	enum eid_vwr_pinops p = (enum eid_vwr_pinops) data;
	if((retval = eid_vwr_p11_do_pinop_real(p)) != CKR_OK) {
		be_pinresult(p, EID_VWR_RES_FAILED);
	} else {
		be_pinresult(p, EID_VWR_RES_SUCCESS);
	}
	return retval;
}

/* Called by state machine at end of TOKEN_PINOP state. */
int eid_vwr_p11_leave_pinop() {
	check_rv(C_Logout(session));

	return 0;
}

int eid_vwr_p11_check_version() {
	CK_INFO info;
	check_rv(C_GetInfo(&info));
	if(info.libraryVersion.major < 4 || (info.libraryVersion.major == 4 && info.libraryVersion.minor < 2)) {
		be_log(EID_VWR_LOG_ERROR, "eID middleware outdated. Found version %d.%d, whereas version 4.2 or higher is required for this version of the eID viewer", info.libraryVersion.major, info.libraryVersion.minor);
		return 1;
	}

	return 0;
}
