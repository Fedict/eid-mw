#include "insertcarddialogimpl.h"
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#ifdef _WIN32
    #include <qt_windows.h>
#endif

/* 
 *  Constructs a InsertCardDialogImpl which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
InsertCardDialogImpl::InsertCardDialogImpl( InsertCardData *pData, QWidget* parent,  const char* name, bool modal, WFlags fl )
    : InsertCardDialog( parent, name, modal, fl )
{
#ifdef _WIN32
    m_hEvent = NULL;
#endif
    m_pInsertData = pData;
    if(m_pInsertData != NULL)
    {
        //buttonOk->hide();
        setCaption(m_pInsertData->title);
        if(m_pInsertData->btn_ok)
        {
            //buttonOk->setText(m_pInsertData->btn_ok);
        }
        if(m_pInsertData->btn_cancel)
        {
            buttonCancel->setText(m_pInsertData->btn_cancel);
        }
        if(m_pInsertData->msg)
        {
            textLabelInsertCard->setText(m_pInsertData->msg);
        }
    }
    m_pTimer = new QTimer( this );
    connect( m_pTimer, SIGNAL(timeout()), this, SLOT(timerDone()) );
    m_pTimer->start( 100, TRUE );
#ifdef _WIN32
    m_hEvent = ::CreateEvent(NULL, FALSE, FALSE, "BEID_CSP_INSERT_CARD");
    ::SetForegroundWindow(this->winId());
#endif
}

/*  
 *  Destroys the object and frees any allocated resources
 */
InsertCardDialogImpl::~InsertCardDialogImpl()
{
    // no need to delete child widgets, Qt does it all for us
#ifdef _WIN32
    if(m_hEvent)
    {
        ::CloseHandle(m_hEvent);
        m_hEvent = NULL;
    }
#endif
}

void InsertCardDialogImpl::accept()
{
    if(m_pInsertData != NULL)
    {
        m_pInsertData->ret = SCGUI_OK;
    }
    QDialog::accept();
}

void InsertCardDialogImpl::timerDone()
{
    if(m_pInsertData != NULL && m_pInsertData->timer_func != NULL)
    {
        if(m_pInsertData->timer_func() != 0)
        {
            m_pTimer->start( 100, TRUE );
        }
        else
        {
            m_pInsertData->ret = SCGUI_OK;
#ifdef _WIN32
            if(m_hEvent)
            {
                ::SetEvent(m_hEvent);
            }
#endif
            this->close();
        }
    }
}
