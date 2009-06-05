// downloadfile.cpp: implementation of the CDownloadFile class.
//
//////////////////////////////////////////////////////////////////////

#include "downloadfile.h"
#include "../beidcommon/bytearray.h"
#include "../beidcommon/config.h"
#include <openssl/ocsp.h>

#ifdef WIN32
    #include <winsock.h>
    #pragma warning(disable:4786)
#else
    #define strnicmp strncasecmp
    #define stricmp strcasecmp
    #include <sys/socket.h> 
#endif


namespace eidcommon
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDownloadFile::CDownloadFile() : m_bSuccess(false), m_bRunning(true)
{
    CConfig oConfig;
    oConfig.Load();
    m_strProxyHost = oConfig.GetProxyHost();
    m_strProxyPort = oConfig.GetProxyPort();  
}

CDownloadFile::~CDownloadFile()
{
}

bool CDownloadFile::Download(const std::string & strFileFrom, const std::string & strFileTo)
{
    bool bRet = false;
    int  iDataLen = 0;
    void  *pvData = 0;
    FILE *pf = 0;    
    if (NULL != (pvData = RetrieveWebdata(strFileFrom.c_str(), &iDataLen))) 
    {
        // Store on disk
        pf = fopen(strFileTo.c_str(), "wb");
        if(NULL != pf)
        {
            int iNumWritten = 0;
            iNumWritten = fwrite(pvData, sizeof(unsigned char), iDataLen, pf);
            fclose(pf);
            bRet = true;
        }
    }
    if (pvData) free(pvData);

    return bRet;
}

bool CDownloadFile::Download(const std::string & strFileFrom, CByteArray & bytesTo)
{
    bool bRet = false;
    int  iDataLen = 0;
    void  *pvData = 0;
    bytesTo.RemoveAll();
    if (NULL != (pvData = RetrieveWebdata(strFileFrom.c_str(), &iDataLen))) 
    {
        bytesTo.Append((unsigned char *)pvData, iDataLen);
        free(pvData);
        bRet = true;
    }

    return bRet;
}

BIO *CDownloadFile::Connect(char *pszHost, int iPort, int iSSL, SSL_CTX **ppSSLCtx) 
{
    BIO *pConn = NULL;
   
    if (iSSL) 
    {
        if (!(pConn = ConnectSSL(pszHost, iPort, ppSSLCtx))) goto error_exit;
        return pConn;
    }

    *ppSSLCtx = 0;
    if (!(pConn = BIO_new_connect(pszHost))) goto error_exit;
    BIO_set_conn_int_port(pConn, &iPort);
    if (BIO_do_connect(pConn) <= 0) goto error_exit;
    return pConn;
   
error_exit:
    if (pConn) BIO_free_all(pConn);
    return 0;
}

BIO *CDownloadFile::ConnectSSL(char *pszHost, int iPort, SSL_CTX **ppSSLCtx) 
{
    BIO *pConn = NULL;
    OpenSSL_add_all_algorithms();
    *ppSSLCtx = SSL_CTX_new(SSLv23_client_method());
    
    if (!(pConn = BIO_new_ssl_connect(*ppSSLCtx))) goto error_exit;
    BIO_set_conn_hostname(pConn, pszHost);
    BIO_set_conn_int_port(pConn, &iPort);
   
    if (BIO_do_connect(pConn) <= 0) goto error_exit;  
    return pConn;
   
error_exit:
  if (pConn) BIO_free_all(pConn);
  if (*ppSSLCtx) 
  {
      SSL_CTX_free(*ppSSLCtx);
      *ppSSLCtx = NULL;
  }
  return 0;
}


void *CDownloadFile::RetrieveWebdata(const char *pszUri, int *piDataLen) 
{
    int iBytes = 0;
    int iContentLength = 0;
    int iHeaderLen = 0;
    int iSd = 0;
    int iSSL = 0;
    int iSel = 0;
    BIO *pConn = 0;
    SSL *pSSL = 0;
    SSL_CTX *pSSLCtx = 0;
    char szBuffer[1024] = {0};
    char *pszHeaders = 0;
    char *pszHost = 0;
    char *pszPath = 0;
    char *pszPort = 0;
    char *pszTmp = 0;
    void  *pvData = 0;
    fd_set  rmask, wmask;
    const char *pszConfigHost = m_strProxyHost.c_str();
    const char *pszConfigPort = m_strProxyPort.c_str();
    struct timeval tTimeOut;
    tTimeOut.tv_sec = 5;
    tTimeOut.tv_usec = 0;
   
    *piDataLen = 0;

    if(pszConfigHost != NULL && strlen(pszConfigHost) > 0)
    {
        pszHost = (char *)OPENSSL_malloc(strlen(pszConfigHost) + 1);
        memset(pszHost, 0, strlen(pszConfigHost) + 1);
        strcpy(pszHost, pszConfigHost); 
    }
    if(pszConfigPort != NULL && strlen(pszConfigPort) > 0)
    {
        pszPort = (char *)OPENSSL_malloc(strlen(pszConfigPort) + 1);
        memset(pszPort, 0, strlen(pszConfigPort) + 1);
        strcpy(pszPort, pszConfigPort); 
    }

     if(pszHost == NULL || pszPort == NULL)
    {
        if (!OCSP_parse_url((char *)pszUri, &pszHost, &pszPort, &pszPath, &iSSL)) goto end_error;
     }
     else
     {
        pszPath = (char *)OPENSSL_malloc(strlen(pszUri) + 1);
        memset(pszPath, 0, strlen(pszUri) + 1);
        strcpy(pszPath, pszUri); 
     }

    if (!(pConn = Connect(pszHost, atoi(pszPort), iSSL, &pSSLCtx))) goto end_error;
   
    // Send the request for the data
    BIO_printf(pConn, "GET %s HTTP/1.0\r\nConnection: close\r\n\r\n", pszPath);
   
    // Put the socket into non-blocking mode
    BIO_get_fd(pConn, &iSd);
    BIO_socket_nbio(iSd, 1);
    if (iSSL) 
    {
        BIO_get_ssl(pConn, &pSSL);
        SSL_set_mode(pSSL, SSL_MODE_ENABLE_PARTIAL_WRITE | SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);
    }
   
    // Loop reading data from the socket until we've got all of the headers
    for (;;) 
    {
        FD_ZERO(&rmask);
        FD_SET(iSd, &rmask);
        FD_ZERO(&wmask);
        if (BIO_should_write(pConn)) 
            FD_SET(iSd, &wmask);
        iSel = select(FD_SETSIZE, &rmask, &wmask, 0, &tTimeOut);
        if(iSel < 0) goto end_error;
        if(iSel ==  0) continue;
        if (FD_ISSET(iSd, &wmask)) BIO_write(pConn, szBuffer, 0);
        if (FD_ISSET(iSd, &rmask)) 
        {
            if ((iBytes = BIO_read(pConn, szBuffer, sizeof(szBuffer))) <= 0) 
            {
                if (BIO_should_retry(pConn)) continue;
                goto end_error;
            }
            if (!(pszHeaders = (char *)realloc((pszTmp = pszHeaders), iHeaderLen + iBytes))) 
            {
                pszHeaders = pszTmp;
                goto end_error;
            }
            memcpy(pszHeaders + iHeaderLen, szBuffer, iBytes);
            iHeaderLen += iBytes;
            if ((pszTmp = strstr(pszHeaders, "\r\n\r\n")) != 0) 
            {
                *(pszTmp + 2) = '\0';
                *piDataLen = iHeaderLen - ((pszTmp + 4) - pszHeaders);
                iHeaderLen -= (*piDataLen + 2);
                if (*piDataLen > 0) 
                {
                    if (!(pvData = (char *)malloc(*piDataLen))) goto end_error;
                    memcpy(pvData, pszTmp + 4, *piDataLen);
                }
                break;
            }
        }
    }
  
    /* Examine the headers to determine whether or not to continue.  If we are to
    * continue, look for a content-length header to find out how much data we're
    * going to get.  If there is no content-length header, we'll have to read
    * until the remote server closes the connection.
    */
    if (0 == strnicmp(pszHeaders, "HTTP/1.", 7)) 
    {
        if (!(pszTmp = strchr(pszHeaders, ' '))) goto end_error;
        if (strncmp(pszTmp + 1, "200 ", 4) && strncmp(pszTmp + 1, "200\r\n", 5))
            goto end_error;
        for (pszTmp = strstr(pszHeaders, "\r\n");  pszTmp;  pszTmp = strstr(pszTmp + 2, "\r\n")) 
        {
            if (strnicmp(pszTmp + 2, "content-length: ", 16)) continue;
            iContentLength = atoi(pszTmp + 18);
            break;
        }
    } 
    else 
        goto end_error;
   
    /* Continuously read and accumulate data from the remote server.  Finish when
    * we've read up to the content-length that we received.  If we didn't receive
    * a content-length, read until the remote server closes the connection.
    */
    if(iContentLength > 0)
    {
        char *pvTemp = (char *)malloc(*piDataLen);
        memcpy(pvTemp, pvData, *piDataLen);
        free(pvData);
        pvData = (char *)malloc(iContentLength);
        if(!pvData)
            goto end_error;

        memcpy(pvData, pvTemp, *piDataLen);
        free(pvTemp);
    }
    while ((!iContentLength || *piDataLen < iContentLength) && m_bRunning) 
    {
        FD_ZERO(&rmask);
        FD_SET(iSd, &rmask);
        FD_ZERO(&wmask);
        if (BIO_should_write(pConn)) FD_SET(iSd, &wmask);
        iSel = select(FD_SETSIZE, &rmask, &wmask, 0, &tTimeOut);
        if(iSel < 0) 
            goto end_error;
        if(iSel ==  0) 
            continue;
        if (FD_ISSET(iSd, &wmask)) BIO_write(pConn, szBuffer, 0);
        if (FD_ISSET(iSd, &rmask))
        if ((iBytes = BIO_read(pConn, szBuffer, sizeof(szBuffer))) <= 0) 
        {
            if (BIO_should_retry(pConn)) continue;
            break;
        }
        memcpy((char *)pvData + *piDataLen, szBuffer, iBytes);
        *piDataLen += iBytes;
    }

    if ((iContentLength && *piDataLen != iContentLength) || !m_bRunning) 
        goto end_error;
    goto end;
   
end_error:
  if (pvData) 
  { 
      free(pvData);  
      pvData = 0;  
      *piDataLen = 0; 
  }
end:
  if (pszHeaders) free(pszHeaders);
  if (pConn) BIO_free_all(pConn);
  if (pszHost) OPENSSL_free(pszHost);
  if (pszPort) OPENSSL_free(pszPort);
  if (pszPath) OPENSSL_free(pszPath);
  if (pSSLCtx) SSL_CTX_free(pSSLCtx);
  return pvData;
}

} // namespace eidcommon
