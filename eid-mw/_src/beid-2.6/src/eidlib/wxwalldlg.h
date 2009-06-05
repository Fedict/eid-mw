#ifndef __walldlg_H__
#define __walldlg_H__

#include "walldialog.h"

#define ID_BUTTON_NO 0
#define ID_BUTTON_YES 1
#define ID_BUTTON_ALWAYS 2
#define ID_BUTTON_ALWAYS_ALL 3

class CWallDialog: public WallDialog
{
    Q_OBJECT
public:
    CWallDialog(QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = Qt::WStyle_Customize | Qt::WStyle_NormalBorder | 
        Qt::WStyle_Title | Qt::WStyle_StaysOnTop);
    void FillData(const QString & strApplication, const QString & strFunction);
    int GetResult() { return m_iResult; }

protected slots:
    void OnButtonYes();
    void OnButtonNo();
    void OnButtonAlways();
    void OnButtonAlwaysToAll();
    void languageChange();

private:
    int m_iResult;
};

#endif
