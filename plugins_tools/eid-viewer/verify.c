#include <backend.h>
#include <verify_cert.h>
#include <certhelpers.h>

#include <openssl/err.h>
#include <openssl/ocsp.h>
#include <openssl/x509.h>

#include <string.h>

#include <config.h>

#define PKGDATADIR "/home/wouter"

// All valid OCSP URLs should have the following as their prefix:

#define VALID_OCSP_PREFIX "http://ocsp.eid.belgium.be/"

static void log_error(char* message) {
	char buf[100];
	unsigned long error = ERR_get_error();

	ensure_inited();
	ERR_error_string_n(error, buf, sizeof buf);
	buf[99] = '\0';
	be_log(EID_VWR_LOG_ERROR, message);
	be_log(EID_VWR_LOG_DETAIL, "libssl error: %s", buf);
}

enum eid_vwr_result eid_vwr_verify_cert(void* certificate, size_t certlen, void* ca, size_t calen, void*(*perform_ocsp_request)(char*, void*, long, long*)) {
	X509 *cert_i = NULL, *ca_i = NULL;
	X509_CINF *certv3;
	char* url = NULL;
	int i, j, stat, reason;
	OCSP_REQUEST *req;
	OCSP_CERTID *id;
	OCSP_RESPONSE *resp;
	OCSP_BASICRESP *bresp;
	unsigned char *data = NULL;
	unsigned char *response = NULL;
	long len;
	char *status_string = NULL;
	ASN1_GENERALIZEDTIME *rev, *this, *next;
	X509_STORE *store;
	X509_LOOKUP *lookup;

	if(d2i_X509(&cert_i, (const unsigned char**)&certificate, certlen) == NULL) {
		log_error("Could not parse entity certificate");
		return EID_VWR_RES_FAILED;
	}
	if(d2i_X509(&ca_i, (const unsigned char**)&ca, calen) == NULL) {
		log_error("Could not parse CA certificate");
		return EID_VWR_RES_FAILED;
	}
	certv3 = cert_i->cert_info;
	
	for(i=0; i<sk_X509_EXTENSION_num(certv3->extensions); i++) {
		X509_EXTENSION *ex = sk_X509_EXTENSION_value(certv3->extensions, i);
		ASN1_OBJECT *obj = X509_EXTENSION_get_object(ex);
		int nid = OBJ_obj2nid(obj);
		if(nid == NID_info_access) {
			const X509V3_EXT_METHOD *method;
			void *ext_str;
			const unsigned char *p = ex->value->data;
			STACK_OF(CONF_VALUE) *nval = NULL;

			if(!(method = X509V3_EXT_get(ex)) || !(method->i2v)) {
				log_error("Could not find OCSP URL information");
				return EID_VWR_RES_FAILED;
			}
			if(method->it) {
				ext_str = ASN1_item_d2i(NULL, &p, ex->value->length, ASN1_ITEM_ptr(method->it));
			} else {
				ext_str = method->d2i(NULL, &p, ex->value->length);
			}
			if(!(nval = method->i2v(method, ext_str, NULL))) {
				log_error("Could not read OCSP URL from certificate");
				return EID_VWR_RES_FAILED;
			}
			for(j=0; j<sk_CONF_VALUE_num(nval); j++) {
				CONF_VALUE *val = sk_CONF_VALUE_value(nval, j);
				if(val->name != NULL && val->value != NULL) {
					if(!strcmp(val->name, "OCSP - URI")) {
						url = val->value;
						if(strncmp(url, VALID_OCSP_PREFIX, strlen(VALID_OCSP_PREFIX))) {
							be_log(EID_VWR_LOG_NORMAL, "Invalid OCSP URL. Is this an actual eID card?");
							return EID_VWR_RES_FAILED;
						}
					}
				}
			}
		}
	}
	if(!url) {
		be_log(EID_VWR_LOG_NORMAL, "No OCSP URL found. Is this an actual eID card?");
		return EID_VWR_RES_FAILED;
	}

	req = OCSP_REQUEST_new();
	// !!! TODO !!!
	//have this choose the certificate algorithm based on the
	// algorithm of the certificate chain, rather than hardcoding to SHA1
	id = OCSP_cert_to_id(EVP_sha1(), cert_i, ca_i);
	OCSP_request_add0_id(req, id);
	OCSP_request_add1_nonce(req, 0, -1);
	len = (long)i2d_OCSP_REQUEST(req, &data);

	response = perform_ocsp_request(url, data, len, &len);

	resp = d2i_OCSP_RESPONSE(NULL, (const unsigned char**)&(response), len);
	switch(OCSP_response_status(resp)) {
		case OCSP_RESPONSE_STATUS_SUCCESSFUL:
			break;
		case OCSP_RESPONSE_STATUS_MALFORMEDREQUEST:
			status_string = "malformed request"; break;
		case OCSP_RESPONSE_STATUS_INTERNALERROR:
			status_string = "internal error"; break;
		case OCSP_RESPONSE_STATUS_TRYLATER:
			status_string = "try later"; break;
		case OCSP_RESPONSE_STATUS_SIGREQUIRED:
			status_string = "signature required"; break;
		case OCSP_RESPONSE_STATUS_UNAUTHORIZED:
			status_string = "unauthorized"; break;
	}
	if(status_string != NULL) {
		be_log(EID_VWR_LOG_NORMAL, "eID certificate check failed: %s", status_string);
		return EID_VWR_RES_FAILED;
	}
	bresp = OCSP_response_get1_basic(resp);
	OCSP_resp_find_status(bresp, id, &stat, &reason, &rev, &this, &next);
	switch(stat) {
		case V_OCSP_CERTSTATUS_GOOD:
			break;
		case V_OCSP_CERTSTATUS_REVOKED:
			status_string = "revoked"; break;
		case V_OCSP_CERTSTATUS_UNKNOWN:
			status_string = "unknown"; break;
		default:
			status_string = "weird"; break;
	}
	if(status_string != NULL) {
		be_log(EID_VWR_LOG_NORMAL, "eID certificate %s", status_string);
		return EID_VWR_RES_FAILED;
	}
	store = X509_STORE_new();
	lookup = X509_STORE_add_lookup(store, X509_LOOKUP_hash_dir());
	X509_LOOKUP_add_dir(lookup, PKGDATADIR "/trustdir", X509_FILETYPE_PEM);
	if(OCSP_basic_verify(bresp, bresp->certs, store, 0) <= 0) {
		log_error("OCSP signature invalid, or root certificate unknown");
		return EID_VWR_RES_FAILED;
	}
	return EID_VWR_RES_SUCCESS;
}
