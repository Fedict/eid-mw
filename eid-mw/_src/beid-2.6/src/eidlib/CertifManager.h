// CertifManager.h: interface for the CCertifManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CERTIFMANAGER_H__E765E66E_B38C_4C4C_B98E_A35D430803C6__INCLUDED_)
#define AFX_CERTIFMANAGER_H__E765E66E_B38C_4C4C_B98E_A35D430803C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _WIN32
    #pragma warning(disable:4786)
#endif

#include <vector>
#include <algorithm>
#include "Certif.h"
#include "CardReader.h"
#include "eidlib.h"

class CCertifManager  
{
public:
	CCertifManager();
	virtual ~CCertifManager();
    void SetCardReader(CCardReader *pReader);
    long ReadCertifsP15(BEID_Status *ptStatus);
    void AddCertif(unsigned char *pucData, unsigned long ulDataLen, char *pszLabel);
    void FillCertifs(BEID_Certif_Check *pData);
    void Cleanup();
    int GetCertifCount() { return m_Certifs.size();}
    eidlib::CCertif *GetCertif(const char *pszID);
    long VerifyCertsCRL(bool bDownload = true );
    long VerifyCertsOCSP();
    void UpdateCertStatus(void *pCertX509, long lStatus);
    bool FindNotValidated();
    long ReadRNCertif(BEID_Status *ptStatus);

private:
    std::vector<eidlib::CCertif *> m_Certifs;
    typedef std::vector<eidlib::CCertif *>::iterator ItCertifs;
    CCardReader *m_pCardReader;
};

#endif // !defined(AFX_CERTIFMANAGER_H__E765E66E_B38C_4C4C_B98E_A35D430803C6__INCLUDED_)
