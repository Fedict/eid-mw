// VirtualReader.h: interface for the CVirtualReader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIRTUALREADER_H__F6AAE05F_23EF_4DB8_8E09_38286641913E__INCLUDED_)
#define AFX_VIRTUALREADER_H__F6AAE05F_23EF_4DB8_8E09_38286641913E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CardReader.h"
#include "../beidcommon/bytearray.h"

class CVirtualReader : public CCardReader  
{
public:
	CVirtualReader();
	virtual ~CVirtualReader();

    virtual const char *GetConnectedName();
    virtual long Connect(const char *pszReaderName, BEID_Status *ptStatus);
    virtual long Disconnect(BEID_Status *ptStatus);
    virtual long Transmit(const unsigned char *pucSend, int iSendLen, unsigned char *pucRecv, unsigned long *pulRecvLen, BEID_Status *ptStatus);
    virtual long ReadFile(const unsigned char *ucFile,  int iFileLen, unsigned char *pucOutput, unsigned long *pulOutLen, BEID_Status *ptStatus);
    virtual long SelectFile(const unsigned char *ucFile, int iFileLen, unsigned char ucP1, BEID_Status *ptStatus);
    virtual long ReadBinary(unsigned char *pszOutput, unsigned long *pulOutLen, BEID_Status *ptStatus, int iOffset = 0, int iCount = MAX_FILE_SIZE);
    virtual long UpdateBinary(unsigned char *pucInput, unsigned long ulInLen, BEID_Status *ptStatus);
    virtual long GetCardData(unsigned char *pucOutput, unsigned long *pulOutLen, bool bSigned, BEID_Status *ptStatus);
    virtual long GetPINStatus(unsigned char *pucOutput, unsigned long *pulOutLen, unsigned char ucPinReference, bool bSigned, BEID_Status *ptStatus);
    virtual long VerifyPIN(BEID_Pin *pPinData, const char *pszPin, long *piTriesLeft, BEID_Status *ptStatus);
    virtual long ChangePIN(BEID_Pin *pPinData, char *pszOldPin, char *pszNewPin, long *piTriesLeft, BEID_Status *ptStatus);
    virtual void FlushCache();
    virtual void SetRawData(BEID_Raw *pRawData);
    virtual long GetChallengeResponse(unsigned char *pucOutputCh, unsigned long *pulOutLenCh, unsigned char *pucOutputResp, unsigned long *pulOutLenResp, BEID_Status *ptStatus);
    virtual long GetPINs(BEID_Pins *pPins, BEID_Status *ptStatus);

protected:
    long SelectFile(const unsigned char *ucFile, int iFileLen, BEID_Status *ptStatus);
    void CleanUp();

private:
    eidcommon::CByteArray m_ByteID;
    eidcommon::CByteArray m_ByteSigID;
    eidcommon::CByteArray m_ByteAddress;
    eidcommon::CByteArray m_ByteSigAddress;
    eidcommon::CByteArray m_BytePicture;
    eidcommon::CByteArray m_ByteCardData;
    eidcommon::CByteArray m_ByteTokenInfo;
    eidcommon::CByteArray m_ByteRNCert;
    eidcommon::CByteArray m_Challenge;
    eidcommon::CByteArray m_Response;
    eidcommon::CByteArray m_SelectedFile;
};

#endif // !defined(AFX_VIRTUALREADER_H__F6AAE05F_23EF_4DB8_8E09_38286641913E__INCLUDED_)
