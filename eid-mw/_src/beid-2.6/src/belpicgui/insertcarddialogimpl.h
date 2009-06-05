#ifndef INSERTCARDDIALOGIMPL_H
#define INSERTCARDDIALOGIMPL_H

#include "insertcarddialog.h"
#include "scgui.h"
#include <qtimer.h>

class InsertCardDialogImpl : public InsertCardDialog
{
    Q_OBJECT

public:
    InsertCardDialogImpl( InsertCardData *pData, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = Qt::WStyle_Customize | Qt::WStyle_NormalBorder | Qt::WStyle_Title | Qt::WType_TopLevel |
        Qt::WStyle_StaysOnTop);
    ~InsertCardDialogImpl();

protected slots:
    void accept();
    void timerDone();

private:
    InsertCardData *m_pInsertData;
    QTimer *m_pTimer;
#ifdef _WIN32
    HANDLE m_hEvent;
#endif
};

#endif // INSERTCARDDIALOGIMPL_H
