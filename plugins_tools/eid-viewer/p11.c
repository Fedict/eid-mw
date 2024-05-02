#include <p11.h>

#ifdef WIN32
#include <win32.h>
#pragma pack(push, cryptoki, 1)
#include "pkcs11.h"
#pragma pack(pop, cryptoki)
#else
#include "unix.h"
#include "pkcs11.h"
#endif
#include <backend.h>
#include <state.h>
#include <eid-util/labels.h>
#include "conversions.h"
#include <cache.h>
#include <string.h>
#include <stdio.h>

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

//use this one if you want the slot_changed event handler to select the first slotID
#define SLOTID_FIRSTSLOT -1

#define check_rv_long(call, mods) { \
	CK_RV rv = call; \
	int retval = ckrv_decode_vwr(rv, sizeof(mods) / sizeof(ckrv_mod), mods); \
	if(retval != EIDV_RV_OK) { \
		be_log(EID_VWR_LOG_DETAIL, TEXT(EID_S_FORMAT) TEXT(" returned %#x"),#call, rv); \
		return retval; \
	} \
}
#define check_rv(call) check_rv_long(call, defmod)
#define check_rv_late(call, rv) { \
	int retval = ckrv_decode_vwr(rv, 1, defmod); \
	if(retval != EIDV_RV_OK) { \
		be_log(EID_VWR_LOG_DETAIL, TEXT(EID_S_FORMAT) TEXT(": found return value of %#x"), #call, rv); \
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

	be_log(EID_VWR_LOG_DETAIL, TEXT("Read modus selected. automatic = %d, manualslot = %ul"), automatic, manualslot);

	if ( (automatic == CK_TRUE) && (!is_auto) )
	{
		//when we switch back from manual to automatic, issue a device changed event, 
		//so cards are searched if needed
#ifdef WIN32
		CK_SLOT_ID* pslotID0 = malloc(sizeof(CK_SLOT_ID));
		*pslotID0 = SLOTID_FIRSTSLOT;//let the handler select the first slotID
		sm_handle_event(EVENT_DEVICE_CHANGED, pslotID0, free, NULL);
#endif
	}
	is_auto = automatic;
	if(!is_auto)
	{
		slot_manual = manualslot;
#ifdef WIN32
		CK_SLOT_ID* pslotID =  malloc(sizeof(CK_SLOT_ID));
		*pslotID = manualslot;
		sm_handle_event(EVENT_DEVICE_CHANGED, pslotID, free, NULL);
#endif
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

#ifndef WIN32
/* Called by eid_vwr_poll(). */
int eid_vwr_p11_find_first_slot(CK_BBOOL with_token, CK_SLOT_ID_PTR loc, CK_ULONG_PTR count) {
	CK_RV ret;

	*count = 0;
	if(is_auto) {
		CK_SLOT_ID_PTR slotlist = NULL;
		C_GetSlotList(with_token, slotlist, count);

		if (*count == 0)
		{
			return EIDV_RV_FAIL;
		}
		slotlist = (CK_SLOT_ID_PTR)calloc(sizeof(CK_SLOT_ID), *count);

		if (slotlist == NULL)
		{
			return EIDV_RV_FAIL;
		}
		ret = C_GetSlotList(with_token, slotlist, count);
		check_rv_late("C_GetSlotList", ret);

		*loc = slotlist[0];
		free(slotlist);
		return EIDV_RV_OK;

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
#endif

/* Called by UI to get list of slots */
int eid_vwr_p11_name_slots(struct _slotdesc* slots, CK_ULONG_PTR len) {
	CK_SLOT_ID_PTR slotlist = (CK_SLOT_ID_PTR)calloc(sizeof(CK_SLOT_ID), 1);
	CK_ULONG count = 1;
	CK_RV ret;
	int rv = EIDV_RV_FAIL;
	CK_ULONG i;
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
		size_t label_len;
		be_log(EID_VWR_LOG_DETAIL, TEXT("converting %s"), label);
		str = converted_string(label, (const EID_CHAR*)value);
		be_newstringdata(label, str);
		free(str);
		label_len = (EID_STRLEN(label) + 5) * sizeof(EID_CHAR);
		str = malloc(label_len);
		EID_SNPRINTF(str, label_len, TEXT("%s_raw"), label);
		be_newbindata(str, value, len);
		free(str);
	} else if(is_string(label)) {
		be_newstringdata(label, (const EID_CHAR*)value);
	} else {
		be_newbindata(label, value, len);
	}
}

/* Performs a previously-initialized find operation. 
 * This function may only be called from the state machine thread
 */
static int perform_find(CK_BBOOL do_objid) {
	CK_OBJECT_HANDLE object = 0;
	CK_ULONG count = 0;
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
	sm_handle_event_onthread(EVENT_READ_READY, NULL);
	return 0;
}

/* Called by state machine at end of TOKEN_CERTS and TOKEN_ID states */
int eid_vwr_p11_finalize_find() {
	check_rv(C_FindObjectsFinal(session));
	return 0;
}



/* Called on state machine at start of TOKEN_ID state by eid_vwr_p11_read_id
 * This function may only be called from the state machine thread
 * It returns the card's graphical version byte, which determines
 * the conversion functions to be used
 */
int eid_vwr_p11_read_graph_vers(void* data EIDV_UNUSED) {
	CK_ULONG data_class = CKO_DATA;
	CK_ULONG attribute_len = 2; //the number of attributes in the search template below
	//the searchtemplate that will be used to initialize the search
	CK_ATTRIBUTE attributes[2] = { {CKA_CLASS,&data_class,sizeof(CK_ULONG)},
	{CKA_LABEL,"tokeninfo_graph_perso_version",(CK_ULONG)strlen("tokeninfo_graph_perso_version")} };
	//prepare the findobjects function to find all objects with attributes 
	//CKA_CLASS set to CKO_DATA and with CKA_LABEL set to "tokeninfo_graph_perso_version"

	CK_ULONG ulMaxObjectCount = 1;//we want max one object returned
	CK_ULONG ulObjectCount = 0;	//returns the number of objects found
	CK_OBJECT_HANDLE hKey;
	//retrieve the object with label "tokeninfo_graph_perso_version" 
	check_rv(C_FindObjectsInit(session, attributes, attribute_len));

	CK_RV retval = C_FindObjects(session, &hKey, ulMaxObjectCount, &ulObjectCount);
	if (retval != CKR_OK)
	{
		be_log(EID_VWR_LOG_DETAIL, TEXT(":C_FindObjects returned value 0x%2x"), retval);
		C_FindObjectsFinal(session);
		return retval;
	}
	if ((ulObjectCount == 0) || (hKey == NULL_PTR))
	{
		//no "tokeninfo_graph_perso_version" object was found
		be_log(EID_VWR_LOG_DETAIL, TEXT(":C_FindObjects did not find object with label tokeninfo_graph_perso_version"));
		check_rv(C_FindObjectsFinal(session));
	}
	else
	{
		//"tokeninfo_graph_perso_version" object was found

		unsigned char value = 0;

		CK_ATTRIBUTE data = { CKA_VALUE, &value, 1 };

		check_rv(C_GetAttributeValue(session, hKey, &data, 1));

		//add the graphical version byte to cache
		cache_add_bin(TEXT("tokeninfo_graph_perso_version"), &value, 1);
		be_log(EID_VWR_LOG_DETAIL, TEXT("found data for tokeninfo_graph_perso_version 0x.2%x"), value);


		check_rv(C_FindObjectsFinal(session));
	}

	return retval;
}

/* Called by state machine at start of TOKEN_ID state
 * This function may only be called from the state machine thread
 */
int eid_vwr_p11_read_id(void* data EIDV_UNUSED) {
	CK_ATTRIBUTE attr;
	CK_ULONG type;

	attr.type = CKA_CLASS;
	attr.pValue = &type;
	type = CKO_DATA;
	attr.ulValueLen = sizeof(CK_ULONG);

	check_rv(eid_vwr_p11_read_graph_vers(0));

	check_rv(C_FindObjectsInit(session, &attr, 1));

	return perform_find(1);
}

/* Called by state machine at start of TOKEN_CERTS state
 * This function may only be called from the state machine thread
 */
int eid_vwr_p11_read_certs(void* data EIDV_UNUSED) {
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
	if (p == EID_VWR_PINOP_TEST) {
		check_rv(C_Login(session, CKU_USER, NULL, 0));
	}
	else {
		check_rv(C_SetPIN(session, NULL_PTR, 0, NULL_PTR, 0));
	}
	sm_handle_event(EVENT_PINOP_READY, NULL, NULL, NULL);

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
	/* Try to log out if we can, but don't worry if we can't */
	C_Logout(session);

	return 0;
}

/* Do the actual perform challenge operation. 
 * Separate helper function for the function below,
 * because PKCS#11 "failed" message does not match state machine
 * "failed" message, and otherwise we can't use our check_rv() macro
 */
static int eid_vwr_p11_do_challenge_real(struct eid_vwr_challenge_responsedata *p) {

	/** Struct used by challenge handler */
	//struct eid_vwr_challenge_responseedata response;

	CK_ULONG data_class = CKO_PRIVATE_KEY;
	CK_ULONG attribute_len = 2; //the number of attributes in the search template below
	//the searchtemplate that will be used to initialize the search
	CK_ATTRIBUTE attributes[2] = { {CKA_CLASS,&data_class,sizeof(CK_ULONG)},
	{CKA_LABEL,"Card",(CK_ULONG)strlen("Card")} };
	//prepare the findobjects function to find all objects with attributes 
	//CKA_CLASS set to CKO_PUBLIC_KEY and with CKA_LABEL set to "Card"

	CK_ULONG ulMaxObjectCount = 1;//we want max one object returned
	CK_ULONG ulObjectCount = 0;	//returns the number of objects found
	CK_OBJECT_HANDLE hKey;
	//retrieve the object with label "Card" 
	check_rv(C_FindObjectsInit(session, attributes, attribute_len));

	CK_RV retval = C_FindObjects(session, &hKey, ulMaxObjectCount, &ulObjectCount);
	if (retval != CKR_OK)
	{
		be_log(EID_VWR_LOG_DETAIL, TEXT(":C_FindObjects found return value of 0x%2x"), retval);
		C_FindObjectsFinal(session);
		return retval;
	}
	if ((ulObjectCount == 0) || (hKey == NULL_PTR))
	{
		//no "Card" object was found
		be_log(EID_VWR_LOG_DETAIL, TEXT(":C_FindObjects did not find object with label Card"));
		check_rv(C_FindObjectsFinal(session));
		p->response = NULL_PTR;
		p->responselen = 0;
		p->result = EID_VWR_RES_FAILED;
	}
	else
	{
		//"Card" object was found, now sign the challenge with the card key
		check_rv(C_FindObjectsFinal(session));

		//use the CKM_ECDSA mechanism for the challenge
		CK_MECHANISM mechanism = { CKM_ECDSA, NULL_PTR, 0 };

		//initialize the signature operation
		check_rv(C_SignInit(session, &mechanism, hKey));

		// We should really change struct
		// eid_vwr_challenge_responsedata, but doing so changes
		// the ABI of libeidviewer. Let's try to avoid that for
		// now.
		CK_ULONG resplen = 0;
		check_rv(C_Sign(session, p->challenge, (CK_ULONG)p->challengelen, p->response, &(resplen)));
		p->responselen = (size_t)resplen;

		p->result = EID_VWR_RES_SUCCESS;
	}

	//put the EVENT_CHALLENGE_READY on the event queu
	sm_handle_event(EVENT_CHALLENGE_READY, NULL, NULL, NULL);

	return CKR_OK;
}

/* Called by state machine when a "perform challenge operation" action was
 * requested */
int eid_vwr_p11_do_challenge(void* data) {
	int retval = EIDV_RV_MEM_FAIL;
	struct eid_vwr_challenge_responsedata *p = (struct eid_vwr_challenge_responsedata*) data;
	CK_ULONG signLength;
	CK_BYTE_PTR signature;

	//we currently only support SHA384 hashes
	if (p->challengelen == 48)
	{
		signature = calloc(96, 1);
		signLength = 96;
		p->response = signature;
		p->responselen = signLength;

		if (signature != NULL)
		{
			retval = eid_vwr_p11_do_challenge_real(p);
			if (retval == CKR_OK) {
				be_challengeresult(p->response, p->responselen, p->result);
			}
			free(signature);
		}
		else {
			be_log(EID_VWR_LOG_ERROR, TEXT("Memory allocation error in backend"));
		}
	}
	else {
		be_log(EID_VWR_LOG_ERROR, TEXT("only 48 byte challenges are supported"));
		retval = EIDV_RV_FAIL;
	}
	if (retval != CKR_OK)
	{
		//always report back to the UI
		be_challengeresult(NULL, 0, EID_VWR_RES_FAILED);
	}
	return retval;
}

int eid_vwr_p11_check_version(void* data EIDV_UNUSED) {
	CK_INFO info;
	check_rv(C_GetInfo(&info));
	if(info.libraryVersion.major < 4 || (info.libraryVersion.major == 4 && info.libraryVersion.minor < 2)) {
		be_log(EID_VWR_LOG_ERROR, TEXT("eID middleware outdated. Found version %d.%d, whereas version 4.2 or higher is required for this version of the eID viewer"), info.libraryVersion.major, info.libraryVersion.minor);
		return 1;
	}

	return 0;
}

#ifdef WIN32
/* Called by state machine when a card/reader change is inserted,
 * it should not be called from any other thread but the state machine thread
 */
int eid_vwr_p11_check_reader_list(void* slot_ID) {

	static CK_SLOT_ID_PTR pcurrentSlotList = NULL;
	static CK_ULONG currentReaderCount = 0;
	static CK_SLOT_ID currentCardSlotID = -1;
	CK_RV p11Ret = EIDV_RV_OK;
	int ret = 0;
	CK_ULONG tempCount = 0;
	CK_ULONG cardCount = 0;
	CK_SLOT_ID slotID = *((CK_SLOT_ID_PTR)slot_ID);
	boolean slotIDKnown = FALSE;

	be_log(EID_VWR_LOG_DETAIL, TEXT("eid_vwr_p11_check_reader_list called slotID given = %d"), slotID);

	//if reader list is not known yet, retrieve it
	if (pcurrentSlotList == NULL)
	{
		//pkcs11v2.20: refresh the slotlist
		be_log(EID_VWR_LOG_NORMAL, TEXT("SLOTEVENT No slot list set, resetting slot list slotID given = %d"), slotID);
		eid_vwr_p11_reset_slot_list(&pcurrentSlotList, &currentReaderCount, &currentCardSlotID);
		is_auto = TRUE;
	}
	else 
	{
		//if we received the SLOTID_FIRSTSLOT (when we switch back from manual to automatic slot selection this value is given),
		//set slotID to first slot in the list
		if (slotID == SLOTID_FIRSTSLOT)
		{
			slotID = pcurrentSlotList[0];
			be_log(EID_VWR_LOG_DETAIL, TEXT("SLOTID_FIRSTSLOT received setting it to %d"), slotID);
		}

		//we already have a slotlist, so first check if slot_ID is in this list
		//if not, add the slot by updating the slotlist
		//if so, check for changes of the slot_ID
		for (CK_ULONG i = 0; i < currentReaderCount; i++)
		{
			if (pcurrentSlotList[i] == slotID)
			{
				slotIDKnown = TRUE;
				break;
			}
		}

		if (slotIDKnown == FALSE)
		{
			//new reader attached
			//pkcs11v2.20: refresh the slotlist
			be_log(EID_VWR_LOG_NORMAL, TEXT("SLOTEVENT with unknown slotID = %d resetting slot list"), slotID);
			eid_vwr_p11_reset_slot_list(&pcurrentSlotList, &currentReaderCount, &currentCardSlotID);
		}
		else
		{
			CK_SLOT_INFO info;

			p11Ret = C_GetSlotInfo(slotID, &info);
			if (p11Ret != CKR_OK)
			{
				//the slotID is already known, first check if the reader is removed
				//if so, update the slot list
				be_log(EID_VWR_LOG_COARSE, TEXT("Failed retrieving slot information\n C_GetSlotInfo failed with 0X%x. slotID = %d"), p11Ret, slotID);
				if (p11Ret == CKR_DEVICE_ERROR)
				{
					//error trying to communicate with the slot, reset the slot list (as most lickely the card reader got removed)
					be_log(EID_VWR_LOG_NORMAL, TEXT("CKR_DEVICE_ERROR received, was reader removed?"));
					eid_vwr_p11_reset_slot_list(&pcurrentSlotList, &currentReaderCount, &currentCardSlotID);
					return EIDV_RV_OK;
				}

				//ignore this state change
				be_log(EID_VWR_LOG_COARSE, TEXT("Ignoring this state change"));
				return EIDV_RV_FAIL;
			}

			if ((info.flags & CKF_TOKEN_PRESENT) != 0)
			{
				//a token is present in the slot_ID that caused the event
				//first check if the token is supported by our pkcs11 library
				if (eid_vwr_p11_token_supported(&slotID) != EIDV_RV_OK)
				{
					//we don't support this token, ignore this event
					return EIDV_RV_OK;
				}

				//check if this slot_ID needs to be watched
				if (is_auto)
				{
					//in case of auto, only handle new card if none was active already
					if (currentCardSlotID == -1)
					{
						be_log(EID_VWR_LOG_NORMAL, TEXT("EVENT_TOKEN_INSERTED slotID = %d"), slotID);
						sm_handle_event_onthread(EVENT_TOKEN_INSERTED, &slotID);
						currentCardSlotID = slotID;
					}
				}
				else
				{
					//in case of manual, only handle new card if the slotID is the one set to slot_manual
					if (slot_manual == slotID)
					{
						//if another card was already present in another card reader, remove that one first 
						if ((currentCardSlotID != -1) && (currentCardSlotID != slot_manual))
						{
							be_log(EID_VWR_LOG_NORMAL, TEXT("EVENT_TOKEN_REMOVED slotID = %d"), currentCardSlotID);
							sm_handle_event_onthread(EVENT_TOKEN_REMOVED, &currentCardSlotID);
							currentCardSlotID = -1;
						}
						if (currentCardSlotID != slot_manual)
						{
							be_log(EID_VWR_LOG_NORMAL, TEXT("EVENT_TOKEN_INSERTED slotID = %d"), slotID);
							sm_handle_event_onthread(EVENT_TOKEN_INSERTED, &slotID);
							currentCardSlotID = slotID;
						}
					}
				}
			}
			else
			{
				if (is_auto)
				{
					//if the token that was removed, was in the currentCardSlotID
					if (currentCardSlotID == slotID)
					{
						be_log(EID_VWR_LOG_NORMAL, TEXT("EVENT_TOKEN_REMOVED slotID = %d"), slotID);
						sm_handle_event_onthread(EVENT_TOKEN_REMOVED, &slotID);
						currentCardSlotID = -1;
					}
					if (currentCardSlotID == -1)
					{
						//token was removed (or auto was activated), check if another token is present in another slot (that is being watched)
						ret = eid_vwr_p11_find_eid_card(&currentCardSlotID);
						if (ret == EIDV_RV_OK)
						{
							//we found another eID card, report it
							be_log(EID_VWR_LOG_NORMAL, TEXT("EVENT_TOKEN_INSERTED slotID = %d"), currentCardSlotID);
							sm_handle_event_onthread(EVENT_TOKEN_INSERTED, &currentCardSlotID);
						}
					}
				}
				else if (slot_manual == slotID)
				{
					//if a card was already present in a card reader, remove it 
					if (currentCardSlotID != -1)
					{
						be_log(EID_VWR_LOG_NORMAL, TEXT("EVENT_TOKEN_REMOVED slotID = %d"), slotID);
						sm_handle_event_onthread(EVENT_TOKEN_REMOVED, &slotID);
						currentCardSlotID = -1;
					}
				}
			}
		}
	}
	return ret;
}



//inform the UI of update reader list
int eid_vwr_p11_update_slot_list_ui(CK_SLOT_ID_PTR slotlist, CK_ULONG slotCount)
{
	slotdesc* slotDescs = (slotdesc*)malloc(slotCount * sizeof(slotdesc));
	if (slotDescs == NULL)
	{
		be_log(EID_VWR_LOG_ERROR, TEXT("eid_vwr_p11_update_slot_list_ui failed allocating memory for the slot descriptions"));
		return EIDV_RV_FAIL;
	}
	memset(slotDescs, 0, slotCount * sizeof(slotdesc));

	CK_RV p11Ret;
	int ret = EIDV_RV_OK;
	CK_ULONG i;
	int counter;
	char description[65];
	description[64] = '\0';
	unsigned int description_len = 65;

	//retrieve all slot descriptions
	for (i = 0; i < slotCount; i++)
	{
		CK_SLOT_INFO info;
		slotDescs[i].slot = slotlist[i];

		p11Ret = C_GetSlotInfo(slotlist[i], &info);
		if (p11Ret != CKR_OK)
		{
			be_log(EID_VWR_LOG_ERROR, TEXT("eid_vwr_p11_update_slot_list_ui for slot %d failed C_GetSlotInfo returned 0x%08u"), slotlist[i],p11Ret);
			ret = EIDV_RV_FAIL;
			goto end;
		}
		//null-terminate the description (and remove padding spaces)
		memcpy(description, info.slotDescription, sizeof(info.slotDescription) > description_len ? description_len : sizeof(info.slotDescription));
		for (counter = description_len - 1; (description[counter] == ' ' || description[counter] == '\0') && (counter > 0); counter--)
		{
			description[counter] = '\0';
		}

		//transform it into a wchar if needed, memory will be allocated and need to be freed by caller
		unsigned long len;
		slotDescs[i].description = UTF8TOEID(description, &len);
	}

	if (be_readers_changed(slotCount, slotDescs) != EIDV_RV_OK)
	{
		//no callback present, UI will not be informed
		be_log(EID_VWR_LOG_ERROR, TEXT("readers changed, but cannot inform UI about it, no callback specified"));
		ret = EIDV_RV_FAIL;
	}
	for (i = 0; i < slotCount; i++)
	{
		if (slotDescs[i].description != NULL) 
		{
			free(slotDescs[i].description);
		}
	}

end:
	free(slotDescs);
	return ret;
}


/* May only be called by the state machine thread
 * Discards the current slot list and fetches a new one from pkcs11
 * It will check this new slot list for readers and cards found, 
 * and will handle the corresponding events on its thread
 * It will also send the new reader list to the UI, if the callback for it is set
 */
int eid_vwr_p11_reset_slot_list(CK_SLOT_ID_PTR *ppcurrentSlotList, CK_ULONG *pcurrentReaderCount, CK_SLOT_ID *pcurrentCardSlotID) {

	CK_RV p11Ret = CKR_OK;
	int ret = EIDV_RV_OK;
	CK_ULONG tempCount = 0;
	CK_SLOT_ID cardSlotID = 0;
	CK_ULONG cardCount = 0;

	if (*ppcurrentSlotList != NULL)
	{
		free(*ppcurrentSlotList);
		*ppcurrentSlotList = NULL;
	}
	if (*pcurrentCardSlotID != -1)
	{
		be_log(EID_VWR_LOG_NORMAL, TEXT("EVENT_TOKEN_REMOVED slotID = %d"),*pcurrentCardSlotID);
		sm_handle_event_onthread(EVENT_TOKEN_REMOVED, pcurrentCardSlotID);
		*pcurrentCardSlotID = -1;
	}
	*pcurrentReaderCount = 0;

	is_auto = TRUE;//we are not certain that slots have kept their position in the list, so move back to auto selection
	be_log(EID_VWR_LOG_DETAIL, TEXT("eid_vwr_p11_reset_slot_list resetting slotList"), p11Ret, tempCount);
	p11Ret = C_GetSlotList(CK_FALSE, NULL, &tempCount);
	if ( (tempCount > 0) && (p11Ret == CKR_OK))
	{
		*ppcurrentSlotList = (CK_SLOT_ID_PTR)malloc(tempCount * sizeof(CK_SLOT_ID));
		if (*ppcurrentSlotList == NULL)
		{
			be_log(EID_VWR_LOG_ERROR, TEXT("eid_vwr_p11_refresh_slot_list: malloc failed, could not store the readers list"));
			return EIDV_RV_FAIL;
		}

		p11Ret = C_GetSlotList(CK_FALSE, *ppcurrentSlotList, &tempCount);
		if(p11Ret == CKR_OK)
		{ 
			*pcurrentReaderCount = tempCount;
			//a reader was found, handle the reader found event
			be_log(EID_VWR_LOG_NORMAL, TEXT("EVENT_READER_FOUND slotID = %d"), *(*ppcurrentSlotList));
			sm_handle_event_onthread(EVENT_READER_FOUND, *ppcurrentSlotList);

			//we have a new slot list, now we fetch the reader descriptions and send them to the UI
			eid_vwr_p11_update_slot_list_ui(*ppcurrentSlotList, *pcurrentReaderCount);

			//now try to retrieve an eID card
			ret = eid_vwr_p11_find_eid_card(&cardSlotID);
			if (ret == EIDV_RV_OK)
			{
				*pcurrentCardSlotID = cardSlotID;
				be_log(EID_VWR_LOG_NORMAL, TEXT("EVENT_TOKEN_INSERTED slotID = %d"), cardSlotID);
				sm_handle_event_onthread(EVENT_TOKEN_INSERTED, &cardSlotID);
			}
		}
		else {
			be_log(EID_VWR_LOG_ERROR, TEXT("eid_vwr_p11_reset_slot_list failed populating new slotList: C_GetSlotList returned = 0X%x \n number of slots = %d"), p11Ret, tempCount);
		}
	}
	else{
		be_log(EID_VWR_LOG_ERROR, TEXT("eid_vwr_p11_reset_slot_list failed creating new slotList: C_GetSlotList returned = 0X%x number of slots = %d"), p11Ret, tempCount);
	}
	return ret;
}

/* check if the card that is present in the slotID is in fact an eid card that we support*/
int eid_vwr_p11_token_supported(CK_SLOT_ID_PTR slotID) {
	CK_RV p11Ret = CKR_OK;
	CK_TOKEN_INFO tokenInfo;

	p11Ret = C_GetTokenInfo(*slotID, &tokenInfo);
	if (p11Ret == CKR_OK)
	{
		return EIDV_RV_OK;
	}
	if (p11Ret != CKR_TOKEN_NOT_RECOGNIZED)
	{
		be_log(EID_VWR_LOG_ERROR, TEXT("eid_vwr_p11_token_supported failed getting slot info: C_GetTokenInfo returned = 0x%08x "), p11Ret);
	}
	return EIDV_RV_FAIL;
}


/* try to find an eID card depending on the preferences that are set 
 * will return EIDV_RV_OK if a card is found, EIDV_RV_FAIL otherwise
 * if a card is found, slotID will be its slot number
 */
int eid_vwr_p11_find_eid_card(CK_SLOT_ID_PTR slotID ) {
	CK_RV p11Ret;
	CK_SLOT_ID_PTR slotList;
	CK_TOKEN_INFO tokenInfo;

	CK_ULONG slotCount = 0;
	CK_ULONG counter;

	if (is_auto)
	{
		slotList = (CK_SLOT_ID_PTR)malloc(sizeof(CK_SLOT_ID));
		if (slotList == NULL)
		{
			return EIDV_RV_FAIL;
		}
		p11Ret = C_GetSlotList(CK_TRUE, slotList, &slotCount);

		if (slotCount == 0)
		{
			free(slotList);
			return EIDV_RV_FAIL;//no card found
		}
		else if (p11Ret == CKR_BUFFER_TOO_SMALL)
		{
			slotList = (CK_SLOT_ID_PTR)calloc(sizeof(CK_SLOT_ID), slotCount);

			if (slotList == NULL)
			{
				return EIDV_RV_FAIL;
			}

			p11Ret = C_GetSlotList(CK_TRUE, slotList, &slotCount);
			if (p11Ret != CKR_OK)
			{
				free(slotList);
				return EIDV_RV_FAIL;
			}
		}

		//we retrieved a list of tokens, now check which one of them are eID cards
		for (counter = 0; counter < slotCount; counter++)
		{
			p11Ret = C_GetTokenInfo(slotList[counter], &tokenInfo);
			if (p11Ret == CKR_OK)
			{
				*slotID = slotList[counter];
				free(slotList);
				return EIDV_RV_OK;
			}
		}
		
		//we did not get any ok return from the tokens found, so none of them are eID cards
		free(slotList);
		return EIDV_RV_FAIL;
	}
	else
	{
		CK_SLOT_INFO info;
		p11Ret = C_GetSlotInfo(slot_manual, &info);

		if ((p11Ret == CKR_OK) && ((info.flags & CKF_TOKEN_PRESENT) == CKF_TOKEN_PRESENT))
		{
			//token found, now check if we support it
			p11Ret = C_GetTokenInfo(slot_manual, &tokenInfo);
			if (p11Ret == CKR_OK)
			{
				*slotID = slot_manual;
				return EIDV_RV_OK;
			}	
		}
	}
	return EIDV_RV_FAIL;
}
/* Wait untill pkcs11 detects a slot event
 * 
 * returns EIDV_RV_OK in case a state change was detected
 * returns EIDV_RV_FAIL in case no state change was detected
 * returns EIDV_RV_TERMINATE in case pkcs11 was finalized and caller need to stop calling this function
 */

int eid_vwr_p11_wait_for_slot_event(BOOLEAN blocking, CK_SLOT_ID_PTR pSlotID)
{
	CK_RV p11Ret = CKR_OK;
	CK_FLAGS flags = 0;

	if (blocking == FALSE)
	{
		flags = CKF_DONT_BLOCK;
	}

	p11Ret = C_WaitForSlotEvent(flags,   /*nonblocking flag: CKF_DONT_BLOCK*/
		pSlotID,  /* location that receives the slot ID */
		NULL_PTR); /* reserved.  Should be NULL_PTR */

	if (p11Ret != CKR_OK)
	{
		if (p11Ret == CKR_NO_EVENT)
		{
			be_log(EID_VWR_LOG_DETAIL, TEXT("C_WaitForSlotEvent with CKR_NO_EVENT, retVal: %.8x"), p11Ret);
			return EIDV_RV_FAIL;
		}
		
		if (p11Ret == CKR_CRYPTOKI_NOT_INITIALIZED)
		{
			be_log(EID_VWR_LOG_ERROR, TEXT("C_WaitForSlotEvent returned EIDV_RV_TERMINATE"));
			return EIDV_RV_TERMINATE;
		}
		if (CKR_FUNCTION_CANCELED)
		{
			be_log(EID_VWR_LOG_NORMAL, TEXT("C_WaitForSlotEvent returned CKR_FUNCTION_CANCELED"), p11Ret);
		}
		else
		{
			be_log(EID_VWR_LOG_ERROR, TEXT("C_WaitForSlotEvent with retVal: %.8x"), p11Ret);
		}
		SLEEP(1);
		return EIDV_RV_FAIL;
	}
	return EIDV_RV_OK;
}

#endif
