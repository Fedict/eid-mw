#ifndef AUTHENTICATIONDIALOGIMPL_H
#define AUTHENTICATIONDIALOGIMPL_H

#include "authenticationdialog.h"
#include "scgui.h"

class authenticationdialogImpl : public authenticationdialog
{
    Q_OBJECT

public:
    authenticationdialogImpl( VerifyPinData *pData, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = Qt::WStyle_Customize | Qt::WStyle_NormalBorder | Qt::WStyle_Title | Qt::WType_TopLevel |
        Qt::WStyle_StaysOnTop);
    ~authenticationdialogImpl();

protected slots:
        void accept();
        void textChangedPIN(const QString&);
private:
    VerifyPinData *m_pVerifyData;
};

#endif // AUTHENTICATIONDIALOGIMPL_H
