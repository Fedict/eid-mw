#ifndef APPLE_TEA_H
#define APPLE_TEA_H

#include <sys/time.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/asn1.h>
#include <openssl/asn1t.h>

typedef int TEAResult;

/*typedef void *TEACertificateChainRef;
typedef struct TEAParams
{
    void *purpose;
    time_t check_time;
    time_t time;
} TEAParams;

typedef void *TEACertificateRef;*/

typedef X509_STORE_CTX * TEACertificateChainRef;
typedef X509 * TEACertificateRef;
typedef X509_VERIFY_PARAM TEAParams;

TEACertificateChainRef TEACertificateChainCreate();
TEACertificateChainRef TEAVerifyCert(TEACertificateChainRef cert, TEAParams *params);

const unsigned char *TEACertificateGetData(TEACertificateRef cert);

int TEACertificateChainGetCerts(TEACertificateChainRef, int (^)(const TEACertificateRef cert));
int TEACertificateGetSize(TEACertificateRef cert);
int TEACertificateChainAddCert(TEACertificateChainRef cert, unsigned char *data, int len);
int TEACertificateChainRelease(TEACertificateChainRef cert);

#define kTEAResultErrorOccured -1
#define kTEAResultCertNotTrusted 0
#define kTEAResultCertTrusted 1

#define TEALogInfo(x, ...)
#define TEALogDebug(x, ...)

#endif
