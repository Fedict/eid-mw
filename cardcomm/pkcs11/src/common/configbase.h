#include <wchar.h>
#include <stdbool.h>

#ifndef EIDMW_CONFIGBASE_H

#ifdef __cplusplus
extern "C" {
#endif
const wchar_t *eidmw_get_config_string(wchar_t *name, wchar_t *section, wchar_t *default_value, bool expand);
unsigned int eidmw_config_get_log_level(wchar_t *defaultLevel);
#ifdef __cplusplus
}
#endif
#endif
