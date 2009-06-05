// Certif.cpp: implementation of the CCertif class.
//
//////////////////////////////////////////////////////////////////////

#include "Certif.h"

#include <openssl/ssl.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCertif::CCertif(unsigned char *pData, unsigned long ulLen, const char *pucID, long lCertStatus)
{
    m_oByteCertif.RemoveAll(); 
    m_oByteCertif.Append(pData, ulLen);
    memcpy(m_ucID, pucID, sizeof(m_ucID));
    m_lCertStatus = lCertStatus;
    m_pX509 = NULL;
    m_keyUsage = 0;
    m_KeyLength = 0;
    m_keyID = 0;
}

CCertif::~CCertif()
{
    if(m_pX509 != NULL)
    {
        X509_free((X509 *)m_pX509);
        m_pX509 = NULL;
    }
}
