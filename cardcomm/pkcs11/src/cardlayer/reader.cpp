
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
#include "reader.h"
#include "card.h"
#include "common/log.h"
#include "cardfactory.h"

namespace eIDMW
{

	CReader::CReader(const std::string & csReader, CPCSC * poPCSC):m_poCard(NULL)
	{
		m_csReader = csReader;
		m_wsReader = utilStringWiden(csReader);
		m_poPCSC = poPCSC;
		m_poCard = NULL;
	}

	CReader::~CReader(void)
	{
		if (m_poCard != NULL)
			Disconnect(DISCONNECT_LEAVE_CARD);
	}

	std::string & CReader::GetReaderName()
	{
		return m_csReader;
	}

	bool CReader::CardPresent(unsigned long ulState)
	{
		return (ulState & 0x20) == 0x20;
	}

// Use for logging in Status()
	static const inline wchar_t *Status2String(tCardStatus status)
	{
		switch (status)
		{
			case CARD_INSERTED:
				return L"card inserted";
			case CARD_NOT_PRESENT:
				return L"no card present";
			case CARD_STILL_PRESENT:
				return L"card stil present";
			case CARD_REMOVED:
				return L"card removed";
			case CARD_OTHER:
				return L"card removed and (another) card inserted";
			case CARD_UNKNOWN_STATE:
				return L"card state unknown (slot in use?)";
			default:
				return L"unknown state?!?";
		}
	}

	tCardStatus CReader::Status(bool bReconnect, bool bPresenceOnly)
	{
		tCardStatus status;
		static int iStatusCount = 0;

		try
		{
			if (m_poCard == NULL)
			{
				//no card object created yet, if a card is present we should create one
				if (m_poPCSC->Status(m_csReader))
				{
					if (!bPresenceOnly) {
						status = Connect()? CARD_INSERTED : CARD_NOT_PRESENT;
					} else {
						status = CARD_INSERTED;
					}
				}
				else
				{
					status = CARD_NOT_PRESENT;
				}
			}
			else
			{
				//a card object is created already

				//check if the same card is still present (i.e. if we can still communicate using the current card handle)
				if (m_poCard->Status())
				{
					//we can still use the current card handle, so the same card is still present
					status = CARD_STILL_PRESENT;
				}
				else
				{
					//its a different card, or no card, so drop the old connection (current card handle no longer valid)
					Disconnect();

					//check if a card is also present now
					if (bReconnect && m_poPCSC->Status(m_csReader))
					{
						//try to start a new connection to this card as a reconnection was wanted
						status = Connect() ? CARD_OTHER : CARD_REMOVED;
					}
					else
					{
						//either no reconnection was wanted and we'll report the old card as removed
						//or the card has been removed and no new card is present yet
						status = CARD_REMOVED;
					}
				}
			}
		}
		catch (CMWException &e)
		{
			if (e.GetError() == EIDMW_ERR_CARD_SHARING)
			{
				status = CARD_UNKNOWN_STATE;
			}
			else
			{
				throw e;
			}
		}

		if (iStatusCount < 5)
		{
			MWLOG(LEV_DEBUG, MOD_CAL, L"    ReaderStatus(): %ls", Status2String(status));
			iStatusCount++;
		}

		return status;
	}

// Used for logging in Connect()
	static const inline wchar_t *Type2String(tCardType cardType)
	{
		switch (cardType)
		{
			case CARD_BEID:
				return L"BE eID";;
			default:
				return L"unknown";
		}
	}

	bool CReader::Connect()
	{
		if (m_poCard != NULL)
			Disconnect(DISCONNECT_LEAVE_CARD);

		MWLOG(LEV_INFO, MOD_CAL, L" CReader::Connect()");
		m_poCard = CardConnect(m_csReader, m_poPCSC, &m_oPinpad);
		if (m_poCard != NULL)
		{
			
#ifdef WIN32
			if ((strstr(m_csReader.c_str(), "SPRx32 USB") != NULL))
			{
				m_oPinpad.Init(m_poPCSC, m_poCard->m_hCard, m_csReader, m_poCard->GetPinpadPrefix(), m_poCard->GetIFDVersion());
			} else
			{
#endif
				m_oPinpad.Init(m_poPCSC, m_poCard->m_hCard, m_csReader, m_poCard->GetPinpadPrefix());
#ifdef WIN32
			}
#endif
			MWLOG(LEV_INFO, MOD_CAL, L" Connected to %ls card in reader %ls", Type2String(m_poCard->GetType()), m_wsReader.c_str());
		}

		return m_poCard != 0;
	}

	void CReader::Disconnect(tDisconnectMode disconnectMode)
	{
		if (m_poCard != NULL)
		{
			// Strange behaviour with Ctrl-C:
			// It's possible that this function is called multiple times. Normally,
			// this doesn't hurt except that after a Ctrl-C, m_poCard->Disconnect()
			// throws us out of this function WITHOUT an exception! So the m_poCard
			// is not deleted() and set to NULL allthough the next call to this function
			// it contains rubbisch => CRASH.
			// So we set m_poCard = NULL in advance, and only if an exception is thrown
			// we assign it the old value.
			CCard *poTemp = m_poCard;

			m_poCard = NULL;
			try
			{
				poTemp->Disconnect(disconnectMode);
				MWLOG(LEV_INFO, MOD_CAL, L" Disconnected from card in reader %ls", m_wsReader.c_str());
				delete poTemp;
			}
			catch( ...)
			{
				m_poCard = poTemp;
			}
		}
	}

	CCard* CReader::GetCard(void)
	{
		//if (m_poCard == NULL)
		//	Connect();
		if (m_poCard == NULL)
			throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

		return m_poCard;
	}

}
