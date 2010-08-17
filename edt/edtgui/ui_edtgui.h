/********************************************************************************
** Form generated from reading UI file 'edtgui.ui'
**
** Created: Fri Apr 30 09:37:05 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EDTGUI_H
#define UI_EDTGUI_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QStackedWidget>
#include <QtGui/QStatusBar>
#include <QtGui/QTableWidget>
#include <QtGui/QTextEdit>
#include <QtGui/QToolBox>
#include <QtGui/QToolButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_eDTGuiClass
{
public:
    QWidget *centralWidget;
    QStackedWidget *stackedWidget;
    QWidget *pg_presentation;
    QPushButton *pb_Analyse_nl;
    QPushButton *pb_Analyse_fr;
    QLabel *lbl_Presentation;
    QLabel *lbl_title;
    QLabel *lbl_title_2;
    QLabel *lbl_version;
    QWidget *pg_information;
    QPushButton *pb_Continue_2;
    QLabel *lbl_instructions;
    QWidget *pg_progress;
    QProgressBar *progressBar_Overall;
    QProgressBar *progressBar_Test;
    QPushButton *pb_Cancel;
    QPushButton *pb_Continue;
    QLabel *label;
    QLabel *lbl_testName;
    QLabel *lbl_testNameHead;
    QWidget *pg_signing;
    QPushButton *pb_testAuthentication;
    QPushButton *pb_Continue_3;
    QLabel *label_2;
    QLabel *lbl_signing;
    QProgressBar *progressBar_signing;
    QLabel *lbl_testSignature;
    QWidget *pg_Summary;
    QLabel *lbl_Status;
    QPushButton *pb_SaveReport;
    QPushButton *pb_Details;
    QPushButton *pb_Diag;
    QTableWidget *tbl_Summary;
    QWidget *pg_Details;
    QTextEdit *te_Report;
    QLabel *lbl_Status_2;
    QPushButton *pb_SaveReport_2;
    QPushButton *pb_Summary;
    QPushButton *pb_Diag2;
    QWidget *pg_diagnostics;
    QLabel *label_did_it_solve;
    QPushButton *pb_needhelp;
    QPushButton *pb_solved;
    QPushButton *pb_not_solved;
    QPushButton *pb_Summary_2;
    QPushButton *pb_Quit;
    QPushButton *pb_SaveSend;
    QToolBox *toolBox;
    QWidget *page;
    QFrame *frame_3;
    QLabel *lbl_details;
    QWidget *suggestion;
    QFrame *frame;
    QLabel *lbl_remedy;
    QLabel *lbl_which_diag;
    QToolButton *pb_next;
    QToolButton *pb_previous;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *eDTGuiClass)
    {
        if (eDTGuiClass->objectName().isEmpty())
            eDTGuiClass->setObjectName(QString::fromUtf8("eDTGuiClass"));
        eDTGuiClass->resize(619, 400);
        eDTGuiClass->setIconSize(QSize(24, 24));
        eDTGuiClass->setToolButtonStyle(Qt::ToolButtonIconOnly);
        eDTGuiClass->setDockNestingEnabled(false);
        centralWidget = new QWidget(eDTGuiClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        stackedWidget = new QStackedWidget(centralWidget);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
        stackedWidget->setGeometry(QRect(10, 10, 595, 371));
        pg_presentation = new QWidget();
        pg_presentation->setObjectName(QString::fromUtf8("pg_presentation"));
        pb_Analyse_nl = new QPushButton(pg_presentation);
        pb_Analyse_nl->setObjectName(QString::fromUtf8("pb_Analyse_nl"));
        pb_Analyse_nl->setGeometry(QRect(430, 340, 121, 23));
        pb_Analyse_fr = new QPushButton(pg_presentation);
        pb_Analyse_fr->setObjectName(QString::fromUtf8("pb_Analyse_fr"));
        pb_Analyse_fr->setGeometry(QRect(300, 340, 121, 23));
        lbl_Presentation = new QLabel(pg_presentation);
        lbl_Presentation->setObjectName(QString::fromUtf8("lbl_Presentation"));
        lbl_Presentation->setGeometry(QRect(40, 130, 141, 128));
        lbl_Presentation->setPixmap(QPixmap(QString::fromUtf8(":/Resources/Resources/ICO_CARD_EID_TOOLS_128x128.png")));
        lbl_Presentation->setOpenExternalLinks(true);
        lbl_title = new QLabel(pg_presentation);
        lbl_title->setObjectName(QString::fromUtf8("lbl_title"));
        lbl_title->setGeometry(QRect(200, 170, 321, 21));
        QFont font;
        font.setPointSize(10);
        font.setBold(true);
        font.setWeight(75);
        lbl_title->setFont(font);
        lbl_title->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        lbl_title->setWordWrap(true);
        lbl_title_2 = new QLabel(pg_presentation);
        lbl_title_2->setObjectName(QString::fromUtf8("lbl_title_2"));
        lbl_title_2->setGeometry(QRect(200, 200, 321, 21));
        lbl_title_2->setFont(font);
        lbl_title_2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        lbl_title_2->setWordWrap(true);
        lbl_version = new QLabel(pg_presentation);
        lbl_version->setObjectName(QString::fromUtf8("lbl_version"));
        lbl_version->setGeometry(QRect(40, 260, 141, 21));
        QFont font1;
        font1.setBold(true);
        font1.setWeight(75);
        lbl_version->setFont(font1);
        stackedWidget->addWidget(pg_presentation);
        pg_information = new QWidget();
        pg_information->setObjectName(QString::fromUtf8("pg_information"));
        pb_Continue_2 = new QPushButton(pg_information);
        pb_Continue_2->setObjectName(QString::fromUtf8("pb_Continue_2"));
        pb_Continue_2->setGeometry(QRect(430, 340, 121, 23));
        const QIcon icon = QIcon(QString::fromUtf8(":/Resources/Resources/wizarrow_right.png"));
        pb_Continue_2->setIcon(icon);
        pb_Continue_2->setIconSize(QSize(15, 15));
        lbl_instructions = new QLabel(pg_information);
        lbl_instructions->setObjectName(QString::fromUtf8("lbl_instructions"));
        lbl_instructions->setGeometry(QRect(70, 70, 471, 201));
        lbl_instructions->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        lbl_instructions->setWordWrap(true);
        stackedWidget->addWidget(pg_information);
        pg_progress = new QWidget();
        pg_progress->setObjectName(QString::fromUtf8("pg_progress"));
        progressBar_Overall = new QProgressBar(pg_progress);
        progressBar_Overall->setObjectName(QString::fromUtf8("progressBar_Overall"));
        progressBar_Overall->setGeometry(QRect(180, 210, 351, 23));
        progressBar_Overall->setValue(0);
        progressBar_Test = new QProgressBar(pg_progress);
        progressBar_Test->setObjectName(QString::fromUtf8("progressBar_Test"));
        progressBar_Test->setGeometry(QRect(180, 170, 351, 23));
        progressBar_Test->setMaximum(0);
        progressBar_Test->setValue(0);
        pb_Cancel = new QPushButton(pg_progress);
        pb_Cancel->setObjectName(QString::fromUtf8("pb_Cancel"));
        pb_Cancel->setGeometry(QRect(300, 340, 121, 23));
        pb_Continue = new QPushButton(pg_progress);
        pb_Continue->setObjectName(QString::fromUtf8("pb_Continue"));
        pb_Continue->setGeometry(QRect(430, 340, 121, 23));
        pb_Continue->setIcon(icon);
        pb_Continue->setIconSize(QSize(15, 15));
        label = new QLabel(pg_progress);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 110, 151, 151));
        label->setPixmap(QPixmap(QString::fromUtf8(":/Resources/Resources/ICO_CARD_EID_DIAGNOSIS_128x128.png")));
        lbl_testName = new QLabel(pg_progress);
        lbl_testName->setObjectName(QString::fromUtf8("lbl_testName"));
        lbl_testName->setGeometry(QRect(180, 110, 191, 16));
        lbl_testNameHead = new QLabel(pg_progress);
        lbl_testNameHead->setObjectName(QString::fromUtf8("lbl_testNameHead"));
        lbl_testNameHead->setGeometry(QRect(180, 90, 101, 16));
        stackedWidget->addWidget(pg_progress);
        pg_signing = new QWidget();
        pg_signing->setObjectName(QString::fromUtf8("pg_signing"));
        pb_testAuthentication = new QPushButton(pg_signing);
        pb_testAuthentication->setObjectName(QString::fromUtf8("pb_testAuthentication"));
        pb_testAuthentication->setGeometry(QRect(300, 340, 121, 23));
        pb_Continue_3 = new QPushButton(pg_signing);
        pb_Continue_3->setObjectName(QString::fromUtf8("pb_Continue_3"));
        pb_Continue_3->setGeometry(QRect(430, 340, 121, 23));
        pb_Continue_3->setIcon(icon);
        pb_Continue_3->setIconSize(QSize(15, 15));
        label_2 = new QLabel(pg_signing);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 120, 128, 128));
        label_2->setPixmap(QPixmap(QString::fromUtf8(":/Resources/Resources/ICO_CARD_EID_DIGSIG_128x128.png")));
        lbl_signing = new QLabel(pg_signing);
        lbl_signing->setObjectName(QString::fromUtf8("lbl_signing"));
        lbl_signing->setGeometry(QRect(170, 70, 361, 101));
        lbl_signing->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        lbl_signing->setWordWrap(true);
        progressBar_signing = new QProgressBar(pg_signing);
        progressBar_signing->setObjectName(QString::fromUtf8("progressBar_signing"));
        progressBar_signing->setGeometry(QRect(180, 210, 351, 23));
        progressBar_signing->setValue(0);
        lbl_testSignature = new QLabel(pg_signing);
        lbl_testSignature->setObjectName(QString::fromUtf8("lbl_testSignature"));
        lbl_testSignature->setGeometry(QRect(180, 180, 101, 16));
        stackedWidget->addWidget(pg_signing);
        pg_Summary = new QWidget();
        pg_Summary->setObjectName(QString::fromUtf8("pg_Summary"));
        lbl_Status = new QLabel(pg_Summary);
        lbl_Status->setObjectName(QString::fromUtf8("lbl_Status"));
        lbl_Status->setGeometry(QRect(220, 20, 161, 21));
        pb_SaveReport = new QPushButton(pg_Summary);
        pb_SaveReport->setObjectName(QString::fromUtf8("pb_SaveReport"));
        pb_SaveReport->setGeometry(QRect(170, 340, 121, 23));
        pb_Details = new QPushButton(pg_Summary);
        pb_Details->setObjectName(QString::fromUtf8("pb_Details"));
        pb_Details->setGeometry(QRect(40, 340, 121, 23));
        pb_Diag = new QPushButton(pg_Summary);
        pb_Diag->setObjectName(QString::fromUtf8("pb_Diag"));
        pb_Diag->setGeometry(QRect(430, 340, 121, 23));
        pb_Diag->setIcon(icon);
        pb_Diag->setIconSize(QSize(15, 15));
        tbl_Summary = new QTableWidget(pg_Summary);
        if (tbl_Summary->columnCount() < 2)
            tbl_Summary->setColumnCount(2);
        if (tbl_Summary->rowCount() < 11)
            tbl_Summary->setRowCount(11);
        tbl_Summary->setObjectName(QString::fromUtf8("tbl_Summary"));
        tbl_Summary->setGeometry(QRect(0, 70, 581, 261));
        tbl_Summary->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tbl_Summary->setAlternatingRowColors(true);
        tbl_Summary->setSelectionMode(QAbstractItemView::NoSelection);
        tbl_Summary->setSelectionBehavior(QAbstractItemView::SelectRows);
        tbl_Summary->setShowGrid(false);
        tbl_Summary->setWordWrap(false);
        tbl_Summary->setCornerButtonEnabled(false);
        tbl_Summary->setRowCount(11);
        tbl_Summary->setColumnCount(2);
        stackedWidget->addWidget(pg_Summary);
        pg_Details = new QWidget();
        pg_Details->setObjectName(QString::fromUtf8("pg_Details"));
        te_Report = new QTextEdit(pg_Details);
        te_Report->setObjectName(QString::fromUtf8("te_Report"));
        te_Report->setGeometry(QRect(0, 70, 585, 261));
        QFont font2;
        font2.setFamily(QString::fromUtf8("Courier New"));
        font2.setPointSize(8);
        te_Report->setFont(font2);
        te_Report->setReadOnly(true);
        lbl_Status_2 = new QLabel(pg_Details);
        lbl_Status_2->setObjectName(QString::fromUtf8("lbl_Status_2"));
        lbl_Status_2->setGeometry(QRect(220, 10, 111, 31));
        pb_SaveReport_2 = new QPushButton(pg_Details);
        pb_SaveReport_2->setObjectName(QString::fromUtf8("pb_SaveReport_2"));
        pb_SaveReport_2->setGeometry(QRect(170, 340, 121, 23));
        pb_Summary = new QPushButton(pg_Details);
        pb_Summary->setObjectName(QString::fromUtf8("pb_Summary"));
        pb_Summary->setGeometry(QRect(40, 340, 121, 23));
        pb_Diag2 = new QPushButton(pg_Details);
        pb_Diag2->setObjectName(QString::fromUtf8("pb_Diag2"));
        pb_Diag2->setGeometry(QRect(430, 340, 121, 23));
        pb_Diag2->setIcon(icon);
        pb_Diag2->setIconSize(QSize(15, 15));
        stackedWidget->addWidget(pg_Details);
        pg_diagnostics = new QWidget();
        pg_diagnostics->setObjectName(QString::fromUtf8("pg_diagnostics"));
        label_did_it_solve = new QLabel(pg_diagnostics);
        label_did_it_solve->setObjectName(QString::fromUtf8("label_did_it_solve"));
        label_did_it_solve->setGeometry(QRect(30, 318, 531, 20));
        label_did_it_solve->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        pb_needhelp = new QPushButton(pg_diagnostics);
        pb_needhelp->setObjectName(QString::fromUtf8("pb_needhelp"));
        pb_needhelp->setGeometry(QRect(540, 340, 23, 23));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(pb_needhelp->sizePolicy().hasHeightForWidth());
        pb_needhelp->setSizePolicy(sizePolicy);
        pb_needhelp->setMinimumSize(QSize(0, 0));
        pb_solved = new QPushButton(pg_diagnostics);
        pb_solved->setObjectName(QString::fromUtf8("pb_solved"));
        pb_solved->setGeometry(QRect(320, 340, 101, 23));
        pb_not_solved = new QPushButton(pg_diagnostics);
        pb_not_solved->setObjectName(QString::fromUtf8("pb_not_solved"));
        pb_not_solved->setGeometry(QRect(430, 340, 101, 23));
        pb_Summary_2 = new QPushButton(pg_diagnostics);
        pb_Summary_2->setObjectName(QString::fromUtf8("pb_Summary_2"));
        pb_Summary_2->setGeometry(QRect(10, 340, 27, 23));
        const QIcon icon1 = QIcon(QString::fromUtf8(":/Resources/Resources/wizarrow_left.png"));
        pb_Summary_2->setIcon(icon1);
        pb_Summary_2->setIconSize(QSize(15, 15));
        pb_Quit = new QPushButton(pg_diagnostics);
        pb_Quit->setObjectName(QString::fromUtf8("pb_Quit"));
        pb_Quit->setGeometry(QRect(170, 340, 111, 23));
        pb_SaveSend = new QPushButton(pg_diagnostics);
        pb_SaveSend->setObjectName(QString::fromUtf8("pb_SaveSend"));
        pb_SaveSend->setGeometry(QRect(50, 340, 111, 23));
        toolBox = new QToolBox(pg_diagnostics);
        toolBox->setObjectName(QString::fromUtf8("toolBox"));
        toolBox->setGeometry(QRect(10, 30, 551, 271));
        page = new QWidget();
        page->setObjectName(QString::fromUtf8("page"));
        page->setGeometry(QRect(0, 0, 551, 215));
        frame_3 = new QFrame(page);
        frame_3->setObjectName(QString::fromUtf8("frame_3"));
        frame_3->setGeometry(QRect(0, 0, 551, 221));
        frame_3->setFrameShape(QFrame::StyledPanel);
        frame_3->setFrameShadow(QFrame::Sunken);
        lbl_details = new QLabel(frame_3);
        lbl_details->setObjectName(QString::fromUtf8("lbl_details"));
        lbl_details->setGeometry(QRect(10, 10, 531, 201));
        QPalette palette;
        QBrush brush(QColor(170, 0, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Text, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush);
        QBrush brush1(QColor(118, 116, 108, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::Text, brush1);
        lbl_details->setPalette(palette);
        QFont font3;
        font3.setPointSize(8);
        font3.setBold(true);
        font3.setWeight(75);
        font3.setStyleStrategy(QFont::PreferDefault);
        lbl_details->setFont(font3);
        lbl_details->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        lbl_details->setWordWrap(true);
        lbl_details->setMargin(0);
        lbl_details->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard|Qt::LinksAccessibleByMouse|Qt::NoTextInteraction|Qt::TextBrowserInteraction|Qt::TextSelectableByKeyboard|Qt::TextSelectableByMouse);
        toolBox->addItem(page, QString::fromUtf8("Diagnostic:"));
        suggestion = new QWidget();
        suggestion->setObjectName(QString::fromUtf8("suggestion"));
        frame = new QFrame(suggestion);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(0, 0, 551, 211));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Sunken);
        frame->setLineWidth(1);
        frame->setMidLineWidth(0);
        lbl_remedy = new QLabel(frame);
        lbl_remedy->setObjectName(QString::fromUtf8("lbl_remedy"));
        lbl_remedy->setGeometry(QRect(10, 10, 531, 191));
        QFont font4;
        font4.setPointSize(8);
        font4.setBold(true);
        font4.setWeight(75);
        lbl_remedy->setFont(font4);
        lbl_remedy->setTextFormat(Qt::AutoText);
        lbl_remedy->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        lbl_remedy->setWordWrap(true);
        lbl_remedy->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard|Qt::LinksAccessibleByMouse|Qt::NoTextInteraction|Qt::TextBrowserInteraction|Qt::TextSelectableByKeyboard|Qt::TextSelectableByMouse);
        toolBox->addItem(suggestion, QString::fromUtf8("Suggestion:"));
        lbl_which_diag = new QLabel(pg_diagnostics);
        lbl_which_diag->setObjectName(QString::fromUtf8("lbl_which_diag"));
        lbl_which_diag->setGeometry(QRect(510, 0, 41, 21));
        lbl_which_diag->setAlignment(Qt::AlignCenter);
        pb_next = new QToolButton(pg_diagnostics);
        pb_next->setObjectName(QString::fromUtf8("pb_next"));
        pb_next->setGeometry(QRect(544, 2, 19, 19));
        pb_next->setArrowType(Qt::RightArrow);
        pb_previous = new QToolButton(pg_diagnostics);
        pb_previous->setObjectName(QString::fromUtf8("pb_previous"));
        pb_previous->setGeometry(QRect(497, 2, 19, 19));
        pb_previous->setArrowType(Qt::LeftArrow);
        stackedWidget->addWidget(pg_diagnostics);
        eDTGuiClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(eDTGuiClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        eDTGuiClass->setStatusBar(statusBar);

        retranslateUi(eDTGuiClass);

        stackedWidget->setCurrentIndex(6);
        toolBox->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(eDTGuiClass);
    } // setupUi

    void retranslateUi(QMainWindow *eDTGuiClass)
    {
        eDTGuiClass->setWindowTitle(QApplication::translate("eDTGuiClass", "eDT", 0, QApplication::UnicodeUTF8));
        pb_Analyse_nl->setText(QApplication::translate("eDTGuiClass", "Analyseren", 0, QApplication::UnicodeUTF8));
        pb_Analyse_fr->setText(QApplication::translate("eDTGuiClass", "Analyser", 0, QApplication::UnicodeUTF8));
        lbl_Presentation->setText(QString());
        lbl_title->setText(QApplication::translate("eDTGuiClass", "eID Middleware Diagnoseprogramma", 0, QApplication::UnicodeUTF8));
        lbl_title_2->setText(QApplication::translate("eDTGuiClass", "eID Middleware Outil de Diagnostique", 0, QApplication::UnicodeUTF8));
        lbl_version->setText(QApplication::translate("eDTGuiClass", "eDT : ", 0, QApplication::UnicodeUTF8));
        pb_Continue_2->setText(QApplication::translate("eDTGuiClass", "Start", 0, QApplication::UnicodeUTF8));
        lbl_instructions->setText(QApplication::translate("eDTGuiClass", "instructions", 0, QApplication::UnicodeUTF8));
        pb_Cancel->setText(QApplication::translate("eDTGuiClass", "Cancel", 0, QApplication::UnicodeUTF8));
        pb_Continue->setText(QApplication::translate("eDTGuiClass", "Continue", 0, QApplication::UnicodeUTF8));
        label->setText(QString());
        lbl_testName->setText(QApplication::translate("eDTGuiClass", "TextLabel", 0, QApplication::UnicodeUTF8));
        lbl_testNameHead->setText(QApplication::translate("eDTGuiClass", "Running: ", 0, QApplication::UnicodeUTF8));
        pb_testAuthentication->setText(QApplication::translate("eDTGuiClass", "Authenticate", 0, QApplication::UnicodeUTF8));
        pb_Continue_3->setText(QApplication::translate("eDTGuiClass", "Continue", 0, QApplication::UnicodeUTF8));
        label_2->setText(QString());
        lbl_signing->setText(QApplication::translate("eDTGuiClass", "Signing instructions", 0, QApplication::UnicodeUTF8));
        lbl_testSignature->setText(QApplication::translate("eDTGuiClass", "TextLabel", 0, QApplication::UnicodeUTF8));
        lbl_Status->setText(QApplication::translate("eDTGuiClass", "Succeeded", 0, QApplication::UnicodeUTF8));
        pb_SaveReport->setText(QApplication::translate("eDTGuiClass", "Save", 0, QApplication::UnicodeUTF8));
        pb_Details->setText(QApplication::translate("eDTGuiClass", "Details", 0, QApplication::UnicodeUTF8));
        pb_Diag->setText(QApplication::translate("eDTGuiClass", "Diagnostics", 0, QApplication::UnicodeUTF8));
        lbl_Status_2->setText(QApplication::translate("eDTGuiClass", "Succeeded", 0, QApplication::UnicodeUTF8));
        pb_SaveReport_2->setText(QApplication::translate("eDTGuiClass", "Save", 0, QApplication::UnicodeUTF8));
        pb_Summary->setText(QApplication::translate("eDTGuiClass", "Summary", 0, QApplication::UnicodeUTF8));
        pb_Diag2->setText(QApplication::translate("eDTGuiClass", "Diagnostics", 0, QApplication::UnicodeUTF8));
        label_did_it_solve->setText(QApplication::translate("eDTGuiClass", "Did the suggestion solve your problem?", 0, QApplication::UnicodeUTF8));
        pb_needhelp->setText(QApplication::translate("eDTGuiClass", "?", 0, QApplication::UnicodeUTF8));
        pb_solved->setText(QApplication::translate("eDTGuiClass", "Yes, it did!", 0, QApplication::UnicodeUTF8));
        pb_not_solved->setText(QApplication::translate("eDTGuiClass", "No,it didn't.", 0, QApplication::UnicodeUTF8));
        pb_Summary_2->setText(QString());
        pb_Quit->setText(QApplication::translate("eDTGuiClass", "Quit", 0, QApplication::UnicodeUTF8));
        pb_SaveSend->setText(QApplication::translate("eDTGuiClass", "Save", 0, QApplication::UnicodeUTF8));
        lbl_details->setText(QApplication::translate("eDTGuiClass", "details", 0, QApplication::UnicodeUTF8));
        toolBox->setItemText(toolBox->indexOf(page), QApplication::translate("eDTGuiClass", "Diagnostic:", 0, QApplication::UnicodeUTF8));
        lbl_remedy->setText(QApplication::translate("eDTGuiClass", "suggestion", 0, QApplication::UnicodeUTF8));
        toolBox->setItemText(toolBox->indexOf(suggestion), QApplication::translate("eDTGuiClass", "Suggestion:", 0, QApplication::UnicodeUTF8));
        lbl_which_diag->setText(QString());
        pb_next->setText(QString());
        pb_previous->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class eDTGuiClass: public Ui_eDTGuiClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EDTGUI_H
