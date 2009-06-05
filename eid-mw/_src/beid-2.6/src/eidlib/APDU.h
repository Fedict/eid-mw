// APDU.h: interface for the CAPDU class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APDU_H__6D5D4EE3_CBA0_4308_8A66_506C44DEB912__INCLUDED_)
#define AFX_APDU_H__6D5D4EE3_CBA0_4308_8A66_506C44DEB912__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../beidcommon/bytearray.h"

class CAPDU  
{
public:
    CAPDU();
	CAPDU(unsigned char *pucBuffer, unsigned long iBufLength);
    CAPDU(unsigned long ulSize);

	virtual ~CAPDU();

    void Append(unsigned char *pucBuffer, unsigned long ulBufLength);
    void Append(unsigned char ucByte);
    unsigned char GetByte(int iIndex);
    unsigned char * GetBytes();
    int GetLength();
    void SetByte(unsigned char ucByte, int iIndex);
    void SetLength(int iLength);
    void ClearBuffer();

protected:
    /** A buffer to hold the re-usable command APDU. */
    eidcommon::CByteArray m_oByteArr;
   /** The length of the command APDU currently in the buffer. */ 
    unsigned long m_ulApduLength;
};

#endif // !defined(AFX_APDU_H__6D5D4EE3_CBA0_4308_8A66_506C44DEB912__INCLUDED_)
