// Verify.h: interface for the CVerify class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VERIFY_H__39E69514_6423_4430_B322_73A80AEC9697__INCLUDED_)
#define AFX_VERIFY_H__39E69514_6423_4430_B322_73A80AEC9697__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Certif.h"
#include "CertifManager.h"
#include <vector>
#include <openssl/ssl.h>
#include <qmutex.h>
#include "../beidcommon/downloadfile.h"

//////////////////////////////////////////////////////////////

/* Type Definitions */
typedef int (*spc_x509verifycallback_t)(int, X509_STORE_CTX *);

typedef struct 
{
  char *cafile;
  char *capath;
  char *crlfile;
  spc_x509verifycallback_t callback;
  STACK_OF(X509) *certs;
  STACK_OF(X509_CRL) *crls;
  char *use_certfile;
  STACK_OF(X509) *use_certs;
  char *use_keyfile;
  EVP_PKEY *use_key;
  int flags;
} spc_x509store_t;

typedef struct 
{
  char *url;
  X509 *cert;
  X509 *issuer;
  X509_STORE *store;
  X509 *sign_cert;
  EVP_PKEY *sign_key;
  long skew;
  long maxage;
} spc_ocsprequest_t;

typedef enum 
{
  SPC_OCSPRESULT_ERROR_INVALIDRESPONSE   = -12,
  SPC_OCSPRESULT_ERROR_CONNECTFAILURE    = -11,
  SPC_OCSPRESULT_ERROR_SIGNFAILURE       = -10,
  SPC_OCSPRESULT_ERROR_BADOCSPADDRESS    = -9,
  SPC_OCSPRESULT_ERROR_OUTOFMEMORY       = -8,
  SPC_OCSPRESULT_ERROR_UNKNOWN           = -7,
  SPC_OCSPRESULT_ERROR_UNAUTHORIZED      = -6,
  SPC_OCSPRESULT_ERROR_SIGREQUIRED       = -5,
  SPC_OCSPRESULT_ERROR_TRYLATER          = -3,
  SPC_OCSPRESULT_ERROR_INTERNALERROR     = -2,
  SPC_OCSPRESULT_ERROR_MALFORMEDREQUEST  = -1,
  SPC_OCSPRESULT_CERTIFICATE_VALID       = 0,
  SPC_OCSPRESULT_CERTIFICATE_REVOKED     = 1,
  SPC_OCSPRESULT_CERTIFICATE_UNKNOWN   = 2
} spc_ocspresult_t;

//////////////////////////////////////////////////////////////

class CVerify  
{
public:
	CVerify();
	virtual ~CVerify();
    static int VerifySignature(const unsigned char *pucData, unsigned long ulDataLen, 
                                 const unsigned char *pucSig, unsigned long ulSigLen); 
    static int VerifyHash(const unsigned char *pucData, unsigned long ulDataLen, const unsigned char *pucHash);
    static long VerifyCRL(const std::vector<eidlib::CCertif *> & Certifs, bool bCRLDownload = true);
    static long VerifyOCSP(const std::vector<eidlib::CCertif *> & Certifs);
    static void UpdateCertStatus(const std::vector<eidlib::CCertif *> & Certifs, void *pCertX509, long lStatus);
    static void SetCertifManager(CCertifManager *pCertifManager) { m_pCertifManager = pCertifManager; }
    static bool RetrieveData(const std::string & strUri, eidcommon::CByteArray & oData);
    static CCertifManager *GetCertifManager() { return m_pCertifManager; }

private:
    // Store operations
    static void InitX509Store(spc_x509store_t *spc_store); 
    static void CleanupX509store(spc_x509store_t *spc_store); 
    static X509_STORE *CreateX509store(spc_x509store_t *spc_store);
    static void X509StoreAddcert(spc_x509store_t *spc_store, X509 *cert);
    static void X509StoreAddCRL(spc_x509store_t *spc_store, X509_CRL *crl);
    static void X509StoreSetcallback(spc_x509store_t *spc_store, spc_x509verifycallback_t callback);
    static void X509StoreSetflags(spc_x509store_t *spc_store, int flags);
    static void X509StoreClearflags(spc_x509store_t *spc_store, int flags);

    // Certificate functions
    static X509 *DownloadCert(X509 *pCert);
    static X509* FindIssuer(X509 *cert, X509_STORE *store);
    static X509 *FindLocalIssuer(X509 *pCert);
    static X509 *LoadFromCertFile(const std::string & strFrom);
    static X509_CRL *LoadFromCRLFile(const std::string & strFrom);
    static const char *GetExtensionString(X509_EXTENSION *extension); 
    static char *GetExtensionValue(X509 *pCert, int iNID);
    static X509 *RetrieveCert(char *pszUri, char *pszFile);
    static int VerifyCert(X509 *cert, X509_STORE *store, std::vector<std::string> & CRLDistr, bool bCRLDownload = true);
    static int VerifyCertroot(X509 *cert, X509_STORE *store);
    static int VerifyOID(X509 *pX509);
    static int VerifyCertHostname(X509 *pCert, char *pszHostname);
    static int VerifyRRNDN(X509 *pX509);

    // OCSP Helpers
    static BIO *Connect(char *pszHost, int iPort, int iSSL, SSL_CTX **ppSSLCtx);
    static BIO *ConnectSSL(char *pszHost, int iPort, spc_x509store_t *pStore, SSL_CTX **ppSSLCtx);
    static spc_ocspresult_t VerifyViaOCSP(spc_ocsprequest_t *data, spc_x509store_t *pStore);

    // CRL functions
    static X509_CRL *RetrieveCRL(char *pszUri, char *pszFile); 
    static X509_CRL *FindLocalCRL(char *pCRLDistribution, bool bCRLDownload = true);

    // Callback functions
    static int VerifyCallback(int ok, X509_STORE_CTX *store);

    // Config file
    static void ReadConfig();

// Members
private:
    static CCertifManager *m_pCertifManager;
    static QMutex m_cs;
    static eidcommon::CDownloadFile m_oDownLoad;
};

#endif // !defined(AFX_VERIFY_H__39E69514_6423_4430_B322_73A80AEC9697__INCLUDED_)
