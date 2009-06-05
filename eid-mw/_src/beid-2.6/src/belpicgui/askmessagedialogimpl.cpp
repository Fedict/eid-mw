#include "askmessagedialogimpl.h"
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#ifdef _WIN32
    #include <qt_windows.h>
#endif

/* 
 *  Constructs a askmessagedialogImpl which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
askmessagedialogImpl::askmessagedialogImpl( AskMessageData *pData, QWidget* parent,  const char* name, bool modal, WFlags fl )
    : askmessagedialog( parent, name, modal, fl )
{
    m_pAskData = pData;
    int iNum = 0;
    if(m_pAskData != NULL)
    {
        buttonOk->hide();
        buttonCancel->hide();
        setCaption(m_pAskData->title);
        if(m_pAskData->btn_ok)
        {
            buttonOk->setText(m_pAskData->btn_ok);
            buttonOk->show();
            iNum++;
        }
        if(m_pAskData->btn_cancel)
        {
            buttonCancel->setText(m_pAskData->btn_cancel);
            buttonCancel->show();
            iNum++;
        }
        if(iNum == 1)
        {
            QPushButton *pButton = buttonOk ? buttonOk : buttonCancel;
            QRect oRectButton = pButton->rect();
            QRect oRectParent = this->rect();
            int x = oRectParent.width() /2 - oRectButton.width() / 2;
            int y = pButton->geometry().y();
            pButton->move(x, y);
        }
        if(m_pAskData->pinpad_reader)
        {
//            TextLabelReader->setText(m_pAskData->pinpad_reader);
        }
        if(m_pAskData->short_msg)
        {
            TextLabelShortMsg->setText(m_pAskData->short_msg);
        }
        if(m_pAskData->long_msg)
        {
            TextLabelLongMsg->setText(m_pAskData->long_msg);
        }
    }
#ifdef _WIN32
    ::SetForegroundWindow(this->winId());
#endif
}

/*  
 *  Destroys the object and frees any allocated resources
 */
askmessagedialogImpl::~askmessagedialogImpl()
{
    // no need to delete child widgets, Qt does it all for us
}

void askmessagedialogImpl::accept()
{
    if(m_pAskData != NULL)
    {
        m_pAskData->ret = SCGUI_OK;
    }
    QDialog::accept();
}

void askmessagedialogImpl::reject()
{
    if(m_pAskData != NULL)
    {
        m_pAskData->ret = SCGUI_CANCEL;
    }
    QDialog::reject();
}
