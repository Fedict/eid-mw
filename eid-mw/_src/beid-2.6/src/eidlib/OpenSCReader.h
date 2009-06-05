// OpenSCReader.h: interface for the COpenSCReader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENSCREADER_H__F6AAE05F_23EF_4DB8_8E09_38286641913E__INCLUDED_)
#define AFX_OPENSCREADER_H__F6AAE05F_23EF_4DB8_8E09_38286641913E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _WIN32
#pragma warning(disable:4786 4503)
#endif

#include <map>
#include <string>

#include "CardReader.h"
#include "CommandAPDU.h"
#include "ResponseAPDU.h"

class COpenSCReader : public CCardReader  
{
public:
	COpenSCReader();
	virtual ~COpenSCReader();

    virtual const char *GetConnectedName();
    virtual long Connect(const char *pszReaderName, BEID_Status *ptStatus);
    virtual long Disconnect(BEID_Status *ptStatus);
    virtual long EstablishContext(BEID_Status *ptStatus);
    virtual long ReleaseContext(BEID_Status *ptStatus);
    virtual long Transmit(const unsigned char *pucSend, int iSendLen, unsigned char *pucRecv, unsigned long *pulRecvLen, BEID_Status *ptStatus);
    virtual long BeginTransaction(BEID_Status *ptStatus);
    virtual long EndTransaction(BEID_Status *ptStatus);
    virtual long ReadFile(const unsigned char *ucFile,  int iFileLen, unsigned char *pucOutput, unsigned long *pulOutLen, BEID_Status *ptStatus);
    virtual long SelectFile(const unsigned char *ucFile, int iFileLen, unsigned char ucP1, BEID_Status *ptStatus);
    virtual long ReadBinary(unsigned char *pszOutput, unsigned long *pulOutLen, BEID_Status *ptStatus, int iOffset = 0, int iCount = MAX_FILE_SIZE);
    virtual long UpdateBinary(unsigned char *pucInput, unsigned long ulInLen, BEID_Status *ptStatus);
    virtual long GetCardData(unsigned char *pucOutput, unsigned long *pulOutLen, bool bSigned, BEID_Status *ptStatus);
    virtual long GetPINStatus(unsigned char *pucOutput, unsigned long *pulOutLen, unsigned char ucPinReference, bool bSigned, BEID_Status *ptStatus);
    virtual long VerifyPIN(BEID_Pin *pPinData, const char *pszPin, long *piTriesLeft, BEID_Status *ptStatus);
    virtual long ChangePIN(BEID_Pin *pPinData, char *pszOldPin, char *pszNewPin, long *piTriesLeft, BEID_Status *ptStatus);
    virtual void *GetCard();
    virtual void FlushCache();
    virtual void SetRawData(BEID_Raw *pRawData);
    virtual long GetChallengeResponse(unsigned char *pucOutputCh, unsigned long *pulOutLenCh, unsigned char *pucOutputResp, unsigned long *pulOutLenResp, BEID_Status *ptStatus);
    virtual void *GetP15Card();
    virtual long GetPINs(BEID_Pins *pPins, BEID_Status *ptStatus);

protected:
    long SelectFile(const unsigned char *ucFile, int iFileLen, BEID_Status *ptStatus);
    long Transmit(CCommandAPDU *pCmd, CResponseAPDU *pResp);
    void CleanUp();

private:
    struct sc_pkcs15_pin_info *FindPIN(unsigned char ucPinReference);

private:
    struct sc_context *m_pCtx;
    struct sc_reader *m_pReader;
    struct sc_card *m_pCard;
    struct sc_pkcs15_card *m_p15Card;
};

#endif // !defined(AFX_OPENSCREADER_H__F6AAE05F_23EF_4DB8_8E09_38286641913E__INCLUDED_)
