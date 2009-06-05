#include "authenticationdialogimpl.h"
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qvalidator.h>
#ifdef _WIN32
    #include <qt_windows.h>
#endif

extern int MapLanguage();

static const char *lbl_msg_pin[4] = 
{
	"PIN:",
	"PIN:",
	"Code PIN:",
	"PIN:"
};

/* 
 *  Constructs a authenticationdialogImpl which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
authenticationdialogImpl::authenticationdialogImpl( VerifyPinData *pData, QWidget* parent,  const char* name, bool modal, WFlags fl )
    : authenticationdialog( parent, name, modal, fl )
{
    m_pVerifyData = pData;
    if(m_pVerifyData != NULL)
    {
        setCaption(m_pVerifyData->title);
        buttonOk->setText(m_pVerifyData->btn_ok);
        buttonCancel->setText(m_pVerifyData->btn_cancel);
        TextLabelVerify->setText(m_pVerifyData->msg);
        if(m_pVerifyData->iconSign != SCGUI_SIGN_ICON)
        {
            pixmapSigAuth->setPixmap(QPixmap::fromMimeSource(QString::fromAscii("pincode_128.png")));
        }
        else
        {
            pixmapSigAuth->setPixmap(QPixmap::fromMimeSource(QString::fromAscii("signature_128.png")));
        }
    }
    buttonOk->setEnabled(false);
    lineEditPIN->setMaxLength(BELPIC_PIN_MAX_LENGTH);
    lineEditPIN->setFocus();
    QRegExp rx( "[0-9]\\d{4,12}" );
    lineEditPIN->setValidator(new QRegExpValidator(rx, this));
    connect( lineEditPIN, SIGNAL(textChanged(const QString&)), this, SLOT(textChangedPIN(const QString&)) );
    int iLng = MapLanguage();
    textLabelPIN->setText( lbl_msg_pin[iLng] );
#ifdef _WIN32
    ::SetForegroundWindow(this->winId());
#endif
}

/*  
 *  Destroys the object and frees any allocated resources
 */
authenticationdialogImpl::~authenticationdialogImpl()
{
    // no need to delete child widgets, Qt does it all for us
}

void authenticationdialogImpl::accept()
{
    if(m_pVerifyData != NULL)
    {
        strcpy(m_pVerifyData->pin, lineEditPIN->text());
    }
    QDialog::accept();
}

void authenticationdialogImpl::textChangedPIN(const QString & strPIN)
{
    buttonOk->setEnabled(strPIN.length() >= BELPIC_PIN_MIN_LENGTH ? true : false);
}
