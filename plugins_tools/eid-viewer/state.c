#include "state.h"
#include "p11.h"
#include "backend.h"
#include <stdlib.h>

enum states {
	STATE_LIBOPEN,
	STATE_CALLBACKS,
	STATE_READY,
	STATE_TOKEN,
	STATE_TOKEN_WAIT,
	STATE_TOKEN_ID,
	STATE_TOKEN_CERTS,
	STATE_TOKEN_PINOP,
	STATE_FILE,

	STATE_COUNT,
};

const char* state_to_name(enum states state) {
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
	STATE_NAME(FILE);
#undef STATE_NAME
	}
	return "unknown state";
}

const char* event_to_name(enum eid_vwr_state_event event) {
	switch(event) {
#define EVENT_NAME(e) case EVENT_##e: return #e
	EVENT_NAME(SET_CALLBACKS);
	EVENT_NAME(OPEN_FILE);
	EVENT_NAME(CLOSE_FILE);
	EVENT_NAME(TOKEN_INSERTED);
	EVENT_NAME(TOKEN_REMOVED);
	EVENT_NAME(READ_READY);
	EVENT_NAME(DO_PINOP);
	}
	return "unknown event";
}

struct state {
	enum states me;
	struct state* out[EVENT_COUNT];
	struct state* first_child;
	struct state* parent;
	int(*enter)(void*data);
	int(*leave)();
};

static struct state states[STATE_COUNT];
static struct state* curstate;

int do_initialize(void*data) {
	p11_init();
	be_setcallbacks(data);
	be_newsource(EID_VWR_SRC_NONE);
}

int do_parse_file(void*data) {
}

int source_none(void*data) {
}

int do_pinop(void*which) {
}

int do_end_pinop() {
}

void sm_init() {
	int i;
	for(i=0;i<STATE_COUNT;i++) {
		states[i].me = (enum states)i;
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
	states[STATE_TOKEN].enter = p11_open_session;
	states[STATE_TOKEN].leave = p11_close_session;
	states[STATE_TOKEN].out[EVENT_TOKEN_REMOVED] = &(states[STATE_READY]);

	states[STATE_TOKEN_ID].parent = &(states[STATE_TOKEN]);
	states[STATE_TOKEN_ID].enter = p11_read_id;
	states[STATE_TOKEN_ID].leave = p11_finalize_find;
	states[STATE_TOKEN_ID].out[EVENT_READ_READY] = &(states[STATE_TOKEN_CERTS]);

	states[STATE_TOKEN_CERTS].parent = &(states[STATE_TOKEN]);
	states[STATE_TOKEN_CERTS].enter = p11_read_certs;
	states[STATE_TOKEN_CERTS].leave = p11_finalize_find;
	states[STATE_TOKEN_CERTS].out[EVENT_READ_READY] = &(states[STATE_TOKEN_WAIT]);

	states[STATE_TOKEN_PINOP].parent = &(states[STATE_TOKEN]);
	states[STATE_TOKEN_PINOP].enter = do_pinop;
	states[STATE_TOKEN_PINOP].leave = do_end_pinop;
	states[STATE_TOKEN_PINOP].out[EVENT_READ_READY] = &(states[STATE_TOKEN_WAIT]);

	states[STATE_TOKEN_WAIT].parent = &(states[STATE_TOKEN]);
	states[STATE_TOKEN_WAIT].out[EVENT_DO_PINOP] = &(states[STATE_TOKEN_PINOP]);

	states[STATE_FILE].parent = &(states[STATE_CALLBACKS]);
	states[STATE_FILE].enter = do_parse_file;
	states[STATE_FILE].out[EVENT_CLOSE_FILE] = &(states[STATE_READY]);
	states[STATE_FILE].out[EVENT_TOKEN_INSERTED] = &(states[STATE_TOKEN]);

	curstate = &(states[STATE_LIBOPEN]);
}

void parent_enter_recursive(struct state* start, struct state* end) {
	if(start == end) {
		return;
	}
	parent_enter_recursive(start->parent, end);
	if(start != NULL) {
		be_log(EID_VWR_LOG_DETAIL, "Entering state %s (parent)", state_to_name(start->me));
		if(start->enter != NULL) {
			start->enter(NULL);
		}
	}
}

void sm_handle_event(enum eid_vwr_state_event e, void* data) {
	struct state *thistree, *targettree, *cmnanc, *hold, *target, *oldstate;

	/* We want to be able to detect when a state transition has happened recursively... */
	hold = oldstate = curstate;

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
		curstate->leave();
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

	/* If the target state has parent states that don't share a common
	 * ancestor with the (previously) current state, call their "enter"
	 * function -- but without passing on any data */
	parent_enter_recursive(oldstate->parent, cmnanc);
	/* Call the target state's "enter" function, and pass on the data that
	 * we got from the event */
	if(curstate->enter != NULL) {
		curstate->enter(data);
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
		if(curstate->enter != NULL) {
			curstate->enter(NULL);
		}
		if(hold != curstate) {
			be_log(EID_VWR_LOG_DETAIL, "State transition detected, aborting handling of %s", event_to_name(e));
			return;
		}
	}
	be_log(EID_VWR_LOG_DETAIL, "State transition for %s complete", event_to_name(e));
}
