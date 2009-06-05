// SCardHandler.h: interface for the CSCardHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCARDHANDLER_H__BF4B0501_DD55_4D14_BD47_9DE267C4E551__INCLUDED_)
#define AFX_SCARDHANDLER_H__BF4B0501_DD55_4D14_BD47_9DE267C4E551__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _WIN32
#pragma warning(disable:4786)
#endif

class CPCSCManager;
class EIDCommLIB::CCardMessage;

class CSCardHandler : public EIDCommLIB::CMessageHandler 
{
public:
	CSCardHandler();
	virtual ~CSCardHandler();
    void SetPCSCManager(CPCSCManager *pManager);
	virtual void Execute(EIDCommLIB::CCardMessage *pMessage);
    virtual EIDCommLIB::CMessageHandler *Clone();

protected:
    long SCardMapFunction(EIDCommLIB::CCardMessage *pMessage);
    long CheckIPAllowed();
private:
    CPCSCManager *m_pPCSCManager;
    QStringList m_IPs;
};

#endif // !defined(AFX_SCARDHANDLER_H__BF4B0501_DD55_4D14_BD47_9DE267C4E551__INCLUDED_)
