// PrintOut.h: interface for the CPrintOut class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRINTOUT_H__74140E5E_C258_404E_B505_20582F2FB3A9__INCLUDED_)
#define AFX_PRINTOUT_H__74140E5E_C258_404E_B505_20582F2FB3A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _WIN32
    #pragma warning(disable : 4786)
#endif

#include <wx/print.h>

class CPrintOut : public wxPrintout 
{
public:
	CPrintOut(wxString strTitle, CPrintOutData *pData);
	virtual ~CPrintOut();
    bool OnPrintPage(int page);
    bool HasPage(int page);
    bool OnBeginDocument(int startPage, int endPage);
    void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo);
    void DrawPage(wxDC *dc);

private:
    CPrintOutData *m_pData;
};

#endif // !defined(AFX_PRINTOUT_H__74140E5E_C258_404E_B505_20582F2FB3A9__INCLUDED_)
