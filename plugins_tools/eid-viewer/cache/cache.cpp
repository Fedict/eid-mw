#include "cache.h"
#include "conversions.h"
#include <eid-util/utftranslate.h>
#include <eid-util/labels.h>
#include "cppeidstring.h"
#include <map>
#include <string>
#include <cstdlib>
#include <cstring>



struct cache_item_container
{
	eid_vwr_cache_item *item;

		 cache_item_container(const EID_CHAR * data,
				      size_t len):item(new eid_vwr_cache_item)
	{
		item->data = malloc((len + 1) * sizeof(EID_CHAR));
		memcpy(item->data, data, len * sizeof(EID_CHAR));
		((EID_CHAR *) item->data)[len] = '\0';
		item->len = (int) len;
	}

	cache_item_container (BYTE * data, size_t len, bool bin)
	{
		if (!bin)
			cache_item_container((EID_CHAR *) data, len);
		else
		{
			item = new eid_vwr_cache_item;
			item->data = malloc((len + 1));
			memcpy(item->data, data, len);
			((char *) item->data)[len] = '\0';
			item->len = (int) len;
		}
	}

	~cache_item_container()
	{
		free(item->data);
		delete item;
	}
};

std::map < EID_STRING, cache_item_container * >cache;

void cache_add(const EID_CHAR * label, EID_CHAR * data, unsigned long len)
{
	cache[label] = new cache_item_container(data, len);
	/* TODO: don't special-case the "xml" label here, but add it to the map too */
	if(EID_STRCMP(data, TEXT("xml")) != 0) {
		const EID_CHAR *vers = min_version((const EID_CHAR*)label);
		if(vers != NULL) {
			std::map<EID_STRING, cache_item_container *>::iterator it = cache.find(TEXT("xml_file_version"));
			if(it == cache.end() || EID_STRCMP(vers,(const EID_CHAR*)((*it).second->item->data)) > 0) {
				cache[TEXT("xml_file_version")] = new cache_item_container(vers, EID_STRLEN(vers));
			}
		}
	}
}

void cache_add_bin(const EID_CHAR * label, BYTE * data, unsigned long len)
{
	cache[label] = new cache_item_container(data, len, true);
}

const struct eid_vwr_cache_item *cache_get_data(const EID_CHAR * label)
{
	return cache[label]->item;
}

struct iterator_deref
{
	std::map < EID_STRING, cache_item_container * >::iterator it;
};

void *cache_label_iterator()
{
	iterator_deref *it = new iterator_deref;

	it->it = cache.begin();
	return (void *) it;
}

void cache_label_iterator_free(void *iterator)
{
	iterator_deref *it = (iterator_deref *) iterator;
	delete it;
}

const EID_CHAR *cache_next_label(void *iterator)
{
	iterator_deref *it = (iterator_deref *) iterator;

	if (it->it == cache.end())
	{
		return NULL;
	}
	const EID_CHAR *retval = it->it->first.c_str();

	++(it->it);
	return retval;
}

int cache_clear()
{
	cache.clear();

	return 0;
}

int cache_have_label(const EID_CHAR * label)
{
	return (int) cache.count(label);
}

EID_CHAR *cache_get_xmlform(const EID_CHAR * label)
{
	return (EID_CHAR *) convert_to_xml(label,(EID_CHAR *)cache_get_data(label)->data);
}

void cache_add_xmlform(const EID_CHAR * label, const EID_CHAR * value)
{
	int len = 0;
	EID_CHAR* val = (EID_CHAR*)convert_from_xml(label, value, &len);

	cache_add(label, val, len);
	free(val);
}
