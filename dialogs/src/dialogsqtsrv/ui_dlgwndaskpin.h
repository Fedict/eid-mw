/********************************************************************************
** Form generated from reading UI file 'dlgwndaskpin.ui'
**
** Created: Sat Jun 5 12:34:55 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLGWNDASKPIN_H
#define UI_DLGWNDASKPIN_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_dlgWndAskPINClass
{
public:
    QVBoxLayout *vboxLayout;
    QFrame *fraPIN_Keypad;
    QGridLayout *gridLayout;
    QHBoxLayout *hboxLayout;
    QLabel *lblPINName_2;
    QLineEdit *txtPIN_2;
    QToolButton *tbtNUM_1;
    QToolButton *tbtNUM_2;
    QToolButton *tbtNUM_3;
    QToolButton *tbtNUM_4;
    QToolButton *tbtNUM_5;
    QToolButton *tbtNUM_6;
    QToolButton *tbtNUM_7;
    QToolButton *tbtNUM_8;
    QToolButton *tbtNUM_9;
    QToolButton *tbtNUM_0;
    QToolButton *tbtClear;
    QGridLayout *gridLayout1;
    QLabel *lblIcon;
    QLabel *lblHeader;
    QSpacerItem *spacerItem;
    QFrame *fraPIN_Normal;
    QGridLayout *gridLayout2;
    QSpacerItem *spacerItem1;
    QSpacerItem *spacerItem2;
    QLabel *lblPINName;
    QLineEdit *txtPIN;
    QSpacerItem *spacerItem3;
    QSpacerItem *spacerItem4;
    QHBoxLayout *hboxLayout1;
    QSpacerItem *spacerItem5;
    QPushButton *btnOk;
    QPushButton *btnCancel;

    void setupUi(QDialog *dlgWndAskPINClass)
    {
        if (dlgWndAskPINClass->objectName().isEmpty())
            dlgWndAskPINClass->setObjectName(QString::fromUtf8("dlgWndAskPINClass"));
        dlgWndAskPINClass->resize(363, 359);
        vboxLayout = new QVBoxLayout(dlgWndAskPINClass);
        vboxLayout->setSpacing(6);
        vboxLayout->setContentsMargins(11, 11, 11, 11);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        fraPIN_Keypad = new QFrame(dlgWndAskPINClass);
        fraPIN_Keypad->setObjectName(QString::fromUtf8("fraPIN_Keypad"));
        fraPIN_Keypad->setFrameShape(QFrame::StyledPanel);
        fraPIN_Keypad->setFrameShadow(QFrame::Raised);
        gridLayout = new QGridLayout(fraPIN_Keypad);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        hboxLayout = new QHBoxLayout();
        hboxLayout->setSpacing(6);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        lblPINName_2 = new QLabel(fraPIN_Keypad);
        lblPINName_2->setObjectName(QString::fromUtf8("lblPINName_2"));
        lblPINName_2->setLayoutDirection(Qt::RightToLeft);

        hboxLayout->addWidget(lblPINName_2);

        txtPIN_2 = new QLineEdit(fraPIN_Keypad);
        txtPIN_2->setObjectName(QString::fromUtf8("txtPIN_2"));
        txtPIN_2->setEchoMode(QLineEdit::Password);

        hboxLayout->addWidget(txtPIN_2);


        gridLayout->addLayout(hboxLayout, 0, 0, 1, 3);

        tbtNUM_1 = new QToolButton(fraPIN_Keypad);
        tbtNUM_1->setObjectName(QString::fromUtf8("tbtNUM_1"));
        tbtNUM_1->setPopupMode(QToolButton::DelayedPopup);
        tbtNUM_1->setToolButtonStyle(Qt::ToolButtonIconOnly);

        gridLayout->addWidget(tbtNUM_1, 1, 0, 1, 1);

        tbtNUM_2 = new QToolButton(fraPIN_Keypad);
        tbtNUM_2->setObjectName(QString::fromUtf8("tbtNUM_2"));

        gridLayout->addWidget(tbtNUM_2, 1, 1, 1, 1);

        tbtNUM_3 = new QToolButton(fraPIN_Keypad);
        tbtNUM_3->setObjectName(QString::fromUtf8("tbtNUM_3"));

        gridLayout->addWidget(tbtNUM_3, 1, 2, 1, 1);

        tbtNUM_4 = new QToolButton(fraPIN_Keypad);
        tbtNUM_4->setObjectName(QString::fromUtf8("tbtNUM_4"));

        gridLayout->addWidget(tbtNUM_4, 2, 0, 1, 1);

        tbtNUM_5 = new QToolButton(fraPIN_Keypad);
        tbtNUM_5->setObjectName(QString::fromUtf8("tbtNUM_5"));

        gridLayout->addWidget(tbtNUM_5, 2, 1, 1, 1);

        tbtNUM_6 = new QToolButton(fraPIN_Keypad);
        tbtNUM_6->setObjectName(QString::fromUtf8("tbtNUM_6"));

        gridLayout->addWidget(tbtNUM_6, 2, 2, 1, 1);

        tbtNUM_7 = new QToolButton(fraPIN_Keypad);
        tbtNUM_7->setObjectName(QString::fromUtf8("tbtNUM_7"));

        gridLayout->addWidget(tbtNUM_7, 3, 0, 1, 1);

        tbtNUM_8 = new QToolButton(fraPIN_Keypad);
        tbtNUM_8->setObjectName(QString::fromUtf8("tbtNUM_8"));

        gridLayout->addWidget(tbtNUM_8, 3, 1, 1, 1);

        tbtNUM_9 = new QToolButton(fraPIN_Keypad);
        tbtNUM_9->setObjectName(QString::fromUtf8("tbtNUM_9"));

        gridLayout->addWidget(tbtNUM_9, 3, 2, 1, 1);

        tbtNUM_0 = new QToolButton(fraPIN_Keypad);
        tbtNUM_0->setObjectName(QString::fromUtf8("tbtNUM_0"));

        gridLayout->addWidget(tbtNUM_0, 4, 1, 1, 1);

        tbtClear = new QToolButton(fraPIN_Keypad);
        tbtClear->setObjectName(QString::fromUtf8("tbtClear"));

        gridLayout->addWidget(tbtClear, 4, 2, 1, 1);


        vboxLayout->addWidget(fraPIN_Keypad);

        gridLayout1 = new QGridLayout();
        gridLayout1->setSpacing(6);
        gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
        lblIcon = new QLabel(dlgWndAskPINClass);
        lblIcon->setObjectName(QString::fromUtf8("lblIcon"));

        gridLayout1->addWidget(lblIcon, 0, 0, 2, 1);

        lblHeader = new QLabel(dlgWndAskPINClass);
        lblHeader->setObjectName(QString::fromUtf8("lblHeader"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(lblHeader->sizePolicy().hasHeightForWidth());
        lblHeader->setSizePolicy(sizePolicy);
        lblHeader->setWordWrap(true);

        gridLayout1->addWidget(lblHeader, 0, 1, 1, 1);

        spacerItem = new QSpacerItem(271, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout1->addItem(spacerItem, 1, 1, 1, 1);


        vboxLayout->addLayout(gridLayout1);

        fraPIN_Normal = new QFrame(dlgWndAskPINClass);
        fraPIN_Normal->setObjectName(QString::fromUtf8("fraPIN_Normal"));
        fraPIN_Normal->setFrameShape(QFrame::NoFrame);
        fraPIN_Normal->setFrameShadow(QFrame::Plain);
        gridLayout2 = new QGridLayout(fraPIN_Normal);
        gridLayout2->setSpacing(6);
        gridLayout2->setContentsMargins(11, 11, 11, 11);
        gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
        spacerItem1 = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout2->addItem(spacerItem1, 0, 1, 1, 1);

        spacerItem2 = new QSpacerItem(10, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

        gridLayout2->addItem(spacerItem2, 2, 0, 1, 1);

        lblPINName = new QLabel(fraPIN_Normal);
        lblPINName->setObjectName(QString::fromUtf8("lblPINName"));
        lblPINName->setLayoutDirection(Qt::RightToLeft);

        gridLayout2->addWidget(lblPINName, 2, 1, 1, 1);

        txtPIN = new QLineEdit(fraPIN_Normal);
        txtPIN->setObjectName(QString::fromUtf8("txtPIN"));
        txtPIN->setEchoMode(QLineEdit::Password);

        gridLayout2->addWidget(txtPIN, 2, 2, 1, 1);

        spacerItem3 = new QSpacerItem(10, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

        gridLayout2->addItem(spacerItem3, 2, 3, 1, 1);

        spacerItem4 = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout2->addItem(spacerItem4, 3, 1, 1, 1);


        vboxLayout->addWidget(fraPIN_Normal);

        hboxLayout1 = new QHBoxLayout();
        hboxLayout1->setSpacing(6);
        hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
        spacerItem5 = new QSpacerItem(261, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout1->addItem(spacerItem5);

        btnOk = new QPushButton(dlgWndAskPINClass);
        btnOk->setObjectName(QString::fromUtf8("btnOk"));
        btnOk->setEnabled(false);

        hboxLayout1->addWidget(btnOk);

        btnCancel = new QPushButton(dlgWndAskPINClass);
        btnCancel->setObjectName(QString::fromUtf8("btnCancel"));

        hboxLayout1->addWidget(btnCancel);


        vboxLayout->addLayout(hboxLayout1);


        retranslateUi(dlgWndAskPINClass);
        QObject::connect(btnOk, SIGNAL(clicked()), dlgWndAskPINClass, SLOT(accept()));
        QObject::connect(btnCancel, SIGNAL(clicked()), dlgWndAskPINClass, SLOT(reject()));

        QMetaObject::connectSlotsByName(dlgWndAskPINClass);
    } // setupUi

    void retranslateUi(QDialog *dlgWndAskPINClass)
    {
        dlgWndAskPINClass->setWindowTitle(QApplication::translate("dlgWndAskPINClass", "dlgWndAskPIN", 0, QApplication::UnicodeUTF8));
        lblPINName_2->setText(QApplication::translate("dlgWndAskPINClass", "PIN", 0, QApplication::UnicodeUTF8));
        tbtNUM_1->setText(QApplication::translate("dlgWndAskPINClass", "&1", 0, QApplication::UnicodeUTF8));
        tbtNUM_2->setText(QApplication::translate("dlgWndAskPINClass", "&2", 0, QApplication::UnicodeUTF8));
        tbtNUM_3->setText(QApplication::translate("dlgWndAskPINClass", "&3", 0, QApplication::UnicodeUTF8));
        tbtNUM_4->setText(QApplication::translate("dlgWndAskPINClass", "&4", 0, QApplication::UnicodeUTF8));
        tbtNUM_5->setText(QApplication::translate("dlgWndAskPINClass", "&5", 0, QApplication::UnicodeUTF8));
        tbtNUM_6->setText(QApplication::translate("dlgWndAskPINClass", "&6", 0, QApplication::UnicodeUTF8));
        tbtNUM_7->setText(QApplication::translate("dlgWndAskPINClass", "&7", 0, QApplication::UnicodeUTF8));
        tbtNUM_8->setText(QApplication::translate("dlgWndAskPINClass", "&8", 0, QApplication::UnicodeUTF8));
        tbtNUM_9->setText(QApplication::translate("dlgWndAskPINClass", "&9", 0, QApplication::UnicodeUTF8));
        tbtNUM_0->setText(QApplication::translate("dlgWndAskPINClass", "&0", 0, QApplication::UnicodeUTF8));
        tbtClear->setText(QApplication::translate("dlgWndAskPINClass", "C&E", 0, QApplication::UnicodeUTF8));
        lblIcon->setText(QString());
        lblHeader->setText(QApplication::translate("dlgWndAskPINClass", "Enter you PIN", 0, QApplication::UnicodeUTF8));
        lblPINName->setText(QApplication::translate("dlgWndAskPINClass", "PIN", 0, QApplication::UnicodeUTF8));
        btnOk->setText(QApplication::translate("dlgWndAskPINClass", "&Ok", 0, QApplication::UnicodeUTF8));
        btnCancel->setText(QApplication::translate("dlgWndAskPINClass", "&Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class dlgWndAskPINClass: public Ui_dlgWndAskPINClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGWNDASKPIN_H
