#include <oslayer.h>
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

void eid_vwr_be_mainloop() {
	for(;;) {
		eid_vwr_poll();
		SLEEP(1);
	}
}

void eid_vwr_poll() {
	CK_SLOT_ID_PTR no_token = (CK_SLOT_ID_PTR)malloc(sizeof(CK_SLOT_ID));
	CK_SLOT_ID_PTR token = (CK_SLOT_ID_PTR)malloc(sizeof(CK_SLOT_ID));
	static CK_ULONG count_old = 0;
	CK_ULONG count = 0;

	if(eid_vwr_p11_find_first_slot(CK_FALSE, no_token, &count) == EIDV_RV_OK) {
		sm_handle_event(EVENT_READER_FOUND, no_token, free, NULL);
	}
	if(count_old != count) {
		slotdesc slots[count];
		eid_vwr_p11_name_slots(slots, &count);
		if(be_readers_changed(count, slots) == EIDV_RV_OK) {
			count_old = count;
		}
	}
	if(eid_vwr_p11_find_first_slot(CK_TRUE, token, &count) == EIDV_RV_OK) {
		sm_handle_event(EVENT_TOKEN_INSERTED, token, free, NULL);
	} else {
		sm_handle_event(EVENT_TOKEN_REMOVED, token, free, NULL);
	}
}

void eid_vwr_be_serialize(const EID_CHAR* target_file) {
	EID_CHAR* copy = EID_STRDUP(target_file);
	sm_handle_event(EVENT_SERIALIZE, copy, free, NULL);
}

void eid_vwr_be_deserialize(const EID_CHAR* source_file) {
	EID_CHAR* copy = EID_STRDUP(source_file);
	sm_handle_event(EVENT_OPEN_FILE, copy, free, NULL);
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
