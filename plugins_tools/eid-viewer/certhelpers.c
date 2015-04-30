#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <certhelpers.h>
#include <stdbool.h>
#include <string.h>

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

