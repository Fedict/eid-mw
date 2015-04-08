#include "state.h"
#include "p11.h"
#include "backend.h"
#include "xml.h"
#include "cache.h"
#include <stdlib.h>

static const char* state_to_name(enum eid_vwr_states state) {
	switch(state) {
#define STATE_NAME(s) case STATE_##s: return #s
	STATE_NAME(LIBOPEN);
	STATE_NAME(CALLBACKS);
	STATE_NAME(READY);
	STATE_NAME(TOKEN);
	STATE_NAME(TOKEN_WAIT);
	STATE_NAME(TOKEN_ID);
	STATE_NAME(TOKEN_CERTS);
	STATE_NAME(TOKEN_PINOP);
	STATE_NAME(TOKEN_SERIALIZE);
	STATE_NAME(TOKEN_ERROR);
	STATE_NAME(FILE);
	STATE_NAME(CARD_INVALID);
#undef STATE_NAME
	default:
		return "unknown state";
	}
}

static const char* event_to_name(enum eid_vwr_state_event event) {
	switch(event) {
#define EVENT_NAME(e) case EVENT_##e: return #e
	EVENT_NAME(SET_CALLBACKS);
	EVENT_NAME(OPEN_FILE);
	EVENT_NAME(CLOSE_FILE);
	EVENT_NAME(TOKEN_INSERTED);
	EVENT_NAME(TOKEN_REMOVED);
	EVENT_NAME(READ_READY);
	EVENT_NAME(DO_PINOP);
	EVENT_NAME(STATE_ERROR);
	EVENT_NAME(DATA_INVALID);
	EVENT_NAME(SERIALIZE);
#undef EVENT_NAME
	default:
		return "unknown event";
	}
}

struct state {
	enum eid_vwr_states me;
	struct state* out[EVENT_COUNT];
	struct state* first_child;
	struct state* parent;
	int(*enter)(void*data);
	int(*leave)();
};

static struct state states[STATE_COUNT];
static struct state* curstate;

static int do_initialize(void*data) {
	eid_vwr_p11_init();
	be_setcallbacks(data);

	return 0;
}

static int do_parse_file(void*data) {

	return 0;
}

static int source_none(void*data) {
	be_newsource(EID_VWR_SRC_NONE);

	return 0;
}

void sm_init() {
	int i;
	for(i=0;i<STATE_COUNT;i++) {
		states[i].me = (enum eid_vwr_states)i;
	}
	states[STATE_LIBOPEN].out[EVENT_SET_CALLBACKS] = &(states[STATE_CALLBACKS]);

	states[STATE_CALLBACKS].first_child = &(states[STATE_READY]);
	states[STATE_CALLBACKS].enter = do_initialize;

	states[STATE_READY].parent = &(states[STATE_CALLBACKS]);
	states[STATE_READY].enter = source_none;
	states[STATE_READY].out[EVENT_OPEN_FILE] = &(states[STATE_FILE]);
	states[STATE_READY].out[EVENT_TOKEN_INSERTED] = &(states[STATE_TOKEN]);

	states[STATE_TOKEN].parent = &(states[STATE_CALLBACKS]);
	states[STATE_TOKEN].first_child = &(states[STATE_TOKEN_ID]);
	states[STATE_TOKEN].enter = eid_vwr_p11_open_session;
	states[STATE_TOKEN].leave = eid_vwr_p11_close_session;
	states[STATE_TOKEN].out[EVENT_TOKEN_REMOVED] = &(states[STATE_READY]);
	states[STATE_TOKEN].out[EVENT_DATA_INVALID] = &(states[STATE_CARD_INVALID]);

	states[STATE_CARD_INVALID].out[EVENT_TOKEN_REMOVED] = &(states[STATE_READY]);

	states[STATE_TOKEN_ID].parent = &(states[STATE_TOKEN]);
	states[STATE_TOKEN_ID].enter = eid_vwr_p11_read_id;
	states[STATE_TOKEN_ID].leave = eid_vwr_p11_finalize_find;
	states[STATE_TOKEN_ID].out[EVENT_READ_READY] = &(states[STATE_TOKEN_CERTS]);
	states[STATE_TOKEN_ID].out[EVENT_STATE_ERROR] = &(states[STATE_TOKEN_ERROR]);

	states[STATE_TOKEN_CERTS].parent = &(states[STATE_TOKEN]);
	states[STATE_TOKEN_CERTS].enter = eid_vwr_p11_read_certs;
	states[STATE_TOKEN_CERTS].leave = eid_vwr_p11_finalize_find;
	states[STATE_TOKEN_CERTS].out[EVENT_READ_READY] = &(states[STATE_TOKEN_WAIT]);
	states[STATE_TOKEN_CERTS].out[EVENT_STATE_ERROR] = &(states[STATE_TOKEN_ERROR]);

	states[STATE_TOKEN_PINOP].parent = &(states[STATE_TOKEN]);
	states[STATE_TOKEN_PINOP].enter = eid_vwr_p11_do_pinop;
	states[STATE_TOKEN_PINOP].leave = eid_vwr_p11_leave_pinop;
	states[STATE_TOKEN_PINOP].out[EVENT_READ_READY] = &(states[STATE_TOKEN_WAIT]);
	states[STATE_TOKEN_PINOP].out[EVENT_STATE_ERROR] = &(states[STATE_TOKEN_WAIT]);

	states[STATE_TOKEN_WAIT].parent = &(states[STATE_TOKEN]);
	states[STATE_TOKEN_WAIT].out[EVENT_DO_PINOP] = &(states[STATE_TOKEN_PINOP]);
	states[STATE_TOKEN_WAIT].out[EVENT_SERIALIZE] = &(states[STATE_TOKEN_SERIALIZE]);

	states[STATE_TOKEN_ERROR].parent = &(states[STATE_TOKEN]);

	states[STATE_TOKEN_SERIALIZE].parent = &(states[STATE_TOKEN]);
	states[STATE_TOKEN_SERIALIZE].enter = eid_vwr_serialize;
	states[STATE_TOKEN_SERIALIZE].out[EVENT_READ_READY] = &(states[STATE_TOKEN_WAIT]);
	states[STATE_TOKEN_SERIALIZE].out[EVENT_STATE_ERROR] = &(states[STATE_TOKEN_ERROR]);

	states[STATE_FILE].parent = &(states[STATE_CALLBACKS]);
	states[STATE_FILE].enter = eid_vwr_deserialize;
	states[STATE_FILE].leave = cache_clear;
	states[STATE_FILE].out[EVENT_CLOSE_FILE] = &(states[STATE_READY]);
	states[STATE_FILE].out[EVENT_TOKEN_INSERTED] = &(states[STATE_TOKEN]);

	curstate = &(states[STATE_LIBOPEN]);

	sm_start_thread();
}

static void parent_enter_recursive(struct state* start, struct state* end, enum eid_vwr_state_event e) {
	if(start == end) {
		return;
	}
	if(start != NULL) {
		parent_enter_recursive(start->parent, end, e);
		be_log(EID_VWR_LOG_DETAIL, "Entering state %s (parent)", state_to_name(start->me));
		if(start->enter != NULL) {
			if(start->enter(NULL) != 0 && e != EVENT_STATE_ERROR) {
				sm_handle_event_onthread(EVENT_STATE_ERROR, NULL);
			}
		}
	}
}

void sm_handle_event_onthread(enum eid_vwr_state_event e, void* data) {
	struct state *thistree, *targettree, *cmnanc, *hold, *target;

	/* We want to be able to detect when a state transition has happened recursively... */
	hold = curstate;

	/* First, check if the given event has any relevance to our current
	 * state; if so, store the target state */
	if(curstate->out[e]) {
		target = curstate->out[e];
	} else {
		thistree = curstate->parent;
		while(thistree && thistree->out[e] == NULL) {
			thistree = thistree->parent;
		}
		if(!thistree) {
			return; // event is irrelevant for this state
		}
		target = thistree->out[e];
	}
	/* Now, see if we need to perform a leave() function before leaving the current state */
	be_log(EID_VWR_LOG_DETAIL, "Handling state transition for event %s", event_to_name(e));
	be_log(EID_VWR_LOG_DETAIL, "Leaving state %s", state_to_name(curstate->me));
	if(curstate->leave != NULL) {
		if(curstate->leave() != 0 && e != EVENT_STATE_ERROR) {
			be_log(EID_VWR_LOG_ERROR, "state transition failed");
			sm_handle_event_onthread(EVENT_STATE_ERROR, NULL);
		}
	}
	if(hold != curstate) {
		be_log(EID_VWR_LOG_DETAIL, "State transition detected, aborting handling of %s", event_to_name(e));
		return;
	}
	/* Find the first common ancestor (if any) of the current state and the target state */
	cmnanc = NULL;
	for(thistree=curstate->parent; thistree != NULL; thistree = thistree->parent) {
		for(targettree = target->parent; targettree != NULL; targettree = targettree->parent) {
			if(thistree == targettree) {
				cmnanc = thistree;
				goto exit_loop;
			}
		}
	}
exit_loop:
	/* Call the "leave" method of all parent states of the current state
	 * that don't share a common ancestor with the target state */
	for(thistree = curstate->parent; thistree != cmnanc; thistree = thistree->parent) {
		be_log(EID_VWR_LOG_DETAIL, "Leaving state %s", state_to_name(thistree->me));
		if(thistree->leave != NULL) {
			thistree->leave();
		}
		if(hold != curstate) {
			be_log(EID_VWR_LOG_DETAIL, "State transition detected, aborting handling of %s", event_to_name(e));
			return;
		}
	}

	/* Now do the actual state transition */
	be_log(EID_VWR_LOG_DETAIL, "Entering state %s (target)", state_to_name(target->me));
	hold = curstate = target;
	be_newstate(curstate->me);

	/* If the target state has parent states that don't share a common
	 * ancestor with the (previously) current state, call their "enter"
	 * function -- but without passing on any data */
	parent_enter_recursive(curstate->parent, cmnanc, e);
	/* Call the target state's "enter" function, and pass on the data that
	 * we got from the event */
	if(curstate->enter != NULL) {
		if(curstate->enter(data) != 0 && e != EVENT_STATE_ERROR) {
			sm_handle_event_onthread(EVENT_STATE_ERROR, NULL);
		}
	}
	if(hold != curstate) {
		be_log(EID_VWR_LOG_DETAIL, "State transition detected, aborting handling of %s", event_to_name(e));
		return;
	}
	/* If the target state has a "first child" state, enter that state
	 * instead, not passing on any data. */
	while(curstate->first_child != NULL) {
		be_log(EID_VWR_LOG_DETAIL, "Entering state %s (child)", state_to_name(curstate->first_child->me));
		hold = curstate = curstate->first_child;
		be_newstate(curstate->me);
		if(curstate->enter != NULL) {
			if(curstate->enter(NULL) != 0 && e != EVENT_STATE_ERROR) {
				sm_handle_event_onthread(EVENT_STATE_ERROR, NULL);
			}
		}
		if(hold != curstate) {
			be_log(EID_VWR_LOG_DETAIL, "State transition detected, aborting handling of %s", event_to_name(e));
			return;
		}
	}
	be_log(EID_VWR_LOG_DETAIL, "State transition for %s complete", event_to_name(e));
}

int sm_state_is_active(enum eid_vwr_states s) {
	struct state* ptr = curstate;
	while(ptr && ptr->me != s) {
		ptr = ptr->parent;
	}
	if(ptr) {
		return 1;
	} else {
		return 0;
	}
}
