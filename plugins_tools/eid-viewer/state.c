#include "state.h"
#include "p11.h"
#include "backend.h"
#include "xml.h"
#include "dataverify.h"
#include "cache.h"
#include <stdlib.h>
#include <eid-util/utftranslate.h>
#ifndef _WIN32
#include <pthread.h>
#endif

/* Returns a string representation of the state name */
static const EID_CHAR* state_to_name(enum eid_vwr_states state) {
	switch(state) {
#define STATE_NAME(s) case STATE_##s: return TEXT(#s);
	STATE_NAME(LIBOPEN);
	STATE_NAME(CALLBACKS);
	STATE_NAME(READY);
	STATE_NAME(TOKEN);
	STATE_NAME(TOKEN_WAIT);
	STATE_NAME(TOKEN_ID);
	STATE_NAME(TOKEN_CERTS);
	STATE_NAME(TOKEN_IDLE);
	STATE_NAME(TOKEN_PINOP);
	STATE_NAME(TOKEN_SERIALIZE);
	STATE_NAME(TOKEN_ERROR);
	STATE_NAME(FILE);
	STATE_NAME(FILE_READING);
	STATE_NAME(FILE_WAIT);
	STATE_NAME(CARD_INVALID);
	STATE_NAME(NO_READER);
	STATE_NAME(NO_TOKEN);
	STATE_NAME(TOKEN_CHALLENGE);

#undef STATE_NAME
	default:
		return TEXT("unknown state");
	}
}

/* Returns a string representation of the event name */
static const EID_CHAR* event_to_name(enum eid_vwr_state_event event) {
	switch(event) {
#define EVENT_NAME(e) case EVENT_##e: return TEXT(#e);
	EVENT_NAME(SET_CALLBACKS);
	EVENT_NAME(OPEN_FILE);
	EVENT_NAME(CLOSE_FILE);
	EVENT_NAME(TOKEN_INSERTED);
	EVENT_NAME(TOKEN_REMOVED);
	EVENT_NAME(READ_READY);
	EVENT_NAME(PINOP_READY);
	EVENT_NAME(SERIALIZE_READY);
	EVENT_NAME(DO_PINOP);
	EVENT_NAME(STATE_ERROR);
	EVENT_NAME(DATA_INVALID);
	EVENT_NAME(SERIALIZE);
	EVENT_NAME(READER_FOUND);
	EVENT_NAME(DEVICE_CHANGED);
	EVENT_NAME(READER_LOST);
	EVENT_NAME(DO_CHALLENGE);
	EVENT_NAME(CHALLENGE_READY);

#undef EVENT_NAME
	default:
		return TEXT("unknown event");
	}
}

/* State structure */
struct state {
	enum eid_vwr_states me; /* Our number */
	struct state* out[EVENT_COUNT]; /* Target states to transition
					   to when the given event
					   occurs. May be NULL, which
					   means to ignore the event */
	struct state* first_child; /* If this state has child states,
				      then transitioning to this state
				      will cause the machine to
				      immediately also enter the child
				      state */
	struct state* parent; /* The parent state of this state, if any */
	int(*enter)(void*data); /* Function called when we enter this
				   state. The argument will be NULL
				   except if this state is the target
				   state; parent states or child states
				   of the target state will have their
				   enter() function called, but will not
				   get any data. */
    int(*leave)(void); /* Function called when we leave the current
			  state (directly or by leaving its parent) */
};

static struct state states[STATE_COUNT]; /* The state table */
static struct state* curstate; /* Current state. Defaults to 0=LIBOPEN */

/* enter() function of CALLBACKS state */
static int do_initialize(void*data) {
	eid_vwr_p11_init();
	be_setcallbacks((struct eid_vwr_ui_callbacks*)data);

#ifdef WIN32
	HANDLE thread = NULL;
	thread = CreateThread(NULL, 0, eid_vwr_wait_for_pkcs11_event_loop, NULL, 0, NULL);
#else
	pthread_t thread;
	pthread_create(&thread, NULL, eid_vwr_be_mainloop, NULL);
#endif

	return 0;
}

/* Called whenever we enter the STATE_NO_TOKEN state. */
static int source_none(void*data EIDV_UNUSED) {
	be_newsource(EID_VWR_SRC_NONE);

	return 0;
}

/* called when we enter the TOKEN_WAIT state. */
static int enter_token_wait(void* data) {
	int rv;
	if((rv = eid_vwr_verify_card(data)) != 0) {
		sm_handle_event_onthread(EVENT_DATA_INVALID, NULL);
		return 0;
	}
	return eid_vwr_gen_xml(data);
}

/* called when we enter the FILE_WAIT state. */
static int enter_file_wait(void* data) {
	return eid_vwr_gen_xml(data);
}

/* Initialize the state machine.
   Please see be-statemach.uml in the uml directory for more details */
void sm_init() {
	int i;
	if(states[STATE_COUNT-1].me == STATE_COUNT-1) {
		/* state machine is already initialized */
		return;
	}
	for(i=0;i<STATE_COUNT;i++) {
		states[i].me = (enum eid_vwr_states)i;
	}
	states[STATE_LIBOPEN].out[EVENT_SET_CALLBACKS] = &(states[STATE_CALLBACKS]);

	states[STATE_CALLBACKS].first_child = &(states[STATE_NO_TOKEN]);
	states[STATE_CALLBACKS].enter = do_initialize;

	states[STATE_READY].parent = &(states[STATE_NO_TOKEN]);
	states[STATE_READY].enter = eid_vwr_p11_check_version;
	states[STATE_READY].out[EVENT_TOKEN_INSERTED] = &(states[STATE_TOKEN]);
	states[STATE_READY].out[EVENT_READER_LOST] = &(states[STATE_NO_READER]);

	states[STATE_TOKEN].parent = &(states[STATE_CALLBACKS]);
	states[STATE_TOKEN].first_child = &(states[STATE_TOKEN_ID]);
	states[STATE_TOKEN].enter = eid_vwr_p11_open_session;
	states[STATE_TOKEN].leave = eid_vwr_p11_close_session;
	states[STATE_TOKEN].out[EVENT_TOKEN_REMOVED] = &(states[STATE_READY]);
	states[STATE_TOKEN].out[EVENT_DATA_INVALID] = &(states[STATE_CARD_INVALID]);
	states[STATE_TOKEN].out[EVENT_READER_LOST] = &(states[STATE_NO_READER]);

	states[STATE_CARD_INVALID].parent = &(states[STATE_CALLBACKS]);
	states[STATE_CARD_INVALID].enter = source_none;
	states[STATE_CARD_INVALID].out[EVENT_TOKEN_REMOVED] = &(states[STATE_READY]);
	states[STATE_CARD_INVALID].out[EVENT_READER_LOST] = &(states[STATE_NO_READER]);

	states[STATE_TOKEN_ERROR].parent = &(states[STATE_TOKEN]);

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

	states[STATE_TOKEN_WAIT].parent = &(states[STATE_TOKEN]);
	states[STATE_TOKEN_WAIT].enter = enter_token_wait;
	states[STATE_TOKEN_WAIT].first_child = &(states[STATE_TOKEN_IDLE]);
	states[STATE_TOKEN_WAIT].out[EVENT_DO_PINOP] = &(states[STATE_TOKEN_PINOP]);
	states[STATE_TOKEN_WAIT].out[EVENT_SERIALIZE] = &(states[STATE_TOKEN_SERIALIZE]);
	states[STATE_TOKEN_WAIT].out[EVENT_DO_CHALLENGE] = &(states[STATE_TOKEN_CHALLENGE]);

	states[STATE_TOKEN_IDLE].parent = &(states[STATE_TOKEN_WAIT]);

	states[STATE_TOKEN_PINOP].parent = &(states[STATE_TOKEN_WAIT]);
	states[STATE_TOKEN_PINOP].enter = eid_vwr_p11_do_pinop;
	states[STATE_TOKEN_PINOP].leave = eid_vwr_p11_leave_pinop;
	states[STATE_TOKEN_PINOP].out[EVENT_PINOP_READY] = &(states[STATE_TOKEN_IDLE]);
	states[STATE_TOKEN_PINOP].out[EVENT_STATE_ERROR] = &(states[STATE_TOKEN_IDLE]);

	states[STATE_TOKEN_SERIALIZE].parent = &(states[STATE_TOKEN_WAIT]);
	states[STATE_TOKEN_SERIALIZE].enter = (int(*)(void*))eid_vwr_serialize;
	states[STATE_TOKEN_SERIALIZE].out[EVENT_SERIALIZE_READY] = &(states[STATE_TOKEN_IDLE]);
	states[STATE_TOKEN_SERIALIZE].out[EVENT_STATE_ERROR] = &(states[STATE_TOKEN_ERROR]);

	states[STATE_TOKEN_CHALLENGE].parent = &(states[STATE_TOKEN_WAIT]);
	states[STATE_TOKEN_CHALLENGE].enter = eid_vwr_p11_do_challenge;
	states[STATE_TOKEN_CHALLENGE].out[EVENT_CHALLENGE_READY] = &(states[STATE_TOKEN_IDLE]);
	states[STATE_TOKEN_CHALLENGE].out[EVENT_STATE_ERROR] = &(states[STATE_TOKEN_IDLE]);

	states[STATE_NO_TOKEN].parent = &(states[STATE_CALLBACKS]);
	states[STATE_NO_TOKEN].enter = source_none;
	states[STATE_NO_TOKEN].first_child = &(states[STATE_NO_READER]);
	states[STATE_NO_TOKEN].out[EVENT_OPEN_FILE] = &(states[STATE_FILE_READING]);

	states[STATE_NO_READER].parent = &(states[STATE_NO_TOKEN]);
	states[STATE_NO_READER].out[EVENT_READER_FOUND] = &(states[STATE_READY]);

	states[STATE_FILE].parent = &(states[STATE_CALLBACKS]);
	states[STATE_FILE].leave = cache_clear;
	states[STATE_FILE].first_child = &(states[STATE_FILE_READING]);
	states[STATE_FILE].out[EVENT_CLOSE_FILE] = &(states[STATE_NO_TOKEN]);
	states[STATE_FILE].out[EVENT_TOKEN_INSERTED] = &(states[STATE_TOKEN]);
	states[STATE_FILE].out[EVENT_STATE_ERROR] = &(states[STATE_NO_TOKEN]);

	states[STATE_FILE_READING].parent = &(states[STATE_FILE]);
	states[STATE_FILE_READING].enter = (int(*)(void*))eid_vwr_deserialize;
	states[STATE_FILE_READING].out[EVENT_READ_READY] = &(states[STATE_FILE_WAIT]);

	states[STATE_FILE_WAIT].parent = &(states[STATE_FILE]);
	states[STATE_FILE_WAIT].enter = enter_file_wait;

	curstate = &(states[STATE_LIBOPEN]);

	sm_start_thread();
}

/* Enter the parent states of the target state (that we're not currently
 * in yet), recursively. We will never enter a child state until we've
 * entered its parent state. */
static void parent_enter_recursive(struct state* start, struct state* end, enum eid_vwr_state_event e) {
	if(start == end) {
		return;
	}
	if(start != NULL) {
		parent_enter_recursive(start->parent, end, e);
		be_log(EID_VWR_LOG_DETAIL, TEXT("Entering state %s (parent)"), state_to_name(start->me));
		be_newstate(start->me);
		if(start->enter != NULL) {
			if(start->enter(NULL) != 0 && e != EVENT_STATE_ERROR) {
				sm_handle_event_onthread(EVENT_STATE_ERROR, NULL);
			}
		}
	}
}

/* Handle a state machine event. MUST be called on the state machine
 * thread; if not, undefined behaviour will occur. */
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
			if (e == EVENT_SERIALIZE)
			{
				//UI asked us to write the data to file, but our state has changed so we're no longer
				//able to do so. Report this to the UI
				be_log(EID_VWR_LOG_ERROR, TEXT("failed writing card data, current state is %s"), state_to_name(curstate->me));
			}
			return; // event is irrelevant for this state
		}
		target = thistree->out[e];
	}
	/* Now, see if we need to perform a leave() function before leaving the current state */
	be_log(EID_VWR_LOG_DETAIL, TEXT("Handling state transition for event %s"), event_to_name(e));
	be_log(EID_VWR_LOG_DETAIL, TEXT("Leaving state %s"), state_to_name(curstate->me));
	if(curstate->leave != NULL) {
		int rv;
		if((rv = curstate->leave()) != 0 && e != EVENT_STATE_ERROR) {
			be_log(EID_VWR_LOG_ERROR, TEXT("state transition failed"));
			struct error_data d = {
				.from = curstate->me,
				.which = SM_LEAVE,
				.error = rv,
			};
			sm_handle_event_onthread(EVENT_STATE_ERROR, &d);
		}
	}
	if(hold != curstate) {
		be_log(EID_VWR_LOG_DETAIL, TEXT("State transition detected, aborting handling of %s"), event_to_name(e));
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
		be_log(EID_VWR_LOG_DETAIL, TEXT("Leaving state %s"), state_to_name(thistree->me));
		if(thistree->leave != NULL) {
			thistree->leave();
		}
		if(hold != curstate) {
			be_log(EID_VWR_LOG_DETAIL, TEXT("State transition detected, aborting handling of %s"), event_to_name(e));
			return;
		}
	}
	/* Now do the actual state transition */
	hold = curstate = target;

	/* If the target state has parent states that don't share a common
	 * ancestor with the (previously) current state, call their "enter"
	 * function -- but without passing on any data */
	parent_enter_recursive(curstate->parent, cmnanc, e);
	/* Call the target state's "enter" function, and pass on the data that
	 * we got from the event */
	if(curstate->enter != NULL) {
		int rv;
		if((rv = curstate->enter(data)) != 0 && e != EVENT_STATE_ERROR) {
			struct error_data d = {
				.from = curstate->me,
				.which = SM_ENTER,
				.error = rv,
			};
			sm_handle_event_onthread(EVENT_STATE_ERROR, &d);
		}
	}
	/* Now report the actual state transition (after its parent's state entries have been reported)*/
	be_log(EID_VWR_LOG_DETAIL, TEXT("Entering state %s (target)"), state_to_name(target->me));	
	be_newstate(curstate->me);

	if(hold != curstate) {
		be_log(EID_VWR_LOG_DETAIL, TEXT("State transition detected, aborting handling of %s"), event_to_name(e));
		return;
	}
	/* If the target state has a "first child" state, enter that state
	 * instead, not passing on any data. */
	while(curstate->first_child != NULL) {
		be_log(EID_VWR_LOG_DETAIL, TEXT("Entering state %s (child)"), state_to_name(curstate->first_child->me));
		hold = curstate = curstate->first_child;
		be_newstate(curstate->me);
		if(curstate->enter != NULL) {
			if(curstate->enter(NULL) != 0 && e != EVENT_STATE_ERROR) {
				sm_handle_event_onthread(EVENT_STATE_ERROR, NULL);
			}
		}
		if(hold != curstate) {
			be_log(EID_VWR_LOG_DETAIL, TEXT("State transition detected, aborting handling of %s"), event_to_name(e));
			return;
		}
	}
	be_log(EID_VWR_LOG_DETAIL, TEXT("State transition for %s complete"), event_to_name(e));
}

/* Verify if the given state is active, either because it's the current
 * state or one of its parents */
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
