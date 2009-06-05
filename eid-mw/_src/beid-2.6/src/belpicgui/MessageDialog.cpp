// MessageDialog.cpp: implementation of the MessageDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "resource.h"
#include "MessageDialog.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMessageDialog::CMessageDialog(AskMessageData *pData)
{
    m_pData = pData;
}

CMessageDialog::~CMessageDialog()
{

}

LRESULT CMessageDialog::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CenterWindow(GetParent());
	// set icons
	HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_ICON_EID), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_ICON_EID), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);

    m_CtrlButtonOK = GetDlgItem(IDOK);
    m_CtrlButtonCancel = GetDlgItem(IDCANCEL);
    m_CtrlStaticLong = GetDlgItem(IDC_STATIC_LABEL_LONGMSG);
    m_CtrlStaticShort = GetDlgItem(IDC_STATIC_LABEL_SHORTMSG);
    int iNum = 0;

    m_CtrlButtonOK.ShowWindow(SW_HIDE);
    m_CtrlButtonCancel.ShowWindow(SW_HIDE);
    if(m_pData != NULL)
    {
        SetWindowText(m_pData->title);
        if(m_pData->btn_ok)
        {
            m_CtrlButtonOK.SetWindowText(m_pData->btn_ok); 
            m_CtrlButtonOK.ShowWindow(SW_SHOW);
            iNum++;
        }
        if(m_pData->btn_cancel)
        {
            m_CtrlButtonCancel.SetWindowText(m_pData->btn_cancel); 
            m_CtrlButtonCancel.ShowWindow(SW_SHOW);
            iNum++;
        }
        if(iNum == 1)
        {
            CButton *pButton = m_pData->btn_ok ? &m_CtrlButtonOK : &m_CtrlButtonCancel;
            CRect oRectButton;
            pButton->GetWindowRect(&oRectButton);
            CRect oRectParent;
            this->GetWindowRect(&oRectParent);
            int x = oRectParent.Width() /2 - oRectButton.Width() / 2;
            int y = (oRectButton.top - oRectParent.top) - oRectButton.Height();
            pButton->MoveWindow(x, y, oRectButton.Width(), oRectButton.Height());
        }
        if(m_pData->short_msg)
        {
            m_CtrlStaticShort.SetWindowText(m_pData->short_msg);
        }
        if(m_pData->long_msg)
        {
            m_CtrlStaticLong.SetWindowText(m_pData->long_msg);
        }

    }
    ::SetForegroundWindow(m_hWnd);
	return TRUE;
}

LRESULT CMessageDialog::OnCtlColorStatic(UINT uMsg,
                       WPARAM wParam,
                       LPARAM lParam,
                       BOOL& bHandled)
{
    HDC hdcStatic = (HDC) wParam;    // handle to display context 
    HWND hwndStatic = (HWND) lParam; // handle to static control 

    if ( hwndStatic == GetDlgItem(IDC_STATIC_LABEL_SHORTMSG) || hwndStatic == GetDlgItem(IDC_STATIC_ASKPIN_ICON))
    {
        // m_hBrush is an instance variable that stores the
        // current Color brush. If this is the first time through
        // we need to create it. The brush is deleted in the destructor
        if ( m_hBrush == NULL )
        {
            m_hBrush = CreateSolidBrush( RGB(255, 255,255));
        }

        // Get the DC provided and set the appropriate attributes
        SetBkMode(hdcStatic, TRANSPARENT );
        SetBkColor(hdcStatic, RGB(255, 255, 255));

        // Return a handle to the brush
        HBRUSH far* hbr = (HBRUSH far*) (HBRUSH)m_hBrush;
        return ((DWORD) hbr);
    }
    return 0;
}

LRESULT CMessageDialog::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    if(m_pData != NULL)
    {
        m_pData->ret = SCGUI_CANCEL;
    }
	EndDialog(wID);
	return 0;
}

LRESULT CMessageDialog::OnOKCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    if(m_pData != NULL)
    {
        m_pData->ret = SCGUI_OK;
    }
	EndDialog(wID);
	return 0;
}
