// SlotInfo.h: interface for the CSlotInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SLOTINFO_H__34F625E3_8AB2_46B4_97FB_96564854E25B__INCLUDED_)
#define AFX_SLOTINFO_H__34F625E3_8AB2_46B4_97FB_96564854E25B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _WIN32
    #pragma warning(disable : 4786 4514)
#endif

#include "KeyInfo.h"

#ifdef _WIN32
    #pragma warning(disable : 4786)
#endif

#include <vector>
#include <wx/string.h>

class CSlotInfo  
{
public:
	CSlotInfo();
	virtual ~CSlotInfo();

public:
   void SetSlotName (unsigned char *pbName, unsigned long dwLength);

   unsigned long GetNrOfKeys (void) { return m_tpKeyInfo.size(); }
   unsigned char *GetSlotName (unsigned long *pdwLen);
   wxString GetSlotName();

   void AddKeyInfo (CKeyInfo *pKeyInfo);
   CKeyInfo *GetKeyInfo (unsigned long dwIndex) const;

private:
    unsigned long m_dwSlotLen;
    unsigned char *m_pbSlotName;
    std::vector <CKeyInfo *> m_tpKeyInfo;
};

#endif // !defined(AFX_SLOTINFO_H__34F625E3_8AB2_46B4_97FB_96564854E25B__INCLUDED_)
