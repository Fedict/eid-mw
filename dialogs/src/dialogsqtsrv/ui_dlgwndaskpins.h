/********************************************************************************
** Form generated from reading UI file 'dlgwndaskpins.ui'
**
** Created: Sat Jun 5 12:34:55 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLGWNDASKPINS_H
#define UI_DLGWNDASKPINS_H

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

QT_BEGIN_NAMESPACE

class Ui_dlgWndAskPINsClass
{
public:
    QGridLayout *gridLayout;
    QFrame *fraPIN_Keypad;
    QGridLayout *gridLayout1;
    QHBoxLayout *hboxLayout;
    QLabel *lblPINName_2;
    QLineEdit *txtPIN_Keypad;
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
    QHBoxLayout *hboxLayout1;
    QLabel *lblIcon;
    QLabel *lblHeader;
    QFrame *fraPIN_Normal;
    QGridLayout *gridLayout2;
    QSpacerItem *spacerItem;
    QSpacerItem *spacerItem1;
    QLabel *lblOldPINName;
    QLineEdit *txtOldPIN;
    QSpacerItem *spacerItem2;
    QLabel *lblNewPIN1;
    QLineEdit *txtNewPIN1;
    QLabel *lblNewPIN2;
    QLineEdit *txtNewPIN2;
    QSpacerItem *spacerItem3;
    QLabel *lblError;
    QHBoxLayout *hboxLayout2;
    QSpacerItem *spacerItem4;
    QPushButton *btnOk;
    QPushButton *btnCancel;

    void setupUi(QDialog *dlgWndAskPINsClass)
    {
        if (dlgWndAskPINsClass->objectName().isEmpty())
            dlgWndAskPINsClass->setObjectName(QString::fromUtf8("dlgWndAskPINsClass"));
        dlgWndAskPINsClass->resize(348, 381);
        gridLayout = new QGridLayout(dlgWndAskPINsClass);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        fraPIN_Keypad = new QFrame(dlgWndAskPINsClass);
        fraPIN_Keypad->setObjectName(QString::fromUtf8("fraPIN_Keypad"));
        fraPIN_Keypad->setFrameShape(QFrame::StyledPanel);
        fraPIN_Keypad->setFrameShadow(QFrame::Raised);
        gridLayout1 = new QGridLayout(fraPIN_Keypad);
        gridLayout1->setSpacing(6);
        gridLayout1->setContentsMargins(11, 11, 11, 11);
        gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
        hboxLayout = new QHBoxLayout();
        hboxLayout->setSpacing(6);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        lblPINName_2 = new QLabel(fraPIN_Keypad);
        lblPINName_2->setObjectName(QString::fromUtf8("lblPINName_2"));
        lblPINName_2->setLayoutDirection(Qt::RightToLeft);

        hboxLayout->addWidget(lblPINName_2);

        txtPIN_Keypad = new QLineEdit(fraPIN_Keypad);
        txtPIN_Keypad->setObjectName(QString::fromUtf8("txtPIN_Keypad"));
        txtPIN_Keypad->setEchoMode(QLineEdit::Password);

        hboxLayout->addWidget(txtPIN_Keypad);


        gridLayout1->addLayout(hboxLayout, 0, 0, 1, 3);

        tbtNUM_1 = new QToolButton(fraPIN_Keypad);
        tbtNUM_1->setObjectName(QString::fromUtf8("tbtNUM_1"));
        tbtNUM_1->setPopupMode(QToolButton::DelayedPopup);
        tbtNUM_1->setToolButtonStyle(Qt::ToolButtonIconOnly);

        gridLayout1->addWidget(tbtNUM_1, 1, 0, 1, 1);

        tbtNUM_2 = new QToolButton(fraPIN_Keypad);
        tbtNUM_2->setObjectName(QString::fromUtf8("tbtNUM_2"));

        gridLayout1->addWidget(tbtNUM_2, 1, 1, 1, 1);

        tbtNUM_3 = new QToolButton(fraPIN_Keypad);
        tbtNUM_3->setObjectName(QString::fromUtf8("tbtNUM_3"));

        gridLayout1->addWidget(tbtNUM_3, 1, 2, 1, 1);

        tbtNUM_4 = new QToolButton(fraPIN_Keypad);
        tbtNUM_4->setObjectName(QString::fromUtf8("tbtNUM_4"));

        gridLayout1->addWidget(tbtNUM_4, 2, 0, 1, 1);

        tbtNUM_5 = new QToolButton(fraPIN_Keypad);
        tbtNUM_5->setObjectName(QString::fromUtf8("tbtNUM_5"));

        gridLayout1->addWidget(tbtNUM_5, 2, 1, 1, 1);

        tbtNUM_6 = new QToolButton(fraPIN_Keypad);
        tbtNUM_6->setObjectName(QString::fromUtf8("tbtNUM_6"));

        gridLayout1->addWidget(tbtNUM_6, 2, 2, 1, 1);

        tbtNUM_7 = new QToolButton(fraPIN_Keypad);
        tbtNUM_7->setObjectName(QString::fromUtf8("tbtNUM_7"));

        gridLayout1->addWidget(tbtNUM_7, 3, 0, 1, 1);

        tbtNUM_8 = new QToolButton(fraPIN_Keypad);
        tbtNUM_8->setObjectName(QString::fromUtf8("tbtNUM_8"));

        gridLayout1->addWidget(tbtNUM_8, 3, 1, 1, 1);

        tbtNUM_9 = new QToolButton(fraPIN_Keypad);
        tbtNUM_9->setObjectName(QString::fromUtf8("tbtNUM_9"));

        gridLayout1->addWidget(tbtNUM_9, 3, 2, 1, 1);

        tbtNUM_0 = new QToolButton(fraPIN_Keypad);
        tbtNUM_0->setObjectName(QString::fromUtf8("tbtNUM_0"));

        gridLayout1->addWidget(tbtNUM_0, 4, 1, 1, 1);

        tbtClear = new QToolButton(fraPIN_Keypad);
        tbtClear->setObjectName(QString::fromUtf8("tbtClear"));

        gridLayout1->addWidget(tbtClear, 4, 2, 1, 1);


        gridLayout->addWidget(fraPIN_Keypad, 0, 0, 1, 1);

        hboxLayout1 = new QHBoxLayout();
        hboxLayout1->setSpacing(6);
        hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
        lblIcon = new QLabel(dlgWndAskPINsClass);
        lblIcon->setObjectName(QString::fromUtf8("lblIcon"));

        hboxLayout1->addWidget(lblIcon);

        lblHeader = new QLabel(dlgWndAskPINsClass);
        lblHeader->setObjectName(QString::fromUtf8("lblHeader"));
        lblHeader->setWordWrap(true);

        hboxLayout1->addWidget(lblHeader);


        gridLayout->addLayout(hboxLayout1, 1, 0, 1, 1);

        fraPIN_Normal = new QFrame(dlgWndAskPINsClass);
        fraPIN_Normal->setObjectName(QString::fromUtf8("fraPIN_Normal"));
        fraPIN_Normal->setFrameShape(QFrame::NoFrame);
        fraPIN_Normal->setFrameShadow(QFrame::Plain);
        gridLayout2 = new QGridLayout(fraPIN_Normal);
        gridLayout2->setSpacing(6);
        gridLayout2->setContentsMargins(11, 11, 11, 11);
        gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
        spacerItem = new QSpacerItem(20, 16, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout2->addItem(spacerItem, 0, 1, 1, 1);

        spacerItem1 = new QSpacerItem(10, 71, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

        gridLayout2->addItem(spacerItem1, 1, 0, 3, 1);

        lblOldPINName = new QLabel(fraPIN_Normal);
        lblOldPINName->setObjectName(QString::fromUtf8("lblOldPINName"));
        lblOldPINName->setLayoutDirection(Qt::RightToLeft);
        lblOldPINName->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout2->addWidget(lblOldPINName, 1, 1, 1, 1);

        txtOldPIN = new QLineEdit(fraPIN_Normal);
        txtOldPIN->setObjectName(QString::fromUtf8("txtOldPIN"));
        txtOldPIN->setEchoMode(QLineEdit::Password);

        gridLayout2->addWidget(txtOldPIN, 1, 2, 1, 1);

        spacerItem2 = new QSpacerItem(10, 71, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

        gridLayout2->addItem(spacerItem2, 1, 3, 3, 1);

        lblNewPIN1 = new QLabel(fraPIN_Normal);
        lblNewPIN1->setObjectName(QString::fromUtf8("lblNewPIN1"));
        lblNewPIN1->setLayoutDirection(Qt::RightToLeft);
        lblNewPIN1->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout2->addWidget(lblNewPIN1, 2, 1, 1, 1);

        txtNewPIN1 = new QLineEdit(fraPIN_Normal);
        txtNewPIN1->setObjectName(QString::fromUtf8("txtNewPIN1"));
        txtNewPIN1->setEchoMode(QLineEdit::Password);

        gridLayout2->addWidget(txtNewPIN1, 2, 2, 1, 1);

        lblNewPIN2 = new QLabel(fraPIN_Normal);
        lblNewPIN2->setObjectName(QString::fromUtf8("lblNewPIN2"));
        lblNewPIN2->setLayoutDirection(Qt::RightToLeft);
        lblNewPIN2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout2->addWidget(lblNewPIN2, 3, 1, 1, 1);

        txtNewPIN2 = new QLineEdit(fraPIN_Normal);
        txtNewPIN2->setObjectName(QString::fromUtf8("txtNewPIN2"));
        txtNewPIN2->setEchoMode(QLineEdit::Password);

        gridLayout2->addWidget(txtNewPIN2, 3, 2, 1, 1);

        spacerItem3 = new QSpacerItem(20, 16, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout2->addItem(spacerItem3, 4, 1, 1, 1);


        gridLayout->addWidget(fraPIN_Normal, 2, 0, 1, 1);

        lblError = new QLabel(dlgWndAskPINsClass);
        lblError->setObjectName(QString::fromUtf8("lblError"));
        QPalette palette;
        QBrush brush(QColor(255, 0, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        QBrush brush1(QColor(106, 104, 100, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        lblError->setPalette(palette);

        gridLayout->addWidget(lblError, 3, 0, 1, 1);

        hboxLayout2 = new QHBoxLayout();
        hboxLayout2->setSpacing(6);
        hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
        spacerItem4 = new QSpacerItem(161, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout2->addItem(spacerItem4);

        btnOk = new QPushButton(dlgWndAskPINsClass);
        btnOk->setObjectName(QString::fromUtf8("btnOk"));
        btnOk->setEnabled(false);

        hboxLayout2->addWidget(btnOk);

        btnCancel = new QPushButton(dlgWndAskPINsClass);
        btnCancel->setObjectName(QString::fromUtf8("btnCancel"));

        hboxLayout2->addWidget(btnCancel);


        gridLayout->addLayout(hboxLayout2, 4, 0, 1, 1);


        retranslateUi(dlgWndAskPINsClass);
        QObject::connect(btnCancel, SIGNAL(clicked()), dlgWndAskPINsClass, SLOT(reject()));

        QMetaObject::connectSlotsByName(dlgWndAskPINsClass);
    } // setupUi

    void retranslateUi(QDialog *dlgWndAskPINsClass)
    {
        dlgWndAskPINsClass->setWindowTitle(QApplication::translate("dlgWndAskPINsClass", "dlgWndAskPINs", 0, QApplication::UnicodeUTF8));
        lblPINName_2->setText(QApplication::translate("dlgWndAskPINsClass", "PIN", 0, QApplication::UnicodeUTF8));
        tbtNUM_1->setText(QApplication::translate("dlgWndAskPINsClass", "&1", 0, QApplication::UnicodeUTF8));
        tbtNUM_2->setText(QApplication::translate("dlgWndAskPINsClass", "&2", 0, QApplication::UnicodeUTF8));
        tbtNUM_3->setText(QApplication::translate("dlgWndAskPINsClass", "&3", 0, QApplication::UnicodeUTF8));
        tbtNUM_4->setText(QApplication::translate("dlgWndAskPINsClass", "&4", 0, QApplication::UnicodeUTF8));
        tbtNUM_5->setText(QApplication::translate("dlgWndAskPINsClass", "&5", 0, QApplication::UnicodeUTF8));
        tbtNUM_6->setText(QApplication::translate("dlgWndAskPINsClass", "&6", 0, QApplication::UnicodeUTF8));
        tbtNUM_7->setText(QApplication::translate("dlgWndAskPINsClass", "&7", 0, QApplication::UnicodeUTF8));
        tbtNUM_8->setText(QApplication::translate("dlgWndAskPINsClass", "&8", 0, QApplication::UnicodeUTF8));
        tbtNUM_9->setText(QApplication::translate("dlgWndAskPINsClass", "&9", 0, QApplication::UnicodeUTF8));
        tbtNUM_0->setText(QApplication::translate("dlgWndAskPINsClass", "&0", 0, QApplication::UnicodeUTF8));
        tbtClear->setText(QApplication::translate("dlgWndAskPINsClass", "C&E", 0, QApplication::UnicodeUTF8));
        lblIcon->setText(QString());
        lblHeader->setText(QApplication::translate("dlgWndAskPINsClass", "Enter you PIN", 0, QApplication::UnicodeUTF8));
        lblOldPINName->setText(QApplication::translate("dlgWndAskPINsClass", "Current PIN", 0, QApplication::UnicodeUTF8));
        lblNewPIN1->setText(QApplication::translate("dlgWndAskPINsClass", "New PIN", 0, QApplication::UnicodeUTF8));
        lblNewPIN2->setText(QApplication::translate("dlgWndAskPINsClass", "Confirm new PIN", 0, QApplication::UnicodeUTF8));
        lblError->setText(QApplication::translate("dlgWndAskPINsClass", "Error: ", 0, QApplication::UnicodeUTF8));
        btnOk->setText(QApplication::translate("dlgWndAskPINsClass", "&Ok", 0, QApplication::UnicodeUTF8));
        btnCancel->setText(QApplication::translate("dlgWndAskPINsClass", "&Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class dlgWndAskPINsClass: public Ui_dlgWndAskPINsClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGWNDASKPINS_H
