// eidviewerFrame.cpp: implementation of the eidviewerFrame class.
//
//////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif

#include "eidviewerApp.h"
#include "eidviewerFrame.h"
#include "eidviewerTypes.h"

// the application icon
#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) 
//    #include "res/eidviewer.xpm"
static const char *eidviewer_xpm[] = {
"32 32 5 1",
"  c None",
"a c Black",
"b c #FF0000",
"c c #FFFF00",
"d c #800000",
/* pixels */
"                d               ",
"               ddddd            ",
"     dd       ddddddd           ",
"   dddddddddddddddddddddd       ",
"  dddddddddddddddddddddddd      ",
"ddddddddddddddddddddddddddd     ",
"ddddddddddddddddddddddddddd     ",
"dddddddddddddddddddddddddd      ",
"ddddddddddddddddacccacdddd      ",
" ddddddddddddddcacccaaadddd     ",
"  dd ddddddddddccaccaccddddd    ",
"     ddddddddddcaaaaaacddddddd  ",
"      dddddddddacaccacadddddd   ",
"        ddddddddcaccacddddddddd ",
"         dddddddddddddddddddddd ",
"            ddddddddddddddddddd ",
"             ddddddddddddddddd  ",
"             dddddddddddddddd   ",
"             ddddddddddddddd    ",
"             dddd dddddddddd    ",
"             ddd  dddddddddd    ",
"                  dddddddddd    ",
"                  ddddddddddd   ",
"                   dddddddddd   ",
"aaaaccccbbbb           dddddd   ",
"aaaaccccbbbb           ddddd    ",
"aaaaccccbbbb            ddd     ",
"aaaaccccbbbb                    ",
"aaaaccccbbbb                    ",
"aaaaccccbbbb                    ",
"aaaaccccbbbb                    ",
"aaaaccccbbbb                    "};

#endif


class StatusbarHandler : public wxEvtHandler
{ 
    public:
        StatusbarHandler(wxStatusBar *bar, wxGauge *gauge) :
                 m_bar(bar), m_gauge(gauge) {}

    private:
        void OnSize(wxSizeEvent& event)
        {
            wxRect rect;
            m_bar->GetFieldRect(1, rect);
            m_gauge->SetSize(rect.x+2, rect.y+2, rect.width-4, rect.height-4);
            event.Skip();
        }

        DECLARE_EVENT_TABLE() 

        wxStatusBar *m_bar;
        wxGauge     *m_gauge;
};

BEGIN_EVENT_TABLE(StatusbarHandler, wxEvtHandler)
   EVT_SIZE(StatusbarHandler::OnSize)
END_EVENT_TABLE()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(eidviewerFrame, wxFrame)
    EVT_BUTTON(IDC_BUTTON_CHIP, eidviewerFrame::OnButtonChip)
    EVT_BUTTON(IDC_BUTTON_BITMAP_ARROW, eidviewerFrame::OnButtonArrow)
    EVT_TOOL(IDC_BUTTON_IDENTITY_PRINT, eidviewerFrame::OnButtonPrint)
    EVT_TOOL(IDC_BUTTON_IDENTITY_OPEN, eidviewerFrame::OnButtonOpenFile)
    EVT_TOOL(IDC_BUTTON_IDENTITY_SAVE, eidviewerFrame::OnButtonSaveFile)
    EVT_TOOL(IDC_BUTTON_CHIP, eidviewerFrame::OnButtonChip)
    EVT_TOOL(IDC_BUTTON_BITMAP_ARROW, eidviewerFrame::OnButtonArrow)
    EVT_TOOL(IDC_BUTTON_IDENTITY_EXIT, eidviewerFrame::OnQuit)
    EVT_ACTIVATE(eidviewerFrame::OnActivate) 
END_EVENT_TABLE()

eidviewerFrame::eidviewerFrame(const wxChar *title, int x, int y, int width, int height)
       : wxFrame(NULL, -1, title, wxPoint(x, y), wxSize(width, height), wxDEFAULT_FRAME_STYLE)
{
     m_notebook = NULL;
     m_sizerNotebook = NULL;
     m_statusGauge = NULL;
     m_IDPanel = NULL;

     m_bReading = false;

     m_tbar = new wxToolBar(this, -1, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_HORIZONTAL | wxNO_BORDER);
     m_tbar->SetMargins( 4, 4 );
    
    m_tbar->AddTool( IDC_BUTTON_CHIP, eidBitmapsFunc( 33 ), _("Read") );
    m_tbar->AddTool( IDC_BUTTON_BITMAP_ARROW, eidBitmapsFunc( 34 ), _("Clear") );
    m_tbar->AddSeparator();
    m_tbar->AddTool( IDC_BUTTON_IDENTITY_OPEN, eidBitmapsFunc( 28 ), _("Open") );
    m_tbar->AddTool( IDC_BUTTON_IDENTITY_SAVE, eidBitmapsFunc( 27 ), _("Save") );
    m_tbar->AddSeparator();
#ifndef __APPLE__
    m_tbar->AddTool( IDC_BUTTON_IDENTITY_PRINT, eidBitmapsFunc( 29 ), _("Print") );
    m_tbar->AddSeparator();
#endif
    m_tbar->AddTool( IDC_BUTTON_IDENTITY_EXIT, eidBitmapsFunc( 35 ), _("Exit") );
    
    m_tbar->Realize();
    SetToolBar(m_tbar);
    
    m_notebook = new eidviewerNotebook(this, -1);
    m_sizerNotebook = new wxNotebookSizer( m_notebook );
    m_IDPanel = m_notebook->CreatePage(IDD_PAGE_IDENTITY);
    m_notebook->CreatePage(IDD_PAGE_CERTIFICATES);
    m_notebook->CreatePage(IDD_PAGE_CARDDATA);
    m_notebook->CreatePage(IDD_PAGE_OPTIONS);
#ifdef _WIN32
    m_notebook->CreatePage(IDD_PAGE_VERSIONINFO);
#endif
    m_notebook->SetSelection(0);

    // Give it an icon
    // The wxICON() macros loads an icon from a resource under Windows
    // and uses an #included XPM image under GTK+ and Motif

    SetIcon( wxICON(eidviewer) );
    int widths[] = {-1, 200};
    CreateStatusBar(2, 0);
    wxStatusBar *pBar = GetStatusBar();
    m_statusGauge = new wxGauge(pBar, -1, 100, wxDefaultPosition, wxDefaultSize, wxGA_SMOOTH);
    pBar->SetStatusWidths(2, widths);
    pBar->PushEventHandler(new StatusbarHandler(pBar, m_statusGauge));

#ifdef __WXMSW__    
    pBar->SetSize(-1,-1,-1,-1);
#endif

#if wxUSE_TOOLTIPS
    wxToolTip::SetDelay(500);
    wxToolTip::Enable(TRUE);
#endif

    Centre(wxBOTH);
}

eidviewerFrame::~eidviewerFrame()
{
    if(m_sizerNotebook)
    {
        delete m_sizerNotebook;
        m_sizerNotebook = NULL;
    }
}

void eidviewerFrame::OnQuit (wxCommandEvent & )
{
    Close(TRUE);
}

void eidviewerFrame::OnButtonChip(wxCommandEvent &)
{
    if(!m_bReading)
    {
        m_bReading = true;
        wxGetApp().ReadCard();
        m_bReading = false;
    }
}

void eidviewerFrame::OnButtonArrow(wxCommandEvent &)
{
    if(!m_bReading)
    {
        wxGetApp().ClearPages();
    }
}

void eidviewerFrame::OnButtonPrint(wxCommandEvent & event)
{
    if(!m_bReading)
    {
        m_notebook->OnButtonPrint(event);
        m_tbar->Refresh();
    }
}

void eidviewerFrame::OnButtonOpenFile(wxCommandEvent &)
{
    if(!m_bReading)
    {
        m_bReading = true;
        wxString strFile = wxEmptyString;
        wxGetApp().ReadFileCard(strFile);
        m_bReading = false;
    }
}

void eidviewerFrame::OnButtonSaveFile(wxCommandEvent &)
{
    if(!m_bReading)
    {
        m_bReading = true;
        wxGetApp().ReadRawCard();
        m_bReading = false;
    }
}

/*
    Function overriden to disable toolbar help strings in statusbar
*/
void eidviewerFrame::DoGiveHelp(const wxString&, bool)
{
}

void eidviewerFrame::SetStatusText(const wxString &text, int number, bool bSafeYield /* false */)
{
    wxFrame::SetStatusText(text, number);
    bSafeYield ? wxSafeYield() : wxYieldIfNeeded();
#ifndef _WIN32
    GetStatusBar()->Refresh();
#endif
}

void eidviewerFrame::SetProgressValue(int iValue)
{
    if(m_statusGauge != NULL)
    {
        m_statusGauge->SetValue(iValue);
    }
}

void eidviewerFrame::OnActivate (wxActivateEvent&)
{
    if(m_IDPanel)
    {
        m_IDPanel->SetBackgroundColour(wxColour(135,207,163));
    }
}
