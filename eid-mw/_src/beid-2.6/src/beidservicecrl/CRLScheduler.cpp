// CRLScheduler.cpp: implementation of the CCRLScheduler class.
//
//////////////////////////////////////////////////////////////////////

#include "CRLScheduler.h"
#include "../beidcommon/bytearray.h"
#include "../beidcommon/config.h"
#include <qdir.h>
#include <qstringlist.h>

#include <openssl/opensslv.h> // SK
#include <sys/stat.h>

#ifndef _WIN32
#define _fstat fstat
#define _stat stat
#define _fileno fileno
#endif

// SK
#if OPENSSL_VERSION_NUMBER >= 0x00908000L // SK
#define OPENSSL_CAST(a) const_cast<const unsigned char **>((a))
#else
#define OPENSSL_CAST(a) static_cast<unsigned char **>((a))
#endif

using namespace eidcommon;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCRLScheduler::CCRLScheduler() : m_bRunning(false)
{
    CConfig oConfig;
    oConfig.Load();
    m_strHttpCRLStore = oConfig.GetHttpCRLStorePath();
    std::string szTemp = oConfig.GetCRLStorePath();
    m_strCRLStore = QDir::convertSeparators(szTemp.c_str()).ascii();
}

CCRLScheduler::~CCRLScheduler()
{

}

void CCRLScheduler::run ()
{
    m_bRunning = true;
    bool bWait = false;
    while(!bWait)
    {
        bWait = m_oWait.wait(300000);
        if(!bWait)
        {
            BuildCRLList();
            CheckCRLList();
        }
    }
}

void CCRLScheduler::BuildCRLList()
{
    m_oVecCRLList.clear();
    QDir dir(m_strCRLStore.c_str());
    QStringList lst = dir.entryList( "*.crl" );
    for ( QStringList::Iterator it = lst.begin(); it != lst.end(); ++it ) 
    {
        m_oVecCRLList.push_back(dir.filePath(*it).ascii());
    }
}

void CCRLScheduler::CheckCRLList()
{
    for(unsigned int i = 0; i < m_oVecCRLList.size(); ++i)
    {
        if(m_bRunning)
        {
            FindLocalCRL(m_oVecCRLList[i]);
        }
    }
}

void CCRLScheduler::FindLocalCRL(const std::string & strLocalCRL)
{
    X509_CRL *pX509CRL = NULL;
    unsigned char *pBuffer = NULL;
    unsigned char *pTempBuffer = NULL;
    bool bDownload = false;
    const char *pszCRLStorePath = m_strCRLStore.c_str();
    const char *pszHttpCRLStore = m_strHttpCRLStore.c_str();
    const char *pCRLDistribution = strLocalCRL.c_str();

    if(pCRLDistribution != NULL && pszCRLStorePath != NULL)
    {
        char *pTemp; 
        const char *pFile = pCRLDistribution;
        while(NULL != (pTemp = strstr(pFile, "/")))
        {
            pFile = ++pTemp;
        }

        FILE *pf = NULL;
        pf = fopen(pCRLDistribution, "rb");
        if(NULL != pf)
        {
            int numread = 0;
            struct _stat buf = {0};
            if(0 == _fstat(_fileno(pf), &buf))
            {
                pBuffer = (unsigned char *)malloc(buf.st_size);
                pTempBuffer = pBuffer;
                numread = fread(pBuffer, sizeof(unsigned char), buf.st_size, pf);
                char buffer[20] = {0};
                if(numread == buf.st_size)
                {
                    pX509CRL = d2i_X509_CRL(&pX509CRL, OPENSSL_CAST(&pBuffer), numread);   
                    if(pX509CRL != NULL)
                    {
                        // Check period
                        int iCheck = 0;
	                    iCheck = X509_cmp_time(X509_CRL_get_lastUpdate(pX509CRL), NULL);
	                    if (iCheck >= 0)
		                {
                            // X509_V_ERR_CRL_NOT_YET_VALID or X509_V_ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD error
                            bDownload = true;
		                }

	                    if(X509_CRL_get_nextUpdate(pX509CRL))
		                {
		                    iCheck = X509_cmp_time(X509_CRL_get_nextUpdate(pX509CRL), NULL);
		                    if (iCheck <= 0)
			                {
                                // X509_V_ERR_CRL_HAS_EXPIRED or X509_V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD error
                                bDownload = true;
			                 }
		                }
                    }
                    else
                    {
                        bDownload = true;
                    }                    
                }
            }
            fclose(pf);
        }
        else
        {
            bDownload = true;
        }
        if(pX509CRL != NULL)
        {
            X509_CRL_free(pX509CRL);
        }
        if(pTempBuffer)
        {
            free(pTempBuffer);
        }
        if(bDownload && m_bRunning)
        {
            // Try to download CRL
            std::string strUri(pszHttpCRLStore);
            strUri += pFile;
            RetrieveCRL(strUri, strLocalCRL); 
        }
    }
}

void CCRLScheduler::RetrieveCRL(const std::string & strFrom, const std::string & strTo) 
{
    m_oDownLoad.Download(strFrom, strTo);
}

void CCRLScheduler::StopRunning()
{
    m_bRunning = false;
    m_oDownLoad.StopDownload();
    m_oWait.wakeOne();
}
