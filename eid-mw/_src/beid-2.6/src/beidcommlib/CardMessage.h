// CardMessage.h: interface for the CCardMessage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CARDMESSAGE_H__68CF9E35_6312_4234_8F52_EAA42B55FEB0__INCLUDED_)
#define AFX_CARDMESSAGE_H__68CF9E35_6312_4234_8F52_EAA42B55FEB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "config.h"

NAMESPACE_BEGIN(EIDCommLIB)

class CCardMessage  
{
public:
	CCardMessage();
	virtual ~CCardMessage();
    void Clear();
    void Set(const std::string & strKey, const std::string & strValue);
    void Set(const std::string & strKey, long lValue);
    void Set(const std::string & strKey, const byte *pValue, unsigned int iSize);
    bool Get(const std::string & strKey, std::string & strValue);
    bool Get(const std::string & strKey, char *pszValue);
    bool Get(const std::string & strKey, long & lValue);
    bool Get(const std::string & strKey, long *plValue);
    unsigned int Get(const std::string & strKey, byte *pValue, unsigned int iSize);
    std::string GetMessageId();
    
    unsigned int Serialize(byte *pValue, unsigned int iSize);
    bool Unserialize(const byte *pValue, unsigned int iSize);
    unsigned int GetSerializeSize();

private:
    typedef std::map<std::string, QByteArray *> VecKeyValues;
    typedef VecKeyValues::iterator ItVecKeyValues;
    VecKeyValues m_oVecKeyValues;
    QUuid m_oId;
};

NAMESPACE_END

#endif // !defined(AFX_CARDMESSAGE_H__68CF9E35_6312_4234_8F52_EAA42B55FEB0__INCLUDED_)
