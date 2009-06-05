#include "changepindialogimpl.h"
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qvalidator.h>
#ifdef _WIN32
    #include <qt_windows.h>
#endif

extern int MapLanguage();

static const char *lbl_msg_oldpin[4] = 
{
	"Old PIN:",
	"Oude PIN:",
	"Code PIN actuel:",
	"Alter PIN:"
};

static const char *lbl_msg_newpin[4] = 
{
	"New PIN:",
	"Nieuwe PIN:",
	"Nouveau code PIN:",
	"Neuer PIN:"
};

static const char *lbl_msg_confirmpin[4] = 
{
	"Confirm PIN:",
	"Bevestig nieuwe PIN:",
	"Confirmer nouveau code PIN:",
	"Bestätigen Sie Neuen:"
};


/* 
 *  Constructs a changepindialogImpl which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
changepindialogImpl::changepindialogImpl( ChangePinData *pData, QWidget* parent,  const char* name, bool modal, WFlags fl )
    : changepindialog( parent, name, modal, fl )
{
    m_pChangeData = pData;
    if(m_pChangeData != NULL)
    {
        setCaption(m_pChangeData->title);
        buttonOk->setText(m_pChangeData->btn_ok);
        buttonCancel->setText(m_pChangeData->btn_cancel);
        TextLabelChangePIN->setText(m_pChangeData->msg);
    }
    buttonOk->setEnabled(false);
    lineEditOldPIN->setMaxLength(BELPIC_PIN_MAX_LENGTH);
    lineEditNewPIN->setMaxLength(BELPIC_PIN_MAX_LENGTH);
    QRegExp rx( "[0-9]\\d{4,12}" );
    lineEditNewPIN->setValidator(new QRegExpValidator(rx, this));
    lineEditConfirmNew->setMaxLength(BELPIC_PIN_MAX_LENGTH);
    lineEditConfirmNew->setValidator(new QRegExpValidator(rx, this));
    lineEditOldPIN->setFocus();
    lineEditOldPIN->setValidator(new QRegExpValidator(rx, this));
    connect( lineEditOldPIN, SIGNAL(textChanged(const QString&)), this, SLOT(textChangedPIN(const QString&)) );
    connect( lineEditNewPIN, SIGNAL(textChanged(const QString&)), this, SLOT(textChangedPIN(const QString&)) );
    connect( lineEditConfirmNew, SIGNAL(textChanged(const QString&)), this, SLOT(textChangedPIN(const QString&)) );
    int iLng = MapLanguage();
    textLabelPINOld->setText( lbl_msg_oldpin[iLng] );
    textLabelPINNew->setText( lbl_msg_newpin[iLng] );
    textLabelPINConfirmNew->setText( lbl_msg_confirmpin[iLng] );
#ifdef _WIN32
    ::SetForegroundWindow(this->winId());
#endif
}

/*  
 *  Destroys the object and frees any allocated resources
 */
changepindialogImpl::~changepindialogImpl()
{
    // no need to delete child widgets, Qt does it all for us
}

void changepindialogImpl::accept()
{
    if(lineEditNewPIN->text() != lineEditConfirmNew->text())
    {
        if(m_pChangeData != NULL)
        {
            textLabelErrorMsg->setText(m_pChangeData->confirmPinError);
        }
        else
        {
            textLabelErrorMsg->setText("The new PINs you entered were different.");
        }
        lineEditNewPIN->selectAll();
        lineEditNewPIN->setFocus();
        return;
    }
    if(m_pChangeData != NULL)
    {
        strcpy(m_pChangeData->oldpin, lineEditOldPIN->text());
        strcpy(m_pChangeData->newpin, lineEditNewPIN->text());
    }
    QDialog::accept();
}

void changepindialogImpl::textChangedPIN(const QString & strPIN)
{
    if(lineEditOldPIN->text().length() >= BELPIC_PIN_MIN_LENGTH &&
        lineEditNewPIN->text().length() >= BELPIC_PIN_MIN_LENGTH &&
        lineEditConfirmNew->text().length() >= BELPIC_PIN_MIN_LENGTH)
    {
        buttonOk->setEnabled(true);
    }
    else
    {
        buttonOk->setEnabled(false);
    }
}

