// TransactionStack.h: interface for the CTransactionStack class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRANSACTIONSTACK_H__F57690A6_294B_4970_947E_7B659496EECE__INCLUDED_)
#define AFX_TRANSACTIONSTACK_H__F57690A6_294B_4970_947E_7B659496EECE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <time.h>
#include <deque>

class CTransactionStack : public QThread
{
public:
	CTransactionStack();
	virtual ~CTransactionStack();

	void AddHandle(SCARDHANDLE hCard);
	int IsHandleAvailable (SCARDHANDLE hCard);
	void DeleteHandle(SCARDHANDLE hCard);
	void DeleteAllHandle(SCARDHANDLE hCard);

	void SetMaxTime (long lSec);
	virtual void run ();
    void StopRunning();

protected:
	QMutex m_lock;
	struct __TransactionStack 
	{
		SCARDHANDLE hCard;
		time_t time;
	};

	long m_lMaxTime;
    std::deque<__TransactionStack *> m_oQueue;
    typedef std::deque<__TransactionStack *>::iterator ItQueue;
    QWaitCondition m_oWait;
};

#endif // !defined(AFX_TRANSACTIONSTACK_H__F57690A6_294B_4970_947E_7B659496EECE__INCLUDED_)
