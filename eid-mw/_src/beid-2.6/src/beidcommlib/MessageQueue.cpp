// MessageQueue.cpp: implementation of the CMessageQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "MessageQueue.h"
#include <qwaitcondition.h>
#include <algorithm>

using namespace std;

NAMESPACE_BEGIN(EIDCommLIB)

class CFindMsgId
{
    private:
        string m_strId;
    public:
        CFindMsgId(const string & strId) : m_strId(strId) {}
        bool operator () (pair<string, QByteArray *> & data)
        {
            return (data.first == m_strId);
        }
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMessageQueue::CMessageQueue() : m_lTimeout(60000), m_lWaitUnit(5), m_bWait(true)
{

}

CMessageQueue::~CMessageQueue()
{
    ItMsgQueue it;
    for (it = m_queueSend.begin(); it != m_queueSend.end(); ++it)
    {
        delete (*it).second;
    }
    for (it = m_queueRecv.begin(); it != m_queueRecv.end(); ++it)
    {
        delete (*it).second;
    }
    m_queueSend.clear();
    m_queueRecv.clear();
}

void CMessageQueue::PutSend(const string & strId, const QByteArray & data)
{
    QMutexLocker locker( &m_Lock );
    QByteArray sink;
    QDataStream streamSend(sink, IO_WriteOnly);
    streamSend << (Q_UINT32)data.size();
    streamSend.writeRawBytes(data.data(), data.size());
    QByteArray *pBytes = new QByteArray(sink.copy());
    m_queueSend.push_back(make_pair(strId, pBytes));
}

void CMessageQueue::PutSend(const std::string & strId, const byte *pData, const unsigned int iSize)
{
    if(pData)
    {
        QByteArray bytesBlock;
        bytesBlock.duplicate((const char *)pData, iSize);
        PutSend(strId, bytesBlock);
    }
}

bool CMessageQueue::GetRecv(const string & strId, QByteArray & data, bool bBlock /* true */, long timeOut /* 0 */)
{
    QWaitCondition oWait;
    ItMsgQueue it;
    long lCount = 0;
    bool bFound = false;
    long lTimeout = timeOut > 0 ? timeOut  : m_lTimeout;

    while(!bFound && lCount < lTimeout)
    {
        m_Lock.lock();
        it = std::find_if(m_queueRecv.begin(), m_queueRecv.end(), CFindMsgId(strId));
        if(it != m_queueRecv.end())
        {
            data = ((*it).second)->copy();
            delete (*it).second;
            m_queueRecv.erase(it);
            bFound = true;
        }
        m_Lock.unlock();
        if(m_bWait && bBlock && !bFound)
        {
            oWait.wait(m_lWaitUnit);
            lCount += m_lWaitUnit;
        }
        else
        {
            break;
        }
    }
    return bFound;
}

bool CMessageQueue::GetRecv(const std::string & strId, byte *pValue, unsigned int & iSize, bool bBlock /* true */, long timeOut /* 0 */)
{
    bool bFound = false;
    if(pValue && iSize > 0)
    {
        QByteArray data;
        bFound = GetRecv(strId, data, bBlock, timeOut);
        if(bFound && iSize >= data.size())
        {
            memcpy(pValue, data.data(), data.size());
            iSize = data.size();
        }
        else
        {
            iSize = 0;
        }
    }
    return bFound;
}

bool CMessageQueue::GetSend(QByteArray & data)
{
    QMutexLocker locker( &m_Lock );
    bool bFound = false;
    if(m_queueSend.size() > 0)
    {
        data = (m_queueSend.front().second)->copy();
        delete m_queueSend.front().second;
        m_queueSend.pop_front();
        bFound = true;
    }
    return bFound;
}

bool CMessageQueue::GetSend(byte *pValue, unsigned int & iSize)
{
    QMutexLocker locker( &m_Lock );
    bool bFound = false;
    if(pValue && iSize > 0)
    {
        QByteArray data;
        bFound = GetSend(data);
        if(bFound && iSize >= data.size())
        {
            memcpy(pValue, data.data(), data.size());
            iSize = data.size();
        }
        else
        {
            iSize = 0;
        }
    }
    return bFound;
}

bool CMessageQueue::GetRecv(QByteArray & data)
{
    QMutexLocker locker( &m_Lock );
    bool bFound = false;
    if(m_queueRecv.size() > 0)
    {
        data = (m_queueRecv.front().second)->copy();
        delete m_queueRecv.front().second;
        m_queueRecv.pop_front();
        bFound = true;
    }
    return bFound;
}

bool CMessageQueue::GetRecv(byte *pValue, unsigned int & iSize)
{
    bool bFound = false;
    if(pValue && iSize > 0)
    {
        QByteArray data;
        bFound = GetSend(data);
        if(bFound && iSize >= data.size())
        {
            memcpy(pValue, data.data(), data.size());
            iSize = data.size();
        }
        else
        {
            iSize = 0;
        }
    }
    return bFound;
}

void CMessageQueue::PutRecv(const QByteArray & data)
{
    QMutexLocker locker( &m_Lock );
    string strId = GetMessageId(data);
    QByteArray *pBytes = new QByteArray();
    pBytes->duplicate(data.data(), data.size());
    m_queueRecv.push_back(make_pair(strId, pBytes));
}

void CMessageQueue::PutRecv(const byte *pData, const unsigned int iSize)
{
    if(pData)
    {
        QByteArray bytesBlock;
        bytesBlock.duplicate((const char *)pData, iSize);
        PutRecv(bytesBlock);
    }
}

string CMessageQueue::GetMessageId(const QByteArray & data)
{
    string strId;
    QDataStream stream(data, IO_ReadOnly);
    // Get Message ID
    QUuid oId;
    stream >> oId;

    if(!oId.isNull())
    {
        strId = oId.toString().ascii();
    }
    return strId;
}

NAMESPACE_END
