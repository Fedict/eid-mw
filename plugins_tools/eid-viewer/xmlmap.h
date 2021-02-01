#ifndef EID_VWR_XMLMAP_H
#define EID_VWR_XMLMAP_H

#include <eid-util/utftranslate.h>

#ifdef __cplusplus
extern "C"
{
#endif

	struct attribute_desc
	{
		const EID_CHAR *name;
		const EID_CHAR *label;
		int reqd;
	};

	struct element_desc
	{
		const EID_CHAR *name;
		const EID_CHAR *label;
		int reqd;
		int is_b64;
		struct element_desc *child_elements;
		struct attribute_desc *attributes;
	};

	extern struct element_desc *toplevel;

	struct element_desc *get_elemdesc(const EID_CHAR * xmlname);
	struct attribute_desc *get_attdesc(const EID_CHAR * xmlname);

#ifdef __cplusplus
}
#endif

#endif
