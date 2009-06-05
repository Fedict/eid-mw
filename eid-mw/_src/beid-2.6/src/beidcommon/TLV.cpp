// TLV.cpp: implementation of the CTLV class.
//
//////////////////////////////////////////////////////////////////////

#include "TLV.h"

namespace eidcommon
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTLV::CTLV()
{
    m_ucTag = 0x00;
}

CTLV::CTLV(const CTLV & oTlv)
{
    m_ucTag = oTlv.m_ucTag;
    m_Data.Append(oTlv.m_Data); 
}

CTLV::CTLV(unsigned char ucTag, unsigned char *pucData, unsigned long ulLen)
{
    m_ucTag = ucTag;
    m_Data.Append(pucData, ulLen); 
}

CTLV::~CTLV()
{

}

}  // namespace eidcommon
