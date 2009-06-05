// ReaderObject.h: interface for the CReaderObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_READEROBJECT_H__1A1965AD_3827_47AB_ADCA_1CF84A266F16__INCLUDED_)
#define AFX_READEROBJECT_H__1A1965AD_3827_47AB_ADCA_1CF84A266F16__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CReaderObject
{
public:
	CReaderObject();
	virtual ~CReaderObject();

    void SetCardHandle(SCARDHANDLE hCard) { m_hCard = hCard;}
    SCARDHANDLE GetCardHandle() { return m_hCard;}
    void SetName(char *pszName);
    const char * GetName();
    void SetProtocol(unsigned long ulProtocol) { m_ulProtocol = ulProtocol;}
    unsigned long GetProtocol() { return m_ulProtocol;}
    void AddRef();
    long ReleaseRef();
    void ClearRefCount() { m_lRefCount = 0; }
    long GetRefCount() { return m_lRefCount; }

private:
    SCARDHANDLE m_hCard;
    std::string m_strName;
    unsigned long m_ulProtocol;
    long m_lRefCount;
};

#endif // !defined(AFX_READEROBJECT_H__1A1965AD_3827_47AB_ADCA_1CF84A266F16__INCLUDED_)
