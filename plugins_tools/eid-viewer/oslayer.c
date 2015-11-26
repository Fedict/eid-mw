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
/*
// I keep getting marshalling errors when providing a structure pointer that contains the function pointers,
// so (for now) I'll provide the function pointers directly and allocate memory here
#ifdef WIN32
int eid_vwr_set_cbfuncs(		void(*newsrc)(enum eid_vwr_source), // data source has changed.
	void(*newstringdata)(const char* label, const char* data), // new string data to be displayed in UI.
	void(*newbindata)(const char* label, const void* data, int datalen), // new binary data to be displayed in UI.
	void(*log)(enum eid_vwr_loglevel, const char* line), // log a string at the given level.
	void(*logv)(enum eid_vwr_loglevel, const char* line, va_list ap), // log a string using varargs. Note: a UI needs to implement only one of log() or logv(); the backend will use whichever is implemented.
	void(*newstate)(enum eid_vwr_states), // issued at state machine transition
	void(*pinop_result)(enum eid_vwr_pinops, enum eid_vwr_result) // issued when a PIN operation finished.
	) {

	eid_vwr_ui_callbacks* cb_ = eid_vwr_cbstruct();
	cb_->newsrc = newsrc;
	cb_->newstringdata = newstringdata;
	cb_->newbindata = newbindata;
	cb_->log = log;
	cb_->logv = logv;
	cb_->newstate = newstate;
	cb_->pinop_result = pinop_result;

	be_setcallbacks(cb_);

	struct eid_vwr_ui_callbacks* retval = (struct eid_vwr_ui_callbacks*)calloc(sizeof(struct eid_vwr_ui_callbacks), 1);
	return retval;
}
#endif*/


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
	CK_SLOT_ID_PTR tmp = (CK_SLOT_ID_PTR)malloc(sizeof(CK_SLOT_ID));

	if(eid_vwr_p11_find_first_slot(tmp) == EIDV_RV_OK) {
		sm_handle_event(EVENT_TOKEN_INSERTED, tmp, free, NULL);
	} else {
		sm_handle_event(EVENT_TOKEN_REMOVED, tmp, free, NULL);
	}
}

void eid_vwr_be_serialize(const char* target_file) {
	char* copy = STRDUP(target_file);
	sm_handle_event(EVENT_SERIALIZE, copy, free, NULL);
}

void eid_vwr_be_deserialize(const char* source_file) {
	char* copy = STRDUP(source_file);
	sm_handle_event(EVENT_OPEN_FILE, copy, free, NULL);
}

const char* eid_vwr_be_get_xmlform() {
	const struct eid_vwr_cache_item* item;
	/* When we have card data, the state machine ensures that the
	 * cache will contain the XML form of the data under the "xml"
	 * label. Thus, if that cache entry is empty, this means we
	 * don't have any data yet. */
	if(!cache_have_label("xml")) {
		return NULL;
	}
	item = cache_get_data("xml");
	return (const char*)item->data;
}
