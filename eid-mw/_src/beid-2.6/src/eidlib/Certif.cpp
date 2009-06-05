// Certif.cpp: implementation of the CCertif class.
//
//////////////////////////////////////////////////////////////////////

#include "Certif.h"
#include "eiddefines.h"
#include <string.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace eidlib
{
CCertif::CCertif(unsigned char *pData, unsigned long ulLen, const char *pucID)
{
    m_oByteCertif.Append(pData, ulLen);
    memcpy(m_ucID, pucID, sizeof(m_ucID));
    m_lCertStatus = BEID_CERTSTATUS_CERT_NOT_VALIDATED;
}

CCertif::~CCertif()
{

}
}
