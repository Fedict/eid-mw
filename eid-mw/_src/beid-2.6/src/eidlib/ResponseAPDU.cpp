// ResponseAPDU.cpp: implementation of the CResponseAPDU class.
//
//////////////////////////////////////////////////////////////////////

#include "ResponseAPDU.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CResponseAPDU::CResponseAPDU()
{
}

CResponseAPDU::~CResponseAPDU()
{

}

/**
* Creates a new object of this type and initializes it with 
* the given apdu buffer. The internal buffer's length is set to the 
* length of the buffer passed.
*
* @param pucBuffer the byte array to be used for holding the APDU
* @param ulBufLength  the length of the APDU buffer
*
*/
CResponseAPDU::CResponseAPDU(unsigned char *pucBuffer, unsigned long ulBufLength) : CAPDU(pucBuffer, ulBufLength)
{
}

/**
* Creates a new re-usable APDU with a given buffer size.
* A new buffer with the given size is allocated. The length of the
* internally buffered APDU is set to 0.
*
* @param ulBufLength  the size of the buffer to create
*
*/
CResponseAPDU::CResponseAPDU(unsigned long ulBufLength) : CAPDU(ulBufLength)
{
    m_ulApduLength = ulBufLength;
}

/**
* Gets the value of <tt>SW1</tt> as a byte.
*
* @return    The value of SW1
*/
unsigned char CResponseAPDU::GetSW1()
{
    if (GetLength() > 1)
    {
        unsigned char *pBuf = GetBytes();
        return pBuf[GetLength() - 2];
    }
    return 0x00;
}

/**
* Gets the value of <tt>SW2</tt> as a byte.
*
* @return    The value of SW2
*/
unsigned char CResponseAPDU::GetSW2()
{
    if (GetLength() > 1)
    {
        unsigned char *pBuf = GetBytes();
        return pBuf[GetLength() - 1];
    }
    return 0x00;
}

/**
* Gets the length of APDU buffer without the SW bytes.
*
* @return    The length of the APDU buffer
*/
int CResponseAPDU::GetRespLength()
{
    return CAPDU::GetLength() - 2; 
}
