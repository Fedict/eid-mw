#include <backend.h>
#include <eid-viewer/verify_cert.h>
#include <eid-viewer/certhelpers.h>

#include <openssl/err.h>
#include <openssl/ocsp.h>
#include <openssl/opensslv.h>
#include <openssl/x509.h>

#include <assert.h>

#include <string.h>
#include <stdint.h>

#include "state.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef __APPLE__
#include "trustdirname.h"
#define CERTTRUSTDIR eid_vwr_osl_objc_trustdirname()
#else
#define CERTTRUSTDIR (DATAROOTDIR "/" PACKAGE_NAME "/trustdir")
#endif

#if OPENSSL_VERSION_NUMBER < 0x10100000L || defined(LIBRESSL_VERSION_NUMBER)
#define X509_get0_extensions(ce) ((ce)->cert_info->extensions)

#define X509_get0_tbs_sigalg(ci) ((ci)->sig_alg)
#define X509_OBJECT_new() calloc(sizeof(X509_OBJECT), 1)
#define X509_OBJECT_free(o) free(o)
#define X509_get0_pubkey(x) (x->cert_info->key->pkey)
#define X509_OBJECT_get0_X509(o) (o->data.x509)

#define EVP_MD_CTX_new EVP_MD_CTX_create
#define EVP_MD_CTX_free EVP_MD_CTX_destroy

#define algobjcast(obj) ((ASN1_OBJECT**)obj)
#define ppvalcast(obj) ((void**)obj)

#define OCSP_resp_get0_certs(bresp) ((bresp)->certs)
#else
#define algobjcast(obj) ((const ASN1_OBJECT**)obj)
#define ppvalcast(obj) ((const void**)obj)
#endif

// All valid OCSP URLs should have the following as their prefix:
#define VALID_OCSP_PREFIX "http://ocsp.eid.belgium.be"
// All valid CRL URLs should have the following as their prefix:
#define VALID_CRL_PREFIX "http://crl.eid.belgium.be"

static void log_ssl_error(char* message) {
	char buf[100];
	unsigned long error = ERR_get_error();

	be_log(EID_VWR_LOG_COARSE, message);
	while(error != 0) {
		ERR_error_string_n(error, buf, sizeof buf);
		buf[99] = '\0';
		be_log(EID_VWR_LOG_DETAIL, "libssl error: %s", buf);
		error = ERR_get_error();
	}
}

enum eid_vwr_result eid_vwr_verify_int_cert(const void *certificate, size_t certlen, const void *ca, size_t calen, const void *(*perform_http_request)(char*, long*, void**), void(*free_http_request)(void*)) {
#if OPENSSL_VERSION_NUMBER < 0x10100000L || defined(LIBRESSL_VERSION_NUMBER)
	be_log(EID_VWR_LOG_DETAIL, "Ignoring CRL check: OpenSSL 1.1 required");
	return EID_VWR_RES_UNKNOWN;
#else
	X509 *cert_i = NULL, *ca_i = NULL;
	const STACK_OF(X509_EXTENSION)* exts;
	char *url = NULL;
	int i, j;
	long len;
	void *http_handle = NULL;
	const unsigned char *response;
	enum eid_vwr_result ret = EID_VWR_RES_UNKNOWN;
	EVP_PKEY *ca_k = NULL;
	X509_CRL *crl = NULL;

	if(d2i_X509(&cert_i, (const unsigned char**)&certificate, certlen) == NULL) {
		log_ssl_error("Could not parse certificate");
		ret = EID_VWR_RES_WARNING;
		goto exit;
	}
	if(d2i_X509(&ca_i, (const unsigned char**)&ca, calen) == NULL) {
		log_ssl_error("Could not parse root certificate");
		ret = EID_VWR_RES_WARNING;
		goto exit;
	}
	exts = X509_get0_extensions(cert_i);

	CRL_DIST_POINTS *pts = X509V3_get_d2i(exts, NID_crl_distribution_points, NULL, NULL);
	for(i=0; i<sk_DIST_POINT_num(pts); i++) {
		DIST_POINT *point = sk_DIST_POINT_value(pts, i);
		for(j=0; j<sk_GENERAL_NAME_num(point->distpoint->name.fullname); j++) {
			GENERAL_NAME *name = sk_GENERAL_NAME_value(point->distpoint->name.fullname, j);
			if(name->type == GEN_URI) {
				url = (char*)name->d.ia5->data;
			}
		}
	}
	if(!url) {
		be_log(EID_VWR_LOG_NORMAL, "No CRL URL found. Is this an actual eID card?");
		ret = EID_VWR_RES_FAILED;
		goto exit;
	}
	if((ca_k = X509_get_pubkey(ca_i)) == NULL) {
		be_log(EID_VWR_LOG_NORMAL, "Could not get root certificate public key. Is this an actual eID card?");
		ret = EID_VWR_RES_FAILED;
		goto exit;
	}
	response = perform_http_request(url, &len, &http_handle);
	if(!response) {
		be_log(EID_VWR_LOG_DETAIL, "HTTP request for CRL failed, skipping CRL check");
		goto exit;
	}
	if(d2i_X509_CRL(&crl, &response, len) == NULL) {
		be_log(EID_VWR_LOG_DETAIL, "CRL could not be parsed; skipping CRL check");
		goto exit;
	}
	if(!X509_CRL_verify(crl, ca_k)) {
		be_log(EID_VWR_LOG_NORMAL, "Found certificate revocation list with invalid signature. Certificates not valid");
		ret = EID_VWR_RES_FAILED;
		goto exit;
	}
	if(X509_CRL_get0_by_cert(crl, NULL, cert_i)) {
		be_log(EID_VWR_LOG_ERROR, "Intermediate certificate is revoked! Certificates are not valid");
		ret = EID_VWR_RES_FAILED;
		goto exit;
	}
	ret = EID_VWR_RES_SUCCESS;
exit:
	if(ca_k != NULL) {
		EVP_PKEY_free(ca_k);
	}
	if(http_handle != NULL) {
		free_http_request(http_handle);
	}
	return ret;
#endif
}

enum eid_vwr_result eid_vwr_verify_cert(const void *certificate, size_t certlen, const void *ca, size_t calen, const void*(*perform_ocsp_request)(char*, void*, long, long*, void**), void(*free_ocsp_request)(void*)) {
	X509 *cert_i = NULL, *ca_i = NULL;
	const STACK_OF(X509_EXTENSION)* exts;
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
	const EVP_MD *md;
	void *ocsp_handle;
	enum eid_vwr_result ret = EID_VWR_RES_UNKNOWN;
	STACK_OF(X509) *certs_dup = NULL;

	if(d2i_X509(&cert_i, (const unsigned char**)&certificate, certlen) == NULL) {
		log_ssl_error("Could not parse entity certificate");
		ret = EID_VWR_RES_WARNING;
		goto exit;
	}
	if(d2i_X509(&ca_i, (const unsigned char**)&ca, calen) == NULL) {
		log_ssl_error("Could not parse CA certificate");
		ret = EID_VWR_RES_WARNING;
		goto exit;
	}
	exts = X509_get0_extensions(cert_i);

	for(i=0; i<sk_X509_EXTENSION_num(exts); i++) {
		X509_EXTENSION *ex = sk_X509_EXTENSION_value(exts, i);
		ASN1_OBJECT *obj = X509_EXTENSION_get_object(ex);
		int nid = OBJ_obj2nid(obj);
		if(nid == NID_info_access) {
			const X509V3_EXT_METHOD *method;
			void *ext_str;
			ASN1_OCTET_STRING *exval = X509_EXTENSION_get_data(ex);
			const unsigned char *p = exval->data;
			STACK_OF(CONF_VALUE) *nval = NULL;

			if(!(method = X509V3_EXT_get(ex)) || !(method->i2v)) {
				log_ssl_error("Could not find OCSP URL information");
				ret = EID_VWR_RES_FAILED;
				goto exit;
			}
			if(method->it) {
				ext_str = ASN1_item_d2i(NULL, &p, exval->length, ASN1_ITEM_ptr(method->it));
			} else {
				ext_str = method->d2i(NULL, &p, exval->length);
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
	md = EVP_sha256();
	id = OCSP_cert_to_id(md, cert_i, ca_i);
	OCSP_request_add0_id(req, id);
	OCSP_request_add1_nonce(req, 0, -1);
	len = (long)i2d_OCSP_REQUEST(req, &data);

	response = perform_ocsp_request(url, data, len, &len, &ocsp_handle);
	if(!response) {
		free_ocsp_request(ocsp_handle);
		ret = EID_VWR_RES_UNKNOWN;
		goto exit;
	}

	resp = d2i_OCSP_RESPONSE(NULL, (const unsigned char**)&(response), len);
	switch(OCSP_response_status(resp)) {
		case OCSP_RESPONSE_STATUS_SUCCESSFUL:
			break;
		case OCSP_RESPONSE_STATUS_MALFORMEDREQUEST:
			ret = EID_VWR_RES_UNKNOWN;
			status_string = "malformed request"; break;
		case OCSP_RESPONSE_STATUS_INTERNALERROR:
			ret = EID_VWR_RES_UNKNOWN;
			status_string = "internal error"; break;
		case OCSP_RESPONSE_STATUS_TRYLATER:
			ret = EID_VWR_RES_UNKNOWN;
			status_string = "try again later"; break;
		case OCSP_RESPONSE_STATUS_SIGREQUIRED:
			ret = EID_VWR_RES_UNKNOWN;
			status_string = "signature required"; break;
		case OCSP_RESPONSE_STATUS_UNAUTHORIZED:
			ret = EID_VWR_RES_FAILED;
			status_string = "invalid certificate, algorithm, or root or intermediate certificate"; break;
	}
	if(status_string != NULL) {
		be_log(EID_VWR_LOG_COARSE, "eID certificate check failed: %s", status_string);
		goto exit;
	}
	bresp = OCSP_response_get1_basic(resp);
	free_ocsp_request(ocsp_handle);
	OCSP_resp_find_status(bresp, id, &stat, &reason, &rev, &this, &next);
	switch(stat) {
		case V_OCSP_CERTSTATUS_GOOD:
			break;
		case V_OCSP_CERTSTATUS_REVOKED:
			ret = EID_VWR_RES_FAILED;
			status_string = "revoked"; break;
		case V_OCSP_CERTSTATUS_UNKNOWN:
			ret = EID_VWR_RES_WARNING;
			status_string = "unknown"; break;
		default:
			ret = EID_VWR_RES_FAILED;
			status_string = "weird"; break;
	}
	if(status_string != NULL) {
		be_log(EID_VWR_LOG_NORMAL, "eID certificate %s", status_string);
		goto exit;
	}
	store = X509_STORE_new();
	lookup = X509_STORE_add_lookup(store, X509_LOOKUP_hash_dir());
	X509_LOOKUP_add_dir(lookup, CERTTRUSTDIR, X509_FILETYPE_PEM);
	certs_dup = sk_X509_dup(OCSP_resp_get0_certs(bresp));
	if(OCSP_basic_verify(bresp, certs_dup, store, 0) <= 0) {
		log_ssl_error("OCSP signature invalid, or root certificate unknown");
		ret = EID_VWR_RES_FAILED;
		goto exit;
	}

	ret = EID_VWR_RES_SUCCESS;
exit:
	if(store) {
		X509_STORE_free(store);
	}
	if(certs_dup) {
		sk_X509_free(certs_dup);
	}
	return ret;
}

enum eid_vwr_result eid_vwr_verify_rrncert(const void* certificate, size_t certlen, const void *root_cert, size_t root_len) {
	X509 *cert_i = NULL;
	X509 *root_i = NULL;
	X509_STORE *store = NULL;
	X509_LOOKUP *lookup = NULL;
	X509_STORE_CTX *ctx = NULL;
	X509_OBJECT *root_object = X509_OBJECT_new();
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
		ret = EID_VWR_RES_WARNING;
		goto exit;
	}

	ctx = X509_STORE_CTX_new();
	if(X509_STORE_CTX_init(ctx, store, cert_i, NULL) != 1) {
		be_log(EID_VWR_LOG_NORMAL, "RRN certificate verification failed: could not build context");
		ret = EID_VWR_RES_UNKNOWN;
		goto exit;
	}

	if(X509_verify_cert(ctx) != 1) {
		log_ssl_error("RRN certificate verification failed: invalid signature, or invalid root certificate.");
		ret = EID_VWR_RES_FAILED;
		goto exit;
	}

	if(d2i_X509(&root_i, (const unsigned char**)&root_cert, root_len) == NULL) {
		be_log(EID_VWR_LOG_NORMAL, "RRN certificate verification failed: Could not parse root certificate");
		ret = EID_VWR_RES_WARNING;
		goto exit;
	}
	if(X509_LOOKUP_by_subject(lookup, X509_LU_X509, X509_get_issuer_name(cert_i), root_object) != 1) {
		be_log(EID_VWR_LOG_NORMAL, "RRN certificate verification verification WARNING: root certificate not found in trust store (should not happen)");
		ret = EID_VWR_RES_WARNING;
		goto exit;
	}
	EVP_PKEY *cert_key = X509_get0_pubkey(root_i);
	X509 *store_cert = X509_OBJECT_get0_X509(root_object);
	EVP_PKEY *trusted_key = X509_get0_pubkey(store_cert);
	if(EVP_PKEY_cmp(trusted_key, cert_key) != 1) {
		be_log(EID_VWR_LOG_COARSE, "RRN certificate verification failed: root certificate on card is not RRN certificate issuer");
		ret = EID_VWR_RES_WARNING;
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

char* eid_vwr_x509_get_details(const void* certificate, size_t certlen) {
	X509* cert = NULL;
	BIO* b = BIO_new(BIO_s_mem());
	char* rv;
	BUF_MEM *p;

	if(d2i_X509(&cert, (const unsigned char**)&certificate, certlen) == NULL) {
		log_ssl_error("Could not parse entity certificate");
		return NULL;
	}
	X509_print_ex(b, cert, 0, 0);
	BIO_get_mem_ptr(b, &p);
	rv = malloc(p->length + 1);
	strncpy(rv, p->data, p->length);

	BIO_free(b);

	return rv;
}

enum eid_vwr_result eid_vwr_verify_root_cert(const void *certificate, size_t certlen) {
	X509 *cert_i = NULL;
	X509_STORE *store = NULL;
	X509_LOOKUP *lookup = NULL;
	X509_OBJECT *store_key = X509_OBJECT_new();
	enum eid_vwr_result ret;

	store = X509_STORE_new();
	if(!(lookup = X509_STORE_add_lookup(store, X509_LOOKUP_hash_dir()))) {
		be_log(EID_VWR_LOG_COARSE, "Root certificate verification failed: could not open trust store");
		ret = EID_VWR_RES_UNKNOWN;
		goto out;
	}
	X509_LOOKUP_add_dir(lookup, CERTTRUSTDIR, X509_FILETYPE_PEM);
	if(d2i_X509(&cert_i, (const unsigned char**)&certificate, certlen) == NULL) {
		be_log(EID_VWR_LOG_NORMAL, "Root certificate verification failed: could not parse root certificate");
		ret = EID_VWR_RES_WARNING;
		goto out;
	}
	if(X509_LOOKUP_by_subject(lookup, X509_LU_X509, X509_get_subject_name(cert_i), store_key) != 1) {
		be_log(EID_VWR_LOG_COARSE, "Root certificate verification failed: root certificate not found in trust store");
		ret = EID_VWR_RES_FAILED;
		goto out;
	}
	EVP_PKEY *cert_key = X509_get0_pubkey(cert_i);
	X509 *store_cert = X509_OBJECT_get0_X509(store_key);
	EVP_PKEY *trusted_key = X509_get0_pubkey(store_cert);
	if(EVP_PKEY_cmp(cert_key, trusted_key) != 1) {
		be_log(EID_VWR_LOG_COARSE, "Root certificate verification failed: public key of root certificate does not match public key of certificate from truststore");
		ret = EID_VWR_RES_FAILED;
		goto out;
	}
	ret = EID_VWR_RES_SUCCESS;
out:
	X509_OBJECT_free(store_key);
	return ret;
}

void eid_vwr_check_signature(const void* pubkey, size_t pubkeylen, const void* sig, size_t siglen, const void* data, size_t datalen) {
	EVP_PKEY *pk = d2i_PUBKEY(NULL, (const unsigned char**)(&pubkey), pubkeylen);
	EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
	EVP_PKEY_CTX *pctx;
	const EVP_MD *md = EVP_sha384();
	unsigned char *dersig = NULL;
	if(EVP_DigestVerifyInit(mdctx, &pctx, md, NULL, pk) != 1) {
		be_log(EID_VWR_LOG_ERROR, "Initialization for basic key validation failed");
		goto err;
	}
	if(EVP_DigestVerifyUpdate(mdctx, (const unsigned char*)data, datalen) != 1) {
		be_log(EID_VWR_LOG_ERROR, "Hashing for basic key validation failed");
		goto err;
	}
	ECDSA_SIG *ec_sig;
	BIGNUM *r;
	BIGNUM *s;
	ec_sig = ECDSA_SIG_new();
	if((r = BN_bin2bn(sig, siglen / 2, NULL)) == NULL) {
		be_log(EID_VWR_LOG_ERROR, "Could not convert R part of basic key signature");
		goto err;
	}
	if((s = BN_bin2bn(sig + (siglen / 2), siglen / 2, NULL)) == NULL) {
		be_log(EID_VWR_LOG_ERROR, "Could not convert S part of basic key signature");
		goto err;
	}
	siglen = i2d_ECDSA_SIG(ec_sig, NULL);
	dersig = malloc(siglen);
	siglen = i2d_ECDSA_SIG(ec_sig, &dersig);
	if(EVP_DigestVerifyFinal(mdctx, dersig, siglen) != 1) {
		be_log(EID_VWR_LOG_ERROR, "Basic key signature fails validation. Is this a forged eID card?");
		goto err;
	}
	goto end;
err:
	sm_handle_event(EVENT_DATA_INVALID, NULL, NULL, NULL);
end:
	if(dersig) {
		free(dersig);
	}
	if(ec_sig) {
		ECDSA_SIG_free(ec_sig);
	}
	EVP_MD_CTX_free(mdctx);
	EVP_PKEY_free(pk);
	return;
}
