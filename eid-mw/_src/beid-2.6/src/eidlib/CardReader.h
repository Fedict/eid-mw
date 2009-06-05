// CardReader.h: interface for the CCardReader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CARDREADER_H__AE0557F3_8313_4F7A_BCD7_8F6166618038__INCLUDED_)
#define AFX_CARDREADER_H__AE0557F3_8313_4F7A_BCD7_8F6166618038__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "eidlib.h"

#ifdef _WIN32
    #pragma warning(disable:4786)
#endif

#include <string>

#define MAX_FILE_SIZE		5000

class CCardReader  
{
public:
	CCardReader();
	virtual ~CCardReader();

    virtual long GetHandle() { return m_hCard;}
    virtual void *GetCard() { return NULL; }
    virtual void *GetP15Card();
    virtual long EstablishContext(BEID_Status *ptStatus);
    virtual long ReleaseContext(BEID_Status *ptStatus);
    virtual long BeginTransaction(BEID_Status *ptStatus);
    virtual long EndTransaction(BEID_Status *ptStatus);
    virtual int CheckSW(BEID_Status *ptStatus);

    virtual const char *GetConnectedName() = 0;
    virtual long Connect(const char *pszReaderName, BEID_Status *ptStatus) = 0;
    virtual long Disconnect(BEID_Status *ptStatus) = 0;
    virtual long Transmit(const unsigned char *pucSend, int iSendLen, unsigned char *pucRecv, unsigned long *pulRecvLen, BEID_Status *ptStatus) = 0;
    virtual long ReadFile(const unsigned char *ucFile,  int iFileLen, unsigned char *pucOutput, unsigned long *pulOutLen, BEID_Status *ptStatus) = 0;
    virtual long SelectFile(const unsigned char *ucFile, int iFileLen, unsigned char ucP1, BEID_Status *ptStatus) = 0;
    virtual long ReadBinary(unsigned char *pucOutput, unsigned long *pulOutLen, BEID_Status *ptStatus, int iOffset = 0, int iCount = MAX_FILE_SIZE) = 0;
    virtual long UpdateBinary(unsigned char *pucInput, unsigned long ulInLen, BEID_Status *ptStatus) = 0;
    virtual long GetCardData(unsigned char *pucOutput, unsigned long *pulOutLen, bool bSigned, BEID_Status *ptStatus) = 0;
    virtual long GetPINStatus(unsigned char *pucOutput, unsigned long *pulOutLen, unsigned char ucPinReference, bool bSigned, BEID_Status *ptStatus) = 0;
    virtual long VerifyPIN(BEID_Pin *pPinData, const char *pszPin, long *piTriesLeft, BEID_Status *ptStatus) = 0;
    virtual long ChangePIN(BEID_Pin *pPinData, char *pszOldPin, char *pszNewPin, long *piTriesLeft, BEID_Status *ptStatus) = 0;
    virtual void FlushCache() = 0;
    virtual void SetRawData(BEID_Raw *pRawData) = 0;
    virtual long GetChallengeResponse(unsigned char *pucOutputCh, unsigned long *pulOutLenCh, 
        unsigned char *pucOutputResp, unsigned long *pulOutLenResp, BEID_Status *ptStatus) = 0;
    virtual long GetPINs(BEID_Pins *pPins, BEID_Status *ptStatus) = 0;

protected:
    long NoError(BEID_Status *ptStatus);

protected:
    long m_hCard;
};

#endif // !defined(AFX_CARDREADER_H__AE0557F3_8313_4F7A_BCD7_8F6166618038__INCLUDED_)
