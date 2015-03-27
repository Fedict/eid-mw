#ifndef EID_VWR_CACHE_H
#define EID_VWR_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

struct eid_vwr_cache_item {
	void* data;
	int len;
};

void cache_add(const char* label, void* data, unsigned long len);
const struct eid_vwr_cache_item* cache_get_data(const char* label);
void* cache_label_iterator();
const char* cache_next_label(void* iterator);
void cache_label_iterator_free(void* iterator);
int cache_clear();
int cache_have_label(const char* label);
char* cache_get_xmlform(const char* label);

#ifdef __cplusplus
}
#endif

#endif
