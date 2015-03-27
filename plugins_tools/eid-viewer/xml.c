#include "cache.h"
#include "backend.h"

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include <assert.h>

#define check_xml(call) if((rc = call) < 0) { \
	be_log(EID_VWR_LOG_DETAIL, "Error while writing to file (calling '%s'): %d", #call, rc); \
	return -1; \
}

struct attribute_desc {
	char* name;
	char* label;
	int reqd;
};

struct element_desc {
	char* name;
	char* label;
	int reqd;
	int is_b64;
	struct element_desc *child_elements;
	struct attribute_desc *attributes;
};

struct attribute_desc identity_attributes[] = {
	{ "nationalnumber", "national_number", 1 },
	{ "dateofbirth", "date_of_birth", 1 },
	{ "gender", "gender", 1 },
	{ "noblecondition", "nobility", 0 },
	{ "specialstatus", "special_status", 0 },
	{ "duplicate", "duplicata", 0 },
	{ NULL, NULL, 0 },
};

struct element_desc identity_elements[] = {
	{ "name", "surname", 1, 0, NULL, NULL },
	{ "firstname", "firstnames", 1, 0, NULL, NULL },
	{ "middlenames", "first_letter_of_third_given_name", 0, 0, NULL, NULL },
	{ "nationality", "nationality", 1, 0, NULL, NULL },
	{ "placeofbirth", "location_of_birth", 1, 0, NULL, NULL },
	{ "photo", "PHOTO_FILE", 1, 1, NULL, NULL },
	{ NULL, NULL, 0, 0, NULL, NULL },
};

struct element_desc card_elements[] = {
	{ "deliverymunicipality", "issuing_municipality", 1, 0, NULL, NULL },
	{ NULL, NULL, 0, 0, NULL, NULL },
};

struct attribute_desc card_attributes[] = {
	{ "documenttype", "document_type", 1 },
	{ "cardnumber", "card_number", 1 },
	{ "chipnumber", "chip_number", 1 },
	{ "validitydatebegin", "validity_begin_date", 1 },
	{ "validitydateend", "validity_end_date", 1 },
	{ NULL, NULL, 0 },
};

struct element_desc address_elements[] = {
	{ "streetandnumber", "address_street_and_number", 1, 0, NULL, NULL },
	{ "zip", "address_zip", 1, 0, NULL, NULL },
	{ "municipality", "address_municipality", 1, 0, NULL, NULL },
	{ NULL, NULL, 0, 0, NULL, NULL },
};

struct element_desc certificate_elements[] = {
	{ "root", "Root", 1, 1, NULL, NULL },
	{ "citizenca", "CA", 1, 1, NULL, NULL },
	{ "authentication", "Authentication", 0, 1, NULL, NULL },
	{ "signing", "Signature", 0, 1, NULL, NULL },
	{ "rrn", "CERT_RN_FILE", 1, 1, NULL, NULL },
	{ NULL, NULL, 0, 0, NULL, NULL },
};

struct element_desc eid_elements[] = {
	{ "identity", NULL, 1, 0, identity_elements, identity_attributes },
	{ "card", NULL, 1, 0, card_elements, card_attributes },
	{ "address", NULL, 1, 0, address_elements, NULL },
	{ "certificates", NULL, 1, 0, certificate_elements, NULL },
	{ NULL, NULL, 0, 0, NULL, NULL },
};

struct element_desc toplevel[] = {
	{ "eid", NULL, 1, 0, eid_elements, NULL },
	{ NULL, NULL, 0, 0, NULL, NULL },
};

static int write_attributes(xmlTextWriterPtr writer, struct attribute_desc *attribute) {
	int rc;
	while(attribute->name) {
		int have_cache = cache_have_label(attribute->label);
		if(attribute->reqd && !have_cache) {
			be_log(EID_VWR_LOG_COARSE, "Error: no data found for required label %s", attribute->label);
			return -1;
		}
		if(have_cache) {
			check_xml(xmlTextWriterWriteAttribute(writer, BAD_CAST attribute->name, BAD_CAST cache_get_xmlform(attribute->label)));
		}
		attribute++;
	}

	return 0;
}

static int write_elements(xmlTextWriterPtr writer, struct element_desc *element) {
	int rc;
	while(element->name) {
		if(element->label == NULL) {
			assert(element->child_elements != NULL);
			check_xml(xmlTextWriterStartElement(writer, BAD_CAST element->name));
			if(element->attributes != NULL) {
				write_attributes(writer, element->attributes);
			}
			check_xml(write_elements(writer, element->child_elements));
			check_xml(xmlTextWriterEndElement(writer));
		} else {
			int have_cache = cache_have_label(element->label);
			assert(element->child_elements == NULL);

			if(element->reqd && !have_cache) {
				be_log(EID_VWR_LOG_COARSE, "Error: no data found for required label %s", element->label);
				return -1;
			}
			if(have_cache) {
				if(!element->is_b64) {
					check_xml(xmlTextWriterWriteElement(writer, BAD_CAST element->name, BAD_CAST cache_get_xmlform(element->label)));
				} else {
					const struct eid_vwr_cache_item *item = cache_get_data(element->label);
					check_xml(xmlTextWriterStartElement(writer, BAD_CAST element->name));
					check_xml(xmlTextWriterWriteBase64(writer, item->data, 0, item->len));
					check_xml(xmlTextWriterEndElement(writer));
				}
			}
		}
		element++;
	}
	return 0;
}

int eid_vwr_serialize(void* data) {
	xmlTextWriterPtr writer;
	int i, rc;
	const char* filename = (const char*)data;

	writer = xmlNewTextWriterFilename(filename, 0);
	if(writer == NULL) {
		be_log(EID_VWR_LOG_COARSE, "Could not open file");
		return -1;
	}

	check_xml(xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL));
	check_xml(write_elements(writer, toplevel));
	check_xml(xmlTextWriterEndDocument(writer));

	xmlFreeTextWriter(writer);

	return 0;
}
