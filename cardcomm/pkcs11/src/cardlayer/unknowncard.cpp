
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
#include "unknowncard.h"

namespace eIDMW
{

	bool CUnknownCard::IsUnknownCard(SCARDHANDLE hCard,
					 CContext * poContext,
					 CByteArray & oData)
	{
		// This function shouldn't be called.
		return true;
	}

	CUnknownCard::CUnknownCard(SCARDHANDLE hCard, CContext * poContext,
				   CPinpad * poPinpad,
				   const CByteArray & oData)
		: CCard(hCard, poContext, poPinpad)
	{
	}

	CUnknownCard::~CUnknownCard(void)
	{
	}

	CByteArray CUnknownCard::ReadUncachedFile(const std::string & csPath,
						  unsigned long ulOffset,
						  unsigned long ulMaxLen)
	{
		throw CMWEXCEPTION(EIDMW_ERR_CARDTYPE_UNKNOWN);
	}

	std::string CUnknownCard::GetSerialNr()
	{
		return "";
	}

	std::string CUnknownCard::GetLabel()
	{
		return "Unknown";
	}

}
