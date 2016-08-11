#ifndef EID_VWR_GTK_LOGGING_H
#define EID_VWR_GTK_LOGGING_H

#include <eid-viewer/oslayer.h>
#include <stdarg.h>

typedef void (*logfunc) (enum eid_vwr_loglevel, const char *, va_list ap);

logfunc ui_log_init();
void uilog(enum eid_vwr_loglevel l, const char *line, ...);

#endif
