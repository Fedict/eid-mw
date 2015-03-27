#ifndef EID_VWR_BACKEND_H
#define EID_VWR_BACKEND_H

#ifdef __cplusplus
extern "C" {
#endif

#include "oslayer.h"

void be_setcallbacks(struct eid_vwr_ui_callbacks* cb_);
void be_newsource(enum eid_vwr_source which);
void be_log(enum eid_vwr_loglevel, const char* line, ...);
void be_newstate(enum eid_vwr_states s);
void be_newstringdata(const char* label, const char* data);
void be_newbindata(char* label, void* data, int datalen);

#ifdef __cplusplus
}
#endif

#endif
