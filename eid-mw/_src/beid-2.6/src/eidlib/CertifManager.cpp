// CertifManager.cpp: implementation of the CCertifManager class.
//
//////////////////////////////////////////////////////////////////////

#include "CertifManager.h"
#include "eiddefines.h"
#include "Verify.h"
#include "eiderrors.h"
#include "../beidcommon/config.h"
#include "../beidcommon/beidconst.h"

#include <opensc/pkcs15.h>

//////////////////////

class CFindCertif
{
private:
    char m_szID[256];
public:
    explicit CFindCertif(const char *pszID)
    {
        memset(m_szID, 0, sizeof(m_szID));
        if(pszID != NULL)
        {
            strcpy(m_szID, pszID);
        }
    }

    bool operator() (eidlib::CCertif *pCertif)
    {
        if(0 == strcmp(pCertif->GetID(), m_szID))
        {
            return true;
        }
        return false;
    }
};

using namespace eidcommon;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCertifManager::CCertifManager()
{
    m_pCardReader = NULL;
}

CCertifManager::~CCertifManager()
{
    Cleanup();
}

void CCertifManager::Cleanup()
{
    for(unsigned int i = 0; i < m_Certifs.size(); ++i)
    {
        delete m_Certifs[i];
    }
    m_Certifs.clear();
}

void CCertifManager::SetCardReader(CCardReader *pReader)
{
    m_pCardReader = pReader;
}

long CCertifManager::ReadRNCertif(BEID_Status *ptStatus)
{
    long lRet = SC_NO_ERROR;
    if(NULL != GetCertif(g_ucRNLabel))
    {
        // Already in cache
        return lRet;
    }

    if(m_pCardReader != NULL)
    {
        unsigned char ucCertif[BEID_MAX_CERT_LEN] = {0};
        unsigned long ulCertifLen = BEID_MAX_CERT_LEN;
        unsigned char ucFileRN[6] = {0};
        memcpy(ucFileRN, g_ucMF, 2);
        memcpy(ucFileRN + 2, g_ucDFCert, 2);
        memcpy(ucFileRN + 4, g_ucRN, 2);

        if(SC_NO_ERROR == (lRet = m_pCardReader->ReadFile(ucFileRN, sizeof(ucFileRN), ucCertif, &ulCertifLen, ptStatus)))
        {
            AddCertif(ucCertif, ulCertifLen, (char *)g_ucRNLabel);
        }
    }
    return lRet;
}

long CCertifManager::ReadCertifsP15(BEID_Status *ptStatus)
{
    long lRet = SC_NO_ERROR;
    if(m_pCardReader != NULL)
    {
        struct sc_pkcs15_card *p15Card = (sc_pkcs15_card *)m_pCardReader->GetP15Card();
        if(p15Card != NULL)
        {
            if(SC_NO_ERROR == (lRet = m_pCardReader->BeginTransaction(ptStatus)))
            {
                // List Certificates
                int iNumber = 0;
                struct sc_pkcs15_object *pObjs[32];
	            iNumber = sc_pkcs15_get_objects(p15Card, SC_PKCS15_TYPE_CERT_X509, pObjs, 32);

                for (int i = 0; i < iNumber; i++) 
                {
		            struct sc_pkcs15_cert_info *pCertInfo = (struct sc_pkcs15_cert_info *) pObjs[i]->data;
                    if(NULL == GetCertif(pObjs[i]->label))
                    {
		                struct sc_pkcs15_cert *pCert = NULL;
		                if(SC_NO_ERROR != (lRet = sc_pkcs15_read_certificate(p15Card, pCertInfo, &pCert)))
                            break;
                        AddCertif(pCert->data, pCert->data_len, pObjs[i]->label);
		                sc_pkcs15_free_certificate(pCert);
                    }
                }        
                m_pCardReader->EndTransaction(ptStatus);
            }
        }
    }
    ConvertOpenSCError(lRet, ptStatus);
    return lRet;
}

void CCertifManager::AddCertif(unsigned char *pucData, unsigned long ulDataLen, char *pszLabel)
{
    if(pucData != NULL && ulDataLen > 0)
    {
        eidlib::CCertif *pCertif = new eidlib::CCertif(pucData, ulDataLen, pszLabel);
        m_Certifs.push_back(pCertif);
    }
}


void CCertifManager::FillCertifs(BEID_Certif_Check *pData)
{
    if(pData != NULL)
    {
        int iCertNumber = m_Certifs.size();
        if(iCertNumber > BEID_MAX_CERT_NUMBER)
        {
            iCertNumber = BEID_MAX_CERT_NUMBER;
        }
        pData->certificatesLength =  iCertNumber;
        for(int i = 0; i < iCertNumber; ++i)
        {
            eidlib::CCertif *pCertif = m_Certifs[i];
            if(pCertif != NULL)
            {
                memcpy(pData->certificates[i].certif, pCertif->GetData(), pCertif->GetLength()); 
                pData->certificates[i].certifLength = pCertif->GetLength();
                pData->certificates[i].certifStatus = pCertif->GetCertStatus();
                memset(pData->certificates[i].certifLabel, 0, BEID_MAX_CERT_LABEL_LEN);
                strcpy(pData->certificates[i].certifLabel, pCertif->GetID()); 
            }
        }
    }
}

eidlib::CCertif *CCertifManager::GetCertif(const char *pszID)
{
    eidlib::CCertif *pCertif = NULL;
    if(pszID != NULL)
    {
        ItCertifs it = std::find_if(m_Certifs.begin(), m_Certifs.end(), CFindCertif(pszID));
        if(it != m_Certifs.end())
        {
            pCertif = (*it);
        }
    }    
    return pCertif;
}

long CCertifManager::VerifyCertsCRL(bool bDownload /* true */)
{
    long lRet = CVerify::VerifyCRL(m_Certifs, bDownload); 
    return lRet;
}

long CCertifManager::VerifyCertsOCSP()
{
    long lRet = CVerify::VerifyOCSP(m_Certifs); 
    return lRet;
}

void CCertifManager::UpdateCertStatus(void *pCertX509, long lStatus)
{
    CVerify::UpdateCertStatus(m_Certifs, pCertX509, lStatus);
}

bool CCertifManager::FindNotValidated()
{
    bool bRet = false;
    for(unsigned int i = 0; i < m_Certifs.size() && !bRet; ++i)
    {
        eidlib::CCertif *pCertif = m_Certifs[i];
        if(pCertif != NULL && BEID_CERTSTATUS_CERT_NOT_VALIDATED == pCertif->GetCertStatus())
        {
            // Found one
            bRet = true;
        }
    }
    return bRet;
}

