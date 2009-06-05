// eidviewerFrame.h: interface for the eidviewerFrame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EIDVIEWERFRAME_H__2A8D044D_8B95_4B3C_97A8_4B1F986BD230__INCLUDED_)
#define AFX_EIDVIEWERFRAME_H__2A8D044D_8B95_4B3C_97A8_4B1F986BD230__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _WIN32
    #pragma warning(disable : 4786 4514 4710)
#endif

#include "eidviewerNotebook.h"

class eidviewerFrame : public wxFrame  
{
public:
	eidviewerFrame(const wxChar *title, int x, int y, int width, int height);
	virtual ~eidviewerFrame();
    eidviewerNotebook *GetNotebook() { return m_notebook; }

    void OnQuit(wxCommandEvent& event);
    void OnButtonChip(wxCommandEvent& event);
    void OnButtonArrow(wxCommandEvent& event);
    bool GetReading() { return m_bReading; }
    void OnButtonPrint(wxCommandEvent& event);
    void OnButtonOpenFile(wxCommandEvent& event);
    void OnButtonSaveFile(wxCommandEvent& event);
    virtual void DoGiveHelp(const wxString& text, bool show);
    virtual void SetStatusText(const wxString &text, int number = 0, bool bSafeYield = false);
    virtual void SetProgressValue(int iValue);
    void OnActivate (wxActivateEvent&);
private:
    eidviewerNotebook *m_notebook;
    // Sizer for m_notebook
    wxNotebookSizer *m_sizerNotebook;
    wxPanel *m_IDPanel;
    bool m_bReading;

    wxToolBar *m_tbar;
    wxGauge *m_statusGauge;

    DECLARE_EVENT_TABLE()
};


#endif // !defined(AFX_EIDVIEWERFRAME_H__2A8D044D_8B95_4B3C_97A8_4B1F986BD230__INCLUDED_)
