#ifndef CHANGEPINDIALOGIMPL_H
#define CHANGEPINDIALOGIMPL_H

#include "changepindialog.h"
#include "scgui.h"

class changepindialogImpl : public changepindialog
{
    Q_OBJECT

public:
    changepindialogImpl( ChangePinData *pData, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = Qt::WStyle_Customize | Qt::WStyle_NormalBorder | Qt::WStyle_Title | Qt::WType_TopLevel |
        Qt::WStyle_StaysOnTop);
    ~changepindialogImpl();

protected slots:
        void accept();
        void textChangedPIN(const QString&);

private:
    ChangePinData *m_pChangeData;
};

#endif // CHANGEPINDIALOGIMPL_H
