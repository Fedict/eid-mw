#include <conversions.h>
#include "convertor.h"
#include "convworker.h"
#include "cache.h"
#include "backend.h"

#include <cstring>
#include <map>

char* converted_string(const char* label, const char* normal) {
	Convertor conv;
	return conv.convert(label, normal);
}

char* convert_to_xml(const char* label, const char* normal) {
	Convertor conv;
	return conv.convert_to_xml(label, normal);
}

void* convert_from_xml(const char* name, const char* value, int* len_return) {
	Convertor conv;
	return conv.convert_from_xml(name, value, len_return);
}

int can_convert(const char* label) {
	Convertor conv;
	return conv.can_convert(label);
}

void convert_set_lang(enum eid_vwr_langs which) {
	if(ConversionWorker::get_lang() != which) {
		void* iterator;
		const char* label;
		ConversionWorker::set_lang(which);
		for(iterator = cache_label_iterator(),label=cache_next_label(iterator); label != NULL; label = cache_next_label(iterator)) {
			if(can_convert(label)) {
				const eid_vwr_cache_item *item = cache_get_data(label);
				char* str = converted_string(label, (const char*)item->data);
				be_newstringdata(label, str);
				free(str);
			}
		}
		cache_label_iterator_free(iterator);
	}
}

enum eid_vwr_langs convert_get_lang() {
    return ConversionWorker::get_lang();
}