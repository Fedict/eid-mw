#include "cache.h"
#include <map>
#include <string>

std::map<std::string, void*> cache;

void cache_add(char* label, void* data, unsigned long len) {
	void* copy = new char[len];
	cache[label] = data;
}

const void* cache_get_data(char* label) {
	return cache[label];
}

struct iterator_deref {
	std::map<std::string, void*>::iterator it;
};

void* cache_label_iterator() {
	iterator_deref* it = new iterator_deref;
	it->it = cache.begin();
	return (void*)it;
}

const char* cache_next_label(void* iterator) {
	iterator_deref* it = (iterator_deref*)iterator;
	const char* retval = it->it->first.c_str();
	++(it->it);
	return retval;
}

void cache_clear() {
	cache.erase(cache.begin(), cache.end());
}
