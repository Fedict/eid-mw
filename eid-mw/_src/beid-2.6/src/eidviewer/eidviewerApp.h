/////////////////////////////////////////////////////////////////////////////
// Name:        eidviewerApp.h
/////////////////////////////////////////////////////////////////////////////

#ifndef eidviewerApp_H
#define eidviewerApp_H

#ifdef _WIN32
    #pragma warning(disable : 4786 4514 4702)
#endif

#include "Config.h"
#include "eidviewerFrame.h"
#include "Pin.h"

#include <map>
#include <vector>

#ifdef __APPLE__
  #include <qapplication.h>
#endif

class wxSingleInstanceChecker;

// Define a new application type, each program should derive a class from wxApp
class eidviewerApp : public wxApp
{
public:
    eidviewerApp();
    virtual ~eidviewerApp();

    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
    virtual int OnExit();

    void LoadConfiguration();
    void SaveConfiguration();
    eidviewer::CConfig & GetConfig() { return m_Config; }
    char *Hexify(unsigned char * pData, unsigned long ulLen);
    void ReadCard();
    long GetLanguage();
    void SwitchLanguage();
    void ClearPages();
    wxFont *GetDataFont() { return m_pFontData; }
    bool ChangePin(CPin *pPin);
    void SetStatusBarText(wxString strMsg);
    void ReadRawCard();
    void ReadFileCard(wxString & strFile);

protected:
    void  LoadLocale();
    wxFont * ParseFonts();
    void FillStatusError(BEID_Status *pStatus);
    long ReadPINs(std::map<wxString, std::vector<CPin> > & PinMap);
    void GetPINStatus(std::map<wxString, std::vector<CPin> > & PinMap);

  /*  
    void ClearPages();
    CConfig & GetConfig() { return m_Config; }
    CString GetDataFont() { return m_FontData; }
    char *Hexify(unsigned char * pData, unsigned long ulLen);
    bool ChangePin(CPin *pPin);
    void SwitchLanguage();
    CString DecodeFromUTF8(char *pszUTF8);
    CString MakeTempFile();
    void SetStatusBarText(unsigned int uiMsg);
    void ClearStatusBarText();
*/

private:
    eidviewerFrame *m_pFrame;
    static char hexChars[];
    eidviewer::CConfig m_Config;
    wxLocale *m_pLocale;
    wxFont *m_pFontData;
    std::map<int, wxString> m_oErrMap;
    wxSingleInstanceChecker *m_pChecker;
#ifdef __APPLE__
    QApplication *gMyApp;
#endif
};

DECLARE_APP(eidviewerApp)

#endif // eidviewerApp_H


