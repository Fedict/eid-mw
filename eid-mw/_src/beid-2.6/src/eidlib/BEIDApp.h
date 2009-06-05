// BEIDInit.h: interface for the CBEIDApp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BEIDINIT_H__FC542E74_A82A_4AD1_8115_842E20214495__INCLUDED_)
#define AFX_BEIDINIT_H__FC542E74_A82A_4AD1_8115_842E20214495__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifdef _WIN32
#pragma warning(disable:4786 4503)
#endif

#include "eidlib.h"
#include "../beidcommon/config.h"
#include "../beidcommon/TLVBuffer.h"

#include <qmutex.h> 
#include <qstring.h> 

#define BELPIC_MAX_USER_PIN_LEN     12

class CCardReader;
class CCertifManager;
class CAutoUpdate;

class CBEIDApp  
{
public:
	CBEIDApp(char *pszReaderName, int iOCSP, int iCRL);
	virtual ~CBEIDApp();

    // High Level
    BOOL Init(long *plHandle, BEID_Status *ptStatus);
    BOOL Exit(BEID_Status *ptStatus);
    BOOL GetID(BEID_ID_Data *ptData, BEID_Certif_Check *ptCertifCheck, BEID_Status *ptStatus, BEID_Raw *pRawData = NULL);
    BOOL GetAddress(BEID_Address *ptAddress, BEID_Certif_Check *ptCertifCheck, BEID_Status *ptStatus, BEID_Raw *pRawData = NULL);
    BOOL GetPicture(BEID_Bytes *ptPicture, BEID_Certif_Check *ptCertifCheck, BEID_Status *ptStatus, BEID_Raw *pRawData = NULL);
    BOOL GetRawData(BEID_Raw *pRawData, BEID_Status *ptStatus);
    BOOL SetRawData(BEID_Raw *pRawData, BEID_Status *ptStatus);
    BOOL GetCertificates(BEID_Certif_Check *ptCertifCheck, BEID_Status *ptStatus);
    BOOL GetRawFile(BEID_Bytes *pRawFile, BEID_Status *ptStatus);
    BOOL SetRawFile(BEID_Bytes *pRawFile, BEID_Status *ptStatus);

    // Mid Level
    BOOL BeginTransaction(BEID_Status *ptStatus);
    BOOL EndTransaction(BEID_Status *ptStatus);
    BOOL SelectApplication(BEID_Bytes *ptApplication, BEID_Status *ptStatus);
    BOOL GetVersionInfo(BEID_VersionInfo *ptVersionInfo, BOOL bSignature, BEID_Bytes *ptSignedStatus, BEID_Status *ptStatus, BEID_Raw *pRawData = NULL);
    BOOL GetPINStatus(BEID_Pin *pPinData, long *piTriesLeft, BOOL bSignature, BEID_Bytes *ptSignedStatus, BEID_Status *ptStatus);
    BOOL VerifyPIN(BEID_Pin *pPinData, const char *pszPin, long *piTriesLeft, BEID_Status *ptStatus);
    BOOL ChangePIN(BEID_Pin *pPinData, char *pszOldPin, char *pszNewPin, long *piTriesLeft, BEID_Status *ptStatus);
    BOOL ReadFile(BEID_Bytes *ptFileID, BEID_Bytes *ptOutData, BEID_Pin *pPinData, BEID_Status *ptStatus);
    BOOL WriteFile(BEID_Bytes *ptFileID, BEID_Bytes *ptInData, BEID_Pin *pPinData, BEID_Status *ptStatus);
    BOOL GetPINs(BEID_Pins *pPins, BEID_Status *ptStatus);
    BOOL VerifyCertificatesCRL(BEID_Certif_Check *ptCertifCheck, BOOL bDownload, BEID_Status *ptStatus);
    BOOL VerifyCertificatesOCSP(BEID_Certif_Check *ptCertifCheck, BEID_Status *ptStatus);

    // Low Level
    BOOL FlushCache(BEID_Status *ptStatus);
    BOOL SendAPDU(BEID_Bytes *ptCmdAPDU, BEID_Pin *pPinData, BEID_Bytes *ptRespAPDU, BEID_Status *ptStatus);
    BOOL ReadBinary(BEID_Bytes *ptFileID, int iOffset, int iCount, BEID_Bytes *ptOutData, BEID_Status *ptStatus);

    // Other
    CCertifManager *GetCertifManager() { return m_pCertifManager; }
    static int AskDownload(QString & strMessage);
    static int GetLang() { return m_iLang; }

private:
    CCardReader *CreateReader();
    void FillIDData(eidcommon::CTLVBuffer & oTLVBuffer, BEID_ID_Data *ptData);
    void FillAddressData(eidcommon::CTLVBuffer & oTLVBuffer, BEID_Address *ptData);
    long VerifySignatureID(unsigned char *pData, unsigned long ulLenData, BEID_Certif_Check *ptCertifCheck, BEID_Status *ptStatus, BEID_Raw *pRawData = NULL);
    long VerifySignatureAddress(unsigned char *pData, unsigned long ulLenData, BEID_Certif_Check *ptCertifCheck, BEID_Status *ptStatus, BEID_Raw *pRawData = NULL);
    long VerifyHashPicture(unsigned char *pData, unsigned long ulLenData, BEID_Certif_Check *ptCertifCheck, BEID_Status *ptStatus, BEID_Raw *pRawData = NULL);
    long ReadCertificates(BEID_Certif_Check *ptCertifCheck, BEID_Status *ptStatus, bool bAll = false);
    long ReadHashData(unsigned char ucPicHash[20], BEID_Status *ptStatus);
    long GetTokenInfo(BEID_VersionInfo *ptVersionInfo, BEID_Status *ptStatus, BEID_Raw *pRawData = NULL);
    long VerifyCertificates(BEID_Certif_Check *ptCertifCheck);
    BOOL FireWallAllowed(char *pszType);
    void FillProcessName();
    bool CheckVersion(BEID_VersionInfo *ptVersionInfo);
    int AskTestCard();
    void MapLanguage();
    bool TestSignature(long lSignatureCheck);

protected:
    std::string m_szReaderName;
    std::string m_szProcessName;
    int m_iOCSP;
    int m_iCRL;
    bool m_bShowRootWarning;
    bool m_bAllowTestRoot;

private:
    CCardReader *m_pReader;
    CCertifManager *m_pCertifManager;
    static QMutex m_cs;
    CAutoUpdate *m_pAutoUpdate;
    static int m_iLang;
};

#endif // !defined(AFX_BEIDINIT_H__FC542E74_A82A_4AD1_8115_842E20214495__INCLUDED_)
