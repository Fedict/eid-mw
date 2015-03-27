#ifndef EID_VWR_XMLMAP_H
#define EID_VWR_XMLMAP_H

#ifdef __cplusplus
extern "C" {
#endif

struct attribute_desc {
	const char* name;
	const char* label;
	int reqd;
};

struct element_desc {
	const char* name;
	const char* label;
	int reqd;
	int is_b64;
	struct element_desc *child_elements;
	struct attribute_desc *attributes;
};

extern struct element_desc *toplevel;

struct element_desc* get_elemdesc(const char* xmlname);
struct attribute_desc* get_attdesc(const char* xmlname);

#ifdef __cplusplus
}
#endif

#endif
