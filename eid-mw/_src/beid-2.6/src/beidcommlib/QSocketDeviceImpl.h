// QSocketDeviceImpl.h: interface for the QSocketDeviceImpl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QSOCKETDEVICEIMPL_H__9DC9D0FE_5669_4101_B686_A356C2C7C5E4__INCLUDED_)
#define AFX_QSOCKETDEVICEIMPL_H__9DC9D0FE_5669_4101_B686_A356C2C7C5E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "config.h"
#include "MessageQueue.h"

class QSocketDeviceImpl;

class QSendRecvThread : public QThread
{
    public:
	    QSendRecvThread();
        void SetSocket(QSocketDeviceImpl *pSocket) { m_pSocket = pSocket; }
        void Stop() { m_bRun = false; }
        virtual void run();
    protected:
        void ProcessSendRecv();
    private:
        QSocketDeviceImpl *m_pSocket;
        bool m_bRun;
};

class QSocketDeviceImpl : public QSocketDevice
{
public:
	QSocketDeviceImpl();
	virtual ~QSocketDeviceImpl();
    void SetQueue(EIDCommLIB::CMessageQueue *pQueue);
    EIDCommLIB::CMessageQueue *GetQueue() { return m_pQueue; }
    void Stop(); 
    
private:
    EIDCommLIB::CMessageQueue *m_pQueue;
    QSendRecvThread *m_pThreadSendRecv;
};


#endif // !defined(AFX_QSOCKETDEVICEIMPL_H__9DC9D0FE_5669_4101_B686_A356C2C7C5E4__INCLUDED_)
