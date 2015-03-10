#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <unix.h>
#include <pkcs11.h>

#include "oslayer.h"
#include "backend.h"
#include "labels.h"
#include "cache.h"
#include "conversions.h"
#include "state.h"
#include "p11.h"

struct eid_vwr_ui_callbacks* cb;

struct eid_vwr_ui_callbacks* eid_vwr_cbstruct() {
	struct eid_vwr_ui_callbacks* retval = calloc(sizeof(struct eid_vwr_ui_callbacks), 1);
	return retval;
}

#define NEED_CB_FUNC(f) if(!cb) return; if(!(cb->f)) return

int eid_vwr_serialize(void** data, int* len) {
	return 0;
}

int eid_vwr_pinop(enum eid_vwr_pinops op) {
}

void be_setcallbacks(struct eid_vwr_ui_callbacks* cb_) {
	cb = cb_;
}

void be_newsource(enum eid_vwr_source which) {
	NEED_CB_FUNC(newsrc);
	cb->newsrc(which);
}

void be_newstate(enum eid_vwr_states which) {
	NEED_CB_FUNC(newstate);
	cb->newstate(which);
}

void be_log(enum eid_vwr_loglevel l, char* string, ...) {
	NEED_CB_FUNC(log);
	va_list ap, ac;
	va_start(ap, string);
	va_copy(ac, ap);
	cb->log(l, string, ac);
	va_end(ac);
	va_end(ap);
}

void be_status(char* data, ...) {
	va_list ap, ac;
	va_start(ap, data);
	va_copy(ac, ap);
	cb->status(data, ac);
	va_end(ac);
	va_end(ap);
}

void be_newstringdata(char* label, char* data) {
	NEED_CB_FUNC(newstringdata);
	cb->newstringdata(label, data);
}

void be_newbindata(char* label, void* data, int datalen) {
	NEED_CB_FUNC(newbindata);
	cb->newbindata(label, data, datalen);
}

struct eid_vwr_preview* eid_vwr_get_preview(char* filename) {
	// TODO: open the file, deserialize until we find a photo, pass back the photo.
	// For now, show the same photo in all cases...
	FILE* f;
	struct eid_vwr_preview* p;
	p = calloc(sizeof(struct eid_vwr_preview), 1);
	if(!filename) {
		return p;
	}
	if(strstr(filename, ".eid") != filename + (strlen(filename) - 4)) {
		return p;
	}
	f = fopen("../../tests/unit/foto.jpg", "r");
	if(!f) {
		perror("fopen");
		return p;
	}
	fseek(f, 0, SEEK_END);
	p->imagelen = ftell(f);
	fseek(f, 0, SEEK_SET);
	p->imagedata = malloc(p->imagelen);
	fread(p->imagedata, p->imagelen, 1, f);
	p->have_data = 1;
	return p;
}
