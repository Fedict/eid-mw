// ConnectionManager.cpp: implementation of the CConnectionManager class.
//
//////////////////////////////////////////////////////////////////////

#include "ConnectionManager.h"
#include "MessageHandler.h"

NAMESPACE_BEGIN(EIDCommLIB)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

QConnectionThread::QConnectionThread() : m_bRun(false), m_pConnectionMgr(NULL)
{
}

void QConnectionThread::run()
{
    m_bRun = true;
    while (m_bRun)
    {
        if(m_pConnectionMgr)
        {
            m_pConnectionMgr->CleanUp();
            m_pConnectionMgr->SendAlive();
        }
        m_oWait.wait(5000);
    }
}

CConnectionManager::CConnectionManager() : m_pHandler(NULL)
{
}

CConnectionManager::~CConnectionManager()
{
    QMutexLocker lock(&m_oLock);
    m_oCleaner.Stop();
    m_oCleaner.wait(BEID_STOP_THREAD_TIME);
    for(unsigned int i = 0; i < m_oConnections.size(); ++i)
    {
        CConnection *pConn = m_oConnections[i];
        if(pConn->GetMessageHandler())
        {
            delete pConn->GetMessageHandler();
        }
        delete m_oConnections[i];
    }
    m_oConnections.clear();
}

CConnection *CConnectionManager::CreateConnection()
{
    CConnection *pConnection = new CConnection();
    if(m_pHandler)
    {
        CMessageHandler *pMessageHandler = m_pHandler->Clone();
        pMessageHandler->SetConnection(pConnection); 
        pConnection->SetMessageHandler(pMessageHandler); 
    }
    m_oConnections.push_back(pConnection);
    return pConnection;
}

void CConnectionManager::StartServer(int iPort)
{
    m_oCleaner.SetConnectionManager(this); 
    m_oCleaner.start();
    m_oListener.SetConnectionManager(this); 
    m_oListener.StartListen(iPort); 
}

void CConnectionManager::StopServer()
{
    m_oCleaner.Stop();
    m_oCleaner.wait(BEID_STOP_THREAD_TIME);
    m_oListener.StopListen(); 
}

void CConnectionManager::CleanUp()
{
    QMutexLocker lock(&m_oLock);
    ItVecConn it;
    bool bCont = true;
    while (bCont)
    {
        bCont = false;
        for(it = m_oConnections.begin(); it != m_oConnections.end(); ++it)
        {
            CConnection *pConn = (*it);
            if(pConn && !pConn->isValid())
            {
                delete pConn;
                m_oConnections.erase(it);
                bCont = true;
                break;
            }
        }
    }
}

void CConnectionManager::SendAlive()
{
    QMutexLocker lock(&m_oLock);
    ItVecConn it;
    for(it = m_oConnections.begin(); it != m_oConnections.end(); ++it)
    {
        CConnection *pConn = (*it);
        if(pConn && pConn->isValid())
        {
            CCardMessage oMessage;
            oMessage.Set("Alive", 1); 
            pConn->SdMessage(&oMessage);
        }
    }
}

CConnection *CConnectionManager::GetConnection(int iSocket)
{
    QMutexLocker lock(&m_oLock);
    CConnection *pConnRet = NULL;
    ItVecConn it;
    for(it = m_oConnections.begin(); it != m_oConnections.end(); ++it)
    {
        CConnection *pConn = (*it);
        if(pConn && pConn->GetConnectionID() == iSocket)
        {
            return pConn;
        }
    }
    return pConnRet;
}


NAMESPACE_END
