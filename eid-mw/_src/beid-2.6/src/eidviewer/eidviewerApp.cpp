/////////////////////////////////////////////////////////////////////////////
// Name:        eidviewerApp.cpp
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "wx/image.h"
#include "wx/fontenum.h"
#include "wx/file.h"
#include "wx/snglinst.h"

#include "eidviewerApp.h"

#include "eidlib.h"
#include "../beidcommon/TLVBuffer.h"

#include <opensc/opensc.h>
#include <opensc/pkcs15.h>

#ifdef __APPLE__
  #include <qapplication.h>
 #include <CoreFoundation/CFBundle.h> 
#endif

#ifndef _WIN32
    #include <winscard.h>
    #define SCARD_E_INVALID_CHV                     0x8010002A
#else
    #define SCARD_W_INSERTED_CARD           0x8010006A
#endif

// #define _TIMING_READ
    char *win9x_pcsc_stringify_error(long Error);

class MyFontEnumerator : public wxFontEnumerator
{
public:
    bool GotAny() const
        { return !m_facenames.IsEmpty(); }

    const wxArrayString& GetFacenames() const
        { return m_facenames; }

    bool IsInstalled(const wxString & strName) const
    {
        bool bRet = false;
        for ( unsigned int i = 0; i < m_facenames.GetCount(); i++ )
        {
            if (strName == m_facenames.Item(i))
            {
                bRet = true;
                break;
            }
        }
        return bRet;
    }

protected:
    virtual bool OnFacename(const wxString& facename)
    {
        m_facenames.Add(facename);
        return TRUE;
    }

    private:
        wxArrayString m_facenames;
};


char eidviewerApp::hexChars[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. eidviewerApp and
// not wxApp)
IMPLEMENT_APP(eidviewerApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

eidviewerApp::eidviewerApp()
{
    m_pFrame = NULL;
    m_pLocale = NULL;
    m_pFontData = NULL;
#ifdef __APPLE__
	static char app_path[300];

        CFURLRef pluginRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
        CFStringRef macPath = CFURLCopyFileSystemPath(pluginRef, 
                                               kCFURLPOSIXPathStyle);
        const char *pathPtr = CFStringGetCStringPtr(macPath, 
                                               CFStringGetSystemEncoding());
	strcpy(app_path, pathPtr);
        CFRelease(pluginRef);
        CFRelease(macPath);

        int i = 1;
        static char *s = app_path; // "/usr/local/lib/libbelpicgui.dylib";
        gMyApp = new QApplication(i, &s); 
#endif
}

eidviewerApp::~eidviewerApp()
{
    if(m_pFontData)
    {
        delete m_pFontData;
        m_pFontData = NULL;
    }
    if(m_pLocale)
    {
        delete m_pLocale;
        m_pLocale = NULL;
    }
}

// 'Main program' equivalent: the program execution "starts" here
bool eidviewerApp::OnInit()
{
    LoadConfiguration();
    const wxString strAppName = wxString::Format(wxT("beidgui-%s"), wxGetUserId().c_str());
    m_pChecker = new wxSingleInstanceChecker(strAppName);
    if ( m_pChecker->IsAnotherRunning() )
    {
        SwitchLanguage();
#ifdef _WIN32
        HWND hwnd = ::FindWindow(wxT("wxWindowClass"), _("Identity Card"));
        if (hwnd) 
        {
            ::SetForegroundWindow(hwnd);  // <-This works, but is non-portable.
        }
#endif
        return false;
    }

    wxImage::AddHandler( new wxJPEGHandler );

    m_pFontData = ParseFonts();

    // Create the main frame window
#ifdef __APPLE__
    m_pFrame = new eidviewerFrame(_("Identity Card"), 50, 50, 850, 600);
#else
    m_pFrame = new eidviewerFrame(_("Identity Card"), 50, 50, 850, 650);
#endif

#if defined(__WIN16__) || defined(__WXMOTIF__)
    int width, height;
    m_pFrame->GetSize(& width, & height);
    m_pFrame->SetSize(-1, -1, width, height);
#endif

    SwitchLanguage();
    m_pFrame->SetTitle(_("Identity Card"));
    m_pFrame->SetStatusText(_("Click on the chip to read the card"));
    m_pFrame->Show(TRUE);
    SetTopWindow(m_pFrame);
    wxSize oSize = m_pFrame->GetBestSize();
    oSize.SetHeight(oSize.GetHeight() + m_pFrame->GetStatusBar()->GetSize().GetHeight());
    oSize.SetHeight(oSize.GetHeight() + m_pFrame->GetToolBar()->GetSize().GetHeight());
    m_pFrame->SetSize(oSize);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned FALSE here, the
    // application would exit immediately.

    if (wxApp::argc > 1)
    {
         wxString strArg = wxApp::argv[1];    
         ReadFileCard(strArg);
    }
    return TRUE;
}

void eidviewerApp::LoadLocale()
{
    if(m_pLocale == NULL)
    {
        m_pLocale = new wxLocale();
    }
    wxString strCat;
#ifdef _WIN32
    int iPreset = 3;
    if(-1 == m_Config.m_dwLanguage)
    {
	    switch (GetUserDefaultLangID() & 0x00FF) 
        {
	    case 0x13: 
            iPreset =  0;
            break;
	    case 0x0C: 
            iPreset =  1;
            break;
	    case 0x07: 
            iPreset =  2;
            break;
	    default: 
            iPreset =  3;
            break;
	    }
        m_Config.m_dwLanguage = iPreset;
    }
#endif
    int iLanguage = wxLANGUAGE_ENGLISH;
    switch(m_Config.m_dwLanguage)
    {
    case 0: // Dutch
        iLanguage = wxLANGUAGE_DUTCH_BELGIAN;
        strCat = wxT("beidgui_nl");
        break;
    case 1: // French
        iLanguage = wxLANGUAGE_FRENCH_BELGIAN;
        strCat = wxT("beidgui_fr");
        break;
    case 2: // German
        iLanguage = wxLANGUAGE_GERMAN;
        strCat = wxT("beidgui_de");
        break;
    default:
        iLanguage = wxLANGUAGE_ENGLISH;
        break;
    }
    m_pLocale->Init(iLanguage);
#ifdef _WIN32
    wxString strValue;
    if(wxGetEnv(wxT("SYSTEMROOT"), &strValue))
    {
        m_pLocale->AddCatalogLookupPathPrefix(strValue);
    }
    if(wxGetEnv(wxT("WINDIR"), &strValue))
    {
        m_pLocale->AddCatalogLookupPathPrefix(strValue);
    }
#endif
    if(!strCat.IsEmpty())
    {
        m_pLocale->AddCatalog(strCat);
    }
}

int eidviewerApp::OnExit()
{
    try
    {
        SaveConfiguration();
        BEID_Exit();
        delete m_pChecker;
#ifdef __APPLE__
	   if(qApp)
        {
          qApp->wakeUpGuiThread();           
        }
#endif  
    }      
    catch(...)
    {
    }

    return 0;
}

long eidviewerApp::GetLanguage()
{
    return m_Config.m_dwLanguage; 
}

void eidviewerApp::LoadConfiguration()
{
    m_Config.Load();  
}

void eidviewerApp::SaveConfiguration()
{
    m_Config.Save();
}

char *eidviewerApp::Hexify(unsigned char * pData, unsigned long ulLen) 
{
    char *pszHex = new char[ulLen*2 + 1];
    memset(pszHex, 0, ulLen*2 + 1);
    if(pData != NULL)
    {
        int j = 0;
        for(unsigned long i = 0; i < ulLen; i++) 
        {
            pszHex[j++] = hexChars[pData[i]>>4 & 0x0F];
            pszHex[j++] = hexChars[pData[i] & 0x0F];
         }
    }
    return pszHex;
}

void eidviewerApp::SwitchLanguage ()
{
    LoadLocale();

    if(m_pFrame == NULL)
        return;

    m_pFrame->GetNotebook()->LoadStrings(m_Config.m_dwLanguage); 
    m_oErrMap.clear();
    m_oErrMap[BEID_E_BAD_PARAM] = _("Invalid parameter");
    m_oErrMap[BEID_E_INTERNAL] = _("An internal consistency check failed");
    m_oErrMap[BEID_E_INVALID_HANDLE] = _("Invalid handle");
    m_oErrMap[BEID_E_INSUFFICIENT_BUFFER] = _("The data buffer to receive returned data is too small for the returned data");
    m_oErrMap[BEID_E_COMM_ERROR] = _("An internal communications error has been detected");
    m_oErrMap[BEID_E_TIMEOUT] = _("A specified timeout value has expired");
    m_oErrMap[BEID_E_UNKNOWN_CARD] = _("Unknown card in reader");
    m_oErrMap[BEID_E_KEYPAD_CANCELLED] = _("Input cancelled");
    m_oErrMap[BEID_E_KEYPAD_TIMEOUT] = _("Timout returned from pinpad");
    m_oErrMap[BEID_E_KEYPAD_PIN_MISMATCH] = _("The two PINs did not match");
    m_oErrMap[BEID_E_KEYPAD_MSG_TOO_LONG] = _("Message too long on pinpad");
    m_oErrMap[BEID_E_INVALID_PIN_LENGTH] = _("Invalid PIN length");
    m_oErrMap[BEID_E_VERIFICATION] = _("Signature verification failed");
    m_oErrMap[BEID_E_NOT_INITIALIZED] = _("Library not initialized");
    m_oErrMap[BEID_E_UNKNOWN] = _("An internal error has been detected, but the source is unknown");
    m_oErrMap[BEID_E_UNSUPPORTED_FUNCTION] = _("Function is not supported");
    m_oErrMap[BEID_E_INCORRECT_VERSION] = _("Incorrect library version");
    m_oErrMap[BEID_E_INVALID_ROOT_CERT] = _("Wrong Root Certificate");    
    m_oErrMap[BEID_E_VALIDATION] = _("Certificate validation failed (OCSP/CRL)");    
}

void eidviewerApp::ClearPages()
{
    if(m_pFrame == NULL)
        return;

    m_pFrame->SetStatusText(wxT(""));
    m_pFrame->SetProgressValue(0);
    m_pFrame->GetNotebook()->ClearPages(); 
    m_pFrame->GetNotebook()->Refresh();
}

void eidviewerApp::ReadCard()
{
    ClearPages();
    BEID_Exit();

#ifdef _TIMING_READ
    clock_t         t1, t2;        
#endif

    long lHandle = 0;
    // Alround buffer
    BYTE buffer[BEID_MAX_PICTURE_LEN] = {0};
    BEID_Bytes tBytes = {0};
    tBytes.length = BEID_MAX_PICTURE_LEN;
    tBytes.data = buffer;

    BEID_Status tStatus = {0};
    BEID_ID_Data idData = {0};
    BEID_Address adData = {0};
    BEID_Certif_Check tCheck = {0};

    if(m_pFrame == NULL)
        return;

    wxBusyCursor wait;

    m_pFrame->SetStatusText(_("Connecting to card..."));

    try
    {
#if wxUSE_UNICODE
        tStatus = BEID_Init((char *)(m_Config.m_StrReader.mb_str().data()), 
            m_Config.m_lQuick < 0 ? m_Config.m_lQuick : m_Config.m_dwOCSP, m_Config.m_lQuick < 0 ? m_Config.m_lQuick : m_Config.m_dwCRL,
            &lHandle);
#else
        tStatus = BEID_Init((char *)(m_Config.m_StrReader.c_str()), 
            m_Config.m_lQuick < 0 ? m_Config.m_lQuick : m_Config.m_dwOCSP, m_Config.m_lQuick < 0 ? m_Config.m_lQuick : m_Config.m_dwCRL,
            &lHandle);
#endif
#ifdef _TIMING_READ
    t1 = clock();
#endif
        if(BEID_OK != tStatus.general)
            throw(&tStatus);
        m_pFrame->SetProgressValue(15);

        // Read ID Data
        m_pFrame->SetStatusText(_("Reading Identity..."));

        tStatus = BEID_GetID(&idData, &tCheck);
        if(BEID_OK != tStatus.general)
            throw(&tStatus);

        long lSignatureID = tCheck.signatureCheck;
        m_pFrame->GetNotebook()->SetIDData(&idData, tCheck.signatureCheck);
        if(tCheck.certificatesLength <= 1)
        {
            memset(&tCheck, 0, sizeof(BEID_Certif_Check));
            tStatus = BEID_GetCertificates(&tCheck);
            if(BEID_OK != tStatus.general)
                throw(&tStatus);
        }
        m_pFrame->GetNotebook()->SetCertificateData(&tCheck); 
        memset(&tCheck, 0, sizeof(BEID_Certif_Check));
        m_pFrame->SetProgressValue(30);

        // Read Address Data
        m_pFrame->SetStatusText(_("Reading Address..."));

        tStatus = BEID_GetAddress(&adData, &tCheck);
        if(BEID_OK != tStatus.general)
            throw(&tStatus);

        m_pFrame->GetNotebook()->SetAddressData(&adData, tCheck.signatureCheck);
        memset(&tCheck, 0, sizeof(BEID_Certif_Check));
        m_pFrame->SetProgressValue(45);

        // Read Picture Data
        if(BEID_SIGNATURE_VALID == lSignatureID || BEID_SIGNATURE_VALID_WRONG_RRNCERT == lSignatureID)
        {
            m_pFrame->SetStatusText(_("Reading Picture..."));

            tStatus = BEID_GetPicture(&tBytes, &tCheck);
            if(BEID_OK != tStatus.general)
                throw(&tStatus);

            if(BEID_SIGNATURE_VALID == tCheck.signatureCheck && BEID_SIGNATURE_VALID_WRONG_RRNCERT == lSignatureID)
            {
                tCheck.signatureCheck = lSignatureID;
            }
            m_pFrame->GetNotebook()->SetPictureData(&tBytes, tCheck.signatureCheck);
            m_pFrame->SetProgressValue(60);
        }
        else
        {
            BEID_Bytes tBytesClear = {0};
            m_pFrame->GetNotebook()->SetPictureData(&tBytesClear, BEID_SIGNATURE_INVALID, TRUE);
        }

        if(m_Config.m_lQuick >= 0)
        {
            // Read VersionInfo
            BEID_VersionInfo tVersionInfo = {0};
            BEID_Bytes tSignature = {0};
            BYTE bufferSig[256] = {0};
            tSignature.length = 256;
            tSignature.data = bufferSig;

            m_pFrame->SetStatusText(_("Reading VersionInfo..."));

            tStatus = BEID_GetVersionInfo(&tVersionInfo, FALSE, &tSignature);
            if(BEID_OK != tStatus.general)
                throw(&tStatus);

            m_pFrame->GetNotebook()->SetVersionInfoData(&tVersionInfo);
            m_pFrame->SetProgressValue(75);

            // Read PinData
            m_pFrame->SetStatusText(_("Reading PIN Data..."));

            std::map<wxString, std::vector<CPin> > PinMap;
            if( BEID_OK == ReadPINs(PinMap))
            {
                GetPINStatus(PinMap);
                m_pFrame->GetNotebook()->SetPINData(PinMap); 
            }
        }
        m_pFrame->SetProgressValue(0);
        m_pFrame->SetStatusText(_("Done"));
#ifdef _TIMING_READ
    t2 = clock();
    double elapsed = (double)(t2 - t1) / CLOCKS_PER_SEC;
    char buf[256] = {0};
    sprintf(buf, "elapsed = %lf", elapsed);
    MessageBoxA(NULL,buf,"Timer",MB_OK);
#endif        
        BEID_Exit();
    }
    catch(BEID_Status *pStatus)
    {
        FillStatusError(pStatus);
        m_pFrame->SetProgressValue(0);
        BEID_Exit();
    }

}

wxFont *eidviewerApp::ParseFonts()
{
    wxFont *pFont = NULL;

    MyFontEnumerator fontEnumerator;
    fontEnumerator.EnumerateFacenames();
    if ( fontEnumerator.GotAny() )
    {
        eidviewer::CConfig & oConfig = GetConfig();
        wxString strFonts = oConfig.m_strFonts;
        wxString strFontSel;
        int iIndex = 0;
        int iStart = 0;
        long lFontSize = oConfig.m_lFontSize; 
        bool bFound = false;
        while(-1 != (iIndex = strFonts.Find(',')))
        {
            strFontSel = strFonts.Mid(iStart, iIndex - iStart);
            strFontSel.Trim();
            if(fontEnumerator.IsInstalled(strFontSel))
            {
                bFound = true;
                break;
            }
            iStart = ++iIndex;
            strFonts = strFonts.Mid(iStart);
        }
        if(!bFound && !strFonts.IsEmpty())
        {
            strFontSel = strFonts;
            strFontSel.Trim(); 
            if(fontEnumerator.IsInstalled(strFontSel))
            {
                bFound = true;
            }
        }
        if(bFound)
        {
            pFont = new wxFont(lFontSize, wxDEFAULT, wxNORMAL, wxNORMAL, FALSE, strFontSel);
        } 
        else
        {
            wxFont wxFontTemp = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
            pFont = new wxFont(lFontSize, wxDEFAULT, wxNORMAL, wxNORMAL, FALSE, wxFontTemp.GetFaceName());
        }
    }

    return pFont;
}

void eidviewerApp::FillStatusError(BEID_Status *pStatus)
{
    wxString strTemp;
    switch (pStatus->general)
    {
    case BEID_OK:
    break;
    case BEID_E_SYSTEM:
    case BEID_E_PCSC:
        {
#ifndef _WIN32
            if(pStatus->general == BEID_E_PCSC)
            {
                strTemp = wxString(pcsc_stringify_error(pStatus->pcsc), wxConvUTF8);
            }
            else
            {
#endif
#ifdef _WIN32
            int iMajor = 0;
            int iMinor = 0;
            ::wxGetOsVersion(&iMajor, &iMinor);
            if (iMajor == 4)
            {
#if wxUSE_UNICODE
                strTemp = wxString(win9x_pcsc_stringify_error(pStatus->pcsc), *wxConvCurrent);
#else
                strTemp = win9x_pcsc_stringify_error(pStatus->pcsc);
#endif
            }   
#endif            
            if(strTemp.length() == 0)
            {            
                strTemp = wxSysErrorMsg(pStatus->general == BEID_E_PCSC ? pStatus->pcsc : pStatus->system);
            }
#ifndef _WIN32
            }
#endif
            wxString strPrefix(_("System Error : "));
            if(!strTemp.IsEmpty())
            {
                strTemp = strPrefix + strTemp;
            }
            else
            {
                strTemp = strPrefix + _("Unknown errorcode");
            }
        }
        break;
    case BEID_E_CARD:
        {
            strTemp = wxString::Format(_("Card error : SW=%02X%02X"), pStatus->cardSW[0], pStatus->cardSW[1]); 
        }
        break;
    default: // BEID_ERR
        strTemp = _("Error : ");
        strTemp += m_oErrMap[pStatus->general];
        break;
    }
    m_pFrame->SetStatusText(strTemp);
    if(BEID_E_KEYPAD_CANCELLED != pStatus->general)
    {
        wxMessageDialog oDialog(m_pFrame, strTemp, m_pFrame->GetTitle(), wxOK | wxICON_ERROR);
        oDialog.ShowModal();
    }
}

long eidviewerApp::ReadPINs(std::map<wxString, std::vector<CPin> > & PinMap)
{
    long lRet = BEID_OK;
    BEID_Pins tPINs = {0};
    BEID_Status tStatus = {0};

    tStatus = BEID_GetPINs(&tPINs);
    if(tStatus.general == BEID_OK && tPINs.pinsLength > 0)
    {
        int iNumber = tPINs.pinsLength;
        std::vector<CPin> VecPin;

        wxString strApp("BELPIC", *wxConvCurrent);
        for (int i = 0; i < iNumber; i++) 
        {
            CPin oPin;
            oPin.SetReference(tPINs.pins[i].id);
            oPin.SetTriesLeft(tPINs.pins[i].triesLeft);
            oPin.SetFlags(tPINs.pins[i].flags);
            oPin.SetLabel(wxString(tPINs.pins[i].label, *wxConvCurrent));
            oPin.SetType(tPINs.pins[i].pinType);
            oPin.SetUsageCode(tPINs.pins[i].usageCode); 
            oPin.SetApplication(strApp);
            VecPin.push_back(oPin);
        }
        if(VecPin.size() > 0)
        {
            PinMap[strApp] = VecPin;
            VecPin.clear();
        }
    }
    return lRet;
}

void eidviewerApp::GetPINStatus(std::map<wxString, std::vector<CPin> > & PinMap)
{
    // Get Pin Status
    BEID_Status tStatus = {0};

    std::map<wxString, std::vector<CPin> >::iterator it;
    for (it = PinMap.begin(); it != PinMap.end(); ++it)
    {
        for(unsigned int i = 0; i < (*it).second.size(); ++i)
        {
            CPin & oPin = (*it).second[i];
            BEID_Bytes tSignature = {0};
            BYTE bufferSig[256] = {0};
            tSignature.length = 256;
            tSignature.data = bufferSig;
            BEID_Pin tPin = {0};
            tPin.id = oPin.GetReference();
            tPin.pinType = oPin.GetType();
            tPin.usageCode = oPin.GetUsageCode(); 
            long lLeft = 0;
            tStatus = BEID_GetPINStatus(&tPin, &lLeft, FALSE, &tSignature);
            if(BEID_OK == tStatus.general)
            {
                oPin.SetTriesLeft(lLeft);
            }
            else
            {
                oPin.SetTriesLeft(-1);
            }
        }
    }
}

bool eidviewerApp::ChangePin(CPin *pPin)
{
    bool bRet = true;
     m_pFrame->SetStatusText(_("Changing PIN..."));

    long lHandle = 0;
    BEID_Status tStatus = {0};

    BEID_Pin tPin = {0};
    tPin.id = pPin->GetReference();
    tPin.pinType = pPin->GetType();
    tPin.usageCode = pPin->GetUsageCode(); 
    long lLeft = 0;

    wxBusyCursor wait;

    try
    {
#if wxUSE_UNICODE
        tStatus = BEID_Init((char *)(m_Config.m_StrReader.mb_str().data()), -1, -1, &lHandle);
#else
        tStatus = BEID_Init((char *)(m_Config.m_StrReader.c_str()), -1, -1, &lHandle);
#endif
        if(BEID_OK != tStatus.general)
            throw(&tStatus);

        tStatus = BEID_ChangePIN(&tPin, NULL, NULL, &lLeft);
        if(BEID_OK != tStatus.general)
        {
            if(BEID_E_PCSC == tStatus.general && (long)SCARD_E_INVALID_CHV == tStatus.pcsc)
            {
                pPin->SetTriesLeft(lLeft);
            }
            else
            {
                pPin->SetTriesLeft(-1);
            }
            FillStatusError(&tStatus);
            if(BEID_E_PCSC == tStatus.general && (long)SCARD_E_INVALID_CHV != tStatus.pcsc)
            {
                ClearPages();
                bRet = false;
            }
        }
        else
        {
            BEID_Bytes tSendBytes = {(unsigned char *)"\x80\xE6\x00\x00", 4}; 
            BEID_Bytes tRespBytes = {0};
            BYTE respbuffer[2] = {0};
            tRespBytes.data = respbuffer;
            tRespBytes.length = sizeof(respbuffer);
            tStatus = BEID_SendAPDU(&tSendBytes, &tPin, &tRespBytes);

             pPin->SetTriesLeft(-1);
             wxString strMsg(_("PIN successfully changed"));
             m_pFrame->SetStatusText(strMsg);
             wxMessageDialog oDialog(m_pFrame, strMsg, m_pFrame->GetTitle(), wxOK | wxICON_INFORMATION);
             oDialog.ShowModal();        
        }
    }
    catch(BEID_Status *pStatus)
    {
        ClearPages();
        FillStatusError(pStatus);
        BEID_Exit();
        return false;
    }
    BEID_Exit();
    return bRet;
}

void eidviewerApp::SetStatusBarText(wxString strMsg)
{
    if(m_pFrame)
    {
        m_pFrame->SetStatusText(strMsg);
    }
}

void eidviewerApp::ReadRawCard()
{
    BEID_Exit();
    long lHandle = 0;

    BEID_Status tStatus = {0};

    if(m_pFrame == NULL)
        return;

    m_pFrame->GetToolBar()->SetFocus(); 
    wxBusyCursor wait;

    m_pFrame->SetStatusText(_("Connecting to card..."));

    try
    {
#if wxUSE_UNICODE
        tStatus = BEID_Init((char *)(m_Config.m_StrReader.mb_str().data()), -1, -1, &lHandle);
#else
        tStatus = BEID_Init((char *)(m_Config.m_StrReader.c_str()), -1, -1, &lHandle);
#endif
        
        if(BEID_OK != tStatus.general)
            throw(&tStatus);

        m_pFrame->SetProgressValue(25);

        // Fill filename
        BEID_ID_Data idData = {0};
        BEID_Certif_Check tCheck = {0};

        m_pFrame->SetStatusText(_("Reading..."));
           
        tStatus = BEID_GetID(&idData, &tCheck);
        if(BEID_OK != tStatus.general)
            throw(&tStatus);
        m_pFrame->SetProgressValue(50);

        wxString strFileName(idData.nationalNumber, wxConvUTF8);
        strFileName += wxT(".eid");
        wxFileDialog oDialog(m_pFrame, _("Save an eID file"), wxT(""), strFileName, wxT("eID files (*.eid)|*.eid"), wxSAVE | wxHIDE_READONLY | wxOVERWRITE_PROMPT);
        if (oDialog.ShowModal() != wxID_OK)
        {
            m_pFrame->SetStatusText(wxT(""));
            m_pFrame->SetProgressValue(0);
            BEID_Exit();
            return;
        }
        strFileName = oDialog.GetPath();

        // Read Raw Data
        m_pFrame->SetStatusText(_("Storing Data..."));

        BEID_Bytes tBytes = {0};
        tBytes.length = 8192;
        tBytes.data = new BYTE[tBytes.length];
        memset(tBytes.data, 0, tBytes.length);
        tStatus = BEID_GetRawFile(&tBytes);
        if(BEID_OK != tStatus.general)
            throw(&tStatus);
        m_pFrame->SetProgressValue(75);

        bool bOK = false;
        wxFile oEIDFile;
        if(oEIDFile.Open(strFileName, wxFile::write))
        {
            unsigned int iWritten = oEIDFile.Write(tBytes.data, tBytes.length);
            oEIDFile.Close();
            if(iWritten != tBytes.length)
            {
                m_pFrame->SetStatusText(_("Error saving eID file"));
            }
            else
            {
                bOK = true;
            }
        }

        delete [] tBytes.data;
        if(bOK)
        {
            m_pFrame->SetStatusText(_("Done"));
        }
        m_pFrame->SetProgressValue(100);

        BEID_Exit();
    }
    catch(BEID_Status *pStatus)
    {
        FillStatusError(pStatus);
        m_pFrame->SetProgressValue(0);
        BEID_Exit();
    }
}

void eidviewerApp::ReadFileCard(wxString & strFile)
{
    BYTE ucBuffer[1024] = {0};
    int iRead = -1;
    eidcommon::CByteArray oTempArray;

    if(m_pFrame == NULL)
        return;

    m_pFrame->SetStatusText(_("Opening eID file..."));

    if(strFile.IsEmpty())
    {
        wxFileDialog oDialog(m_pFrame, _("Open an eID file"), wxT(""), wxT(""), wxT("eID files (*.eid)|*.eid"), wxOPEN | wxHIDE_READONLY);
        if (oDialog.ShowModal() != wxID_OK)
        {
            m_pFrame->SetStatusText(wxT(""));
            return;
        }
        strFile = oDialog.GetPath();
    }

    if (strFile.IsEmpty() || !wxFileExists(strFile))
    {
        m_pFrame->SetStatusText( _("File doesn't exist") );
        return;
    }
    wxFile oEIDFile;
    if(oEIDFile.Open(strFile))
    {
        while(!oEIDFile.Eof())
        {
            if(wxInvalidOffset != (iRead = oEIDFile.Read(ucBuffer, sizeof(ucBuffer))))
            {
                oTempArray.Append(ucBuffer, iRead); 
            }
        }
        oEIDFile.Close();
    }
    
    if(oTempArray.GetSize()<= 0)
    {
        m_pFrame->SetStatusText( _("Invalid eID File") );
        return;
    }

    BEID_Exit();
    long lHandle = 0;

    // Alround buffer
    BYTE buffer[BEID_MAX_PICTURE_LEN] = {0};
    BEID_Bytes tBytes = {0};
    tBytes.length = BEID_MAX_PICTURE_LEN;
    tBytes.data = buffer;
    BEID_Status tStatus = {0};
    BEID_ID_Data idData = {0};
    BEID_Address adData = {0};
    BEID_Certif_Check tCheck = {0};
    BEID_Bytes rawFile = {0};
    rawFile.data = oTempArray.GetData(); 
    rawFile.length = oTempArray.GetSize();
    
    try
    {
#if wxUSE_UNICODE
        tStatus = BEID_Init("VIRTUAL", -1, -1, &lHandle);
#else
        tStatus = BEID_Init("VIRTUAL", -1, -1, &lHandle);
#endif
        
        if(BEID_OK != tStatus.general)
            throw(&tStatus);

        // Read Raw Data
        tStatus = BEID_SetRawFile(&rawFile);
        if(BEID_OK != tStatus.general)
            throw(&tStatus);

        // Read ID Data
        m_pFrame->SetStatusText(_("Reading Identity..."));

        tStatus = BEID_GetID(&idData, &tCheck);
        if(BEID_OK != tStatus.general)
            throw(&tStatus);

        ClearPages();

        m_pFrame->GetNotebook()->SetIDData(&idData, tCheck.signatureCheck);
        m_pFrame->GetNotebook()->SetCertificateData(&tCheck); 
        memset(&tCheck, 0, sizeof(BEID_Certif_Check));

        // Read Address Data
        m_pFrame->SetStatusText(_("Reading Address..."));

        tStatus = BEID_GetAddress(&adData, &tCheck);
        if(BEID_OK != tStatus.general)
            throw(&tStatus);

        m_pFrame->GetNotebook()->SetAddressData(&adData, tCheck.signatureCheck);
        memset(&tCheck, 0, sizeof(BEID_Certif_Check));
        
        // Read Picture Data
        m_pFrame->SetStatusText(_("Reading Picture..."));

        tStatus = BEID_GetPicture(&tBytes, &tCheck);
        if(BEID_OK != tStatus.general)
            throw(&tStatus);

        m_pFrame->GetNotebook()->SetPictureData(&tBytes, tCheck.signatureCheck);

        // Read VersionInfo
        BEID_VersionInfo tVersionInfo = {0};
        BEID_Bytes tSignature = {0};
        BYTE bufferSig[256] = {0};
        tSignature.length = 256;
        tSignature.data = bufferSig;

        m_pFrame->SetStatusText(_("Reading VersionInfo..."));

        tStatus = BEID_GetVersionInfo(&tVersionInfo, FALSE, &tSignature);
        if(BEID_OK != tStatus.general)
            throw(&tStatus);

        m_pFrame->GetNotebook()->SetVersionInfoData(&tVersionInfo);

        m_pFrame->SetStatusText(_("Done"));
        BEID_Exit();
    }
    catch(BEID_Status *pStatus)
    {
        FillStatusError(pStatus);
        BEID_Exit();
    }
}

char *win9x_pcsc_stringify_error(long Error)
{
    static char strError[75] = {0};

	switch (Error)
	{
	case SCARD_S_SUCCESS:
		strcpy(strError, "Command successful.");
		break;
	case SCARD_E_CANCELLED:
		strcpy(strError, "Command cancelled.");
		break;
	case SCARD_E_CANT_DISPOSE:
		strcpy(strError, "Cannot dispose handle.");
		break;
	case SCARD_E_INSUFFICIENT_BUFFER:
		strcpy(strError, "Insufficient buffer.");
		break;
	case SCARD_E_INVALID_ATR:
		strcpy(strError, "Invalid ATR.");
		break;
	case SCARD_E_INVALID_HANDLE:
		strcpy(strError, "Invalid handle.");
		break;
	case SCARD_E_INVALID_PARAMETER:
		strcpy(strError, "Invalid parameter given.");
		break;
	case SCARD_E_INVALID_TARGET:
		strcpy(strError, "Invalid target given.");
		break;
	case SCARD_E_INVALID_VALUE:
		strcpy(strError, "Invalid value given.");
		break;
	case SCARD_E_NO_MEMORY:
		strcpy(strError, "Not enough memory.");
		break;
	case SCARD_F_COMM_ERROR:
		strcpy(strError, "RPC transport error.");
		break;
	case SCARD_F_INTERNAL_ERROR:
		strcpy(strError, "Unknown internal error.");
		break;
	case SCARD_F_UNKNOWN_ERROR:
		strcpy(strError, "Unknown internal error.");
		break;
	case SCARD_F_WAITED_TOO_LONG:
		strcpy(strError, "Waited too long.");
		break;
	case SCARD_E_UNKNOWN_READER:
		strcpy(strError, "Unknown reader specified.");
		break;
	case SCARD_E_TIMEOUT:
		strcpy(strError, "Command timeout.");
		break;
	case SCARD_E_SHARING_VIOLATION:
		strcpy(strError, "Sharing violation.");
		break;
	case SCARD_E_NO_SMARTCARD:
		strcpy(strError, "No smartcard inserted.");
		break;
	case SCARD_E_UNKNOWN_CARD:
		strcpy(strError, "Unknown card.");
		break;
	case SCARD_E_PROTO_MISMATCH:
		strcpy(strError, "Card protocol mismatch.");
		break;
	case SCARD_E_NOT_READY:
		strcpy(strError, "Subsystem not ready.");
		break;
	case SCARD_E_SYSTEM_CANCELLED:
		strcpy(strError, "System cancelled.");
		break;
	case SCARD_E_NOT_TRANSACTED:
		strcpy(strError, "Transaction failed.");
		break;
	case SCARD_E_READER_UNAVAILABLE:
		strcpy(strError, "Reader/s is unavailable.");
		break;
	case SCARD_W_UNSUPPORTED_CARD:
		strcpy(strError, "Card is not supported.");
		break;
	case SCARD_W_UNRESPONSIVE_CARD:
		strcpy(strError, "Card is unresponsive.");
		break;
	case SCARD_W_UNPOWERED_CARD:
		strcpy(strError, "Card is unpowered.");
		break;
	case SCARD_W_RESET_CARD:
		strcpy(strError, "Card was reset.");
		break;
	case SCARD_W_REMOVED_CARD:
		strcpy(strError, "Card was removed.");
		break;
	case SCARD_W_INSERTED_CARD:
		strcpy(strError, "Card was inserted.");
		break;
	case SCARD_E_UNSUPPORTED_FEATURE:
		strcpy(strError, "Feature not supported.");
		break;
	case SCARD_E_PCI_TOO_SMALL:
		strcpy(strError, "PCI struct too small.");
		break;
	case SCARD_E_READER_UNSUPPORTED:
		strcpy(strError, "Reader is unsupported.");
		break;
	case SCARD_E_DUPLICATE_READER:
		strcpy(strError, "Reader already exists.");
		break;
	case SCARD_E_CARD_UNSUPPORTED:
		strcpy(strError, "Card is unsupported.");
		break;
	case SCARD_E_NO_SERVICE:
		strcpy(strError, "Service not available.");
		break;
	case SCARD_E_SERVICE_STOPPED:
		strcpy(strError, "Service was stopped.");
		break;
	case SCARD_E_INVALID_CHV:
		strcpy(strError, "The supplied PIN is incorrect.");
		break;
    default:
		strcpy(strError, "An internal error has been detected, but the source is unknown.");
		break;
	};

	return strError;
}
