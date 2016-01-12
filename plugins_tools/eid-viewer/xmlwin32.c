#include "cache.h"
#include "backend.h"

#include "xml.h"
#include "xmlmap.h"

#include <string.h>
#include "p11.h"

#ifdef WIN32
#include <stdio.h>
#include <tchar.h>
//#include <objbase.h>
//#include <msxml6.h>
#include "eidxmllite.h"
#endif

#include <assert.h>



//TODO (msxml)
int eid_vwr_gen_xml(void* data) {
	return 0;
}

int eid_vwr_serialize(void* data) {
	const struct eid_vwr_cache_item* item = cache_get_data("xml");
	FILE* f = fopen((const char*)data, "w");
	fwrite(item->data, item->len, 1, f);
	return fclose(f);
}

int eid_vwr_deserialize(char* data) {
	return eid_vwr_do_deserialize(data);
}
