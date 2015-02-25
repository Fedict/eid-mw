#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "oslayer.h"

struct eid_vwr_ui_callbacks* cb;

void eid_vwr_poll() {
}

struct eid_vwr_ui_callbacks* eid_vwr_cbstruct() {
	struct eid_vwr_ui_callbacks* retval = calloc(sizeof(struct eid_vwr_ui_callbacks), 1);
	return retval;
}

int eid_vwr_serialize(void** data, int* len) {
}

int eid_vwr_deserialize(void* data, int len) {
	assert(cb);

	if(!len) {
		cb->newsrc(EID_VWR_SRC_NONE);
	}
	// TODO: parse the XML file, set the source
}

int eid_vwr_pinop(enum eid_vwr_pinops op) {
}

int eid_vwr_createcallbacks(struct eid_vwr_ui_callbacks* cb_) {
	assert(cb == NULL);
	cb = cb_;
	cb->newsrc(EID_VWR_SRC_NONE);
}

void eid_vwr_be_mainloop() {
}

struct eid_vwr_preview* eid_vwr_get_preview(char* filename) {
	// TODO: open the file, deserialize until we find a photo, pass back the photo.
	// For now, show the same photo in all cases...
	FILE* f;
	struct eid_vwr_preview* p;
	p = calloc(sizeof(struct eid_vwr_preview), 1);
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
