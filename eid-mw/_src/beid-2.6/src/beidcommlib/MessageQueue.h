// MessageQueue.h: interface for the CMessageQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESSAGEQUEUE_H__8491E233_9068_443C_9E6D_32D5FF404707__INCLUDED_)
#define AFX_MESSAGEQUEUE_H__8491E233_9068_443C_9E6D_32D5FF404707__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "config.h"

NAMESPACE_BEGIN(EIDCommLIB)

class CMessageQueue  
{
public:
	CMessageQueue();
	virtual ~CMessageQueue();

    void PutSend(const std::string & strId, const QByteArray & data);
    void PutSend(const std::string & strId, const byte *pData, const unsigned int iSize);
    bool GetSend(QByteArray & data);
    bool GetSend(byte *pValue, unsigned int & iSize);
    bool GetRecv(QByteArray & data);
    bool GetRecv(byte *pValue, unsigned int & iSize);
    void PutRecv(const QByteArray & data);
    void PutRecv(const byte *pData, const unsigned int iSize);
    bool GetRecv(const std::string & strId, QByteArray & data, bool bBlock = true, long timeout = 0);
    bool GetRecv(const std::string & strId, byte *pValue, unsigned int & iSize, bool bBlock = true, long timeout = 0);
    void EnableWait(bool bEnable) { m_bWait = bEnable; }

protected:
    std::string GetMessageId(const QByteArray & data);

private:
    typedef std::deque<std::pair<std::string, QByteArray *> > MsgQueue;
    typedef MsgQueue::iterator ItMsgQueue;
    MsgQueue m_queueSend; 
    MsgQueue m_queueRecv; 
    QMutex m_Lock;
    long m_lTimeout;
    long m_lWaitUnit;
    bool m_bWait;
};

NAMESPACE_END

#endif // !defined(AFX_MESSAGEQUEUE_H__8491E233_9068_443C_9E6D_32D5FF404707__INCLUDED_)
