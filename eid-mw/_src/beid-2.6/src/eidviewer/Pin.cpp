// Pin.cpp: implementation of the CPin class.
//
//////////////////////////////////////////////////////////////////////

#include "Pin.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPin::CPin()
{
    m_iRef = 0;
    m_iTriesLeft = -1;
    m_iFlags = 0;
    m_iType = 0;
    m_bPinPad = false;
    m_lUsageCode = 0;
}

CPin::~CPin()
{

}

CPin::CPin(const CPin & oPin)
{
    Copy(oPin);
}

CPin& CPin::operator =( const CPin & oPin )
{
    if(this != &oPin)
    {
		this->CPin::~CPin();
		this->Copy(oPin);
    }
    return *this;
}

void CPin::Copy(const CPin & oPin)
{
    m_iRef = oPin.GetReference(); 
    m_iTriesLeft = -oPin.GetTriesLeft(); 
    m_iFlags = oPin.GetFlags(); 
    m_iType = oPin.GetType();
    m_AuthID.RemoveAll();
    //m_AuthID.Append(oPin.GetAuthID());   
    m_strApp = oPin.GetApplication();
    m_bPinPad = oPin.GetPinPad();
    m_strLabel = oPin.GetLabel();
    m_lUsageCode = oPin.GetUsageCode();
}
