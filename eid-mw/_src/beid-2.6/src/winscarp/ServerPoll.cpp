// ServerPoll.cpp: implementation of the CServerPoll class.
//
//////////////////////////////////////////////////////////////////////

#include "ServerPoll.h"
#include "../beidcommlib/beidcommlib.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

using namespace EIDCommLIB;

extern bool gbServerMode;
extern bool Initialize();
extern CConnection *gpConnectionClient;
CServerPoll::CServerPoll() : m_bRun(true), m_bStopped(true)
{
}

CServerPoll::~CServerPoll()
{

}

void CServerPoll::run()
{
    m_bStopped = false;
    while(m_bRun)
    {
        if(!gbServerMode)
        {
            Initialize();
        }
        else
        {
            if(gpConnectionClient != NULL)
            {
                if(!gpConnectionClient->isValid())
                {
                    gbServerMode = false;
                    continue;
                }
                else
                {
                    CCardMessage oMessage;
                    oMessage.Set("Alive", 1); 
                    gpConnectionClient->SdMessage(&oMessage);
                }
            }
        }
        m_oWait.wait(1000);
    }
    m_bStopped = true;
}

void CServerPoll::Stop()
{
    m_bRun = false;
    m_oWait.wakeOne();
}

