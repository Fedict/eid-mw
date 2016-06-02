#include <backend.h>
#include <eid-viewer/verify_cert.h>
#include <eid-viewer/certhelpers.h>

#include <openssl/err.h>
#include <openssl/ocsp.h>
#include <openssl/x509.h>

#include <string.h>
#include <stdint.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef __APPLE__
#include "trustdirname.h"
#define CERTTRUSTDIR eid_vwr_osl_objc_trustdirname()
#else
#define CERTTRUSTDIR (DATAROOTDIR "/" PACKAGE_NAME "/trustdir")
#endif
// All valid OCSP URLs should have the following as their prefix:

#define VALID_OCSP_PREFIX "http://ocsp.eid.belgium.be"

static void log_ssl_error(char* message) {
	char buf[100];
	unsigned long error = ERR_get_error();

	ensure_inited();
	ERR_error_string_n(error, buf, sizeof buf);
	buf[99] = '\0';
	be_log(EID_VWR_LOG_COARSE, message);
	be_log(EID_VWR_LOG_DETAIL, "libssl error: %s", buf);
}

enum eid_vwr_result eid_vwr_verify_cert(const void* certificate, size_t certlen, const void* ca, size_t calen, const void*(*perform_ocsp_request)(char*, void*, long, long*, void**), void(*free_ocsp_request)(void*)) {
	X509 *cert_i = NULL, *ca_i = NULL;
	X509_CINF *certv3;
	char* url = NULL;
	int i, j, stat, reason;
	OCSP_REQUEST *req;
	OCSP_CERTID *id;
	OCSP_RESPONSE *resp;
	OCSP_BASICRESP *bresp;
	unsigned char *data = NULL;
	const char *response = NULL;
	long len;
	char *status_string = NULL;
	ASN1_GENERALIZEDTIME *rev, *this, *next;
	X509_STORE *store = NULL;
	X509_LOOKUP *lookup = NULL;
	int md_nid, sig_nid, pkey_nid;
	intptr_t dummy;
	const EVP_MD *md;
	ASN1_OBJECT *algobj;
	void *ocsp_handle;
	enum eid_vwr_result ret = EID_VWR_RES_UNKNOWN;

	if(d2i_X509(&cert_i, (const unsigned char**)&certificate, certlen) == NULL) {
		log_ssl_error("Could not parse entity certificate");
		ret = EID_VWR_RES_FAILED;
		goto exit;
	}
	if(d2i_X509(&ca_i, (const unsigned char**)&ca, calen) == NULL) {
		log_ssl_error("Could not parse CA certificate");
		ret = EID_VWR_RES_FAILED;
		goto exit;
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
				log_ssl_error("Could not find OCSP URL information");
				ret = EID_VWR_RES_FAILED;
				goto exit;
			}
			if(method->it) {
				ext_str = ASN1_item_d2i(NULL, &p, ex->value->length, ASN1_ITEM_ptr(method->it));
			} else {
				ext_str = method->d2i(NULL, &p, ex->value->length);
			}
			if(!(nval = method->i2v(method, ext_str, NULL))) {
				log_ssl_error("Could not read OCSP URL from certificate");
				ret = EID_VWR_RES_FAILED;
				goto exit;
			}
			for(j=0; j<sk_CONF_VALUE_num(nval); j++) {
				CONF_VALUE *val = sk_CONF_VALUE_value(nval, j);
				if(val->name != NULL && val->value != NULL) {
					if(!strcmp(val->name, "OCSP - URI")) {
						url = val->value;
						if(strncmp(url, VALID_OCSP_PREFIX, strlen(VALID_OCSP_PREFIX))) {
							be_log(EID_VWR_LOG_NORMAL, "Invalid OCSP URL. Is this an actual eID card?");
							ret = EID_VWR_RES_FAILED;
							goto exit;
						}
					}
				}
			}
		}
	}
	if(!url) {
		be_log(EID_VWR_LOG_NORMAL, "No OCSP URL found. Is this an actual eID card?");
		ret = EID_VWR_RES_FAILED;
		goto exit;
	}

	req = OCSP_REQUEST_new();
	X509_ALGOR_get0(&algobj, (int*)&dummy, (void**)&dummy, cert_i->sig_alg);
	sig_nid = OBJ_obj2nid(algobj);
	OBJ_find_sigid_algs(sig_nid, &md_nid, &pkey_nid);
	if(md_nid == OBJ_sn2nid("SHA1")) {
		md = EVP_sha1();
	} else if (md_nid == OBJ_sn2nid("SHA256")) {
		md = EVP_sha256();
	} else {
		be_log(EID_VWR_LOG_NORMAL, "Card is signed with unknown hashing algorithm %s (aka %s), cannot continue", OBJ_nid2sn(md_nid), OBJ_nid2ln(md_nid));
		ret = EID_VWR_RES_FAILED;
		goto exit;
	}
	id = OCSP_cert_to_id(md, cert_i, ca_i);
	OCSP_request_add0_id(req, id);
	OCSP_request_add1_nonce(req, 0, -1);
	len = (long)i2d_OCSP_REQUEST(req, &data);

	response = perform_ocsp_request(url, data, len, &len, &ocsp_handle);
	if(!response) {
		free_ocsp_request(ocsp_handle);
		// we couldn't do an OCSP request, so retain the UNKNOWN status
		goto exit;
	}

	resp = d2i_OCSP_RESPONSE(NULL, (const unsigned char**)&(response), len);
	free_ocsp_request(ocsp_handle);
	switch(OCSP_response_status(resp)) {
		case OCSP_RESPONSE_STATUS_SUCCESSFUL:
			break;
		case OCSP_RESPONSE_STATUS_MALFORMEDREQUEST:
			status_string = "malformed request"; break;
		case OCSP_RESPONSE_STATUS_INTERNALERROR:
			status_string = "internal error"; break;
		case OCSP_RESPONSE_STATUS_TRYLATER:
			status_string = "try again later"; break;
		case OCSP_RESPONSE_STATUS_SIGREQUIRED:
			status_string = "signature required"; break;
		case OCSP_RESPONSE_STATUS_UNAUTHORIZED:
			status_string = "invalid certificate, algorithm, or root certificate"; break;
	}
	if(status_string != NULL) {
		be_log(EID_VWR_LOG_COARSE, "eID certificate check failed: %s", status_string);
		ret = EID_VWR_RES_FAILED;
		goto exit;
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
		ret = EID_VWR_RES_FAILED;
		goto exit;
	}
	store = X509_STORE_new();
	lookup = X509_STORE_add_lookup(store, X509_LOOKUP_hash_dir());
	X509_LOOKUP_add_dir(lookup, CERTTRUSTDIR, X509_FILETYPE_PEM);
	if(OCSP_basic_verify(bresp, bresp->certs, store, 0) <= 0) {
		be_log(EID_VWR_LOG_COARSE, "OCSP signature invalid, or root certificate unknown");
		ret = EID_VWR_RES_FAILED;
		goto exit;
	}

	ret = EID_VWR_RES_SUCCESS;
exit:
	if(store) {
		X509_STORE_free(store);
	}
	return ret;
}

enum eid_vwr_result eid_vwr_verify_rrncert(const void* certificate, size_t certlen) {
	X509 *cert_i = NULL;
	X509_STORE *store = NULL;
	X509_LOOKUP *lookup = NULL;
	X509_STORE_CTX *ctx = NULL;
	enum eid_vwr_result ret = EID_VWR_RES_UNKNOWN;

	store = X509_STORE_new();
	if(!(lookup = X509_STORE_add_lookup(store, X509_LOOKUP_hash_dir()))) {
		be_log(EID_VWR_LOG_NORMAL, "RRN certificate verification failed: Could not load root certificates");
		ret = EID_VWR_RES_UNKNOWN;
		goto exit;
	}
	X509_LOOKUP_add_dir(lookup, CERTTRUSTDIR, X509_FILETYPE_PEM);

	if(d2i_X509(&cert_i, (const unsigned char**)&certificate, certlen) == NULL) {
		be_log(EID_VWR_LOG_NORMAL, "RRN certificate verification failed: Could not parse RRN certificate");
		ret = EID_VWR_RES_UNKNOWN;
		goto exit;
	}

	ctx = X509_STORE_CTX_new();
	if(X509_STORE_CTX_init(ctx, store, cert_i, NULL) != 1) {
		be_log(EID_VWR_LOG_NORMAL, "RRN certificate verification failed: could not build context");
		ret = EID_VWR_RES_UNKNOWN;
		goto exit;
	}

	if(X509_verify_cert(ctx) != 1) {
		be_log(EID_VWR_LOG_COARSE, "RRN certificate verification failed: invalid signature, or invalid root certificate.");
		ret = EID_VWR_RES_FAILED;
		goto exit;
	}
	ret = EID_VWR_RES_SUCCESS;
exit:
	if(ctx) {
		X509_STORE_CTX_free(ctx);
	}
	if(store) {
		X509_STORE_free(store);
	}
	return ret;
}
