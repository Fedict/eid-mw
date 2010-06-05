/********************************************************************************
** Form generated from reading UI file 'dlgwndpinpadinfo.ui'
**
** Created: Sat Jun 5 12:34:55 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLGWNDPINPADINFO_H
#define UI_DLGWNDPINPADINFO_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_dlgWndPinpadInfo
{
public:
    QGridLayout *gridLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout1;
    QLabel *label_2;
    QLabel *lblIcon;
    QLabel *label;

    void setupUi(QWidget *dlgWndPinpadInfo)
    {
        if (dlgWndPinpadInfo->objectName().isEmpty())
            dlgWndPinpadInfo->setObjectName(QString::fromUtf8("dlgWndPinpadInfo"));
        dlgWndPinpadInfo->resize(406, 299);
        gridLayout = new QGridLayout(dlgWndPinpadInfo);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        groupBox = new QGroupBox(dlgWndPinpadInfo);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout1 = new QGridLayout(groupBox);
        gridLayout1->setSpacing(6);
        gridLayout1->setContentsMargins(11, 11, 11, 11);
        gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout1->addWidget(label_2, 0, 1, 1, 1);

        lblIcon = new QLabel(groupBox);
        lblIcon->setObjectName(QString::fromUtf8("lblIcon"));

        gridLayout1->addWidget(lblIcon, 0, 0, 1, 1);


        gridLayout->addWidget(groupBox, 0, 0, 1, 1);

        label = new QLabel(dlgWndPinpadInfo);
        label->setObjectName(QString::fromUtf8("label"));
        label->setWordWrap(true);

        gridLayout->addWidget(label, 1, 0, 1, 1);


        retranslateUi(dlgWndPinpadInfo);

        QMetaObject::connectSlotsByName(dlgWndPinpadInfo);
    } // setupUi

    void retranslateUi(QWidget *dlgWndPinpadInfo)
    {
        dlgWndPinpadInfo->setWindowTitle(QApplication::translate("dlgWndPinpadInfo", "dlgWndPinpadInfo", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("dlgWndPinpadInfo", "Pinpad", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("dlgWndPinpadInfo", "How to use your pinpad:", 0, QApplication::UnicodeUTF8));
        lblIcon->setText(QString());
        label->setText(QApplication::translate("dlgWndPinpadInfo", "PinName and the message", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class dlgWndPinpadInfo: public Ui_dlgWndPinpadInfo {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGWNDPINPADINFO_H
