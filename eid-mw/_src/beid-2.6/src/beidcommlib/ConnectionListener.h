// ConnectionListener.h: interface for the CConnectionListener class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONNECTIONLISTENER_H__2C29CA4B_CA9F_41C5_89CF_4ED33B39A739__INCLUDED_)
#define AFX_CONNECTIONLISTENER_H__2C29CA4B_CA9F_41C5_89CF_4ED33B39A739__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "config.h"
#include "QSocketDeviceImpl.h"

NAMESPACE_BEGIN(EIDCommLIB)

class CConnectionListener;
class CConnectionManager;

class QListenThread : public QThread
{
    public:
	    QListenThread();
        void SetSocket(QSocketDeviceImpl *pSocket) { m_pSocket = pSocket; }
        void SetConnectionManager(CConnectionManager *pMgr) { m_pConnectionMgr = pMgr; }
        void Stop() { m_bRun = false; }
        virtual void run();
    private:
        QSocketDeviceImpl *m_pSocket;
        bool m_bRun;
        CConnectionManager *m_pConnectionMgr;
};

class CConnectionListener  
{
public:
	CConnectionListener();
	virtual ~CConnectionListener();
    void SetConnectionManager(CConnectionManager *pMgr) { m_pConnectionMgr = pMgr; }
    void StartListen(int iPort);
    void StopListen();

private:
    QListenThread m_ThreadListen;
    QSocketDeviceImpl m_oSocket;
    CConnectionManager *m_pConnectionMgr;
};

NAMESPACE_END

#endif // !defined(AFX_CONNECTIONLISTENER_H__2C29CA4B_CA9F_41C5_89CF_4ED33B39A739__INCLUDED_)
