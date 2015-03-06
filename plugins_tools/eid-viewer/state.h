#ifndef EID_VWR_STATE_H
#define EID_VWR_STATE_H

enum eid_vwr_state_event {
	EVENT_SET_CALLBACKS,
	EVENT_OPEN_FILE,
	EVENT_CLOSE_FILE,
	EVENT_TOKEN_INSERTED,
	EVENT_TOKEN_REMOVED,
	EVENT_READ_READY,
	EVENT_DO_PINOP,

	EVENT_COUNT
};

void sm_init();
void sm_handle_event(enum eid_vwr_state_event e, void* data);

#endif
