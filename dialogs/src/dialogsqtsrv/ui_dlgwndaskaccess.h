/********************************************************************************
** Form generated from reading UI file 'dlgwndaskaccess.ui'
**
** Created: Fri Jun 4 17:32:05 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLGWNDASKACCESS_H
#define UI_DLGWNDASKACCESS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_dlgWndAskAccess
{
public:
    QGridLayout *gridLayout;
    QLabel *lblIcon;
    QLabel *lblMessage_1;
    QSpacerItem *spacerItem;
    QLabel *lblMessage_2;
    QLabel *lblMessage_3;
    QSpacerItem *spacerItem1;
    QSpacerItem *spacerItem2;
    QLabel *lblMessage_4;
    QLabel *lblMessage_5;
    QSpacerItem *spacerItem3;
    QLabel *lblMessage_6;
    QCheckBox *chkForAll;
    QHBoxLayout *hboxLayout;
    QPushButton *btnYes;
    QPushButton *btnNo;
    QPushButton *btnAlways;
    QPushButton *btnNever;

    void setupUi(QDialog *dlgWndAskAccess)
    {
        if (dlgWndAskAccess->objectName().isEmpty())
            dlgWndAskAccess->setObjectName(QString::fromUtf8("dlgWndAskAccess"));
        dlgWndAskAccess->resize(425, 256);
        dlgWndAskAccess->setModal(true);
        gridLayout = new QGridLayout(dlgWndAskAccess);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        lblIcon = new QLabel(dlgWndAskAccess);
        lblIcon->setObjectName(QString::fromUtf8("lblIcon"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(lblIcon->sizePolicy().hasHeightForWidth());
        lblIcon->setSizePolicy(sizePolicy);

        gridLayout->addWidget(lblIcon, 0, 0, 4, 1);

        lblMessage_1 = new QLabel(dlgWndAskAccess);
        lblMessage_1->setObjectName(QString::fromUtf8("lblMessage_1"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(5);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(lblMessage_1->sizePolicy().hasHeightForWidth());
        lblMessage_1->setSizePolicy(sizePolicy1);
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        lblMessage_1->setFont(font);

        gridLayout->addWidget(lblMessage_1, 0, 1, 1, 1);

        spacerItem = new QSpacerItem(20, 16, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(spacerItem, 1, 1, 1, 1);

        lblMessage_2 = new QLabel(dlgWndAskAccess);
        lblMessage_2->setObjectName(QString::fromUtf8("lblMessage_2"));
        sizePolicy1.setHeightForWidth(lblMessage_2->sizePolicy().hasHeightForWidth());
        lblMessage_2->setSizePolicy(sizePolicy1);
        QFont font1;
        font1.setUnderline(true);
        lblMessage_2->setFont(font1);

        gridLayout->addWidget(lblMessage_2, 2, 1, 1, 1);

        lblMessage_3 = new QLabel(dlgWndAskAccess);
        lblMessage_3->setObjectName(QString::fromUtf8("lblMessage_3"));
        sizePolicy1.setHeightForWidth(lblMessage_3->sizePolicy().hasHeightForWidth());
        lblMessage_3->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(lblMessage_3, 3, 1, 2, 1);

        spacerItem1 = new QSpacerItem(20, 141, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(spacerItem1, 4, 0, 7, 1);

        spacerItem2 = new QSpacerItem(20, 16, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(spacerItem2, 5, 1, 1, 1);

        lblMessage_4 = new QLabel(dlgWndAskAccess);
        lblMessage_4->setObjectName(QString::fromUtf8("lblMessage_4"));
        sizePolicy1.setHeightForWidth(lblMessage_4->sizePolicy().hasHeightForWidth());
        lblMessage_4->setSizePolicy(sizePolicy1);
        lblMessage_4->setFont(font1);

        gridLayout->addWidget(lblMessage_4, 6, 1, 1, 1);

        lblMessage_5 = new QLabel(dlgWndAskAccess);
        lblMessage_5->setObjectName(QString::fromUtf8("lblMessage_5"));
        sizePolicy1.setHeightForWidth(lblMessage_5->sizePolicy().hasHeightForWidth());
        lblMessage_5->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(lblMessage_5, 7, 1, 1, 1);

        spacerItem3 = new QSpacerItem(20, 16, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(spacerItem3, 8, 1, 1, 1);

        lblMessage_6 = new QLabel(dlgWndAskAccess);
        lblMessage_6->setObjectName(QString::fromUtf8("lblMessage_6"));
        sizePolicy1.setHeightForWidth(lblMessage_6->sizePolicy().hasHeightForWidth());
        lblMessage_6->setSizePolicy(sizePolicy1);
        lblMessage_6->setFont(font);

        gridLayout->addWidget(lblMessage_6, 9, 1, 1, 1);

        chkForAll = new QCheckBox(dlgWndAskAccess);
        chkForAll->setObjectName(QString::fromUtf8("chkForAll"));

        gridLayout->addWidget(chkForAll, 10, 1, 1, 1);

        hboxLayout = new QHBoxLayout();
        hboxLayout->setSpacing(6);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        btnYes = new QPushButton(dlgWndAskAccess);
        btnYes->setObjectName(QString::fromUtf8("btnYes"));

        hboxLayout->addWidget(btnYes);

        btnNo = new QPushButton(dlgWndAskAccess);
        btnNo->setObjectName(QString::fromUtf8("btnNo"));

        hboxLayout->addWidget(btnNo);

        btnAlways = new QPushButton(dlgWndAskAccess);
        btnAlways->setObjectName(QString::fromUtf8("btnAlways"));

        hboxLayout->addWidget(btnAlways);

        btnNever = new QPushButton(dlgWndAskAccess);
        btnNever->setObjectName(QString::fromUtf8("btnNever"));

        hboxLayout->addWidget(btnNever);


        gridLayout->addLayout(hboxLayout, 11, 0, 1, 2);


        retranslateUi(dlgWndAskAccess);

        QMetaObject::connectSlotsByName(dlgWndAskAccess);
    } // setupUi

    void retranslateUi(QDialog *dlgWndAskAccess)
    {
        dlgWndAskAccess->setWindowTitle(QApplication::translate("dlgWndAskAccess", "dlgWndModal", 0, QApplication::UnicodeUTF8));
        lblIcon->setText(QApplication::translate("dlgWndAskAccess", "Icon", 0, QApplication::UnicodeUTF8));
        lblMessage_1->setText(QApplication::translate("dlgWndAskAccess", "-----> Message here <-----", 0, QApplication::UnicodeUTF8));
        lblMessage_2->setText(QApplication::translate("dlgWndAskAccess", "-----> Message here <-----", 0, QApplication::UnicodeUTF8));
        lblMessage_3->setText(QApplication::translate("dlgWndAskAccess", "-----> Message here <-----", 0, QApplication::UnicodeUTF8));
        lblMessage_4->setText(QApplication::translate("dlgWndAskAccess", "-----> Message here <-----", 0, QApplication::UnicodeUTF8));
        lblMessage_5->setText(QApplication::translate("dlgWndAskAccess", "-----> Message here <-----", 0, QApplication::UnicodeUTF8));
        lblMessage_6->setText(QApplication::translate("dlgWndAskAccess", "-----> Message here <-----", 0, QApplication::UnicodeUTF8));
        chkForAll->setText(QApplication::translate("dlgWndAskAccess", "For all operations", 0, QApplication::UnicodeUTF8));
        btnYes->setText(QApplication::translate("dlgWndAskAccess", "&Yes", 0, QApplication::UnicodeUTF8));
        btnNo->setText(QApplication::translate("dlgWndAskAccess", "&Cancel (No)", 0, QApplication::UnicodeUTF8));
        btnAlways->setText(QApplication::translate("dlgWndAskAccess", "&Always", 0, QApplication::UnicodeUTF8));
        btnNever->setText(QApplication::translate("dlgWndAskAccess", "&Never", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class dlgWndAskAccess: public Ui_dlgWndAskAccess {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGWNDASKACCESS_H
