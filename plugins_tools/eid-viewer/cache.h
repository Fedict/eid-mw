#ifndef EID_VWR_CACHE_H
#define EID_VWR_CACHE_H

#include <eid-util/utftranslate.h>

#ifdef __cplusplus
extern "C"
{
#endif

	struct eid_vwr_cache_item
	{
		void *data;
		int len;
	};

#ifndef WIN32
#include <stdint.h>
	typedef uint8_t BYTE;
#endif

	void cache_add(const EID_CHAR * label, EID_CHAR * data,
		       unsigned long len);
	void cache_add_bin(const EID_CHAR * label, BYTE * data,
			   unsigned long len);
	const struct eid_vwr_cache_item *cache_get_data(const EID_CHAR *
							label);
    void *cache_label_iterator(void);
	const EID_CHAR *cache_next_label(void *iterator);
	void cache_label_iterator_free(void *iterator);
    int cache_clear(void);
	int cache_have_label(const EID_CHAR * label);
	EID_CHAR *cache_get_xmlform(const EID_CHAR * label);

#ifdef __cplusplus
}
#endif

#endif
