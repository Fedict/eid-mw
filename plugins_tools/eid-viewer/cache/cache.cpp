#include "cache.h"
#include "conversions.h"
#include <map>
#include <string>
#include <cstdlib>
#include <cstring>

struct cache_item_container {	
	eid_vwr_cache_item* item;

	cache_item_container(void* data, size_t len) : item(new eid_vwr_cache_item) {
		item->data = malloc(len+1);
		memcpy(item->data, data, len);
		((char*)item->data)[len]='\0';
		item->len = len;
	}

	~cache_item_container() {
		free(item->data);
		delete item;
	}
};

std::map<std::string, cache_item_container*> cache;

void cache_add(const char* label, void* data, unsigned long len) {
	cache[label] = new cache_item_container(data, len);
}

const struct eid_vwr_cache_item* cache_get_data(const char* label) {
	return cache[label]->item;
}

struct iterator_deref {
	std::map<std::string, cache_item_container*>::iterator it;
};

void* cache_label_iterator() {
	iterator_deref* it = new iterator_deref;
	it->it = cache.begin();
	return (void*)it;
}

void cache_label_iterator_free(void* iterator) {
	iterator_deref* it = (iterator_deref*)iterator;
	delete it;
}

const char* cache_next_label(void* iterator) {
	iterator_deref* it = (iterator_deref*)iterator;
	if(it->it == cache.end()) {
		return NULL;
	}
	const char* retval = it->it->first.c_str();
	++(it->it);
	return retval;
}

int cache_clear() {
	cache.clear();

	return 0;
}

int cache_have_label(const char* label) {
	return cache.count(label);
}

char* cache_get_xmlform(const char* label) {
	return (char*)convert_to_xml(label, (char*)cache_get_data(label)->data);
}

void cache_add_xmlform(const char* label, const char* value) {
	int len = 0;
	cache_add(label, convert_from_xml(label, value, &len), len);
}
