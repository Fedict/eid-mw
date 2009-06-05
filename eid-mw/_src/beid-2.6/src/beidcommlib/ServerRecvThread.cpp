// ServerRecvThread.cpp: implementation of the CServerRecvThread class.
//
//////////////////////////////////////////////////////////////////////

#include "ServerRecvThread.h"
#include "Connection.h"
#include "MessageHandler.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NAMESPACE_BEGIN(EIDCommLIB)

CServerRecvThread::CServerRecvThread() : m_bRun(true), m_pConnection(NULL)
{

}

CServerRecvThread::~CServerRecvThread()
{

}

void CServerRecvThread::run()
{
    QWaitCondition oWait;
    CCardMessage *pMessage = NULL;
    while (m_bRun)
    {
        if(m_pConnection)
        {
            CMessageHandler *pHandler = m_pConnection->GetMessageHandler();
            pMessage = m_pConnection->RecvMessage();            
            if(pMessage && pHandler)
            {
                long lAlive = 0;
                if(!pMessage->Get("Alive", lAlive))
                {
                    pHandler->Execute(pMessage);
                    long lRetLater = 0;
                    // Check for later return
                    if(!pMessage->Get("ReturnLater", lRetLater))
                    {
                        m_pConnection->SdMessage(pMessage);
                    }
                }
                delete pMessage;
            }
            /* Echo test
            if(pMessage)
            {
                long lAlive = 0;
                if(!pMessage->Get("Alive", lAlive))
                {
                    m_pConnection->SdMessage(pMessage);
                }
                delete pMessage;
            }*/
        }
        oWait.wait(5);
    }
}

NAMESPACE_END
