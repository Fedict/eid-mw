// winscarp.cpp : Defines the entry point for the DLL application.
//
#include <qapplication.h>
#ifndef _WIN32
    #include <winscard.h>
#else
    #include <process.h>
#endif

#include "winscarp.h"

#include "../beidcommon/config.h"
#include "ServerPoll.h"
#include "../beidcommon/scr.h"
#include "../beidcommon/TLVBuffer.h"
#include "../beidcommlib/beidcommlib.h"
#include <qlibrary.h>
#include <qdir.h>
#include <memory>
#include <qdatetime.h> 

std::auto_ptr<QApplication> gMyApp;

// Function prototypes
bool Initialize();
bool Finalize();

class CStartLoader  
{
public:
    CStartLoader() { Initialize();}
    virtual ~CStartLoader() { Finalize(); }
};

///////////
//#define _DEBUGLOGTIME
//#define _DEBUGLOG

#ifdef _DEBUGLOGTIME
void DebugLogMessage(const std::string & strMsg)
{
    QFile oFile(QDir::homeDirPath() + "/beidwinscard.log");
    if(oFile.open(IO_WriteOnly | IO_Append))
    {
        std::string strTemp(strMsg);
        strTemp += "\r\n";
        oFile.writeBlock(strTemp.c_str(), strTemp.length());
        oFile.close();
    }
}
QTime t;

#endif


#ifdef _WIN32
    static const SCARD_IO_REQUEST g_rgSCardT0Pci = {1, 8};
    static const SCARD_IO_REQUEST g_rgSCardT1Pci = {2, 8};
    static const SCARD_IO_REQUEST g_rgSCardRawPci = {65536, 8};
    #define SCARD_W_INSERTED_CARD           0x8010006A
#else
    #define SCARD_AUTOALLOCATE (DWORD)(-1)
   
    typedef struct _GUID {          // size is 16
        DWORD Data1;
        WORD   Data2;
        WORD   Data3;
        BYTE  Data4[8];
    } GUID;

    typedef const GUID *LPCGUID;
    typedef GUID *LPGUID;
    typedef char *LPWSTR;
    typedef const char *LPCWSTR;
    typedef SCARD_READERSTATE_A *LPSCARD_READERSTATE_W;
    typedef const char *LPCSTR;
#endif //_WIN32

#ifdef BELPIC_PIN_PAD
    #include "../beidcommon/scr.h"
    #include "../beidcommon/pinpad.h"
    CPinPad *gpPinPad = NULL;
#endif // BELPIC_PIN_PAD


using namespace EIDCommLIB;
using namespace eidcommon;

//#ifdef _WIN32
//    #ifdef _DEBUG
//    #pragma comment(lib, "../beidcommlib/Debug/beidcommlib")
//    #else
//    #pragma comment(lib, "../beidcommlib/Release/beidcommlib")
//    #endif
//#endif

// Global Variables
#ifndef _WIN32
static CStartLoader gLoader;
#endif
bool gbServerMode = false;
CConnectionManager *gpConnClientMan = NULL;
CConnection *gpConnectionClient = NULL;
CServerPoll *gpServerPoll = NULL;
CConfig *gpConfig = NULL;
QLibrary *gpWinScardLoader = NULL;

///////////////////////////////////////////////////////
// Original Winscard functions
#ifndef WINSCARDAPI
    #define WINSCARDAPI
#endif

#ifdef _WIN32
    #ifndef WINAPI
        #define WINAPI  __stdcall
    #endif
    #define PCSCNAME "winscard" 
    #define SCARDCONNECTFUNC "SCardConnectA"
    #define SCARDGETSTATUSCHANGEFUNC "SCardGetStatusChangeA"
    #define SCARDLISTREADERSFUNC "SCardListReadersA"
    #define SCARDGETSTATUSFUNC "SCardStatusA"
#else
    #define WINAPI
    #define PCSCNAME "pcsclite" 
    #define SCARDCONNECTFUNC "SCardConnect"
    #define SCARDGETSTATUSCHANGEFUNC "SCardGetStatusChange"
    #define SCARDLISTREADERSFUNC "SCardListReaders"
    #define SCARDGETSTATUSFUNC "SCardStatus"
#endif

#ifndef IN
    #define IN
#endif

#ifndef OUT
    #define OUT
#endif

typedef long (WINAPI *SCTransmit)(SCARDHANDLE,LPCSCARD_IO_REQUEST,LPCBYTE,DWORD,LPSCARD_IO_REQUEST,LPBYTE,LPDWORD);
typedef long (WINAPI *SCBeginTransaction)(SCARDHANDLE);
typedef long (WINAPI *SCEndTransaction)(SCARDHANDLE, DWORD);
typedef long (WINAPI *SCConnectA)(SCARDCONTEXT,LPCSTR,DWORD,DWORD,LPSCARDHANDLE,LPDWORD);
typedef long (WINAPI *SCControl)(SCARDHANDLE,DWORD,LPCVOID,DWORD,LPVOID,DWORD,LPDWORD);
typedef long (WINAPI *SCDisconnect)(SCARDHANDLE,DWORD);
typedef long (WINAPI *SCEstablishContext)(DWORD,LPCVOID,LPCVOID,LPSCARDCONTEXT);
typedef long (WINAPI *SCReleaseContext)(SCARDCONTEXT);
typedef long (WINAPI *SCGetStatusChangeA)(SCARDCONTEXT,DWORD,LPSCARD_READERSTATE_A,DWORD);
typedef long (WINAPI *SCListReadersA)(SCARDCONTEXT,LPCSTR,LPTSTR,LPDWORD);
typedef long (WINAPI *SCStatusA)(SCARDHANDLE,LPTSTR,LPDWORD,LPDWORD,LPDWORD,LPBYTE,LPDWORD);

#define WINSCARPAPI WINSCARDAPI

static WINSCARDAPI LONG (WINAPI *Original_SCardTransmit)(
  IN SCARDHANDLE hCard,  
  IN LPCSCARD_IO_REQUEST pioSendPci,
  IN LPCBYTE pbSendBuffer,
  IN DWORD cbSendLength,
  IN OUT LPSCARD_IO_REQUEST pioRecvPci,
  OUT LPBYTE pbRecvBuffer,
  IN OUT LPDWORD pcbRecvLength
) = NULL;

static WINSCARDAPI LONG (WINAPI *Original_SCardBeginTransaction)(
    IN SCARDHANDLE hCard
) = NULL;

static WINSCARDAPI LONG (WINAPI *Original_SCardEndTransaction)(
    IN  SCARDHANDLE hCard,
    IN  DWORD dwDisposition
) = NULL;

static WINSCARDAPI LONG (WINAPI *Original_SCardConnectA)(
    IN  SCARDCONTEXT hContext,
    IN  LPCSTR szReader,
    IN  DWORD dwShareMode,
    IN  DWORD dwPreferredProtocols,
    OUT LPSCARDHANDLE phCard,
    OUT LPDWORD pdwActiveProtocol
) = NULL;

static WINSCARDAPI LONG (WINAPI *Original_SCardControl)(
    IN SCARDHANDLE hCard,
    IN DWORD dwControlCode,
    IN LPCVOID lpInBuffer,
    IN DWORD nInBufferSize,
    OUT LPVOID lpOutBuffer,
    IN  DWORD nOutBufferSize,
    OUT LPDWORD lpBytesReturned
) = NULL;

static WINSCARDAPI LONG (WINAPI *Original_SCardDisconnect)(
    IN SCARDHANDLE hCard,
    IN DWORD dwDisposition
) = NULL;

static WINSCARDAPI LONG (WINAPI *Original_SCardEstablishContext)(
    IN  DWORD dwScope,
    IN  LPCVOID pvReserved1,
    IN  LPCVOID pvReserved2,
    OUT LPSCARDCONTEXT phContext
) = NULL;

static WINSCARDAPI LONG (WINAPI *Original_SCardReleaseContext)(
    IN SCARDCONTEXT hContext
) = NULL;

static WINSCARDAPI LONG (WINAPI *Original_SCardGetStatusChangeA)(
    IN SCARDCONTEXT hContext,
    IN DWORD dwTimeout,
    IN OUT LPSCARD_READERSTATE_A rgReaderStates,
    IN DWORD cReaders
) = NULL;

static WINSCARDAPI LONG (WINAPI *Original_SCardListReadersA)(
    IN SCARDCONTEXT hContext,
    IN LPCSTR mszGroups,
    OUT LPTSTR mszReaders,
    IN OUT LPDWORD pcchReaders
) = NULL;

static WINSCARDAPI LONG (WINAPI *Original_SCardStatusA)(
    IN SCARDHANDLE hCard,
    OUT LPTSTR szReaderName,
    IN OUT LPDWORD pcchReaderLen,
    OUT LPDWORD pdwState,
    OUT LPDWORD pdwProtocol,
    OUT LPBYTE pbAtr,
    OUT LPDWORD pcbAtrLen
) = NULL;

//////////////////////////////////////////////////////////


/////////////////////////////////////////
// Function prototypes
EIDCommLIB::CCardMessage *SCardCreateMessage(char *pszName);
void FillUserData(EIDCommLIB::CCardMessage *pMessage);

/////////////////////////////////////////

#ifdef _WIN32
BOOL WINAPI DllMain( HINSTANCE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hModule);
            Initialize();
            break;
		case DLL_THREAD_ATTACH:
            break;
		case DLL_THREAD_DETACH:
            break;
		case DLL_PROCESS_DETACH:
            Finalize();
			break;
    }
    return TRUE;
}
#endif

bool Initialize()
{
    bool bRet = true;

    if (gpWinScardLoader == NULL)
    {
#ifdef PROBLEM_WITH_SWING_OFFLINE_SIGNTOOL
#ifdef WIN32
        if(!qApp && QApplication::winVersion() != Qt::WV_NT)
#else
        if(!qApp)
#endif
        {
            int i = 1;
            char *s = "beidwinscard";
            std::auto_ptr<QApplication> app(new QApplication(i, &s)); 
            gMyApp = app;
        }
#endif
#ifdef WIN32
        char szSysDir[MAX_PATH + 1] = {0}; 
        ::GetSystemDirectoryA(szSysDir, MAX_PATH + 1);
        QString strSysDir(szSysDir);
        strSysDir = QDir::convertSeparators(strSysDir);
        if(strSysDir.right(1) != QString("\\"))
        {
            strSysDir += "\\";
        }
        gpWinScardLoader = new QLibrary(strSysDir + PCSCNAME);
#else
        gpWinScardLoader = new QLibrary(PCSCNAME);
#endif
        if(gpWinScardLoader->load())
        {
            Original_SCardTransmit = (SCTransmit)gpWinScardLoader->resolve("SCardTransmit");
            Original_SCardBeginTransaction = (SCBeginTransaction)gpWinScardLoader->resolve("SCardBeginTransaction");
            Original_SCardEndTransaction = (SCEndTransaction)gpWinScardLoader->resolve("SCardEndTransaction");
            Original_SCardConnectA = (SCConnectA)gpWinScardLoader->resolve(SCARDCONNECTFUNC);
            Original_SCardControl = (SCControl)gpWinScardLoader->resolve("SCardControl");
            Original_SCardDisconnect = (SCDisconnect)gpWinScardLoader->resolve("SCardDisconnect");
            Original_SCardEstablishContext = (SCEstablishContext)gpWinScardLoader->resolve("SCardEstablishContext");
            Original_SCardReleaseContext = (SCReleaseContext)gpWinScardLoader->resolve("SCardReleaseContext");
            Original_SCardGetStatusChangeA = (SCGetStatusChangeA)gpWinScardLoader->resolve(SCARDGETSTATUSCHANGEFUNC);
            Original_SCardListReadersA = (SCListReadersA)gpWinScardLoader->resolve(SCARDLISTREADERSFUNC);
            Original_SCardStatusA = (SCStatusA)gpWinScardLoader->resolve(SCARDGETSTATUSFUNC);        
        }
    }

#ifdef BELPIC_PIN_PAD
    if(gpPinPad == NULL)
    {
        gpPinPad = new CPinPad();
    }
#endif

    // Read config file
    if(gpConfig == NULL)
    {
        gpConfig = new CConfig;
        gpConfig->Load();
    }

    if(gpConfig->GetServiceEnabled())
    {
        std::string szServerAddress = gpConfig->GetServerAddress();
        unsigned long ulServerPort = gpConfig->GetServerPort();
 
        if(szServerAddress.length() > 0 && ulServerPort > 0)
        {
            // Try to connect to PS/SC Service
            if (gpConnClientMan == NULL)
            {
                gpConnClientMan = new CConnectionManager();
            }
            if(gpConnectionClient == NULL)
            {
                gpConnectionClient = gpConnClientMan->CreateConnection();
            }
            if (!gpConnectionClient->Open(szServerAddress, ulServerPort))
	        {
                gbServerMode = false;
                gpConnectionClient->Close();
	        }
	        else
	        {
                gbServerMode = true;
	        }
        }

        if(gpServerPoll == NULL)
        {
             gpServerPoll = new CServerPoll();
             gpServerPoll->start();
        }
    }
    return bRet;
}

bool Finalize()
{
    bool bRet = true;
    QWaitCondition oWait;

    if(NULL != gpServerPoll)
    {
        gpServerPoll->Stop();
        for(int i = 0; i < 3 && !gpServerPoll->IsStopped(); ++i)
        {
            oWait.wait(500);
        }
    }

	if (NULL != gpConnectionClient)
	{
        gpConnectionClient->Close();
        gbServerMode = false;
		gpConnectionClient = NULL;
	}

    if (gpConnClientMan != NULL)
	{
		delete gpConnClientMan;
		gpConnClientMan = NULL;
	}

#ifdef BELPIC_PIN_PAD
    if(gpPinPad != NULL)
    {
        delete gpPinPad;
        gpPinPad = NULL;
    }
#endif

    if (gpWinScardLoader != NULL)
    {
        delete gpWinScardLoader;
        gpWinScardLoader = NULL;
    }

    if(gpConfig != NULL)
    {
        gpConfig->Unload();
        delete gpConfig;
        gpConfig = NULL;
    }
    return bRet;
}

void CheckConnection()
{
     if(gpConnectionClient != NULL && !gpConnectionClient->isValid())
    {
        gbServerMode = false;        
    }
}

WINSCARPAPI LONG WINAPI SCardEstablishContext(
    IN  DWORD dwScope,
    IN  LPCVOID pvReserved1,
    IN  LPCVOID pvReserved2,
    OUT LPSCARDCONTEXT phContext)
{
    CheckConnection();
    if(gbServerMode)
    {
        long lRet = SCARD_F_COMM_ERROR;
        *phContext = 0;
#ifdef _DEBUGLOG
        t.start();
        QString str("SCardEstablishContext Sending");
        DebugLogMessage(str.ascii());
#endif

        CCardMessage *pMessage = SCardCreateMessage("SCardEstablishContext");
	    if (pMessage != NULL)
	    {
            pMessage->Set("Scope", (unsigned long)dwScope);
            std::string strId = gpConnectionClient->SdMessage (pMessage);
            delete pMessage;
		    pMessage = gpConnectionClient->WaitMessageForID (strId);
            if(pMessage != NULL)
            {
                pMessage->Get("Context", (long *)phContext);  
                pMessage->Get("Return", lRet);  
                delete pMessage;
            }
        }
#ifdef _DEBUGLOG
        str = QString("SCardEstablishContext Received in Time elapsed = %1 ms").arg(t.elapsed());
        DebugLogMessage(str.ascii());
#endif
        return lRet;
    }
    else if(gpWinScardLoader && gpWinScardLoader->isLoaded())
    {
        return Original_SCardEstablishContext(dwScope, pvReserved1, pvReserved2, phContext);
    }
    return SCARD_E_NO_SERVICE;
}

WINSCARPAPI LONG WINAPI SCardReleaseContext(
    IN      SCARDCONTEXT hContext)
{
    CheckConnection();
    if(gbServerMode)
    {
        long lRet = SCARD_F_COMM_ERROR;
#ifdef _DEBUGLOG
        t.start();
        QString str("SCardReleaseContext Sending");
        DebugLogMessage(str.ascii());
#endif

        CCardMessage *pMessage = SCardCreateMessage("SCardReleaseContext");

	    if (pMessage != NULL)
	    {
            pMessage->Set("Context", hContext);
            std::string strId = gpConnectionClient->SdMessage (pMessage);
            delete pMessage;
		    pMessage = gpConnectionClient->WaitMessageForIDTimeOut (strId, 2000);
            if(pMessage != NULL)
            {
                pMessage->Get("Return", lRet);
                delete pMessage;
            }
        }
#ifdef _DEBUGLOG
        str = QString("SCardReleaseContext Received in Time elapsed = %1 ms").arg(t.elapsed());
        DebugLogMessage(str.ascii());
#endif
        return lRet;
    }
    else if(gpWinScardLoader && gpWinScardLoader->isLoaded())
    {
        return Original_SCardReleaseContext(hContext);
    }
    return SCARD_E_NO_SERVICE;
}

WINSCARPAPI LONG WINAPI SCardFreeMemory(
    IN SCARDCONTEXT hContext,
    IN LPVOID pvMem)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

//
// Database Reader routines
//

WINSCARPAPI LONG WINAPI SCardListReaderGroupsA(
    IN      SCARDCONTEXT hContext,
    OUT     LPTSTR mszGroups,
    IN OUT  LPDWORD pcchGroups)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

WINSCARPAPI LONG WINAPI SCardListReaderGroupsW(
    IN      SCARDCONTEXT hContext,
    OUT     LPWSTR mszGroups,
    IN OUT  LPDWORD pcchGroups)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

#ifdef _WIN32
WINSCARPAPI LONG WINAPI SCardListReadersA(
#else
WINSCARPAPI LONG WINAPI SCardListReaders(
#endif
    IN      SCARDCONTEXT hContext,
    IN      LPCSTR mszGroups,
    OUT     LPTSTR mszReaders,
    IN OUT  LPDWORD pcchReaders)
{
    CheckConnection();
    if(gbServerMode)
    {
        long lRet = SCARD_F_COMM_ERROR;
#ifdef _DEBUGLOG
        t.start();
        QString str("ScardListReaders Sending");
        DebugLogMessage(str.ascii());
#endif

        CCardMessage *pMessage = SCardCreateMessage("SCardListReaders");
	    if (pMessage != NULL)
	    {
            pMessage->Set("Context", (long)hContext);
            if(mszGroups != NULL)
            {
                pMessage->Set("Groups", mszGroups);
            }
            pMessage->Set("ReadersLen", (long)*pcchReaders);
            std::string strId = gpConnectionClient->SdMessage (pMessage);
            delete pMessage;
		    pMessage = gpConnectionClient->WaitMessageForID (strId);
            if(pMessage != NULL)
            {
                long lLen = 0;
                pMessage->Get("ReadersLen", lLen);
                if(NULL != mszReaders)
                {
                    if(*pcchReaders == SCARD_AUTOALLOCATE )
                    {
                        char *pmszReaders = new char[lLen+1];
                        memset(pmszReaders, 0, lLen+1);
                        pMessage->Get("Readers", (unsigned char *)pmszReaders, lLen);
                        memcpy(mszReaders, &pmszReaders, 4);
                    }
                    else
                    {
                        pMessage->Get("Readers", (unsigned char *)mszReaders, lLen);  
                    }
                }
                *pcchReaders = lLen;
                pMessage->Get("Return", lRet);  
                delete pMessage;
            }
        }
#ifdef _DEBUGLOG
        str = QString("SCardListReaders Received in Time elapsed = %1 ms").arg(t.elapsed());
        DebugLogMessage(str.ascii());
#endif
        return lRet;
    }
    else if(gpWinScardLoader && gpWinScardLoader->isLoaded())
    {
        return Original_SCardListReadersA(hContext, mszGroups, mszReaders, pcchReaders);
    }
    return SCARD_E_NO_SERVICE;
}

WINSCARPAPI LONG WINAPI SCardListReadersW(
    IN      SCARDCONTEXT hContext,
    IN      LPCWSTR mszGroups,
    OUT     LPWSTR mszReaders,
    IN OUT  LPDWORD pcchReaders)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

WINSCARPAPI LONG WINAPI SCardListCardsA(
    IN      SCARDCONTEXT hContext,
    IN      LPCBYTE pbAtr,
    IN      LPCGUID rgquidInterfaces,
    IN      DWORD cguidInterfaceCount,
    OUT     LPTSTR mszCards,
    IN OUT  LPDWORD pcchCards)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

WINSCARPAPI LONG WINAPI SCardListCardsW(
    IN      SCARDCONTEXT hContext,
    IN      LPCBYTE pbAtr,
    IN      LPCGUID rgquidInterfaces,
    IN      DWORD cguidInterfaceCount,
    OUT     LPWSTR mszCards,
    IN OUT  LPDWORD pcchCards)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

WINSCARPAPI LONG WINAPI SCardListInterfacesA(
    IN      SCARDCONTEXT hContext,
    IN      LPCSTR szCard,
    OUT     LPGUID pguidInterfaces,
    IN OUT  LPDWORD pcguidInterfaces)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

WINSCARPAPI LONG WINAPI SCardListInterfacesW(
    IN      SCARDCONTEXT hContext,
    IN      LPCWSTR szCard,
    OUT     LPGUID pguidInterfaces,
    IN OUT  LPDWORD pcguidInterfaces)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

WINSCARPAPI LONG WINAPI SCardGetProviderIdA(
    IN      SCARDCONTEXT hContext,
    IN      LPCSTR szCard,
    OUT     LPGUID pguidProviderId)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

WINSCARPAPI LONG WINAPI SCardGetProviderIdW(
    IN      SCARDCONTEXT hContext,
    IN      LPCWSTR szCard,
    OUT     LPGUID pguidProviderId)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

//
// Database Writer routines
//

WINSCARPAPI LONG WINAPI SCardIntroduceReaderGroupA(
    IN SCARDCONTEXT hContext,
    IN LPCSTR szGroupName)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

WINSCARPAPI LONG WINAPI SCardIntroduceReaderGroupW(
    IN SCARDCONTEXT hContext,
    IN LPCWSTR szGroupName)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

WINSCARPAPI LONG WINAPI SCardForgetReaderGroupA(
    IN SCARDCONTEXT hContext,
    IN LPCSTR szGroupName)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

WINSCARPAPI LONG WINAPI SCardForgetReaderGroupW(
    IN SCARDCONTEXT hContext,
    IN LPCWSTR szGroupName)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

WINSCARPAPI LONG WINAPI SCardIntroduceReaderA(
    IN SCARDCONTEXT hContext,
    IN LPCSTR szReaderName,
    IN LPCSTR szDeviceName)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

WINSCARPAPI LONG WINAPI
SCardIntroduceReaderW(
    IN SCARDCONTEXT hContext,
    IN LPCWSTR szReaderName,
    IN LPCWSTR szDeviceName)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

WINSCARPAPI LONG WINAPI
SCardForgetReaderA(
    IN SCARDCONTEXT hContext,
    IN LPCSTR szReaderName)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

WINSCARPAPI LONG WINAPI
SCardForgetReaderW(
    IN SCARDCONTEXT hContext,
    IN LPCWSTR szReaderName)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

WINSCARPAPI LONG WINAPI SCardAddReaderToGroupA(
    IN SCARDCONTEXT hContext,
    IN LPCSTR szReaderName,
    IN LPCSTR szGroupName)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

WINSCARPAPI LONG WINAPI SCardAddReaderToGroupW(
    IN SCARDCONTEXT hContext,
    IN LPCWSTR szReaderName,
    IN LPCWSTR szGroupName)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

WINSCARPAPI LONG WINAPI SCardRemoveReaderFromGroupA(
    IN SCARDCONTEXT hContext,
    IN LPCSTR szReaderName,
    IN LPCSTR szGroupName)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

WINSCARPAPI LONG WINAPI SCardRemoveReaderFromGroupW(
    IN SCARDCONTEXT hContext,
    IN LPCWSTR szReaderName,
    IN LPCWSTR szGroupName)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

WINSCARPAPI LONG WINAPI SCardIntroduceCardTypeA(
    IN SCARDCONTEXT hContext,
    IN LPCSTR szCardName,
    IN LPGUID pguidPrimaryProvider,
    IN LPGUID rgguidInterfaces,
    IN DWORD dwInterfaceCount,
    IN LPCBYTE pbAtr,
    IN LPCBYTE pbAtrMask,
    IN DWORD cbAtrLen)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

WINSCARPAPI LONG WINAPI SCardIntroduceCardTypeW(
    IN SCARDCONTEXT hContext,
    IN LPCWSTR szCardName,
    IN LPGUID pguidPrimaryProvider,
    IN LPGUID rgguidInterfaces,
    IN DWORD dwInterfaceCount,
    IN LPCBYTE pbAtr,
    IN LPCBYTE pbAtrMask,
    IN DWORD cbAtrLen)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

WINSCARPAPI LONG WINAPI SCardForgetCardTypeA(
    IN SCARDCONTEXT hContext,
    IN LPCSTR szCardName)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

WINSCARPAPI LONG WINAPI SCardForgetCardTypeW(
    IN SCARDCONTEXT hContext,
    IN LPCWSTR szCardName)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

//  Reader Services
//
//      The following services are supplied to simplify the use of the Service
//      Manager API.
//

WINSCARPAPI LONG WINAPI SCardLocateCardsA(
    IN      SCARDCONTEXT hContext,
    IN      LPCSTR mszCards,
    IN OUT  LPSCARD_READERSTATE_A rgReaderStates,
    IN      DWORD cReaders)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

WINSCARPAPI LONG WINAPI SCardLocateCardsW(
    IN      SCARDCONTEXT hContext,
    IN      LPCWSTR mszCards,
    IN OUT  LPSCARD_READERSTATE_W rgReaderStates,
    IN      DWORD cReaders)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

#ifdef _WIN32
WINSCARPAPI LONG WINAPI SCardGetStatusChangeA(
#else
WINSCARPAPI LONG WINAPI SCardGetStatusChange(
#endif

    IN      SCARDCONTEXT hContext,
    IN      DWORD dwTimeout,
    IN OUT  LPSCARD_READERSTATE_A rgReaderStates,
    IN      DWORD cReaders)
{
    CheckConnection();
    if(gbServerMode)
    {
        long lRet = SCARD_F_COMM_ERROR;
#ifdef _DEBUGLOG
        t.start();
        QString str("ScardGetStatusChange Sending");
        DebugLogMessage(str.ascii());
#endif

        CCardMessage *pMessage = SCardCreateMessage("SCardGetStatusChange");
	    if (pMessage != NULL)
	    {
            pMessage->Set("Context", hContext);
            pMessage->Set("Timeout", dwTimeout);
            pMessage->Set("ReadersLen", cReaders);
            // Fill Readers
            for(unsigned int i = 0; i < cReaders; ++i)
            {
                char szReader[16] = {0};
                char szState[16] = {0};
                sprintf(szReader, "Reader%d", i); 
                sprintf(szState, "CurrentState%d", i); 
                pMessage->Set(szReader, (char *)rgReaderStates[i].szReader);
                pMessage->Set(szState, rgReaderStates[i].dwCurrentState);
            }
            std::string strId = gpConnectionClient->SdMessage (pMessage);
            delete pMessage;
		    pMessage = gpConnectionClient->WaitMessageForID (strId);
            if(pMessage != NULL)
            {
                for(unsigned int i = 0; i < cReaders; ++i)
                {
                    char szEventstate[16] = {0};
                    char szAtrLen[16] = {0};
                    char szAtr[16] = {0};
                    sprintf(szEventstate, "EventState%d", i); 
                    sprintf(szAtrLen, "AtrLen%d", i); 
                    sprintf(szAtr, "Atr%d", i);
                    long lTemp = 0;
                    if(pMessage->Get(szEventstate, lTemp))
                    {
                        rgReaderStates[i].dwEventState = lTemp;  
                    }
                    if(pMessage->Get(szAtrLen, lTemp))
                    {
                        rgReaderStates[i].cbAtr = lTemp;
                    }
                    pMessage->Get(szAtr, rgReaderStates[i].rgbAtr, rgReaderStates[i].cbAtr);  
                }
                pMessage->Get("Return", lRet);
                delete pMessage;                
            }
        }
#ifdef _DEBUGLOG
        str = QString("SCardGetStatusChange Received in Time elapsed = %1 ms").arg(t.elapsed());
        DebugLogMessage(str.ascii());
#endif
        return lRet;
    }
    else if(gpWinScardLoader && gpWinScardLoader->isLoaded())
    {
        return Original_SCardGetStatusChangeA(hContext, dwTimeout, rgReaderStates, cReaders);
    }
    return SCARD_E_NO_SERVICE;
}

WINSCARPAPI LONG WINAPI SCardGetStatusChangeW(
    IN      SCARDCONTEXT hContext,
    IN      DWORD dwTimeout,
    IN OUT  LPSCARD_READERSTATE_W rgReaderStates,
    IN      DWORD cReaders)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

WINSCARPAPI LONG WINAPI SCardCancel(
    IN      SCARDCONTEXT hContext)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

//  Card/Reader Access Services
//
//      The following services provide means for establishing communication with
//      the card.
//

#ifdef _WIN32
WINSCARPAPI LONG WINAPI SCardConnectA(
#else
WINSCARPAPI LONG WINAPI SCardConnect(
#endif
    IN      SCARDCONTEXT hContext,
    IN      LPCSTR szReader,
    IN      DWORD dwShareMode,
    IN      DWORD dwPreferredProtocols,
    OUT     LPSCARDHANDLE phCard,
    OUT     LPDWORD pdwActiveProtocol)
{
    CheckConnection();
    if(gbServerMode)
    {
        long lRet = SCARD_F_COMM_ERROR;
#ifdef _DEBUGLOG
        t.start();
        QString str("SCardConnect Sending");
        DebugLogMessage(str.ascii());
#endif

        CCardMessage *pMessage = SCardCreateMessage("SCardConnect");

	    if (pMessage != NULL)
	    {
            pMessage->Set("Context", hContext);
            pMessage->Set("Reader", (char *)szReader);
            pMessage->Set("ShareMode", dwShareMode);
            pMessage->Set("Protocol", dwPreferredProtocols);

            std::string strId = gpConnectionClient->SdMessage (pMessage);
            delete pMessage;
		    pMessage = gpConnectionClient->WaitMessageForID (strId);
            if(pMessage != NULL)
            {
                pMessage->Get("Card", (long *)phCard);  
                pMessage->Get("ActiveProtocol", (long *)pdwActiveProtocol);  
                pMessage->Get("Return", lRet);  
                delete pMessage;
            }
        }
#ifdef _DEBUGLOG
        str = QString("SCardConnect Received in Time elapsed = %1 ms").arg(t.elapsed());
        DebugLogMessage(str.ascii());
#endif
        return lRet;
    }
    else if(gpWinScardLoader && gpWinScardLoader->isLoaded())
    {
        return Original_SCardConnectA(hContext, szReader, dwShareMode, dwPreferredProtocols, phCard, pdwActiveProtocol);
    }
    return SCARD_E_NO_SERVICE;
}

WINSCARPAPI LONG WINAPI SCardConnectW(
    IN      SCARDCONTEXT hContext,
    IN      LPCWSTR szReader,
    IN      DWORD dwShareMode,
    IN      DWORD dwPreferredProtocols,
    OUT     LPSCARDHANDLE phCard,
    OUT     LPDWORD pdwActiveProtocol)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

WINSCARPAPI LONG WINAPI SCardReconnect(
    IN      SCARDHANDLE hCard,
    IN      DWORD dwShareMode,
    IN      DWORD dwPreferredProtocols,
    IN      DWORD dwInitialization,
    OUT     LPDWORD pdwActiveProtocol)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

WINSCARPAPI LONG WINAPI SCardDisconnect(
    IN      SCARDHANDLE hCard,
    IN      DWORD dwDisposition)
{
    CheckConnection();
    if(gbServerMode)
    {
        long lRet = SCARD_F_COMM_ERROR;
#ifdef _DEBUGLOG
        t.start();
        QString str("SCardDisconnect Sending");
        DebugLogMessage(str.ascii());
#endif

        CCardMessage *pMessage = SCardCreateMessage("SCardDisconnect");
	    if (pMessage != NULL)
	    {
            pMessage->Set("Card", hCard);
            pMessage->Set("Disposition", dwDisposition);
            std::string strId = gpConnectionClient->SdMessage (pMessage);
            delete pMessage;
		    pMessage = gpConnectionClient->WaitMessageForIDTimeOut (strId, 2000);
            if(pMessage != NULL)
            {
                pMessage->Get("Return", lRet);  
                delete pMessage;
            }
        }
#ifdef _DEBUGLOG
        str = QString("SCardDisconnect Received in Time elapsed = %1 ms").arg(t.elapsed());
        DebugLogMessage(str.ascii());
#endif
        return lRet;
    }
    else if(gpWinScardLoader && gpWinScardLoader->isLoaded())
    {
        return Original_SCardDisconnect(hCard, dwDisposition);
    }
    return SCARD_E_NO_SERVICE;
}

WINSCARPAPI LONG WINAPI SCardBeginTransaction(
    IN      SCARDHANDLE hCard)
{
    CheckConnection();
    if(gbServerMode)
    {
        long lRet = SCARD_F_COMM_ERROR;
#ifdef _DEBUGLOG
        t.start();
        QString str("SCardBeginTransaction Sending");
        DebugLogMessage(str.ascii());
#endif

        CCardMessage *pMessage = SCardCreateMessage("SCardBeginTransaction");

	    if (pMessage != NULL)
	    {
            pMessage->Set("Card", hCard);
            std::string strId = gpConnectionClient->SdMessage (pMessage);
            delete pMessage;
		    pMessage = gpConnectionClient->WaitMessageForID (strId);
            if(pMessage != NULL)
            {
                pMessage->Get("Return", lRet);  
                delete pMessage;
            }
        }
#ifdef _DEBUGLOG
        str = QString("SCardBeginTransaction Received in Time elapsed = %1 ms").arg(t.elapsed());
        DebugLogMessage(str.ascii());
#endif
        return lRet;
    }
    else if(gpWinScardLoader && gpWinScardLoader->isLoaded())
    {
        return Original_SCardBeginTransaction(hCard);
    }
    return SCARD_E_NO_SERVICE;
}

WINSCARPAPI LONG WINAPI SCardEndTransaction(
    IN      SCARDHANDLE hCard,
    IN      DWORD dwDisposition)
{
    CheckConnection();
    if(gbServerMode)
    {
        long lRet = SCARD_F_COMM_ERROR;
#ifdef _DEBUGLOG
        t.start();
        QString str("SCardEndTransaction Sending");
        DebugLogMessage(str.ascii());
#endif

        CCardMessage *pMessage = SCardCreateMessage("SCardEndTransaction");

	    if (pMessage != NULL)
	    {
            pMessage->Set("Card", hCard);
            pMessage->Set("Disposition", dwDisposition);

            std::string strId = gpConnectionClient->SdMessage (pMessage);
            delete pMessage;
		    pMessage = gpConnectionClient->WaitMessageForID (strId);
            if(pMessage != NULL)
            {
                pMessage->Get("Return", lRet);  
                delete pMessage;
            }
        }
#ifdef _DEBUGLOG
        str = QString("SCardEndTransaction Received in Time elapsed = %1 ms").arg(t.elapsed());
        DebugLogMessage(str.ascii());
#endif
        return lRet;
    }
    else if(gpWinScardLoader && gpWinScardLoader->isLoaded())
    {
        return Original_SCardEndTransaction(hCard, dwDisposition);
    }
    return SCARD_E_NO_SERVICE;
}

WINSCARPAPI LONG WINAPI SCardState(
    IN SCARDHANDLE hCard,
    OUT LPDWORD pdwState,
    OUT LPDWORD pdwProtocol,
    OUT LPBYTE pbAtr,
    OUT LPDWORD pcbAtrLen)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

#ifdef _WIN32
WINSCARPAPI LONG WINAPI SCardStatusA(
#else
WINSCARPAPI LONG WINAPI SCardStatus(
#endif
    IN SCARDHANDLE hCard,
    OUT LPTSTR szReaderName,
    IN OUT LPDWORD pcchReaderLen,
    OUT LPDWORD pdwState,
    OUT LPDWORD pdwProtocol,
    OUT LPBYTE pbAtr,
    IN OUT LPDWORD pcbAtrLen)
{
    CheckConnection();
    if(gbServerMode)
    {
        long lRet = SCARD_F_COMM_ERROR;

        CCardMessage *pMessage = SCardCreateMessage("SCardStatus");
#ifdef _DEBUGLOG
        t.start();
        QString str("SCardStatus Sending");
        DebugLogMessage(str.ascii());
#endif

	    if (pMessage != NULL)
	    {
            pMessage->Set("Card", hCard);
            std::string strId = gpConnectionClient->SdMessage (pMessage);
            delete pMessage;
		    pMessage = gpConnectionClient->WaitMessageForID (strId);
            if(pMessage != NULL)
            {
                long lLen = 0;
                pMessage->Get("ReaderLen", lLen);
                if(NULL != szReaderName && lLen > 0)
                {
                    if(*pcchReaderLen == SCARD_AUTOALLOCATE )
                    {
                        char *pmszReader = new char[lLen+1];
                        memset(pmszReader, 0, lLen+1);
                        pMessage->Get("ReaderName", (unsigned char *)pmszReader, lLen);
                        memcpy(szReaderName, &pmszReader, 4);
                    }
                    else
                    {
                        pMessage->Get("ReaderName", szReaderName);  
                    }
                }
                *pcchReaderLen = lLen;
                pMessage->Get("State", (long *)pdwState);  
                pMessage->Get("Protocol", (long *)pdwProtocol);  

                lLen = 0;
                pMessage->Get("AtrLen", lLen);
                if(NULL != pbAtr && lLen > 0)
                {
                    if(*pcbAtrLen == SCARD_AUTOALLOCATE )
                    {
                        BYTE *pAtr = new BYTE[32];
                        memset(pAtr, 0, 32);
                        pMessage->Get("Atr", pAtr, lLen);
                        memcpy(pbAtr, &pAtr, 4);
                    }
                    else
                    {
                        pMessage->Get("Atr", pbAtr, lLen);  
                    }
                }
                if(pcbAtrLen != NULL)
                {
                    *pcbAtrLen = lLen;
                }
                pMessage->Get("Return", lRet);  
                delete pMessage;
            }
        }
#ifdef _DEBUGLOG
        str = QString("SCardStatus Received in Time elapsed = %1 ms").arg(t.elapsed());
        DebugLogMessage(str.ascii());
#endif
        return lRet;    
    }
    else if(gpWinScardLoader && gpWinScardLoader->isLoaded())
    {
        return Original_SCardStatusA(hCard, szReaderName, pcchReaderLen, pdwState, pdwProtocol, pbAtr, pcbAtrLen);
    }
    return SCARD_E_NO_SERVICE;
}

WINSCARPAPI LONG WINAPI SCardStatusW(
    IN SCARDHANDLE hCard,
    OUT LPWSTR szReaderName,
    IN OUT LPDWORD pcchReaderLen,
    OUT LPDWORD pdwState,
    OUT LPDWORD pdwProtocol,
    OUT LPBYTE pbAtr,
    OUT LPDWORD pcbAtrLen)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

//  I/O Services
//
//      The following services provide access to the I/O capabilities of the
//      reader drivers.  Services of the Smart Card are requested by placing the
//      following structure into the protocol buffer:
//

WINSCARPAPI LONG WINAPI SCardTransmit(
    IN SCARDHANDLE hCard,
    IN LPCSCARD_IO_REQUEST pioSendPci,
    IN LPCBYTE pbSendBuffer,
    IN DWORD cbSendLength,
    IN OUT LPSCARD_IO_REQUEST pioRecvPci,
    OUT LPBYTE pbRecvBuffer,
    IN OUT LPDWORD pcbRecvLength)
{
    CheckConnection();
    if(gbServerMode)
    {
        long lRet = SCARD_F_COMM_ERROR;

#ifdef _DEBUGLOGTIME
        t.start();
        QString str;
        str = QString("ScardTransmit: Sending %1 bytes").arg(cbSendLength);
        DebugLogMessage(str.ascii());
#endif
        CCardMessage *pMessage = SCardCreateMessage("SCardTransmit");

	    if (pMessage != NULL)
	    {
            pMessage->Set("Card", hCard);
            pMessage->Set("SendProtocol", pioSendPci->dwProtocol);
            pMessage->Set("SendProtocolLen", pioSendPci->cbPciLength);
            pMessage->Set("SendBuffer", (BYTE *)pbSendBuffer, cbSendLength);
            pMessage->Set("SendBufferLen", cbSendLength);
            if(pioRecvPci != NULL)
            {
                pMessage->Set("RecvProtocol", pioRecvPci->dwProtocol);
                pMessage->Set("RecvProtocolLen", pioRecvPci->cbPciLength);
            }
            if(pcbRecvLength != NULL && *pcbRecvLength > 0)
            {
                pMessage->Set("RecvLen", *pcbRecvLength);
            }

            std::string strId = gpConnectionClient->SdMessage (pMessage);
            delete pMessage;
		    pMessage = gpConnectionClient->WaitMessageForID (strId);
             if(pMessage != NULL)
            {
                if(pioRecvPci != NULL)
                {
                    long lTemp = 0;
                    if(pMessage->Get("RecvProtocol", lTemp))
                    {
                        pioRecvPci->dwProtocol = lTemp;
                    }
                    if(pMessage->Get("RecvProtocolLen", lTemp)) 
                    {
                        pioRecvPci->cbPciLength = lTemp;
                    }
                }
                long lLen = 0;
                pMessage->Get("RecvLen", lLen);
                if(pbRecvBuffer != NULL && lLen > 0)
                {
                    if(*pcbRecvLength == SCARD_AUTOALLOCATE )
                    {
                        BYTE *pBuffer = new BYTE[lLen];
                        memset(pBuffer, 0, lLen);
                        pMessage->Get("RecvBuffer", pBuffer, lLen);
                        memcpy(pbRecvBuffer, &pBuffer, 4);
                    }
                    else
                    {
                        pMessage->Get("RecvBuffer", pbRecvBuffer, lLen);  
                    }
                }
                *pcbRecvLength = lLen;
                pMessage->Get("Return", lRet);  
                delete pMessage;
            }
        }
#ifdef _DEBUGLOGTIME
        str = QString("ScardTransmit: Received %1 bytes in Time elapsed = %2 ms").arg(*pcbRecvLength).arg(t.elapsed());
        DebugLogMessage(str.ascii());
#endif
        return lRet;
    }
    else if(gpWinScardLoader && gpWinScardLoader->isLoaded())
    {
#ifdef _DEBUGLOGTIME
        t.start();
        QString str;
        str = QString("ScardTransmit: Sending %1 bytes").arg(cbSendLength);
        DebugLogMessage(str.ascii());
#endif
        long lRet = Original_SCardTransmit(hCard, pioSendPci, pbSendBuffer, cbSendLength, pioRecvPci, pbRecvBuffer, pcbRecvLength);
#ifdef _DEBUGLOGTIME
        str = QString("ScardTransmit: Received %1 bytes in Time elapsed = %2 ms").arg(*pcbRecvLength).arg(t.elapsed());
        DebugLogMessage(str.ascii());
#endif
        return lRet;
    }
    return SCARD_E_NO_SERVICE;
}


//  Reader Control Routines
//
//      The following services provide for direct, low-level manipulation of the
//      reader by the calling application allowing it control over the
//      attributes of the communications with the card.
//

WINSCARPAPI LONG WINAPI SCardControl(
    IN      SCARDHANDLE hCard,
    IN      DWORD dwControlCode,
    IN      LPCVOID lpInBuffer,
    IN      DWORD nInBufferSize,
    OUT     LPVOID lpOutBuffer,
    IN      DWORD nOutBufferSize,
    OUT     LPDWORD lpBytesReturned)
{
    CheckConnection();
    if(gbServerMode)
    {
        long lRet = SCARD_F_COMM_ERROR;
#ifdef _DEBUGLOG
        t.start();
        QString str("SCardControl Sending");
        DebugLogMessage(str.ascii());
#endif

        CCardMessage *pMessage = NULL;
        SCARDHANDLE hLocal = hCard;

#ifdef BELPIC_PIN_PAD
        // Support for native pinpad
        if(hCard == SCR_CARD_HANDLE)
        {
            pMessage = SCardCreateMessage("SCardPinPadControl");
            // Parse TLV
            CTLVBuffer oTLVBuffer;
            oTLVBuffer.ParseTLV((unsigned char *)lpInBuffer, nInBufferSize);
            long lOp = 0;
            oTLVBuffer.FillLongData(0x01, &lOp);
            if(SCR_VERIFY_ID == lOp || SCR_CHANGE_ID == lOp)
            {
                oTLVBuffer.FillLongData(0x03, (long *)&hLocal);
            }
        }
        else
#endif // BELPIC_PIN_PAD
        {
            pMessage = SCardCreateMessage("SCardControl");
        }

	    if (pMessage != NULL)
	    {
            pMessage->Set("Card", hLocal);
            pMessage->Set("ControlCode", dwControlCode);
            if(lpInBuffer != NULL && nInBufferSize > 0)
            {
                pMessage->Set("SendBuffer", (BYTE *)lpInBuffer, nInBufferSize);
                pMessage->Set("SendBufferLen", nInBufferSize);
            }
            if(nOutBufferSize > 0)
            {
                pMessage->Set("RecvLen", nOutBufferSize);
            }
            std::string strId = gpConnectionClient->SdMessage (pMessage);
            delete pMessage;
		    pMessage = gpConnectionClient->WaitMessageForID (strId);
             if(pMessage != NULL)
            {
                long lLen = 0;
                pMessage->Get("RecvLen", lLen);
                if(lpOutBuffer != NULL && lLen > 0)
                {
                    if(nOutBufferSize == SCARD_AUTOALLOCATE )
                    {
                        BYTE *pBuffer = new BYTE[lLen];
                        memset(pBuffer, 0, lLen);
                        pMessage->Get("RecvBuffer", pBuffer, lLen);
                        memcpy(lpOutBuffer, &pBuffer, 4);
                    }
                    else
                    {
                        pMessage->Get("RecvBuffer", (BYTE *)lpOutBuffer, lLen);  
                    }
                }
                if(lpBytesReturned != NULL)
                {
                    *lpBytesReturned = lLen;
                }
                pMessage->Get("Return", lRet);  
                delete pMessage;
            }
        }
#ifdef _DEBUGLOG
        str = QString("SCardControl Received in Time elapsed = %1 ms").arg(t.elapsed());
        DebugLogMessage(str.ascii());
#endif
        return lRet;
    }
    else if(gpWinScardLoader && gpWinScardLoader->isLoaded())
    {
#ifdef BELPIC_PIN_PAD
        // Support for native pinpad
        if(hCard == SCR_CARD_HANDLE && gpPinPad != NULL)
        {
            return gpPinPad->HandlePinPad(lpInBuffer, nInBufferSize, lpOutBuffer, lpBytesReturned, 0);
        }
#endif
        return Original_SCardControl(hCard, dwControlCode, lpInBuffer, nInBufferSize, lpOutBuffer, nOutBufferSize, lpBytesReturned);
    }
    return SCARD_E_NO_SERVICE;
}


WINSCARPAPI LONG WINAPI SCardGetAttrib(
    IN SCARDHANDLE hCard,
    IN DWORD dwAttrId,
    OUT LPBYTE pbAttr,
    IN OUT LPDWORD pcbAttrLen)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

WINSCARPAPI LONG WINAPI SCardSetAttrib(
    IN SCARDHANDLE hCard,
    IN DWORD dwAttrId,
    IN LPCBYTE pbAttr,
    IN DWORD cbAttrLen)
{
    return SCARD_E_CARD_UNSUPPORTED;
}

///////////////////////////////////////////////////////////
CCardMessage *SCardCreateMessage(char *pszName)
{
	CCardMessage *pMessage = NULL;
    if(pszName != NULL)
    {
	    pMessage = new CCardMessage();
	    if (pMessage != NULL)
	    {
            pMessage->Set("FunctionName", pszName);
            FillUserData(pMessage);
        }
    }
    return pMessage;
}

void FillUserData(CCardMessage *pMessage)
{
    // Lookup ProcessID
    long lProcessID = getpid();
    pMessage->Set("ProcessID", lProcessID);
}

char *pcsc_stringify_error(long Error)
{
	static char strError[75];

	switch (Error)
	{
	case SCARD_S_SUCCESS:
		strcpy(strError, "Command successful.");
		break;
	case SCARD_E_CANCELLED:
		strcpy(strError, "Command cancelled.");
		break;
	case SCARD_E_CANT_DISPOSE:
		strcpy(strError, "Cannot dispose handle.");
		break;
	case SCARD_E_INSUFFICIENT_BUFFER:
		strcpy(strError, "Insufficient buffer.");
		break;
	case SCARD_E_INVALID_ATR:
		strcpy(strError, "Invalid ATR.");
		break;
	case SCARD_E_INVALID_HANDLE:
		strcpy(strError, "Invalid handle.");
		break;
	case SCARD_E_INVALID_PARAMETER:
		strcpy(strError, "Invalid parameter given.");
		break;
	case SCARD_E_INVALID_TARGET:
		strcpy(strError, "Invalid target given.");
		break;
	case SCARD_E_INVALID_VALUE:
		strcpy(strError, "Invalid value given.");
		break;
	case SCARD_E_NO_MEMORY:
		strcpy(strError, "Not enough memory.");
		break;
	case SCARD_F_COMM_ERROR:
		strcpy(strError, "RPC transport error.");
		break;
	case SCARD_F_INTERNAL_ERROR:
		strcpy(strError, "Unknown internal error.");
		break;
	case SCARD_F_UNKNOWN_ERROR:
		strcpy(strError, "Unknown internal error.");
		break;
	case SCARD_F_WAITED_TOO_LONG:
		strcpy(strError, "Waited too long.");
		break;
	case SCARD_E_UNKNOWN_READER:
		strcpy(strError, "Unknown reader specified.");
		break;
	case SCARD_E_TIMEOUT:
		strcpy(strError, "Command timeout.");
		break;
	case SCARD_E_SHARING_VIOLATION:
		strcpy(strError, "Sharing violation.");
		break;
	case SCARD_E_NO_SMARTCARD:
		strcpy(strError, "No smartcard inserted.");
		break;
	case SCARD_E_UNKNOWN_CARD:
		strcpy(strError, "Unknown card.");
		break;
	case SCARD_E_PROTO_MISMATCH:
		strcpy(strError, "Card protocol mismatch.");
		break;
	case SCARD_E_NOT_READY:
		strcpy(strError, "Subsystem not ready.");
		break;
	case SCARD_E_SYSTEM_CANCELLED:
		strcpy(strError, "System cancelled.");
		break;
	case SCARD_E_NOT_TRANSACTED:
		strcpy(strError, "Transaction failed.");
		break;
	case SCARD_E_READER_UNAVAILABLE:
		strcpy(strError, "Reader/s is unavailable.");
		break;
	case SCARD_W_UNSUPPORTED_CARD:
		strcpy(strError, "Card is not supported.");
		break;
	case SCARD_W_UNRESPONSIVE_CARD:
		strcpy(strError, "Card is unresponsive.");
		break;
	case SCARD_W_UNPOWERED_CARD:
		strcpy(strError, "Card is unpowered.");
		break;
	case SCARD_W_RESET_CARD:
		strcpy(strError, "Card was reset.");
		break;
	case SCARD_W_REMOVED_CARD:
		strcpy(strError, "Card was removed.");
		break;
	case SCARD_W_INSERTED_CARD:
		strcpy(strError, "Card was inserted.");
		break;
	case SCARD_E_UNSUPPORTED_FEATURE:
		strcpy(strError, "Feature not supported.");
		break;
	case SCARD_E_PCI_TOO_SMALL:
		strcpy(strError, "PCI struct too small.");
		break;
	case SCARD_E_READER_UNSUPPORTED:
		strcpy(strError, "Reader is unsupported.");
		break;
	case SCARD_E_DUPLICATE_READER:
		strcpy(strError, "Reader already exists.");
		break;
	case SCARD_E_CARD_UNSUPPORTED:
		strcpy(strError, "Card is unsupported.");
		break;
	case SCARD_E_NO_SERVICE:
		strcpy(strError, "Service not available.");
		break;
	case SCARD_E_SERVICE_STOPPED:
		strcpy(strError, "Service was stopped.");
		break;

	};

	return strError;
}
