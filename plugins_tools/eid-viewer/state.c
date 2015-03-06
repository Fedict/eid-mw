#include "state.h"
#include <stdlib.h>

enum states {
	STATE_LIBOPEN,
	STATE_CALLBACKS,
	STATE_READY,
	STATE_TOKEN,
	STATE_FILE,

	STATE_COUNT,
};

struct state {
	struct state* out[EVENT_COUNT];
	struct state* first_child;
	struct state* parent;
	void(*enter)(void*data);
	void(*leave)();
};

static struct state states[STATE_COUNT];
static struct state* curstate;

void do_initialize(void*data) {
}

void do_open_session(void*data) {
}

void do_close_session(void*data) {
}

void do_parse_file(void*data) {
}

void source_none(void*data) {
}

void sm_init() {
	states[STATE_LIBOPEN].out[EVENT_SET_CALLBACKS] = &(states[STATE_CALLBACKS]);

	states[STATE_CALLBACKS].first_child = &(states[STATE_READY]);
	states[STATE_CALLBACKS].enter = do_initialize;

	states[STATE_READY].parent = &(states[STATE_CALLBACKS]);
	states[STATE_READY].enter = source_none;
	states[STATE_READY].out[EVENT_OPEN_FILE] = &(states[STATE_FILE]);
	states[STATE_READY].out[EVENT_TOKEN_INSERTED] = &(states[STATE_TOKEN]);

	states[STATE_TOKEN].parent = &(states[STATE_CALLBACKS]);
	states[STATE_TOKEN].enter = do_open_session;
	states[STATE_TOKEN].leave = do_close_session;
	states[STATE_TOKEN].out[EVENT_TOKEN_REMOVED] = &(states[STATE_READY]);

	states[STATE_FILE].parent = &(states[STATE_CALLBACKS]);
	states[STATE_FILE].enter = do_parse_file;
	states[STATE_FILE].out[EVENT_CLOSE_FILE] = &(states[STATE_READY]);
	states[STATE_FILE].out[EVENT_TOKEN_INSERTED] = &(states[STATE_TOKEN]);

	curstate = &(states[STATE_LIBOPEN]);
}

void sm_handle_event(enum eid_vwr_state_event e, void* data) {
	struct state* parent;
	if(curstate->out[e] == NULL) {
		return; // event irrelevant for current state
	}
	if(curstate->leave != NULL) {
		curstate->leave();
	}
	while((parent = curstate->parent) != NULL) {
		if(parent->parent != curstate->parent) {
			parent->leave();
		}
	}
	curstate = curstate->out[e];
	if(curstate->enter != NULL) {
		curstate->enter(data);
	}
}
