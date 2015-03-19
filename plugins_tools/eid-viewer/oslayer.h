#ifndef EID_VWR_GTK_OSLAYER_H
#define EID_VWR_GTK_OSLAYER_H

#include <conversions.h>
#include <stdlib.h>
#include <stdarg.h>

enum eid_vwr_source {
	EID_VWR_SRC_NONE,
	EID_VWR_SRC_FILE,
	EID_VWR_SRC_CARD,
	EID_VWR_SRC_UNKNOWN,
};

enum eid_vwr_loglevel {
	EID_VWR_LOG_DETAIL = 0,
	EID_VWR_LOG_NORMAL = 1,
	EID_VWR_LOG_COARSE = 2,
};

enum eid_vwr_pinops {
	EID_VWR_PINOP_TEST,
	EID_VWR_PINOP_CHG,
};

enum eid_vwr_states {
	STATE_LIBOPEN,
	STATE_CALLBACKS,
	STATE_READY,
	STATE_TOKEN,
	STATE_TOKEN_WAIT,
	STATE_TOKEN_ID,
	STATE_TOKEN_CERTS,
	STATE_TOKEN_PINOP,
	STATE_TOKEN_SERIALIZE,
	STATE_TOKEN_ERROR,
	STATE_FILE,
	STATE_CARD_INVALID,

	STATE_COUNT,
};

struct eid_vwr_ui_callbacks {
	void(*newsrc)(enum eid_vwr_source);
	void(*newstringdata)(const char* label, char* data);
	void(*newbindata)(const char* label, void* data, int datalen);
	void(*log)(enum eid_vwr_loglevel, char* line, va_list ap);
	void(*newstate)(enum eid_vwr_states);
};

struct eid_vwr_preview {
	void* imagedata;
	size_t imagelen;
	int have_data;
};

int eid_vwr_serialize(void** data, int* len);
int eid_vwr_deserialize(void* data, int len, void(*write)(void*));
int eid_vwr_pinop(enum eid_vwr_pinops);
int eid_vwr_createcallbacks(struct eid_vwr_ui_callbacks*);
struct eid_vwr_ui_callbacks* eid_vwr_cbstruct();
struct eid_vwr_preview* eid_vwr_get_preview(char* filename);

void eid_vwr_poll();
void eid_vwr_be_mainloop();

#endif
