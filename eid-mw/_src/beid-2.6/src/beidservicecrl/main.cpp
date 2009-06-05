#include <qapplication.h>
#include "../beidcommon/qtservice.h"
#include "CRLScheduler.h"

class CRLService : public QtService
{
public:
    CRLService();
    virtual ~CRLService();

protected:
    bool initialize();
    int run( int argc, char **argv );
    void stop();

private:
    CCRLScheduler *m_pCRLScheduler;
};

CRLService::CRLService()
    : QtService( "eID CRL Service", "Automatic download of the eID CRL files." )
{
    m_pCRLScheduler = NULL;
}

CRLService::~CRLService()
{
}

int CRLService::run( int argc, char **argv )
{
    QApplication app( argc, argv );
    if(m_pCRLScheduler)
    {
        m_pCRLScheduler->start();
    }
    return app.exec();
}

bool CRLService::initialize()
{
    bool bRet = true;
    m_pCRLScheduler = new CCRLScheduler();
    return bRet;
}

void CRLService::stop()
{
    //Cleanup
    if(m_pCRLScheduler)
    {
	    m_pCRLScheduler->StopRunning();
        m_pCRLScheduler->wait(5000);
	    delete m_pCRLScheduler;
	    m_pCRLScheduler = NULL;
    }
    QtService::stop();
}

int main( int argc, char **argv )
{
    CRLService service;
    return service.parseArguments( argc, argv );
}
