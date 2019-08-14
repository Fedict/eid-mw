#include <eid-viewer/oslayer.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlreader.h>
#include <b64/base64dec.h>
#include <string.h>
#include <xsdloc.h>
#include <backend.h>

#define check_xml(call) if((rc = call) < 0) { \
be_log(EID_VWR_LOG_DETAIL, "Error while dealing with file (calling '%s'): %d", #call, rc); \
goto out; \
}

/* Produce a preview for a given eID file: parse the JPEG photo out. */
struct eid_vwr_preview* eid_vwr_get_preview(const char* filename) {
	int rc;
	xmlTextReaderPtr reader = NULL;
	struct eid_vwr_preview* p = calloc(sizeof(struct eid_vwr_preview), 1);
	
	if(!filename) {
		return p;
	}
	if(strstr(filename, ".eid") != filename + (strlen(filename) -4)) {
		return p;
	}
	
	reader = xmlNewTextReaderFilename(filename);
	if(!reader) {
		return p;
	}
	while((rc = xmlTextReaderRead(reader)) > 0) {
		const char *curnode = (const char*)xmlTextReaderConstLocalName(reader);
		if(!strcmp(curnode, "photo")) {
			check_xml(xmlTextReaderRead(reader));
			const char* tmp;
			tmp = (const char*)xmlTextReaderConstValue(reader);
			base64_decodestate(state);
			base64_init_decodestate(&state);
			p->imagelen = strlen(tmp);
			p->imagedata = malloc(p->imagelen);
			p->imagelen = base64_decode_block(tmp, (int)p->imagelen, p->imagedata, &state);
			p->have_data = 1;
			goto out;
		}
	}
out:
	if(reader) {
		xmlFreeTextReader(reader);
	}
	return p;
}

void eid_vwr_release_preview(struct eid_vwr_preview* prv) {
	if(prv->have_data) {
		free(prv->imagedata);
	}
	free(prv);
}
