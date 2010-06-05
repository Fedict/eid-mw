/********************************************************************************
** Form generated from reading UI file 'dlgwndmodal.ui'
**
** Created: Sat Jun 5 12:34:55 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLGWNDMODAL_H
#define UI_DLGWNDMODAL_H

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

class Ui_dlgWndModal
{
public:
    QGridLayout *gridLayout;
    QLabel *lblIcon;
    QLabel *lblMessage;
    QSpacerItem *spacerItem;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem1;
    QPushButton *btnOk;
    QPushButton *btnYes;
    QPushButton *btnRetry;
    QPushButton *btnCancel;
    QPushButton *btnNo;

    void setupUi(QDialog *dlgWndModal)
    {
        if (dlgWndModal->objectName().isEmpty())
            dlgWndModal->setObjectName(QString::fromUtf8("dlgWndModal"));
        dlgWndModal->resize(425, 183);
        dlgWndModal->setModal(true);
        gridLayout = new QGridLayout(dlgWndModal);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        lblIcon = new QLabel(dlgWndModal);
        lblIcon->setObjectName(QString::fromUtf8("lblIcon"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(lblIcon->sizePolicy().hasHeightForWidth());
        lblIcon->setSizePolicy(sizePolicy);

        gridLayout->addWidget(lblIcon, 0, 0, 1, 1);

        lblMessage = new QLabel(dlgWndModal);
        lblMessage->setObjectName(QString::fromUtf8("lblMessage"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(5);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(lblMessage->sizePolicy().hasHeightForWidth());
        lblMessage->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(lblMessage, 0, 1, 2, 1);

        spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(spacerItem, 1, 0, 1, 1);

        hboxLayout = new QHBoxLayout();
        hboxLayout->setSpacing(6);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem1);

        btnOk = new QPushButton(dlgWndModal);
        btnOk->setObjectName(QString::fromUtf8("btnOk"));

        hboxLayout->addWidget(btnOk);

        btnYes = new QPushButton(dlgWndModal);
        btnYes->setObjectName(QString::fromUtf8("btnYes"));

        hboxLayout->addWidget(btnYes);

        btnRetry = new QPushButton(dlgWndModal);
        btnRetry->setObjectName(QString::fromUtf8("btnRetry"));

        hboxLayout->addWidget(btnRetry);

        btnCancel = new QPushButton(dlgWndModal);
        btnCancel->setObjectName(QString::fromUtf8("btnCancel"));

        hboxLayout->addWidget(btnCancel);

        btnNo = new QPushButton(dlgWndModal);
        btnNo->setObjectName(QString::fromUtf8("btnNo"));

        hboxLayout->addWidget(btnNo);


        gridLayout->addLayout(hboxLayout, 2, 0, 1, 2);


        retranslateUi(dlgWndModal);

        QMetaObject::connectSlotsByName(dlgWndModal);
    } // setupUi

    void retranslateUi(QDialog *dlgWndModal)
    {
        dlgWndModal->setWindowTitle(QApplication::translate("dlgWndModal", "dlgWndModal", 0, QApplication::UnicodeUTF8));
        lblIcon->setText(QApplication::translate("dlgWndModal", "Icon", 0, QApplication::UnicodeUTF8));
        lblMessage->setText(QApplication::translate("dlgWndModal", "-----> Message here <-----", 0, QApplication::UnicodeUTF8));
        btnOk->setText(QApplication::translate("dlgWndModal", "&Ok", 0, QApplication::UnicodeUTF8));
        btnYes->setText(QApplication::translate("dlgWndModal", "&Yes", 0, QApplication::UnicodeUTF8));
        btnRetry->setText(QApplication::translate("dlgWndModal", "&Retry", 0, QApplication::UnicodeUTF8));
        btnCancel->setText(QApplication::translate("dlgWndModal", "&Cancel", 0, QApplication::UnicodeUTF8));
        btnNo->setText(QApplication::translate("dlgWndModal", "&No", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class dlgWndModal: public Ui_dlgWndModal {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGWNDMODAL_H
