
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
#include "card.h"
#include "thread.h"
#include "common/log.h"

namespace eIDMW
{

	CCard::CCard(SCARDHANDLE hCard, CContext * poContext,
		     CPinpad * poPinpad):m_hCard(hCard),
		m_poContext(poContext), m_poPinpad(poPinpad),
		m_cardType(CARD_UNKNOWN), m_ulLockCount(0),
		m_bSerialNrString(false), m_ucCLA(0)
	{
	}

	CCard::  ~CCard(void)
	{
		Disconnect(DISCONNECT_LEAVE_CARD);
	}

	void CCard::Disconnect(tDisconnectMode disconnectMode)
	{
		if (m_hCard != 0)
		{
			SCARDHANDLE hTemp = m_hCard;

			m_hCard = 0;
			m_poContext->m_oPCSC.Disconnect(hTemp,
							disconnectMode);
		}
	}

	CByteArray CCard::GetATR()
	{
		return m_poContext->m_oPCSC.GetATR(m_hCard);
	}

	CByteArray CCard::GetIFDVersion()
	{
		return m_poContext->m_oPCSC.GetIFDVersion(m_hCard);
	}

	bool CCard::Status()
	{
		return m_poContext->m_oPCSC.Status(m_hCard);
	}

	bool CCard::IsPinpadReader()
	{
		return false;
	}

	std::string CCard::GetPinpadPrefix()
	{
		return "";
	}

	std::string CCard::GetSerialNr()
	{
		if (!m_bSerialNrString)
		{
			m_csSerialNr =
				GetSerialNrBytes().ToString(false, true);
			m_bSerialNrString = true;
		}

		return m_csSerialNr;
	}

	CByteArray CCard::GetSerialNrBytes()
	{
		throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
	}

	std::string CCard::GetLabel()
	{
		return "";
	}

	CByteArray CCard::GetInfo()
	{
		return CByteArray();
	}

	void CCard::Lock()
	{
		if (m_ulLockCount == 0)
			m_poContext->m_oPCSC.BeginTransaction(m_hCard);
		m_ulLockCount++;
	}

	void CCard::Unlock()
	{
		if (m_ulLockCount == 0)
			MWLOG(LEV_ERROR, MOD_CAL,
			      L"More Unlock()s then Lock()s called!!");
		else
		{
			m_ulLockCount--;
			if (m_ulLockCount == 0)
				m_poContext->m_oPCSC.EndTransaction(m_hCard);
		}
	}

	void CCard::SelectApplication(const CByteArray & oAID)
	{
		throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
	}

	bool CCard::SerialNrPresent(const CByteArray & oData)
	{
		CByteArray oSerial = GetSerialNrBytes();
		const unsigned char *pucSerial = oSerial.GetBytes();
		unsigned long ulSerialLen = (unsigned long) oSerial.Size();

		const unsigned char *pucData = oData.GetBytes();
		unsigned long ulEnd = oData.Size() - ulSerialLen;

		//printf("serial: %s\n", CByteArray(csSerial).ToString(false, false).c_str());
		//printf("data: %s\n", oData.ToString(false, false).c_str());
		for (unsigned long i = 0; i < ulEnd; i++)
		{
			if (memcmp(pucData + i, pucSerial, ulSerialLen) == 0)
				return true;
		}

		return false;
	}

	CByteArray CCard::ReadFile(const std::string & csPath,
				   unsigned long ulOffset,
				   unsigned long ulMaxLen, bool bDoNotCache)
	{
		return ReadUncachedFile(csPath, ulOffset, ulMaxLen);
	}

	tCacheInfo CCard::GetCacheInfo(const std::string & csPath)
	{
		// By default no caching, card must implement this method
		// to allow certain files to be cached (in a certain way).
		tCacheInfo dontCache = { DONT_CACHE, 0 };

		return dontCache;
	}

	unsigned long CCard::PinStatus(const tPin & Pin)
	{
		throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
	}

	bool CCard::PinCmd(tPinOperation operation, const tPin & Pin,
			   const std::string & csPin1,
			   const std::string & csPin2,
			   unsigned long &ulRemaining, const tPrivKey * pKey)
	{
		throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
	}

	DlgPinUsage CCard::PinUsage2Dlg(const tPin & Pin,
					const tPrivKey * pKey)
	{
		return DLG_PIN_UNKNOWN;
	}

	unsigned long CCard::GetSupportedAlgorithms()
	{
		return 0;
	}

	CByteArray CCard::Sign(const tPrivKey & key, const tPin & Pin,
			       unsigned long algo, const CByteArray & oData)
	{
		throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
	}

	CByteArray CCard::SendAPDU(const CByteArray & oCmdAPDU)
	{
		CAutoLock oAutoLock(this);
		long lRetVal = 0;

		CByteArray oResp =
			m_poContext->m_oPCSC.Transmit(m_hCard, oCmdAPDU,
						      &lRetVal);

		if (m_cardType == CARD_BEID &&
		    (lRetVal == SCARD_E_COMM_DATA_LOST
		     || lRetVal == SCARD_E_NOT_TRANSACTED))
		{
			m_poContext->m_oPCSC.Recover(m_hCard, &m_ulLockCount);
			// try again to select the applet
			CByteArray oData;
			CByteArray oCmd(40);
			const unsigned char Cmd[] =
				{ 0x00, 0xA4, 0x04, 0x00, 0x0F, 0xA0, 0x00,
		    0x00, 0x00, 0x30, 0x29, 0x05, 0x70, 0x00, 0xAD, 0x13,
		    0x10, 0x01, 0x01, 0xFF };
			oCmd.Append(Cmd, sizeof(Cmd));

			oData = m_poContext->m_oPCSC.Transmit(m_hCard, oCmd,
							      &lRetVal);

			if ((oData.Size() == 2) &&
			    ((oData.GetByte(0) == 0x61)
			     || ((oData.GetByte(0) == 0x90)
				 && (oData.GetByte(1) == 0x00))))
			{
				//try again, now that the card has been reset
				oResp = m_poContext->m_oPCSC.Transmit(m_hCard,
								      oCmdAPDU,
								      &lRetVal);
			}
		}

		if (oResp.Size() == 2)
		{
			// If SW1 = 0x61, then SW2 indicates the maximum value to be given to the
			// short Le  field (length of extra/ data still available) in a GET RESPONSE.
			if (oResp.GetByte(0) == 0x61)
				return SendAPDU(0xC0, 0x00, 0x00, oResp.GetByte(1));	// Get Response

			// If SW1 = 0x6c, then SW2 indicates the value to be given to the short
			// Le field (exact length of requested data) when re-issuing the same command.
			if (oResp.GetByte(0) == 0x6c)
			{
				unsigned long ulCmdLen = oCmdAPDU.Size();
				const unsigned char *pucCmd =
					oCmdAPDU.GetBytes();
				CByteArray oNewCmdAPDU(ulCmdLen);

				oNewCmdAPDU.Append(pucCmd, 4);
				oNewCmdAPDU.Append(oResp.GetByte(1));
				if (ulCmdLen > 5)
					oNewCmdAPDU.Append(pucCmd + 5,
							   ulCmdLen - 5);

				// for cards that may need a delay (e.g. Belpic V1)
				unsigned long ulDelay = Get6CDelay();

				if (ulDelay != 0)
					CThread::SleepMillisecs(ulDelay);

				return SendAPDU(oNewCmdAPDU);
			}
		}

		return oResp;
	}

	CByteArray CCard::SendAPDU(unsigned char ucINS, unsigned char ucP1,
				   unsigned char ucP2, unsigned long ulOutLen)
	{
		CByteArray oAPDU(5);

		oAPDU.Append(m_ucCLA);
		oAPDU.Append(ucINS);
		oAPDU.Append(ucP1);
		oAPDU.Append(ucP2);
		oAPDU.Append((unsigned char) ulOutLen);

		return SendAPDU(oAPDU);
	}

	CByteArray CCard::SendAPDU(unsigned char ucINS, unsigned char ucP1,
				   unsigned char ucP2,
				   const CByteArray & oData)
	{
		CByteArray oAPDU(5 + oData.Size());

		oAPDU.Append(m_ucCLA);
		oAPDU.Append(ucINS);
		oAPDU.Append(ucP1);
		oAPDU.Append(ucP2);
		oAPDU.Append((unsigned char) oData.Size());
		oAPDU.Append(oData);

		return SendAPDU(oAPDU);
	}


	CP15Correction *CCard::GetP15Correction()
	{
		return NULL;
	}

	unsigned long CCard::Get6CDelay()
	{
		return 0;
	}

	unsigned char CCard::Hex2Byte(char cHex)
	{
		if (cHex >= '0' && cHex <= '9')
			return (unsigned char) (cHex - '0');
		if (cHex >= 'A' && cHex <= 'F')
			return (unsigned char) (cHex - 'A' + 10);
		if (cHex >= 'a' && cHex <= 'f')
			return (unsigned char) (cHex - 'a' + 10);

		MWLOG(LEV_ERROR, MOD_CAL,
		      L"Invalid hex character 0x%02X found", cHex);
		throw CMWEXCEPTION(EIDMW_ERR_BAD_PATH);
	}

	unsigned char CCard::Hex2Byte(const std::string & csHex,
				      unsigned long ulIdx)
	{
		return (unsigned char) (16 * Hex2Byte(csHex[2 * ulIdx]) +
					Hex2Byte(csHex[2 * ulIdx + 1]));
	}

	bool CCard::IsDigit(char c)
	{
		return (c >= '0' && c <= '9');
	}

	unsigned long CCard::getSW12(const CByteArray & oRespAPDU,
				     unsigned long ulExpected)
	{
		unsigned long ulRespLen = oRespAPDU.Size();

		if (ulRespLen < 2)
		{
			MWLOG(LEV_ERROR, MOD_CAL,
			      L"Only %d byte(s) returned from the card",
			      ulRespLen);
			throw CMWEXCEPTION(EIDMW_ERR_CARD_COMM);
		}

		unsigned long ulSW12 =
			256 * oRespAPDU.GetByte(ulRespLen - 2) +
			oRespAPDU.GetByte(ulRespLen - 1);

		if (ulExpected != 0 && ulExpected != ulSW12)
		{
			MWLOG(LEV_WARN, MOD_CAL,
			      L"Card returned SW12 = %04X, expected %04X",
			      ulSW12, ulExpected);
			throw CMWEXCEPTION(m_poContext->m_oPCSC.
					   SW12ToErr(ulSW12));
		}

		return ulSW12;
	}

////////////////////////////////////////////////////////////////:

      CAutoLock::CAutoLock(CCard * poCard):m_poCard(poCard), m_poPCSC(NULL),
		m_hCard(0)
	{
		m_poCard->Lock();
	}

      CAutoLock::CAutoLock(CPCSC * poPCSC, SCARDHANDLE hCard):m_poCard(NULL), m_poPCSC(poPCSC),
		m_hCard
		(hCard)
	{
		poPCSC->BeginTransaction(hCard);
	}

	CAutoLock::~CAutoLock()
	{
		if (m_poCard)
			m_poCard->Unlock();
		else
			m_poPCSC->EndTransaction(m_hCard);
	}

}
