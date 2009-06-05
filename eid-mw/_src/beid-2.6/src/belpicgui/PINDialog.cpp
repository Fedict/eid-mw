// PINDialog.cpp: implementation of the PINDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "resource.h"
#include "PINDialog.h"


extern int MapLanguage();

static char *lbl_pin[4] = 
{
	"PIN:",
	"PIN:",
	"Code PIN:",
	"PIN:"
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPINDialog::CPINDialog(VerifyPinData *pData)
{
    m_pVerifyData = pData;
}

CPINDialog::~CPINDialog()
{

}

LRESULT CPINDialog::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CenterWindow(GetParent());
	// set icons
	HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_ICON_EID), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_ICON_EID), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);

    m_CtrlEditPIN = GetDlgItem(IDC_EDIT_PIN);
    m_CtrlButtonOK = GetDlgItem(IDOK);
    m_CtrlButtonCancel = GetDlgItem(IDCANCEL);
    m_CtrlStaticPIN = GetDlgItem(IDC_STATIC_PIN);
    m_CtrlStaticPINHeader = GetDlgItem(IDC_STATIC_LABEL_ENTERPIN);
    m_CtrlStaticBitmapHeader = GetDlgItem(IDC_STATIC_ASKPIN_ICON);

    m_CtrlButtonOK.EnableWindow(FALSE);
    if(m_pVerifyData != NULL)
    {
        SetWindowText(m_pVerifyData->title);
        m_CtrlButtonOK.SetWindowText(m_pVerifyData->btn_ok); 
        m_CtrlButtonCancel.SetWindowText(m_pVerifyData->btn_cancel); 
        if(m_pVerifyData->iconSign != SCGUI_SIGN_ICON)
        {
            m_oBmp.LoadBitmap(IDB_BITMAP_AUTH); 
            m_CtrlStaticPINHeader.ModifyStyle(0, SS_CENTERIMAGE);
            m_CtrlStaticPINHeader.SetWindowText(m_pVerifyData->msg); 
        }
        else
        {
            CString strText("\r\n");
            strText += m_pVerifyData->msg;
            m_CtrlStaticPINHeader.SetWindowText(strText); 
            m_oBmp.LoadBitmap(IDB_BITMAP_SIGN); 
        }
        m_CtrlStaticBitmapHeader.SetBitmap(m_oBmp); 
    }
    int iLng = MapLanguage();
    m_CtrlStaticPIN.SetWindowText(lbl_pin[iLng]); 
    ::SetForegroundWindow(m_hWnd);
	return TRUE;
}

LRESULT CPINDialog::OnCtlColorStatic(UINT uMsg,
                       WPARAM wParam,
                       LPARAM lParam,
                       BOOL& bHandled)
{
    HDC hdcStatic = (HDC) wParam;    // handle to display context 
    HWND hwndStatic = (HWND) lParam; // handle to static control 

    if ( hwndStatic == GetDlgItem(IDC_STATIC_LABEL_ENTERPIN) || hwndStatic == GetDlgItem(IDC_STATIC_ASKPIN_ICON))
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

LRESULT CPINDialog::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}

LRESULT CPINDialog::OnOKCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    if(DoDataExchange(DDX_SAVE))
    {
        if(!CheckFields())
        {
            return 0;
        }
    }
    if(m_pVerifyData != NULL)
    {
        strcpy(m_pVerifyData->pin, m_strPIN);
    }
	EndDialog(wID);
	return 0;
}

bool CPINDialog::CheckFields()
{
    bool bRet = false;
    int iLen = m_strPIN.GetLength();
    if(iLen >= MIN_PIN_LEN && iLen <= MAX_PIN_LEN)
    {
        bRet = true;
    }
    return bRet;
}

LRESULT CPINDialog::OnChangePIN(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    if(DoDataExchange(DDX_SAVE))
    {
        m_CtrlButtonOK.EnableWindow(CheckFields() ? TRUE : FALSE);
    }
    return 0;
}

