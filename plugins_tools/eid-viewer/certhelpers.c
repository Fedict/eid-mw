#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/crypto.h>
#include <openssl/opensslv.h>
#include <openssl/rand.h>
#include <eid-viewer/certhelpers.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include "cache.h"
#include "dataverify.h"
#include "state.h"

#include "backend.h"

#if OPENSSL_VERSION_NUMBER < 0x10100000L || defined(LIBRESSL_VERSION_NUMBER)
#define X509_get0_extensions(ce) ((ce)->cert_info->extensions)
#define ASN1_STRING_get0_data ASN1_STRING_data
#define EVP_MD_CTX_new EVP_MD_CTX_create
#define EVP_MD_CTX_free EVP_MD_CTX_destroy
#endif

pthread_once_t once_rand_init = PTHREAD_ONCE_INIT;

void eid_vwr_init_crypto() {
	ERR_load_crypto_strings();
	be_log(EID_VWR_LOG_DETAIL, "Built with %s", OPENSSL_VERSION_TEXT);
	be_log(EID_VWR_LOG_DETAIL, "Using %s", SSLeay_version(SSLEAY_VERSION));
	OpenSSL_add_all_algorithms();
}

/* Return a string representation of the X509v3 uses of the given certificate. */
char* eid_vwr_get_use_flags(const char* label EIDV_UNUSED, X509* cert) {
	const STACK_OF(X509_EXTENSION) *exts = X509_get0_extensions(cert);
	int i;
	char* retval = 0;
	int nid = OBJ_sn2nid("keyUsage");

	/* Search for the object with the NID of the keyUsage field */
	for(i=0; i<sk_X509_EXTENSION_num(exts); i++) {
		X509_EXTENSION *ex = sk_X509_EXTENSION_value(exts, i);
		ASN1_OBJECT* obj = X509_EXTENSION_get_object(ex);

		if(OBJ_obj2nid(obj) == nid) {
			/* Found it, now get the string representation */
			size_t size;
			BIO *bio = BIO_new(BIO_s_mem());

			X509V3_EXT_print(bio, ex, X509V3_EXT_DEFAULT, 0);
			retval = malloc((size = BIO_ctrl_pending(bio)) + 1);
			BIO_read(bio, retval, (int)size);
			retval[size] = '\0';
			BIO_free(bio);
			return retval;
		}
	}

	return NULL;
}

/* Return a detailed description of the X509 certificate (a multiline string of
 * all the subject name fields) */
char* eid_vwr_detail_cert(const char* label EIDV_UNUSED, X509* cert) {
	X509_NAME* subject = X509_get_subject_name(cert);
	X509_NAME_ENTRY* entry;
	int i;
	char* retval = NULL;
	size_t len = 1; // end-of-string marker
	bool first = true;

	for(i=0;i<X509_NAME_entry_count(subject);i++) {
		const char* name;
		const unsigned char* value;
		char* tmp = retval;
		entry = X509_NAME_get_entry(subject, i);
		ASN1_OBJECT* obj = X509_NAME_ENTRY_get_object(entry); 
		ASN1_STRING* str = X509_NAME_ENTRY_get_data(entry);

		name = OBJ_nid2sn(OBJ_obj2nid(obj));
		value = ASN1_STRING_get0_data(str);
		if(!first) {
			len++; // newline
			tmp = strdup(retval);
		}
		len += strlen(name) + strlen((const char*)value) + 1;
		retval = realloc(retval, len);
		if(first) {
			snprintf(retval, len, "%s=%s", name, value);
		} else {
			snprintf(retval, len, "%s=%s\n%s", name, value, tmp);
			free(tmp);
		}
		first = false;
	}
	return retval;
}

/* Return a short description of the X509 certificate (i.e., the certificate's
 * common name) */
char* eid_vwr_describe_cert(const char* label, X509* cert) {
	X509_NAME* subject = X509_get_subject_name(cert);
	int index = X509_NAME_get_index_by_NID(subject, OBJ_sn2nid("CN"), -1);
	if(index < 0) {
		return strdup(label);
	}
	X509_NAME_ENTRY* entry = X509_NAME_get_entry(subject, index);
	const unsigned char* value = ASN1_STRING_get0_data(X509_NAME_ENTRY_get_data(entry));

	return strdup((char*)value);
}

bool verify_once(EVP_PKEY *pubkey, const EVP_MD *md, const unsigned char *data, size_t datalen, const unsigned char *sig, size_t siglen) {
	EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
	EVP_PKEY_CTX *pctx;
	int key_base_id = EVP_PKEY_base_id(pubkey);
	bool rv = false;

	if(key_base_id != EVP_PKEY_RSA && key_base_id != EVP_PKEY_EC) {
		be_log(EID_VWR_LOG_COARSE, "Could not verify card validity: wrong key type (expecting RSA or EC, got %d)", key_base_id);
		goto exit;
	}
	if(key_base_id == EVP_PKEY_EC) {
		md = EVP_get_digestbyname("sha384");
		//check if the signature length is correct
		size_t asnsiglen = (*(sig+1))+2; //length value of asn.1 data is in second byte, add 2 for the 2 initial bytes: 0x30 and len
		if (*sig != 0x30)
		{
			be_log(EID_VWR_LOG_COARSE, "Signature not in asn.1 encoding");
		}
		else if( asnsiglen != siglen)
		{
			be_log(EID_VWR_LOG_COARSE, "signature length mismatch between asn.1 encoding and filelength, will use the value in asn.1");
			siglen = asnsiglen;
		}
	}
	if(EVP_DigestVerifyInit(mdctx, &pctx, md, NULL, pubkey) != 1) {
		be_log(EID_VWR_LOG_COARSE, "Could not verify card validity: failed to initialize verification context");
		goto exit;
	}
	if(EVP_DigestVerifyUpdate(mdctx, data, datalen) != 1) {
		be_log(EID_VWR_LOG_COARSE, "Could not verify card validity: hashing failed");
		goto exit;
	}
	if(EVP_DigestVerifyFinal(mdctx, sig, siglen) != 1) {
		be_log(EID_VWR_LOG_COARSE, "Signature validity check failed");
		goto exit;
	}
	rv = true;
exit:
	EVP_MD_CTX_free(mdctx);
	return rv;
}

/* Test if the card data signatures (identity signature, address signature) are
 * valid for the given rrn certificate*/
int eid_vwr_check_data_validity(const void* photo, int plen,
		const void* photohash, int hashlen,
		const void* datafile, int datfilelen,
		const void* datasig, int datsiglen,
		const void* addrfile, int addfilelen,
		const void* addrsig, int addsiglen,
		const void* cert, int certlen) {
	BIO *bio;
	X509 *rrncert;
	EVP_PKEY *pubkey;
	const EVP_MD *md;
	unsigned char*(*hash)(const unsigned char*, size_t, unsigned char*);
	unsigned char digest[SHA384_DIGEST_LENGTH];
	unsigned char *address_data, *ptr;

	assert(photo != NULL && plen != 0 && photohash != NULL
			&& (hashlen == SHA_DIGEST_LENGTH || hashlen == SHA256_DIGEST_LENGTH || hashlen == SHA384_DIGEST_LENGTH)
			&& datafile != NULL && datfilelen != 0 && datasig != NULL && datsiglen != 0
			&& addrfile != NULL && addfilelen != 0 && addrsig != NULL && addsiglen != 0
			&& cert != NULL && certlen != 0);

	bio = BIO_new_mem_buf((char*)cert, certlen);
	rrncert = d2i_X509_bio(bio, NULL);
	if(rrncert == NULL) {
		be_log(EID_VWR_LOG_COARSE, "Could not verify data validity: RRN certificate could not be parsed");
		return 0;
	}

	switch(hashlen) {
		case SHA_DIGEST_LENGTH:
			md = EVP_get_digestbyname("sha1");
			hash = SHA1;
			break;
		case SHA256_DIGEST_LENGTH:
			md = EVP_get_digestbyname("sha256");
			hash = SHA256;
			break;
		case SHA384_DIGEST_LENGTH:
			md = EVP_get_digestbyname("sha384");
			hash = SHA384;
			break;
		default:
			be_log(EID_VWR_LOG_COARSE, "Could not verify data validity: unknown hash type");
			return 0;
	}

	/* compute photo hash and compare against passed hash */
	hash(photo, plen, digest);
	if(memcmp(digest, photohash, hashlen)) {
		be_log(EID_VWR_LOG_COARSE, "Could not verify data validity: photo hash invalid");
		return 0;
	}
	pubkey = X509_get_pubkey(rrncert);
	if(!verify_once(pubkey, md, datafile, datfilelen, datasig, datsiglen)) {
		/* Some CA4 cards are re-signed CA3 ones where the photo hash
		 * is still SHA1, but everything else is SHA256. Try if this is
		 * such a card. */
		if(hashlen == SHA_DIGEST_LENGTH)
		{
			md = EVP_get_digestbyname("sha256");
			if(!verify_once(pubkey, md, datafile, datfilelen, datasig, datsiglen)) {
				be_log(EID_VWR_LOG_COARSE, "Data signature fails validation!");
				return 0;
			}
		}
	}
	address_data = calloc(addfilelen + datsiglen, 1);
	memcpy(address_data, addrfile, addfilelen);
	/* The documentation on the address file claims that the
	 * signature of the identity file is simply concatenated to the
	 * address file, at the given length of the address file with
	 * terminating NULL bytes retained, and that a hash is taken
	 * from the result. However, the documentation is wrong.
	 *
	 * In reality, the address data is stripped of any terminating
	 * NULL bytes, regardless of specified length of the address
	 * file, and the data signature is concatenated to the result */
	for(ptr = address_data + addfilelen; *ptr == 0; ptr--);
	ptr++;
	memcpy(ptr, datasig, datsiglen);
	if(!verify_once(pubkey, md, address_data, (ptr - address_data) + datsiglen, addrsig, addsiglen)) {
		be_log(EID_VWR_LOG_COARSE, "Could not verify data validity: address signature invalid!");
		free(address_data);
		return 0;
	}
	free(address_data);

	return 1;
}

/* Write the given certificate (in DER format) to the passed file
 * descriptor. */
void eid_vwr_dumpcert(int fd, const void* derdata, int len, enum dump_type how) {
	BIO *bio;
	X509 *cert = NULL;

	switch(how) {
		case DUMP_DER:
			write(fd, derdata, len);
			break;
		case DUMP_PEM:
			/* Clear errors */
			while(ERR_get_error() != 0) {};
			bio = BIO_new_fd(fd, 0);
			if(d2i_X509(&cert, (const unsigned char**)&derdata, len) == NULL) {
				char buf[100];
				unsigned long error = ERR_get_error();
				ERR_error_string_n(error, buf, sizeof(buf));
				buf[99]='\0';
				be_log(EID_VWR_LOG_ERROR, "Could not parse certificate");
				be_log(EID_VWR_LOG_DETAIL, "libssl error: %s", buf);
				return;
			}
			PEM_write_bio_X509(bio, cert);
			BIO_free(bio);
			break;
	}
}

static unsigned char random_buffer[SHA384_DIGEST_LENGTH];

void eid_vwr_challenge_result(const unsigned char *response, int responselen, enum eid_vwr_result res) {
	if(res != EID_VWR_RES_SUCCESS) {
		be_log(EID_VWR_LOG_DETAIL, "Basic key challenge failed, marking data as invalid");
		sm_handle_event(EVENT_DATA_INVALID, NULL, NULL, NULL);
		return;
	}
	const struct eid_vwr_cache_item *basic_key = cache_get_data("BASIC_KEY_FILE");
	eid_vwr_check_signature(basic_key->data, basic_key->len, response, responselen, random_buffer, sizeof random_buffer);
}

static void init_random(void) {
	if(RAND_load_file("/dev/random", 32) != 32) {
		be_log(EID_VWR_LOG_ERROR, "Could not initialize randomizer; possibly unsafe random in use");
	}
}

#define ossl_check(f,m) if((f) != 1) {be_log(EID_VWR_LOG_ERROR, m); goto error; }
static void do_challenge(void) {
	// Initialize OpenSSL's randomizer with secure random, if not already done
	pthread_once(&once_rand_init, init_random);

	// Make sure that the basic key file and basic key hash match
	const struct eid_vwr_cache_item *basic_key = cache_get_data("BASIC_KEY_FILE");
	const struct eid_vwr_cache_item *key_hash = cache_get_data("basic_key_hash");
	EVP_MD_CTX *key_ctx = EVP_MD_CTX_new();
	unsigned char digest[SHA384_DIGEST_LENGTH];
	unsigned int size = 0;

	if(key_hash->len != SHA384_DIGEST_LENGTH) {
		be_log(EID_VWR_LOG_DETAIL, "Could not compare basic key hash: unexpected hash length");
		goto end;
	}

	// Verify that basic key matches its hash
	ossl_check(EVP_DigestInit(key_ctx, EVP_sha384()), "Could not compare basic key hash: could not initialize hash");
	ossl_check(EVP_DigestUpdate(key_ctx, basic_key->data, basic_key->len), "Could not compare basic key hash: could not hash key");
	ossl_check(EVP_DigestFinal_ex(key_ctx, digest, &size), "Could not compare basic key hash: could not retrieve hash");
	if(size != key_hash->len) {
		be_log(EID_VWR_LOG_ERROR, "Could not compare basic key hash: hash length does not match");
		goto error;
	}
	if(memcmp(key_hash->data, digest, size)) {
		be_log(EID_VWR_LOG_ERROR, "Basic key does not match basic key fingerprint. Is this a forged ID card?");
		goto error;
	}

	// Generate a nonce, and have the card sign the hash of that nonce
	ossl_check(RAND_bytes(random_buffer, sizeof random_buffer), "Could not perform basic key challenge: could not retrieve random data");
	eid_vwr_challenge(random_buffer, sizeof random_buffer);

	goto end;
error:
	sm_handle_event(EVENT_DATA_INVALID, NULL, NULL, NULL);
end:
	EVP_MD_CTX_free(key_ctx);
	return;
}
#undef ossl_check

void eid_vwr_maybe_perform_challenge() {
	if(cache_have_label("basic_key_hash")) {
		do_challenge();
	}
}

int eid_vwr_verify_card(void* d EIDV_UNUSED) {
	const struct eid_vwr_cache_item *photo, *phash, *data, *datsig, *address, *adsig, *cert;

#define GET(t, s) if(!cache_have_label(s)) { return 1; }; t = cache_get_data(s)
	GET(photo, "PHOTO_FILE");
	GET(phash, "photo_hash");
	GET(data, "DATA_FILE");
	GET(datsig, "SIGN_DATA_FILE");
	GET(address, "ADDRESS_FILE");
	GET(adsig, "SIGN_ADDRESS_FILE");
	GET(cert, "CERT_RN_FILE");
#undef GET
	return 1 - eid_vwr_check_data_validity(photo->data, photo->len,
			phash->data, phash->len,
			data->data, data->len,
			datsig->data, datsig->len,
			address->data, address->len,
			adsig->data, adsig->len,
			cert->data, cert->len);
}
