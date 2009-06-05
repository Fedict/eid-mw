// MessageHandler.h: interface for the CMessageHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESSAGEHANDLER_H__CFEB91A0_C114_474B_878A_80EBF99F4F48__INCLUDED_)
#define AFX_MESSAGEHANDLER_H__CFEB91A0_C114_474B_878A_80EBF99F4F48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "config.h"

NAMESPACE_BEGIN(EIDCommLIB)

class CConnection;

class CMessageHandler  
{
public:
    CMessageHandler() { }
    virtual ~CMessageHandler() { }
    void SetConnection(CConnection *pConnection) { m_pConnection = pConnection; }
    CConnection *GetConnection() { return m_pConnection; }
	virtual void Execute(CCardMessage *pMessage) = 0;
    virtual CMessageHandler *Clone() = 0;

protected:
    CConnection *m_pConnection;
};

NAMESPACE_END

#endif // !defined(AFX_MESSAGEHANDLER_H__CFEB91A0_C114_474B_878A_80EBF99F4F48__INCLUDED_)
