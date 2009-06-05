// ApplicationManager.cpp: implementation of the CApplicationManager class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ApplicationManager.h"
#include "ApplicationObject.h"
#include <stdlib.h>
#include <time.h>

using namespace EIDCommLIB;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CApplicationManager::CApplicationManager()
{
    srand( (unsigned)time( NULL ) );
}

CApplicationManager::~CApplicationManager()
{
    CApplicationObject *pObject = NULL;
    ItMapApps it;
    for(it = m_Applications.begin(); it != m_Applications.end(); ++it)
    {
	    pObject = (*it).second;
        if(pObject)
        {
            delete pObject;
        }
    }
    m_Applications.clear();
}

CApplicationObject *CApplicationManager::FindExisting(long lConnID)
{
    QMutexLocker oLock(&m_lock);
    CApplicationObject *pObject = NULL;
    ItMapApps it;
    for(it = m_Applications.begin(); it != m_Applications.end(); ++it)
    {
	    pObject = (*it).second;
        if(pObject && pObject->GetConnID() == lConnID)
        {
            break;
        }
    }
    return pObject;
}

CApplicationObject * CApplicationManager::FindApplication(SCARDCONTEXT hContext)
{
    QMutexLocker oLock(&m_lock);
    CApplicationObject *pApp = NULL;
    ItMapApps it = m_Applications.find(hContext);
    if(it != m_Applications.end())
    {
        pApp = (*it).second;
    }
    return pApp;
}

CApplicationObject *CApplicationManager::CreateApplication(CCardMessage *pMessage, long lConnID)
{
    QMutexLocker oLock(&m_lock);
    CApplicationObject *pApp = NULL;
    if(pMessage != NULL)
    {
        pApp = new CApplicationObject;
        // Create random Context
        SCARDCONTEXT hContext = rand();
        pApp->SetContext(hContext); 
        long lPID = 0;
        if(pMessage->Get("ProcessID", lPID))
        {
            pApp->SetPID(lPID);
        }
        pApp->SetConnID(lConnID);
        pApp->AddRef(); 
        
        // Add to collection
        m_Applications[hContext] = pApp;
    }
    return pApp;
}

void CApplicationManager::DeleteApplication(SCARDCONTEXT hContext)
{
    QMutexLocker oLock(&m_lock);
    CApplicationObject *pApp = NULL;
    ItMapApps it = m_Applications.find(hContext);
    if(it != m_Applications.end())
    {
        pApp = (*it).second;
        if(pApp != NULL)
        {
            long lRef = pApp->ReleaseRef(); 
            if (0 == lRef)
            {
                delete pApp;
                pApp = NULL;
                m_Applications.erase(it);
            }
        }
    }
}

void CApplicationManager::DeleteApplicationReaders(CReaderObject *pReader, std::vector<long> & oDummyHandles)
{
    QMutexLocker oLock(&m_lock);
    if(pReader)
    {
        CApplicationObject *pObject = NULL;
        ItMapApps it;
        for(it = m_Applications.begin(); it != m_Applications.end(); ++it)
        {
	        pObject = (*it).second;
            if(pObject)
            {
                pObject->DeleteReader(pReader, oDummyHandles); 
            }
        }
    }
}

