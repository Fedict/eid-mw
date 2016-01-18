#ifndef EID_VWR_CACHE_H
#define EID_VWR_CACHE_H

#include "utftranslate.h"

#ifdef __cplusplus
extern "C" {
#endif

struct eid_vwr_cache_item {
	void* data;
	int len;
};

void cache_add(const EID_CHAR* label, void* data, unsigned long len);
const struct eid_vwr_cache_item* cache_get_data(const EID_CHAR* label);
void* cache_label_iterator();
const EID_CHAR* cache_next_label(void* iterator);
void cache_label_iterator_free(void* iterator);
int cache_clear();
int cache_have_label(const EID_CHAR* label);
EID_CHAR* cache_get_xmlform(const EID_CHAR* label);

#ifdef __cplusplus
}
#endif

#endif
