/////////////////////////////////////////////////////////////////////////////
// Name:        eidviewerDlg.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     
// RCS-ID:      
// Copyright:   (C)2003 
// Licence:     wxWindows
/////////////////////////////////////////////////////////////////////////////

#ifndef eidviewerDialog_H
#define eidviewerDialog_H

// Define a new Dialog type: this is going to be our main Dialog
class eidviewerDialog : public wxDialog
{
public:
    // ctor(s)
    eidviewerDialog(const wxString& title, const wxPoint& pos, const wxSize& size);
	virtual ~eidviewerDialog();

    // event handlers (these functions should _not_ be virtual)

protected:
    void OnQuit( wxCommandEvent &event );

private:
    DECLARE_CLASS(eidviewerDialog)
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

#endif // eidviewerDialog_H
