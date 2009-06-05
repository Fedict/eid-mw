// Certif.h: interface for the CCertif class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CERTIF_H__DDEFD093_FCDF_40C4_8D76_15E40460B014__INCLUDED_)
#define AFX_CERTIF_H__DDEFD093_FCDF_40C4_8D76_15E40460B014__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../beidcommon/bytearray.h"

namespace eidlib
{
class CCertif  
{
public:
	CCertif(unsigned char *pData, unsigned long ulLen, const char *pszID);
	virtual ~CCertif();

    unsigned char *GetData() { return m_oByteCertif.GetData();}
    unsigned long GetLength() { return m_oByteCertif.GetSize();}
    char *GetID() { return m_ucID;}
    void SetCertStatus(long lStatus) { m_lCertStatus = lStatus; }
    long GetCertStatus() { return m_lCertStatus; }

private:
    eidcommon::CByteArray m_oByteCertif;
    char m_ucID[256];
    long m_lCertStatus;
};
}

#endif // !defined(AFX_CERTIF_H__DDEFD093_FCDF_40C4_8D76_15E40460B014__INCLUDED_)
