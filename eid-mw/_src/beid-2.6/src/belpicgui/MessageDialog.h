// MessageDialog.h: interface for the MessageDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESSAGEDIALOG_H__4C56E002_2C1C_4B6C_9E7F_F66756C7B5AA__INCLUDED_)
#define AFX_MESSAGEDIALOG_H__4C56E002_2C1C_4B6C_9E7F_F66756C7B5AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "belpicgui.h"
#include "scgui.h"
#include "resource.h"

class CMessageDialog : public CDialogImpl<CMessageDialog>, public CWinDataExchange< CMessageDialog >  
{
public:
	CMessageDialog(AskMessageData *pData);
	virtual ~CMessageDialog();

	enum { IDD = IDD_DIALOG_MESSAGE };

	BEGIN_MSG_MAP(CMessageDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColorStatic)
		COMMAND_ID_HANDLER(IDOK, OnOKCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnOKCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCtlColorStatic(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

private:
    CButton m_CtrlButtonOK;
    CButton m_CtrlButtonCancel;
    AskMessageData *m_pData;
    CStatic m_CtrlStaticLong;
    CStatic m_CtrlStaticShort;
    CBrush m_hBrush;
};

#endif // !defined(AFX_MESSAGEDIALOG_H__4C56E002_2C1C_4B6C_9E7F_F66756C7B5AA__INCLUDED_)
