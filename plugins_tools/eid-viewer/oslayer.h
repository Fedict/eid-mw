#ifndef EID_VWR_GTK_OSLAYER_H
#define EID_VWR_GTK_OSLAYER_H

enum eid_vwr_source {
	EID_VWR_SRC_NONE,
	EID_VWR_SRC_FILE,
	EID_VWR_SRC_CARD,
	EID_VWR_SRC_UNKNOWN,
};

enum eid_vwr_loglevel {
	EID_VWR_LOG_DETAIL,
	EID_VWR_LOG_NORMAL,
	EID_VWR_LOG_COARSE,
};

enum eid_vwr_pinops {
	EID_VWR_PINOP_TEST,
	EID_VWR_PINOP_CHG,
};

struct eid_vwr_ui_callbacks {
	void(*newsrc)(enum eid_vwr_source);
	void(*newstringdata)(char* label, char* data);
	void(*newbindata)(char* label, void* data, int datalen);
	void(*log)(enum eid_vwr_loglevel, char* line);
};

int eid_vwr_serialize(void** data, int* len);
int eid_vwr_deserialize(void* data, int len);
int eid_vwr_pinop(enum eid_vwr_pinops);
int eid_vwr_createcallbacks(struct eid_vwr_ui_callbacks*);
struct eid_vwr_ui_callbacks* eid_vwr_cbstruct();

void eid_vwr_poll();
void eid_vwr_be_mainloop();

#endif
