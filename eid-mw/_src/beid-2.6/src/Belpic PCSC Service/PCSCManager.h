// PCSCManager.h: interface for the CPCSCManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PCSCMANAGER_H__ED195A11_33EC_4309_B422_33CF927E638A__INCLUDED_)
#define AFX_PCSCMANAGER_H__ED195A11_33EC_4309_B422_33CF927E638A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "maindialogimpl.h"

class CTransactionStack;
class CApplicationObject;
class CReaderObject;
class CApplicationManager;
class EIDCommLIB::CCardMessage;
class EIDCommLIB::CConnectionManager;

class CPCSCManager
{
public:
	CPCSCManager();
	virtual ~CPCSCManager();

    long Initialize();
    void SetConnectionManager(EIDCommLIB::CConnectionManager *pMgr) { m_pConnMgr = pMgr; }
    void SetMainDialog(maindialogImpl *pMainDlg) { m_pMainDlg = pMainDlg; }
    SCARDCONTEXT GetContextHandle();
    void EventCardState(unsigned long ulState, const char *pszReaderName);

    // Interface to MessageHandlers
    long S_CardEstablishContext(EIDCommLIB::CCardMessage *pMessage);
    long S_CardReleaseContext(EIDCommLIB::CCardMessage *pMessage);
    long S_CardConnect(EIDCommLIB::CCardMessage *pMessage);
    long S_CardDisconnect(EIDCommLIB::CCardMessage *pMessage);
    long S_CardStatus(EIDCommLIB::CCardMessage *pMessage);
    long S_CardListReaders(EIDCommLIB::CCardMessage *pMessage);
    long S_CardGetStatusChange(EIDCommLIB::CCardMessage *pMessage);
    long S_CardBeginTransaction(EIDCommLIB::CCardMessage *pMessage);
    long S_CardEndTransaction(EIDCommLIB::CCardMessage *pMessage);
    long S_CardTransmit(EIDCommLIB::CCardMessage *pMessage);
    long S_CardControl(EIDCommLIB::CCardMessage *pMessage);
    long S_CardControlPinPad(EIDCommLIB::CCardMessage *pMessage);

    BOOL Suspend();

protected:
//    BOOL CheckAccess(CApplicationObject* pApplication, BYTE *pSendBuffer, unsigned long ulSendLen);
protected:
    SCARDCONTEXT m_hContext;
    typedef std::map<std::string, CReaderObject *> MapReaders;
    typedef MapReaders::iterator ItMapReaders;
    MapReaders m_Readers; 
    CApplicationManager *m_pApplicationManager; 
    CTransactionStack *m_pTransActionStack;
    EIDCommLIB::CConnectionManager *m_pConnMgr;
    maindialogImpl *m_pMainDlg;
};

#endif // !defined(AFX_PCSCMANAGER_H__ED195A11_33EC_4309_B422_33CF927E638A__INCLUDED_)
