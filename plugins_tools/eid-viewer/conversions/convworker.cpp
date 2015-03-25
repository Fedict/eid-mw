#include "convworker.h"
#include "cache.h"

eid_vwr_langs ConversionWorker::target_ = EID_VWR_LANG_NONE;

std::string ConversionWorker::convert_xml(const struct eid_vwr_cache_item* original) {
	return (char*)original->data;
}
