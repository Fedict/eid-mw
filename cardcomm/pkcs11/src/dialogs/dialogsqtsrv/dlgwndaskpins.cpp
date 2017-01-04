
/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2010 FedICT.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */

#include "dlgwndaskpins.h"
#include "../langutil.h"

#include <stdio.h>

#define KP_BTN_SIZE 48

dlgWndAskPINs::dlgWndAskPINs(DlgPinInfo pinInfo1, DlgPinInfo pinInfo2, QString & Header, QString & PINName, bool UseKeypad, QWidget * parent):dlgWndBase
	(parent)
{
	ui.setupUi(this);

	//this->resize( 350, 280 );
	m_UK_InputField = 0;
	m_ulPin1MinLen = pinInfo1.ulMinLen;
	m_ulPin2MinLen = pinInfo2.ulMinLen;
	m_ulPin1MaxLen = pinInfo1.ulMaxLen;
	m_ulPin2MaxLen = pinInfo2.ulMaxLen;
	m_ulPin2Flags = pinInfo2.ulFlags;

	if (pinInfo1.ulFlags & PIN_FLAG_DIGITS)
	{
		char buffer[20];

		sprintf(buffer, "[0-9]{%d,%d}", m_ulPin1MinLen,
			m_ulPin1MaxLen);
		QRegExp rx(buffer);

		m_Pin1Validator = new QRegExpValidator(rx, 0);
	} else
	{
		m_Pin1Validator = NULL;
	}

	if (pinInfo2.ulFlags & PIN_FLAG_DIGITS)
	{
		char buffer[20];

		sprintf(buffer, "[0-9]{%d,%d}", m_ulPin2MinLen,
			m_ulPin2MaxLen);
		QRegExp rx(buffer);

		m_Pin2Validator = new QRegExpValidator(rx, 0);
	} else
	{
		m_Pin2Validator = NULL;
	}

	QString Title;

	//if( DApplic == DLG_APP_BELPIC )
	//{
	this->setWindowIcon(QIcon
			    (":/Resources/ICO_CARD_EID_PLAIN_16x16.png"));
	//      Title+= QString::fromWCharArray(GETSTRING_DLG(Belpic));
	//      Title+= ": ";
	//}
	Title += QString::fromWCharArray(GETSTRING_DLG(RenewingPinCode));
	this->setWindowTitle(Title);

	ui.lblHeader->
		setText(QString::fromWCharArray(GETSTRING_DLG(EnterYourPin)));
	ui.lblPINName_2->setText(QString::fromWCharArray(GETSTRING_DLG(Pin)));

	ui.lblOldPINName->
		setText(QString::fromWCharArray(GETSTRING_DLG(CurrentPin)));
	ui.lblNewPIN1->
		setText(QString::fromWCharArray(GETSTRING_DLG(NewPin)));
	ui.lblNewPIN2->
		setText(QString::
			fromWCharArray(GETSTRING_DLG(ConfirmNewPin)));

	ui.btnOk->setText(QString::fromWCharArray(GETSTRING_DLG(Ok)));
	ui.btnCancel->setText(QString::fromWCharArray(GETSTRING_DLG(Cancel)));

	ui.lblIcon->
		setPixmap(QPixmap(":/Resources/ICO_CARD_PIN_128x128.png"));

	if (m_UseKeypad = UseKeypad)
	{
		QList < QToolButton * >allTBT =
			ui.fraPIN_Keypad->findChildren < QToolButton * >();
		for (int i = 0; i < allTBT.size(); ++i)
		{
			allTBT.at(i)->setMinimumSize(KP_BTN_SIZE,
						     KP_BTN_SIZE);
			allTBT.at(i)->
				setIconSize(QSize(KP_BTN_SIZE, KP_BTN_SIZE));
			allTBT.at(i)->setAutoRaise(true);
		}

		ui.tbtNUM_1->setIcon(QIcon(":/Resources/KeyPadButton1.png"));
		ui.tbtNUM_2->setIcon(QIcon(":/Resources/KeyPadButton2.png"));
		ui.tbtNUM_3->setIcon(QIcon(":/Resources/KeyPadButton3.png"));
		ui.tbtNUM_4->setIcon(QIcon(":/Resources/KeyPadButton4.png"));
		ui.tbtNUM_5->setIcon(QIcon(":/Resources/KeyPadButton5.png"));
		ui.tbtNUM_6->setIcon(QIcon(":/Resources/KeyPadButton6.png"));
		ui.tbtNUM_7->setIcon(QIcon(":/Resources/KeyPadButton7.png"));
		ui.tbtNUM_8->setIcon(QIcon(":/Resources/KeyPadButton8.png"));
		ui.tbtNUM_9->setIcon(QIcon(":/Resources/KeyPadButton9.png"));
		ui.tbtNUM_0->setIcon(QIcon(":/Resources/KeyPadButton0.png"));
		ui.tbtClear->setIcon(QIcon(":/Resources/KeyPadButtonCE.png"));

		ui.fraPIN_Normal->setVisible(false);
		connect(ui.btnOk, SIGNAL(clicked()), this, SLOT(NextField()));
		m_ulPinMaxLen = m_ulPin1MaxLen;
		if (pinInfo1.ulFlags & PIN_FLAG_DIGITS)
			ui.txtPIN_Keypad->setValidator(m_Pin1Validator);
		else
			ui.txtPIN_Keypad->setMaxLength(m_ulPin1MaxLen);


		if (!PINName.isEmpty())
			ui.lblHeader->
				setText(QString::
					fromWCharArray(GETSTRING_DLG
						       (EnterYour)) + " " +
					PINName + " " +
					QString::
					fromWCharArray(GETSTRING_DLG(Code)));
	} else
	{
		ui.fraPIN_Keypad->setVisible(false);
		connect(ui.btnOk, SIGNAL(clicked()), this,
			SLOT(FinalCheck()));

		if (!Header.isEmpty())
			ui.lblHeader->setText(Header);
		if (!PINName.isEmpty())
			ui.lblOldPINName->setText(PINName);
	}

	ui.lblError->setVisible(false);
	OldPIN_OK = NewPIN1_OK = NewPIN2_OK = false;


	if (pinInfo1.ulFlags & PIN_FLAG_DIGITS)
		ui.txtOldPIN->setValidator(m_Pin1Validator);
	else
		ui.txtOldPIN->setMaxLength(m_ulPin1MaxLen);

	if (pinInfo2.ulFlags & PIN_FLAG_DIGITS)
	{
		ui.txtNewPIN1->setValidator(m_Pin2Validator);
		ui.txtNewPIN2->setValidator(m_Pin2Validator);
	} else
	{
		ui.txtNewPIN1->setMaxLength(m_ulPin2MaxLen);
		ui.txtNewPIN2->setMaxLength(m_ulPin2MaxLen);
	}

}

dlgWndAskPINs::~dlgWndAskPINs()
{
	if (m_Pin1Validator)
	{
		delete m_Pin1Validator;

		m_Pin1Validator = NULL;
	}

	if (m_Pin2Validator)
	{
		delete m_Pin2Validator;

		m_Pin2Validator = NULL;
	}
}

void dlgWndAskPINs::on_txtOldPIN_textChanged(const QString & text)
{
	OldPIN_OK = (unsigned) text.length() >= m_ulPin1MinLen;
	ui.btnOk->setEnabled(TestPINs());
}

void dlgWndAskPINs::on_txtNewPIN1_textChanged(const QString & text)
{
	NewPIN1_OK = (unsigned) text.length() >= m_ulPin2MinLen;
	ui.btnOk->setEnabled(TestPINs());
}

void dlgWndAskPINs::on_txtNewPIN2_textChanged(const QString & text)
{
	NewPIN2_OK = (unsigned) text.length() >= m_ulPin2MinLen;
	ui.btnOk->setEnabled(TestPINs());
}

void dlgWndAskPINs::on_txtPIN_Keypad_textChanged(const QString & text)
{
	if (m_UK_InputField == INPUTFIELD_OLD)
	{
		ui.btnOk->setEnabled((unsigned) text.length() >=
				     m_ulPin1MinLen);
	} else			// INPUTFIELD_NEW || INPUTFIELD_CONFIRM
	{
		ui.btnOk->setEnabled((unsigned) text.length() >=
				     m_ulPin2MinLen);
	}
}

void dlgWndAskPINs::NextField()
{
	switch (m_UK_InputField)
	{
		case INPUTFIELD_OLD:	// 0
		{
			ui.txtOldPIN->setText(ui.txtPIN_Keypad->text());
			ui.lblHeader->
				setText(QString::
					fromWCharArray(GETSTRING_DLG
						       (EnterYourNewPinCode)));
			m_ulPinMaxLen = m_ulPin2MaxLen;
			if (m_ulPin2Flags & PIN_FLAG_DIGITS)
				ui.txtPIN_Keypad->
					setValidator(m_Pin2Validator);
			else
				ui.txtPIN_Keypad->
					setMaxLength(m_ulPin2MaxLen);

			break;
		}

		case INPUTFIELD_NEW:	// 1
		{
			ui.txtNewPIN1->setText(ui.txtPIN_Keypad->text());
			ui.lblHeader->
				setText(QString::
					fromWCharArray(GETSTRING_DLG
						       (EnterYourNewPinCodeAgainToConfirm)));
			ui.lblError->setVisible(false);
			break;
		}

		case INPUTFIELD_CONFIRM:	// 2
		{
			ui.txtNewPIN2->setText(ui.txtPIN_Keypad->text());
			FinalCheck();
			return;
		}
	}
	m_UK_InputField++;
	ui.txtPIN_Keypad->clear();
}

void dlgWndAskPINs::FinalCheck()
{
	if (ui.txtNewPIN2->text() == ui.txtNewPIN1->text()
	    && !ui.txtNewPIN1->text().isEmpty())
	{
		this->accept();
	} else
	{
		ui.lblError->
			setText(QString::
				fromWCharArray(GETSTRING_DLG
					       (ErrorTheNewPinCodesAreNotIdentical)));
		ui.lblError->setVisible(true);
		if (m_UseKeypad)
		{
			m_UK_InputField = INPUTFIELD_NEW;
			ui.txtPIN_Keypad->clear();
			ui.lblHeader->
				setText(QString::
					fromWCharArray(GETSTRING_DLG
						       (RetryEnterYourNewPinCode)));
		}
	}
}

void dlgWndAskPINs::on_tbtNUM_0_clicked()
{
	if ((unsigned) ui.txtPIN_Keypad->text().size() < m_ulPinMaxLen)
		ui.txtPIN_Keypad->setText(ui.txtPIN_Keypad->text() + "0");
}

void dlgWndAskPINs::on_tbtNUM_1_clicked()
{
	if ((unsigned) ui.txtPIN_Keypad->text().size() < m_ulPinMaxLen)
		ui.txtPIN_Keypad->setText(ui.txtPIN_Keypad->text() + "1");
}

void dlgWndAskPINs::on_tbtNUM_2_clicked()
{
	if ((unsigned) ui.txtPIN_Keypad->text().size() < m_ulPinMaxLen)
		ui.txtPIN_Keypad->setText(ui.txtPIN_Keypad->text() + "2");
}

void dlgWndAskPINs::on_tbtNUM_3_clicked()
{
	if ((unsigned) ui.txtPIN_Keypad->text().size() < m_ulPinMaxLen)
		ui.txtPIN_Keypad->setText(ui.txtPIN_Keypad->text() + "3");
}

void dlgWndAskPINs::on_tbtNUM_4_clicked()
{
	if ((unsigned) ui.txtPIN_Keypad->text().size() < m_ulPinMaxLen)
		ui.txtPIN_Keypad->setText(ui.txtPIN_Keypad->text() + "4");
}

void dlgWndAskPINs::on_tbtNUM_5_clicked()
{
	if ((unsigned) ui.txtPIN_Keypad->text().size() < m_ulPinMaxLen)
		ui.txtPIN_Keypad->setText(ui.txtPIN_Keypad->text() + "5");
}

void dlgWndAskPINs::on_tbtNUM_6_clicked()
{
	if ((unsigned) ui.txtPIN_Keypad->text().size() < m_ulPinMaxLen)
		ui.txtPIN_Keypad->setText(ui.txtPIN_Keypad->text() + "6");
}

void dlgWndAskPINs::on_tbtNUM_7_clicked()
{
	if ((unsigned) ui.txtPIN_Keypad->text().size() < m_ulPinMaxLen)
		ui.txtPIN_Keypad->setText(ui.txtPIN_Keypad->text() + "7");
}

void dlgWndAskPINs::on_tbtNUM_8_clicked()
{
	if ((unsigned) ui.txtPIN_Keypad->text().size() < m_ulPinMaxLen)
		ui.txtPIN_Keypad->setText(ui.txtPIN_Keypad->text() + "8");
}

void dlgWndAskPINs::on_tbtNUM_9_clicked()
{
	if ((unsigned) ui.txtPIN_Keypad->text().size() < m_ulPinMaxLen)
		ui.txtPIN_Keypad->setText(ui.txtPIN_Keypad->text() + "9");
}

void dlgWndAskPINs::on_tbtClear_clicked()
{
	ui.txtPIN_Keypad->clear();
}
