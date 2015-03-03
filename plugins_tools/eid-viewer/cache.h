#ifndef EID_VWR_CACHE_H
#define EID_VWR_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

void cache_add(char* label, void* data, unsigned long len);
const void* cache_get_data(char* label);
void* cache_label_iterator();
const char* cache_next_label(void* iterator);
void cache_clear();

#ifdef __cplusplus
}
#endif

#endif
