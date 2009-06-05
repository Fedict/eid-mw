// eidviewerNotebook.cpp: implementation of the eidviewerNotebook class.
//
//////////////////////////////////////////////////////////////////////

#ifdef _WIN32
    #pragma warning(disable : 4786 4514)
#endif

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include <wx/image.h>
#include <wx/statline.h>
#include <wx/spinbutt.h>
#include <wx/spinctrl.h>
#include <wx/splitter.h>
#include <wx/listctrl.h>
#include <wx/treectrl.h>
#include <wx/notebook.h>
#include <wx/grid.h>
#include <wx/datetime.h>
#include <wx/mstream.h>
#include <wx/printdlg.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/mimetype.h> // mimetype support

/*
#ifndef _WIN32
#if !wxUSE_POSTSCRIPT
#undef wxUSE_POSTSCRIPT
#define wxUSE_POSTSCRIPT 1
#endif
#endif
*/


#include "eidviewerApp.h"
#include "eidviewerNotebook.h"
#include "eidviewerTypes.h"
#include "PrintOut.h"

#include <opensc/opensc.h>
#include <openssl/opensslv.h>

#if OPENSSL_VERSION_NUMBER >= 0x00908000L
#define OPENSSL_CAST(a) const_cast<const unsigned char **>((a))
#else
#define OPENSSL_CAST(a) static_cast<unsigned char **>((a))
#endif

#ifdef _WIN32
#define STR_DUTCH_BEL wxT("BELGIË")
#else
#define STR_DUTCH_BEL wxT("BELGIE")
#endif
#define STR_DUTCH_IDEN wxT("IDENTITEITSKAART")
#define STR_FRENCH_BEL wxT("BELGIQUE")
#define STR_FRENCH_IDEN wxT("CARTE D'IDENTITE")
#define STR_GERMAN_BEL wxT("BELGIEN")
#define STR_GERMAN_IDEN wxT("PERSONALAUSWEIS")
#define STR_ENGLISH_BEL wxT("BELGIUM")
#define STR_ENGLISH_IDEN wxT("IDENTITY CARD")

#define FRAME_WIDTH 810

class wxTreeItemCertif : public wxTreeItemData
{
public:
    wxTreeItemCertif(CCertif *pCertif)
    {
        m_pCertif = pCertif;
    }
    virtual ~wxTreeItemCertif() {}
    CCertif * GetCertif() { return m_pCertif; }
private:
    CCertif *m_pCertif;
};

class wxTreeItemPin : public wxTreeItemData
{
public:
    wxTreeItemPin(CPin *pPin)
    {
        m_pPin = pPin;
    }
    virtual ~wxTreeItemPin() 
    {
        if(m_pPin != NULL)
        {
            delete m_pPin;
            m_pPin = NULL;
        }
    }

    CPin * GetPin() { return m_pPin; }
private:
    CPin *m_pPin;
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(eidviewerNotebook, wxNotebook)
    EVT_BUTTON(IDC_BUTTONCERTIFICATES_DETAILS, eidviewerNotebook::OnButtonCertificatesDetails)
    EVT_BUTTON(IDC_BUTTON_REGISTER, eidviewerNotebook::OnButtonRegister)
    EVT_BUTTON(IDC_BUTTON_CARDDATA_CHANGEPIN, eidviewerNotebook::OnButtonCarddataChangepin)
    EVT_TREE_SEL_CHANGED(IDC_TREE_CERTIFICATES, eidviewerNotebook::OnSelchangedTreeCertificates)
    EVT_TREE_SEL_CHANGED(IDC_TREE_CARDDATA_PIN, eidviewerNotebook::OnSelchangedTreePins)
    EVT_TREE_ITEM_ACTIVATED(IDC_TREE_CERTIFICATES, eidviewerNotebook::OnDblClickedTreeCertificates)
    EVT_CHOICE(IDC_COMBO_OPTIONS_OCSP_VAL, eidviewerNotebook::OnSelchangeComboOptionsOcspVal)
    EVT_CHOICE(IDC_COMBO_OPTIONS_CRL_VAL, eidviewerNotebook::OnSelchangeComboOptionsCrlVal)
    EVT_CHOICE(IDC_COMBO_OPTIONS_READER_VAL, eidviewerNotebook::OnSelchangeComboOptionsReaderVal)
    EVT_RADIOBOX(IDC_RADIOBOX_OPTIONS_LANGUAGE, eidviewerNotebook::OnOptionsLanguage)
END_EVENT_TABLE()

eidviewerNotebook::eidviewerNotebook(wxWindow *parent, wxWindowID id,
     const wxPoint& pos, const wxSize& size, long style)
    : wxNotebook(parent, id, pos, size, style), m_ColorBack(135,207,163) //m_ColorBack(128,255,128)
{
    memset(&m_Picture, 0, sizeof(BEID_Bytes));
    memset(&m_VersionInfo, 0, sizeof(BEID_VersionInfo));
    m_pCertTreeImgList = new wxImageList(16,16);
#ifdef _WIN32
    m_pCertTreeImgList->Add(eidBitmapsFunc(16), *wxRED);
    m_pCertTreeImgList->Add(eidBitmapsFunc(17), *wxRED);
#else
    m_pCertTreeImgList->Add(eidBitmapsFunc(16));
    m_pCertTreeImgList->Add(eidBitmapsFunc(17));
#endif

    m_pPINTreeImgList = new wxImageList(16,16);
#ifdef _WIN32
    m_pPINTreeImgList->Add(eidBitmapsFunc(16), *wxRED);
    m_pPINTreeImgList->Add(eidBitmapsFunc(17), *wxRED);
#else
    m_pPINTreeImgList->Add(eidBitmapsFunc(16));
    m_pPINTreeImgList->Add(eidBitmapsFunc(17));
#endif
}

eidviewerNotebook::~eidviewerNotebook()
{
    if(m_Picture.length > 0)
    {
        delete [] m_Picture.data;
    }
    memset(&m_Picture, 0, sizeof(BEID_Bytes));
    ClearMap();
    wxTreeCtrl *pCtrlTreeCerts = (wxTreeCtrl *)FindWindow(IDC_TREE_CERTIFICATES);
    if(pCtrlTreeCerts != NULL)
    {
        pCtrlTreeCerts->DeleteAllItems();
    }
    m_vecVersionLabels.clear();

}

wxPanel *eidviewerNotebook::CreatePage(int iID)
{
    switch(iID)
    {
    case IDD_PAGE_IDENTITY:
        return CreateIdentityPage(iID);
        break;
    case IDD_PAGE_CERTIFICATES:
        return CreateCertificatesPage(iID);
        break;
    case IDD_PAGE_CARDDATA:
        return CreateCardDataPage(iID);
        break;
    case IDD_PAGE_OPTIONS:
        return CreateOptionsPage(iID);
        break;
#ifdef _WIN32
    case IDD_PAGE_VERSIONINFO:
        return CreateVersionPage(iID);
        break;
#endif
    }
    return NULL;
}

wxPanel *eidviewerNotebook::CreateIdentityPage(int iID)
{
    wxPanel *panel = new wxPanel(this, iID);
    wxPanel *parent = panel;

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxFlexGridSizer *item1 = new wxFlexGridSizer( 7, 0, 0 );
    item1->AddGrowableCol( 0 );
    item1->AddGrowableCol( 2 );
    item1->AddGrowableCol( 4 );
    item1->AddGrowableCol( 6 );

    wxBoxSizer *item2 = new wxBoxSizer( wxVERTICAL );

#ifdef _WIN32
    wxStaticText *item3 = new wxStaticText( parent, IDC_STATIC_IDENTITY_HEADER1, wxT("BELGIË"), wxDefaultPosition, wxDefaultSize, 0 );
#else
    wxStaticText *item3 = new wxStaticText( parent, IDC_STATIC_IDENTITY_HEADER1, wxT("BELGIE"), wxDefaultPosition, wxDefaultSize, 0 );
#endif
    item2->Add( item3, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxBOTTOM, 5 );

    wxStaticText *item4 = new wxStaticText( parent, IDC_STATIC_IDENTITY_HEADER5, wxT("IDENTITEITSKAART"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item4, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    item1->Add( item2, 0, wxGROW|wxALL, 5 );

    item1->Add( 5, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxBoxSizer *item5 = new wxBoxSizer( wxVERTICAL );

    wxStaticText *item6 = new wxStaticText( parent, IDC_STATIC_IDENTITY_HEADER2, wxT("BELGIQUE"), wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( item6, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxBOTTOM, 5 );

    wxStaticText *item7 = new wxStaticText( parent, IDC_STATIC_IDENTITY_HEADER6, wxT("CARTE D'IDENTITE"), wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( item7, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    item1->Add( item5, 0, wxGROW|wxALL, 5 );

    item1->Add( 5, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxBoxSizer *item8 = new wxBoxSizer( wxVERTICAL );

    wxStaticText *item9 = new wxStaticText( parent, IDC_STATIC_IDENTITY_HEADER3, wxT("BELGIEN"), wxDefaultPosition, wxDefaultSize, 0 );
    item8->Add( item9, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxBOTTOM, 5 );

    wxStaticText *item10 = new wxStaticText( parent, IDC_STATIC_IDENTITY_HEADER7, wxT("PERSONALAUSWEIS"), wxDefaultPosition, wxDefaultSize, 0 );
    item8->Add( item10, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    item1->Add( item8, 0, wxGROW|wxALL, 5 );

    item1->Add( 5, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxBoxSizer *item11 = new wxBoxSizer( wxVERTICAL );

    wxStaticText *item12 = new wxStaticText( parent, IDC_STATIC_IDENTITY_HEADER4, wxT("BELGIUM"), wxDefaultPosition, wxDefaultSize, 0 );
    item11->Add( item12, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxBOTTOM, 5 );

    wxStaticText *item13 = new wxStaticText( parent, IDC_STATIC_IDENTITY_HEADER8, wxT("IDENTITY CARD"), wxDefaultPosition, wxDefaultSize, 0 );
    item11->Add( item13, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    item1->Add( item11, 0, wxGROW|wxALL, 5 );

    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxStaticLine *item14 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    item0->Add( item14, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxBOTTOM, 5 );

    wxFlexGridSizer *item15 = new wxFlexGridSizer( 3, 0, 0 );
    item15->AddGrowableCol( 0 );
    item15->AddGrowableCol( 2 );

    wxStaticBox *item17 = new wxStaticBox( parent, -1, wxT("") );
    wxStaticBoxSizer *item16 = new wxStaticBoxSizer( item17, wxVERTICAL );

    wxBoxSizer *item18 = new wxBoxSizer( wxHORIZONTAL );

    item16->Add( item18, 0, wxALIGN_CENTER_VERTICAL, 5 );

    item16->Add( 10, 20, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item20 = new wxBoxSizer( wxHORIZONTAL );

    wxBitmapButton *item21 = new wxBitmapButton( parent, IDC_BUTTON_BITMAP_ARROW, eidBitmapsFunc( 3 ), wxDefaultPosition, wxDefaultSize );
    item21->SetToolTip( _("Clear Screen") );
    item20->Add( item21, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    wxBitmapButton *item22 = new wxBitmapButton( parent, IDC_BUTTON_CHIP, eidBitmapsFunc( 2 ), wxDefaultPosition, wxDefaultSize );
    item22->SetToolTip( _("Read Card") );
    item20->Add( item22, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    item20->Add( 5, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

    item16->Add( item20, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    item15->Add( item16, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL, 5 );

    wxStaticBitmap *item23 = new wxStaticBitmap( parent, IDC_STATIC_STATUS_IDENTITY, eidBitmapsFunc( 6 ), wxDefaultPosition, wxDefaultSize );
    item23->Show(FALSE);
    item15->Add( item23, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

    wxBoxSizer *item24 = new wxBoxSizer( wxVERTICAL );

    wxStaticBox *item26 = new wxStaticBox( parent, IDC_STATIC_IDENTITY_GROUP, _("Identity") );
    wxStaticBoxSizer *item25 = new wxStaticBoxSizer( item26, wxVERTICAL );

    wxStaticText *item27 = new wxStaticText( parent, IDC_STATIC_IDENTITY_NAME, _("Name"), wxDefaultPosition, wxSize(20,-1), 0 );
    item25->Add( item27, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxTOP, 5 );

    wxTextCtrl *item28 = new wxTextCtrl( parent, IDC_STATIC_IDENTITY_NAME_VAL, wxT(""), wxDefaultPosition, wxSize(200,-1), wxTE_READONLY | wxNO_BORDER );
    item25->Add( item28, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxStaticText *item29 = new wxStaticText( parent, IDC_STATIC_IDENTITY_GIVENNAMES, _("Givennames"), wxDefaultPosition, wxSize(20,-1), 0 );
    item25->Add( item29, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxFlexGridSizer *item30 = new wxFlexGridSizer( 1, 0, 0, 5 );
    item30->AddGrowableCol( 0 );
    item30->AddGrowableRow( 0 );

    wxTextCtrl *item31 = new wxTextCtrl( parent, IDC_STATIC_IDENTITY_GIVENNAMES_VAL1, wxT(""), wxDefaultPosition, wxSize(200,-1), wxTE_READONLY | wxNO_BORDER );
    item30->Add( item31, 0, wxGROW, 5 );

    wxTextCtrl *item32 = new wxTextCtrl( parent, IDC_STATIC_IDENTITY_GIVENNAMES_VAL3, wxT(""), wxDefaultPosition, wxSize(20,-1), wxTE_READONLY | wxNO_BORDER );
    item30->Add( item32, 0, wxGROW, 5 );

    item25->Add( item30, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxFlexGridSizer *item33 = new wxFlexGridSizer( 4, 0, 5 );
    item33->AddGrowableCol( 0 );
    item33->AddGrowableCol( 1 );
    item33->AddGrowableCol( 2 );
    item33->AddGrowableCol( 3 );
    item33->AddGrowableRow( 0 );
    item33->AddGrowableRow( 1 );

    wxStaticText *item34 = new wxStaticText( parent, IDC_STATIC_IDENTITY_BIRTHPLACE, _("Place of Birth"), wxDefaultPosition, wxSize(20,-1), 0 );
    item33->Add( item34, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxStaticText *item35 = new wxStaticText( parent, IDC_STATIC_IDENTITY_BIRTHDATE, _("Date of Birth"), wxDefaultPosition, wxSize(20,-1), 0 );
    item33->Add( item35, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxStaticText *item36 = new wxStaticText( parent, IDC_STATIC_IDENTITY_SEX, _("Sex"), wxDefaultPosition, wxSize(20,-1), 0 );
    item33->Add( item36, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxStaticText *item37 = new wxStaticText( parent, IDC_STATIC_IDENTITY_NATIONALITY, _("Nationality"), wxDefaultPosition, wxSize(20,-1), 0 );
    item33->Add( item37, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxTextCtrl *item38 = new wxTextCtrl( parent, IDC_STATIC_IDENTITY_BIRTHPLACE_VAL, wxT(""), wxDefaultPosition, wxSize(120,-1), wxTE_READONLY | wxNO_BORDER );
    item33->Add( item38, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxTextCtrl *item39 = new wxTextCtrl( parent, IDC_STATIC_IDENTITY_BIRTHDATE_VAL, wxT(""), wxDefaultPosition, wxSize(80,-1), wxTE_READONLY | wxNO_BORDER );
    item33->Add( item39, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxTextCtrl *item40 = new wxTextCtrl( parent, IDC_STATIC_IDENTITY_SEX_VAL, wxT(""), wxDefaultPosition, wxSize(30,-1), wxTE_READONLY | wxNO_BORDER );
    item33->Add( item40, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxTextCtrl *item41 = new wxTextCtrl( parent, IDC_STATIC_IDENTITY_NATIONALITY_VAL, wxT(""), wxDefaultPosition, wxSize(50,-1), wxTE_READONLY | wxNO_BORDER );
    item33->Add( item41, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    item25->Add( item33, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxFlexGridSizer *item42 = new wxFlexGridSizer( 2, 0, 5 );
    item42->AddGrowableCol( 0 );
    item42->AddGrowableCol( 1 );

    wxStaticText *item43 = new wxStaticText( parent, IDC_STATIC_IDENTITY_TITLE, _("Title"), wxDefaultPosition, wxSize(20,-1), 0 );
    item42->Add( item43, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxStaticText *item44 = new wxStaticText( parent, IDC_STATIC_IDENTITY_NATIONALNR, _("National Number"), wxDefaultPosition, wxSize(20,-1), 0 );
    item42->Add( item44, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxTextCtrl *item45 = new wxTextCtrl( parent, IDC_STATIC_IDENTITY_TITLE_VAL, wxT(""), wxDefaultPosition, wxSize(80,-1), wxTE_READONLY | wxNO_BORDER );
    item42->Add( item45, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxTextCtrl *item46 = new wxTextCtrl( parent, IDC_STATIC_IDENTITY_NATIONALNR_VAL, wxT(""), wxDefaultPosition, wxSize(20,-1), wxTE_READONLY | wxNO_BORDER );
    item42->Add( item46, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    item25->Add( item42, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    item24->Add( item25, 0, wxGROW, 35 );

    item15->Add( item24, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL, 5 );

    wxStaticBox *item48 = new wxStaticBox( parent, IDC_STATIC_CARDINFO_GROUP, _("Card Info") );
    wxStaticBoxSizer *item47 = new wxStaticBoxSizer( item48, wxVERTICAL );

    wxStaticText *item49 = new wxStaticText( parent, IDC_STATIC_CARDINFO_CHIPNR, _("Chip Number"), wxDefaultPosition, wxSize(20,-1), 0 );
    item47->Add( item49, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxTOP, 5 );

    wxTextCtrl *item50 = new wxTextCtrl( parent, IDC_STATIC_CARDINFO_CHIPNR_VAL, wxT(""), wxDefaultPosition, wxSize(140,-1), wxTE_READONLY | wxNO_BORDER );
    item47->Add( item50, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxStaticText *item51 = new wxStaticText( parent, IDC_STATIC_CARDINFO_CARDNR, _("Card Number"), wxDefaultPosition, wxSize(20,-1), 0 );
    item47->Add( item51, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxTextCtrl *item52 = new wxTextCtrl( parent, IDC_STATIC_CARDINFO_CARDNR_VAL, wxT(""), wxDefaultPosition, wxSize(80,-1), wxTE_READONLY | wxNO_BORDER );
    item47->Add( item52, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxFlexGridSizer *item53 = new wxFlexGridSizer( 2, 0, 5 );
    item53->AddGrowableCol( 0 );
    item53->AddGrowableCol( 1 );

    wxStaticText *item54 = new wxStaticText( parent, IDC_STATIC_CARDINFO_VALIDFROM, _("Valid From"), wxDefaultPosition, wxSize(20,-1), 0 );
    item53->Add( item54, 0, wxGROW, 5 );

    wxStaticText *item55 = new wxStaticText( parent, IDC_STATIC_CARDINFO_VALIDUNTIL, _("Until"), wxDefaultPosition, wxSize(20,-1), 0 );
    item53->Add( item55, 0, wxGROW, 5 );

    wxTextCtrl *item56 = new wxTextCtrl( parent, IDC_STATIC_CARDINFO_VALIDFROM_VAL, wxT(""), wxDefaultPosition, wxSize(80,-1), wxTE_READONLY | wxNO_BORDER );
    item53->Add( item56, 0, wxGROW, 5 );

    wxTextCtrl *item57 = new wxTextCtrl( parent, IDC_STATIC_CARDINFO_VALIDUNTIL_VAL, wxT(""), wxDefaultPosition, wxSize(80,-1), wxTE_READONLY | wxNO_BORDER );
    item53->Add( item57, 0, wxGROW, 5 );

    item47->Add( item53, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxStaticText *item58 = new wxStaticText( parent, IDC_STATIC_CARDINFO_MUNICIPALITY, _("Issuing Municipality"), wxDefaultPosition, wxSize(20,-1), 0 );
    item47->Add( item58, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxTextCtrl *item59 = new wxTextCtrl( parent, IDC_STATIC_CARDINFO_MUNICIPALITY_VAL, wxT(""), wxDefaultPosition, wxSize(80,-1), wxTE_READONLY | wxNO_BORDER );
    item47->Add( item59, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    item15->Add( item47, 0, wxGROW, 5 );

    wxStaticBitmap *item60 = new wxStaticBitmap( parent, IDC_STATIC_STATUS_ADDRESS, eidBitmapsFunc( 6 ), wxDefaultPosition, wxDefaultSize );
    item60->Show(FALSE);
    item15->Add( item60, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

    wxBoxSizer *item61 = new wxBoxSizer( wxVERTICAL );

    wxStaticBox *item63 = new wxStaticBox( parent, IDC_STATIC_ADDRESS_GROUP, _("Address") );
    wxStaticBoxSizer *item62 = new wxStaticBoxSizer( item63, wxVERTICAL );

    wxStaticText *item64 = new wxStaticText( parent, IDC_STATIC_ADDRESS_STREET, _("Street"), wxDefaultPosition, wxDefaultSize, 0 );
    item62->Add( item64, 0, wxALIGN_CENTER_VERTICAL|wxTOP, 5 );

    wxTextCtrl *item65 = new wxTextCtrl( parent, IDC_STATIC_ADDRESS_STREET_VAL, wxT(""), wxDefaultPosition, wxSize(150,-1), wxTE_READONLY | wxNO_BORDER );
    item62->Add( item65, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxFlexGridSizer *item66 = new wxFlexGridSizer( 3, 0, 5 );
    item66->AddGrowableCol( 1 );
    item66->AddGrowableRow( 1 );

    wxStaticText *item67 = new wxStaticText( parent, IDC_STATIC_ADDRESS_ZIP, _("Postal Code"), wxDefaultPosition, wxDefaultSize, 0 );
    item66->Add( item67, 0, wxALIGN_CENTER_VERTICAL, 5 );

    wxStaticText *item68 = new wxStaticText( parent, IDC_STATIC_ADDRESS_MUNICIPALITY, _("Municipality"), wxDefaultPosition, wxDefaultSize, 0 );
    item66->Add( item68, 0, wxALIGN_CENTER_VERTICAL, 5 );

    wxStaticText *item69 = new wxStaticText( parent, IDC_STATIC_ADDRESS_COUNTRY, _("Country"), wxDefaultPosition, wxDefaultSize, 0 );
    item66->Add( item69, 0, wxALIGN_CENTER_VERTICAL, 5 );

    wxTextCtrl *item70 = new wxTextCtrl( parent, IDC_STATIC_ADDRESS_ZIP_VAL, wxT(""), wxDefaultPosition, wxSize(80,-1), wxTE_READONLY | wxNO_BORDER );
    item66->Add( item70, 0, wxALIGN_CENTER_VERTICAL, 5 );

    wxTextCtrl *item71 = new wxTextCtrl( parent, IDC_STATIC_ADDRESS_MUNICIPALITY_VAL, wxT(""), wxDefaultPosition, wxSize(80,-1), wxTE_READONLY | wxNO_BORDER );
    item66->Add( item71, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxTextCtrl *item72 = new wxTextCtrl( parent, IDC_STATIC_ADDRESS_COUNTRY_VAL, wxT(""), wxDefaultPosition, wxSize(80,-1), wxTE_READONLY | wxNO_BORDER );
    item66->Add( item72, 0, wxALIGN_CENTER_VERTICAL, 5 );

    item62->Add( item66, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    item61->Add( item62, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxBoxSizer *item73 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticBox *item75 = new wxStaticBox( parent, IDC_STATIC_SPECIAL_GROUP, _("Special Status") );
    wxStaticBoxSizer *item74 = new wxStaticBoxSizer( item75, wxVERTICAL );

    wxBoxSizer *item76 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticBitmap *item77 = new wxStaticBitmap( parent, IDC_CHECK_SPECIALSTATUS_WHITE_VAL, eidBitmapsFunc( 12 ), wxDefaultPosition, wxDefaultSize );
    item76->Add( item77, 0, wxALIGN_CENTRE, 5 );

    wxStaticText *item78 = new wxStaticText( parent, IDC_STATIC_SPECIALSTATUS_WHITE, _("White Cane                    "), wxDefaultPosition, wxDefaultSize, 0 );
    item76->Add( item78, 0, wxALIGN_CENTRE|wxLEFT, 5 );

    item74->Add( item76, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item79 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticBitmap *item80 = new wxStaticBitmap( parent, IDC_CHECK_SPECIALSTATUS_YELLOW_VAL, eidBitmapsFunc( 12 ), wxDefaultPosition, wxDefaultSize );
    item79->Add( item80, 0, wxALIGN_CENTRE, 5 );

    wxStaticText *item81 = new wxStaticText( parent, IDC_STATIC_SPECIALSTATUS_YELLOW, _("Yellow Cane                    "), wxDefaultPosition, wxDefaultSize, 0 );
    item79->Add( item81, 0, wxALIGN_CENTRE|wxLEFT, 5 );

    item74->Add( item79, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item82 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticBitmap *item83 = new wxStaticBitmap( parent, IDC_CHECK_SPECIALSTATUS_MINORITY_VAL, eidBitmapsFunc( 12 ), wxDefaultPosition, wxDefaultSize );
    item82->Add( item83, 0, wxALIGN_CENTRE, 5 );

    wxStaticText *item84 = new wxStaticText( parent, IDC_STATIC_SPECIALSTATUS_MINORITY, _("Extended Minority                    "), wxDefaultPosition, wxDefaultSize, 0 );
    item82->Add( item84, 0, wxALIGN_CENTRE|wxLEFT, 5 );

    item74->Add( item82, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item73->Add( item74, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL, 5 );

    wxBoxSizer *item85 = new wxBoxSizer( wxHORIZONTAL );



    /*
    item85->Add( 5, 20, 0, wxALIGN_CENTRE|wxALL|wxGROW, 5 );

    wxStaticBitmap *item86 = new wxStaticBitmap( parent, IDC_STATIC_BITMAP_BELGIUM, eidBitmapsFunc( 0 ), wxDefaultPosition, wxSize(90,85) );
    item85->Add( item86, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    item85->Add( 5, 20, 0, wxALIGN_CENTRE|wxALL|wxGROW, 5 );

    wxBoxSizer *item87 = new wxBoxSizer( wxVERTICAL );

    item87->Add( 20, 50, 0, wxALIGN_CENTRE|wxALL|wxGROW, 5 );

    wxStaticBitmap *item88 = new wxStaticBitmap( parent, IDC_STATIC_BITMAP_LOGO, eidBitmapsFunc( 1 ), wxDefaultPosition, wxSize(90,90) );
    item87->Add( item88, 0, wxALIGN_CENTER_VERTICAL, 5 );

    item85->Add( item87, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL, 5 );

    item85->Add( 20, 20, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    wxStaticBitmap *item89 = new wxStaticBitmap( parent, IDC_STATIC_STATUS_PICTURE, eidBitmapsFunc( 6 ), wxDefaultPosition, wxDefaultSize );
    item89->Show(FALSE);
    item85->Add( item89, 0, wxALIGN_RIGHT, 5 );

    wxStaticBitmap *item90 = new wxStaticBitmap( parent, IDC_STATIC_PICTURE_CARD, eidBitmapsFunc( 5 ), wxDefaultPosition, wxSize(140, 200) );
    item85->Add( item90, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL, 5 );

    item73->Add( item85, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    item61->Add( item73, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    item15->Add( item61, 0, wxGROW, 5 );

    item0->Add( item15, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
*/

    item85->Add( 1, 20, 0, wxALIGN_CENTRE|wxLEFT, 5 );

    wxStaticBitmap *item86 = new wxStaticBitmap( parent, IDC_STATIC_BITMAP_BELGIUM, eidBitmapsFunc( 0 ), wxDefaultPosition, wxSize(90,90) );
    item85->Add( item86, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );

    item85->Add( 1, 20, 0, wxALIGN_CENTRE|wxLEFT, 5 );

    wxBoxSizer *item87 = new wxBoxSizer( wxVERTICAL );
    item87->Add( 20, 80, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxStaticBitmap *item88 = new wxStaticBitmap( parent, IDC_STATIC_BITMAP_LOGO, eidBitmapsFunc( 1 ), wxDefaultPosition, wxSize(80,90) );
    item87->Add( item88, 0, wxALIGN_CENTER_VERTICAL, 5 );

    item85->Add( item87, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL, 5 );

    item85->Add( 1, 20, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL, 5 );

    wxStaticBitmap *item89 = new wxStaticBitmap( parent, IDC_STATIC_STATUS_PICTURE, eidBitmapsFunc( 6 ), wxDefaultPosition, wxDefaultSize );
    item89->Show(FALSE);
    item85->Add( item89, 0, wxALIGN_RIGHT, 5 );

    wxStaticBitmap *item90 = new wxStaticBitmap( parent, IDC_STATIC_PICTURE_CARD, eidBitmapsFunc( 5 ), wxDefaultPosition, wxSize(140,200) );
    item85->Add( item90, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL, 5 );

    item73->Add( item85, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );

    item61->Add( item73, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    item15->Add( item61, 0, wxGROW, 5 );

    item0->Add( item15, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    panel->SetAutoLayout( TRUE );
    panel->SetSizer( item0 );
    item0->Fit( panel );
    item0->SetSizeHints( panel );
   
     AddPage( panel, _("Identity") );

    // Set Backcolor
     panel->SetBackgroundColour(m_ColorBack);

     return panel;
}

wxPanel *eidviewerNotebook::CreateCertificatesPage(int iID)
{
    wxPanel *panel = new wxPanel(this, iID);
    wxPanel *parent = panel;

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxFlexGridSizer *item1 = new wxFlexGridSizer( 2, 0, 0 );
    item1->AddGrowableCol( 0 );
    item1->AddGrowableCol( 1 );
    item1->AddGrowableRow( 0 );

    wxBoxSizer *item2 = new wxBoxSizer( wxVERTICAL );

    wxStaticText *item3 = new wxStaticText( parent, IDC_STATIC_TREE_CERTIFICATES_LABEL, _("Certificates"), wxDefaultPosition, wxSize(20,-1), 0 );
    item2->Add( item3, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxBOTTOM, 5 );

    wxTreeCtrl *item4 = new wxTreeCtrl( parent, IDC_TREE_CERTIFICATES, wxDefaultPosition, wxSize(250,400), wxTR_HAS_BUTTONS|wxTR_LINES_AT_ROOT|wxNO_BORDER );
    item2->Add( item4, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    item1->Add( item2, 0, wxGROW|wxALL, 5 );

    wxFlexGridSizer *item5 = new wxFlexGridSizer( 13, 0, 0, 0 );
    item5->AddGrowableCol( 0 );
    item5->AddGrowableRow( 11 );

    item5->Add( 20, 20, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxStaticText *item6 = new wxStaticText( parent, IDC_STATIC_CERTIFICATES_OWNER, _("Owner"), wxDefaultPosition, wxSize(20,-1), 0 );
    item5->Add( item6, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxTextCtrl *item7 = new wxTextCtrl( parent, IDC_STATIC_CERTIFICATES_OWNER_VAL, wxT(""), wxDefaultPosition, wxSize(250,-1), wxTE_READONLY | wxNO_BORDER );
    item5->Add( item7, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxStaticText *item8 = new wxStaticText( parent, IDC_STATIC_CERTIFICATES_ISSUER, _("Issuer"), wxDefaultPosition, wxSize(20,-1), 0 );
    item5->Add( item8, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxTOP, 5 );

    wxTextCtrl *item9 = new wxTextCtrl( parent, IDC_STATIC_CERTIFICATES_ISSUER_VAL, wxT(""), wxDefaultPosition, wxSize(80,-1), wxTE_READONLY | wxNO_BORDER );
    item5->Add( item9, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxStaticText *item10 = new wxStaticText( parent, IDC_STATIC_CERTIFICATES_KEYLENGTH, _("Key Length"), wxDefaultPosition, wxSize(20,-1), 0 );
    item5->Add( item10, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxTOP, 5 );

    wxTextCtrl *item11 = new wxTextCtrl( parent, IDC_STATIC_CERTIFICATES_KEYLENGTH_VAL, wxT(""), wxDefaultPosition, wxSize(80,-1), wxTE_READONLY | wxNO_BORDER );
    item5->Add( item11, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxFlexGridSizer *item12 = new wxFlexGridSizer( 2, 0, 5 );
    item12->AddGrowableCol( 0 );
    item12->AddGrowableCol( 1 );
    item12->AddGrowableRow( 0 );
    item12->AddGrowableRow( 1 );

    wxStaticText *item13 = new wxStaticText( parent, IDC_STATIC_CERTIFICATES_VALIDFROM, _("Valid From"), wxDefaultPosition, wxSize(20,-1), 0 );
    item12->Add( item13, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxStaticText *item14 = new wxStaticText( parent, IDC_STATIC_CERTIFICATES_VALIDUNTIL, _("Until"), wxDefaultPosition, wxSize(20,-1), 0 );
    item12->Add( item14, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxTextCtrl *item15 = new wxTextCtrl( parent, IDC_STATIC_CERTIFICATES_VALIDFROM_VAL, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxNO_BORDER );
    item12->Add( item15, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxTextCtrl *item16 = new wxTextCtrl( parent, IDC_STATIC_CERTIFICATES_VALIDUNTIL_VAL, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxNO_BORDER );
    item12->Add( item16, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    item5->Add( item12, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxTOP, 5 );

    wxStaticText *item17 = new wxStaticText( parent, IDC_STATIC_CERTIFICATES_STATUS, _("Certificate Status"), wxDefaultPosition, wxSize(20,-1), 0 );
    item5->Add( item17, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxTOP, 5 );

    wxTextCtrl *item18 = new wxTextCtrl( parent, IDC_STATIC_CERTIFICATES_STATUS_VAL, wxT(""), wxDefaultPosition, wxSize(80,-1), wxTE_READONLY | wxNO_BORDER );
    item5->Add( item18, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    item5->Add( 20, 50, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item19 = new wxStaticText( parent, IDC_STATIC_CERTIFICATES_REGISTER, wxT(""),
        wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( item19, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item20 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item21 = new wxButton( parent, IDC_BUTTON_REGISTER, _("Register"), wxDefaultPosition, wxDefaultSize, 0 );
    item20->Add( item21, 0, wxALIGN_BOTTOM|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    wxButton *item22 = new wxButton( parent, IDC_BUTTONCERTIFICATES_DETAILS, _("Details >>"), wxDefaultPosition, wxDefaultSize, 0 );
    item20->Add( item22, 0, wxALIGN_BOTTOM|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    item5->Add( item20, 0, wxALIGN_RIGHT|wxALIGN_BOTTOM, 5 );

    item1->Add( item5, 0, wxGROW|wxALL, 5 );

    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    panel->SetAutoLayout( TRUE );
    panel->SetSizer( item0 );
    item0->Fit( panel );
    item0->SetSizeHints( panel );

    // Set Tree images
    wxTreeCtrl *pCtrlTreeCerts = (wxTreeCtrl *)FindWindow(IDC_TREE_CERTIFICATES);
    pCtrlTreeCerts->AssignImageList(m_pCertTreeImgList);
    wxButton *pButtonCerts = NULL;
    pButtonCerts = (wxButton *)FindWindow(IDC_BUTTON_REGISTER);
    pButtonCerts->Enable(FALSE);
#ifndef _WIN32
    pButtonCerts->Show(FALSE);
#endif
    pButtonCerts = (wxButton *)FindWindow(IDC_BUTTONCERTIFICATES_DETAILS);
    pButtonCerts->Enable(FALSE);
#ifndef _WIN32
    pButtonCerts->Show(FALSE);
#endif

    AddPage( panel, _("Certificates") );

    return panel;
}

wxPanel *eidviewerNotebook::CreateCardDataPage(int iID)
{
    wxPanel *panel = new wxPanel(this, iID);
    wxPanel *parent = panel;

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxFlexGridSizer *item1 = new wxFlexGridSizer( 1, 0, 0 );
    item1->AddGrowableCol( 0 );
    item1->AddGrowableRow( 0 );
    item1->AddGrowableRow( 1 );

    wxStaticBox *item3 = new wxStaticBox( parent, IDC_STATIC_CARDDATA_VERSIONINFO_GROUP, _("Version Info") );
    wxStaticBoxSizer *item2 = new wxStaticBoxSizer( item3, wxVERTICAL );

    wxListCtrl *item4 = new wxListCtrl( parent, IDC_LIST_CARDDATA_VERSIONINFO, wxDefaultPosition, wxSize(500,200), wxLC_REPORT|wxLC_SINGLE_SEL|wxSUNKEN_BORDER );
    item2->Add( item4, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item1->Add( item2, 0, wxGROW|wxALL, 5 );

    wxStaticBox *item6 = new wxStaticBox( parent, IDC_STATIC_CARDDATA_PIN_GROUP, _("PIN Info") );
    wxStaticBoxSizer *item5 = new wxStaticBoxSizer( item6, wxVERTICAL );

    wxFlexGridSizer *item7 = new wxFlexGridSizer( 2, 0, 0 );
    item7->AddGrowableCol( 0 );
    item7->AddGrowableCol( 1 );
    item7->AddGrowableRow( 0 );

    wxBoxSizer *item8 = new wxBoxSizer( wxVERTICAL );

    wxStaticText *item9 = new wxStaticText( parent, IDC_STATIC_CARDDATA_PINTREE_LABEL, _("Pins"), wxDefaultPosition, wxDefaultSize, 0 );
    item8->Add( item9, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxBOTTOM, 5 );

    wxTreeCtrl *item10 = new wxTreeCtrl( parent, IDC_TREE_CARDDATA_PIN, wxDefaultPosition, wxSize(250,160), wxTR_HAS_BUTTONS|wxTR_LINES_AT_ROOT|wxNO_BORDER );
    item8->Add( item10, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    item7->Add( item8, 0, wxGROW|wxALL, 5 );

    wxFlexGridSizer *item11 = new wxFlexGridSizer( 5, 0, 0, 0 );
    item11->AddGrowableCol( 0 );
    item11->AddGrowableRow( 4 );

    item11->Add( 20, 20, 0, wxGROW, 5 );

    wxFlexGridSizer *item12 = new wxFlexGridSizer( 2, 0, 5 );
    item12->AddGrowableCol( 0 );
    item12->AddGrowableRow( 0 );

    wxStaticText *item13 = new wxStaticText( parent, IDC_STATIC_CARDDATA_PINNAME_LABEL, _("PIN Name"), wxDefaultPosition, wxSize(20,-1), 0 );
    item12->Add( item13, 0, wxGROW, 5 );

    wxStaticText *item14 = new wxStaticText( parent, IDC_STATIC_CARDDATA_PINID_LABEL, _("Pin ID"), wxDefaultPosition, wxSize(20,-1), 0 );
    item12->Add( item14, 0, wxGROW, 5 );

    wxTextCtrl *item15 = new wxTextCtrl( parent, IDC_STATIC_CARDDATA_PINNAME_VAL, wxT(""), wxDefaultPosition, wxSize(100,-1), wxTE_READONLY | wxNO_BORDER );
    item12->Add( item15, 0, wxGROW, 5 );

    wxTextCtrl *item16 = new wxTextCtrl( parent, IDC_STATIC_CARDDATA_PINID_VAL, wxT(""), wxDefaultPosition, wxSize(50,-1), wxTE_READONLY | wxNO_BORDER );
    item12->Add( item16, 0, 0, 5 );

    item11->Add( item12, 0, wxGROW, 5 );

    wxStaticText *item17 = new wxStaticText( parent, IDC_STATIC_CARDDATA_PINSTATUS_LABEL, _("PIN Status"), wxDefaultPosition, wxSize(20,-1), 0 );
    item11->Add( item17, 0, wxGROW|wxTOP, 5 );

    wxTextCtrl *item18 = new wxTextCtrl( parent, IDC_STATIC_CARDDATA_PINSTATUS_VAL, wxT(""), wxDefaultPosition, wxSize(80,-1), wxTE_READONLY | wxNO_BORDER );
    item11->Add( item18, 0, wxGROW, 5 );

    wxBoxSizer *item19 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item20 = new wxButton( parent, IDC_BUTTON_CARDDATA_CHANGEPIN, _("Change Pin"), wxDefaultPosition, wxDefaultSize, 0 );
    item19->Add( item20, 0, wxALIGN_BOTTOM|wxALIGN_CENTER_HORIZONTAL, 5 );

    item11->Add( item19, 0, wxALIGN_BOTTOM|wxTOP, 5 );

    item7->Add( item11, 0, wxGROW|wxALL, 5 );

    item5->Add( item7, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item1->Add( item5, 0, wxGROW|wxALL, 5 );

    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    panel->SetAutoLayout( TRUE );
    panel->SetSizer( item0 );
    item0->Fit( panel );
    item0->SetSizeHints( panel );

    wxListCtrl *pListCtrl = (wxListCtrl *)FindWindow(IDC_LIST_CARDDATA_VERSIONINFO);
    pListCtrl->InsertColumn(0, _("Field"), wxLIST_FORMAT_LEFT, FRAME_WIDTH / 2);
    pListCtrl->InsertColumn(1, _("Value"), wxLIST_FORMAT_LEFT, FRAME_WIDTH / 2);

    // Set Tree images
    wxTreeCtrl *pCtrlTreePins = (wxTreeCtrl *)FindWindow(IDC_TREE_CARDDATA_PIN);
    pCtrlTreePins->AssignImageList(m_pPINTreeImgList);

    wxButton *pButtonPIN = NULL;
    pButtonPIN = (wxButton *)FindWindow(IDC_BUTTON_CARDDATA_CHANGEPIN);
    pButtonPIN->Enable(FALSE);

#ifdef _WIN32    
    AddPage( panel, _("Card && PIN") );
#else
    AddPage( panel, _("Card & PIN") );
#endif

    return panel;
}

wxPanel *eidviewerNotebook::CreateOptionsPage(int iID)
{
    wxPanel *panel = new wxPanel(this, iID);
    wxPanel *parent = panel;

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxFlexGridSizer *item1 = new wxFlexGridSizer( 1, 5, 0 );

    wxString strs2[] = 
    {
        wxT("Nederlands"), 
#ifdef WIN32
        wxT("Français"),
#else
        wxT("Francais"),
#endif
        wxT("Deutsch"), 
        wxT("English")
    };
    wxRadioBox *item2 = new wxRadioBox( parent, IDC_RADIOBOX_OPTIONS_LANGUAGE, _("Language"), wxDefaultPosition, wxDefaultSize, 4, strs2, 1, wxRA_SPECIFY_COLS );
    item1->Add( item2, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxStaticBox *item4 = new wxStaticBox( parent, IDC_STATIC_OPTIONS_VALIDATION_GROUP, _("Certificate Validation") );
    wxStaticBoxSizer *item3 = new wxStaticBoxSizer( item4, wxVERTICAL );

    wxFlexGridSizer *item5 = new wxFlexGridSizer( 2, 5, 0 );
    item5->AddGrowableCol( 1 );
    item5->AddGrowableRow( 0 );
    item5->AddGrowableRow( 1 );

    item5->Add( 20, 5, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item5->Add( 20, 5, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item6 = new wxStaticText( parent, IDC_STATIC_OPTIONS_OCSP, _("OCSP"), wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( item6, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP, 5 );

    wxString *strs7 = (wxString*) NULL;
    wxChoice *item7 = new wxChoice( parent, IDC_COMBO_OPTIONS_OCSP_VAL, wxDefaultPosition, wxSize(100,-1), 0, strs7, 0 );
    item5->Add( item7, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxTOP, 5 );

    wxStaticText *item8 = new wxStaticText( parent, IDC_STATIC_OPTIONS_CRL, _("CRL"), wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( item8, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP, 5 );

    wxString *strs9 = (wxString*) NULL;
    wxChoice *item9 = new wxChoice( parent, IDC_COMBO_OPTIONS_CRL_VAL, wxDefaultPosition, wxSize(100,-1), 0, strs9, 0 );
    item5->Add( item9, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxTOP, 5 );

    item5->Add( 20, 5, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item5->Add( 20, 5, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item3->Add( item5, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item1->Add( item3, 0, wxGROW, 5 );

    wxStaticBox *item11 = new wxStaticBox( parent, IDC_STATIC_OPTIONS_READER_GROUP, _("Smartcard Reader") );
    wxStaticBoxSizer *item10 = new wxStaticBoxSizer( item11, wxVERTICAL );

    item10->Add( 20, 10, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxString *strs12 = (wxString*) NULL;
    wxChoice *item12 = new wxChoice( parent, IDC_COMBO_OPTIONS_READER_VAL, wxDefaultPosition, wxSize(250,-1), 0, strs12, 0 );
    item10->Add( item12, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item10->Add( 20, 10, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item1->Add( item10, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    panel->SetAutoLayout( TRUE );
    panel->SetSizer( item0 );
    item0->Fit( panel );
    item0->SetSizeHints( panel );

    wxChoice *pChoiceOCSP = (wxChoice *)FindWindow(IDC_COMBO_OPTIONS_OCSP_VAL);
    wxChoice *pChoiceCRL = (wxChoice *)FindWindow(IDC_COMBO_OPTIONS_CRL_VAL);
    wxChoice *pChoiceReaders = (wxChoice *)FindWindow(IDC_COMBO_OPTIONS_READER_VAL);

    eidviewer::CConfig & oConfig = wxGetApp().GetConfig();
    // OCSP CRL
    pChoiceOCSP->Append(_("Not used"));
    pChoiceOCSP->Append(_("Optional"));
    pChoiceOCSP->Append(_("Mandatory"));
    pChoiceCRL->Append(_("Not used"));
    pChoiceCRL->Append(_("Optional"));
    pChoiceCRL->Append(_("Mandatory"));
    pChoiceReaders->Append(_("Not specified"));

    pChoiceOCSP->SetSelection(oConfig.m_dwOCSP);
    pChoiceCRL->SetSelection(oConfig.m_dwCRL);

    AdjustOCSP();
    AdjustCRL();

    // List Readers
    struct sc_context *pCtx = NULL;

	if( SC_NO_ERROR == sc_establish_context(&pCtx, NULL))
    {
	    for (int i = 0; i < pCtx->reader_count; i++) 
        {
		    pChoiceReaders->Append(wxString(pCtx->reader[i]->name, *wxConvCurrent));
	    }
        if (pCtx)
		    sc_release_context(pCtx);
    }

    // Set Selection
    wxString strSel;
    if (!oConfig.m_StrReader.IsEmpty())
    {
        if(-1 != (pChoiceReaders->FindString(oConfig.m_StrReader)))
        {
            strSel = oConfig.m_StrReader;
        }
        else
        {
            oConfig.m_StrReader.Empty();  
        }
    }
    if(!strSel.IsEmpty())
    {
        pChoiceReaders->SetStringSelection(strSel);
    }
    else
    {
        pChoiceReaders->SetSelection(0);
    }

    wxRadioBox *pRadioBox = (wxRadioBox *)FindWindow(IDC_RADIOBOX_OPTIONS_LANGUAGE);
    pRadioBox->SetSelection(oConfig.m_dwLanguage > -1 ? oConfig.m_dwLanguage : 3);

    AddPage( panel, _("Options") );

    return panel;
}

#ifdef _WIN32
wxPanel *eidviewerNotebook::CreateVersionPage(int iID)
{
    wxPanel *panel = new wxPanel(this, iID);
    wxPanel *parent = panel;


    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *item1 = new wxBoxSizer( wxVERTICAL );

    wxGridSizer *item2 = new wxGridSizer( 2, 0, 0 );

    wxBoxSizer *item3 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticBitmap *item4 = new wxStaticBitmap( parent, IDC_BITMAP_VERSION_LOGO, eidBitmapsFunc( 30 ), wxDefaultPosition, wxDefaultSize );
    item3->Add( item4, 0, wxALIGN_CENTRE|wxALL, 5 );

    item3->Add( 10, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxStaticText *item5 = new wxStaticText( parent, IDC_STATIC_VERSION_PRODUCT, _("Belgian eID Run-time Version                           "), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item5, 0, wxALIGN_CENTER_VERTICAL, 5 );

    item2->Add( item3, 0, wxALIGN_CENTER_VERTICAL, 5 );

    item1->Add( item2, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxListCtrl *item6 = new wxListCtrl( parent, IDC_LISTCTRL_VERSION_MODULES, wxDefaultPosition, wxSize(500,200), wxLC_REPORT|wxLC_SINGLE_SEL|wxSUNKEN_BORDER );
    item1->Add( item6, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item1->Add( 20, 50, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxStaticText *item7 = new wxStaticText( parent, IDC_STATIC_INFO_DEVELOPEDBY, _("Developed for the Belgian Government by Zetes / Computer Sciences Corporation"), wxDefaultPosition, wxSize(20,20), wxALIGN_CENTRE );
    item1->Add( item7, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
/*
    wxBoxSizer *item8 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticBitmap *item9 = new wxStaticBitmap( parent, IDC_BITMAP_VERSION_ZETES, eidBitmapsFunc( 31 ), wxDefaultPosition, wxDefaultSize );
    item8->Add( item9, 0, wxALIGN_CENTRE|wxALL, 5 );

    item8->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxStaticBitmap *item10 = new wxStaticBitmap( parent, IDC_BITMAP_VERSION_CSC, eidBitmapsFunc( 32), wxDefaultPosition, wxDefaultSize );
    item8->Add( item10, 0, wxALIGN_CENTRE|wxALL, 5 );

    item1->Add( item8, 0, wxALIGN_CENTRE|wxALL, 5 );
*/
    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    panel->SetAutoLayout( TRUE );
    panel->SetSizer( item0 );
    item0->Fit( panel );
    item0->SetSizeHints( panel );


    wxListCtrl *pListCtrl = (wxListCtrl *)FindWindow(IDC_LISTCTRL_VERSION_MODULES);
    pListCtrl->InsertColumn(0, _("Name"), wxLIST_FORMAT_LEFT, (FRAME_WIDTH * 2) / 3);
    pListCtrl->InsertColumn(1, _("Version"), wxLIST_FORMAT_LEFT, FRAME_WIDTH / 3);
    AddPage( panel, _("Info") );

    FillModuleVersionInfo();

    return panel;
}
#endif

void eidviewerNotebook::SetIDData(BEID_ID_Data *pIDData, long lSignature, BOOL bClear /* FALSE */)
{
    if(bClear || BEID_SIGNATURE_VALID == lSignature || BEID_SIGNATURE_VALID_WRONG_RRNCERT == lSignature)
    {
        wxString strTemp;
        wxTextCtrl *pCtrl = NULL;
        wxFont *pFont = wxGetApp().GetDataFont();

        wxString strName(pIDData->name, wxConvUTF8);
        pCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_IDENTITY_NAME_VAL);
        if(pCtrl != NULL)
            pCtrl->SetValue(strName);
        if(pFont)
            pCtrl->SetFont(*pFont);

        wxString strName1(pIDData->firstName1, wxConvUTF8);
        wxString strName2(pIDData->firstName2, wxConvUTF8);
        wxString strName3(pIDData->firstName3, wxConvUTF8);
        strTemp = strName1;
        strTemp += wxT("  ");
        strTemp += strName2;
        pCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_IDENTITY_GIVENNAMES_VAL1);
        if(pCtrl != NULL)
            pCtrl->SetValue(strTemp);
        if(pFont)
            pCtrl->SetFont(*pFont);
        pCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_IDENTITY_GIVENNAMES_VAL3);
        if(pCtrl != NULL)
            pCtrl->SetValue(strName3);
        if(pFont)
            pCtrl->SetFont(*pFont);

        wxString strBirthLoc(pIDData->birthLocation, wxConvUTF8);
        pCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_IDENTITY_BIRTHPLACE_VAL);
        if(pCtrl != NULL)
            pCtrl->SetValue(strBirthLoc);
        if(pFont)
            pCtrl->SetFont(*pFont);

        FillDate(pIDData->birthDate, strTemp);
        pCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_IDENTITY_BIRTHDATE_VAL);
        if(pCtrl != NULL)
            pCtrl->SetValue(strTemp);
        if(pFont)
            pCtrl->SetFont(*pFont);

        wxString strNat(pIDData->nationality, wxConvUTF8);
        pCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_IDENTITY_NATIONALITY_VAL);
        if(pCtrl != NULL)
            pCtrl->SetValue(strNat);
        if(pFont)
            pCtrl->SetFont(*pFont);

        wxString strSex(pIDData->sex, wxConvUTF8);
        pCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_IDENTITY_SEX_VAL);
        if(pCtrl != NULL)
            pCtrl->SetValue(strSex);
        if(pFont)
            pCtrl->SetFont(*pFont);

        wxString strTitle(pIDData->nobleCondition, wxConvUTF8);
        pCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_IDENTITY_TITLE_VAL);
        if(pCtrl != NULL)
            pCtrl->SetValue(strTitle);
        if(pFont)
            pCtrl->SetFont(*pFont);


        strTemp.Empty();
        wxString strNatNr(pIDData->nationalNumber, wxConvUTF8);
        if(!strNatNr.IsEmpty())
        {
            strTemp += strNatNr.Left(2);
            strTemp += wxT(".");
            strTemp += strNatNr.Mid(2, 2);
            strTemp += wxT(".");
            strTemp += strNatNr.Mid(4, 2);
            strTemp += wxT("-");
            strTemp += strNatNr.Mid(6, 3);
            strTemp += wxT(".");
            strTemp += strNatNr.Mid(9, 2);
        }
        pCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_IDENTITY_NATIONALNR_VAL);
        if(pCtrl != NULL)
            pCtrl->SetValue(strTemp);
        if(pFont)
            pCtrl->SetFont(*pFont);

        // Special status
        wxStaticBitmap *pChck = NULL;
        pChck = (wxStaticBitmap *)FindWindow(IDC_CHECK_SPECIALSTATUS_WHITE_VAL);
        if(pChck != NULL)
            pChck->SetBitmap(pIDData->whiteCane ? eidBitmapsFunc( 11 ) : eidBitmapsFunc( 12 ));

        pChck = (wxStaticBitmap *)FindWindow(IDC_CHECK_SPECIALSTATUS_YELLOW_VAL);
        if(pChck != NULL)
            pChck->SetBitmap(pIDData->yellowCane ? eidBitmapsFunc( 11 ) : eidBitmapsFunc( 12 ));

        pChck = (wxStaticBitmap *)FindWindow(IDC_CHECK_SPECIALSTATUS_MINORITY_VAL);
        if(pChck != NULL)
            pChck->SetBitmap(pIDData->extendedMinority ? eidBitmapsFunc( 11 ) : eidBitmapsFunc( 12 ));

        // Card Info
        wxString strChipNr(pIDData->chipNumber, wxConvUTF8);
        pCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_CARDINFO_CHIPNR_VAL);
        if(pCtrl != NULL)
            pCtrl->SetValue(strChipNr);
        if(pFont)
            pCtrl->SetFont(*pFont);

        wxString strCardNr(pIDData->cardNumber, wxConvUTF8);        
        strTemp.Empty(); 
        if(!strCardNr.IsEmpty())
        {
            strTemp += strCardNr.Left(3);
            strTemp += wxT(".");
            strTemp += strCardNr.Mid(3, 7);
            strTemp += wxT(".");
            strTemp += strCardNr.Mid(10, 2);
        }
        pCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_CARDINFO_CARDNR_VAL);
        if(pCtrl != NULL)
            pCtrl->SetValue(strTemp);
        if(pFont)
            pCtrl->SetFont(*pFont);

        wxString strMunic(pIDData->municipality, wxConvUTF8);
        pCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_CARDINFO_MUNICIPALITY_VAL);
        if(pCtrl != NULL)
            pCtrl->SetValue(strMunic);
        if(pFont)
            pCtrl->SetFont(*pFont);

        FillDate(pIDData->validityDateBegin, strTemp);
        pCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_CARDINFO_VALIDFROM_VAL);
        if(pCtrl != NULL)
            pCtrl->SetValue(strTemp);
        if(pFont)
            pCtrl->SetFont(*pFont);

        FillDate(pIDData->validityDateEnd, strTemp);
        pCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_CARDINFO_VALIDUNTIL_VAL);
        if(pCtrl != NULL)
            pCtrl->SetValue(strTemp);
        if(pFont)
            pCtrl->SetFont(*pFont);
    }
    wxStaticBitmap *pStatusBitmap = (wxStaticBitmap *)FindWindow(IDC_STATIC_STATUS_IDENTITY);
    if(pStatusBitmap != NULL)
        FillStatus(pStatusBitmap, lSignature);
}

void eidviewerNotebook::SetAddressData(BEID_Address *pADData, long lSignature, BOOL bClear /* FALSE */)
{
    if(bClear || BEID_SIGNATURE_VALID == lSignature || BEID_SIGNATURE_VALID_WRONG_RRNCERT == lSignature)
    {
        wxString strTemp;
        wxTextCtrl *pCtrl = NULL;
        wxFont *pFont = wxGetApp().GetDataFont();

        wxString strStreet(pADData->street, wxConvUTF8);
        wxString strNr(pADData->streetNumber, wxConvUTF8);
        wxString strBox(pADData->boxNumber, wxConvUTF8);
        strTemp = strStreet;
        if(!strNr.IsEmpty())
        {
            strTemp += wxT(" ");
            strTemp = strNr;
        }
        if(!strBox.IsEmpty())
        {
            strTemp += wxT(" ");
            strTemp = strBox;
        }
        pCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_ADDRESS_STREET_VAL);
        if(pCtrl != NULL)
            pCtrl->SetValue(strTemp);
        if(pFont)
            pCtrl->SetFont(*pFont);

        wxString strZip(pADData->zip, wxConvUTF8);
        pCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_ADDRESS_ZIP_VAL);
        if(pCtrl != NULL)
            pCtrl->SetValue(strZip);
        if(pFont)
            pCtrl->SetFont(*pFont);

        wxString strMunic(pADData->municipality, wxConvUTF8);
        pCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_ADDRESS_MUNICIPALITY_VAL);
        if(pCtrl != NULL)
            pCtrl->SetValue(strMunic);
        if(pFont)
            pCtrl->SetFont(*pFont);

        if(!strMunic.IsEmpty()) 
        {
            wxString strCountry(pADData->country, wxConvUTF8);
            if (!bClear && strCountry.IsEmpty())
            {
                strCountry = wxT("be");
            }
            pCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_ADDRESS_COUNTRY_VAL);
            if(pCtrl != NULL)
                pCtrl->SetValue(strCountry);
            if(pFont)
                pCtrl->SetFont(*pFont);
        }
        else
        {
            pCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_ADDRESS_COUNTRY_VAL);
            if(pCtrl != NULL)
                pCtrl->SetValue(wxT(""));
            if(pFont)
                pCtrl->SetFont(*pFont);
        }
    }
    wxStaticBitmap *pStatusBitmap = (wxStaticBitmap *)FindWindow(IDC_STATIC_STATUS_ADDRESS);
    if(pStatusBitmap != NULL)
        FillStatus(pStatusBitmap, lSignature);
}

void eidviewerNotebook::SetPictureData(BEID_Bytes *pPicData, long lSignature, BOOL bClear /* FALSE */)
{
    if(m_Picture.length > 0)
    {
        delete [] m_Picture.data;
    }
    memset(&m_Picture, 0, sizeof(BEID_Bytes));
    m_Picture.length = pPicData->length;
    m_Picture.data = new unsigned char[m_Picture.length];
    memcpy(m_Picture.data, pPicData->data, m_Picture.length);

    if(bClear || BEID_SIGNATURE_VALID == lSignature || BEID_SIGNATURE_VALID_WRONG_RRNCERT == lSignature)
    {
        wxStaticBitmap *pPicture = NULL;
        pPicture = (wxStaticBitmap *)FindWindow(IDC_STATIC_PICTURE_CARD);
        if(pPicture != NULL)
        {
            if(pPicData->length > 0 && pPicData->data != NULL)
            {
                wxMemoryInputStream oStream(m_Picture.data, m_Picture.length);                
		        wxImage image( oStream, wxBITMAP_TYPE_JPEG);
                wxSize oSize = pPicture->GetSize();
//#ifndef _WIN32	
        	wxBitmap bitmap( image.Scale(oSize.GetWidth(),oSize.GetHeight()) );
/*#else	
        	wxBitmap bitmap( image );
                bitmap.SetHeight(oSize.GetHeight());
                bitmap.SetWidth(oSize.GetWidth());
#endif*/		
                pPicture->SetBitmap(bitmap);
            }
            else
            {
                wxSize oSize = pPicture->GetSize();
                wxBitmap bitmap( eidBitmapsFunc( 5 ));
                bitmap.SetHeight(oSize.GetHeight());
                bitmap.SetWidth(oSize.GetWidth());
                pPicture->SetBitmap(bitmap);
            }
        }
    }
    wxStaticBitmap *pStatusBitmap = (wxStaticBitmap *)FindWindow(IDC_STATIC_STATUS_PICTURE);
    if(pStatusBitmap != NULL)
        FillStatus(pStatusBitmap, lSignature);
}

void eidviewerNotebook::FillDate(char *pszSource, wxString & strResult)
{
    strResult.Empty();
    if(strlen(pszSource) > 0)
    {
        char szYear[5] = {0};
        char szMonth[3] = {0};
        char szDay[3] = {0};
        char szDate[16] = {0};
        strncpy(szYear, pszSource, 4);
        strncpy(szMonth, pszSource + 4, 2);
        strncpy(szDay, pszSource + 6, 2);
        sprintf(szDate, "%s/%s/%s", szDay, szMonth, szYear);
        wxString strTemp(szDate, wxConvUTF8);
        strResult = strTemp;
    }
}

void eidviewerNotebook::LoadStrings(int iLanguage)
{
    SetPageText(0, _("Identity"));
    wxStaticText *pText1 = (wxStaticText *)FindWindow(IDC_STATIC_IDENTITY_HEADER1);
    pText1->SetBackgroundColour(m_ColorBack);
    wxStaticText *pText2 = (wxStaticText *)FindWindow(IDC_STATIC_IDENTITY_HEADER2);
    pText2->SetBackgroundColour(m_ColorBack);
    wxStaticText *pText3 = (wxStaticText *)FindWindow(IDC_STATIC_IDENTITY_HEADER3);
    pText3->SetBackgroundColour(m_ColorBack);
    wxStaticText *pText4 = (wxStaticText *)FindWindow(IDC_STATIC_IDENTITY_HEADER4);
    pText4->SetBackgroundColour(m_ColorBack);
    wxStaticText *pText5 = (wxStaticText *)FindWindow(IDC_STATIC_IDENTITY_HEADER5);
    pText5->SetBackgroundColour(m_ColorBack);
    wxStaticText *pText6 = (wxStaticText *)FindWindow(IDC_STATIC_IDENTITY_HEADER6);
    pText6->SetBackgroundColour(m_ColorBack);
    wxStaticText *pText7 = (wxStaticText *)FindWindow(IDC_STATIC_IDENTITY_HEADER7);
    pText7->SetBackgroundColour(m_ColorBack);
    wxStaticText *pText8 = (wxStaticText *)FindWindow(IDC_STATIC_IDENTITY_HEADER8);
    pText8->SetBackgroundColour(m_ColorBack);

    switch(iLanguage)
    {
    case 0:
        pText1->SetLabel(STR_DUTCH_BEL);
        pText2->SetLabel(STR_FRENCH_BEL);
        pText3->SetLabel(STR_GERMAN_BEL);
        pText4->SetLabel(STR_ENGLISH_BEL);
        pText5->SetLabel(STR_DUTCH_IDEN);
        pText6->SetLabel(STR_FRENCH_IDEN);
        pText7->SetLabel(STR_GERMAN_IDEN);
        pText8->SetLabel(STR_ENGLISH_IDEN);
        break;
    case 1:
        pText1->SetLabel(STR_FRENCH_BEL);
        pText2->SetLabel(STR_DUTCH_BEL);
        pText3->SetLabel(STR_GERMAN_BEL);
        pText4->SetLabel(STR_ENGLISH_BEL);
        pText5->SetLabel(STR_FRENCH_IDEN);
        pText6->SetLabel(STR_DUTCH_IDEN);
        pText7->SetLabel(STR_GERMAN_IDEN);
        pText8->SetLabel(STR_ENGLISH_IDEN);
        break;
    case 2:
        pText1->SetLabel(STR_GERMAN_BEL);
        pText2->SetLabel(STR_FRENCH_BEL);
        pText3->SetLabel(STR_DUTCH_BEL);
        pText4->SetLabel(STR_ENGLISH_BEL);
        pText5->SetLabel(STR_GERMAN_IDEN);
        pText6->SetLabel(STR_FRENCH_IDEN);
        pText7->SetLabel(STR_DUTCH_IDEN);
        pText8->SetLabel(STR_ENGLISH_IDEN);
        break;
    default:
        pText1->SetLabel(STR_ENGLISH_BEL);
        pText2->SetLabel(STR_FRENCH_BEL);
        pText3->SetLabel(STR_DUTCH_BEL);
        pText4->SetLabel(STR_GERMAN_BEL);
        pText5->SetLabel(STR_ENGLISH_IDEN);
        pText6->SetLabel(STR_FRENCH_IDEN);
        pText7->SetLabel(STR_DUTCH_IDEN);
        pText8->SetLabel(STR_GERMAN_IDEN);
        break;
    }

    wxFont oFontHeader =  pText1->GetFont();
    oFontHeader.SetWeight(wxBOLD);
    pText1->SetFont(oFontHeader);
    pText2->SetFont(oFontHeader);
    pText3->SetFont(oFontHeader);
    pText4->SetFont(oFontHeader);
    pText5->SetFont(oFontHeader);
    pText6->SetFont(oFontHeader);
    pText7->SetFont(oFontHeader);
    pText8->SetFont(oFontHeader);

    // Labels
    wxStaticText *pCtrl = NULL;
    pCtrl = (wxStaticText *)FindWindow(IDC_STATIC_IDENTITY_NAME);
    pCtrl->SetLabel(_("Name"));
    pCtrl->SetBackgroundColour(m_ColorBack);
    pCtrl = (wxStaticText *)FindWindow(IDC_STATIC_IDENTITY_GIVENNAMES);
    pCtrl->SetLabel(_("Givennames"));
    pCtrl->SetBackgroundColour(m_ColorBack);
    pCtrl = (wxStaticText *)FindWindow(IDC_STATIC_IDENTITY_BIRTHPLACE);
    pCtrl->SetLabel(_("Place of Birth"));
    pCtrl->SetBackgroundColour(m_ColorBack);
    pCtrl = (wxStaticText *)FindWindow(IDC_STATIC_IDENTITY_BIRTHDATE);
    pCtrl->SetLabel(_("Date of Birth"));
    pCtrl->SetBackgroundColour(m_ColorBack);
    pCtrl = (wxStaticText *)FindWindow(IDC_STATIC_IDENTITY_SEX);
    pCtrl->SetLabel(_("Sex"));
    pCtrl->SetBackgroundColour(m_ColorBack);
    pCtrl = (wxStaticText *)FindWindow(IDC_STATIC_IDENTITY_NATIONALITY);
    pCtrl->SetLabel(_("Nationality"));
    pCtrl->SetBackgroundColour(m_ColorBack);
    pCtrl = (wxStaticText *)FindWindow(IDC_STATIC_IDENTITY_TITLE);
    pCtrl->SetLabel(_("Title"));
    pCtrl->SetBackgroundColour(m_ColorBack);
    pCtrl = (wxStaticText *)FindWindow(IDC_STATIC_IDENTITY_NATIONALNR);
    pCtrl->SetLabel(_("National Number"));
    pCtrl->SetBackgroundColour(m_ColorBack);
    pCtrl = (wxStaticText *)FindWindow(IDC_STATIC_CARDINFO_CHIPNR);
    pCtrl->SetLabel(_("Chip Number"));
    pCtrl->SetBackgroundColour(m_ColorBack);
    pCtrl = (wxStaticText *)FindWindow(IDC_STATIC_CARDINFO_CARDNR);
    pCtrl->SetLabel(_("Card Number"));
    pCtrl->SetBackgroundColour(m_ColorBack);
    pCtrl = (wxStaticText *)FindWindow(IDC_STATIC_CARDINFO_VALIDFROM);
    pCtrl->SetLabel(_("Valid From"));
    pCtrl->SetBackgroundColour(m_ColorBack);
    pCtrl = (wxStaticText *)FindWindow(IDC_STATIC_CARDINFO_VALIDUNTIL);
    pCtrl->SetLabel(_("Until"));
    pCtrl->SetBackgroundColour(m_ColorBack);
    pCtrl = (wxStaticText *)FindWindow(IDC_STATIC_CARDINFO_MUNICIPALITY);
    pCtrl->SetLabel(_("Issuing Municipality"));
    pCtrl->SetBackgroundColour(m_ColorBack);
    pCtrl = (wxStaticText *)FindWindow(IDC_STATIC_ADDRESS_STREET);
    pCtrl->SetLabel(_("Street"));
    pCtrl->SetBackgroundColour(m_ColorBack);
    pCtrl = (wxStaticText *)FindWindow(IDC_STATIC_ADDRESS_ZIP);
    pCtrl->SetLabel(_("Postal Code"));
    pCtrl->SetBackgroundColour(m_ColorBack);
    pCtrl = (wxStaticText *)FindWindow(IDC_STATIC_ADDRESS_MUNICIPALITY);
    pCtrl->SetLabel(_("Municipality"));
    pCtrl->SetBackgroundColour(m_ColorBack);
    pCtrl = (wxStaticText *)FindWindow(IDC_STATIC_ADDRESS_COUNTRY);
    pCtrl->SetLabel(_("Country"));
    pCtrl->SetBackgroundColour(m_ColorBack);
    pCtrl = (wxStaticText *)FindWindow(IDC_STATIC_SPECIALSTATUS_WHITE);
    pCtrl->SetLabel(_("White Cane"));
    pCtrl->SetBackgroundColour(m_ColorBack);
    pCtrl = (wxStaticText *)FindWindow(IDC_STATIC_SPECIALSTATUS_YELLOW);
    pCtrl->SetLabel(_("Yellow Cane"));
    pCtrl->SetBackgroundColour(m_ColorBack);
    pCtrl = (wxStaticText *)FindWindow(IDC_STATIC_SPECIALSTATUS_MINORITY);
    pCtrl->SetLabel(_("Extended Minority"));
    pCtrl->SetBackgroundColour(m_ColorBack);

    // Special Status
    wxStaticBitmap *pStatusBitmap = (wxStaticBitmap *)FindWindow(IDC_CHECK_SPECIALSTATUS_WHITE_VAL);
    pStatusBitmap->SetBackgroundColour(m_ColorBack);
    pStatusBitmap = (wxStaticBitmap *)FindWindow(IDC_CHECK_SPECIALSTATUS_YELLOW_VAL);
    pStatusBitmap->SetBackgroundColour(m_ColorBack);
    pStatusBitmap = (wxStaticBitmap *)FindWindow(IDC_CHECK_SPECIALSTATUS_MINORITY_VAL);
    pStatusBitmap->SetBackgroundColour(m_ColorBack);

    // Group Box
    wxStaticBox *pStaticBox = NULL;
    pStaticBox = (wxStaticBox *)FindWindow(IDC_STATIC_IDENTITY_GROUP);
    pStaticBox->SetLabel(_("Identity"));
    pStaticBox->SetBackgroundColour(m_ColorBack);
    pStaticBox = (wxStaticBox *)FindWindow(IDC_STATIC_ADDRESS_GROUP);
    pStaticBox->SetLabel(_("Address"));
    pStaticBox->SetBackgroundColour(m_ColorBack);
    pStaticBox = (wxStaticBox *)FindWindow(IDC_STATIC_CARDINFO_GROUP);
    pStaticBox->SetLabel(_("Card Info"));
    pStaticBox->SetBackgroundColour(m_ColorBack);
    pStaticBox = (wxStaticBox *)FindWindow(IDC_STATIC_SPECIAL_GROUP);
    pStaticBox->SetLabel(_("Special Status"));
    pStaticBox->SetBackgroundColour(m_ColorBack);

    // Cursors and bitmaps
    wxBitmapButton *pBitmapButton = NULL;
    pBitmapButton = (wxBitmapButton *)FindWindow(IDC_BUTTON_CHIP);
    pBitmapButton->SetCursor(wxCURSOR_HAND);
    pBitmapButton->SetWindowStyleFlag(wxNO_BORDER);
    pBitmapButton->SetToolTip( _("Read Card") );
#ifdef _WIN32    
    pBitmapButton->SetBackgroundColour(m_ColorBack);
#endif    
    pBitmapButton = (wxBitmapButton *)FindWindow(IDC_BUTTON_BITMAP_ARROW);
    pBitmapButton->SetCursor(wxCURSOR_HAND);
    pBitmapButton->SetWindowStyleFlag(wxNO_BORDER);
    pBitmapButton->SetToolTip( _("Clear Screen") );
#ifdef _WIN32    
    pBitmapButton->SetBackgroundColour(m_ColorBack);
#endif    

#ifndef _WIN32
    wxStaticBitmap *pStatBmp = (wxStaticBitmap *)FindWindow(IDC_STATIC_BITMAP_BELGIUM);
    pStatBmp->SetSize(90, 85);
    pStatBmp = (wxStaticBitmap *)FindWindow(IDC_STATIC_BITMAP_LOGO);
    pStatBmp->SetSize(90, 90);
    pStatBmp = (wxStaticBitmap *)FindWindow(IDC_STATIC_STATUS_PICTURE);
    pStatBmp->SetSize(90, 100);
#endif

    eidviewerFrame *pParent = (eidviewerFrame *)GetParent();
    if(pParent != NULL)
    {
        wxToolBar *pToolbar = pParent->GetToolBar();
        if(pToolbar != NULL)
        {
            pToolbar->SetToolShortHelp(IDC_BUTTON_CHIP,  _("Read"));
            pToolbar->SetToolShortHelp(IDC_BUTTON_BITMAP_ARROW,  _("Clear"));
            pToolbar->SetToolShortHelp(IDC_BUTTON_IDENTITY_OPEN,  _("Open"));
            pToolbar->SetToolShortHelp(IDC_BUTTON_IDENTITY_SAVE,  _("Save"));
            pToolbar->SetToolShortHelp(IDC_BUTTON_IDENTITY_PRINT,  _("Print"));
            pToolbar->SetToolShortHelp(IDC_BUTTON_IDENTITY_EXIT,  _("Exit"));
        }
        pParent->SetTitle(_("Identity Card"));
        wxGetApp().SetStatusBarText(wxT(""));
    }

    /////////////////////////////
    // Certificates Page
    SetPageText(1, _("Certificates"));

    // Labels
    wxStaticText *pCtrlCerts = NULL;
    pCtrlCerts = (wxStaticText *)FindWindow(IDC_STATIC_TREE_CERTIFICATES_LABEL);
    pCtrlCerts->SetLabel(_("Certificates"));
    pCtrlCerts = (wxStaticText *)FindWindow(IDC_STATIC_CERTIFICATES_OWNER);
    pCtrlCerts->SetLabel(_("Owner"));
    pCtrlCerts = (wxStaticText *)FindWindow(IDC_STATIC_CERTIFICATES_ISSUER);
    pCtrlCerts->SetLabel(_("Issuer"));
    pCtrlCerts = (wxStaticText *)FindWindow(IDC_STATIC_CERTIFICATES_KEYLENGTH);
    pCtrlCerts->SetLabel(_("Key Length"));
    pCtrlCerts = (wxStaticText *)FindWindow(IDC_STATIC_CERTIFICATES_VALIDFROM);
    pCtrlCerts->SetLabel(_("Valid From"));
    pCtrlCerts = (wxStaticText *)FindWindow(IDC_STATIC_CERTIFICATES_VALIDUNTIL);
    pCtrlCerts->SetLabel(_("Until"));
    pCtrlCerts = (wxStaticText *)FindWindow(IDC_STATIC_CERTIFICATES_STATUS);
    pCtrlCerts->SetLabel(_("Certificate Status"));
#ifdef _WIN32
    pCtrlCerts = (wxStaticText *)FindWindow(IDC_STATIC_CERTIFICATES_REGISTER);
    pCtrlCerts->SetLabel(_("To use your identity card with applications like Internet Explorer, Microsoft Office, OpenOffice.org, Adobe Reader, Outlook, ...  you first have to register the card in Windows. Select a certificate and click the \"Register\" button to register your certificates."));
#endif

    // Buttons
    wxButton *pButtonCerts = NULL;
    pButtonCerts = (wxButton *)FindWindow(IDC_BUTTON_REGISTER);
    pButtonCerts->SetLabel(_("Register"));
    pButtonCerts = (wxButton *)FindWindow(IDC_BUTTONCERTIFICATES_DETAILS);
    pButtonCerts->SetLabel(_("Details >>"));

    /////////////////////////////
    // Card Data Page
#ifdef _WIN32    
    SetPageText(2, _("Card && PIN"));
#else
    SetPageText(2, _("Card & PIN"));
#endif    
    wxStaticText *pCtrlCardData = NULL;
    pCtrlCardData = (wxStaticText *)FindWindow(IDC_STATIC_CARDDATA_PINTREE_LABEL);
    pCtrlCardData->SetLabel(_("Pins"));
    pCtrlCardData = (wxStaticText *)FindWindow(IDC_STATIC_CARDDATA_PINNAME_LABEL);
    pCtrlCardData->SetLabel(_("PIN Name"));
    pCtrlCardData = (wxStaticText *)FindWindow(IDC_STATIC_CARDDATA_PINID_LABEL);
    pCtrlCardData->SetLabel(_("PIN ID"));
    pCtrlCardData = (wxStaticText *)FindWindow(IDC_STATIC_CARDDATA_PINSTATUS_LABEL);
    pCtrlCardData->SetLabel(_("PIN Status"));
    
    // Buttons
    wxButton *pButtonCardData = NULL;
    pButtonCardData = (wxButton *)FindWindow(IDC_BUTTON_CARDDATA_CHANGEPIN);
    pButtonCardData->SetLabel(_("Change Pin"));

    // Group Boxes
    wxStaticBox *pStaticBoxCardData = NULL;
    pStaticBoxCardData = (wxStaticBox *)FindWindow(IDC_STATIC_CARDDATA_VERSIONINFO_GROUP);
    pStaticBoxCardData->SetLabel(_("Version Info"));
    pStaticBoxCardData = (wxStaticBox *)FindWindow(IDC_STATIC_CARDDATA_PIN_GROUP);
    pStaticBoxCardData->SetLabel(_("PIN Info"));
 
    // List Control
    m_vecVersionLabels.clear();
    m_vecVersionLabels.push_back(_("Chip Number"));
    m_vecVersionLabels.push_back(_("Component Code"));
    m_vecVersionLabels.push_back(_("OS Number"));
    m_vecVersionLabels.push_back(_("OS Version"));
    m_vecVersionLabels.push_back(_("Softmask Number"));
    m_vecVersionLabels.push_back(_("Softmask Version"));
    m_vecVersionLabels.push_back(_("Applet Version"));
    m_vecVersionLabels.push_back(_("Global OS Version"));
    m_vecVersionLabels.push_back(_("Applet Interface Version"));
    m_vecVersionLabels.push_back(_("PKCS1 Support"));
    m_vecVersionLabels.push_back(_("Key Exchange Version"));
    m_vecVersionLabels.push_back(_("Application Life Cycle"));
    m_vecVersionLabels.push_back(_("Graphical Personalisation"));
    m_vecVersionLabels.push_back(_("Electrical Personalisation"));
    m_vecVersionLabels.push_back(_("Electrical Personalisation Interface"));

    wxListCtrl *pListCtrl = (wxListCtrl *)FindWindow(IDC_LIST_CARDDATA_VERSIONINFO);
    wxFont *pFont = wxGetApp().GetDataFont();
    if(pFont)
        pListCtrl->SetFont(*pFont); 

    wxListItem colItem;
    colItem.m_mask = wxLIST_MASK_TEXT;
    pListCtrl->GetColumn(0, colItem);
    colItem.SetText(_("Field"));
    pListCtrl->SetColumn(0, colItem);
    colItem.m_mask = wxLIST_MASK_TEXT;
    pListCtrl->GetColumn(1, colItem);
    colItem.SetText(_("Value"));
    pListCtrl->SetColumn(1, colItem);

    /////////////////////////////
    // Options Page
    SetPageText(3, _("Options"));

    // Group Boxes
    wxStaticBox *pStaticBoxOptions = NULL;
    pStaticBoxOptions = (wxStaticBox *)FindWindow(IDC_STATIC_OPTIONS_VALIDATION_GROUP);
    pStaticBoxOptions->SetLabel(_("Certificate Validation"));
    pStaticBoxOptions = (wxStaticBox *)FindWindow(IDC_STATIC_OPTIONS_READER_GROUP);
    pStaticBoxOptions->SetLabel(_("Smartcard Reader"));

    wxRadioBox *pRadioBox = NULL;
    pRadioBox = (wxRadioBox *)FindWindow(IDC_RADIOBOX_OPTIONS_LANGUAGE);
    ((wxWindow *)pRadioBox)->SetLabel(_("Language"));

    wxChoice *pChoiceOCSP = (wxChoice *)FindWindow(IDC_COMBO_OPTIONS_OCSP_VAL);
    wxChoice *pChoiceCRL = (wxChoice *)FindWindow(IDC_COMBO_OPTIONS_CRL_VAL);
    wxChoice *pChoiceReaders = (wxChoice *)FindWindow(IDC_COMBO_OPTIONS_READER_VAL);
    int iSel = 0;
    if (pChoiceReaders->GetCount() > 0)
    {
        iSel = pChoiceReaders->GetSelection();
#ifdef _WIN32
        pChoiceReaders->SetString(0, _("Not specified"));
#else
        wxArrayString arrReaders;
        for (int iCount = 1; iCount < pChoiceReaders->GetCount(); iCount++)
        {
            arrReaders.Add(pChoiceReaders->GetString(iCount));
        }
        pChoiceReaders->Clear();
        pChoiceReaders->Append(_("Not specified"));
        for (unsigned int iArr = 0; iArr < arrReaders.GetCount(); iArr++)
        {
            pChoiceReaders->Append(arrReaders.Item(iArr));
        }
        arrReaders.Clear();
#endif
        if(iSel != -1)
        {
            pChoiceReaders->SetSelection(iSel);
        }
        else
        {
            pChoiceReaders->SetSelection(0);
        }
    }
    eidviewer::CConfig & oConfig = wxGetApp().GetConfig();
    wxRadioBox *pRadioBoxLanguage = (wxRadioBox *)FindWindow(IDC_RADIOBOX_OPTIONS_LANGUAGE);
    pRadioBoxLanguage->SetSelection(oConfig.m_dwLanguage > -1 ? oConfig.m_dwLanguage : 3);

#ifdef _WIN32
    int iCount = pChoiceOCSP->GetCount();
    if (iCount > 0)
    {
        iSel = pChoiceOCSP->GetSelection();
        pChoiceOCSP->SetString(0, _("Not used"));
        pChoiceOCSP->SetString(1, _("Optional"));
        if(iCount > 2)
        {
            pChoiceOCSP->SetString(2, _("Mandatory"));
        }
        if(iSel != -1)
        {
            pChoiceOCSP->SetSelection(iSel);
        }
        else
        {
            pChoiceOCSP->SetSelection(0);
        }
    }
    iCount = pChoiceCRL->GetCount();
    if (iCount > 0)
    {
        iSel = pChoiceCRL->GetSelection();
        pChoiceCRL->SetString(0, _("Not used"));
        pChoiceCRL->SetString(1, _("Optional"));
        if(iCount > 2)
        {
            pChoiceCRL->SetString(2, _("Mandatory"));
        }
        if(iSel != -1)
        {
            pChoiceCRL->SetSelection(iSel);
        }
        else
        {
            pChoiceCRL->SetSelection(0);
        }
    }   
#else
    int iCount = pChoiceOCSP->GetCount();
    if (iCount > 0)
    {
        iSel = pChoiceOCSP->GetSelection();
        pChoiceOCSP->Clear();
        pChoiceOCSP->Append(_("Not used"));
        pChoiceOCSP->Append(_("Optional"));
        if(iCount > 2)
        {
            pChoiceOCSP->Append(_("Mandatory"));
        }
        if(iSel != -1)
        {
            pChoiceOCSP->SetSelection(iSel);
        }
        else
        {
            pChoiceOCSP->SetSelection(0);
        }
    }
    iCount = pChoiceCRL->GetCount();
    if (iCount > 0)
    {
        iSel = pChoiceCRL->GetSelection();
        pChoiceCRL->Clear();
        pChoiceCRL->Append(_("Not used"));
        pChoiceCRL->Append(_("Optional"));
        if(iCount > 2)
        {
            pChoiceCRL->Append(_("Mandatory"));
        }
        if(iSel != -1)
        {
            pChoiceCRL->SetSelection(iSel);
        }
        else
        {
            pChoiceCRL->SetSelection(0);
        }
    }   
#endif

#ifdef _WIN32

    /////////////////////////////
    // Info Page
    SetPageText(4, _("Info"));

    pListCtrl = (wxListCtrl *)FindWindow(IDC_LISTCTRL_VERSION_MODULES);
    pFont = wxGetApp().GetDataFont();
    if(pFont)
        pListCtrl->SetFont(*pFont); 

    wxListItem colItemAbout;
    colItemAbout.m_mask = wxLIST_MASK_TEXT;
    pListCtrl->GetColumn(0, colItemAbout);
    colItemAbout.SetText(_("Name"));
    pListCtrl->SetColumn(0, colItemAbout);
    colItemAbout.m_mask = wxLIST_MASK_TEXT;
    pListCtrl->GetColumn(1, colItemAbout);
    colItemAbout.SetText(_("Version"));
    pListCtrl->SetColumn(1, colItemAbout);
#endif

    wxSize oMySize = GetSize();
    oMySize.SetWidth(oMySize.GetWidth() + 1);
    SetSize(oMySize);
}

void eidviewerNotebook::ClearPages()
{
    BEID_Bytes tBytes = {0};
    BEID_ID_Data idData = {0};
    BEID_Address adData = {0};

    SetIDData(&idData, BEID_SIGNATURE_PROCESSING_ERROR, TRUE);
    SetAddressData(&adData, BEID_SIGNATURE_PROCESSING_ERROR, TRUE);
    SetPictureData(&tBytes, BEID_SIGNATURE_PROCESSING_ERROR, TRUE);

    BEID_VersionInfo tVersionInfo = {0};
    BEID_Certif_Check tCheck = {0};
    SetCertificateData(&tCheck);
    SetVersionInfoData(&tVersionInfo);

    std::map<wxString, std::vector<CPin> > PinMap;
    SetPINData(PinMap); 
}

void eidviewerNotebook::FillStatus(wxStaticBitmap *pWindow, long lSignature)
{
    switch (lSignature)
    {
    case BEID_SIGNATURE_PROCESSING_ERROR:
        pWindow->SetBitmap(eidBitmapsFunc( 6 ));
        pWindow->SetToolTip(wxT(""));
        break;
    case BEID_SIGNATURE_VALID:
        pWindow->SetBitmap(eidBitmapsFunc( 7 ));
        pWindow->SetToolTip(_("Valid signature"));
        break;
    case BEID_SIGNATURE_INVALID:
    case BEID_SIGNATURE_INVALID_WRONG_RRNCERT:
        pWindow->SetBitmap(eidBitmapsFunc( 9 ));
        pWindow->SetToolTip(_("Invalid signature"));
        break;
    case BEID_SIGNATURE_VALID_WRONG_RRNCERT:
        pWindow->SetBitmap(eidBitmapsFunc( 8 ));
        pWindow->SetToolTip(_("Valid signature but invalid RRN certificate"));
        break;
    }
}

void eidviewerNotebook::OnButtonPrint(wxCommandEvent& WXUNUSED(event))
{
    wxWindow *pParent = GetParent();
    if(pParent != NULL)
    {
        if(((eidviewerFrame *)pParent)->GetReading())
        {
            return;
        }
    }
    wxBusyCursor wc;
    CPrintOutData oData;
    FillPrintData(oData);
    wxPrinter printer;
    CPrintOut printout(_("Belgian EID Card"), &oData);

    if (!printer.Print(this, &printout, TRUE))
    {
        if (wxPrinter::GetLastError() == wxPRINTER_ERROR)
            wxGetApp().SetStatusBarText(_("There was a problem with the printer."));
        else
            wxGetApp().SetStatusBarText(_("Printing canceled."));
    }
}

void eidviewerNotebook::FillPrintData(CPrintOutData & oData)
{
    wxBitmap oLogo;
    int iLanguage = wxGetApp().GetLanguage();
    switch (iLanguage)
    {
    case 0:
        oLogo = wxBitmap(eidBitmapsFunc( 13 ));
        break;
    case 1:
         oLogo = wxBitmap(eidBitmapsFunc( 14 ));
        break;
    case 2:
         oLogo = wxBitmap(eidBitmapsFunc( 15 ));
        break;
    default:
         oLogo = wxBitmap(eidBitmapsFunc( 13 ));
        break;
    }
    oData.m_Logo = oLogo;

    if(m_Picture.length > 0 && m_Picture.data != NULL)
    {
        wxMemoryInputStream oStream(m_Picture.data, m_Picture.length);
        wxImage image( oStream, wxBITMAP_TYPE_JPEG);
        wxBitmap bitmap( image );
        oData.m_Picture = bitmap;
    }

    wxString strTemp;

    wxStaticText *pStaticTextCtrl = NULL;
    wxTextCtrl *pTextCtrl = NULL;
    wxStaticBox *pStaticBoxCtrl = (wxStaticBox *)FindWindow(IDC_STATIC_ADDRESS_GROUP);
    oData.m_strAddressLabel = pStaticBoxCtrl->GetLabel();
    pStaticTextCtrl = (wxStaticText *)FindWindow(IDC_STATIC_IDENTITY_HEADER1);
    oData.m_strLangCountry = pStaticTextCtrl->GetLabel(); 
    pStaticTextCtrl = (wxStaticText *)FindWindow(IDC_STATIC_IDENTITY_HEADER5);
    oData.m_strLangCard = pStaticTextCtrl->GetLabel();
    pTextCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_IDENTITY_BIRTHDATE_VAL);
    oData.m_strBirthDate = pTextCtrl->GetValue();   
    pStaticTextCtrl = (wxStaticText *)FindWindow(IDC_STATIC_IDENTITY_BIRTHDATE);
    oData.m_strBirthDateLabel = pStaticTextCtrl->GetLabel(); 
    pTextCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_IDENTITY_BIRTHPLACE_VAL);
    oData.m_strBirthPlace = pTextCtrl->GetValue();   
    pStaticTextCtrl = (wxStaticText *)FindWindow(IDC_STATIC_IDENTITY_BIRTHPLACE);
    oData.m_strBirthPlaceLabel = pStaticTextCtrl->GetLabel(); 
    pTextCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_CARDINFO_CARDNR_VAL);
    oData.m_strCardNr = pTextCtrl->GetValue();   
    pStaticTextCtrl = (wxStaticText *)FindWindow(IDC_STATIC_CARDINFO_CARDNR);
    oData.m_strCardNrLabel = pStaticTextCtrl->GetLabel(); 
    pTextCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_CARDINFO_CHIPNR_VAL);
    oData.m_strChipNr = pTextCtrl->GetValue();   
    pStaticTextCtrl = (wxStaticText *)FindWindow(IDC_STATIC_CARDINFO_CHIPNR);
    oData.m_strChipNrLabel = pStaticTextCtrl->GetLabel(); 
    pTextCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_IDENTITY_GIVENNAMES_VAL1);
    strTemp = pTextCtrl->GetValue();
    strTemp += wxT(" ");
    pTextCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_IDENTITY_GIVENNAMES_VAL3);
    oData.m_strGivenName = strTemp + pTextCtrl->GetValue();   
    pStaticTextCtrl = (wxStaticText *)FindWindow(IDC_STATIC_IDENTITY_GIVENNAMES);
    oData.m_strGivenNameLabel = pStaticTextCtrl->GetLabel(); 
    pTextCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_CARDINFO_MUNICIPALITY_VAL);
    oData.m_strIssMunic = pTextCtrl->GetValue();   
    pStaticTextCtrl = (wxStaticText *)FindWindow(IDC_STATIC_CARDINFO_MUNICIPALITY);
    oData.m_strIssMunicLabel = pStaticTextCtrl->GetLabel(); 
    pTextCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_ADDRESS_MUNICIPALITY_VAL);
    oData.m_strMunicip = pTextCtrl->GetValue();   
    pTextCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_IDENTITY_NAME_VAL);
    oData.m_strName = pTextCtrl->GetValue();   
    pStaticTextCtrl = (wxStaticText *)FindWindow(IDC_STATIC_IDENTITY_NAME);
    oData.m_strNameLabel = pStaticTextCtrl->GetLabel(); 
    pTextCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_IDENTITY_NATIONALITY_VAL);
    oData.m_strNational = pTextCtrl->GetValue();   
    pStaticTextCtrl = (wxStaticText *)FindWindow(IDC_STATIC_IDENTITY_NATIONALITY);
    oData.m_strNationalLabel = pStaticTextCtrl->GetLabel(); 
    pTextCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_IDENTITY_NATIONALNR_VAL);
    oData.m_strNatNr = pTextCtrl->GetValue();   
    pStaticTextCtrl = (wxStaticText *)FindWindow(IDC_STATIC_IDENTITY_NATIONALNR);
    oData.m_strNatNrLabel = pStaticTextCtrl->GetLabel(); 
    pTextCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_IDENTITY_SEX_VAL);
    oData.m_strSex = pTextCtrl->GetValue();   
    pStaticTextCtrl = (wxStaticText *)FindWindow(IDC_STATIC_IDENTITY_SEX);
    oData.m_strSexLabel = pStaticTextCtrl->GetLabel(); 
    pTextCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_ADDRESS_STREET_VAL);
    oData.m_strStreet = pTextCtrl->GetValue();   
    pTextCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_IDENTITY_TITLE_VAL);
    oData.m_strTitle = pTextCtrl->GetValue();   
    pStaticTextCtrl = (wxStaticText *)FindWindow(IDC_STATIC_IDENTITY_TITLE);
    oData.m_strTitleLabel = pStaticTextCtrl->GetLabel(); 
    pTextCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_CARDINFO_VALIDFROM_VAL);
    oData.m_strValidFrom = pTextCtrl->GetValue();   
    pStaticTextCtrl = (wxStaticText *)FindWindow(IDC_STATIC_CARDINFO_VALIDFROM);
    oData.m_strValidFromLabel = pStaticTextCtrl->GetLabel(); 
    pTextCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_CARDINFO_VALIDUNTIL_VAL);
    oData.m_strValidUntil = pTextCtrl->GetValue();   
    pStaticTextCtrl = (wxStaticText *)FindWindow(IDC_STATIC_CARDINFO_VALIDUNTIL);
    oData.m_strValidUntilLabel = pStaticTextCtrl->GetLabel(); 
    pTextCtrl = (wxTextCtrl *)FindWindow(IDC_STATIC_ADDRESS_ZIP_VAL);
    oData.m_strZip = pTextCtrl->GetValue();
}


void eidviewerNotebook::SetCertificateData(BEID_Certif_Check *pCertifs)
{
    ClearMap();

    for (int i = 0; i < pCertifs->certificatesLength; ++i)
    {
        char *pszLabel = pCertifs->certificates[i].certifLabel;
        long lLength = pCertifs->certificates[i].certifLength;

        X509 *pX509 = NULL;
        if(lLength > 0 && 0 != strcmp(pszLabel, "RN"))
        {
            unsigned char *pData = pCertifs->certificates[i].certif;
            CCertif *pCertif = new CCertif(pData, lLength, 
                                                        pszLabel, pCertifs->certificates[i].certifStatus);
            if (NULL != (pX509 = d2i_X509(&pX509, OPENSSL_CAST(&pData), lLength)))
            {
                pCertif->SetX509(pX509);
                ASN1_INTEGER *pSerialNr = X509_get_serialNumber(pX509);
                char *pSerialKey = wxGetApp().Hexify(pSerialNr->data, pSerialNr->length);
                m_oMapCertifs[wxString(pSerialKey, *wxConvCurrent)] = pCertif; 
                delete pSerialKey;
                pX509 = NULL;
            }
        }
    }
    FillTree();
}

void eidviewerNotebook::SetVersionInfoData(BEID_VersionInfo *pVersion)
{
    memcpy(&m_VersionInfo, pVersion, sizeof(BEID_VersionInfo));
    FillListVersionInfo();
}

void eidviewerNotebook::ClearMap()
{
    MapCertifsIt it;
    for (it = m_oMapCertifs.begin(); it != m_oMapCertifs.end(); ++it)
    {
        CCertif *pCertif = (*it).second;
        if(pCertif != NULL)
        {
            delete pCertif;
        }
    }
    m_oMapCertifs.clear();
}

void eidviewerNotebook::GetCertChain(X509 *cert, X509_STORE *store, STACK_OF(X509) **ppChain)
{
  X509_STORE_CTX *ctx = 0;
  STACK_OF(X509) *chn = NULL;

  if ((ctx = X509_STORE_CTX_new()) != 0) 
  {
    if (X509_STORE_CTX_init(ctx, store, cert, 0) == 1)
    {
	    X509_verify_cert(ctx); 
        chn = X509_STORE_CTX_get1_chain(ctx);
    }

    *ppChain = chn;
    X509_STORE_CTX_free(ctx);
  }
}

void eidviewerNotebook::FillTree()
{
    wxTreeCtrl *pCtrlTreeCerts = (wxTreeCtrl *)FindWindow(IDC_TREE_CERTIFICATES);
    if(pCtrlTreeCerts != NULL)
    {
        pCtrlTreeCerts->DeleteAllItems();
        SetTextFields (NULL);

        if (0 == m_oMapCertifs.size())
        {
            return;
        }
        STACK_OF(X509) *certs = sk_X509_new_null();
        X509_STORE *store = NULL;
        MapCertifsIt it;

        // Fill certs stack
        for (it = m_oMapCertifs.begin(); it != m_oMapCertifs.end(); ++it)
        {
                CCertif *pCertif = (*it).second;
                if(pCertif)
                {
                    X509 *pX509 = (X509 *)pCertif->GetX509(); 
                    sk_X509_push(certs, pX509);
                    pX509 = NULL;
                }
        }

        // Create store
        int i = 0;
        store = X509_STORE_new();
        for (i = 0; i < sk_X509_num(certs); i++)
        {
            X509_STORE_add_cert(store, sk_X509_value(certs, i));
        }

        // Build tree
        wxFont *pFont = wxGetApp().GetDataFont();
        if(pFont)
            pCtrlTreeCerts->SetFont(*pFont); 

        wxTreeItemId hRoot = pCtrlTreeCerts->AddRoot(wxT("BELPIC"), 1);
        wxTreeItemId hLast = hRoot;
        for(i = 0; i < sk_X509_num(certs); ++i)
        {
            X509 *pValue = sk_X509_value(certs, i);
            if(pValue != NULL)
            {
                STACK_OF(X509) *pChain = NULL;
                GetCertChain(pValue, store, &pChain);
                if(pChain != NULL)
                {
                    wxTreeItemId hCurrent = hRoot;
                    for(int j = sk_X509_num(pChain) - 1; j  >= 0 ; j--)
                    {
                        X509 *pCert = sk_X509_value(pChain, j);
                        ASN1_INTEGER *pSerialNr = X509_get_serialNumber(pCert);
                        char *pSerialKey = wxGetApp().Hexify(pSerialNr->data, pSerialNr->length);
                
                        MapCertifsIt it = m_oMapCertifs.find(wxString(pSerialKey, *wxConvCurrent));
                        delete pSerialKey;
                        if(it == m_oMapCertifs.end())
                            continue;

                        CCertif *pCertif = (*it).second;
                        // First Find in Tree
                        wxTreeItemId hFound = FindData(hRoot, pCertif);

                        if(!hFound.IsOk())
                        {
                            char szName[256] = {0};
                            X509_NAME_get_text_by_NID(X509_get_subject_name(pCert), NID_commonName, szName, sizeof(szName));
                            wxString strItem(szName, wxConvUTF8);
                            hCurrent = pCtrlTreeCerts->AppendItem(hCurrent, strItem, 0, -1, new wxTreeItemCertif(pCertif));
                            hLast =hCurrent;
                        }
                        else
                        {
                            hCurrent = hFound;
                        }
                    }
                    sk_X509_free(pChain);
                }
                pValue = NULL;
            }
        }
        X509_STORE_free(store);

        if(hLast.IsOk())
        {
            pCtrlTreeCerts->EnsureVisible(hLast);
        }
    }
}

void eidviewerNotebook::SetTextFields(CCertif *pCertif) 
{
    wxTextCtrl *pCtrlOwner = (wxTextCtrl *)FindWindow(IDC_STATIC_CERTIFICATES_OWNER_VAL);
    wxTextCtrl *pCtrlIssuer = (wxTextCtrl *)FindWindow(IDC_STATIC_CERTIFICATES_ISSUER_VAL);
    wxTextCtrl *pCtrlKeyLength = (wxTextCtrl *)FindWindow(IDC_STATIC_CERTIFICATES_KEYLENGTH_VAL);
    wxTextCtrl *pCtrlFrom = (wxTextCtrl *)FindWindow(IDC_STATIC_CERTIFICATES_VALIDFROM_VAL);
    wxTextCtrl *pCtrlUntil = (wxTextCtrl *)FindWindow(IDC_STATIC_CERTIFICATES_VALIDUNTIL_VAL);
    wxTextCtrl *pCtrlStatus = (wxTextCtrl *)FindWindow(IDC_STATIC_CERTIFICATES_STATUS_VAL);
    wxButton *pCtrlDetails = (wxButton *)FindWindow(IDC_BUTTONCERTIFICATES_DETAILS);
    wxButton *pCtrlRegister = (wxButton *)FindWindow(IDC_BUTTON_REGISTER);
    wxFont *pFont = wxGetApp().GetDataFont();

    if(pCertif == NULL)
    {
        // Clear all fields
        pCtrlOwner->SetValue(wxT(""));
        pCtrlIssuer->SetValue(wxT(""));
        pCtrlKeyLength->SetValue(wxT(""));
        pCtrlFrom->SetValue(wxT(""));
        pCtrlUntil->SetValue(wxT(""));
        pCtrlStatus->SetValue(wxT(""));
        pCtrlDetails->Enable(FALSE);
        pCtrlRegister->Enable(FALSE);
    }
    else
    {
        X509 *pX509 = (X509 *)pCertif->GetX509();
        char szName[256] = {0};
        X509_NAME_get_text_by_NID(X509_get_subject_name(pX509), NID_commonName, szName, sizeof(szName));
        wxString strSubName(szName, wxConvUTF8);
	    pCtrlOwner->SetValue(strSubName);
        if(pFont)
            pCtrlOwner->SetFont(*pFont);    
        memset(szName, 0, sizeof(szName));
        X509_NAME_get_text_by_NID(X509_get_issuer_name(pX509), NID_commonName, szName, sizeof(szName));
        wxString strIssName(szName, wxConvUTF8);
	    pCtrlIssuer->SetValue(strIssName);
        if(pFont)
            pCtrlIssuer->SetFont(*pFont);    
        memset(szName, 0, sizeof(szName));

        wxString strLen;
        EVP_PKEY *pKey = X509_get_pubkey(pX509);
        if(pKey != NULL)
        {
            long lLen = EVP_PKEY_bits(pKey);
            strLen = wxString::Format(wxT("%ld bits"), lLen);  
            EVP_PKEY_free(pKey);
        }
	    pCtrlKeyLength->SetValue(strLen);
        if(pFont)
            pCtrlKeyLength->SetFont(*pFont);    

        wxString strTemp;
        ASN1_TIME *pASN1Time = X509_get_notBefore(pX509);
        ASN1_TIME_Print(pASN1Time, strTemp);
	    pCtrlFrom->SetValue(strTemp);
        if(pFont)
            pCtrlFrom->SetFont(*pFont);    
        pASN1Time = X509_get_notAfter(pX509);
        ASN1_TIME_Print(pASN1Time, strTemp);
	    pCtrlUntil->SetValue(strTemp);
        if(pFont)
            pCtrlUntil->SetFont(*pFont);    

        VerifyCertErrorString(pCertif->GetCertStatus(), strTemp); 
	    pCtrlStatus->SetValue(strTemp);
        if(pFont)
            pCtrlStatus->SetFont(*pFont);    
        pCtrlDetails->Enable(TRUE);
    }
}

void eidviewerNotebook::VerifyCertErrorString(long lStatus, wxString & strError)
{
    strError.Empty();
	switch ((int)lStatus)
	{
	case BEID_CERTSTATUS_CERT_VALIDATED_OK:
        strError = _("Valid");
		return;
	case BEID_CERTSTATUS_CERT_NOT_VALIDATED:
        strError = _("Not Validated");
		return;
	case BEID_CERTSTATUS_UNABLE_TO_GET_ISSUER_CERT:
        strError = _("Unable to get issuer certificate");
		return;
	case BEID_CERTSTATUS_UNABLE_TO_GET_CRL:
        strError = _("Unable to get certificate CRL");
		return;
	case BEID_CERTSTATUS_UNABLE_TO_DECRYPT_CERT_SIGNATURE:
        strError = _("Unable to decrypt certificate's signature");
		return;
	case BEID_CERTSTATUS_UNABLE_TO_DECRYPT_CRL_SIGNATURE:
        strError = _("Unable to decrypt CRL's signature");
		return;
	case BEID_CERTSTATUS_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY:
        strError = _("Unable to decode issuer public key");
		return;
	case BEID_CERTSTATUS_CERT_SIGNATURE_FAILURE:
        strError = _("Certificate signature failure");
		return;
	case BEID_CERTSTATUS_CRL_SIGNATURE_FAILURE:
        strError = _("CRL signature failure");
		return;
	case BEID_CERTSTATUS_CERT_NOT_YET_VALID:
        strError = _("Certificate is not yet valid");
		return;
	case BEID_CERTSTATUS_CRL_NOT_YET_VALID:
        strError = _("CRL is not yet valid");
		return;
	case BEID_CERTSTATUS_CERT_HAS_EXPIRED:
        strError = _("Certificate has expired");
		return;
	case BEID_CERTSTATUS_CRL_HAS_EXPIRED:
        strError = _("CRL has expired");
		return;
	case BEID_CERTSTATUS_ERR_IN_CERT_NOT_BEFORE_FIELD:
        strError = _("Format error in certificate's notBefore field");
		return;
	case BEID_CERTSTATUS_ERR_IN_CERT_NOT_AFTER_FIELD:
        strError = _("Format error in certificate's notAfter field");
		return;
	case BEID_CERTSTATUS_ERR_IN_CRL_LAST_UPDATE_FIELD:
        strError = _("Format error in CRL's lastUpdate field");
		return;
	case BEID_CERTSTATUS_ERR_IN_CRL_NEXT_UPDATE_FIELD:
        strError = _("Format error in CRL's nextUpdate field");
		return;
	case BEID_CERTSTATUS_OUT_OF_MEM:
        strError = _("Out of memory");
		return;
	case BEID_CERTSTATUS_DEPTH_ZERO_SELF_SIGNED_CERT:
        strError = _("Self signed certificate");
		return;
	case BEID_CERTSTATUS_SELF_SIGNED_CERT_IN_CHAIN:
        strError = _("Self signed certificate in certificate chain");
		return;
	case BEID_CERTSTATUS_UNABLE_TO_GET_ISSUER_CERT_LOCALLY:
        strError = _("Unable to get local issuer certificate");
		return;
	case BEID_CERTSTATUS_UNABLE_TO_VERIFY_LEAF_SIGNATURE:
        strError = _("Unable to verify the first certificate");
		return;
	case BEID_CERTSTATUS_CERT_CHAIN_TOO_LONG:
        strError = _("Certificate chain too long");
		return;
	case BEID_CERTSTATUS_CERT_REVOKED:
        strError = _("Certificate revoked");
		return;
	case BEID_CERTSTATUS_INVALID_CA:
        strError = _("Invalid CA certificate");
		return;
	case BEID_CERTSTATUS_PATH_LENGTH_EXCEEDED:
        strError = _("Path length constraint exceeded");
		return;
	case BEID_CERTSTATUS_INVALID_PURPOSE:
        strError = _("Unsupported certificate purpose");
		return;
	case BEID_CERTSTATUS_CERT_UNTRUSTED:
        strError = _("Certificate not trusted");
		return;
	case BEID_CERTSTATUS_CERT_REJECTED:
        strError = _("Certificate rejected");
		return;
	case BEID_CERTSTATUS_SUBJECT_ISSUER_MISMATCH:
        strError = _("Subject issuer mismatch");
		return;
	case BEID_CERTSTATUS_AKID_SKID_MISMATCH:
        strError = _("Authority and subject key identifier mismatch");
		return;
	case BEID_CERTSTATUS_AKID_ISSUER_SERIAL_MISMATCH:
        strError = _("Authority and issuer serial number mismatch");
		return;
	case BEID_CERTSTATUS_KEYUSAGE_NO_CERTSIGN:
        strError = _("Key usage does not include certificate signing");
		return;
	case BEID_CERTSTATUS_UNABLE_TO_GET_CRL_ISSUER:
        strError = _("Unable to get CRL issuer certificate");
		return;
	case BEID_CERTSTATUS_UNHANDLED_CRITICAL_EXTENSION:
        strError = _("Unhandled critical extension");
		return;

	default:
        strError = _("Unknown certificate status");
		return;
		}
}

wxTreeItemId eidviewerNotebook::FindData(wxTreeItemId hti, CCertif *pData)
{
    wxTreeCtrl *pCtrlTreeCerts = (wxTreeCtrl *)FindWindow(IDC_TREE_CERTIFICATES);
    if(!hti.IsOk())
        return hti;
    wxTreeItemCertif *pItemData = (wxTreeItemCertif *)pCtrlTreeCerts->GetItemData( hti );
    if(pItemData != NULL && pItemData->GetCertif() == pData)
    {
        return hti;
    }

    long lCookie = 0;
    hti = pCtrlTreeCerts->GetFirstChild( hti, lCookie );	
    do	
    {		
	    wxTreeItemId hti_res = FindData( hti, pData);
	    if(hti_res.IsOk())
		    return hti_res; 

        if(hti.IsOk())
        {
            hti = pCtrlTreeCerts->GetNextSibling( hti );
        }
    }while( hti.IsOk() );		
    return hti;
}

void eidviewerNotebook::OnDblClickedTreeCertificates(wxTreeEvent&) 
{
}

void eidviewerNotebook::OnSelchangedTreeCertificates(wxTreeEvent& event) 
{
    wxTreeCtrl *pCtrlTreeCerts = (wxTreeCtrl *)FindWindow(IDC_TREE_CERTIFICATES);
    wxButton *pButtonRegister = (wxButton *)FindWindow(IDC_BUTTON_REGISTER);
    CCertif *pCertif = NULL;
    wxTreeItemId hItem = event.GetItem();
    if(hItem.IsOk())
    {
        wxTreeItemCertif *pItemData = (wxTreeItemCertif *)pCtrlTreeCerts->GetItemData (hItem);
        if(pItemData != NULL && pItemData->GetCertif() != NULL)
        {
            pCertif = pItemData->GetCertif();
        }
        if(!pCtrlTreeCerts->ItemHasChildren(hItem))
        {
            pButtonRegister->Enable(TRUE); 
        }
        else
        {
            pButtonRegister->Enable(FALSE); 
        }
    }
    SetTextFields (pCertif);
}

void eidviewerNotebook::OnButtonCertificatesDetails(wxCommandEvent& WXUNUSED(event)) 
{
    CCertif *pCertif = NULL;
    wxTreeCtrl *pCtrlTreeCerts = (wxTreeCtrl *)FindWindow(IDC_TREE_CERTIFICATES);
    wxTreeItemId hItem = pCtrlTreeCerts->GetSelection ();
    if (hItem.IsOk())
    {
        wxTreeItemCertif *pItemData = (wxTreeItemCertif *)pCtrlTreeCerts->GetItemData (hItem);
        if(pItemData != NULL && pItemData->GetCertif() != NULL)
        {
            pCertif = pItemData->GetCertif();
        }
    }

    if (NULL != pCertif)
    {
        wxBusyCursor wc;

        wxString strFileTmp = wxFileName::CreateTempFileName(wxT("cert"), NULL);
        wxFileName oName(strFileTmp);
        oName.SetExt(wxT("cer"));
        wxString strFile;
        strFile = oName.GetFullPath();
        wxFile oFile(strFile, wxFile::write);

        if(!oFile.IsOpened())
            return;

        oFile.Write(pCertif->GetData(), pCertif->GetLength());
        oFile.Close();

        wxFileType *filetype = wxTheMimeTypesManager->GetFileTypeFromExtension(wxT("cer"));
        if(NULL == filetype)
        {
            filetype = wxTheMimeTypesManager->GetFileTypeFromMimeType(wxT("application/x-x509-ca-cert"));
        }
        if (filetype) 
        {
            wxString cmd;
            if (filetype->GetOpenCommand (&cmd, wxFileType::MessageParameters (strFile))) 
            {
                ::wxExecute(cmd);
            }
            delete filetype;
        }
//#ifdef _WIN32
        // Obsolete
        // wxString strExec(wxT("rundll32.exe cryptext.dll,CryptExtOpenCER "));
        // strExec += strFile;
        // wxExecute(strExec);
//#endif
        wxRemoveFile(strFileTmp);
        // When the System is too slow gives problem
        //wxRemoveFile(strFile);
    }
}

void eidviewerNotebook::OnButtonRegister(wxCommandEvent& WXUNUSED(event)) 
{
#ifdef _WIN32
    wxGetApp().SetStatusBarText(wxT(""));
    wxBusyCursor wc;
	if(0 == CreateContextFile())
    {
        wxString strMsg(_("Registration of the certificate succeeded"));
        wxGetApp().SetStatusBarText(strMsg);
        wxMessageDialog oDialog(wxGetApp().GetTopWindow(), strMsg, wxGetApp().GetTopWindow()->GetTitle(), wxOK | wxICON_INFORMATION);
        oDialog.ShowModal();        
    }
    else
    {
        wxString strMsg(_("Registration of the certificate failed !")); 
        wxGetApp().SetStatusBarText(strMsg);
        wxMessageDialog oDialog(wxGetApp().GetTopWindow(), strMsg, wxGetApp().GetTopWindow()->GetTitle(), wxOK | wxICON_ERROR);
        oDialog.ShowModal();        
    }
#endif
}


void eidviewerNotebook::FillListVersionInfo()
{
    wxListCtrl *pListCtrl = (wxListCtrl *)FindWindow(IDC_LIST_CARDDATA_VERSIONINFO);

    pListCtrl->DeleteAllItems();

    unsigned char ucSerial[16] = {0};
    if(0 == memcmp(m_VersionInfo.SerialNumber, ucSerial, 16))
        return;

    int i = 0;
    pListCtrl->InsertItem(i, m_vecVersionLabels[i]);
    char *pSerialNr = wxGetApp().Hexify(m_VersionInfo.SerialNumber, 16);
    wxString strSerial(pSerialNr, *wxConvCurrent);
    pListCtrl->SetItem(i++, 1, strSerial);
    delete pSerialNr;
    pListCtrl->InsertItem(i, m_vecVersionLabels[i]); 
    pListCtrl->SetItem(i++, 1, wxString::Format(wxT("%02X"), m_VersionInfo.ComponentCode)); 
    pListCtrl->InsertItem(i, m_vecVersionLabels[i]);   
    pListCtrl->SetItem(i++, 1, wxString::Format(wxT("%02X"), m_VersionInfo.OSNumber)); 
    pListCtrl->InsertItem(i, m_vecVersionLabels[i]);  
    pListCtrl->SetItem(i++, 1, wxString::Format(wxT("%02X"), m_VersionInfo.OSVersion)); 
    pListCtrl->InsertItem(i, m_vecVersionLabels[i]);      
    pListCtrl->SetItem(i++, 1, wxString::Format(wxT("%02X"), m_VersionInfo.SoftmaskNumber)); 
    pListCtrl->InsertItem(i, m_vecVersionLabels[i]);  
    pListCtrl->SetItem(i++, 1, wxString::Format(wxT("%02X"), m_VersionInfo.SoftmaskVersion)); 
    pListCtrl->InsertItem(i, m_vecVersionLabels[i]);      
    pListCtrl->SetItem(i++, 1, wxString::Format(wxT("%02X"), m_VersionInfo.AppletVersion)); 
    pListCtrl->InsertItem(i, m_vecVersionLabels[i]);      
    pListCtrl->SetItem(i++, 1, wxString::Format(wxT("%04X"), m_VersionInfo.GlobalOSVersion)); 
    pListCtrl->InsertItem(i, m_vecVersionLabels[i]);  
    pListCtrl->SetItem(i++, 1, wxString::Format(wxT("%02X"), m_VersionInfo.AppletInterfaceVersion)); 
    pListCtrl->InsertItem(i, m_vecVersionLabels[i]);      
    pListCtrl->SetItem(i++, 1, wxString::Format(wxT("%02X"), m_VersionInfo.PKCS1Support)); 
    pListCtrl->InsertItem(i, m_vecVersionLabels[i]);  
    pListCtrl->SetItem(i++, 1, wxString::Format(wxT("%02X"), m_VersionInfo.KeyExchangeVersion)); 
    pListCtrl->InsertItem(i, m_vecVersionLabels[i]);      
    pListCtrl->SetItem(i++, 1, wxString::Format(wxT("%02X"), m_VersionInfo.ApplicationLifeCycle)); 
    pListCtrl->InsertItem(i, m_vecVersionLabels[i]);      
    pListCtrl->SetItem(i++, 1, wxString::Format(wxT("%02X"), m_VersionInfo.GraphPerso)); 
    pListCtrl->InsertItem(i, m_vecVersionLabels[i]);  
    pListCtrl->SetItem(i++, 1, wxString::Format(wxT("%02X"), m_VersionInfo.ElecPerso)); 
    pListCtrl->InsertItem(i, m_vecVersionLabels[i]);      
    pListCtrl->SetItem(i++, 1, wxString::Format(wxT("%02X"), m_VersionInfo.ElecPersoInterface)); 

    int iWidth = 0;
    int iHeight = 0;
    pListCtrl->GetClientSize(&iWidth, &iHeight);
    pListCtrl->SetColumnWidth(0, iWidth/2);
    pListCtrl->SetColumnWidth(1, iWidth/2);
}

void eidviewerNotebook::FillTreePins()
{
    wxTreeCtrl *pTreeCtrl = (wxTreeCtrl *)FindWindow(IDC_TREE_CARDDATA_PIN);

    pTreeCtrl->DeleteAllItems();
  
    SetTextFieldsPinData (NULL);

     if (0 == m_oMapPins.size())
    {
        return;
    }
    
    wxFont *pFont = wxGetApp().GetDataFont();
    if(pFont)
        pTreeCtrl->SetFont(*pFont); 

    std::map<wxString, std::vector<CPin> >::iterator it;
    for (it = m_oMapPins.begin(); it != m_oMapPins.end(); ++it)
    {
        wxTreeItemId hRoot = pTreeCtrl->AddRoot((*it).first, 1);
        for(unsigned int i = 0; i < (*it).second.size(); ++i)
        {
              CPin & oPin = (*it).second[i];
              CPin *pPin = new CPin(oPin);
              pTreeCtrl->AppendItem(hRoot, oPin.GetLabel(), 0, -1, new wxTreeItemPin(pPin));
        }
        if(hRoot.IsOk())
        {
            pTreeCtrl->Expand(hRoot); 
        }
    }
}

void eidviewerNotebook::SetPINData(std::map<wxString, std::vector<CPin> > & PinMap)
{
    m_oMapPins.clear();

    m_oMapPins = PinMap;
    FillTreePins();
}

void eidviewerNotebook::SetTextFieldsPinData(CPin *pPin) 
{
    wxTextCtrl *pCtrlName = (wxTextCtrl *)FindWindow(IDC_STATIC_CARDDATA_PINNAME_VAL);
    wxTextCtrl *pCtrlID = (wxTextCtrl *)FindWindow(IDC_STATIC_CARDDATA_PINID_VAL);
    wxTextCtrl *pCtrlStatus = (wxTextCtrl *)FindWindow(IDC_STATIC_CARDDATA_PINSTATUS_VAL);
    wxButton *pCtrlButtonChange = (wxButton *)FindWindow(IDC_BUTTON_CARDDATA_CHANGEPIN);
    wxFont *pFont = wxGetApp().GetDataFont();

    if(pPin == NULL)
    {
        // Clear all fields
        pCtrlStatus->SetValue(wxT(""));
	    pCtrlName->SetValue(wxT(""));
	    pCtrlID->SetValue(wxT(""));
        pCtrlButtonChange->Enable(FALSE);
    }
    else
    {
        long lLeft = pPin->GetTriesLeft(); 
        if(-1 == lLeft)
        {
            pCtrlStatus->SetValue(_("Unknown PIN Status"));
        }
        else
        {
            pCtrlStatus->SetValue(wxString::Format(_("%ld tries remaining"), lLeft));
        }
        if(pFont)
            pCtrlStatus->SetFont(*pFont); 
	    pCtrlName->SetValue(pPin->GetLabel());
        if(pFont)
            pCtrlName->SetFont(*pFont); 
        pCtrlID->SetValue(wxString::Format(wxT("%02d"), pPin->GetReference()));
        if(pFont)
            pCtrlID->SetFont(*pFont); 
        pCtrlButtonChange->Enable(TRUE);
    }
}

void eidviewerNotebook::OnSelchangedTreePins(wxTreeEvent& event) 
{
    wxTreeCtrl *pCtrlTree = (wxTreeCtrl *)FindWindow(IDC_TREE_CARDDATA_PIN);
    CPin *pPin = NULL;
    wxTreeItemId hItem = event.GetItem();
    if(hItem.IsOk())
    {
        wxTreeItemPin *pItemData = (wxTreeItemPin *)pCtrlTree->GetItemData (hItem);
        if(pItemData != NULL && pItemData->GetPin() != NULL)
        {
            pPin = pItemData->GetPin();
        }
    }
    SetTextFieldsPinData (pPin);
}

void eidviewerNotebook::OnButtonCarddataChangepin(wxCommandEvent& WXUNUSED(event)) 
{
    wxTreeCtrl *pCtrlTree = (wxTreeCtrl *)FindWindow(IDC_TREE_CARDDATA_PIN);
    wxTextCtrl *pCtrlStatus = (wxTextCtrl *)FindWindow(IDC_STATIC_CARDDATA_PINSTATUS_VAL);
    CPin *pPin = NULL;
    wxTreeItemId hItem = pCtrlTree->GetSelection();
    if (hItem.IsOk())
    {
        wxTreeItemPin *pItemData = (wxTreeItemPin *)pCtrlTree->GetItemData (hItem);
        if(pItemData != NULL && pItemData->GetPin() != NULL)
        {
            pPin = pItemData->GetPin();
        }
    }

    if (NULL != pPin)
    {
	    if(!wxGetApp().ChangePin(pPin))
            return;

        long lLeft = pPin->GetTriesLeft(); 
        if(-1 == lLeft)
        {
            pCtrlStatus->SetValue(_("Unknown PIN Status"));
        }
        else
        {
            pCtrlStatus->SetValue(wxString::Format(_("%ld tries remaining"), lLeft));
        }
    }
}

void eidviewerNotebook::AdjustCRL()
{
    wxChoice *pChoiceOCSP = (wxChoice *)FindWindow(IDC_COMBO_OPTIONS_OCSP_VAL);
    wxChoice *pChoiceCRL = (wxChoice *)FindWindow(IDC_COMBO_OPTIONS_CRL_VAL);

    int iIndex = pChoiceOCSP->GetSelection();
    int iOldIndex = pChoiceCRL->GetSelection();
    if (iIndex == 2)
    {
        // Remove Mandatory selection from CRL
        if(3 == pChoiceCRL->GetCount())
        {
            pChoiceCRL->Delete(2); 
        }
    }
    else
    {
        // Add Mandatory selection to CRL
        if(2 == pChoiceCRL->GetCount())
        {
            pChoiceCRL->Append(_("Mandatory"));             
        }
    }
    if(iOldIndex != -1)
    {
           pChoiceCRL->SetSelection(iOldIndex); 
    }
}

void eidviewerNotebook::AdjustOCSP() 
{
    wxChoice *pChoiceOCSP = (wxChoice *)FindWindow(IDC_COMBO_OPTIONS_OCSP_VAL);
    wxChoice *pChoiceCRL = (wxChoice *)FindWindow(IDC_COMBO_OPTIONS_CRL_VAL);

    int iIndex = pChoiceCRL->GetSelection();
    int iOldIndex = pChoiceOCSP->GetSelection();
    if (iIndex == 2)
    {
        // Remove Mandatory selection from OCSP
        if(3 == pChoiceOCSP->GetCount())
        {
            pChoiceOCSP->Delete(2); 
        }
    }
    else
    {
        // Add Mandatory selection to OCSP
        if(2 == pChoiceOCSP->GetCount())
        {
            pChoiceOCSP->Append(_("Mandatory"));             
        }
    }
    if(iOldIndex != -1)
    {
           pChoiceOCSP->SetSelection(iOldIndex); 
    }
}


void eidviewerNotebook::OnSelchangeComboOptionsOcspVal(wxCommandEvent& WXUNUSED(event)) 
{
    AdjustCRL();
    OnButtonOptionsApply();
}

void eidviewerNotebook::OnSelchangeComboOptionsCrlVal(wxCommandEvent& WXUNUSED(event)) 
{
    AdjustOCSP();
    OnButtonOptionsApply();	
}

void eidviewerNotebook::OnSelchangeComboOptionsReaderVal(wxCommandEvent& WXUNUSED(event)) 
{
    OnButtonOptionsApply();	
}

void eidviewerNotebook::OnOptionsLanguage(wxCommandEvent& WXUNUSED(event))
{
    OnButtonOptionsApply();	
    wxGetApp().SwitchLanguage(); 
}

void eidviewerNotebook::OnButtonOptionsApply() 
{
    wxChoice *pChoiceOCSP = (wxChoice *)FindWindow(IDC_COMBO_OPTIONS_OCSP_VAL);
    wxChoice *pChoiceCRL = (wxChoice *)FindWindow(IDC_COMBO_OPTIONS_CRL_VAL);
    wxChoice *pChoiceReaders = (wxChoice *)FindWindow(IDC_COMBO_OPTIONS_READER_VAL);
    wxRadioBox *pRadioBox = (wxRadioBox *)FindWindow(IDC_RADIOBOX_OPTIONS_LANGUAGE);

    eidviewer::CConfig & oConfig = wxGetApp().GetConfig();
    oConfig.m_dwLanguage = pRadioBox->GetSelection();
    oConfig.m_dwOCSP = pChoiceOCSP->GetSelection();    
    oConfig.m_dwCRL = pChoiceCRL->GetSelection();
    int iIndex = pChoiceReaders->GetSelection();
    if (iIndex > 0)
    {
         oConfig.m_StrReader = pChoiceReaders->GetStringSelection();   
    }
    else
    {
        oConfig.m_StrReader = _T("");
    }
    wxGetApp().SaveConfiguration(); 
}


////////////////////////////////////
#ifdef _WIN32

int eidviewerNotebook::CreateContextFile()
{
    int iError = 0;
    PCCERT_CONTEXT pCertContext = NULL;
    BYTE  KeyUsageBits = 0;    // Intended key usage bits copied to here.
    DWORD cbKeyUsageByteCount = 1; // 1 byte will be copied to *pbKeyUsage

    try
    {
        wxTreeCtrl *pCtrlTreeCerts = (wxTreeCtrl *)FindWindow(IDC_TREE_CERTIFICATES);
        wxTreeItemId hItem = pCtrlTreeCerts->GetSelection ();
        while(hItem.IsOk())
        {
            CCertif *pCertif = NULL;
            wxTreeItemCertif *pItemData = (wxTreeItemCertif *)pCtrlTreeCerts->GetItemData (hItem);
            if(pItemData != NULL && pItemData->GetCertif() != NULL)
            {
                pCertif = pItemData->GetCertif();
            }
            if(pCertif != NULL)
            {
                // Create a certificate context in memory
                pCertContext = CertCreateCertificateContext(MY_ENCODING_TYPE, pCertif->GetData(), pCertif->GetLength());

                if(NULL == pCertContext)  // Did we get a context?
                    throw (int)ERR_CREATE_CERT_CONTEXT;

                CertGetIntendedKeyUsage(X509_ASN_ENCODING, pCertContext->pCertInfo, &KeyUsageBits, cbKeyUsageByteCount);
                pCertif->SetKeyUsage(KeyUsageBits);
                pCertif->SetKeyLength(CertGetPublicKeyLength (X509_ASN_ENCODING, &pCertContext->pCertInfo->SubjectPublicKeyInfo)/8); 

                // Only store the context of the certificates with usages for an end-user i.e. no CA or root certificates
                if (((KeyUsageBits & CERT_DIGITAL_SIGNATURE_KEY_USAGE) == CERT_DIGITAL_SIGNATURE_KEY_USAGE) ||
                    ((KeyUsageBits & CERT_NON_REPUDIATION_KEY_USAGE) == CERT_NON_REPUDIATION_KEY_USAGE))
                {
                    if(!StoreUserCerts (pCertContext, KeyUsageBits, pCertif))
                        throw (int)ERR_CANNOT_ADD_CERTIFICATE_TO_STORE;
                }
                else
                {
                    if(!StoreAuthorityCerts (pCertContext, KeyUsageBits))
                        throw (int)ERR_CANNOT_ADD_CERTIFICATE_TO_STORE;
                }
                if (NULL != pCertContext)
                    CertFreeCertificateContext (pCertContext);
            }
            hItem = pCtrlTreeCerts->GetItemParent(hItem);
        }
        /*
        if (!StoreConfigurationFile ())
            throw (int)ERR_CANNOT_SAVE_CONFIGURATIONFILE;
        */
    }
    catch (int e)
    {
        iError = e;
    }

    return (iError);
}

BOOL eidviewerNotebook::StoreUserCerts (PCCERT_CONTEXT pCertContext, BYTE KeyUsageBits, CCertif *pCertData)
{
    DWORD  dwFlags =  CERT_STORE_NO_CRYPT_RELEASE_FLAG;
    HCERTSTORE  hMyStore = NULL;              // memory store handle
    int iError = 0;

    try
    {
        hMyStore = CertOpenSystemStore (NULL, wxT("MY"));
        if (NULL == hMyStore)
            throw (int) ERR_CANNOT_CREATE_MEM_STORE;

        PCCERT_CONTEXT  pDesiredCert = NULL;
        if(NULL != (pDesiredCert = CertFindCertificateInStore(hMyStore, X509_ASN_ENCODING, 0, 
                    CERT_FIND_EXISTING, pCertContext , NULL)))
        {
            CertFreeCertificateContext(pDesiredCert);
            CertCloseStore (hMyStore, CERT_CLOSE_STORE_FORCE_FLAG);
            return true;
        }

        // Initalize the CRYPT_KEY_PROV_INFO data structure.
        // Note: pwszContainerName and pwszProvName can be set to NULL 
        // to use the default container and provider.
        CRYPT_KEY_PROV_INFO *pCryptKeyProvInfo;
        DWORD dwPropId = CERT_KEY_PROV_INFO_PROP_ID; 
        pCryptKeyProvInfo = new CRYPT_KEY_PROV_INFO;

        // Get the serial number from the card
        char *pSerialKey = wxGetApp().Hexify(m_VersionInfo.SerialNumber, sizeof(m_VersionInfo.SerialNumber));

        wxString strContainerName(pCertData->GetID(), *wxConvCurrent);
        strContainerName += wxT("(");
        wxString strSerial(pSerialKey, *wxConvCurrent);
        strContainerName += strSerial;
        strContainerName += wxT(")");
 
#if wxUSE_UNICODE
        pCryptKeyProvInfo->pwszContainerName = (wxChar *)strContainerName.c_str();
        pCryptKeyProvInfo->pwszProvName = wxT("Belgium Identity Card CSP");
#else
        wxWCharBuffer pTemp = strContainerName.wc_str(*wxConvCurrent);
        pCryptKeyProvInfo->pwszContainerName = (unsigned short *)pTemp.data();
        pCryptKeyProvInfo->pwszProvName = L"Belgium Identity Card CSP";
#endif

        pCryptKeyProvInfo->dwProvType = PROV_RSA_FULL;
        pCryptKeyProvInfo->dwFlags = 0;
        pCryptKeyProvInfo->cProvParam = 0;
        pCryptKeyProvInfo->rgProvParam = NULL;
        pCryptKeyProvInfo->dwKeySpec = AT_KEYEXCHANGE;
        //
        // Set the property.
        if(!CertSetCertificateContextProperty(
                                                pCertContext,       // A pointer to the certificate
                                                     // where the propertiy will be set.
                                                dwPropId,           // An identifier of the property to be set. 
                                                     // In this case, CERT_KEY_PROV_INFO_PROP_ID
                                                     // is to be set to provide a pointer with the
                                                     // certificate to its associated private key 
                                                     // container.
                                                dwFlags,            // The flag used in this case is   
                                                     // CERT_STORE_NO_CRYPT_RELEASE_FLAG
                                                     // indicating that the cryptographic 
                                                     // context aquired should not
                                                     // be released when the function finishes.
                                                pCryptKeyProvInfo   // A pointer to a data structure that holds
                                                     // infomation on the private key container to
                                                     // be associated with this certificate.
                                                ))
            throw (int) ERR_CANNOT_SET_PROPERTY;

        if (NULL != pCryptKeyProvInfo)
            delete (pCryptKeyProvInfo);
        pCryptKeyProvInfo = NULL;

        // Set friendly names for the certificates
        CRYPT_DATA_BLOB tpFriendlyName;
        wxString strFriendlyName;

        if (((KeyUsageBits & CERT_NON_REPUDIATION_KEY_USAGE) == CERT_NON_REPUDIATION_KEY_USAGE))
        {
            pCertData->SetKeyID(0x03);
            strFriendlyName = _("Signature Key");
        }
        else
        {
            pCertData->SetKeyID(0x02);
            strFriendlyName = _("Authentication Key");
        }

#if wxUSE_UNICODE
        int iFriendLen = (strFriendlyName.Length() + 1) * sizeof(wxChar);
        tpFriendlyName.pbData = new BYTE[iFriendLen];
        memset(tpFriendlyName.pbData, 0, iFriendLen);
        memcpy(tpFriendlyName.pbData, strFriendlyName.c_str(), strFriendlyName.Length() * sizeof(wxChar));
        tpFriendlyName.cbData = iFriendLen;
#else
        int iFriendLen = (strFriendlyName.Length() + 1) * sizeof(wxChar);
        iFriendLen = iFriendLen * 2;
        tpFriendlyName.pbData = new BYTE[iFriendLen];
        memset(tpFriendlyName.pbData, 0, iFriendLen);
        wxWCharBuffer pTempName = strFriendlyName.wc_str(*wxConvCurrent);
        memcpy(tpFriendlyName.pbData, pTempName.data(), iFriendLen - 2);
        tpFriendlyName.cbData = iFriendLen;
#endif

        // Set the property.
        if(!CertSetCertificateContextProperty(
                                                pCertContext,       // A pointer to the certificate
                                                     // where the propertiy will be set.
                                                CERT_FRIENDLY_NAME_PROP_ID,           // An identifier of the property to be set. 
                                                     // In this case, CERT_KEY_PROV_INFO_PROP_ID
                                                     // is to be set to provide a pointer with the
                                                     // certificate to its associated private key 
                                                     // container.
                                                dwFlags,            // The flag used in this case is   
                                                     // CERT_STORE_NO_CRYPT_RELEASE_FLAG
                                                     // indicating that the cryptographic 
                                                     // context aquired should not
                                                     // be released when the function finishes.
                                                &tpFriendlyName   // A pointer to a data structure that holds
                                                     // infomation on the private key container to
                                                     // be associated with this certificate.
                                                ))
            throw (int) ERR_CANNOT_SET_PROPERTY;

       if (NULL != tpFriendlyName.pbData)
            delete [] (tpFriendlyName.pbData);
        tpFriendlyName.pbData = NULL;

        if (((KeyUsageBits & CERT_NON_REPUDIATION_KEY_USAGE) == CERT_NON_REPUDIATION_KEY_USAGE))
        {
            if (!CertAddEnhancedKeyUsageIdentifier (pCertContext, szOID_PKIX_KP_EMAIL_PROTECTION))
                throw (int) ERR_CANNOT_SET_PROPERTY;
        }
        else
        {
            if (!CertAddEnhancedKeyUsageIdentifier (pCertContext, szOID_PKIX_KP_EMAIL_PROTECTION))
                throw (int) ERR_CANNOT_SET_PROPERTY;
            if (!CertAddEnhancedKeyUsageIdentifier (pCertContext, szOID_PKIX_KP_CLIENT_AUTH))
                throw (int) ERR_CANNOT_SET_PROPERTY;
        }

        if(!CertAddCertificateContextToStore(hMyStore, pCertContext, CERT_STORE_ADD_REPLACE_EXISTING, NULL))
            throw (int) ERR_CANNOT_ADD_CERTIFICATE_TO_STORE;
        CertCloseStore (hMyStore, CERT_CLOSE_STORE_FORCE_FLAG);
        hMyStore = NULL;
    }
    catch (int e)
    {
        CertCloseStore (hMyStore, CERT_CLOSE_STORE_FORCE_FLAG);
        hMyStore = NULL;
        iError = e;
    }

    return (iError == 0 ? TRUE : FALSE);
}

BOOL eidviewerNotebook::StoreAuthorityCerts(PCCERT_CONTEXT  pCertContext, BYTE  KeyUsageBits)
{
    HCERTSTORE  hMemoryStore = NULL;              // memory store handle
    int iError = 0;

    try
    {
        if ((KeyUsageBits & CERT_KEY_CERT_SIGN_KEY_USAGE) == CERT_KEY_CERT_SIGN_KEY_USAGE)
        {
            if (memcmp (pCertContext->pCertInfo->Issuer.pbData, 
                        pCertContext->pCertInfo->Subject.pbData, 
                        pCertContext->pCertInfo->Subject.cbData) == 0)
            {
                hMemoryStore = CertOpenSystemStore (NULL, wxT("ROOT"));
                if (NULL == hMemoryStore)
                    throw (int) ERR_CANNOT_CREATE_MEM_STORE;
            }
            else
            {
                hMemoryStore = CertOpenSystemStore (NULL, wxT("CA"));
                if (NULL == hMemoryStore)
                    throw (int) ERR_CANNOT_CREATE_MEM_STORE;
            }
            PCCERT_CONTEXT  pDesiredCert = NULL;
            if(NULL != (pDesiredCert = CertFindCertificateInStore(hMemoryStore, X509_ASN_ENCODING, 0, 
                        CERT_FIND_EXISTING, pCertContext , NULL)))
            {
                CertFreeCertificateContext(pDesiredCert);
                CertCloseStore (hMemoryStore, CERT_CLOSE_STORE_FORCE_FLAG);
                return true;
            }
            if (!CertAddEnhancedKeyUsageIdentifier (pCertContext, szOID_PKIX_KP_EMAIL_PROTECTION))
                throw (int) ERR_CANNOT_SET_PROPERTY;
            if (!CertAddEnhancedKeyUsageIdentifier (pCertContext, szOID_PKIX_KP_SERVER_AUTH))
                throw (int) ERR_CANNOT_SET_PROPERTY;
            if(!CertAddCertificateContextToStore(hMemoryStore, pCertContext, CERT_STORE_ADD_NEWER, NULL))
            {
                if (GetLastError () != CRYPT_E_EXISTS)
                    throw (int) ERR_CANNOT_ADD_CERTIFICATE_TO_STORE;
            }
            CertCloseStore (hMemoryStore, CERT_CLOSE_STORE_FORCE_FLAG);
            hMemoryStore = NULL;
        }
    }
    catch (int e)
    {
        CertCloseStore (hMemoryStore, CERT_CLOSE_STORE_FORCE_FLAG);
        hMemoryStore = NULL;
        iError = e;
    }

    return (iError == 0 ? TRUE : FALSE);

}


BOOL eidviewerNotebook::StoreConfigurationFile ()
{
   BOOL bReturnVal = TRUE;
   bReturnVal = StoreBelgianEIDConfigurationFile ();
   return (bReturnVal);
}

BOOL eidviewerNotebook::StoreBelgianEIDConfigurationFile ()
{
    BOOL bReturnVal = TRUE;
    wxString strOS = wxGetOSDirectory();
    wxString strFileName(strOS);
    strFileName += wxT("\\System32\\beidcsp.conf");
    MapCertifsIt it;
    CBelgianEIDConfig *pConfig = static_cast <CBelgianEIDConfig *>(CScConfig::GetConfigObject(strFileName));
    if(pConfig == NULL)
    {
        pConfig = static_cast <CBelgianEIDConfig *>(CScConfig::CreateConfigObject (BELGIAN_EID_CARD));            
    }

    CSlotInfo *pSlotInfo;
    std::vector <CSlotInfo *> oVecSlotInfo = pConfig->GetSlotInfo();
    if(oVecSlotInfo.size() > 0)
    {
        pSlotInfo = oVecSlotInfo[0];
    }
    else
    {
        pSlotInfo = new CSlotInfo();
        unsigned char ucCardID[7] = {0x42, 0x45, 0x4C, 0x50, 0x49, 0x43, 0x00};
        pConfig->SetCardID (ucCardID, sizeof(ucCardID));
    }

    for (it = m_oMapCertifs.begin (); it != m_oMapCertifs.end (); it++)
    {
        CCertif *pCertif = (*it).second;
        // Only keep  the certificates in case it is a certificate with usages for an end-user i.e. no CA or root certificates
        if ( ((pCertif->GetKeyUsage () & CERT_DIGITAL_SIGNATURE_KEY_USAGE) == CERT_DIGITAL_SIGNATURE_KEY_USAGE) ||
             ((pCertif->GetKeyUsage () & CERT_NON_REPUDIATION_KEY_USAGE) == CERT_NON_REPUDIATION_KEY_USAGE) )
        {
            CKeyInfo *pKeyInfo;
            pKeyInfo = new CKeyInfo;

            pKeyInfo->SetKeyContainerName ((unsigned char *)pCertif->GetID(), strlen ((const char *)pCertif->GetID()));
            pKeyInfo->SetKeyLength (pCertif->GetKeyLength ());
            pKeyInfo->SetKeyID (pCertif->GetKeyID());

            pKeyInfo->SetKeyUsage ((DWORD)pCertif->GetKeyUsage());
            pSlotInfo->AddKeyInfo (pKeyInfo);
        }
    }

    pConfig->ClearSlots(); 
    pConfig->AddSlotInfo (pSlotInfo);

    pConfig->Write (strFileName);
    return (bReturnVal);
}

void eidviewerNotebook::FillModuleVersionInfo()
{
    int iFileIndex = 0;
    std::map<int, wxString> oMapFiles;
    oMapFiles[iFileIndex++] = wxT("beidlibopensc.dll");
    oMapFiles[iFileIndex++] = wxT("beidgui.dll");
    oMapFiles[iFileIndex++] = wxT("beidpkcs11.dll");
    oMapFiles[iFileIndex++] = wxT("beidcsp.dll");
    oMapFiles[iFileIndex++] = wxT("beidservicepcsc.exe");
    oMapFiles[iFileIndex++] = wxT("beidservicecrl.exe");
    oMapFiles[iFileIndex++] = wxT("beidlib.dll");
    oMapFiles[iFileIndex++] = wxT("beidwinscard.dll");
    oMapFiles[iFileIndex++] = wxT("beidlibeay32.dll");
    oMapFiles[iFileIndex++] = wxT("beidssleay32.dll");

    wxListCtrl *pListCtrl = (wxListCtrl *)FindWindow(IDC_LISTCTRL_VERSION_MODULES);
    pListCtrl->DeleteAllItems();

    CFileVersionInfo fvi;
	char szFile[ MAX_PATH ] = { 0 };
    ::GetModuleFileNameA( NULL, szFile, MAX_PATH );

    iFileIndex = 0;
    if(fvi.Open(wxString(szFile, *wxConvCurrent)))
    {
        wxString strVersion = wxString::Format(wxT("%s %d.%d.%d"), _("Belgian eID Run-time Version"), fvi.GetProductVersionMajor(), fvi.GetProductVersionMinor(),
            fvi.GetProductVersionBuild());
        wxStaticText *pProductVersion = (wxStaticText *)FindWindow(IDC_STATIC_VERSION_PRODUCT);
        wxFont oFontHeader =  pProductVersion->GetFont();
        oFontHeader.SetWeight(wxBOLD);
        pProductVersion->SetFont(oFontHeader);
        pProductVersion->SetLabel(strVersion);
        strVersion = wxString::Format(wxT("%d.%d.%d.%d"), fvi.GetFileVersionMajor(), fvi.GetFileVersionMinor(),
                fvi.GetFileVersionBuild(), fvi.GetFileVersionQFE());  
        pListCtrl->InsertItem(iFileIndex, wxT("beidgui.exe"));
        pListCtrl->SetItem(iFileIndex++, 1, strVersion); 
        
        fvi.Close();
    }

    for(unsigned int i = 0; i < oMapFiles.size(); i++)
    {
        if(fvi.Open(oMapFiles[i]))
        {
            wxString strVersion = wxString::Format(wxT("%d.%d.%d.%d"), fvi.GetFileVersionMajor(), fvi.GetFileVersionMinor(),
                fvi.GetFileVersionBuild(), fvi.GetFileVersionQFE());   
            pListCtrl->InsertItem(iFileIndex, oMapFiles[i]);
            pListCtrl->SetItem(iFileIndex++, 1, strVersion); 
            fvi.Close();
        }
    }

}

#endif
