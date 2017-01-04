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

#include <eid-viewer/oslayer.h>
#include <eid-util/utftranslate.h>
#include <eid-util/labels.h>
#include "backend.h"
#include "cache.h"
#include "conversions.h"
#include "state.h"
#include "p11.h"



struct eid_vwr_ui_callbacks* cb;


// I keep getting marshalling errors when providing a structure pointer that contains the function pointers,
// so (for now) I'll provide the function pointers directly and allocate memory for the structure here

int eid_vwr_set_cbfuncs(void(*newsrc)(enum eid_vwr_source source), // data source has changed.
			void(*newstringdata)(const EID_CHAR* label, const EID_CHAR* data), // new string data to be displayed in UI.
			void(*newbindata)(const EID_CHAR* label, const unsigned char* data, int datalen), // new binary data to be displayed in UI.
			void(*log)(enum eid_vwr_loglevel loglevel, const EID_CHAR* line), // log a string at the given level.
			//void(*logv)(enum eid_vwr_loglevel loglevel, const char* line, va_list ap), // log a string using varargs. Note: a UI needs to implement only one of log() or logv(); the backend will use whichever is implemented.
			void(*newstate)(enum eid_vwr_states states), // issued at state machine transition
			void(*pinop_result)(enum eid_vwr_pinops pinops, enum eid_vwr_result result), // issued when a PIN operation finished.
			void(*readers_changed)(unsigned long nreaders, slotdesc* slots)
) {

	struct eid_vwr_ui_callbacks* cb_ = eid_vwr_cbstruct();
	cb_->newsrc = newsrc;
	cb_->newstringdata = newstringdata;
	cb_->newbindata = newbindata;
	cb_->log = log;
	cb_->logv = NULL;
	cb_->newstate = newstate;
	cb_->pinop_result = pinop_result;
	cb_->readers_changed = readers_changed;

	return eid_vwr_createcallbacks(cb_);
}



/* Allocate (and initialize) a callbacks struct. Caller: UI. */

struct eid_vwr_ui_callbacks* eid_vwr_cbstruct() {
	struct eid_vwr_ui_callbacks* retval = (struct eid_vwr_ui_callbacks*)calloc(sizeof(struct eid_vwr_ui_callbacks), 1);
	return retval;
}

#define NEED_CB_FUNC(f) if(!cb) return EIDV_RV_FAIL; if(!(cb->f)) return EIDV_RV_FAIL;

/* Perform a PIN operation. Caller: UI. */
void eid_vwr_pinop(enum eid_vwr_pinops op) {
	sm_handle_event(EVENT_DO_PINOP, (void*)op, NULL, NULL);
	return;
}

/* Actually assign the callbacks to the correct static variable. Caller: state machine. */
void be_setcallbacks(struct eid_vwr_ui_callbacks* cb_) {
	cb = cb_;
}

/* Issue a "new source" event (if implemented). Caller: state machine. */
int be_newsource(enum eid_vwr_source which) {
	NEED_CB_FUNC(newsrc);
	cb->newsrc(which);
	return EIDV_RV_OK;
}

/* Issue a "new state" event (if implemented). Caller: state machine. */
int be_newstate(enum eid_vwr_states which) {
	NEED_CB_FUNC(newstate);
	cb->newstate(which);
	return EIDV_RV_OK;
}

static EID_CHAR* format_string(const EID_CHAR* format, va_list ap) {
	size_t strsize = 0;
	size_t strnewsize = 0;
	EID_CHAR* str = NULL;
	va_list apc;
	do {
		va_copy(apc, ap);
		strnewsize = EID_VSNPRINTF(str, strsize, format, apc);
		va_end(apc);
		strsize = strnewsize + 1;
		str = (EID_CHAR*)realloc(str, strsize * sizeof(EID_CHAR));
		if(!str) {
			return NULL;
		}
		va_copy(apc, ap);
		strnewsize = EID_VSNPRINTF(str, strsize, format, apc);
		va_end(apc);
	} while(strnewsize >= strsize);
	return str;
}

/* Log data to the UI (if implemented). Caller: entire backend. */
void be_log(enum eid_vwr_loglevel l, const EID_CHAR* string, ...) {
	va_list ap;
	EID_CHAR* str = NULL;
	static EID_CHAR* logbuf[10];
	static enum eid_vwr_loglevel loglev[10];
	static int lastlog=0;
	static int have_buffered = 0;

	va_start(ap, string);
	if(!cb) {
		if(logbuf[lastlog] != NULL) {
			free(logbuf[lastlog]);
			logbuf[lastlog] = NULL;
		}
		logbuf[lastlog] = format_string(string, ap);
		loglev[lastlog++] = l;
		if(lastlog >= 10) {
			lastlog = 0;
		}
		have_buffered = 1;
		goto end;
	}
	if(have_buffered != 0) {
		int i;

		have_buffered = 0;
		for(i=lastlog; i<10; i++) {
			if(logbuf[i] != NULL) {
				be_log(loglev[i], logbuf[i]);
				free(logbuf[i]);
			}
		}
		for(i=0; i<lastlog; i++) {
			be_log(loglev[i], logbuf[i]);
		}
	}
	if(cb->logv) {
		cb->logv(l, string, ap);
	} else if(cb->log) {
		str = format_string(string, ap);
		cb->log(l, str);
		free(str);
	}
end:
	va_end(ap);
}

/* Send string data to the UI (if implemented). Caller: p11.c, cache subsystem
 * on translation */
int be_newstringdata(const EID_CHAR* label, const EID_CHAR* data) {
	NEED_CB_FUNC(newstringdata);
	cb->newstringdata(label, data);
	return EIDV_RV_OK;
}

/* Send binary data to the UI (if implemented). Caller: p11.c, cache subsystem
 on translation */
int be_newbindata(const EID_CHAR* label, const unsigned char* data, int datalen) {
	NEED_CB_FUNC(newbindata);
	cb->newbindata(label, data, datalen);
	return EIDV_RV_OK;
}

/* Send the result of a PIN operation to the UI. Caller: p11.c */
int be_pinresult(enum eid_vwr_pinops p, enum eid_vwr_result res) {
	NEED_CB_FUNC(pinop_result);
	cb->pinop_result(p, res);
	return EIDV_RV_OK;
}

int be_readers_changed(unsigned long nreaders, slotdesc* slots) {
	NEED_CB_FUNC(readers_changed);
	cb->readers_changed(nreaders, slots);
	return EIDV_RV_OK;
}
