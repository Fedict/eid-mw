// Config.cpp: implementation of the CConfig class.
//
//////////////////////////////////////////////////////////////////////

#include "Config.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <wx/config.h>
#include <wx/fileconf.h>
#include <wx/utils.h>

#ifdef __WXMAC__
#include <wx/mac/carbon/private.h>
#endif

/* Config file for storing user settings (e.g. language) */
#define BEID_EIDVIEWER_CONF_FILE      ".beidgui_ui.conf"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace eidviewer
{

CConfig::CConfig()
{
    m_dwLanguage = -1;
    m_StrReader = wxT("");
    m_dwCRL = 0;
    m_dwOCSP = 0;
    m_lFontSize = 10;
    m_lQuick = 0;
#ifdef _WIN32
    m_strFonts = wxT("Arial Unicode MS");
#else
    m_strFonts = wxT("");
#endif
}

CConfig::~CConfig()
{

}

/* Fixme:
 * Despite the name, this function returns the user home/config dir.
 * On Windows, this function is not used (and shouldn't because it returns the Windows dir!).
 */
wxString CConfig::GetGlobalDir()
{
  wxString strDir;

#ifdef __VMS__ // Note if __VMS is defined __UNIX is also defined
    strDir = wxT("sys$manager:");
#elif defined(__WXMAC__)
    strDir = wxMacFindFolder(  (short) kOnSystemDisk, kPreferencesFolderType, kDontCreateFolder ) ;
#elif defined( __UNIX__ )
    strDir = wxGetHomeDir();
    strDir += wxT("/");
#elif defined(__WXPM__)
    ULONG aulSysInfo[QSV_MAX] = {0};
    UINT drive;
    APIRET rc;

    rc = DosQuerySysInfo( 1L, QSV_MAX, (PVOID)aulSysInfo, sizeof(ULONG)*QSV_MAX);
    if (rc == 0)
    {
        drive = aulSysInfo[QSV_BOOT_DRIVE - 1];
        strDir.Printf(wxT("%c:\\OS2\\"), 'A'+drive-1);
    }
#elif defined(__WXSTUBS__)
    wxASSERT_MSG( FALSE, wxT("TODO") ) ;
#elif defined(__DOS__)
    // There's no such thing as global cfg dir in MS-DOS, let's return
    // current directory (FIXME_MGL?)
    return wxT(".\\");
#else // Windows
    wxChar szWinDir[MAX_PATH];
    ::GetWindowsDirectory(szWinDir, MAX_PATH);

    strDir = szWinDir;
    strDir << wxT('\\');
#endif // Unix/Windows

    return strDir;
}

void CConfig::Load()
{
#ifdef _WIN32
    wxConfig *pConfig = new wxConfig(wxT("Fedict\\beidgui"));
#else
    wxString strConfPath = GetGlobalDir() + wxT(BEID_EIDVIEWER_CONF_FILE);
    wxFileConfig *pConfig = new wxFileConfig(wxT(""), wxT(""), strConfPath, wxT(""), wxCONFIG_USE_GLOBAL_FILE);
#endif

    if(pConfig != NULL)
    {
        wxString str;
        long dwTemp = 0;
        if ( pConfig->Read(wxT("SCReader"), &str) ) 
        {
            m_StrReader = str;
        }
        if ( pConfig->Read(wxT("Language"), &dwTemp) ) 
        {
            m_dwLanguage = dwTemp;
        }
        if ( pConfig->Read(wxT("OCSP"), &dwTemp) ) 
        {
            m_dwOCSP = dwTemp;
        }
        if ( pConfig->Read(wxT("CRL"), &dwTemp) ) 
        {
            m_dwCRL = dwTemp;
        }
        if ( pConfig->Read(wxT("Fonts"), &str) ) 
        {
            m_strFonts = str;
        }
        if ( pConfig->Read(wxT("Fontsize"), &dwTemp) ) 
        {
            m_lFontSize = dwTemp;
        }
        if ( pConfig->Read(wxT("Quick"), &dwTemp) ) 
        {
            m_lQuick = -dwTemp;
        }
       // the changes will be written back automatically
       delete pConfig;
    }
}

void CConfig::Save()
{
#ifdef _WIN32
   wxConfig *pConfig = new wxConfig(wxT("Fedict\\beidgui"));
#else
    wxString strConfPath = GetGlobalDir() + wxT(BEID_EIDVIEWER_CONF_FILE);
    wxFileConfig *pConfig = new wxFileConfig(wxT(""), wxT(""), strConfPath, wxT(""), wxCONFIG_USE_GLOBAL_FILE);
#endif

    if(pConfig != NULL)
    {
        pConfig->Write(wxT("SCReader"), m_StrReader); 
        pConfig->Write(wxT("Language"), m_dwLanguage); 
        pConfig->Write(wxT("OCSP"), m_dwOCSP); 
        pConfig->Write(wxT("CRL"), m_dwCRL); 
        pConfig->Write(wxT("Fonts"), m_strFonts); 
        pConfig->Write(wxT("Fontsize"), m_lFontSize); 
       // the changes will be written back automatically
       delete pConfig;
    }
}

}
