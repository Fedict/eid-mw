
/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2011 FedICT.
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


#include "cardfactory.h"
#include "thread.h"
#include "common/log.h"
#include "common/util.h"
#include "card.h"

#include <string>
#include <algorithm>


namespace eIDMW
{
/**
 * The CardConnect() function returns a pointer to a CCard object
 * (that should free()-ed when no longer used) that can be used
 * to communicate to a beidcard.
 */
	CCard *CardConnect(const std::string & csReader, CPCSC * poPCSC, CPinpad * poPinpad)
	{
		CCard *poCard = NULL;
		const char *strReader = NULL;

		// Skip virtual Windows Hello for Business readers to avoid hammering
		std::string readerLower = csReader;
		std::transform(readerLower.begin(), readerLower.end(), readerLower.begin(), ::tolower);
		if (readerLower.find("windows hello for business") != std::string::npos)
		{
			MWLOG(LEV_INFO, MOD_CAL, L"    Skipping virtual reader %ls", utilStringWiden(csReader).c_str());
			return poCard;
		}

		if (poPCSC->m_ulConnectionDelay != 0)
		{
			CThread::SleepMillisecs(poPCSC->m_ulConnectionDelay);
		}
		// Try if we can connect to the card via a normal SCardConnect()
		SCARDHANDLE hCard = 0;
		try
		{
			hCard = poPCSC->Connect(csReader);
			if (hCard == 0)
			{
				return poCard;
			}
		}
		catch(CMWException & e)
		{
			MWLOG(LEV_INFO, MOD_CAL, L"    CardConnect threw error: [0x%08X]", e.GetError());
			if (e.GetError() != (long)EIDMW_ERR_CANT_CONNECT && e.GetError() != (long)EIDMW_ERR_CARD_COMM && e.GetError() != (long)EIDMW_ERR_NO_CARD)
			{
				throw;
			}
			return poCard;
		}

		if (hCard != 0)
		{
			strReader = csReader.c_str();
			// 1. A card is present and we could connect to it via a normal SCardConnect()
			if (poCard == NULL)
			{
				poCard = BeidCardGetInstance(strReader, hCard, poPCSC, poPinpad);
			}

			// 2. Card is not recognized as BE eID, fall back to unknown card handling
			if (poCard == NULL)
			{
				poCard = UnknownCardGetInstance(strReader, hCard, poPCSC, poPinpad);
			}
		}

		return poCard;
	}
}
