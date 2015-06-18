#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <certhelpers.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "backend.h"

char* get_use_flags(const char* label, X509* cert) {
	X509_CINF* ci = cert->cert_info;
	int i;
	char* retval = 0;
	int nid = OBJ_sn2nid("keyUsage");

	for(i=0; i<sk_X509_EXTENSION_num(ci->extensions); i++) {
		X509_EXTENSION *ex = sk_X509_EXTENSION_value(ci->extensions, i);
		ASN1_OBJECT* obj = X509_EXTENSION_get_object(ex);

		if(OBJ_obj2nid(obj) == nid) {
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

char* detail_cert(const char* label, X509* cert) {
	X509_NAME* subject = X509_get_subject_name(cert);
	X509_NAME_ENTRY* entry;
	int i;
	char* retval = NULL;
	size_t len = 1; // end-of-string marker
	bool first = true;

	for(i=0;i<X509_NAME_entry_count(subject);i++) {
		const char* name;
		const unsigned char* value;
		char* tmp;
		entry = X509_NAME_get_entry(subject, i);
		ASN1_OBJECT* obj = X509_NAME_ENTRY_get_object(entry); 
		ASN1_STRING* str = X509_NAME_ENTRY_get_data(entry);

		name = OBJ_nid2sn(OBJ_obj2nid(obj));
		value = ASN1_STRING_data(str);
		if(!first) {
			len++; // newline
			tmp = strdup(retval);
		}
		len += strlen(name) + strlen(value) + 1;
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

char* describe_cert(const char* label, X509* cert) {
	X509_NAME* subject = X509_get_subject_name(cert);
	int index = X509_NAME_get_index_by_NID(subject, OBJ_sn2nid("CN"), -1);
	if(index < 0) {
		return strdup(label);
	}
	X509_NAME_ENTRY* entry = X509_NAME_get_entry(subject, index);
	const unsigned char* value = ASN1_STRING_data(X509_NAME_ENTRY_get_data(entry));

	return strdup((char*)value);
}

int check_data_validity(const char* photo, int plen,
		const char* photohash, int hashlen,
		const char* datafile, int datfilelen,
		const char* datasig, int datsiglen,
		const char* addrfile, int addfilelen,
		const char* addrsig, int addsiglen,
		const char* cert, int certlen) {
	BIO *bio;
	X509* rrncert;
	EVP_PKEY* pubkey;
	unsigned char digest[SHA256_DIGEST_LENGTH];
	unsigned char*(*hash)(const unsigned char*, size_t, unsigned char*);
	unsigned char *address_data, *ptr;
	int nid;

	bio = BIO_new_mem_buf((char*)cert, certlen);
	rrncert = d2i_X509_bio(bio, NULL);

	assert(photo != NULL && plen != 0
			&& photohash != NULL && (hashlen == SHA_DIGEST_LENGTH || hashlen == SHA256_DIGEST_LENGTH)
			&& datafile != NULL && datfilelen != 0 && datasig != NULL && datsiglen != 0
			&& addrfile != NULL && addfilelen != 0 && addrsig != NULL && addsiglen != 0
			&& rrncert != NULL);

	switch(hashlen) {
		case SHA_DIGEST_LENGTH:
			hash = SHA1;
			nid = NID_sha1;
			break;
		case SHA256_DIGEST_LENGTH:
			hash = SHA256;
			nid = NID_sha256;
			break;
	}

	hash(photo, plen, digest);
	if(memcmp(digest, photohash, hashlen)) {
		be_log(EID_VWR_LOG_COARSE, "Could not verify data validity: photo hash invalid");
		return 0;
	}
	pubkey = X509_get_pubkey(rrncert);
	if(EVP_PKEY_type(pubkey->type) != EVP_PKEY_RSA) {
		be_log(EID_VWR_LOG_COARSE, "Could not verify data validity: wrong key type (expecting RSA, got %d)", pubkey->type);
		return 0;
	}
	hash(datafile, datfilelen, digest);
	if(RSA_verify(nid, digest, hashlen, datasig, datsiglen, EVP_PKEY_get1_RSA(pubkey)) != 1) {
		be_log(EID_VWR_LOG_COARSE, "Could not verify data validity: data signature invalid!");
		return 0;
	}

	address_data = calloc(addfilelen + datsiglen, 1);
	memcpy(address_data, addrfile, addfilelen);
	for(ptr = address_data + addfilelen; *ptr == 0; ptr--);
	ptr++;
	memcpy(ptr, datasig, datsiglen);
	hash(address_data, (ptr - address_data) + datsiglen, digest);
	free(address_data);
	if(RSA_verify(nid, digest, hashlen, addrsig, addsiglen, EVP_PKEY_get1_RSA(pubkey)) != 1) {
		be_log(EID_VWR_LOG_COARSE, "Could not verify data validity: address signature invalid!");
		return 0;
	}

	return 1;
}

void dumpcert(int fd, const void* derdata, int len, enum dump_type how) {
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
				ERR_load_crypto_strings();
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
