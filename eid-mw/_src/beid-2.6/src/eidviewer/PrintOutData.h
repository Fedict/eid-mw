// PrintOutData.h: interface for the CPrintOutData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRINTOUTDATA_H__432F3974_CD7E_430E_85C5_3E945C6DA3F1__INCLUDED_)
#define AFX_PRINTOUTDATA_H__432F3974_CD7E_430E_85C5_3E945C6DA3F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wx/string.h>
#include <wx/bitmap.h>

class CPrintOutData  
{
public:
	CPrintOutData();
	virtual ~CPrintOutData();

    wxString m_strLangCountry;
    wxString m_strLangCard;
    wxBitmap m_Logo;
    wxBitmap m_Picture;
    wxString m_strCardNrLabel;
    wxString m_strCardNr;
    wxString m_strChipNrLabel;
    wxString m_strChipNr;
    wxString m_strValidFromLabel;
    wxString m_strValidFrom;
    wxString m_strValidUntilLabel;
    wxString m_strValidUntil;
    wxString m_strNatNrLabel;
    wxString m_strNatNr;
    wxString m_strTitleLabel;
    wxString m_strTitle;
    wxString m_strNameLabel;
    wxString m_strName;
    wxString m_strGivenNameLabel;
    wxString m_strGivenName;
    wxString m_strNationalLabel;
    wxString m_strNational;
    wxString m_strSexLabel;
    wxString m_strSex;
    wxString m_strBirthDateLabel;
    wxString m_strBirthDate;
    wxString m_strBirthPlaceLabel;
    wxString m_strBirthPlace;
    wxString m_strAddressLabel;
    wxString m_strStreet;
    wxString m_strZip;
    wxString m_strMunicip;
    wxString m_strIssMunicLabel;
    wxString m_strIssMunic;
};

#endif // !defined(AFX_PRINTOUTDATA_H__432F3974_CD7E_430E_85C5_3E945C6DA3F1__INCLUDED_)
