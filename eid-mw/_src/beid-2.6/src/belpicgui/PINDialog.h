// PINDialog.h: interface for the PINDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PINDIALOG_H__4C56E002_2C1C_4B6C_9E7F_F66756C7B5B5__INCLUDED_)
#define AFX_PINDIALOG_H__4C56E002_2C1C_4B6C_9E7F_F66756C7B5B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "belpicgui.h"
#include "scgui.h"
#include "resource.h"

#define MIN_PIN_LEN 4
#define MAX_PIN_LEN 12

class CPINDialog : public CDialogImpl<CPINDialog>, public CWinDataExchange< CPINDialog >  
{
public:
	CPINDialog(VerifyPinData *pData);
	virtual ~CPINDialog();

	enum { IDD = IDD_DIALOG_ASKPIN };

	BEGIN_MSG_MAP(CPINDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColorStatic)
        COMMAND_HANDLER(IDC_EDIT_PIN, EN_CHANGE, OnChangePIN)
		COMMAND_ID_HANDLER(IDOK, OnOKCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
	END_MSG_MAP()

    BEGIN_DDX_MAP(CPINDialog)
        DDX_TEXT_LEN(IDC_EDIT_PIN, m_strPIN, MAX_PIN_LEN)
    END_DDX_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnOKCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnChangePIN(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCtlColorStatic(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

    bool CheckFields();

private:
    CString m_strPIN;
    CEdit m_CtrlEditPIN;
    CButton m_CtrlButtonOK;
    CButton m_CtrlButtonCancel;
    VerifyPinData *m_pVerifyData;
    CStatic m_CtrlStaticPIN;
    CStatic m_CtrlStaticPINHeader;
    CStatic m_CtrlStaticBitmapHeader;
    CBitmap m_oBmp;
    CBrush m_hBrush;
};

#endif // !defined(AFX_PINDIALOG_H__4C56E002_2C1C_4B6C_9E7F_F66756C7B5B5__INCLUDED_)
