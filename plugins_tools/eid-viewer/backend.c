#include <stdlib.h>
#include "oslayer.h"

void eid_vwr_poll() {
}

struct eid_vwr_ui_callbacks* eid_vwr_cbstruct() {
	struct eid_vwr_ui_callbacks* retval = calloc(sizeof(struct eid_vwr_ui_callbacks), 1);
	return retval;
}

int eid_vwr_serialize(void** data, int* len) {
}

int eid_vwr_deserialize(void* data, int len) {
}

int eid_vwr_pinop(enum eid_vwr_pinops op) {
}

int eid_vwr_createcallbacks(struct eid_vwr_ui_callbacks* cb) {
}

void eid_vwr_be_mainloop() {
}

