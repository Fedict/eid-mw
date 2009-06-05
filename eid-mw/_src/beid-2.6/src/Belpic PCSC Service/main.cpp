#include "StdAfx.h"
#include <qapplication.h>
#include "../beidcommon/qtservice.h"
#include "../beidcommon/config.h"
#include "PCSCManager.h"
#include "CardChangeMonitor.h"
#include "SCardHandler.h"
#include "maindialogimpl.h"

using namespace EIDCommLIB;


#ifdef WIN32
    #ifdef _DEBUG
    #pragma comment(lib, "../beidcommlib/Debug/beidcommlib")
    #else
    #pragma comment(lib, "../beidcommlib/Release/beidcommlib")
    #endif
#else
    #define PBT_APMQUERYSUSPEND             0x0000
    #define PBT_APMQUERYSUSPENDFAILED    0x0002
    #define PBT_APMRESUMESUSPEND           0x0007
#endif

#ifdef _DEBUGLOG
void DebugLogMessage(const std::string & strMsg)
{
    QFile oFile(QDir::homeDirPath() + "/beidpcscd.log");
    if(oFile.open(IO_WriteOnly | IO_Append))
    {
        std::string strTemp(strMsg);
        strTemp += "\r\n";
        oFile.writeBlock(strTemp.c_str(), strTemp.length());
        oFile.close();
    }
}
#endif

class PrivacyService : public QtService
{
public:
    PrivacyService();
protected:
    bool initialize();
    int run( int argc, char **argv );
    void stop();
    void powerEvent( unsigned long eventType, void *pEventData,  void *pContext);

private:
    CPCSCManager *m_pPCSCManager;
    CCardChangeMonitor *m_pCardChangeMonitor;
    CSCardHandler *m_pSCardHandler;
    CConnectionManager *m_pConnectionManager;
};


PrivacyService::PrivacyService()
#ifdef _PRIVACY_REMOTE_SVC
    : QtService( "eID Remote Service", "Remote service for the eID card." )
#else
    : QtService( "eID Privacy Service", "Privacy filter for the eID card." )
#endif
{
    m_pPCSCManager = NULL;
    m_pCardChangeMonitor = NULL;
    m_pSCardHandler = NULL;
    m_pConnectionManager = NULL;
}

int PrivacyService::run(int argc, char **argv)
{
    QApplication app(argc, argv);
	maindialogImpl dialog( 0, 0, TRUE );
	app.setMainWidget(&dialog);
    dialog.SetConnectionManager(m_pConnectionManager);

    // Read config file
    eidcommon::CConfig oConfig;
    oConfig.Load();
    unsigned long ulServerPort = oConfig.GetServerPort();
#ifdef _PRIVACY_REMOTE_SVC
    ulServerPort++;
#endif
    m_pPCSCManager->SetMainDialog(&dialog); 
    m_pPCSCManager->Initialize();
    m_pConnectionManager->StartServer(ulServerPort);
    m_pCardChangeMonitor->start();
    oConfig.Unload();

    return app.exec();
}

bool PrivacyService::initialize()
{
    bool bRet = true;
    m_pSCardHandler = new CSCardHandler();
    m_pPCSCManager = new CPCSCManager();
    m_pCardChangeMonitor = new CCardChangeMonitor();
    m_pConnectionManager = new CConnectionManager();
    m_pSCardHandler->SetPCSCManager(m_pPCSCManager);
    m_pCardChangeMonitor->SetPCSCManager(m_pPCSCManager);
    m_pConnectionManager->SetMessageHandler(m_pSCardHandler);
    m_pPCSCManager->SetConnectionManager(m_pConnectionManager);
    return bRet;
}

void PrivacyService::stop()
{
    if(m_pCardChangeMonitor != NULL)
    {
	    m_pCardChangeMonitor->StopRunning();
	    m_pCardChangeMonitor->wait(5000);
	    delete m_pCardChangeMonitor;
	    m_pCardChangeMonitor = NULL;
    }
    if(m_pPCSCManager != NULL)
    {
        delete m_pPCSCManager;
        m_pPCSCManager = NULL;
    }
    if(m_pConnectionManager != NULL)
    {
	    m_pConnectionManager->StopServer();
	    delete m_pConnectionManager;
	    m_pConnectionManager = NULL;
    }
    if(m_pSCardHandler != NULL)
    {
        delete m_pSCardHandler;
        m_pSCardHandler = NULL;
    }
    QtService::stop();
}

void PrivacyService::powerEvent( unsigned long eventType, void *pEventData,  void *pContext)
{
    if(m_pCardChangeMonitor != NULL)
    {
        if(PBT_APMQUERYSUSPEND == eventType)
        {
            // Suspend CardMonitor
            m_pCardChangeMonitor->Suspend();
        }
        else if(PBT_APMRESUMESUSPEND == eventType || PBT_APMQUERYSUSPENDFAILED == eventType)
        {
            // Resume CardMonitor
            m_pCardChangeMonitor->Resume();
        }
    }
}

int main( int argc, char **argv )
{
    PrivacyService service;
    return service.parseArguments( argc, argv );
}
