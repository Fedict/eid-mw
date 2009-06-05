// CardChangeMonitor.cpp: implementation of the CCardChangeMonitor class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CardChangeMonitor.h"
#include "PCSCManager.h"
#include "ReaderState.h"
#include "../beidcommon/beidconst.h"

#ifdef _DEBUGLOG
    static char gszLogBufferMon[256] = {0};
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCardChangeMonitor::CCardChangeMonitor()
{
    m_pPCSCManager = NULL;
    m_bCanSuspend = false;
    m_bSuspend = false;
}

CCardChangeMonitor::~CCardChangeMonitor()
{
    CReaderState *pObject = NULL;
    ItMapReaderState it;
    for(it = m_readerStates.begin(); it != m_readerStates.end(); ++it)
    {
	    pObject = (*it).second;
        if(pObject)
        {
            delete pObject;
        }
    }
    m_readerStates.clear();
}

void CCardChangeMonitor::SetPCSCManager(CPCSCManager *pManager)
{
    m_pPCSCManager = pManager;
}

void CCardChangeMonitor::run()
{
    while(!m_bSuspend)
    {      
        m_bCanSuspend = false;
        SCARDCONTEXT hContext = 0;
        if(m_pPCSCManager != NULL)
        {
            hContext = m_pPCSCManager->GetContextHandle();
        }
        if(hContext == 0)
        {
            m_pPCSCManager->Initialize(); 
            hContext = m_pPCSCManager->GetContextHandle();
        }

        if(hContext != 0)
        {
            SCARD_READERSTATE_A rgscState[MAXIMUM_SMARTCARD_READERS] = {0};
            long  lReturn;
            int iCount = 0;
            int i, j;
            unsigned long cchReaders = 0;

            // Determine which readers are available.
            char *pszReaders = NULL;
            lReturn = SCardListReaders(hContext, NULL, NULL,  &cchReaders );
            if ( SCARD_S_SUCCESS == lReturn )
            {
                pszReaders = new char[cchReaders];
                memset(pszReaders, 0, cchReaders);
                lReturn = SCardListReaders(hContext, NULL, pszReaders,  &cchReaders );
            }

            if ( SCARD_S_SUCCESS != lReturn )
            {
#ifdef _DEBUGLOG
            sprintf(gszLogBufferMon, "*** CCardChangeMonitor SCardListReaders error=%ld", lReturn);
            DebugLogMessage(gszLogBufferMon);
#endif
                if(pszReaders)
                {
                    delete [] pszReaders;
                }
                m_pPCSCManager->Initialize(); 
                SetUnawareState();
                m_bCanSuspend = true;
                m_oWait.wait(3000);
                continue;
            }

            // Place the readers into the state array.
            char *pszRdr = pszReaders;
            for ( i = 0; i < MAXIMUM_SMARTCARD_READERS; i++ )
            {
                if ( 0 == *pszRdr )
                    break;
                CReaderState *pReaderState = NULL;
                ItMapReaderState it = m_readerStates.find(pszRdr);
                if(it != m_readerStates.end())
                {
                    pReaderState =(*it).second;
                }
                else
                {
                    pReaderState = new CReaderState();
                    pReaderState->SetState(SCARD_STATE_UNAWARE);
                    m_readerStates[pszRdr] = pReaderState;
                }
                rgscState[i].szReader = pszRdr;
                rgscState[i].dwCurrentState = SCARD_STATE_UNAWARE;
       		    rgscState[i].dwEventState = SCARD_STATE_UNAWARE;
                pszRdr += strlen(pszRdr) + 1;
            }
            iCount = i;
        
            // If any readers are available, proceed.
            if ( iCount > 0 )
            {
                lReturn = SCardGetStatusChange(hContext, 0, rgscState, iCount );
                if ( SCARD_S_SUCCESS == lReturn )
                {
                    for (j = 0; j < iCount; j++)
                    {
                        CReaderState *pReaderState = NULL;
                        ItMapReaderState it = m_readerStates.find((char *)rgscState[j].szReader);
                        if(it != m_readerStates.end())
                        {
                            pReaderState = (*it).second;
                            unsigned long ulState = pReaderState->GetState();
                            if((ulState ==  SCARD_STATE_UNAWARE || ulState ==  SCARD_STATE_EMPTY) && SCARD_STATE_PRESENT & rgscState[j].dwEventState)
                            {
                                pReaderState->SetState(SCARD_STATE_PRESENT);
#ifndef _PRIVACY_REMOTE_SVC
                                // Card Inserted
                                if(MatchAtr(rgscState[j].rgbAtr, rgscState[j].cbAtr, rgscState[j].szReader))
                                {
                                    m_pPCSCManager->EventCardState(SCARD_STATE_PRESENT, rgscState[j].szReader);
                                }
#endif
                            }
                            else if((ulState ==  SCARD_STATE_PRESENT || ulState ==  SCARD_STATE_UNAWARE) && SCARD_STATE_EMPTY & rgscState[j].dwEventState)
                            {
                                // Card Removed
                                pReaderState->SetState(SCARD_STATE_EMPTY); 
                                m_pPCSCManager->EventCardState(SCARD_STATE_EMPTY, rgscState[j].szReader);
                            }
                        }
                    }
                }
            }
            delete [] pszReaders;
        }
        else
        {
            SetUnawareState();
            m_bCanSuspend = true;
            m_oWait.wait(3000);
            continue;
        }
        m_bCanSuspend = true;
        m_oWait.wait(100);
    }
}

bool CCardChangeMonitor::MatchAtr(BYTE *atr, DWORD atr_len, const char *pszReaderName)
{
	for (int i = 0; BelpicAtrs[i].atr != NULL; i++) 
    {
		if (BelpicAtrs[i].atr_len != atr_len)
			continue;
		if (memcmp(BelpicAtrs[i].atr, atr, atr_len) != 0)
			continue;
		return true;
	}

    return SelectApplication(pszReaderName);
}

void CCardChangeMonitor::Suspend()
{
    QWaitCondition oDummyWait;

    m_bSuspend = true;
    m_oWait.wakeOne();

    while (running())
    {
        oDummyWait.wait(100);
    }

    if(m_pPCSCManager != NULL)
    {
        m_pPCSCManager->Suspend(); 
    }

    CReaderState *pObject = NULL;
    ItMapReaderState it;
    for(it = m_readerStates.begin(); it != m_readerStates.end(); ++it)
    {
	    pObject = (*it).second;
        if(pObject)
        {
            pObject->SetState(SCARD_STATE_UNAWARE); 
        }
    }
}

void CCardChangeMonitor::Resume()
{
    m_bSuspend = false;
}

bool CCardChangeMonitor::SelectApplication(const char *pszReaderName)
{
    bool bRet = false;
    tAtrTable tAID = {(const BYTE *)"\x00\xA4\x04\x0C\x0C\xA0\x00\x00\x01\x77\x50\x4B\x43\x53\x2D\x31\x35", 17};
    tAtrTable tMF = {(const BYTE *)"\x00\xA4\x02\x0C\x02\x3F\x00", 7};

    SCARDCONTEXT hContext = 0;
    SCARDHANDLE hCard = 0;
    unsigned long lReturn;
    unsigned long ulAP;
    if(m_pPCSCManager != NULL)
    {
        hContext = m_pPCSCManager->GetContextHandle();
    }
    lReturn = SCardConnect(hContext, pszReaderName, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1 , &hCard, &ulAP);
    if(lReturn == SCARD_S_SUCCESS)
    {
        SCARD_IO_REQUEST tSendPci = {0};
        tSendPci.dwProtocol = ulAP;
        tSendPci.cbPciLength = sizeof(tSendPci);
        unsigned long ulRecvLen = 2;        
        BYTE ucRecvBuffer[2] = {0};

         if(SCARD_S_SUCCESS == SCardTransmit(hCard, &tSendPci, tMF.atr, tMF.atr_len, NULL, ucRecvBuffer, &ulRecvLen))
         {
             if(ucRecvBuffer[0] == 0x90 && ucRecvBuffer[1] == 0x00)
             {
                ulRecvLen = 2;
                memset(ucRecvBuffer, 0, sizeof(ucRecvBuffer));
                if(SCARD_S_SUCCESS == SCardTransmit(hCard, &tSendPci, tAID.atr, tAID.atr_len, NULL, ucRecvBuffer, &ulRecvLen))
                {
                     if(ucRecvBuffer[0] == 0x90 && ucRecvBuffer[1] == 0x00)
                     {
                        bRet = true;
                     }
                }
             }
         }
         SCardDisconnect(hCard, SCARD_LEAVE_CARD);
    }
    return bRet;
}

void CCardChangeMonitor::SetUnawareState()
{
    CReaderState *pObject = NULL;
    ItMapReaderState it;
    for(it = m_readerStates.begin(); it != m_readerStates.end(); ++it)
    {
	    pObject = (*it).second;
        if(pObject)
        {
            pObject->SetState(SCARD_STATE_UNAWARE); 
        }
    }
    if(m_pPCSCManager != NULL)
    {
        m_pPCSCManager->Suspend(); 
    }
}

 void CCardChangeMonitor::StopRunning()
 {
    m_bSuspend = true;
    m_oWait.wakeOne();
}
