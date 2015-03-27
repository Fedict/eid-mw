#ifndef EID_VWR_CONVERSIONS_H
#define EID_VWR_CONVERSIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cache.h"

enum eid_vwr_langs {
	EID_VWR_LANG_NONE,
	EID_VWR_LANG_DE,
	EID_VWR_LANG_EN,
	EID_VWR_LANG_FR,
	EID_VWR_LANG_NL,
};

char* converted_string(const char* label, const char* normal);
char* convert_to_xml(const char* label, const char* item);
void* convert_from_xml(const char* name, const char* value, int* len_return);
int can_convert(const char* label);
void convert_set_lang(enum eid_vwr_langs which);

#ifdef __cplusplus
}
#endif

#endif
