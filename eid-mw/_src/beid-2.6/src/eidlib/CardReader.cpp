// CardReader.cpp: implementation of the CCardReader class.
//
//////////////////////////////////////////////////////////////////////

#include "CardReader.h"
#include "eiderrors.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCardReader::CCardReader()
{
    m_hCard = 0;
}

CCardReader::~CCardReader()
{

}

void *CCardReader::GetP15Card() 
{ 
    return NULL; 
}


long CCardReader::NoError(BEID_Status *ptStatus)
{
    long lRet = SC_NO_ERROR;
    ConvertOpenSCError(lRet, ptStatus);
    return lRet;
}

long CCardReader::EstablishContext(BEID_Status *ptStatus)
{
    return NoError(ptStatus);
}

long CCardReader::ReleaseContext(BEID_Status *ptStatus)
{
    return NoError(ptStatus);
}

long CCardReader::BeginTransaction(BEID_Status *ptStatus)
{
    return NoError(ptStatus);
}

long CCardReader::EndTransaction(BEID_Status *ptStatus)
{
    return NoError(ptStatus);
}

int CCardReader::CheckSW(BEID_Status *ptStatus)
{
    unsigned char *ucStatus = ptStatus->cardSW;
    
    if(ucStatus[0] == 0x00 && ucStatus[1] == 0x00)
        return TRUE;

    if(ucStatus[0] == 0x90 && ucStatus[1] == 0x00)
        return TRUE;
    
    if(ucStatus[0] == 0x6B && ucStatus[1] == 0x00)
        return TRUE;

    if(ucStatus[0] == 0x6C)
        return TRUE;

    return FALSE;
}
