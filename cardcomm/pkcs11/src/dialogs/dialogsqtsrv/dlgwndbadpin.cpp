
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

#include "dlgwndbadpin.h"
#include "../langutil.h"

dlgWndBadPIN::dlgWndBadPIN(QString & PINName, unsigned long RemainingTries,
			   QWidget * parent):dlgWndBase(parent)
{
	ui.setupUi(this);

	QString Title = "";

	//if( DApplic == DLG_APP_BELPIC )
	//{
	this->setWindowIcon(QIcon
			    (":/Resources/ICO_CARD_EID_PLAIN_16x16.png"));
	//      Title+=QString::fromWCharArray(GETSTRING_DLG(Belpic));
	//      Title+=": ";
	//}
	//else
	//{
	Title += QString::fromWCharArray(GETSTRING_DLG(Notification));
	Title += ": ";
	//}
	Title += QString::fromWCharArray(GETSTRING_DLG(Bad));
	Title += " ";
	Title += PINName;
	this->setWindowTitle(Title);

	ui.btnOk->setText(QString::fromWCharArray(GETSTRING_DLG(Ok)));
	ui.btnCancel->setText(QString::fromWCharArray(GETSTRING_DLG(Cancel)));
	ui.btnRetry->setText(QString::fromWCharArray(GETSTRING_DLG(Retry)));
	ui.lblIcon->setPixmap(QPixmap(":/Resources/ICO_CARD_NOK_64x64.png"));

	QString sHeader;

	sHeader = QString::fromWCharArray(GETSTRING_DLG(Bad));
	sHeader += " ";
	sHeader += PINName;
	sHeader += ": ";
	sHeader += QString().setNum(RemainingTries);
	sHeader += " ";
	sHeader += QString::fromWCharArray(GETSTRING_DLG(RemainingAttempts));
	ui.lblHeader->setText(sHeader);

	QString sCenter = "";

	if (RemainingTries == 0)
	{
		sCenter += PINName;
		sCenter += " ";
		sCenter += QString::fromWCharArray(GETSTRING_DLG(PinBlocked));
		ui.btnRetry->setVisible(false);
		ui.btnCancel->setVisible(false);
	} else
	{
		sCenter +=
			QString::
			fromWCharArray(GETSTRING_DLG(TryAgainOrCancel));
		ui.btnOk->setVisible(false);
	}

	ui.lblCenter->setText(sCenter);

}

dlgWndBadPIN::~dlgWndBadPIN()
{

}
