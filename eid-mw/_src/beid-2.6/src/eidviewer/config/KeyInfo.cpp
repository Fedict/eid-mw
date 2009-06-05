// KeyInfo.cpp: implementation of the CKeyInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "KeyInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKeyInfo::CKeyInfo()
{
   m_strContainerName = 0;
   m_dwKeyLength = 0;
   m_dwKeyUsage = 0;
   m_dwKeyID = 0;
   m_dwNameLength = 0;
}

CKeyInfo::~CKeyInfo()
{
   if (0 != m_strContainerName)
   {
      delete m_strContainerName;
      m_strContainerName = 0;
      m_dwNameLength = 0;
   }
}

unsigned char *CKeyInfo::GetKeyContainerName (unsigned long *pdwLen)
{
     unsigned char *pCont = 0;
     *pdwLen = 0;
     if (0 != m_strContainerName)
     {
        pCont = new unsigned char[m_dwNameLength];
        memset (pCont, 0, m_dwNameLength);
        memcpy (pCont, m_strContainerName, m_dwNameLength);
        *pdwLen = m_dwNameLength;
     }
     return pCont;
}

wxString CKeyInfo::GetKeyContainerName()
{
    if (0 != m_strContainerName)
    {
        wxString strRet(wxString((char *)m_strContainerName, *wxConvCurrent), m_dwNameLength);
        return strRet;
    }
    return wxEmptyString;
}


void CKeyInfo::SetKeyContainerName (unsigned char *strContainerName, unsigned long dwContainerNameLg)
{
   if (0 != m_strContainerName)
   {
      delete m_strContainerName;
      m_strContainerName = 0;
      m_dwNameLength = 0;
   }
   m_dwNameLength = dwContainerNameLg + 1;
   m_strContainerName = new unsigned char [m_dwNameLength];          // Include the terminating null character
   memset (m_strContainerName, 0, m_dwNameLength);         // Set the buffer to null
   memcpy (m_strContainerName, strContainerName, dwContainerNameLg);  // Copy the Key ID
}
