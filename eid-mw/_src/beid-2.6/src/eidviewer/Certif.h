// Certif.h: interface for the CCertif class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CERTIF_H__D08099D7_FEC4_401D_80B1_4DECB917ED8F__INCLUDED_)
#define AFX_CERTIF_H__D08099D7_FEC4_401D_80B1_4DECB917ED8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "../beidcommon/bytearray.h"
#include "eidviewerTypes.h"

class CCertif  
{
public:
	CCertif(unsigned char *pData, unsigned long ulLen, const char *pszID, long lCertStatus);
	virtual ~CCertif();

    unsigned char *GetData() { return m_oByteCertif.GetData();}
    unsigned long GetLength() { return m_oByteCertif.GetSize();}
    char *GetID() { return m_ucID;}
    long GetCertStatus() { return m_lCertStatus; }
    void SetX509(void *pX509) { m_pX509 = pX509; }
    void *GetX509() { return m_pX509; }
    void SetKeyUsage(BYTE keyUsage) { m_keyUsage = keyUsage; }
    BYTE GetKeyUsage() { return m_keyUsage; }
    void SetKeyLength(long keyLength) { m_KeyLength = keyLength; }
    long GetKeyLength() { return m_KeyLength; }
    void SetKeyID(BYTE keyID) { m_keyID = keyID; }
    BYTE GetKeyID() { return m_keyID; }


private:
    eidcommon::CByteArray m_oByteCertif;
    char m_ucID[256];
    long m_lCertStatus;
    void *m_pX509;
    BYTE m_keyUsage;
    long m_KeyLength;
    BYTE m_keyID;
};

#endif // !defined(AFX_CERTIF_H__D08099D7_FEC4_401D_80B1_4DECB917ED8F__INCLUDED_)
