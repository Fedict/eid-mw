/********************************************************************************
** Form generated from reading UI file 'dlgwndbadpin.ui'
**
** Created: Sat Jun 5 12:34:55 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLGWNDBADPIN_H
#define UI_DLGWNDBADPIN_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_dlgWndBadPIN
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *hboxLayout;
    QLabel *lblIcon;
    QLabel *lblHeader;
    QSpacerItem *spacerItem;
    QLabel *lblCenter;
    QSpacerItem *spacerItem1;
    QHBoxLayout *hboxLayout1;
    QSpacerItem *spacerItem2;
    QPushButton *btnOk;
    QSpacerItem *spacerItem3;
    QPushButton *btnRetry;
    QPushButton *btnCancel;

    void setupUi(QDialog *dlgWndBadPIN)
    {
        if (dlgWndBadPIN->objectName().isEmpty())
            dlgWndBadPIN->setObjectName(QString::fromUtf8("dlgWndBadPIN"));
        dlgWndBadPIN->resize(344, 204);
        gridLayout = new QGridLayout(dlgWndBadPIN);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        hboxLayout = new QHBoxLayout();
        hboxLayout->setSpacing(6);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        lblIcon = new QLabel(dlgWndBadPIN);
        lblIcon->setObjectName(QString::fromUtf8("lblIcon"));

        hboxLayout->addWidget(lblIcon);

        lblHeader = new QLabel(dlgWndBadPIN);
        lblHeader->setObjectName(QString::fromUtf8("lblHeader"));

        hboxLayout->addWidget(lblHeader);


        gridLayout->addLayout(hboxLayout, 0, 0, 1, 1);

        spacerItem = new QSpacerItem(336, 16, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(spacerItem, 1, 0, 1, 1);

        lblCenter = new QLabel(dlgWndBadPIN);
        lblCenter->setObjectName(QString::fromUtf8("lblCenter"));
        lblCenter->setAlignment(Qt::AlignCenter|Qt::AlignHCenter|Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout->addWidget(lblCenter, 2, 0, 1, 1);

        spacerItem1 = new QSpacerItem(336, 16, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(spacerItem1, 3, 0, 1, 1);

        hboxLayout1 = new QHBoxLayout();
        hboxLayout1->setSpacing(6);
        hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
        spacerItem2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout1->addItem(spacerItem2);

        btnOk = new QPushButton(dlgWndBadPIN);
        btnOk->setObjectName(QString::fromUtf8("btnOk"));

        hboxLayout1->addWidget(btnOk);

        spacerItem3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout1->addItem(spacerItem3);

        btnRetry = new QPushButton(dlgWndBadPIN);
        btnRetry->setObjectName(QString::fromUtf8("btnRetry"));

        hboxLayout1->addWidget(btnRetry);

        btnCancel = new QPushButton(dlgWndBadPIN);
        btnCancel->setObjectName(QString::fromUtf8("btnCancel"));

        hboxLayout1->addWidget(btnCancel);


        gridLayout->addLayout(hboxLayout1, 4, 0, 1, 1);


        retranslateUi(dlgWndBadPIN);
        QObject::connect(btnOk, SIGNAL(clicked()), dlgWndBadPIN, SLOT(accept()));
        QObject::connect(btnRetry, SIGNAL(clicked()), dlgWndBadPIN, SLOT(accept()));
        QObject::connect(btnCancel, SIGNAL(clicked()), dlgWndBadPIN, SLOT(reject()));

        QMetaObject::connectSlotsByName(dlgWndBadPIN);
    } // setupUi

    void retranslateUi(QDialog *dlgWndBadPIN)
    {
        dlgWndBadPIN->setWindowTitle(QApplication::translate("dlgWndBadPIN", "dlgWndBadPIN", 0, QApplication::UnicodeUTF8));
        lblIcon->setText(QString());
        lblHeader->setText(QApplication::translate("dlgWndBadPIN", "3 Remaining tries", 0, QApplication::UnicodeUTF8));
        lblCenter->setText(QApplication::translate("dlgWndBadPIN", "Try again or cancel?", 0, QApplication::UnicodeUTF8));
        btnOk->setText(QApplication::translate("dlgWndBadPIN", "&Ok", 0, QApplication::UnicodeUTF8));
        btnRetry->setText(QApplication::translate("dlgWndBadPIN", "&Retry", 0, QApplication::UnicodeUTF8));
        btnCancel->setText(QApplication::translate("dlgWndBadPIN", "&Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class dlgWndBadPIN: public Ui_dlgWndBadPIN {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGWNDBADPIN_H
