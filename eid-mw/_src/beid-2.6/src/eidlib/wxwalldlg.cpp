#include "BEIDApp.h"
#include "wxwalldlg.h"
#include "eidliblang.h"
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qlabel.h>

struct tMapFunc
{
    const char *pszFunc;
    const char *pszMsg;
};

CWallDialog::CWallDialog(QWidget* parent, const char* name, bool modal, WFlags fl) :
    WallDialog(parent, name, modal, fl)
{
    m_iResult = ID_BUTTON_NO;
    pixmapLabelWarning->setPixmap(QMessageBox::standardIcon ( QMessageBox::Warning  ));
    languageChange();
    connect( buttonYes, SIGNAL( clicked() ), this, SLOT( OnButtonYes() ) );
    connect( buttonNo, SIGNAL( clicked() ), this, SLOT( OnButtonNo() ) );
    connect( buttonAlways, SIGNAL( clicked() ), this, SLOT( OnButtonAlways() ) );
    connect( buttonAlwaysAll, SIGNAL( clicked() ), this, SLOT( OnButtonAlwaysToAll() ) );
}

void CWallDialog::OnButtonYes()
{
    m_iResult = ID_BUTTON_YES;
    QDialog::accept();
}

void CWallDialog::OnButtonNo()
{
    m_iResult = ID_BUTTON_NO;
    QDialog::accept();
}

void CWallDialog::OnButtonAlways()
{
    m_iResult = ID_BUTTON_ALWAYS;
    QDialog::accept();
}

void CWallDialog::OnButtonAlwaysToAll()
{
    m_iResult = ID_BUTTON_ALWAYS_ALL;
    QDialog::accept();
}

void CWallDialog::FillData(const QString & strApplication, const QString & strFunction)
{
    int iLng = CBEIDApp::GetLang();
    struct tMapFunc MsgMapFunc[] =
    {
        {"id", pLangReadId[iLng]},
        {"address",  pLangReadAddr[iLng]},
        {"photo",  pLangReadPic[iLng]},
        {"readfile",  pLangReadFile[iLng]},
        {"writefile",  pLangWriteFile[iLng]},
        {"sendapdu",  pLangSendApdu[iLng]},
        {NULL}
    };    
        
    textLabelApplicationName->setText(strApplication);
    bool bFound = false;
    unsigned int i = 0;
	for (i = 0; MsgMapFunc[i].pszFunc != NULL; i++) 
    {
		if (0 == strcmp(MsgMapFunc[i].pszFunc, strFunction.ascii()))
        {
            bFound = true;
            break;
        }
	}
    if (bFound)
    {
        textLabelFunctionName->setText(MsgMapFunc[i].pszMsg);
    }
}

void CWallDialog::languageChange()
{
    int iLng = CBEIDApp::GetLang();
    setCaption( pLangTitleConf[iLng] );
    textLabelApplicationText->setText( pLangAppAccess[iLng]);
    textLabelApplication->setText( pLangApp[iLng]);
    textLabelFunction->setText( pLangFunc[iLng] );
    textLabelAcceptText->setText( pLangAccept[iLng] );
    buttonYes->setText( pLangYes[iLng] );
    buttonNo->setText( pLangNo[iLng] );
    buttonAlways->setText( pLangAlways[iLng] );
    buttonAlwaysAll->setText( pLangAlwaysAll[iLng]);
}
