// Connection.cpp: implementation of the CConnection class.
//
//////////////////////////////////////////////////////////////////////

#include "Connection.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NAMESPACE_BEGIN(EIDCommLIB)

CConnection::CConnection() : m_pHandler(NULL), m_pSocket(NULL)
{
    m_pQueue = new CMessageQueue();
    m_pSocket = NULL;
}

CConnection::~CConnection()
{
    Close();
    if(m_pQueue)
    {
        delete m_pQueue;
        m_pQueue = NULL;
    }
}

bool CConnection::Open(const string & strHost, int iPort)
{
    if(NULL == m_pSocket)
    {
        m_pSocket = new QSocketDeviceImpl();
    }

    bool bRet = m_pSocket->connect(QHostAddress(strHost.c_str()), iPort);
    if(bRet)
    {
        m_pSocket->SetQueue(m_pQueue);
        m_pQueue->EnableWait(true);    
    }
    else
    {
        m_pQueue->EnableWait(true);    
    }
    return bRet;
}

void CConnection::Close()
{
    m_pQueue->EnableWait(false);    
    m_oRecvThread.Stop(); 
    m_oRecvThread.wait(BEID_STOP_THREAD_TIME); 
    if(m_pSocket)
    {
        if(m_pSocket->isValid())
        {
            m_pSocket->Stop();
        }
        delete m_pSocket;
        m_pSocket = NULL;
    }
}

string CConnection::SdMessage(CCardMessage *pMessage)
{
    string strId;
    if(pMessage && m_pQueue)
    {
        strId = pMessage->GetMessageId(); 
        unsigned int iSize = pMessage->GetSerializeSize();
        byte *pBytes = new byte[iSize];
        pMessage->Serialize(pBytes, iSize); 
        m_pQueue->PutSend(strId, pBytes, iSize); 
        delete [] pBytes;
    }
    return strId;
}

CCardMessage *CConnection::WaitMessageForID(string strId)
{
    CCardMessage *pMessage = NULL;
    if(m_pQueue)
    {
        QByteArray dataRecv;
        if(m_pQueue->GetRecv(strId, dataRecv))
        {
            pMessage = new CCardMessage();
            if(!pMessage->Unserialize((const byte *)dataRecv.data(), dataRecv.size()))
            {
                delete pMessage;
                pMessage = NULL;
            }
        }
    }
    return pMessage;
}

CCardMessage *CConnection::WaitMessageForIDTimeOut(string strId, long lTimeOut)
{
    CCardMessage *pMessage = NULL;
    if(m_pQueue)
    {
        QByteArray dataRecv;
        if(m_pQueue->GetRecv(strId, dataRecv, true, lTimeOut))
        {
            pMessage = new CCardMessage();
            if(!pMessage->Unserialize((const byte *)dataRecv.data(), dataRecv.size()))
            {
                delete pMessage;
                pMessage = NULL;
            }
        }
    }
    return pMessage;
}

void CConnection::AttachSocket(int iSocket)
{
    if(NULL == m_pSocket)
    {
        m_pSocket = new QSocketDeviceImpl();
        m_pSocket->SetQueue(m_pQueue);
    }
    m_pSocket->setSocket(iSocket, QSocketDevice::Stream);
    m_pQueue->EnableWait(true);
    m_oRecvThread.SetConnection(this);
    m_oRecvThread.start(); 
}

CCardMessage *CConnection::RecvMessage()
{
    CCardMessage *pMessage = NULL;
    if(m_pQueue)
    {
        QByteArray dataRecv;
        if(m_pQueue->GetRecv(dataRecv))
        {
            pMessage = new CCardMessage();
            if(!pMessage->Unserialize((const byte *)dataRecv.data(), dataRecv.size()))
            {
                delete pMessage;
                pMessage = NULL;
            }
        }
    }
    return pMessage;
}

bool CConnection::isValid() 
{   
    if(m_pSocket)
    {
        return (m_pSocket->isValid());
    }
    return false;
}

NAMESPACE_END
