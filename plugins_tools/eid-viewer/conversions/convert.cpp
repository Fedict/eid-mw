#include <conversions.h>
#include "convertor.h"
#include "convworker.h"

#include <cstring>
#include <map>

const char* converted_string(const char* label, const char* normal) {
	Convertor conv;
	return strdup(conv.convert(label, normal));
}

int can_convert(const char* label) {
	Convertor conv;
	return conv.can_convert(label);
}

void convert_set_lang(enum eid_vwr_langs which) {
	ConversionWorker::set_lang(which);
}
