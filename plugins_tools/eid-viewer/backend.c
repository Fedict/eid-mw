#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifdef WIN32
#include <win32.h>
#else
#include <unix.h>
#endif
#include <pkcs11.h>

#include "oslayer.h"
#include "backend.h"
#include "labels.h"
#include "cache.h"
#include "conversions.h"
#include "state.h"
#include "p11.h"



struct eid_vwr_ui_callbacks* cb;

/* Allocate (and initialize) a callbacks struct. Caller: UI. */
struct eid_vwr_ui_callbacks* eid_vwr_cbstruct() {
	struct eid_vwr_ui_callbacks* retval = (struct eid_vwr_ui_callbacks*)calloc(sizeof(struct eid_vwr_ui_callbacks), 1);
	return retval;
}

#define NEED_CB_FUNC(f) if(!cb) return; if(!(cb->f)) return

/* Perform a PIN operation. Caller: UI. */
int eid_vwr_pinop(enum eid_vwr_pinops op) {
	sm_handle_event(EVENT_DO_PINOP, (void*)op, NULL, NULL);
	return 0;
}

/* Actually assign the callbacks to the correct static variable. Caller: state machine. */
void be_setcallbacks(struct eid_vwr_ui_callbacks* cb_) {
	cb = cb_;
}

/* Issue a "new source" event (if implemented). Caller: state machine. */
void be_newsource(enum eid_vwr_source which) {
	NEED_CB_FUNC(newsrc);
	cb->newsrc(which);
}

/* Issue a "new state" event (if implemented). Caller: state machine. */
void be_newstate(enum eid_vwr_states which) {
	NEED_CB_FUNC(newstate);
	cb->newstate(which);
}

/* Log data to the UI (if implemented). Caller: entire backend. */
void be_log(enum eid_vwr_loglevel l, const char* string, ...) {
	va_list ap;
	char* str = NULL;
	size_t size, newsize = 40;
	if(!cb) return;
	if(cb->log) {
		do {
			va_start(ap, string);
			size = newsize+1;
			str = realloc(str, size);
			if(!str) return;
			newsize = vsnprintf(str, size, string, ap);
			va_end(ap);
		} while (newsize >= size);
		cb->log(l, str);
		free(str);
	} else if(cb->logv) {
		va_start(ap, string);
		cb->logv(l, string, ap);
		va_end(ap);
	}
}

/* Send string data to the UI (if implemented). Caller: p11.c, cache subsystem
 * on translation */
void be_newstringdata(const char* label, const char* data) {
	NEED_CB_FUNC(newstringdata);
	cb->newstringdata(label, data);
}

/* Send binary data to the UI (if implemented). Caller: p11.c, cache subsystem
   on translation */
void be_newbindata(const char* label, const void* data, int datalen) {
	NEED_CB_FUNC(newbindata);
	cb->newbindata(label, data, datalen);
}

/* Send the result of a PIN operation to the UI. Caller: p11.c */
void be_pinresult(enum eid_vwr_pinops p, enum eid_vwr_result res) {
    NEED_CB_FUNC(pinop_result);
    cb->pinop_result(p, res);
}
