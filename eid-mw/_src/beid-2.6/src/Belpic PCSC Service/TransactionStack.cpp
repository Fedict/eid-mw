// TransactionStack.cpp: implementation of the CTransactionStack class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "TransactionStack.h"
#include "../beidcommon/config.h"

#include <sys/timeb.h>

#ifdef _WIN32
    #define timeb _timeb
    #define ftime _ftime
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTransactionStack::CTransactionStack()
{
    eidcommon::CConfig oConfig;
    oConfig.Load();
    m_lMaxTime = oConfig.GetLockTimeout(); //Default 60 sec in stack
}

CTransactionStack::~CTransactionStack()
{
    ItQueue it;
    for (it = m_oQueue.begin(); it != m_oQueue.end(); ++it)
    {
        __TransactionStack *pTemp = (*it);
        if(pTemp)
        {
            delete pTemp;
        }        
    }
    m_oQueue.clear();
}

void CTransactionStack::SetMaxTime (long lSec)
{
	m_lMaxTime = lSec;
}

void CTransactionStack::AddHandle(SCARDHANDLE hCard)
{
    QMutexLocker oLock(&m_lock);
	struct timeb timebuffer;
	ftime( &timebuffer );
	__TransactionStack *temp = new __TransactionStack();
    temp->time = timebuffer.time;
    temp->hCard = hCard;
    m_oQueue.push_back(temp);
}

void CTransactionStack::DeleteHandle(SCARDHANDLE hCard)
{
    QMutexLocker oLock(&m_lock);
    ItQueue it;
    for (it = m_oQueue.begin(); it != m_oQueue.end(); ++it)
    {
        __TransactionStack *pTemp = (*it);
        if(pTemp && pTemp->hCard == hCard)
        {
            m_oQueue.erase(it);
            delete pTemp;
            break;
        }        
    }
}

void CTransactionStack::DeleteAllHandle(SCARDHANDLE hCard)
{
    QMutexLocker oLock(&m_lock);    
    ItQueue it;
    bool bNext = true;
    while (bNext)
    {
        bNext = false;
        for (it = m_oQueue.begin(); it != m_oQueue.end() && !bNext; ++it)
        {
            __TransactionStack *pTemp = (*it);
            if(pTemp && pTemp->hCard == hCard)
            {
                m_oQueue.erase(it);
                delete pTemp;
                bNext = true;
            }
        }
    }
}

int CTransactionStack::IsHandleAvailable (SCARDHANDLE hCard)
{
    QMutexLocker oLock(&m_lock);
    bool bFound = false;
    int iRet = 0;

    if(m_oQueue.size() > 0)
    {
        if(m_oQueue.front()->hCard == hCard)
        {
            bFound = true;
            iRet = 1;
        }
        else
        {
            ItQueue it;
            for (it = m_oQueue.begin(); it != m_oQueue.end(); ++it)
            {
                __TransactionStack *pTemp = (*it);
                if(pTemp && pTemp->hCard == hCard)
                {
                    bFound = true;
                    break;
                }        
            }
        }
    }

    if(!bFound)
    {
        iRet = -1;
    }
    return iRet;
}

void CTransactionStack::run()
{
    bool bWait = false;
    while(!bWait)
    {
        if(m_oQueue.size() > 0)
        {
            m_lock.lock();
	        struct timeb timebuffer;
	        ftime( &timebuffer );
            __TransactionStack *pTemp = m_oQueue.front();
            if(pTemp && (pTemp->time + m_lMaxTime) < timebuffer.time)
            {
                m_oQueue.pop_front();
                delete pTemp;
                if(m_oQueue.size() > 0)
                {
                    pTemp = m_oQueue.front();
                    if(pTemp)
                    {
                        pTemp->time = timebuffer.time;
                    }
                }
            }
            m_lock.unlock();
        }
        bWait = m_oWait.wait(5000);
    }
}

void CTransactionStack::StopRunning()
{
    m_oWait.wakeOne();
}
