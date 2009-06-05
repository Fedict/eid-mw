// SlotInfo.cpp: implementation of the CSlotInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "SlotInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSlotInfo::CSlotInfo()
{
    m_pbSlotName = 0;
    m_tpKeyInfo.clear ();
    m_dwSlotLen = 0;
}

CSlotInfo::~CSlotInfo()
{
   if (0 != m_pbSlotName)
   {
      delete [] m_pbSlotName;
      m_pbSlotName = 0;
      m_dwSlotLen = 0;
   }

    CKeyInfo *pKeyInfo=0;
    for (unsigned int i = 0; i < m_tpKeyInfo.size(); ++i)
    {
        pKeyInfo = m_tpKeyInfo[i];
        if(pKeyInfo != 0)
        {
            delete (pKeyInfo);
            pKeyInfo = 0;
        }
    }
    m_tpKeyInfo.clear ();
}

void CSlotInfo::SetSlotName (unsigned char *pbName, unsigned long dwLength)
{
   if (0 != m_pbSlotName)
   {
      delete [] m_pbSlotName;
      m_pbSlotName = 0;
      m_dwSlotLen = 0;
   }
   m_dwSlotLen = dwLength + 1;
   m_pbSlotName = new unsigned char [m_dwSlotLen];         // Include the terminating 0 character
   memset (m_pbSlotName, '\0', m_dwSlotLen);        // Set the buffer to 0
   memcpy (m_pbSlotName, pbName, dwLength);  // Copy the Slot name
}

unsigned char *CSlotInfo::GetSlotName(unsigned long *pdwLen)
{
     *pdwLen = 0;
     unsigned char *pSlot = 0;
     if (0 != m_pbSlotName)
     {
        pSlot = new unsigned char[m_dwSlotLen];
        memset (pSlot, 0, m_dwSlotLen);
        memcpy (pSlot, m_pbSlotName, m_dwSlotLen);
        *pdwLen = m_dwSlotLen;
     }
     return pSlot;
}

wxString CSlotInfo::GetSlotName()
{
    if (0 != m_pbSlotName)
    {
        wxString strRet(wxString((char *)m_pbSlotName, *wxConvCurrent), m_dwSlotLen);
        return strRet;
    }
    return wxEmptyString;
}

void CSlotInfo::AddKeyInfo (CKeyInfo *pKeyInfo)
{
    bool bFound = false;
    std::vector <CKeyInfo *>::iterator it;
    wxString strNewName = pKeyInfo->GetKeyContainerName(); 
    for( it = m_tpKeyInfo.begin(); it != m_tpKeyInfo.end(); ++it )
    {
        wxString strName = (*it)->GetKeyContainerName();
        if(0 == strName.CmpNoCase(strNewName))
        {
            bFound = true;
            (*it) = pKeyInfo;
            break;
        }
    }

    if(!bFound)
    {
        m_tpKeyInfo.push_back(pKeyInfo);
    }
}

CKeyInfo *CSlotInfo::GetKeyInfo (unsigned long dwIndex) const
{
   if (dwIndex < m_tpKeyInfo.size())
      return (m_tpKeyInfo[dwIndex]);   
   else
      return (0);
}