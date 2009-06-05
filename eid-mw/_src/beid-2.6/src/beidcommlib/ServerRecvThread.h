// ServerRecvThread.h: interface for the CServerRecvThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVERRECVTHREAD_H__B5204A61_95EC_4280_9F26_6CD4D5B1E06C__INCLUDED_)
#define AFX_SERVERRECVTHREAD_H__B5204A61_95EC_4280_9F26_6CD4D5B1E06C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "config.h"

NAMESPACE_BEGIN(EIDCommLIB)

class CConnection;

class CServerRecvThread : public QThread  
{
public:
	CServerRecvThread();
	virtual ~CServerRecvThread();
    void SetConnection(CConnection *pConn) { m_pConnection = pConn; }
    void Stop() { m_bRun = false; }
    virtual void run();

private:
    bool m_bRun;
    CConnection *m_pConnection;
};

NAMESPACE_END

#endif // !defined(AFX_SERVERRECVTHREAD_H__B5204A61_95EC_4280_9F26_6CD4D5B1E06C__INCLUDED_)
