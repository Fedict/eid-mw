// Connection.h: interface for the CConnection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONNECTION_H__7DFEEC58_4AD8_4237_99E8_B1927924E530__INCLUDED_)
#define AFX_CONNECTION_H__7DFEEC58_4AD8_4237_99E8_B1927924E530__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "config.h"
#include "QSocketDeviceImpl.h"
#include "CardMessage.h"
#include "ServerRecvThread.h"

NAMESPACE_BEGIN(EIDCommLIB)

class CMessageHandler;

class CConnection  
{
public:
	CConnection();
	virtual ~CConnection();
    bool Open(const std::string & strHost, int iPort);
    void Close();
    void AttachSocket(int iSocket);
    std::string SdMessage(CCardMessage *pMessage);
    CCardMessage *RecvMessage();
    CCardMessage *WaitMessageForID(std::string strId);
    CCardMessage *WaitMessageForIDTimeOut(std::string strId, long lTimeOut);
    bool isValid();
    void SetMessageHandler(CMessageHandler *pHandler) { m_pHandler = pHandler; }
    CMessageHandler *GetMessageHandler() { return m_pHandler; }
    QSocketDevice *GetSocket() { return m_pSocket; }
    int GetConnectionID() { return m_pSocket->socket(); }

private:
    CMessageQueue *m_pQueue;
    QSocketDeviceImpl *m_pSocket;
    CServerRecvThread m_oRecvThread;
    CMessageHandler *m_pHandler;
};

NAMESPACE_END

#endif // !defined(AFX_CONNECTION_H__7DFEEC58_4AD8_4237_99E8_B1927924E530__INCLUDED_)
