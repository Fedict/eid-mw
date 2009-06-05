// ResponseAPDU.h: interface for the CResponseAPDU class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESPONSEAPDU_H__D887757B_2EDB_47B0_BCE3_038DC7B2A1FF__INCLUDED_)
#define AFX_RESPONSEAPDU_H__D887757B_2EDB_47B0_BCE3_038DC7B2A1FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "APDU.h"

class CResponseAPDU : public CAPDU  
{
public:
	CResponseAPDU();
	CResponseAPDU(unsigned long ulBufLength);
	CResponseAPDU(unsigned char *pucBuffer, unsigned long ulBufLength);
	virtual ~CResponseAPDU();

    int GetRespLength();
    unsigned char GetSW1();
    unsigned char GetSW2();
};

#endif // !defined(AFX_RESPONSEAPDU_H__D887757B_2EDB_47B0_BCE3_038DC7B2A1FF__INCLUDED_)
