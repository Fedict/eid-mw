#include <eid-viewer/oslayer.h>
#include <state.h>
#include <p11.h>
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <string.h>
#include <cache.h>
#include "backend.h"

int eid_vwr_createcallbacks(struct eid_vwr_ui_callbacks* cb_) {
	sm_init();
	sm_handle_event(EVENT_SET_CALLBACKS, cb_, NULL, NULL);
	return 0;
}

#ifdef WIN32
DWORD WINAPI eid_vwr_wait_for_pkcs11_event_loop(void* val) {
	int ret;
	CK_FLAGS flags = 0;
	CK_SLOT_ID slotID = 0;

	//fire a EVENT_DEVICE_CHANGED event in order to detect the slots and cards in the state thread
	sm_handle_event(EVENT_DEVICE_CHANGED, &slotID, NULL, NULL);

	while (1)
	{
		ret = eid_vwr_p11_wait_for_slot_event(TRUE, &slotID);

		if (ret == EIDV_RV_OK)
		{
			CK_SLOT_ID* pslotID = malloc(sizeof(CK_SLOT_ID)); //will be freed by statehandler
			*pslotID = slotID;
			sm_handle_event(EVENT_DEVICE_CHANGED, pslotID, free, NULL);
		}
		else if (ret == EIDV_RV_TERMINATE)
		{
			return 0;
		}
		//according to pkcs#11v2.20 standard, C_GetSlotList need to be called (with NULL) again before the new slots will be taken into account
		//if a new reader is added and we would start waiting (blocked) for a new event before the mainloop has updated the slotlist, 
		//we would not be getting the events of the new reader untill the slotlist is updated. (C_WaitForSlotEvent will not notify us because in the 
		//old slot list, no change was detected)
	}
	return ret;
}
#else
	void* eid_vwr_be_mainloop(void* val EIDV_UNUSED) {
	for (;;) {
		eid_vwr_poll();
		SLEEP(1);
		}
	}

int eid_vwr_poll() {
	CK_SLOT_ID_PTR no_token = (CK_SLOT_ID_PTR)malloc(sizeof(CK_SLOT_ID));
	CK_SLOT_ID_PTR token = (CK_SLOT_ID_PTR)malloc(sizeof(CK_SLOT_ID));
	static CK_ULONG count_old = 0;
	static CK_SLOT_ID token_old = 0xCAFEBABE;
	CK_ULONG count = 0;
	CK_ULONG i;
	int retval = 0;

	if(eid_vwr_p11_find_first_slot(CK_FALSE, no_token, &count) == EIDV_RV_OK) {
		sm_handle_event(EVENT_READER_FOUND, no_token, free, NULL);
	} else {
		free(no_token);
		//pkcs11 not ready yet?
		retval = 1;
	}
	if(count == 0) {
		sm_handle_event(EVENT_READER_LOST, NULL, NULL, NULL);
	}
	if(count_old != count) {
		slotdesc* slots = (slotdesc*)malloc(count * sizeof(slotdesc));
		memset(slots, 0, count * sizeof(slotdesc));
		eid_vwr_p11_name_slots(slots, &count);
		if(be_readers_changed(count, slots) == EIDV_RV_OK) {
			count_old = count;
		}
		else
		{
			//no callback present
			retval = 2;
		}
		for (i = 0; i < count; i++)
		{
			if (slots[i].description != NULL) {
				free(slots[i].description);
			}
		}
		free (slots);
	}
	if(eid_vwr_p11_find_first_slot(CK_TRUE, token, &count) == EIDV_RV_OK) {
		if(token_old != *token) {
			CK_SLOT_ID_PTR tmp = malloc(sizeof(CK_SLOT_ID));
			*tmp = *token;
			sm_handle_event(EVENT_TOKEN_REMOVED, tmp, free, NULL);
			token_old = *token;
		}
		sm_handle_event(EVENT_TOKEN_INSERTED, token, free, NULL);
	} else {
		sm_handle_event(EVENT_TOKEN_REMOVED, token, free, NULL);
	}
	return retval;
}
#endif

void eid_vwr_be_serialize(const EID_CHAR* target_file) {
    if (target_file != NULL)
    {
        EID_CHAR* copy = EID_STRDUP(target_file);
        sm_handle_event(EVENT_SERIALIZE, copy, free, NULL);
    } else {
        sm_handle_event(EVENT_SERIALIZE, (EID_CHAR *)target_file, free, NULL);
    }
}

void eid_vwr_be_deserialize(const EID_CHAR* source_file) {
    if (source_file != NULL)
    {
        EID_CHAR* copy = EID_STRDUP(source_file);
        sm_handle_event(EVENT_OPEN_FILE, copy, free, NULL);
    } else {
        sm_handle_event(EVENT_OPEN_FILE, (EID_CHAR *)source_file, free, NULL);
    }
}

const char* eid_vwr_be_get_xmlform() {
	const struct eid_vwr_cache_item* item;
	/* When we have card data, the state machine ensures that the
	 * cache will contain the XML form of the data under the "xml"
	 * label. Thus, if that cache entry is empty, this means we
	 * don't have any data yet. */
	if(!cache_have_label(TEXT("xml"))) {
		return NULL;
	}
	item = cache_get_data(TEXT("xml"));
	return (const char*)item->data;
}

void eid_vwr_be_select_slot(int automatic, unsigned long manualslot) {
	eid_vwr_p11_select_slot(automatic ? CK_TRUE : CK_FALSE, (CK_SLOT_ID)manualslot);
}

void eid_vwr_be_set_invalid() {
	sm_handle_event(EVENT_DATA_INVALID, NULL, NULL, NULL);
}

void eid_vwr_close_file() {
	sm_handle_event(EVENT_CLOSE_FILE, NULL, NULL, NULL);
}
