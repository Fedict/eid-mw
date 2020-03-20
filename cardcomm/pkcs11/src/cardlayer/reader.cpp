
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
#include "common/mwexception.h"

namespace eIDMW
{
//the DigestInfo Values:
	static const unsigned char MD5_AID[] = {
		0x30, 0x20,
		0x30, 0x0c,
		0x06, 0x08, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x02, 0x05,
		0x05, 0x00,
		0x04, 0x10
	};

	static const unsigned char SHA1_AID[] = {
		0x30, 0x21,
		0x30, 0x09,
		0x06, 0x05, 0x2b, 0x0e, 0x03, 0x02, 0x1a,
		0x05, 0x00,
		0x04, 0x14
	};
	static const unsigned char SHA256_AID[] = {
		0x30, 0x31,
		0x30, 0x0d,
		0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02,
			0x01,
		0x05, 0x00,
		0x04, 0x20
	};
	static const unsigned char SHA384_AID[] = {
		0x30, 0x41,
		0x30, 0x0d,
		0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02,
			0x02,
		0x05, 0x00,
		0x04, 0x30
	};
	static const unsigned char SHA512_AID[] = {
		0x30, 0x51,
		0x30, 0x0d,
		0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02,
			0x03,
		0x05, 0x00,
		0x04, 0x40
	};
	static const unsigned char RIPEMD160_AID[] = {
		0x30, 0x21,
		0x30, 0x09,
		0x06, 0x05, 0x2B, 0x24, 0x03, 0x02, 0x01,
		0x05, 0x00,
		0x04, 0x14
	};

	CReader::CReader(const std::string & csReader, CContext * poContext):m_poCard(NULL),m_oPKCS15()
	{
		m_csReader = csReader;
		m_wsReader = utilStringWiden(csReader);
		m_poContext = poContext;
		m_poCard = NULL;
		m_bIgnoreRemoval = false;
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

	tCardStatus CReader::Status(bool bReconnect)
	{
		tCardStatus status;
		static int iStatusCount = 0;
		bool cardPresent = FALSE;
		long lRet = SCARD_S_SUCCESS;

		try
		{
			if (m_poCard == NULL)
			{
				//no card object created yet, if a card is present we should create one
				lRet = m_poContext->m_oPCSC.Status(m_csReader, cardPresent);
				if (lRet == SCARD_S_SUCCESS)
				{
					if (cardPresent == TRUE)
					{
						status = Connect() ? CARD_INSERTED : CARD_NOT_PRESENT;
					}
					else
					{
						status = CARD_NOT_PRESENT;
					}
				}
				else if (lRet == SCARD_E_SHARING_VIOLATION)
				{
					status = CARD_UNKNOWN_STATE;
				}
				else
				{
					throw CMWEXCEPTION(m_poContext->m_oPCSC.PcscToErr(lRet));
				}
			}
			else
			{
				//card object was present already

				lRet = m_poContext->m_oPCSC.Status(m_csReader, cardPresent);

				if (lRet == SCARD_S_SUCCESS)
				{
					if (cardPresent == TRUE)
					{
						//check if card is still the same
						if (m_poCard->Status())
						{
							status = CARD_STILL_PRESENT;
						}
						else
						{
							//its a different card, so drop the old connection
							Disconnect();

							if (bReconnect)
							{
								//try to start a new connection to this card
								status = Connect() ? CARD_OTHER : CARD_REMOVED;
							}
							else
							{
								//no reconnection was wanted, so report as card removed
								status = CARD_REMOVED;
							}
						}
					}
					//else
					//{
						//Disconnect(); not dropping the card context yet
						//status = CARD_REMOVED;
					//}
				}
				else if (lRet == SCARD_E_SHARING_VIOLATION)
				{
					status = CARD_UNKNOWN_STATE;
				}
			}

			if (iStatusCount < 5)
			{
				MWLOG(LEV_DEBUG, MOD_CAL, L"    ReaderStatus(): %ls", Status2String(status));
				iStatusCount++;
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

		return status;
	}

// Used for logging in Connect()
	static const inline wchar_t *Type2String(tCardType cardType)
	{
		switch (cardType)
		{
			case CARD_BEID:
				return L"BE eID";;
			case CARD_SIS:
				return L"SIS";
			default:
				return L"unknown";
		}
	}

	bool CReader::Connect()
	{
		if (m_poCard != NULL)
			Disconnect(DISCONNECT_LEAVE_CARD);

		m_poCard = CardConnect(m_csReader, m_poContext, &m_oPinpad, m_oCardPluginLib);
		if (m_poCard != NULL)
		{
			m_oPKCS15.SetCard(m_poCard);
#ifdef WIN32
			if ((strstr(m_csReader.c_str(), "SPRx32 USB") !=
			     NULL))
			{
				m_oPinpad.Init(m_poContext, m_poCard->m_hCard, m_csReader, m_poCard->GetPinpadPrefix(), m_poCard->GetIFDVersion());
			} else
			{
#endif
				m_oPinpad.Init(m_poContext, m_poCard->m_hCard, m_csReader, m_poCard->GetPinpadPrefix());
#ifdef WIN32
			}
#endif
			MWLOG(LEV_INFO, MOD_CAL, L" Connected to %ls card in reader %ls", Type2String(m_poCard->GetType()), m_wsReader.c_str());
		}

		return m_poCard != 0;
	}

	void CReader::Disconnect(tDisconnectMode disconnectMode)
	{
		m_oPKCS15.Clear(NULL);

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

	CByteArray CReader::GetATR()
	{
		if (m_poCard == NULL)
			throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

		return m_poCard->GetATR();
	}

	bool CReader::IsPinpadReader()
	{
		if (m_poCard == NULL)
			throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

		return m_poCard->IsPinpadReader();
	}

	tCardType CReader::GetCardType()
	{
		if (m_poCard == NULL)
			throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

		return m_poCard->GetType();
	}

	CByteArray CReader::GetInfo()
	{
		if (m_poCard == NULL)
			throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

		return m_poCard->GetInfo();
	}

	std::string CReader::GetSerialNr()
	{
		if (m_poCard == NULL)
			throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

		try
		{
			return m_poCard->GetSerialNr();
		}
		catch(CMWException & e)
		{
			(void) e.GetError();
			return m_oPKCS15.GetSerialNr();
		}
	}

	std::string CReader::GetCardLabel()
	{
		if (m_poCard == NULL)
			throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

		std::string csLabel = m_poCard->GetLabel();

		if (csLabel == "")
			csLabel = m_oPKCS15.GetCardLabel();

		return csLabel;
	}

	void CReader::Lock()
	{
		if (m_poCard == NULL)
			throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

		return m_poCard->Lock();
	}

	void CReader::Unlock()
	{
		if (m_poCard == NULL)
			throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

		return m_poCard->Unlock();
	}

	void CReader::SelectApplication(const CByteArray & oAID)
	{
		if (m_poCard == NULL)
			throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

		return m_poCard->SelectApplication(oAID);
	}

	CByteArray CReader::ReadFile(const std::string & csPath, unsigned long ulOffset, unsigned long ulMaxLen, bool bDoNotCache)
	{
		if (m_poCard == NULL)
			throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

		try
		{
			return m_poCard->ReadFile(csPath, ulOffset, ulMaxLen, bDoNotCache);
		}
		catch(const CNotAuthenticatedException & e)
		{
			// A PIN is needed to read -> ask the correct PIN and do a verification
			unsigned long ulRemaining;
			tPin pin = m_oPKCS15.GetPinByRef(e.GetPinRef());

			if (pin.bValid)
			{
				if (m_poCard-> PinCmd(PIN_OP_VERIFY, pin, "", "", ulRemaining, NULL))
				{
					return m_poCard->ReadFile(csPath, ulOffset, ulMaxLen);
				} 
				else {
					throw CMWEXCEPTION(ulRemaining == 0 ? EIDMW_ERR_PIN_BLOCKED : EIDMW_ERR_PIN_BAD);
				}
			} else
				throw CMWEXCEPTION(EIDMW_ERR_CMD_NOT_ALLOWED);
		}
	}

	unsigned long CReader::PinStatus(const tPin & Pin)
	{
		if (m_poCard == NULL)
			throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

		return m_poCard->PinStatus(Pin);
	}

	bool CReader::PinCmd(tPinOperation operation, const tPin & Pin,
			     const std::string & csPin1,
			     const std::string & csPin2,
			     unsigned long &ulRemaining)
	{
		if (m_poCard == NULL)
			throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

		return m_poCard->PinCmd(operation, Pin, csPin1, csPin2, ulRemaining);
	}

	unsigned long CReader::GetSupportedAlgorithms()
	{
		if (m_poCard == NULL)
			throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

		unsigned long algos = m_poCard->GetSupportedAlgorithms();

		if (algos & SIGN_ALGO_RSA_RAW)
			algos |= SIGN_ALGO_RSA_PKCS;
		if (algos & SIGN_ALGO_RSA_PKCS)
			algos |= (SIGN_ALGO_MD5_RSA_PKCS |
				  SIGN_ALGO_SHA1_RSA_PKCS |
				  SIGN_ALGO_SHA256_RSA_PKCS |
				  SIGN_ALGO_SHA384_RSA_PKCS |
				  SIGN_ALGO_SHA512_RSA_PKCS |
				  SIGN_ALGO_RIPEMD160_RSA_PKCS);

		return algos;
	}

	CByteArray CReader::Sign(const tPrivKey & key, unsigned long algo,
				 const CByteArray & oData)
	{
		if (m_poCard == NULL)
			throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

		unsigned long ulSupportedAlgos = m_poCard->GetSupportedAlgorithms();

		if (algo & ulSupportedAlgos)
			return m_poCard->Sign(key, GetPinByID(key.ulAuthID), algo, oData);
		else
		{
			CByteArray oAID_Data;

			if (algo & SIGN_ALGO_MD5_RSA_PKCS)
				oAID_Data.Append(MD5_AID, sizeof(MD5_AID));
			else if (algo & SIGN_ALGO_SHA1_RSA_PKCS)
				oAID_Data.Append(SHA1_AID, sizeof(SHA1_AID));
			else if (algo & SIGN_ALGO_SHA256_RSA_PKCS)
				oAID_Data.Append(SHA256_AID, sizeof(SHA256_AID));
			else if (algo & SIGN_ALGO_SHA384_RSA_PKCS)
				oAID_Data.Append(SHA384_AID, sizeof(SHA384_AID));
			else if (algo & SIGN_ALGO_SHA512_RSA_PKCS)
				oAID_Data.Append(SHA512_AID, sizeof(SHA512_AID));
			else if (algo & SIGN_ALGO_RIPEMD160_RSA_PKCS)
				oAID_Data.Append(RIPEMD160_AID, sizeof(RIPEMD160_AID));
			oAID_Data.Append(oData);

			if (ulSupportedAlgos & SIGN_ALGO_RSA_PKCS)
			{
				return m_poCard->Sign(key, GetPinByID(key.ulAuthID), SIGN_ALGO_RSA_PKCS, oAID_Data);
			} else if (ulSupportedAlgos & SIGN_ALGO_RSA_RAW)
			{
				if (oAID_Data.Size() > key.ulKeyLenBytes - 11)
				{
					throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);
				}

				CByteArray oRawData(NULL, 0, key.ulKeyLenBytes);

				oRawData.Append(0x00);
				oRawData.Append(0x01);
				for (unsigned long i = 2; i < key.ulKeyLenBytes - oAID_Data.Size() - 1; i++)
				{
					oRawData.Append(0xFF);
				}		
				oRawData.Append(0x00);
				oRawData.Append(oAID_Data);

				return m_poCard->Sign(key, GetPinByID(key.ulID), SIGN_ALGO_RSA_RAW, oData);
			} else
				throw CMWEXCEPTION(EIDMW_ERR_CHECK);
		}
	}

	CByteArray CReader::SendAPDU(const CByteArray & oCmdAPDU)
	{
		if (m_poCard == NULL)
			throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

		return m_poCard->SendAPDU(oCmdAPDU);
	}

	unsigned long CReader::PinCount()
	{
		if (m_poCard == NULL)
			throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

		return m_oPKCS15.PinCount();
	}

	tPin CReader::GetPin(unsigned long ulIndex)
	{
		if (m_poCard == NULL)
			throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

		return m_oPKCS15.GetPin(ulIndex);
	}

	tPin CReader::GetPinByID(unsigned long ulID)
	{
		if (m_poCard == NULL)
			throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

		return m_oPKCS15.GetPinByID(ulID);
	}

	unsigned long CReader::CertCount()
	{
		if (m_poCard == NULL)
			throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

		return m_oPKCS15.CertCount();
	}

	tCert CReader::GetCert(unsigned long ulIndex)
	{
		if (m_poCard == NULL)
			throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

		return m_oPKCS15.GetCert(ulIndex);
	}

	tCert CReader::GetCertByID(unsigned long ulID)
	{
		if (m_poCard == NULL)
			throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

		return m_oPKCS15.GetCertByID(ulID);
	}

	unsigned long CReader::PrivKeyCount()
	{
		if (m_poCard == NULL)
			throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

		return m_oPKCS15.PrivKeyCount();
	}

	tPrivKey CReader::GetPrivKey(unsigned long ulIndex)
	{
		if (m_poCard == NULL)
			throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

		return m_oPKCS15.GetPrivKey(ulIndex);
	}

	tPrivKey CReader::GetPrivKeyByID(unsigned long ulID)
	{
		if (m_poCard == NULL)
			throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

		return m_oPKCS15.GetPrivKeyByID(ulID);
	}

	PinUsage CReader::GetPinUsage(const tPin & pin)
	{
		if (m_poCard == NULL)
			throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

		DlgPinUsage usage = m_poCard->PinUsage2Dlg(pin, NULL);

		switch (usage)
		{
			case DLG_PIN_AUTH:
				return DLG_USG_PIN_AUTH;
			case DLG_PIN_SIGN:
				return DLG_USG_PIN_SIGN;
			case DLG_PIN_ADDRESS:
				return DLG_USG_PIN_ADDRESS;
			default:
				return PIN_USG_UNKNOWN;
		}
	}

	unsigned int CReader::GetRSAKeySize()
	{
		if (m_poCard == NULL)
			throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

		return m_poCard->GetRSAKeySize();
	}

	unsigned char CReader::GetAppletVersion()
	{
		if (m_poCard == NULL)
			throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

		return m_poCard->GetAppletVersion();
	}

}
