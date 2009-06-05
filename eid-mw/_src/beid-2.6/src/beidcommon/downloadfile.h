// downloadfile.h: interface for the CDownloadFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOWNLOADFILE_H__037ED7EC_86E3_48C2_87E7_ED40DF23DCDE__INCLUDED_)
#define AFX_DOWNLOADFILE_H__037ED7EC_86E3_48C2_87E7_ED40DF23DCDE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <openssl/ssl.h>
#include <string>

namespace eidcommon
{
class CByteArray;

class CDownloadFile
{
public:
	CDownloadFile();
	virtual ~CDownloadFile();
    bool Download(const std::string & strFileFrom, const std::string & strFileTo);
    bool Download(const std::string & strFileFrom, CByteArray & bytesTo);
    bool isSuccess() { return m_bSuccess; }
    void StopDownload() { m_bRunning = false; }
   
protected:
    BIO *Connect(char *pszHost, int iPort, int iSSL, SSL_CTX **ppSSLCtx);
    BIO *ConnectSSL(char *pszHost, int iPort, SSL_CTX **ppSSLCtx);
    void *RetrieveWebdata(const char *pszUri, int *piDataLen);

private:
    bool m_bSuccess;
    bool m_bRunning;
    std::string m_strProxyHost;
    std::string m_strProxyPort; 
};

} // namespace eidcommon

#endif // !defined(AFX_DOWNLOADFILE_H__037ED7EC_86E3_48C2_87E7_ED40DF23DCDE__INCLUDED_)
