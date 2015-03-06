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
	if(start != NULL && start->enter != NULL) {
		start->enter(NULL);
	}
}

void sm_handle_event(enum eid_vwr_state_event e, void* data) {
	struct state *thistree, *targettree, *cmnanc;
	if(curstate->out[e] == NULL) {
		thistree = curstate->parent;
		while(thistree && thistree->out[e] == NULL) {
			thistree = thistree->parent;
		}
		if(!thistree) {
			return; // event is irrelevant for this state
		}
	}
	if(curstate->leave != NULL) {
		curstate->leave();
	}
	cmnanc = NULL;
	for(thistree=curstate->parent; thistree != NULL; thistree = thistree->parent) {
		for(targettree = curstate->out[e]->parent; targettree != NULL; targettree = targettree->parent) {
			if(thistree == targettree) {
				cmnanc = thistree;
				goto exit_loop;
			}
		}
	}
exit_loop:
	for(thistree = curstate->parent; thistree != cmnanc; thistree = thistree->parent) {
		if(thistree->leave != NULL) {
			thistree->leave();
		}
	}
	curstate = curstate->out[e];
	parent_enter_recursive(curstate->parent, cmnanc);
	if(curstate->enter != NULL) {
		curstate->enter(data);
	}
	while(curstate->first_child != NULL) {
		curstate = curstate->first_child;
		if(curstate->enter != NULL) {
			curstate->enter(NULL);
		}
	}
}
