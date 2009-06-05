// ConnectionListener.cpp: implementation of the CConnectionListener class.
//
//////////////////////////////////////////////////////////////////////

#include "ConnectionManager.h"
#include "ConnectionListener.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NAMESPACE_BEGIN(EIDCommLIB)

QListenThread::QListenThread() : m_bRun(true), m_pConnectionMgr(NULL)
{
}

void QListenThread::run()
{
    while (m_bRun)
    {
        int iSock = m_pSocket->accept();
        if( -1 != iSock)
        {
            CConnection *pConn = m_pConnectionMgr->CreateConnection();
            pConn->AttachSocket(iSock);
        }
        else
        {
            m_bRun = false;
        }
    }
}

CConnectionListener::CConnectionListener() : m_pConnectionMgr(NULL)
{

}

CConnectionListener::~CConnectionListener()
{

}

void CConnectionListener::StartListen(int iPort)
{
    m_oSocket.bind(QHostAddress("localhost"), iPort);
    m_oSocket.listen(15);
    m_ThreadListen.SetConnectionManager(m_pConnectionMgr);
    m_ThreadListen.SetSocket(&m_oSocket); 
    m_ThreadListen.start();
}

void CConnectionListener::StopListen()
{
    m_oSocket.close();
    m_ThreadListen.Stop();
    m_ThreadListen.wait(BEID_STOP_THREAD_TIME);
}


NAMESPACE_END
