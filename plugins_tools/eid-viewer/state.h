#ifndef EID_VWR_STATE_H
#define EID_VWR_STATE_H

enum eid_vwr_state_event
{
	EVENT_SET_CALLBACKS,
	EVENT_OPEN_FILE,
	EVENT_CLOSE_FILE,
	EVENT_SERIALIZE,
	EVENT_TOKEN_INSERTED,
	EVENT_TOKEN_REMOVED,
	EVENT_READ_READY,
	EVENT_DO_PINOP,
	EVENT_SERIALIZE_READY,
	EVENT_PINOP_READY,
	EVENT_DATA_INVALID,
	EVENT_STATE_ERROR,
	EVENT_READER_FOUND,
	EVENT_DEVICE_CHANGED,
	EVENT_READER_LOST,
	EVENT_DO_CHALLENGE,
	EVENT_CHALLENGE_READY,

	EVENT_COUNT
};

struct error_data {
	int from;
	enum {
		SM_ENTER,
		SM_LEAVE
	} which;
	int error;
};

void sm_init(void);
void sm_handle_event(enum eid_vwr_state_event e, void *data,
		     void (*freefunc) (void *), void (*donefunc) (void *));
void sm_handle_event_onthread(enum eid_vwr_state_event e, void *data);
void sm_start_thread(void);
void sm_stop_thread(void);

#endif
