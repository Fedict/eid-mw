// ApplicationObject.cpp: implementation of the CApplicationObject class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ApplicationObject.h"
#include <stdlib.h>
#include <time.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CApplicationObject::CApplicationObject()
{
    m_hContext = 0;
    m_lConnID = 0;
    m_lRefCount = 0;
    srand( (unsigned)time( NULL ) );
}

CApplicationObject::~CApplicationObject()
{
    CAppReaderObject *pObject = NULL;
    ItMapReaders it;
    for(it = m_oReaders.begin(); it != m_oReaders.end(); ++it)
    {
	    pObject = (*it).second;
        if(pObject)
        {
            delete pObject;
        }
    }
    m_oReaders.clear();
}

void CApplicationObject::SetConnID(long lConnID)
{
    m_lConnID = lConnID;
}

long CApplicationObject::GetConnID()
{
    return m_lConnID;
}

void CApplicationObject::SetPID(long lPID)
{
    m_lPID = lPID;
}

long CApplicationObject::GetPID()
{
    return m_lPID;
}

void CApplicationObject::SetContext(SCARDCONTEXT hContext)
{
    m_hContext = hContext;
}

SCARDCONTEXT CApplicationObject::GetContext()
{
    return m_hContext;
}

void CApplicationObject::SetApplicationName(char *pszApplicationName)
{
    if(pszApplicationName)
    {
        m_strApplicationName = pszApplicationName;
    }
}

const char *CApplicationObject::GetApplicationName()
{
    return m_strApplicationName.c_str();
}

void CApplicationObject::SetUserName(char *pszUserName)
{
    if(pszUserName)
    {
        m_strUserName = pszUserName;
    }
}

const char *CApplicationObject::GetUserName()
{
    return m_strUserName.c_str();
}

SCARDHANDLE CApplicationObject::AddReader(CReaderObject *pReader)
{
    SCARDHANDLE hCard = 0;
    if(pReader != NULL)
    {
        // Lookup
        ItMapReaders it = m_oReaders.find(pReader->GetCardHandle());
        if(it != m_oReaders.end())
        {
            pReader->AddRef(); 
            return (*it).first;
        }

        // Not found, create random CardHandle
        hCard = rand();
        pReader->AddRef();
        CAppReaderObject *pAppReaderObject = new CAppReaderObject();
        pAppReaderObject->pReader = pReader;
        pAppReaderObject->hOriginal = pReader->GetCardHandle(); 
        m_oReaders[hCard] = pAppReaderObject;
    }
    return hCard;
}

void CApplicationObject::DeleteReader(SCARDHANDLE hHandle)
{
    CReaderObject *pObject = NULL;
    ItMapReaders it = m_oReaders.find(hHandle);
    if(it != m_oReaders.end())
    {
        pObject = ((*it).second)->pReader;
        if (pObject != NULL) 
        {
            long lRef = pObject->ReleaseRef();
            if (0 == lRef)
            {
                delete (*it).second;
                m_oReaders.erase(it);
            }        
        }     
    }
}

CReaderObject *CApplicationObject::GetReader(SCARDHANDLE hHandle)
{
    CReaderObject *pReader = NULL;
    ItMapReaders it = m_oReaders.find(hHandle);
    if(it != m_oReaders.end())
    {
        pReader = ((*it).second)->pReader;
    }
    return pReader;
}

BOOL CApplicationObject::MatchOriginal(SCARDHANDLE hHandle)
{
    BOOL bRet = FALSE;
    CReaderObject *pReader = NULL;
    ItMapReaders it = m_oReaders.find(hHandle);
    if(it != m_oReaders.end())
    {
        pReader = ((*it).second)->pReader;
        if(pReader != NULL)
        {
             SCARDHANDLE hReal = pReader->GetCardHandle();
             if(hReal > 0 && hReal == ((*it).second)->hOriginal)
             {
                 bRet = TRUE;
             }
        }
    }
    return bRet;
}

SCARDHANDLE CApplicationObject::GetOriginal(SCARDHANDLE hHandle)
{
    SCARDHANDLE hOri = 0;
    CReaderObject *pReader = NULL;
    ItMapReaders it = m_oReaders.find(hHandle);
    if(it != m_oReaders.end())
    {
        pReader = ((*it).second)->pReader;
        if(pReader != NULL)
        {
             hOri = ((*it).second)->hOriginal;
        }
    }
    return hOri;
}

void CApplicationObject::AddRef()
{
    m_lRefCount++;
}

long CApplicationObject::ReleaseRef()
{
    if(m_lRefCount > 0)
    {
        m_lRefCount--;
    }
    return m_lRefCount;
}

void CApplicationObject::DeleteReader(CReaderObject *pReader, std::vector<long> & oDummyHandles)
{
    if(pReader != NULL)
    {
        // Lookup
        CAppReaderObject *pObject = NULL;
        ItMapReaders it;
        bool bCont = true;
        while (bCont)
        {
            bCont = false;
            for(it = m_oReaders.begin(); it != m_oReaders.end(); ++it)
            {
	            pObject = (*it).second;
                if (pObject && pObject->pReader->GetCardHandle() == pReader->GetCardHandle() )
                {
                    oDummyHandles.push_back((*it).first);
                    delete (*it).second;
                    m_oReaders.erase(it);
                    bCont = true;
                    break;
                }
            }
        }
    }
}

SCARDHANDLE CApplicationObject::GetDummy(SCARDHANDLE hRealHandle)
{
    SCARDHANDLE hDummy = 0;
    CAppReaderObject *pObject = NULL;
    ItMapReaders it;
    for(it = m_oReaders.begin(); it != m_oReaders.end(); ++it)
    {
	    pObject = (*it).second;
        if(pObject && pObject->pReader->GetCardHandle() == hRealHandle)
        {
            hDummy = (*it).first;
            break;
        }
    }
    return hDummy;
}
