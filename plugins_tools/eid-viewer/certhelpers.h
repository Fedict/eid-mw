#ifndef EID_VWR_CERT_HELPERS_H
#define EID_VWR_CERT_HELPERS_H

#include <openssl/x509.h>

enum cert_columns {
	CERT_COL_LABEL,
	CERT_COL_IMAGE,
	CERT_COL_VALIDFROM,
	CERT_COL_VALIDFROM_PAST,
	CERT_COL_VALIDTO,
	CERT_COL_VALIDTO_FUTURE,
	CERT_COL_USE,
	CERT_COL_VALIDITY,
	CERT_COL_DESC,
	CERT_COL_DATA,
	CERT_COL_NCOLS
};

enum dump_type {
	DUMP_DER,
	DUMP_PEM,
};

char* get_use_flags(const char* label, X509* cert);
char* detail_cert(const char* label, X509* cert);
char* describe_cert(const char* label, X509* cert);
int check_data_validity(const void* photo, int plen,
		const void* photohash, int hashlen,
		const void* datafile, int datfilelen,
		const void* datasig, int datsiglen,
		const void* addrfile, int addfilelen,
		const void* addrsig, int addsiglen,
		const void* rrncert, int certlen);
void dumpcert(int fd, const void* derdata, int len, enum dump_type how);
void ensure_inited();
#endif
