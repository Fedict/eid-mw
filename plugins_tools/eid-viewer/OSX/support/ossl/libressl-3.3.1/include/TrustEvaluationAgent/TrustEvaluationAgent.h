#ifndef APPLE_TEA_H
#define APPLE_TEA_H

#include <sys/time.h>

#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/asn1.h>
#include <openssl/asn1t.h>

typedef int TEAResult;

#ifndef USE_X509
typedef void *TEACertificateChainRef;
typedef void *TEACertificateRef;

typedef struct TEAParams
{
    void *purpose;

    time_t check_time;
    time_t time;
} TEAParams;
#else /* USE_X509 == 1 */
typedef X509_STORE_CTX * TEACertificateChainRef;
typedef X509 * TEACertificateRef;
typedef X509_VERIFY_PARAM TEAParams;
#endif /* USE_X509 */

TEACertificateChainRef TEACertificateChainCreate();
TEACertificateChainRef TEAVerifyCert(TEACertificateChainRef cert, TEAParams *params);

const unsigned char *TEACertificateGetData(TEACertificateRef cert);

int TEACertificateChainGetCerts(TEACertificateChainRef, int (^)(const TEACertificateRef cert));
int TEACertificateGetSize(TEACertificateRef cert);
int TEACertificateChainAddCert(TEACertificateChainRef cert, unsigned char *data, int len);
int TEACertificateChainRelease(TEACertificateChainRef cert);

#ifndef kTEAResultErrorOccured
#define kTEAResultErrorOccured -1
#endif

#ifndef kTEAResultCertNotTrusted
#define kTEAResultCertNotTrusted 0
#endif

#ifndef kTEAResultCertTrusted
#define kTEAResultCertTrusted 1
#endif

#if defined(DEBUG) || defined(_DEBUG)
#ifndef TEALogInfo
#define TEALogInfo(x...) fprintf(stderr, "INFO: " x)
#endif

#ifndef TEALogDebug
#define TEALogDebug(x...) fprintf(stderr, "DEBUG: " x)
#endif
#else
#ifndef TEALogInfo
#define TEALogInfo(x, ...)
#endif

#ifndef TEALogDebug
#define TEALogDebug(x, ...)
#endif
#endif

#endif
