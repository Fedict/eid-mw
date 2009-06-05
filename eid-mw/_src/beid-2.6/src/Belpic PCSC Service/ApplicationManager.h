// ApplicationManager.h: interface for the CApplicationManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPLICATIONMANAGER_H__04CB2474_0C4D_4CCD_A2ED_8667E375889B__INCLUDED_)
#define AFX_APPLICATIONMANAGER_H__04CB2474_0C4D_4CCD_A2ED_8667E375889B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CApplicationObject;
class CReaderObject;
class EIDCommLIB::CCardMessage;

class CApplicationManager
{
public:
	CApplicationManager();
	virtual ~CApplicationManager();

    CApplicationObject *CreateApplication(EIDCommLIB::CCardMessage *pMessage, long lConnID);
    CApplicationObject *FindExisting(long lConnID);
    CApplicationObject *FindApplication(SCARDCONTEXT hContext);
    void DeleteApplication(SCARDCONTEXT hContext);
    void DeleteApplicationReaders(CReaderObject *pReader, std::vector<long> & oDummyHandles);
protected:
	QMutex m_lock;
    typedef std::map<long, CApplicationObject *> MapApps;
    typedef MapApps::iterator ItMapApps;
    MapApps m_Applications;
};

#endif // !defined(AFX_APPLICATIONMANAGER_H__04CB2474_0C4D_4CCD_A2ED_8667E375889B__INCLUDED_)
