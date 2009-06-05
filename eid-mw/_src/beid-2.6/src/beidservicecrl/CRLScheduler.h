// CRLScheduler.h: interface for the CCRLScheduler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRLSCHEDULER_H__205BA3B6_3468_48C9_AC1F_BCA4BA5A0DA6__INCLUDED_)
#define AFX_CRLSCHEDULER_H__205BA3B6_3468_48C9_AC1F_BCA4BA5A0DA6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _WIN32
    #pragma warning(disable:4786)
#endif

#include <string>
#include <vector>
#include <qthread.h>
#include "../beidcommon/downloadfile.h"

class CCRLScheduler : public QThread
{
public:
	CCRLScheduler();
	virtual ~CCRLScheduler();

    // Thread execution starts here
    virtual void run();
    void StopRunning();

protected:
    void BuildCRLList();
    void CheckCRLList();
    void FindLocalCRL(const std::string & strLocalCRL);
    void RetrieveCRL(const std::string & strFrom, const std::string & strTo);

private:
    std::vector<std::string> m_oVecCRLList;
    QWaitCondition m_oWait;
    bool m_bRunning;
    std::string m_strHttpCRLStore;
    std::string m_strCRLStore;
    eidcommon::CDownloadFile m_oDownLoad;
};

#endif // !defined(AFX_CRLSCHEDULER_H__205BA3B6_3468_48C9_AC1F_BCA4BA5A0DA6__INCLUDED_)
