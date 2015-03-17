#include <oslayer.h>
#include <state.h>
#include <p11.h>
#include <unistd.h>

struct eid_vwr_deserialize_info {
	void* data;
	int len;
};

int eid_vwr_deserialize(void* data, int len, void(*write)(void*)) {
	struct eid_vwr_deserialize_info i;
	i.data = data;
	i.len = len;
	sm_handle_event(EVENT_OPEN_FILE, &i, NULL, write);
	return 0;
}

int eid_vwr_createcallbacks(struct eid_vwr_ui_callbacks* cb_) {
	sm_handle_event(EVENT_SET_CALLBACKS, cb_, NULL, NULL);
	return 0;
}

void eid_vwr_be_mainloop() {
	for(;;) {
		eid_vwr_poll();
		sleep(1);
	}
}

void eid_vwr_poll() {
	CK_SLOT_ID_PTR tmp = malloc(sizeof(CK_SLOT_ID));

	if(eid_vwr_p11_find_first_slot(tmp) == EIDV_RV_OK) {
		sm_handle_event(EVENT_TOKEN_INSERTED, tmp, free, NULL);
	} else {
		sm_handle_event(EVENT_TOKEN_REMOVED, tmp, free, NULL);
	}
}
