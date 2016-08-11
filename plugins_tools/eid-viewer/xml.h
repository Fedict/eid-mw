#ifndef EID_VWR_XML_H
#define EID_VWR_XML_H
#include "backend.h"

#ifdef __cplusplus
extern "C"
{
#endif

	int eid_vwr_deserialize(const EID_CHAR * filename);
	int eid_vwr_serialize(const EID_CHAR * filename);
	int eid_vwr_gen_xml(void *data);

#ifdef __cplusplus
}
#endif
#endif
