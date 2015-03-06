#include <oslayer.h>
#include <state.h>
#include <p11.h>

struct eid_vwr_deserialize_info {
	void* data;
	int len;
};

int eid_vwr_deserialize(void* data, int len) {
	struct eid_vwr_deserialize_info i;
	i.data = data;
	i.len = len;
	sm_handle_event(EVENT_OPEN_FILE, &i);
	return 0;
}

int eid_vwr_createcallbacks(struct eid_vwr_ui_callbacks* cb_) {
	sm_handle_event(EVENT_SET_CALLBACKS, cb_);
	/*assert(cb == NULL);
	cb = cb_;
	cb->newsrc(EID_VWR_SRC_NONE);
	check_rv(C_Initialize(NULL_PTR));*/
	return 0;
}

void eid_vwr_be_mainloop() {
	for(;;) {
		eid_vwr_poll();
		sleep(1);
	}
}

void eid_vwr_poll() {
	CK_SLOT_ID tmp;

	if(p11_find_first_slot(&tmp) == EIDV_RV_OK) {
		sm_handle_event(EVENT_TOKEN_INSERTED, &tmp);
		/*if(!had_slot || (tmp != prev_slot)) {
			cb->log(EID_VWR_LOG_NORMAL, "found a card in slot %lu", tmp);
			prev_slot = tmp;
			cb->newsrc(EID_VWR_SRC_CARD);
			had_slot = (read_card(prev_slot) == EIDV_RV_OK);
		}*/
	} else {
		sm_handle_event(EVENT_TOKEN_REMOVED, &tmp);
		/*
		if(had_slot) {
			cb->log(EID_VWR_LOG_NORMAL, "card gone, clearing data");
			cb->newsrc(EID_VWR_SRC_NONE);
		}
		had_slot = 0;*/
	}
}
