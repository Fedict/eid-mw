// ReaderObject.cpp: implementation of the CReaderObject class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ReaderObject.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReaderObject::CReaderObject() : m_strName("")
{
    m_hCard = 0;
    m_ulProtocol = 0;
    m_lRefCount = 0;
}

CReaderObject::~CReaderObject()
{

}

void CReaderObject::SetName(char *pszName) 
{
    if(pszName != NULL)
    {
        m_strName = pszName;
    }
}

const char *CReaderObject::GetName() 
{
    return m_strName.c_str();
}

void CReaderObject::AddRef()
{
    m_lRefCount++;
}

long CReaderObject::ReleaseRef()
{
    if(m_lRefCount > 0)
    {
        m_lRefCount--;
    }
    return m_lRefCount;
}

