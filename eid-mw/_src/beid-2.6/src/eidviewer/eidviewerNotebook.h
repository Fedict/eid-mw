// eidviewerNotebook.h: interface for the eidviewerNotebook class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EIDVIEWERNOTEBOOK_H__CC439B05_03C2_4DFF_AA82_F9EB33B027C8__INCLUDED_)
#define AFX_EIDVIEWERNOTEBOOK_H__CC439B05_03C2_4DFF_AA82_F9EB33B027C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _WIN32
    #pragma warning(disable : 4786 4514)
#endif

#include "wx/notebook.h"
#include "wx/treectrl.h"
#include "eidlib.h"
#include "PrintOutData.h"
#include "Certif.h"
#include "Pin.h"

#include <openssl/ssl.h>


#include <vector>
#include <map>

#ifdef _WIN32
#include "./config/BelgianEIDConfig.h"

#include <wincrypt.h>

#define MY_ENCODING_TYPE  (X509_ASN_ENCODING | PKCS_7_ASN_ENCODING)
#define BELGIAN_EID_CARD 0x00000020

#define ERR_CREATE_CERT_CONTEXT                       0x8000000D
#define ERR_CANNOT_SET_PROPERTY                       0x8000000F
#define ERR_CANNOT_CREATE_MEM_STORE               0x80000010
#define ERR_CANNOT_ADD_CERTIFICATE_TO_STORE   0x80000011
#define ERR_CANNOT_CREATE_CONTEXT_FILE            0x80000012
#define ERR_CANNOT_SAVE_CONTEXT_FILE               0x80000013
#define ERR_CANNOT_WRITE_CONTEXT_FILE             0x80000014
#define ERR_CANCELLED_BY_USER                            0x80000015
#define ERR_CANNOT_SET_REG_KEY                         0x80000016
#define ERR_CANNOT_SAVE_CONFIGURATIONFILE       0x80000017

#include "verinfo.h"

#endif

class eidviewerNotebook  : public wxNotebook
{
public:
	eidviewerNotebook(wxWindow *parent, wxWindowID id,
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
	virtual ~eidviewerNotebook();

    wxPanel *CreatePage(int iID);
    void SetIDData(BEID_ID_Data *pIDData, long lSignature, BOOL bClear = FALSE);
    void SetAddressData(BEID_Address *pADData, long lSignature, BOOL bClear = FALSE);
    void SetPictureData(BEID_Bytes *pPicData, long lSignature, BOOL bClear = FALSE);
    void LoadStrings(int iLanguage);
    void ClearPages();

    void SetCertificateData(BEID_Certif_Check *pCertifs);
    void SetVersionInfoData(BEID_VersionInfo *pVersion);
    void SetPINData(std::map<wxString, std::vector<CPin> > & PinMap);
    void OnButtonPrint(wxCommandEvent& event);

private:
    wxPanel *CreateIdentityPage(int iID);
    wxPanel *CreateCertificatesPage(int iID);
    wxPanel *CreateCardDataPage(int iID);
    wxPanel *CreateOptionsPage(int iID);

    void FillDate(char *pszSource, wxString & strResult);
    void FillStatus(wxStaticBitmap *pWindow, long lSignature);
    void FillPrintData(CPrintOutData & oData);
    void ClearMap();
    void GetCertChain(X509 *cert, X509_STORE *store, STACK_OF(X509) **ppChain);
    wxTreeItemId FindData(wxTreeItemId hti, CCertif *pData);
    void FillTree();
    void InitControls();
    void SetTextFields(CCertif *pCertif) ;
    void VerifyCertErrorString(long lStatus, wxString & strError);
    void OnSelchangedTreeCertificates(wxTreeEvent& event); 
    void OnButtonCertificatesDetails(wxCommandEvent & event);
    void OnButtonRegister(wxCommandEvent & event) ;
    void FillListVersionInfo();
    void FillTreePins();
    void SetTextFieldsPinData(CPin *pPin);
    void OnSelchangedTreePins(wxTreeEvent& event); 
    void OnButtonCarddataChangepin(wxCommandEvent & event);
    void AdjustCRL();
    void AdjustOCSP();
    void OnDblClickedTreeCertificates(wxTreeEvent& event);
    void OnSelchangeComboOptionsOcspVal(wxCommandEvent & event);
    void OnSelchangeComboOptionsCrlVal(wxCommandEvent & event);
    void OnSelchangeComboOptionsReaderVal(wxCommandEvent & event);
    void OnOptionsLanguage(wxCommandEvent & event);
    void OnButtonOptionsApply();

#ifdef _WIN32
    int CreateContextFile();
    BOOL StoreUserCerts (PCCERT_CONTEXT pCertContext, BYTE KeyUsageBits, CCertif *pCertData);
    BOOL StoreAuthorityCerts(PCCERT_CONTEXT pCertContext, BYTE  KeyUsageBits);
    BOOL StoreConfigurationFile();
    BOOL StoreBelgianEIDConfigurationFile();
    void FillModuleVersionInfo();
    wxPanel *CreateVersionPage(int iID);
#endif

private:
    wxColour m_ColorBack;
    BEID_Bytes m_Picture;
    BEID_VersionInfo m_VersionInfo;

    std::map<wxString, CCertif *> m_oMapCertifs;
    typedef std::map<wxString, CCertif *>::iterator MapCertifsIt;
  	wxImageList *m_pCertTreeImgList;

    std::vector<wxString> m_vecVersionLabels;
    std::map<wxString, std::vector<CPin> > m_oMapPins;
  	wxImageList *m_pPINTreeImgList;

    DECLARE_EVENT_TABLE()
};

#endif // !defined(AFX_EIDVIEWERNOTEBOOK_H__CC439B05_03C2_4DFF_AA82_F9EB33B027C8__INCLUDED_)
