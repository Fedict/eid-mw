#include <backend.h>
#include <verify_cert.h>

#include <libxml/encoding.h>
#include <libxml/parser.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlreader.h>

#include <xmlsec/xmlsec.h>
#include <xmlsec/crypto.h>

#include <soap.h>

#include <string.h>

#include <pthread.h>
pthread_once_t once = PTHREAD_ONCE_INIT;

static void initialize() {
	xmlInitParser();
	xmlLoadExtDtdDefaultValue = XML_DETECT_IDS | XML_COMPLETE_ATTRS;
	xmlSubstituteEntitiesDefault(1);
	if(xmlSecInit() < 0) {
		be_log(EID_VWR_LOG_ERROR, "Error: xml-security library could not be initalized. Certificate verification is not possible.\n");
		return;
	}

	if(xmlSecCheckVersion() != 1) {
		be_log(EID_VWR_LOG_ERROR, "Error: xml-security library is not compatible. Certificate verification is not possible.\n");
		return;
	}

	if(xmlSecCryptoAppInit(NULL) < 0) {
		be_log(EID_VWR_LOG_ERROR, "Error: xml-security library crypto initialization failed. Certificate verification is not possible.\n");
		return;
	}
	
	if(xmlSecCryptoInit() < 0) {
		be_log(EID_VWR_LOG_ERROR, "Error: xml-sec-crypto initialization failed. Certificate verification is not possible.\n");
		return;
	}
}

#define check_xml(call) if((rv=(call)) < 0) { \
	be_log(EID_VWR_LOG_DETAIL, "Error while trying to verify certificate validity (calling '%s'): returned %d", #call, rv); \
	goto out; \
}

struct attr {
	char* name;
	char* val;
} ValidateAttrs[] = {
	{ "xmlns:SOAP-ENV", "http://schemas.xmlsoap.org/soap/envelope/" },
	{ "xmlns:SOAP-ENC", "http://schemas.xmlsoap.org/soap/encoding/" },
	{ "xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance" },
	{ "xmlns:xsd", "http://www.w3.org/2001/XMLSchema" },
	{ "xmlns:xkmsext", "urn:be:fedict:trust:xkms:extensions" },
	{ "xmlns:ds", "http://www.w3.org/2000/09/xmldsig#" },
	{ "xmlns:xenc", "http://www.w3.org/2001/04/xmlenc#" },
	{ "xmlns", "http://www.w3.org/2002/03/xkms#" },
	{ NULL, NULL },
};

enum eid_vwr_result eid_vwr_verify_cert(void* certificate, size_t certlen) {
	xmlBufferPtr buf;
	xmlTextWriterPtr writer = NULL;
	xmlTextReaderPtr reader = NULL;
	enum eid_vwr_result rc = EID_VWR_FAILED;
	struct attr *a = ValidateAttrs;
	char *reply;
	int rv;

	pthread_once(&once, initialize);

	buf = xmlBufferCreate();
	if(!buf) {
		be_log(EID_VWR_LOG_COARSE, "Could not query certificate validity: error creating the XML buffer");
		goto out;
	}
	writer = xmlNewTextWriterMemory(buf, 0);
	if(!writer) {
		be_log(EID_VWR_LOG_COARSE, "Could not query certificate validity: error initializing the XML writer");
		goto out;
	}
	
	/* SOAP wrapper */
	check_xml(xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL));
	check_xml(xmlTextWriterStartElement(writer, "SOAP-ENV:Envelope"));
	do {
		check_xml(xmlTextWriterWriteAttribute(writer, BAD_CAST a->name, BAD_CAST a->val));
	} while((++a)->name);
	check_xml(xmlTextWriterStartElement(writer, "SOAP-ENV:Body"));
	/* Initialize ValidateRequest */
	check_xml(xmlTextWriterStartElement(writer, BAD_CAST "ValidateRequest"));
	check_xml(xmlTextWriterWriteAttribute(writer, BAD_CAST "Service", BAD_CAST "https://trust-ws.services.belgium.be/eid-trust-service-ws/xkms2"));
	check_xml(xmlTextWriterWriteElement(writer, BAD_CAST "RespondWith", BAD_CAST "http://www.w3.org/2002/03/xkms#X509Cert"));
	check_xml(xmlTextWriterStartElement(writer, BAD_CAST "QueryKeyBinding"));
	check_xml(xmlTextWriterStartElement(writer, BAD_CAST "ds:KeyInfo"));
	check_xml(xmlTextWriterStartElement(writer, BAD_CAST "ds:X509Data"));
	check_xml(xmlTextWriterStartElement(writer, BAD_CAST "ds:X509Certificate"));
	check_xml(xmlTextWriterWriteBase64(writer, certificate, 0, certlen));
	check_xml(xmlTextWriterEndElement(writer)); // X509Certificate
	check_xml(xmlTextWriterEndElement(writer)); // X509Data
	check_xml(xmlTextWriterEndElement(writer)); // KeyInfo
	check_xml(xmlTextWriterWriteElement(writer, BAD_CAST "KeyUsage", BAD_CAST "http://www.w3.org/2002/03/xkms#Signature"));
	check_xml(xmlTextWriterEndDocument(writer));

	reply = eid_vwr_send_soap(buf->content);
	if(!reply) goto out;

	reader = xmlReaderForMemory(reply, strlen(reply), NULL, "UTF-8", 0);
	if(!reader) {
		be_log(EID_VWR_LOG_COARSE, "Could not query certificate validity: error initializing the XML reader");
		goto out;
	}
out:
	if(writer) {
		xmlFreeTextWriter(writer);
	}
	if(buf) {
		xmlBufferFree(buf);
	}
	return rc;
}
