#include "xmlmap.h"
#include <map>
#include <string>

static std::map<std::string, struct element_desc*> xml_to_elem;
static std::map<std::string, struct attribute_desc*> xml_to_att;
static bool inited = false;

/* This file describes our XML format. It is used by xml.c to map an element
 * label to an element struct. We wrote it in C++ so we can use the STL
 * containers, but other than that C++ isn't really needed... */

/* Attributes of the <identity> element */
struct attribute_desc identity_attributes[] = {
	{ "nationalnumber", "national_number", 1 },
	{ "dateofbirth", "date_of_birth", 1 },
	{ "gender", "gender", 1 },
	{ "noblecondition", "nobility", 0 },
	{ "specialstatus", "special_status", 0 },
	{ "duplicate", "duplicata", 0 },
	{ NULL, NULL, 0 },
};

/* Child elements of the <identity> element */
struct element_desc identity_elements[] = {
	{ "name", "surname", 1, 0, NULL, NULL },
	{ "firstname", "firstnames", 1, 0, NULL, NULL },
	{ "middlenames", "first_letter_of_third_given_name", 0, 0, NULL, NULL },
	{ "nationality", "nationality", 1, 0, NULL, NULL },
	{ "placeofbirth", "location_of_birth", 1, 0, NULL, NULL },
	{ "photo", "PHOTO_FILE", 1, 1, NULL, NULL },
	{ NULL, NULL, 0, 0, NULL, NULL },
};

/* Child elements of the <card> element */
static struct element_desc card_elements[] = {
	{ "deliverymunicipality", "issuing_municipality", 1, 0, NULL, NULL },
	{ NULL, NULL, 0, 0, NULL, NULL },
};

/* Attributes of the <card> element */
static struct attribute_desc card_attributes[] = {
	{ "documenttype", "document_type", 1 },
	{ "cardnumber", "card_number", 1 },
	{ "chipnumber", "chip_number", 1 },
	{ "validitydatebegin", "validity_begin_date", 1 },
	{ "validitydateend", "validity_end_date", 1 },
	{ NULL, NULL, 0 },
};

/* Child elements of the <address> element */
static struct element_desc address_elements[] = {
	{ "streetandnumber", "address_street_and_number", 1, 0, NULL, NULL },
	{ "zip", "address_zip", 1, 0, NULL, NULL },
	{ "municipality", "address_municipality", 1, 0, NULL, NULL },
	{ NULL, NULL, 0, 0, NULL, NULL },
};

/* Child elements of the <certificates> element */
static struct element_desc certificate_elements[] = {
	{ "root", "Root", 1, 1, NULL, NULL },
	{ "citizenca", "CA", 1, 1, NULL, NULL },
	{ "authentication", "Authentication", 0, 1, NULL, NULL },
	{ "signing", "Signature", 0, 1, NULL, NULL },
	{ "rrn", "CERT_RN_FILE", 1, 1, NULL, NULL },
	{ NULL, NULL, 0, 0, NULL, NULL },
};

/* Child elements of the <eid> element (the toplevel element) */
static struct element_desc eid_elements[] = {
	{ "identity", NULL, 1, 0, identity_elements, identity_attributes },
	{ "card", NULL, 1, 0, card_elements, card_attributes },
	{ "address", NULL, 1, 0, address_elements, NULL },
	{ "certificates", NULL, 1, 0, certificate_elements, NULL },
	{ NULL, NULL, 0, 0, NULL, NULL },
};

/* The toplevel element, <eid> */
static struct element_desc toplevel_arr[] = {
	{ "eid", NULL, 1, 0, eid_elements, NULL },
	{ NULL, NULL, 0, 0, NULL, NULL },
};

struct element_desc *toplevel = toplevel_arr;

/* Initialize the std::map so we can do quick lookups later on */
static void initmap(struct element_desc *elem) {
	while(elem->name) {
		xml_to_elem[elem->name] = elem;
		if(elem->child_elements != NULL) {
			initmap(elem->child_elements);
		}
		if(elem->attributes != NULL) {
			for(struct attribute_desc *att = elem->attributes; att->name != NULL; att++) {
				xml_to_att[att->name] = att;
			}
		}
		elem++;
	}
}

/* Do a lookup of the struct element_desc* corresponding to the given XML
 * element name. Returns NULL if the element is unknown. */
struct element_desc* get_elemdesc(const char* xmlname) {
	if(!inited) {
		initmap(toplevel);
		inited = true;
	}
	if(xml_to_elem.count(xmlname)) {
		return xml_to_elem[xmlname];
	} else {
		return NULL;
	}
}

/* Do a lookup of the struct attribute_desc* corresponding to the given XML
 * element name. Returns NULL if the attribute is unknown.
 *
 * This works because we don't have two elements with attributes having
 * the same name but different semantics. If that ever changes, we may
 * need to change this function's signature. */
struct attribute_desc* get_attdesc(const char* xmlname) {
	if(!inited) {
		initmap(toplevel);
		inited = true;
	}
	if(xml_to_att.count(xmlname)) {
		return xml_to_att[xmlname];
	} else {
		return NULL;
	}
}
