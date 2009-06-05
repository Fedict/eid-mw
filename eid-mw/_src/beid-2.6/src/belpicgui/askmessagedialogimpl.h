#ifndef ASKMESSAGEDIALOGIMPL_H
#define ASKMESSAGEDIALOGIMPL_H

#include "askmessagedialog.h"
#include "scgui.h"

class askmessagedialogImpl : public askmessagedialog
{
    Q_OBJECT

public:
    askmessagedialogImpl( AskMessageData *pData, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = Qt::WStyle_Customize | Qt::WStyle_NormalBorder | Qt::WStyle_Title | Qt::WType_TopLevel |
        Qt::WStyle_StaysOnTop);
    ~askmessagedialogImpl();

protected slots:
    void accept();
    void reject();

private:
    AskMessageData *m_pAskData;
};

#endif // ASKMESSAGEDIALOGIMPL_H
