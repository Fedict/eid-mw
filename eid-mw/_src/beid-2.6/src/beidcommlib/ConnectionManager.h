// ConnectionManager.h: interface for the CConnectionManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONNECTIONMANAGER_H__B6CE8A1A_DECE_44D1_B403_9FC4143F92AC__INCLUDED_)
#define AFX_CONNECTIONMANAGER_H__B6CE8A1A_DECE_44D1_B403_9FC4143F92AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "config.h"
#include "Connection.h"
#include "ConnectionListener.h"

NAMESPACE_BEGIN(EIDCommLIB)

class QConnectionThread : public QThread
{
    public:
	    QConnectionThread();
        void SetConnectionManager(CConnectionManager *pMgr) { m_pConnectionMgr = pMgr; }
        void Stop() 
        { 
            m_bRun = false;
            m_oWait.wakeOne();
        }
        virtual void run();
    private:
        bool m_bRun;
        CConnectionManager *m_pConnectionMgr;
        QWaitCondition m_oWait;
};

class CConnectionManager  
{
public:
	CConnectionManager();
	virtual ~CConnectionManager();
    CConnection *CreateConnection();
    CConnection *GetConnection(int iSocket);
    void StartServer(int iPort);
    void StopServer();    
    void CleanUp();
    void SetMessageHandler(CMessageHandler *pHandler) { m_pHandler = pHandler; }
    void SendAlive();

private:
    typedef std::vector<CConnection *> VecConnections;
    typedef VecConnections::iterator ItVecConn;
    VecConnections m_oConnections; 
    CConnectionListener m_oListener;
    QConnectionThread m_oCleaner;
    QMutex m_oLock;
    CMessageHandler *m_pHandler;
};

NAMESPACE_END

#endif // !defined(AFX_CONNECTIONMANAGER_H__B6CE8A1A_DECE_44D1_B403_9FC4143F92AC__INCLUDED_)
