// TLV.h: interface for the CTLV class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TLV_H__32C8BE0E_67DE_46C9_92F5_DDBE4B2B7372__INCLUDED_)
#define AFX_TLV_H__32C8BE0E_67DE_46C9_92F5_DDBE4B2B7372__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bytearray.h"

namespace eidcommon
{

class CTLV  
{
public:
	CTLV();
	CTLV(unsigned char ucTag, unsigned char *pucData, unsigned long ulLen);
	CTLV(const CTLV & oTlv);

	virtual ~CTLV();

    void SetTag(unsigned char ucTag) { m_ucTag = ucTag; }
    void SetData(unsigned char *pucData, unsigned long ulLen) { m_Data.Append(pucData, ulLen); }
    unsigned char GetTag() { return m_ucTag; }
    unsigned char *GetData() { return m_Data.GetData(); }
    unsigned long GetLength() { return m_Data.GetSize(); }
private:
    unsigned char m_ucTag;
    CByteArray m_Data;
};

}  // namespace eidcommon

#endif // !defined(AFX_TLV_H__32C8BE0E_67DE_46C9_92F5_DDBE4B2B7372__INCLUDED_)
