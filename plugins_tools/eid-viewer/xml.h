#ifndef EID_VWR_XML_H
#define EID_VWR_XML_H
#include "backend.h"

#ifdef __cplusplus
extern "C" {
#endif

	DllExport int eid_vwr_deserialize(void* filename);
	DllExport int eid_vwr_serialize(void* filename);
	int eid_vwr_gen_xml(void* data);

#ifdef __cplusplus
}
#endif
#endif
