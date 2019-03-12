#include "cache.h"
#include "backend.h"

#include "xmlmap.h"
#include "xsdloc.h"
#include "state.h"

#include <string.h>
#include <stdbool.h>
#include "p11.h"
#include "conversions.h"

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlreader.h>

// libxml2 has a function to write Base64-encoded data, but no function to read
// the same data, so we need our own decoder...
#include <b64/base64dec.h>

#include <assert.h>

#define check_xml(call) if((rc = call) < 0) { \
	be_log(EID_VWR_LOG_DETAIL, "Error while dealing with file (calling '%s'): %d", #call, rc); \
	goto out; \
}

static bool have_attribute_data(struct attribute_desc *attribute) {
	while(attribute != NULL && attribute->name) {
		if(cache_have_label(attribute->label)) {
			return true;
		}
		attribute++;
	}
	return false;
}

static bool have_element_data(struct element_desc *element) {
	while(element != NULL && element->name) {
		if(element->label == NULL) {
			if(have_attribute_data(element->attributes) || have_element_data(element->child_elements)) {
				return true;
			}
		} else {
			if(cache_have_label(element->label)) {
				return true;
			}
		}
		element++;
	}
	return false;
}

/* Write attributes to the description in *attribute */
static int write_attributes(xmlTextWriterPtr writer, struct attribute_desc *attribute) {
	int rc = 0;
	char* val = NULL;
	while(attribute->name) {
		int have_cache = cache_have_label(attribute->label);
		if(attribute->reqd && !have_cache) {
			be_log(EID_VWR_LOG_COARSE, "Could not generate XML version: no data found for required label %s", attribute->label);
			return -1;
		}
		if(have_cache) {
			val = cache_get_xmlform(attribute->label);
			if(strlen(val) || attribute->reqd) {
				check_xml(xmlTextWriterWriteAttribute(writer, BAD_CAST attribute->name, BAD_CAST val));
			}
			free(val);
			val = NULL;
		}
		attribute++;
	}

	rc = 0;
out:
	if(val != NULL) {
		free(val);
	}
	return rc;
}

/* Write elements to the description in *element */
static int write_elements(xmlTextWriterPtr writer, struct element_desc *element) {
	int rc;
	char* val = NULL;
	for(;element->name != NULL; element++) {
		if(element->label == NULL) {
			assert(element->child_elements != NULL || element->attributes != NULL);
			if(!element->reqd) {
				if(!have_attribute_data(element->attributes) || !have_element_data(element->child_elements)) {
					continue;
				}
			}
			check_xml(xmlTextWriterStartElement(writer, BAD_CAST element->name));
			if(element->attributes != NULL) {
				check_xml(write_attributes(writer, element->attributes));
			}
			if(element->child_elements != NULL) {
				check_xml(write_elements(writer, element->child_elements));
			}
			check_xml(xmlTextWriterEndElement(writer));
		} else {
			int have_cache = cache_have_label(element->label);
			assert(element->child_elements == NULL);

			if(element->reqd && !have_cache) {
				be_log(EID_VWR_LOG_ERROR, "Could not write file: no data found for required label %s", element->label);
				return -1;
			}
			if(have_cache) {
				val = cache_get_xmlform(element->label);
				if(!element->is_b64) {
					check_xml(xmlTextWriterWriteElement(writer, BAD_CAST element->name, BAD_CAST cache_get_xmlform(element->label)));
				} else {
					const struct eid_vwr_cache_item *item = cache_get_data(element->label);
					check_xml(xmlTextWriterStartElement(writer, BAD_CAST element->name));
					check_xml(xmlTextWriterWriteBase64(writer, item->data, 0, item->len));
					check_xml(xmlTextWriterEndElement(writer));
				}
				free(val);
				val = NULL;
			}
		}
	}
	rc=0;
out:
	if(val != NULL) {
		free(val);
	}
	return rc;
}

/* Called when we enter the FILE or TOKEN states.
   Note: in theory it would be possible to just store the xml data we
   read from a file in the deserialize event into the cache as-is.
   However, that has a few downsides:
   - If the file has invalid XML or superfluous data, we will write that
     same data back later on.
   - If we would want to modify the XML format at some undefined point
     in the future, it is a good idea generally to ensure that we
     already generate new XML data */
int eid_vwr_gen_xml(void* data EIDV_UNUSED) {
	xmlTextWriterPtr writer = NULL;
	int rc;
	xmlBufferPtr buf;

	buf = xmlBufferCreate();
	if(buf == NULL) {
		be_log(EID_VWR_LOG_COARSE, "Could not generate XML format: error creating the xml buffer");
		rc = -1;
		goto out;
	}
	writer = xmlNewTextWriterMemory(buf, 0);
	if(writer == NULL) {
		be_log(EID_VWR_LOG_ERROR, "Could not open file");
		rc = -1;
		goto out;
	}

	check_xml(xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL));
	check_xml(write_elements(writer, toplevel));
	check_xml(xmlTextWriterEndDocument(writer));

	cache_add("xml", (char*)buf->content, strlen((char*)buf->content));

	rc=0;
out:
	if(writer) {
		xmlFreeTextWriter(writer);
	}
	if(buf) {
		xmlBufferFree(buf);
	}
	return rc;
}

/* Read data from the cache and store it to the file whose name we get
 * in the *data argument */
int eid_vwr_serialize(void* data) {
	int rv = 0;
	const struct eid_vwr_cache_item* item = cache_get_data("xml");
	FILE* f = fopen((const char*)data, "w");
	if(!f) {
		return 1;
	}
	fwrite(item->data, item->len, 1, f);
	rv = fclose(f);
	sm_handle_event(EVENT_SERIALIZE_READY, NULL, NULL, NULL);
	return rv;
}

/* Read elements according to the description in *element */
static int read_elements(xmlTextReaderPtr reader, struct element_desc* element EIDV_UNUSED) {
	int rc;
	void* val = NULL;
	while((rc = xmlTextReaderRead(reader)) > 0) {
		const xmlChar *curnode = xmlTextReaderConstLocalName(reader);
		struct element_desc *desc = get_elemdesc((const char*)curnode);
		struct attribute_desc *att;
		/* libxml2 will generate many "nodes" per element. We're
		 * only interested in element nodes, not in CDATA nodes
		 * or xml header nodes etc */
		if(xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
			continue;
		}
		/* However, if the element has attributes, we're also
		 * interested in those. */
		if(xmlTextReaderHasAttributes(reader) > 0) {
			if(desc->attributes == NULL) {
				be_log(EID_VWR_LOG_ERROR, "Could not read file: found attribute on an element that shouldn't have one.");
				return -1;
			}
			for(att = desc->attributes; att->name != NULL; att++) {
				xmlChar* value = xmlTextReaderGetAttribute(reader, (const xmlChar*)att->name);
				if(value) {
					int len;
					val = convert_from_xml(att->label, (const EID_CHAR*)value, &len);
					cache_add(att->label, val, len);
					eid_vwr_p11_to_ui(att->label, val, len);
					val = NULL;
					xmlFree(value);
				} else {
					if(att->reqd) {
						be_log(EID_VWR_LOG_ERROR, "Could not read file: missing attribute %s on %s", att->name, desc->name);
						return -1;
					}
				}
			}
		}
		/* If we recognize this element, parse it */
		if(desc->label != NULL) {
			int len;
			check_xml(xmlTextReaderRead(reader));
			if(desc->is_b64) {
				const xmlChar* tmp;
				base64_decodestate state;
				base64_init_decodestate(&state);
				tmp = xmlTextReaderConstValue(reader);
				len = (int)strlen((const char*)tmp);
				val = malloc(len);
				len = base64_decode_block((const char*)tmp, len, val, &state);
			} else {
				val = convert_from_xml(desc->label, (const char*)xmlTextReaderConstValue(reader), &len);
			}
			cache_add(desc->label, val, len);
			eid_vwr_p11_to_ui(desc->label, val, len);
			be_log(EID_VWR_LOG_DETAIL, "found data for label %s", desc->label);
			val = NULL;
		}
	}
out:
	if(val != NULL) {
		free(val);
	}
	return rc;
}

/* Read data from the file whose name we get in the *data argument,
 * issue events to the UI with new data, and update the cache. */
int eid_vwr_deserialize(void* data) {
	xmlTextReaderPtr reader = NULL;
	const char* filename = (const char*)data;
	int rc;

	reader = xmlNewTextReaderFilename(filename);
	if(reader == NULL) {
		be_log(EID_VWR_LOG_ERROR, "Could not open file");
		return -1;
	}

	be_newsource(EID_VWR_SRC_FILE);

	/* Enable validation. This requires the XSD, which we ship as a
	 * file "somewhere", in an OS-dependent way. */
	check_xml(xmlTextReaderSchemaValidate(reader, get_xsdloc()));
	check_xml(read_elements(reader, toplevel));
out:
	if(rc) {
		xmlError* err = xmlGetLastError();
		if(err != NULL) {
			be_log(EID_VWR_LOG_ERROR, "Could not read file: %s", err->message);
		}
	}
	if(reader) {
		xmlFreeTextReader(reader);
	}
	if(!rc) {
		sm_handle_event(EVENT_READ_READY, NULL, NULL, NULL);
	}
	return rc;
}
