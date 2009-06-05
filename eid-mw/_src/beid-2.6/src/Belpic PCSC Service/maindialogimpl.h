#ifndef MAINDIALOGIMPL_H
#define MAINDIALOGIMPL_H

#include "StdAfx.h"
#include "maindialog.h"

#include <qevent.h> 
#include <qcstring.h> 
#include "../beidcommon/pinpad.h"

#define MYPINPADEVENT 5100

class EIDCommLIB::CConnectionManager;

class CPinPadEvent : public QEvent  
{
public:
    CPinPadEvent();
    virtual ~CPinPadEvent();
    void SetData(const char *pData, unsigned long lLen)
    {
        m_oData.duplicate(pData, lLen);
    }

    QByteArray m_oData;
};

class maindialogImpl : public maindialog
{
    Q_OBJECT

public:
    maindialogImpl( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~maindialogImpl();
    void SetConnectionManager(EIDCommLIB::CConnectionManager *pMgr) { m_pConnMgr = pMgr; }

protected:
    bool event( QEvent* e );

private:
    CPinPad *m_pPinPad;
    EIDCommLIB::CConnectionManager *m_pConnMgr;
};

#endif // MAINDIALOGIMPL_H
