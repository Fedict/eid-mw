// Config.h: interface for the CConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIG_H__75C0FCFC_412F_4644_B1AC_F05F1D288CEF__INCLUDED_)
#define AFX_CONFIG_H__75C0FCFC_412F_4644_B1AC_F05F1D288CEF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wx/string.h>

namespace eidviewer
{

class CConfig  
{
public:
	CConfig();
	virtual ~CConfig();

    void Load();
    void Save();
    wxString GetGlobalDir();

    long m_dwLanguage;
    wxString m_StrReader;
    long m_dwCRL;
    long m_dwOCSP;
    wxString m_strFonts;
    long m_lFontSize;
    long m_lQuick;
};

}
#endif // !defined(AFX_CONFIG_H__75C0FCFC_412F_4644_B1AC_F05F1D288CEF__INCLUDED_)
