// ApplicationObject.h: interface for the CApplicationObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPLICATIONOBJECT_H__1A307DF6_3EA6_4692_B862_23ED4E9BA407__INCLUDED_)
#define AFX_APPLICATIONOBJECT_H__1A307DF6_3EA6_4692_B862_23ED4E9BA407__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ReaderObject.h" 

class CAppReaderObject
{
public:
    SCARDHANDLE hOriginal;
    CReaderObject *pReader;
};

class CApplicationObject  
{
public:
	CApplicationObject();
	virtual ~CApplicationObject();

    void SetContext(SCARDCONTEXT hContext);
    SCARDCONTEXT GetContext();
    void SetApplicationName(char *pszApplicationName);
    const char *GetApplicationName();
    void SetUserName(char *pszUserName);
    const char *GetUserName();
    SCARDHANDLE AddReader(CReaderObject *pReader);
    void DeleteReader(SCARDHANDLE hHandle);
    CReaderObject *GetReader(SCARDHANDLE hHandle);
    void SetConnID(long lConnID);
    long GetConnID();
    void SetPID(long lPID);
    long GetPID();
    void AddRef();
    long ReleaseRef();
    BOOL MatchOriginal(SCARDHANDLE hHandle);
    SCARDHANDLE GetOriginal(SCARDHANDLE hHandle);
    void DeleteReader(CReaderObject *pReader, std::vector<long> & oDummyHandles);
    SCARDHANDLE GetDummy(SCARDHANDLE hRealHandle);

private:
    long m_lRefCount;
    SCARDCONTEXT m_hContext;
    std::string m_strApplicationName;
    std::string m_strUserName;
    typedef std::map<long, CAppReaderObject *> MapReaders;
    typedef MapReaders::iterator ItMapReaders;
    MapReaders m_oReaders;
    long m_lConnID;
    long m_lPID;
};

#endif // !defined(AFX_APPLICATIONOBJECT_H__1A307DF6_3EA6_4692_B862_23ED4E9BA407__INCLUDED_)
