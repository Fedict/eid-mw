#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <unix.h>
#include <pkcs11.h>

#include "oslayer.h"

typedef struct {
	CK_RV rv;
	int res;
} ckrv_mod;

#define EIDV_RV_OK 0
#define EIDV_RV_FAIL -1

ckrv_mod defmod[] = { { CKR_OK, EIDV_RV_OK } };

int ckrv_decode(CK_RV rv, int count, ckrv_mod* mods) {
	int i;
	for(i=0; i<count; i++) {
		if(mods[i].rv == rv) {
			return mods[i].res;
		}
	}
	return EIDV_RV_FAIL;
}

#define check_rv_long(call, mods) { \
	CK_RV rv = call; \
	int retval = ckrv_decode(rv, sizeof(mods) / sizeof(ckrv_mod), mods); \
	if(retval != EIDV_RV_OK) { \
		return retval; \
	} \
}
#define check_rv(call) check_rv_long(call, defmod)
#define check_rv_late(rv) { \
	int retval = ckrv_decode(rv, 1, defmod); \
	if(retval != EIDV_RV_OK) { \
		return retval; \
	} \
}

struct eid_vwr_ui_callbacks* cb;

struct eid_vwr_ui_callbacks* eid_vwr_cbstruct() {
	struct eid_vwr_ui_callbacks* retval = calloc(sizeof(struct eid_vwr_ui_callbacks), 1);
	return retval;
}

int eid_vwr_serialize(void** data, int* len) {
	return 0;
}

int eid_vwr_deserialize(void* data, int len) {
	assert(cb);

	if(!len) {
		cb->newsrc(EID_VWR_SRC_NONE);
	}
	// TODO: parse the XML file, set the source
	return 0;
}

int eid_vwr_pinop(enum eid_vwr_pinops op) {
}

int eid_vwr_createcallbacks(struct eid_vwr_ui_callbacks* cb_) {
	assert(cb == NULL);
	cb = cb_;
	cb->newsrc(EID_VWR_SRC_NONE);
	return 0;
}

void eid_vwr_be_mainloop() {
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
