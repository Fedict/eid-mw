#ifndef EID_VWR_GTK_LOGGING_H
#define EID_VWR_GTK_LOGGING_H

#include <oslayer.h>
#include <stdarg.h>

typedef void(*logfunc)(enum eid_vwr_loglevel, char*, va_list);

logfunc log_init();
void uilog(enum eid_vwr_loglevel l, char* line, ...);

#endif
