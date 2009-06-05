// PCSCManager.cpp: implementation of the CPCSCManager class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <qapplication.h> 
#include "PCSCManager.h"
#include "ApplicationManager.h"
#include "ReaderObject.h"
#include "ApplicationObject.h"
#include "TransactionStack.h"
#include "../beidcommon/TLVBuffer.h"
#include <string>
#include <qwaitcondition.h> 

using namespace EIDCommLIB;
using namespace eidcommon;

#ifdef _DEBUGLOG
    static char gszLogBuffer[256] = {0};
#endif

#ifdef BELPIC_PIN_PAD
    #include "../beidcommon/scr.h"
#endif // BELPIC_PIN_PAD


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPCSCManager::CPCSCManager()
{
    m_hContext = 0;
    m_pConnMgr = NULL;
    m_pApplicationManager = new CApplicationManager();
    m_pTransActionStack = new CTransactionStack;
    if(NULL != m_pTransActionStack)
    {
        m_pTransActionStack->start();
    }
    m_pMainDlg = NULL;
}

CPCSCManager::~CPCSCManager()
{
	unsigned long ulTimeout = 0;

    if(NULL != m_pTransActionStack)
    {
        m_pTransActionStack->StopRunning();
        m_pTransActionStack->wait(5000);
        delete m_pTransActionStack;
        m_pTransActionStack = NULL;
    }

    CReaderObject *pObject = NULL;
    ItMapReaders it;
    for(it = m_Readers.begin(); it != m_Readers.end(); ++it)
    {
	    pObject = (*it).second;
        if(pObject)
        {
            if(pObject->GetCardHandle () > 0)
            {
                // Disconnect Card
                SCardDisconnect(pObject->GetCardHandle (), SCARD_LEAVE_CARD);
            }
            delete pObject;
        }
    }
    m_Readers.clear();

    if(m_hContext != 0)
    {
         // Release Context
         SCardReleaseContext(m_hContext);
         m_hContext = 0;
    }
}

long CPCSCManager::Initialize()
{
    long lReturn = SCARD_S_SUCCESS;
    // Establish the context.
    if(m_hContext != 0)
    {
        // Release Context
        ::SCardReleaseContext(m_hContext);
        m_hContext = 0;
    }
    lReturn = ::SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &m_hContext);
    if(SCARD_S_SUCCESS != lReturn)
    {
#ifdef _DEBUGLOG
        sprintf(gszLogBuffer, "*** CPCSCManager::Initialize lRet=%ld", lReturn);
        DebugLogMessage(gszLogBuffer);
#endif
    }
    return lReturn;
}

SCARDCONTEXT CPCSCManager::GetContextHandle()
{
    return m_hContext;
}

long CPCSCManager::S_CardEstablishContext(CCardMessage *pMessage)
{
    long lRet = SCARD_F_INTERNAL_ERROR;

    if(m_hContext != 0 && m_pApplicationManager != NULL)
    {
        SCARDCONTEXT hContext = 0;
        long lConnID = 0;
        pMessage->Get("ConnID", lConnID);
        CApplicationObject *pApplication = NULL;
        if(NULL != (pApplication = m_pApplicationManager->FindExisting(lConnID)))
        {
            pApplication->AddRef(); 
        }
        else
        {
            // New Application
            pApplication = m_pApplicationManager->CreateApplication(pMessage, lConnID);
        }
        if(pApplication)
        {
            hContext = pApplication->GetContext();
#ifdef _DEBUGLOG
        sprintf(gszLogBuffer, "*** S_CardEstablishContext Connection %ld Context=%ld", lConnID, hContext);
        DebugLogMessage(gszLogBuffer);
#endif

            lRet = SCARD_S_SUCCESS;
        }
        pMessage->Clear(); 
        pMessage->Set("Context", hContext);
    }        
    return lRet;
}

long CPCSCManager::S_CardReleaseContext(CCardMessage *pMessage)
{
    long lRet = SCARD_F_INTERNAL_ERROR;

    if(m_hContext != 0 && m_pApplicationManager != NULL)
    {
        SCARDCONTEXT hContext = 0;
        long lConnID = 0;
        pMessage->Get("ConnID", lConnID);
        pMessage->Get("Context", (long *)&hContext);
        if(NULL != m_pApplicationManager->FindApplication(hContext))
        {
            m_pApplicationManager->DeleteApplication(hContext);
#ifdef _DEBUGLOG
        sprintf(gszLogBuffer, "*** S_CardReleaseContext Connection %ld Context=%ld", lConnID, hContext);
        DebugLogMessage(gszLogBuffer);
#endif
            lRet = SCARD_S_SUCCESS;
        }
    }        
    return lRet;
}

long CPCSCManager::S_CardConnect(CCardMessage *pMessage)
{
    long lRet = SCARD_F_INTERNAL_ERROR;

    if(m_hContext != 0 && m_pApplicationManager != NULL)
    {
        CApplicationObject *pApplication = NULL;
        long lConnID = 0;
        SCARDCONTEXT hContext = 0;
        pMessage->Get("Context", (long *)&hContext);
        pMessage->Get("ConnID", lConnID);
#ifdef _DEBUGLOG
        sprintf(gszLogBuffer, "*** S_CardConnect Connection %ld Context=%ld", lConnID, hContext);
        DebugLogMessage(gszLogBuffer);
#endif
        if(NULL != (pApplication = m_pApplicationManager->FindApplication(hContext)))
        {
            char szReader[256] = {0};
            pMessage->Get("Reader", szReader);
            CReaderObject *pReader = NULL;
            ItMapReaders it = m_Readers.find(szReader);
            if(it != m_Readers.end())
            {
                pReader = (*it).second;
            }
            if(pReader != NULL)
            {                
                if(pReader->GetCardHandle() == 0)
                {
                    EventCardState(SCARD_STATE_PRESENT, szReader);
                }
                if(pReader->GetCardHandle() > 0)
                {
                    // Card in reader
                    SCARDHANDLE hLocal = pApplication->AddReader(pReader);
#ifdef _DEBUGLOG
                    sprintf(gszLogBuffer, "*** S_CardConnect Connection %ld Handle=%ld", lConnID, hLocal);
                    DebugLogMessage(gszLogBuffer);
#endif
                    pMessage->Clear(); 
                    pMessage->Set("Card", hLocal);
                    pMessage->Set("ActiveProtocol", pReader->GetProtocol());                    
                    lRet = SCARD_S_SUCCESS;
                }
                else
                {
                    lRet = SCARD_E_NO_SMARTCARD;
                }
            }
            else
            {
                lRet = SCARD_E_UNKNOWN_READER;
            }
        }
    }
    return lRet;
}

long CPCSCManager::S_CardDisconnect(CCardMessage *pMessage)
{
    long lRet = SCARD_F_INTERNAL_ERROR;

    if(m_hContext != 0 && m_pApplicationManager != NULL)
    {
        CApplicationObject *pApplication = NULL;
        SCARDHANDLE hCard = 0;
        long lConnID = 0;
        pMessage->Get("Card", (long *)&hCard);
        pMessage->Get("ConnID", lConnID);
        if(NULL != (pApplication = m_pApplicationManager->FindExisting(lConnID)))
        {        
            // Card in reader
            pApplication->DeleteReader(hCard);
#ifdef _DEBUGLOG
            sprintf(gszLogBuffer, "*** S_CardDisconnect Connection %ld Handle=%ld", lConnID, hCard);
            DebugLogMessage(gszLogBuffer);
#endif
            lRet = SCARD_S_SUCCESS;
        }
    }
    return lRet;
}

long CPCSCManager::S_CardStatus(CCardMessage *pMessage)
{
    long lRet = SCARD_F_INTERNAL_ERROR;

    if(m_hContext != 0 && m_pApplicationManager != NULL)
    {
        CApplicationObject *pApplication = NULL;
        SCARDHANDLE hCard = 0;
        long lConnID = 0;
        pMessage->Get("Card", (long *)&hCard);
        pMessage->Get("ConnID", lConnID);
        if(NULL != (pApplication = m_pApplicationManager->FindExisting(lConnID)))
        {        
            // Card in reader
            CReaderObject *pReader = pApplication->GetReader(hCard);
            if(pReader != NULL)
            {
                char szReaderName[256] = {0};
                unsigned long cchLen = sizeof(szReaderName);
                unsigned long ulState = 0;
                unsigned long ulProtocol = 0;
                unsigned char ucAtr[32] = {0};
                unsigned long ulAtrLen = sizeof(ucAtr);
                SCARDHANDLE hReal = pApplication->GetOriginal(hCard);
                if(SCARD_S_SUCCESS == (lRet = SCardStatus(hReal, szReaderName, &cchLen, &ulState, &ulProtocol, ucAtr, &ulAtrLen)))
                {
                    pMessage->Clear(); 
                    pMessage->Set("ReaderName", szReaderName);
                    pMessage->Set("ReaderLen", cchLen);
                    pMessage->Set("State", ulState);
                    pMessage->Set("Protocol", ulProtocol);
                    pMessage->Set("Atr", ucAtr, ulAtrLen);
                    pMessage->Set("AtrLen", ulAtrLen);
                }
            }
        }
    }
    return lRet;
}

long CPCSCManager::S_CardListReaders(CCardMessage *pMessage)
{
    long lRet = SCARD_F_INTERNAL_ERROR;

    if(m_hContext != 0 && m_pApplicationManager != NULL)
    {
        SCARDCONTEXT hContext = 0;
        pMessage->Get("Context", (long *)&hContext);
        if(NULL != m_pApplicationManager->FindApplication(hContext))
        {
            char *pszGroups = NULL;
            unsigned char szGroups[256] = {0};
            int iGroupLen = pMessage->Get("Groups", szGroups, sizeof(szGroups));
            if(iGroupLen > 0)
            {
                memcpy(pszGroups, szGroups, iGroupLen);
            }
            char szReaders[256] = {0};
            unsigned long ulLen = sizeof(szReaders);
            if(SCARD_S_SUCCESS == (lRet = SCardListReaders(m_hContext, pszGroups, szReaders, &ulLen)))
            {
                pMessage->Clear(); 
                pMessage->Set("Readers", (unsigned char *)szReaders, ulLen);
                pMessage->Set("ReadersLen", ulLen);                
            }
#ifdef _DEBUGLOG
            sprintf(gszLogBuffer, "*** S_CardListReaders Context %ld ReturnValue=%ld", m_hContext, lRet);
            DebugLogMessage(gszLogBuffer);
#endif
        }
    }
    return lRet;
}

long CPCSCManager::S_CardGetStatusChange(CCardMessage *pMessage)
{
    long lRet = SCARD_F_INTERNAL_ERROR;

    if(m_hContext != 0 && m_pApplicationManager != NULL)
    {
        SCARDCONTEXT hContext = 0;
        pMessage->Get("Context", (long *)&hContext);
        if(NULL != m_pApplicationManager->FindApplication(hContext))
        {
            long ulTimeout = 0;
            unsigned long ulReaders = 0;
            pMessage->Get("Timeout", ulTimeout);
            pMessage->Get("ReadersLen", (long *)&ulReaders);
            SCARD_READERSTATE_A *prgReaderStates = new SCARD_READERSTATE_A[ulReaders];
            memset(prgReaderStates, 0, sizeof(SCARD_READERSTATE_A) * ulReaders);
            char szReaders[MAXIMUM_SMARTCARD_READERS][64] = {0};
            for(unsigned int i = 0; i < ulReaders; ++i)
            {
                char szReader[16] = {0};
                char szState[16] = {0};
                sprintf(szReader, "Reader%d", i); 
                sprintf(szState, "CurrentState%d", i);                
                pMessage->Get(szReader, (unsigned char *)szReaders[i], 64);
                prgReaderStates[i].szReader = szReaders[i];
                long lTemp = 0;
                if(pMessage->Get(szState, lTemp))
                {
                    prgReaderStates[i].dwCurrentState = lTemp;
                }
            }
            if(ulTimeout > 0)
            {
                ulTimeout = 0;
            }
            lRet = SCardGetStatusChange(m_hContext, ulTimeout, prgReaderStates, ulReaders);
            if(SCARD_S_SUCCESS == lRet)// || SCARD_E_TIMEOUT == lRet)
            {
                pMessage->Clear(); 
                for(unsigned int i = 0; i < ulReaders; ++i)
                {
                    char szEventstate[16] = {0};
                    char szAtrLen[16] = {0};
                    char szAtr[16] = {0};
                    sprintf(szEventstate, "EventState%d", i); 
                    sprintf(szAtrLen, "AtrLen%d", i); 
                    sprintf(szAtr, "Atr%d", i); 
                    pMessage->Set(szEventstate, prgReaderStates[i].dwEventState);  
                    pMessage->Set(szAtrLen, prgReaderStates[i].cbAtr);  
                    pMessage->Set(szAtr, prgReaderStates[i].rgbAtr, prgReaderStates[i].cbAtr);  
                }
            }
#ifdef _DEBUGLOG
            sprintf(gszLogBuffer, "*** SCardGetStatusChange Context %ld ReturnValue=%ld", m_hContext, lRet);
            DebugLogMessage(gszLogBuffer);
#endif
            if(prgReaderStates != NULL)
            {
                delete prgReaderStates;
            }
        }
    }
    return lRet;
}

long CPCSCManager::S_CardBeginTransaction(CCardMessage *pMessage)
{
    long lRet = SCARD_F_INTERNAL_ERROR;

    if(m_hContext != 0 && m_pApplicationManager != NULL)
    {
        CApplicationObject *pApplication = NULL;
        SCARDHANDLE hCard = 0;
        long lConnID = 0;
        pMessage->Get("Card", (long *)&hCard);
        pMessage->Get("ConnID", lConnID);
        if(NULL != (pApplication = m_pApplicationManager->FindExisting(lConnID)))
        {        
            // Card in reader
            CReaderObject *pReader = pApplication->GetReader(hCard);
            if(pReader != NULL && m_pTransActionStack != NULL)
            {
#ifdef _DEBUGLOG
        sprintf(gszLogBuffer, "*** S_CardBeginTransaction Connection %ld Handle=%ld", lConnID, hCard);
        DebugLogMessage(gszLogBuffer);
#endif
                m_pTransActionStack->AddHandle(hCard);
                int iAvail = 0;
                QWaitCondition oWait;
                while ((0 == (iAvail = m_pTransActionStack->IsHandleAvailable(hCard))) && pApplication->MatchOriginal(hCard))
                {
                    oWait.wait(1000);
                    if(m_pConnMgr)
                    {
                        CConnection *pConn = m_pConnMgr->GetConnection(lConnID);
                        if(pConn && !pConn->isValid())
                        {
                            iAvail = -1;
                            break;
                        }
                    }
                }
                if(iAvail > 0)
                {
                    SCARDHANDLE hReal = pApplication->GetOriginal(hCard);
#ifdef _DEBUGLOG
                    sprintf(gszLogBuffer, "*** REAL S_CardBeginTransaction Connection %ld Handle=%ld", lConnID, hReal);
                    DebugLogMessage(gszLogBuffer);
#endif
                    lRet = SCardBeginTransaction(hReal);
                }
                else
                {
                    m_pTransActionStack->DeleteAllHandle(hCard); 
                }
            }
        }
    }
    return lRet;
}


long CPCSCManager::S_CardEndTransaction(CCardMessage *pMessage)
{
    long lRet = SCARD_F_INTERNAL_ERROR;

    if(m_hContext != 0 && m_pApplicationManager != NULL)
    {
        CApplicationObject *pApplication = NULL;
        SCARDHANDLE hCard = 0;
        long lConnID = 0;
        pMessage->Get("Card", (long *)&hCard);
        pMessage->Get("ConnID", lConnID);
        if(NULL != (pApplication = m_pApplicationManager->FindExisting(lConnID)))
        {        
            // Card in reader
            CReaderObject *pReader = pApplication->GetReader(hCard);
            if(pReader != NULL && m_pTransActionStack != NULL)
            {
#ifdef _DEBUGLOG
        sprintf(gszLogBuffer, "*** S_CardEndTransaction Connection %ld Handle=%ld", lConnID, hCard);
        DebugLogMessage(gszLogBuffer);
#endif
                int iAvail = 0;
                QWaitCondition oWait;
                while ((0 == (iAvail = m_pTransActionStack->IsHandleAvailable(hCard))) && pApplication->MatchOriginal(hCard))
                {
                    oWait.wait(1000);
                    if(m_pConnMgr)
                    {
                        CConnection *pConn = m_pConnMgr->GetConnection(lConnID);
                        if(pConn && !pConn->isValid())
                        {
                            iAvail = -1;
                            break;
                        }
                    }
                }            

                if(iAvail > 0)
                {
                    SCARDHANDLE hReal = pApplication->GetOriginal(hCard);
#ifdef _DEBUGLOG
                    sprintf(gszLogBuffer, "*** REAL S_CardEndTransaction Connection %ld Handle=%ld", lConnID, hReal);
                    DebugLogMessage(gszLogBuffer);
#endif
                    lRet = SCardEndTransaction(hReal, SCARD_LEAVE_CARD);
                    m_pTransActionStack->DeleteHandle(hCard);
                }
                else
                {
                    m_pTransActionStack->DeleteAllHandle(hCard); 
                }
            }
        }
    }
    return lRet;
}

long CPCSCManager::S_CardTransmit(CCardMessage *pMessage)
{
    long lRet = SCARD_F_INTERNAL_ERROR;

    if(m_hContext != 0 && m_pApplicationManager != NULL)
    {
        CApplicationObject *pApplication = NULL;
        SCARDHANDLE hCard = 0;
        long lConnID = 0;
        pMessage->Get("Card", (long *)&hCard);
        pMessage->Get("ConnID", lConnID);
        if(NULL != (pApplication = m_pApplicationManager->FindExisting(lConnID)))
        {        
            // Card in reader
            CReaderObject *pReader = pApplication->GetReader(hCard);
            if(pReader != NULL && m_pTransActionStack != NULL)
            {
                if(m_pTransActionStack->IsHandleAvailable(hCard) > 0)
                {
#ifdef _DEBUGLOG
        sprintf(gszLogBuffer, "*** S_CardTransmit Connection %ld Handle=%ld", lConnID, hCard);
        //DebugLogMessage(gszLogBuffer);
#endif
                    SCARDHANDLE hReal = pApplication->GetOriginal(hCard);
                    SCARD_IO_REQUEST ioSend = {0};
                    SCARD_IO_REQUEST ioRecv = {0};
                    unsigned long ulSendLen = 0;
                    unsigned long ulRecvLen = 0;
                    pMessage->Get("SendBufferLen", (long *)&ulSendLen);
                    BYTE *pSendBuffer = new BYTE[ulSendLen];
                    memset(pSendBuffer, 0, ulSendLen);
                    pMessage->Get("SendBuffer", (BYTE *)pSendBuffer, ulSendLen);
                    pMessage->Get("SendProtocol", (long *)&ioSend.dwProtocol);
                    pMessage->Get("SendProtocolLen", (long *)&ioSend.cbPciLength);
                    SCARD_IO_REQUEST *pioRecv = NULL;
                    if(pMessage->Get("RecvProtocol", (long *)&ioRecv.dwProtocol))
                    {
                        pMessage->Get("RecvProtocolLen", (long *)&ioRecv.cbPciLength);
                        pioRecv = &ioRecv;
                    }

                    if(!pMessage->Get("RecvLen", (long *)&ulRecvLen))
                    {
                        ulRecvLen = 256;
                    }
                    BYTE *pRecvBuffer = new BYTE[ulRecvLen];
                    memset(pRecvBuffer, 0, ulRecvLen);
/*#ifdef _DEBUGLOG
        char szBuffer[8192] = {0};
        std::string strTemp;
        for (unsigned int x = 0; x < ulSendLen; x++)
        {
            char szBuffer2[2] = {0};
            sprintf(szBuffer2, "%02X", pSendBuffer[x]);
            strTemp += szBuffer2;
        }
        sprintf(szBuffer, "*** S_CardTransmit Handle=%ld, SendLen=%ld, RecvLen=%ld, sendProto=%ld, SendData=%s", hReal, ulSendLen, ulRecvLen, ioSend.dwProtocol, strTemp.c_str());
        DebugLogMessage(szBuffer);
#endif
*/
                    if(SCARD_S_SUCCESS == (lRet = SCardTransmit(hReal, &ioSend, pSendBuffer, ulSendLen, NULL, pRecvBuffer, &ulRecvLen)))
                    {
                        pMessage->Clear(); 
                        if(pioRecv != NULL)
                        {
                            pMessage->Set("RecvProtocol", ioRecv.dwProtocol);
                            pMessage->Set("RecvProtocolLen", ioRecv.cbPciLength);
                        }
                        pMessage->Set ("RecvLen", ulRecvLen);
                        pMessage->Set ("RecvBuffer", (BYTE *)pRecvBuffer, ulRecvLen);
                    }
/*
#ifdef _DEBUGLOG
         strTemp = "";
        for (x = 0; x < ulRecvLen; x++)
        {
            char szBuffer2[2] = {0};
            sprintf(szBuffer2, "%02X", pRecvBuffer[x]);
            strTemp += szBuffer2;
        }
        sprintf(szBuffer, "*** S_CardTransmit Handle=%ld, SendLen=%ld, RecvLen=%ld, recvdata=%s", hReal, ulSendLen, ulRecvLen, strTemp.c_str());
        DebugLogMessage(szBuffer);
#endif
*/        
                    if(pSendBuffer != NULL)
                    {
                        delete pSendBuffer;
                    }
                    if(pRecvBuffer != NULL)
                    {
                        delete pRecvBuffer;
                    }
                }
            }
        }
    }
#ifdef _DEBUGLOG
    if(SCARD_S_SUCCESS != lRet)
    {
        sprintf(gszLogBuffer, "*** S_CardTransmit Returned=%ld", lRet);
        DebugLogMessage(gszLogBuffer);
    }
#endif
    return lRet;
}

long CPCSCManager::S_CardControl(CCardMessage *pMessage)
{
    long lRet = SCARD_F_INTERNAL_ERROR;

    if(m_hContext != 0 && m_pApplicationManager != NULL)
    {
        CApplicationObject *pApplication = NULL;
        SCARDHANDLE hCard = 0;
        long lConnID = 0;
        pMessage->Get("Card", (long *)&hCard);
        pMessage->Get("ConnID", lConnID);
        if(NULL != (pApplication = m_pApplicationManager->FindExisting(lConnID)))
        {        
            // Card in reader
            CReaderObject *pReader = pApplication->GetReader(hCard);
            if(pReader != NULL && m_pTransActionStack != NULL)
            {
                if(m_pTransActionStack->IsHandleAvailable(hCard) > 0)
                {
                    SCARDHANDLE hReal = pApplication->GetOriginal(hCard);
                    BYTE *pSendBuffer = NULL;
                    unsigned long ulSendLen = 0;
                    unsigned long ulRecvLen = 0;
                    unsigned long ulControlCode = 0;
                    pMessage->Get("ControlCode", (long *)&ulControlCode);
                    if(pMessage->Get("SendBufferLen", (long *)&ulSendLen))
                    {
                        pSendBuffer = new BYTE[ulSendLen];
                        memset(pSendBuffer, 0, ulSendLen);
                        pMessage->Get("SendBuffer", (BYTE *)pSendBuffer, ulSendLen);
                    }

                    if(!pMessage->Get("RecvLen", (long *)&ulRecvLen))
                    {
                        ulRecvLen = 256;
                    }
                    BYTE *pRecvBuffer = new BYTE[ulRecvLen];
                    memset(pRecvBuffer, 0, ulRecvLen);
                    unsigned long ulBytesRet = 0;
                    if(SCARD_S_SUCCESS == (lRet = SCardControl(hReal, ulControlCode, pSendBuffer, ulSendLen, pRecvBuffer, ulRecvLen, &ulBytesRet)))
                    {
                        pMessage->Clear(); 
                        pMessage->Set("RecvLen", ulBytesRet);
                        pMessage->Set("RecvBuffer", (BYTE *)pRecvBuffer, ulBytesRet);
                    }
                    if(pSendBuffer != NULL)
                    {
                        delete pSendBuffer;
                    }
                    if(pRecvBuffer != NULL)
                    {
                        delete pRecvBuffer;
                    }
                }
            }
        }
    }
    return lRet;
}


long CPCSCManager::S_CardControlPinPad(CCardMessage *pMessage)
{
    long lRet = SCARD_F_INTERNAL_ERROR;
#ifdef BELPIC_PIN_PAD

    if(m_hContext != 0 && m_pApplicationManager != NULL)
    {
        CApplicationObject *pApplication = NULL;
        SCARDHANDLE hCard = 0;
        long lConnID = 0;
        pMessage->Get("Card", (long *)&hCard);
        pMessage->Get("ConnID", lConnID);
        if(NULL != (pApplication = m_pApplicationManager->FindExisting(lConnID)))
        {        
            // Card in reader
            CReaderObject *pReader = pApplication->GetReader(hCard);
            if(SCR_CARD_HANDLE == hCard || (pReader != NULL && m_pTransActionStack != NULL))
            {
                if(SCR_CARD_HANDLE == hCard || m_pTransActionStack->IsHandleAvailable(hCard) > 0)
                {
                    SCARDHANDLE hReal = pApplication->GetOriginal(hCard);
                    pMessage->Set("OriginalHandle", hReal);
                    CPinPadEvent *ppEvent = new CPinPadEvent();
                    unsigned int iSerSize = pMessage->GetSerializeSize();
                    byte *pSerBytes = new byte[iSerSize]; 
                    pMessage->Serialize(pSerBytes, iSerSize); 
                    ppEvent->SetData((const char *)pSerBytes, iSerSize);
                    if(m_pMainDlg)
                    {
                        QApplication::postEvent(m_pMainDlg, ppEvent);
                    }
                    pMessage->Clear();
                    pMessage->Set("ReturnLater", 1);
                    if(pSerBytes)
                    {
                        delete [] pSerBytes;
                    }
                    lRet = SCARD_S_SUCCESS;
                }
            }
        }
    }
#endif // BELPIC_PIN_PAD
    return lRet;
}

void CPCSCManager::EventCardState(unsigned long ulState, const char *pszReaderName)
{
#ifdef _DEBUGLOG
        DebugLogMessage("Eventcardstate");
#endif
    SCARDHANDLE hCard = 0;
    unsigned long lReturn;
    unsigned long ulAP;
    CReaderObject *pReader = NULL;
    ItMapReaders it = m_Readers.find(pszReaderName);
    if(it != m_Readers.end())
    {
        pReader = (*it).second;
    }
    if(pReader == NULL)
    {
        pReader = new CReaderObject();
        m_Readers[pszReaderName] = pReader;
        pReader->SetName((char *)pszReaderName); 
    }
    if(ulState == SCARD_STATE_PRESENT && pReader->GetCardHandle() == 0)
    {
        // Card Inserted -> Connect exclusive
        lReturn = SCardConnect(m_hContext, pszReaderName, SCARD_SHARE_EXCLUSIVE, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1 , &hCard, &ulAP);
        if(lReturn == SCARD_E_SHARING_VIOLATION)
        {
            // Try again Shared access
            lReturn = SCardConnect(m_hContext, pszReaderName, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1 , &hCard, &ulAP);
        }
        if(lReturn == SCARD_S_SUCCESS)
        {
            pReader->SetCardHandle(hCard);
            pReader->SetProtocol(ulAP); 
#ifdef _DEBUGLOG
        sprintf(gszLogBuffer, "*** SCardConnect Connection Handle=%ld", hCard);
        DebugLogMessage(gszLogBuffer);
#endif
        }

    }
    else if(ulState == SCARD_STATE_EMPTY && pReader->GetCardHandle() > 0)
    {
        // Card pulled out -> Reinit everything
#ifndef _WIN32
            SCardDisconnect(pReader->GetCardHandle (), SCARD_LEAVE_CARD);
#endif
#ifdef _DEBUGLOG
        sprintf(gszLogBuffer, "*** SCardDisconnect Connection Handle=%ld", pReader->GetCardHandle ());
        DebugLogMessage(gszLogBuffer);
#endif
        pReader->SetCardHandle(hCard);
        pReader->SetProtocol(0);
        /* @@@ */
        std::vector<long> oDummyHandles;
        m_pApplicationManager->DeleteApplicationReaders(pReader, oDummyHandles);
        for(unsigned int i = 0; i < oDummyHandles.size(); ++i)
        {
            m_pTransActionStack->DeleteAllHandle(oDummyHandles[i]);
        }
        pReader->ClearRefCount(); 
    }
}

BOOL CPCSCManager::Suspend()
{
    CReaderObject *pObject = NULL;
    ItMapReaders it;
    for(it = m_Readers.begin(); it != m_Readers.end(); ++it)
    {
	    pObject = (*it).second;
        if(pObject && pObject->GetCardHandle () > 0)
        {
            // Disconnect Card
            SCardDisconnect(pObject->GetCardHandle (), SCARD_LEAVE_CARD);
            pObject->SetCardHandle(0);
            pObject->SetProtocol(0);
        }
    }
    return TRUE;
}

/*BOOL CPCSCManager::CheckAccess(CApplicationObject* pApplication, BYTE *pSendBuffer, unsigned long ulSendLen)
{
    BOOL bRet = TRUE;

    if(ulSendLen > 1 && 0 == memcmp(pSendBuffer, ucSelect, 2) && pSendBuffer[4] > 0x05)
    {
        if(0 == memcmp(pSendBuffer + 5, ucFile, 5))
        {
            int iFile = pSendBuffer[10];
            switch(iFile)
            {
            case ucID:
            ::MessageBox(NULL, "ID read", "Hallo", MB_OK); 
                    break;
            case ucAddress:
            ::MessageBox(NULL, "Address read", "Hallo", MB_OK); 
                break;
            case ucPhoto:
            ::MessageBox(NULL, "Photo read", "Hallo", MB_OK); 
                break;
            }
        }
    }
    return  bRet;
}
*/
