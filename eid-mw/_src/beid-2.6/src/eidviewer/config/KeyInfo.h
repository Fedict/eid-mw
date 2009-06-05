// KeyInfo.h: interface for the CKeyInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KEYINFO_H__C69C4551_3274_4DAD_B323_295F15AF3099__INCLUDED_)
#define AFX_KEYINFO_H__C69C4551_3274_4DAD_B323_295F15AF3099__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _WIN32
    #pragma warning(disable : 4786 4514)
#endif

#include <wx/string.h>

class CKeyInfo  
{
public:
	CKeyInfo();
	virtual ~CKeyInfo();
   
    unsigned long GetKeyLength () { return m_dwKeyLength; }
    unsigned long GetKeyUsage () { return m_dwKeyUsage; }
    unsigned long GetKeyID () { return m_dwKeyID; }
   unsigned char * GetKeyContainerName (unsigned long *pdwLen);

   void SetKeyLength (unsigned long dwKeyLength){m_dwKeyLength = dwKeyLength;}
   void SetKeyUsage (unsigned long dwKeyUsage){m_dwKeyUsage = dwKeyUsage;}
   void SetKeyID (unsigned long dwKeyID) {m_dwKeyID = dwKeyID;}
   void SetKeyContainerName (unsigned char *pbKeyID, unsigned long dwKeyIDLg);
   wxString GetKeyContainerName();

private:
   unsigned char*m_strContainerName;
   unsigned long m_dwNameLength;
   unsigned long m_dwKeyLength;
   unsigned long m_dwKeyUsage;
   unsigned long m_dwKeyID;
};

#endif // !defined(AFX_KEYINFO_H__C69C4551_3274_4DAD_B323_295F15AF3099__INCLUDED_)
