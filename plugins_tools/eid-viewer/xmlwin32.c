#include "cache.h"
#include "backend.h"

#include "xmlmap.h"
#include "xsdloc.h"

#include <string.h>
#include "p11.h"

#ifdef WIN32
#include <stdio.h>
#endif

#include <assert.h>


//TODO (xmlLite?)
int eid_vwr_gen_xml(void* data) {
	return 0;
}

int eid_vwr_serialize(void* data) {
	const struct eid_vwr_cache_item* item = cache_get_data("xml");
	FILE* f = fopen((const char*)data, "w");
	fwrite(item->data, item->len, 1, f);
	return fclose(f);
}

//TODO (xmlLite?)
int eid_vwr_deserialize(void* data) {

	return 0;
}
