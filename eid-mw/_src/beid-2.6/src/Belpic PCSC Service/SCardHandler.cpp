// SCardHandler.cpp: implementation of the CSCardHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "SCardHandler.h"
#include "PCSCManager.h"
#include "../beidcommon/config.h"

using namespace EIDCommLIB;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

typedef  long (CPCSCManager::* S_CardFunction)(EIDCommLIB::CCardMessage *);

typedef struct
{
    char szFuncName[64];
    S_CardFunction funcPtr;
} SCardFunctionInfo;

static SCardFunctionInfo tFuncMap[] = 
{
	{ "SCardTransmit", &CPCSCManager::S_CardTransmit },
	{ "SCardBeginTransaction", &CPCSCManager::S_CardBeginTransaction },
	{ "SCardEndTransaction", &CPCSCManager::S_CardEndTransaction },
	{ "SCardGetStatusChange", &CPCSCManager::S_CardGetStatusChange },
    { "SCardEstablishContext", &CPCSCManager::S_CardEstablishContext },
	{ "SCardReleaseContext", &CPCSCManager::S_CardReleaseContext },
	{ "SCardConnect", &CPCSCManager::S_CardConnect },
	{ "SCardDisconnect", &CPCSCManager::S_CardDisconnect },
	{ "SCardStatus", &CPCSCManager::S_CardStatus },
	{ "SCardListReaders", &CPCSCManager::S_CardListReaders },
	{ "SCardControl", &CPCSCManager::S_CardControl },
	{ "SCardPinPadControl", &CPCSCManager::S_CardControlPinPad },
	{ 0, 0 }
};
    
CSCardHandler::CSCardHandler()
{
    m_pPCSCManager = NULL;
    eidcommon::CConfig oConfig;
    oConfig.Load();
    m_IPs = oConfig.GetAllowedIPAddress(); 
}

CSCardHandler::~CSCardHandler()
{
}

void CSCardHandler::Execute(CCardMessage *pMessage)
{
	long lReturn = SCARD_F_UNKNOWN_ERROR;
    if (m_pConnection == NULL)
	{
		// Error
	    pMessage->Set("Return", lReturn);
		return;
	}

    // Check IP Allowed
   if(SCARD_S_SUCCESS == (lReturn = CheckIPAllowed()))
   {
        long lConnID = m_pConnection->GetConnectionID();
        pMessage->Set("ConnID", lConnID);
        lReturn = SCardMapFunction(pMessage);
   }

    pMessage->Set("Return", lReturn);
}

long CSCardHandler::SCardMapFunction(CCardMessage *pMessage)
{
    long lReturn = SCARD_E_CARD_UNSUPPORTED;
    
	if (m_pPCSCManager == NULL)
	{
		return lReturn;
	}

    std::string strFuncName;
    if(pMessage->Get("FunctionName", strFuncName))
    {
        S_CardFunction pFunc = 0;

        int iFuncCount = sizeof(tFuncMap)/sizeof(tFuncMap[0]);
        int i;
	    for (i = 0; i < iFuncCount - 1; i++)
        {
		    if (strFuncName == tFuncMap[i].szFuncName)
            {
                pFunc = tFuncMap[i].funcPtr; 
                break;
		    }
        }
        if(pFunc != NULL)
        {
#ifdef _DEBUGLOG
        long lConnID = 0;
        char szBuffer[256] = {0};
        pMessage->Get("ConnID", lConnID);
        sprintf(szBuffer, "Connection %ld Func=%s", lConnID, tFuncMap[i].szFuncName);
        DebugLogMessage(szBuffer);
#endif
            lReturn = (m_pPCSCManager->*pFunc)(pMessage);
        }
    }
    return lReturn;
}

long CSCardHandler::CheckIPAllowed()
{
    long lReturn = SCARD_E_NO_ACCESS;
    if(m_pConnection)
    {
        QSocketDevice *pSocket = m_pConnection->GetSocket();
        if(pSocket != NULL)
        {
            QString strAddress = pSocket->peerAddress().toString();
            if(strAddress.length() > 0 )
            {
                if(strAddress == "127.0.0.1")
                {
                    lReturn = SCARD_S_SUCCESS;
                }
#ifndef _PRIVACY_REMOTE_SVC
                else
                {
                    for ( QStringList::Iterator it = m_IPs.begin(); it != m_IPs.end(); ++it ) 
                    {
                        QRegExp regExp(*it, false, true); 
                        if(strAddress.contains(regExp) > 0)
                        {
                            lReturn = SCARD_S_SUCCESS;
                            break;
                        }                
                    }
                }
#endif
            }
        }
    }
    return lReturn;
}

void CSCardHandler::SetPCSCManager(CPCSCManager *pManager)
{
    m_pPCSCManager = pManager;
}

CMessageHandler *CSCardHandler::Clone()
{
    CSCardHandler *pHandler = new CSCardHandler();
    pHandler->SetPCSCManager(m_pPCSCManager);
    return pHandler;
}
