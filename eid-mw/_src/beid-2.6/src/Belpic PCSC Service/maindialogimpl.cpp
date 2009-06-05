#include "maindialogimpl.h"

using namespace EIDCommLIB;

CPinPadEvent::CPinPadEvent() : QEvent( (QEvent::Type) MYPINPADEVENT )
{
}

CPinPadEvent::~CPinPadEvent()
{
}

/* 
 *  Constructs a maindialogImpl which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
maindialogImpl::maindialogImpl( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : maindialog( parent, name, modal, fl )
{
    m_pPinPad = new CPinPad();
    m_pConnMgr = NULL;
}

/*  
 *  Destroys the object and frees any allocated resources
 */
maindialogImpl::~maindialogImpl()
{
    if(m_pPinPad)
    {
        delete m_pPinPad;
        m_pPinPad = NULL;
    }
    // no need to delete child widgets, Qt does it all for us
}

bool maindialogImpl::event( QEvent* e )
{
    if ( e->type() == (QEvent::Type) MYPINPADEVENT ) 
    {
        CPinPadEvent* re = static_cast<CPinPadEvent*>( e );
        if(m_pPinPad)
        {
            CCardMessage oMessage;
            oMessage.Unserialize((const byte *)re->m_oData.data(), re->m_oData.size());   
            byte *pSendBuffer = NULL;
            unsigned long ulSendLen = 0;
            unsigned long ulRecvLen = 0;
            unsigned long ulControlCode = 0;
            long lConnID = 0;
            SCARDHANDLE hReal = 0;
            oMessage.Get("ConnID", lConnID);
            oMessage.Get("OriginalHandle", (long *)&hReal);
            oMessage.Get("ControlCode", (long *)&ulControlCode);
            if(oMessage.Get("SendBufferLen", (long *)&ulSendLen))
            {
                pSendBuffer = new BYTE[ulSendLen];
                memset(pSendBuffer, 0, ulSendLen);
                oMessage.Get("SendBuffer", (BYTE *)pSendBuffer, ulSendLen);
            }

            if(!oMessage.Get("RecvLen", (long *)&ulRecvLen))
            {
                ulRecvLen = 256;
            }
            BYTE *pRecvBuffer = new BYTE[ulRecvLen];
            memset(pRecvBuffer, 0, ulRecvLen);
            unsigned long ulBytesRet = 0;            
            long lRet = m_pPinPad->HandlePinPad(pSendBuffer, ulSendLen, pRecvBuffer, &ulBytesRet, hReal);
            oMessage.Clear(); 
            oMessage.Set("RecvLen", (long)ulBytesRet);
            if(ulBytesRet > 0)
            {
                oMessage.Set("RecvBuffer", (BYTE *)pRecvBuffer, ulBytesRet);
            }
            if(pSendBuffer != NULL)
            {
                delete pSendBuffer;
            }
            if(pRecvBuffer != NULL)
            {
                delete pRecvBuffer;
            }
            if(m_pConnMgr)
            {
                CConnection *pConnection = m_pConnMgr->GetConnection(lConnID);
                if(pConnection)
                {
                    pConnection->SdMessage(&oMessage);
                }
            }
        }
        return true;
    }
    else
        return maindialog::event( e );
}
