// QSocketDeviceImpl.cpp: implementation of the QSocketDeviceImpl class.
//
//////////////////////////////////////////////////////////////////////

#include "QSocketDeviceImpl.h"
#include "CardMessage.h"
#include <qglobal.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

using namespace EIDCommLIB;

QSendRecvThread::QSendRecvThread() : m_pSocket(NULL), m_bRun(true)
{
}

void QSendRecvThread::run()
{
    QWaitCondition oWait;
    CMessageQueue *pQueue = m_pSocket->GetQueue(); 
    while (m_bRun)
    {
        ProcessSendRecv();
        if(!m_pSocket->isValid())
        {
            pQueue->EnableWait(false);
            m_bRun = false;
        }
        else
        {
            oWait.wait(5);
        }
    }
}


void QSendRecvThread::ProcessSendRecv()
{
    CMessageQueue *pQueue = m_pSocket->GetQueue(); 
    QByteArray oSendData;
    if(pQueue && pQueue->GetSend(oSendData))
    {
        if(-1 == m_pSocket->writeBlock(oSendData.data(), oSendData.size()))
        {
            m_pSocket->close();
            return;
        }   
    }
    if(m_pSocket && m_pSocket->bytesAvailable() > 0)
    {
        char *pszLenBuffer = new char[sizeof(Q_UINT32)];
        memset(pszLenBuffer, 0, sizeof(Q_UINT32));
        int iBytesRet = m_pSocket->readBlock(pszLenBuffer, sizeof(Q_UINT32));
        if(iBytesRet > 0 && m_pSocket->bytesAvailable() > 0)
        {  
            char szBuffer[1024] = {0};
            Q_UINT32 iToBeReceived = 0;
            QByteArray sink;
            sink.setRawData(pszLenBuffer, iBytesRet);
            QDataStream streamRecvLen(sink, IO_ReadOnly);
            streamRecvLen >> (Q_UINT32&)iToBeReceived;
            sink.resetRawData(pszLenBuffer, iBytesRet);
            if(iToBeReceived > 0 && iToBeReceived < BEID_MAX_MESSAGE_SIZE)
            {
                QByteArray oRecvData;
                QDataStream streamRecv(oRecvData, IO_WriteOnly);
                unsigned int iReceived = 0;
                do
                {
                    iBytesRet = m_pSocket->readBlock(szBuffer, QMIN(iToBeReceived, sizeof(szBuffer)));
                    if(iBytesRet > 0)
                    {
                        iReceived += iBytesRet;
                        streamRecv.writeRawBytes(szBuffer, iBytesRet);
                    }
                }
                while(m_pSocket->bytesAvailable() > 0 && iBytesRet > 0 && iReceived < iToBeReceived);
                if(oRecvData.size() > 0)
                {
                    CCardMessage oMessage;
                    if(oMessage.Unserialize((const byte *)oRecvData.data(), oRecvData.size()))
                    {
                        long lAlive = 0;
                        if(!oMessage.Get("Alive", lAlive))
                        {
                            pQueue->PutRecv(oRecvData); 
                        }
                    }
                    else
                    {
                        pQueue->PutSend(QUuid::createUuid().toString().ascii(), oRecvData); 
                    }
                }
            }
            else
            {
                iBytesRet  = -1;
            }
        }
        delete [] pszLenBuffer;
        if(-1 == iBytesRet)
        {
            m_pSocket->close();
        }   
    }
}


QSocketDeviceImpl::QSocketDeviceImpl() : m_pThreadSendRecv(NULL)
{
    setBlocking(true);
}

QSocketDeviceImpl::~QSocketDeviceImpl()
{
}

void QSocketDeviceImpl::SetQueue(CMessageQueue *pQueue)
{
    m_pQueue = pQueue;
    m_pThreadSendRecv = new QSendRecvThread();
    m_pThreadSendRecv->SetSocket(this);
    m_pThreadSendRecv->start();
}

void QSocketDeviceImpl::Stop() 
{     
    if(m_pThreadSendRecv)
    {
        QWaitCondition oWait;
        m_pThreadSendRecv->Stop();
        oWait.wait(100);
        delete m_pThreadSendRecv;
        m_pThreadSendRecv = NULL;
    }
}

