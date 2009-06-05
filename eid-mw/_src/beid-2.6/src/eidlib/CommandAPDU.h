// CommandAPDU.h: interface for the CCommandAPDU class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMANDAPDU_H__A9A5411D_FCB8_4B4B_BDDB_FB8EA602233A__INCLUDED_)
#define AFX_COMMANDAPDU_H__A9A5411D_FCB8_4B4B_BDDB_FB8EA602233A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "APDU.h"

class CCommandAPDU : public CAPDU  
{
public:
	CCommandAPDU();
    CCommandAPDU(unsigned long ulSize);

    /* Constructs an ISO 7816-4 case 1 APDU command */
    CCommandAPDU(unsigned char cla, unsigned char ins, unsigned char p1, unsigned char p2);
    /* Constructs an ISO 7816-4 case 2 APDU command */
    CCommandAPDU(unsigned char cla, unsigned char ins, unsigned char p1, unsigned char p2, unsigned char le);
    /* Constructs an ISO 7816-4 case 3 APDU command */
    CCommandAPDU(unsigned char cla, unsigned char ins, unsigned char p1, unsigned char p2, unsigned char *pData, unsigned long ulDataLen);
    /* Constructs an ISO 7816-4 case 4 APDU command */
    CCommandAPDU(unsigned char cla, unsigned char ins, unsigned char p1, unsigned char p2, unsigned char *pData, unsigned long ulDataLen, unsigned char le);

	virtual ~CCommandAPDU();

    void SetClassByte(unsigned char cla);
    unsigned char GetClassByte();
    void SetInstructionByte(unsigned char ins);
    unsigned char GetInstructionByte();
    void SetParameter1Byte(unsigned char p1);
    unsigned char GetParameter1Byte();
    void SetParameter2Byte(unsigned char p1);
    unsigned char GetParameter2Byte();
    void SetInputDataBytes(unsigned char *pData, unsigned long ulDataLen);
    unsigned char *GetInputDataBytes();
    unsigned long GetInputDataBytesLen();
    void SetLeByte(unsigned char Le);
    unsigned char GetLeByte();
    short GetCase();
    void SetCase(short sCase);
    void Flush();

private:
    unsigned char m_CLA;
    unsigned char m_INS;
    unsigned char m_P1;
    unsigned char m_P2;
    eidcommon::CByteArray m_Data;
    unsigned char m_Le;
    short m_Case;
};

#endif // !defined(AFX_COMMANDAPDU_H__A9A5411D_FCB8_4B4B_BDDB_FB8EA602233A__INCLUDED_)
