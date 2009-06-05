/////////////////////////////////////////////////////////////////////////////
// Name:        eidviewerDlg.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     
// RCS-ID:      
// Copyright:   (C)2003 
// Licence:     wxWindows
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "eidviewerApp.h"
#include "eidviewerDlg.h"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------
// the application icon
#if defined(__WXGTK__) || defined(__WXMOTIF__)
    #include "res/eidviewer.xpm"
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Minimal_Quit = 1,
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
IMPLEMENT_CLASS(eidviewerDialog, wxDialog)
BEGIN_EVENT_TABLE(eidviewerDialog, wxDialog)
    EVT_BUTTON(Minimal_Quit,  eidviewerDialog::OnQuit)
   // EVT_BUTTON(Minimal_About, eidviewerDialog::OnAbout)
    EVT_CLOSE(eidviewerDialog::OnQuit)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// main Dialog
// ----------------------------------------------------------------------------

// Dialog constructor
eidviewerDialog::eidviewerDialog(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxDialog((wxDialog *) NULL, -1, title, pos, size)
{
    // Add a control to the dialog
   // wxButton* quitButton = new wxButton(this, Minimal_Quit, _("Quit"), wxPoint(50, 10), wxSize(40, 20));
  //  wxButton* aboutButton = new wxButton(this, Minimal_About, _("About"), wxPoint(50, 40), wxSize(40, 20));

    // set the Dialog icon
    SetIcon(wxICON(eidviewer));
}

// Dialog destructor
eidviewerDialog::~eidviewerDialog()
{
}

void eidviewerDialog::OnQuit( wxCommandEvent &event )
{
    Destroy();
}

// event handlers
