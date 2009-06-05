// BelgianEIDConfig.h: interface for the CBelgianEIDConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BELGIANEIDCONFIG_H__BF13DA6F_2ED5_4814_8E86_1F8AEAFF21A7__INCLUDED_)
#define AFX_BELGIANEIDCONFIG_H__BF13DA6F_2ED5_4814_8E86_1F8AEAFF21A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "ScConfig.h"
#include "SlotInfo.h"
#include "KeyInfo.h"

class CBelgianEIDConfig : public CScConfig  
{
public:
	CBelgianEIDConfig();
	virtual ~CBelgianEIDConfig();
public:
    bool Read(const wxString & strFileName);
    bool Write(const wxString & strFileName);
    void AddSlotInfo(CSlotInfo *pSlotInfo);
    DWORD GetNrOfSlots () {return m_tpSlotInfo.size();}
    void SetCardID (BYTE *pbByte, DWORD dwIDLength);
    BYTE *GetCardID(DWORD *pdwLen);
    std::vector <CSlotInfo *> GetSlotInfo() { return m_tpSlotInfo; }
    void ClearSlots();

private:
   BYTE *m_pbCardID;
   DWORD m_dwCardIDLen;
   std::vector <CSlotInfo *> m_tpSlotInfo;
};

#endif // !defined(AFX_BELGIANEIDCONFIG_H__BF13DA6F_2ED5_4814_8E86_1F8AEAFF21A7__INCLUDED_)
