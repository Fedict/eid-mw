// APDU.cpp: implementation of the CAPDU class.
//
//////////////////////////////////////////////////////////////////////

#include "APDU.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAPDU::CAPDU()
{
    m_ulApduLength = 0;
}

CAPDU::~CAPDU()
{

}

/**
* Creates a new re-usable APDU and initializes it with 
* the given buffer.
* The buffer is assumed to hold an APDU. The length of the
* internally buffered APDU is set to <tt>ulBufLength</tt>.
*
* @param pucBuffer  the byte array to be used for holding the APDU
* @param ulBufLength  the length of the APDU currently in the buffer
*
*/
CAPDU::CAPDU(unsigned char *pucBuffer, unsigned long ulBufLength)
{
    m_oByteArr.Append(pucBuffer, ulBufLength);
    m_ulApduLength = ulBufLength;
}

/**
* Creates a new re-usable APDU with a given buffer size.
* A new buffer with the given size is allocated. The length of the
* internally buffered APDU is set to 0.
*
* @param ulSize  the size of the buffer to create
*
*/
CAPDU::CAPDU(unsigned long ulSize)
{
    m_oByteArr.SetSize(ulSize);
    m_ulApduLength = 0;    
}

/**
* Appends the given byte array to the internally buffered APDU.
*
* @param pucBuffer the byte array to be appended
* @param ulBufLength  the length of the APDU buffer to be appended
*
*/
void CAPDU::Append(unsigned char *pucBuffer, unsigned long ulBufLength)
{
    m_oByteArr.Append(pucBuffer, ulBufLength);
    m_ulApduLength += ulBufLength;    
}

/**
* Appends the given byte to the internally buffered APDU.
*
* @param ucByte the byte to be appended
*
*/
void CAPDU::Append(unsigned char ucByte)
{
    m_oByteArr.Append(&ucByte, 1); 
    m_ulApduLength++;
}

/**
* Gets the byte at the specified position in the buffer.
* This method can only be used to access the APDU currently stored.
* It is not possible to read beyond the end of the APDU.
*
* @param iIndex   the position in the buffer
* @return        the value at the given position,
*                or -1 if the position is invalid
*
*/
unsigned char CAPDU::GetByte(int iIndex)
{
    if ((unsigned long)iIndex < m_ulApduLength)
    {
        return m_oByteArr[iIndex]; 
    }
    return 0x00;
}

/**
* Returns the internal APDU buffer.
* This method allows complex manipulations of the buffered APDU,
* for example MAC calculation. If the length of the APDU is changed
* by such an operation, <tt>SetLength</tt> has to be used to store
* the new length.
*
* @return  the buffer that holds the current APDU
*
*/
unsigned char *CAPDU::GetBytes()
{
    return m_oByteArr.GetData(); 
}

/**
* Returns the length of the buffered APDU.
*
* @return  the length of the APDU currently stored
*/
int CAPDU::GetLength()
{
    return m_ulApduLength; 
}

/**
* Sets the byte at the specified position in the buffer.
* This method can only be used to <i>modify</i> an APDU already stored.
* It is not possible to set bytes beyond the end of the current APDU.
* The method will behave as a no-op if this happens.
* Use <tt>Append(byte)</tt> to extend the APDU.
*
* @param ucByte   the byte to store there
* @param iIndex   the position in the buffer
*
*/
void CAPDU::SetByte(unsigned char ucByte, int iIndex)
{
    if (iIndex < m_oByteArr.GetSize())
    {
        m_oByteArr.SetAt(ucByte, iIndex);
    }
}

/**
* Sets the length of valid range within the APDU buffer.
* This method can be used to cut off the end of the APDU.
* It can also be used to increase the size of the APDU. In this case,
* it is the caller's responsibility to fill the additional bytes with
* useful information.
*
* @param iLength new length of the valid range
*
*/
void CAPDU::SetLength(int iLength)
{
    if(iLength <= m_oByteArr.GetSize())
    {
        m_ulApduLength = iLength; 
    }
}

void CAPDU::ClearBuffer()
{
    m_ulApduLength = 0;
    m_oByteArr.RemoveAll();
}
