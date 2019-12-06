
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
#include "common/thread.h"
#include "pinpad2.h"

#ifdef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
static std::string fuzz_path = "";
static eIDMW::CByteArray fuzz_data = eIDMW::CByteArray();
extern "C" {
	void beid_set_fuzz_data(const uint8_t *data, size_t size, const char *path) {
		fuzz_path = path;
		fuzz_data = eIDMW::CByteArray(data, size);
	}
}
#endif

using namespace eIDMW;
static const unsigned char APPLET_AID[] =
{ 0xA0, 0x00, 0x00, 0x00, 0x30, 0x29, 0x05, 0x70, 0x00, 0xAD, 0x13,
0x10, 0x01, 0x01, 0xFF };
static const unsigned char BELPIC_AID[] =
{ 0xA0, 0x00, 0x00, 0x01, 0x77, 0x50, 0x4B, 0x43, 0x53, 0x2D, 0x31,
0x35 };

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

	CCard::CCard(SCARDHANDLE hCard, CPCSC * poPCSC, CPinpad * poPinpad, tSelectAppletMode selectAppletMode, tCardType cardType)
	  : m_hCard(hCard), m_poPCSC(poPCSC), m_poPinpad(poPinpad), m_cardType(cardType), m_ulLockCount(0),
	    m_bSerialNrString(false), m_selectAppletMode(selectAppletMode), m_pinUsage(BEID_PINS_USE_ONE_PIN), m_ucAppletVersion(0), m_ul6CDelay(0), m_oPKCS15()
	{
		try
		{
			//Get Card Data (compatible with all applets)
			m_oCardData = SendAPDU(0x80, 0xE4, 0x00, 0x00, 0x1C);

			m_ulRemaining[0] = 1;
			if (m_oCardData.Size() < 23)
			{
				throw CMWEXCEPTION(EIDMW_ERR_APPLET_VERSION_NOT_FOUND);
			}
			m_oCardData.Chop(2);	// remove SW12 = '90 00'
			m_oSerialNr = CByteArray(m_oCardData.GetBytes(), 16);
			m_ucAppletVersion = m_oCardData.GetByte(21);
			if (m_ucAppletVersion >= 0x18) {
				// Use applet 1.8-specific extended card data
				m_oCardData = SendAPDU(0x80,0xE4, 0x00, 0x01, 0x1F);
				m_oCardData.Chop(2);
				m_ulRemaining[0] = m_oCardData.GetByte(28);
				m_ulRemaining[1] = m_oCardData.GetByte(29);
				m_ulRemaining[2] = m_oCardData.GetByte(30);
				if (m_ulRemaining[1] != 0xFF || m_ulRemaining[2] != 0xFF) {
//TODO: check this when we get feedback on the usecase
					m_pinUsage = BEID_PINS_USE_SEPARATE_PINS;
				}
			}
			if (m_oCardData.GetByte(22) == 0x00 && m_oCardData.GetByte(23) == 0x01)
			{
				m_ul6CDelay = 50;
			}
			m_oPKCS15.SetCard(this);
		}
		catch (CMWException &e)
		{
			MWLOG(LEV_CRIT, MOD_CAL, L"Failed to get CardData: 0x%0x", e.GetError());
			Disconnect(DISCONNECT_LEAVE_CARD);
		}
		catch (...)
		{
			MWLOG(LEV_CRIT, MOD_CAL, L"Failed to get CardData");
			Disconnect(DISCONNECT_LEAVE_CARD);
		}
	}

	CCard::~CCard(void)
	{
		try {
			Disconnect(DISCONNECT_LEAVE_CARD);
		} catch(CMWException&) {
		}
	}


	void CCard::Disconnect(tDisconnectMode disconnectMode)
	{
		if (m_hCard != 0)
		{
			SCARDHANDLE hTemp = m_hCard;

			m_hCard = 0;
			m_poPCSC->Disconnect(hTemp, disconnectMode);
			m_oPKCS15.Clear(NULL);
		}
	}

	CByteArray CCard::GetATR()
	{
		return m_poPCSC->GetATR(m_hCard);
	}

	CByteArray CCard::GetIFDVersion()
	{
		return m_poPCSC->GetIFDVersion(m_hCard);
	}

	bool CCard::Status()
	{
		return m_poPCSC->Status(m_hCard);
	}

	bool CCard::IsPinpadReader()
	{
		return m_poPinpad->UsePinpad(PIN_OP_VERIFY);
	}

	std::string CCard::GetSerialNr()
	{
		if (!m_bSerialNrString)
		{
			m_csSerialNr = GetSerialNrBytes().ToString(false, true);
			m_bSerialNrString = true;
		}

		return m_csSerialNr;
	}

	std::string CCard::GetCardLabel()
	{
		return "BELPIC";// m_oPKCS15.GetCardLabel();
	}

	tPin CCard::GetPinFor(tPinObjective what) {
		if(m_pinUsage == BEID_PINS_USE_ONE_PIN) {
			return PinBeid;
		}
		switch(what) {
			case BEID_PIN_READ_EF:
				return PinBeidEF;
			case BEID_PIN_AUTH:
				return PinBeidAuth;
			case BEID_PIN_NONREP:
				return PinBeidSign;
		}
		return PinInvalid;
	}

	tPin CCard::GetPinByKeyID(unsigned long key) {
		switch(key) {
			case 0x82:
				return GetPinFor(BEID_PIN_AUTH);
			case 0x83:
				return GetPinFor(BEID_PIN_NONREP);
		}
		return PinInvalid;
	}

	CByteArray CCard::ReadRecordFromFile(const std::string & csPath, unsigned char ucRecordID)
	{
		try
		{
			return ReadRecord(csPath, ucRecordID);
		}
		catch (const CNotAuthenticatedException & e)
		{
			(void)e.GetError();

			/*  unused feature
			// A PIN is needed to read -> ask the correct PIN and do a verification
			unsigned long ulRemaining;

			tPin pin = GetPinFor(BEID_PIN_READ_EF);

			if (pin.bValid)
			{
				if (PinCmd(PIN_OP_VERIFY, pin, "", "", ulRemaining, NULL))
				{
					return ReadRecord(csPath, ucRecordID);
				}
				else {
					throw CMWEXCEPTION(ulRemaining == 0 ? EIDMW_ERR_PIN_BLOCKED : EIDMW_ERR_PIN_BAD);
				}
			}
			else
			*/
			throw CMWEXCEPTION(EIDMW_ERR_CMD_NOT_ALLOWED);
		}
	}

	CByteArray CCard::ReadCardFile(const std::string & csPath, unsigned long ulOffset, unsigned long ulMaxLen)
	{
		try
		{
			return ReadFile(csPath, ulOffset, ulMaxLen);
		}
		catch (const CNotAuthenticatedException & e)
		{
			(void)e.GetError();
			/*
			// unused feature
			// A PIN is needed to read -> ask the correct PIN and do a verification
			unsigned long ulRemaining;

			tPin pin = GetPinFor(BEID_PIN_READ_EF);


			if (pin.bValid)
			{
				if (PinCmd(PIN_OP_VERIFY, pin, "", "", ulRemaining, NULL))
				{
					return ReadFile(csPath, ulOffset, ulMaxLen);
				}
				else {
					throw CMWEXCEPTION(ulRemaining == 0 ? EIDMW_ERR_PIN_BLOCKED : EIDMW_ERR_PIN_BAD);
				}
			}
			else
			*/
			throw CMWEXCEPTION(EIDMW_ERR_CMD_NOT_ALLOWED);
		}
	}

	unsigned long CCard::PinCount()
	{
		return (m_pinUsage == BEID_PINS_USE_ONE_PIN) ? 1 : 3;
	}

	unsigned long CCard::CertCount()
	{
		return m_oPKCS15.CertCount();
	}

	tCert CCard::GetCert(unsigned long ulIndex)
	{
		return m_oPKCS15.GetCert(ulIndex);
	}

	tCert CCard::GetCertByID(unsigned long ulID)
	{
		return m_oPKCS15.GetCertByID(ulID);
	}

	unsigned long CCard::PrivKeyCount()
	{
		return m_oPKCS15.PrivKeyCount();
	}

	tPrivKey CCard::GetPrivKey(unsigned long ulIndex)
	{
		return m_oPKCS15.GetPrivKey(ulIndex);
	}

	tPrivKey CCard::GetPrivKeyByID(unsigned long ulID)
	{
		return m_oPKCS15.GetPrivKeyByID(ulID);
	}

	unsigned long CCard::GetCardSupportedAlgorithms()
	{
		unsigned long algos = GetSupportedAlgorithms();

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

	CByteArray CCard::CardSign(const tPrivKey & key, unsigned long algo, const CByteArray & oData)
	{
		unsigned long ulSupportedAlgos = GetSupportedAlgorithms();

		if (algo & ulSupportedAlgos)
			return Sign(key, GetPinByKeyID(key.ulKeyRef), algo, oData);
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
				return Sign(key, GetPinByKeyID(key.ulKeyRef), SIGN_ALGO_RSA_PKCS, oAID_Data);
			}
			else if (ulSupportedAlgos & SIGN_ALGO_RSA_RAW)
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

				return Sign(key, GetPinByKeyID(key.ulKeyRef), SIGN_ALGO_RSA_RAW, oData);
			}
			else
				throw CMWEXCEPTION(EIDMW_ERR_CHECK);
		}
	}

	void CCard::Lock()
	{
		if (m_ulLockCount == 0)
			m_poPCSC->BeginTransaction(m_hCard);
		m_ulLockCount++;
	}

	void CCard::Unlock()
	{
		if (m_ulLockCount == 0)
			MWLOG(LEV_ERROR, MOD_CAL, L"More Unlock()s then Lock()s called!!");
		else
		{
			m_ulLockCount--;
			if (m_ulLockCount == 0)
				m_poPCSC->EndTransaction(m_hCard);
		}
	}

/*	void CCard::SelectApplication(const CByteArray & oAID)
	{
		CAutoLock autolock(this);

		if (m_selectAppletMode == ALW_SELECT_APPLET)
			SelectApplet();

		// Select File command to select the Application by AID
		CByteArray oResp = SendAPDU(0xA4, 0x04, 0x0C, oAID);

		if (ShouldSelectApplet(0xA4, getSW12(oResp)))
		{
			// First try to select the applet
			if (SelectApplet())
			{
				m_selectAppletMode = ALW_SELECT_APPLET;
				oResp = SendAPDU(0xA4, 0x04, 0x0C, oAID);
			}
		}

		getSW12(oResp, 0x9000);
	}*/

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

	CByteArray CCard::ReadFile(const std::string & csPath, unsigned long ulOffset, unsigned long ulMaxLen)
	{
#ifdef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
		if(csPath == fuzz_path) {
			return fuzz_data;
		}
		return CByteArray();
#endif
		CByteArray oData(ulMaxLen);
		CAutoLock autolock(this);
		SelectFile(csPath);

		// Loop until we've read ulMaxLen bytes or until EOF (End Of File)
		bool bEOF = false;

		for (unsigned long i = 0; i < ulMaxLen && !bEOF; i += MAX_APDU_READ_LEN)
		{
			unsigned long ulLen = ulMaxLen - i <= MAX_APDU_READ_LEN ? ulMaxLen - i : MAX_APDU_READ_LEN;

			CByteArray oResp = ReadBinary(ulOffset + i, ulLen);

			unsigned long ulSW12 = getSW12(oResp);

			// If the file is a multiple of the block read size, you will get
			// an SW12 = 6B00 (at least with BE eID) but that OK then..
			if (ulSW12 == 0x9000 || (i != 0 && ulSW12 == 0x6B00))
			{
				oData.Append(oResp.GetBytes(), oResp.Size() - 2);
			}
			else if (ulSW12 == 0x6982)
			{
				throw CNotAuthenticatedException (EIDMW_ERR_NOT_AUTHENTICATED);
			}
			else if (ulSW12 == 0x6B00)
			{
				throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);
			}
			else
			{
				if (ulSW12 == 0x6D00)
				{
					throw CMWEXCEPTION(EIDMW_ERR_NOT_ACTIVATED);
				}
				else
				{
					throw CMWEXCEPTION(m_poPCSC->SW12ToErr(ulSW12));
				}
			}
			// If the driver/reader itself did the 6CXX handling,
			// we assume we're at the EOF
			if (oResp.Size() < MAX_APDU_READ_LEN)
			{
				bEOF = true;
			}
		}

		MWLOG(LEV_INFO, MOD_CAL,L"   Read file %ls (%d bytes) from card", utilStringWiden(csPath).c_str(), oData.Size());

		return oData;
	}

	CByteArray CCard::ReadRecord(const std::string & csPath, unsigned char ulRecordID)
	{
		CAutoLock autolock(this);
		SelectByPath(csPath);
		unsigned long ulSW12 = 0;

		//Le = 0 to read 256 bytes
		CByteArray oResp = SendAPDU(0x80, 0xB2, ulRecordID, ulRecordID, 0);

		MWLOG(LEV_INFO, MOD_CAL, L"   ReadRecord %ls (0X%x recordID) from card", utilStringWiden(csPath).c_str(), ulRecordID);
		
		ulSW12 = getSW12(oResp);
		switch (ulSW12)
		{
		case 0x6982:
			throw CNotAuthenticatedException(EIDMW_ERR_NOT_AUTHENTICATED);
			break;
		case 0x6B00:
			throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);
			break;
		case 0x6D00:
			throw CMWEXCEPTION(EIDMW_ERR_NOT_ACTIVATED);
			break;
		case 0x9000:
			break;
		default:
			throw CMWEXCEPTION(m_poPCSC->SW12ToErr(ulSW12));
		}
		return oResp;
	}

	CByteArray CCard::SendAPDU(const CByteArray & oCmdAPDU)
	{
		CAutoLock oAutoLock(this);
		long lRetVal = 0;
		unsigned long ulSize = 0;

		CByteArray oResp = m_poPCSC->Transmit(m_hCard, oCmdAPDU, &lRetVal);

		//try to recover from communication issues with the card
		if (m_cardType == CARD_BEID && (lRetVal == SCARD_E_COMM_DATA_LOST || lRetVal == SCARD_E_NOT_TRANSACTED))
		{
			m_poPCSC->Recover(m_hCard, &m_ulLockCount);
			// try again to select the applet
			CByteArray oData;
			CByteArray oCmd(40);
			const unsigned char Cmd[] =
			{ 0x00, 0xA4, 0x04, 0x00, 0x0F, 0xA0, 0x00,
			  0x00, 0x00, 0x30, 0x29, 0x05, 0x70, 0x00, 0xAD, 0x13,
			  0x10, 0x01, 0x01, 0xFF };
			oCmd.Append(Cmd, sizeof(Cmd));

			oData = m_poPCSC->Transmit(m_hCard, oCmd, &lRetVal);

			if ((oData.Size() == 2) && ((oData.GetByte(0) == 0x61)
				|| ((oData.GetByte(0) == 0x90) && (oData.GetByte(1) == 0x00))))
			{
				//try again, now that the card has been reset
				oResp = m_poPCSC->Transmit(m_hCard, oCmdAPDU, &lRetVal);
			}
		}
		ulSize = oResp.Size();
		if (ulSize == 2)
		{
			// If SW1 = 0x61, then SW2 indicates the maximum value to be given to the
			// short Le  field (length of data still available) in a GET RESPONSE.
			if (oResp.GetByte(0) == 0x61)
			{
				//create the GET RESPONSE command
				CByteArray oCommand(10);
				const unsigned char Command[] = { 0x00, 0xC0, 0x00, 0x00 };
				oCommand.Append(Command, sizeof(Command));

				//add the correct length
				oCommand.Append(oResp.GetByte(1));

				return SendAPDU(oCommand);	// Get Response
			}

			// If SW1 = 0x6c, then SW2 indicates the value to be given to the short
			// Le field (exact length of requested data) when re-issuing the same command.
			if (oResp.GetByte(0) == 0x6c)
			{
				unsigned long ulCmdLen = oCmdAPDU.Size();
				const unsigned char *pucCmd = oCmdAPDU.GetBytes();
				CByteArray oNewCmdAPDU(ulCmdLen);

				oNewCmdAPDU.Append(pucCmd, 4);
				oNewCmdAPDU.Append(oResp.GetByte(1));
				if (ulCmdLen > 5)
				{
					oNewCmdAPDU.Append(pucCmd + 5, ulCmdLen - 5);
				}
				// for cards that may need a delay (e.g. Belpic V1)
				unsigned long ulDelay = Get6CDelay();

				if (ulDelay != 0)
				{
					CThread::SleepMillisecs(ulDelay);
				}
				return SendAPDU(oNewCmdAPDU);
			}
		}
		else if (ulSize == 258)
		{
			//use response command chaining to get the entire result, if resultdata > 256
			// If SW1 = 0x61, then SW2 indicates the value to be given to the
			// short Le field (length of data still available) in a GET RESPONSE to receive the extra data.
			while (oResp.GetByte(ulSize-2) == 0x61) 
			{
				//create the GET RESPONSE command
				CByteArray oCommand(10);
				const unsigned char Command[] = { 0x00, 0xC0, 0x00, 0x00 };
				oCommand.Append(Command, sizeof(Command));
				
				oCommand.Append(oResp.GetByte(ulSize-1)); //add the correct length

				CByteArray oExtraBytes = SendAPDU(oCommand);	// Get extra bytes from the get response command
				oResp.Chop(2);									//remove the status word bytes
				oResp.Append(oExtraBytes);						//apend the extra bytes

				ulSize += oExtraBytes.Size();
			}
		}

		return oResp;
	}

	CByteArray CCard::SendAPDU(unsigned char ucCLA, unsigned char ucINS, unsigned char ucP1,
				   unsigned char ucP2, unsigned long ulOutLen)
	{
		CByteArray oAPDU(5);

		oAPDU.Append(ucCLA);
		oAPDU.Append(ucINS);
		oAPDU.Append(ucP1);
		oAPDU.Append(ucP2);
		oAPDU.Append((unsigned char) ulOutLen);

		return SendAPDU(oAPDU);
	}

	CByteArray CCard::SendAPDU(unsigned char ucCLA, unsigned char ucINS, unsigned char ucP1, unsigned char ucP2, const CByteArray & oData)
	{
		CByteArray oAPDU(5 + oData.Size());

		oAPDU.Append(ucCLA);
		oAPDU.Append(ucINS);
		oAPDU.Append(ucP1);
		oAPDU.Append(ucP2);
		oAPDU.Append((unsigned char) oData.Size());
		oAPDU.Append(oData);

		return SendAPDU(oAPDU);
	}

	unsigned char CCard::Hex2Byte(char cHex)
	{
		if (cHex >= '0' && cHex <= '9')
			return (unsigned char) (cHex - '0');
		if (cHex >= 'A' && cHex <= 'F')
			return (unsigned char) (cHex - 'A' + 10);
		if (cHex >= 'a' && cHex <= 'f')
			return (unsigned char) (cHex - 'a' + 10);

		MWLOG(LEV_ERROR, MOD_CAL, L"Invalid hex character 0x%02X found", cHex);
		throw CMWEXCEPTION(EIDMW_ERR_BAD_PATH);
	}

	unsigned char CCard::Hex2Byte(const std::string & csHex, unsigned long ulIdx)
	{
		return (unsigned char) (16 * Hex2Byte(csHex[2 * ulIdx]) + Hex2Byte(csHex[2 * ulIdx + 1]));
	}

	bool CCard::IsDigit(char c)
	{
		return (c >= '0' && c <= '9');
	}

	unsigned long CCard::getSW12(const CByteArray & oRespAPDU, unsigned long ulExpected)
	{
		unsigned long ulRespLen = oRespAPDU.Size();

		if (ulRespLen < 2)
		{
			MWLOG(LEV_ERROR, MOD_CAL, L"Only %d byte(s) returned from the card", ulRespLen);
			throw CMWEXCEPTION(EIDMW_ERR_CARD_COMM);
		}

		unsigned long ulSW12 = 256 * oRespAPDU.GetByte(ulRespLen - 2) + oRespAPDU.GetByte(ulRespLen - 1);

		if (ulExpected != 0 && ulExpected != ulSW12)
		{
			MWLOG(LEV_WARN, MOD_CAL, L"Card returned SW12 = %04X, expected %04X", ulSW12, ulExpected);
			throw CMWEXCEPTION(m_poPCSC->SW12ToErr(ulSW12));
		}

		return ulSW12;
	}


	static bool BeidCardSelectApplet(CPCSC * poPCSC, SCARDHANDLE hCard)
	{
		long lRetVal = 0;
		unsigned char tucSelectApp[] = { 0x00, 0xA4, 0x04, 0x00 };
		CByteArray oCmd(40);
		oCmd.Append(tucSelectApp, sizeof(tucSelectApp));
		oCmd.Append((unsigned char) sizeof(APPLET_AID));
		oCmd.Append(APPLET_AID, sizeof(APPLET_AID));
		CByteArray oResp;
		try
		{
			oResp = poPCSC->Transmit(hCard, oCmd, &lRetVal);
		}
		catch (CMWException &e)
		{
			MWLOG(LEV_CRIT, MOD_CAL, L"Failed to select applet: 0x%0x", e.GetError());
			return false;
		}
		catch (...)
		{
			MWLOG(LEV_CRIT, MOD_CAL, L"Failed to select applet");
			return false;
		}
		return (oResp.Size() == 2 && (oResp.GetByte(0) == 0x90 || oResp.GetByte(1) == 0x00));
	}

	CCard * UnknownCardGetInstance(const char *csReader,
		SCARDHANDLE hCard, CPCSC * poPCSC, CPinpad * poPinpad)
	{
		CCard *poCard = new CCard(hCard, poPCSC, poPinpad, DONT_SELECT_APPLET, CARD_UNKNOWN);

		return poCard;
	}

	CCard * BeidCardGetInstance(const char *csReader,
		SCARDHANDLE hCard, CPCSC * poPCSC, CPinpad * poPinpad)
	{
		CCard * poCard = NULL;

		try
		{
			bool bIsBeidCard = BeidCardSelectApplet(poPCSC, hCard);

			if (bIsBeidCard)
			{
				poCard = new CCard(hCard, poPCSC, poPinpad, TRY_SELECT_APPLET, CARD_BEID);
			}
			else
			{
				poCard = new CCard(hCard, poPCSC, poPinpad, DONT_SELECT_APPLET, CARD_UNKNOWN);
			}
		}
		catch (...)
		{
			MWLOG(LEV_ERROR, MOD_CAL, L"Exception in cardPluginBeid.CardGetInstance()");
		}

		return poCard;
	}

	CByteArray CCard::GetSerialNrBytes()
	{
		return m_oSerialNr;
	}

	CByteArray CCard::GetInfo()
	{
		return m_oCardData;
	}

	std::string CCard::GetPinpadPrefix()
	{
		return "beidpp";
	}

	unsigned long CCard::PinStatus(const tPin & Pin)
	{
		// This command isn't supported on V1 cards
		if (m_oCardData.GetByte(21) < 0x20)
			return PIN_STATUS_UNKNOWN;
		try
		{
			CByteArray oResp = SendAPDU(0x80, 0xEA, 0x00, (unsigned char)Pin.ulAuthID, 1);
			getSW12(oResp, 0x9000);
			return oResp.GetByte(0);
		}
		catch (...)
		{
			throw;
		}
	}

	DlgPinUsage CCard::PinUsage2Dlg(const tPin & Pin,
		const tPrivKey * pKey)
	{
		DlgPinUsage usage = DLG_PIN_UNKNOWN;
		if (pKey != NULL)
		{
			if (pKey->ulID == 2)
				usage = DLG_PIN_AUTH;

			else if (pKey->ulID == 3)
				usage = DLG_PIN_SIGN;
		}
		else if (Pin.ulAuthID == 4)
			usage = DLG_PIN_SIGN;

		else
			usage = DLG_PIN_AUTH;
		return usage;
	}

	void CCard::showPinDialog(tPinOperation operation, const tPin & Pin,
		std::string & csPin1, std::string & csPin2, const tPrivKey * pKey)
	{

#ifndef NO_DIALOGS
		// Convert params
		wchar_t wsPin1[PIN_MAX_LENGTH + 1];
		wchar_t wsPin2[PIN_MAX_LENGTH + 1];
		DlgPinOperation pinOperation = PinOperation2Dlg(operation);
		DlgPinUsage usage = PinUsage2Dlg(Pin, pKey);
		DlgPinInfo pinInfo = { Pin.ulMinLen, Pin.ulMaxLen, PIN_FLAG_DIGITS };

		// The actual call
		DlgRet ret;
		std::wstring wideLabel = utilStringWiden(Pin.csLabel);
		if (operation == PIN_OP_CHANGE) {
			ret = DlgAskPins(pinOperation, usage, wideLabel.c_str(),
				pinInfo, wsPin1, PIN_MAX_LENGTH + 1,
				pinInfo, wsPin2, PIN_MAX_LENGTH + 1);
		}
		else {
			ret = DlgAskPin(pinOperation, usage, wideLabel.c_str(),
				pinInfo, wsPin1, PIN_MAX_LENGTH + 1);
		}

		// Convert back
		if (ret == DLG_OK)
		{
			csPin1 = utilStringNarrow(wsPin1);
			if (operation == PIN_OP_CHANGE)
				csPin2 = utilStringNarrow(wsPin2);
		}

		else if (ret == DLG_CANCEL)
			throw CMWEXCEPTION(EIDMW_ERR_PIN_CANCEL);

		else if (ret == DLG_BAD_PARAM)
			throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);

		else
			throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);

#endif	/* NO_DIALOGS */
	}

	bool CCard::LogOff()
	{
		// No PIN has to be specified
		CByteArray oResp = SendAPDU(0x80, 0xE6, 0x00, 0x00, 0);

		getSW12(oResp, 0x9000);
		return true;
	}

	unsigned long CCard::GetSupportedAlgorithms()
	{
		unsigned long ulAlgos;
		if (m_ucAppletVersion < 0x18) {
			ulAlgos = SIGN_ALGO_RSA_PKCS | SIGN_ALGO_MD5_RSA_PKCS | SIGN_ALGO_SHA1_RSA_PKCS;
			if (m_ucAppletVersion == 0x17)
			{
				ulAlgos |= SIGN_ALGO_SHA256_RSA_PKCS;
				ulAlgos |= SIGN_ALGO_SHA1_RSA_PSS;
				ulAlgos |= SIGN_ALGO_SHA256_RSA_PSS;
			}
		}
		else {
			ulAlgos = SIGN_ALGO_SHA256_ECDSA | SIGN_ALGO_SHA384_ECDSA | SIGN_ALGO_SHA512_ECDSA | 
				SIGN_ALGO_SHA3_256_ECDSA | SIGN_ALGO_SHA3_384_ECDSA | SIGN_ALGO_SHA3_512_ECDSA | SIGN_ALGO_ECDSA_RAW;
		}
		return ulAlgos;
	}
//algo only allowed on applets before applet 1.8
#define ALLOWED_PRE_APPLET18_ONLY if(m_ucAppletVersion >= 0x18) { MWLOG(LEV_WARN, MOD_CAL, L"MSE SET: RSA algorithms not supported on V1.8+ cards"); throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);}

//algo only allowed on applets after applet 1.7
#define ALLOWED_POST_APPLET17_ONLY if(m_ucAppletVersion < 0x18) { MWLOG(LEV_WARN, MOD_CAL, L"MSE SET: ECDSA algorithms not supported on pre V1.8 cards"); throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);}

//algo only allowed on applet 1.7
#define ALLOWED_APPLET17_ONLY if(m_ucAppletVersion != 0x17) { MWLOG(LEV_WARN, MOD_CAL, L"MSE SET: PSS not supported on pre V1.7 cards"); throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);}
	
	void CCard::SetSecurityEnv(const tPrivKey & key, unsigned long algo, unsigned long ulInputLen)
	{
		// Data = [04 80 <algoref> 84 <keyref>]  (5 bytes)
		CByteArray oData(5);

		unsigned char ucAlgo;

		switch (algo)
		{
			case SIGN_ALGO_RSA_PKCS:
				ALLOWED_PRE_APPLET18_ONLY;
				ucAlgo = 0x01;
				break;
			case SIGN_ALGO_SHA1_RSA_PKCS:
				ALLOWED_PRE_APPLET18_ONLY;
				ucAlgo = 0x02;
				break;
			case SIGN_ALGO_MD5_RSA_PKCS:
				ALLOWED_PRE_APPLET18_ONLY;
				ucAlgo = 0x04;
				break;
			case SIGN_ALGO_SHA256_RSA_PKCS:
				if (m_ucAppletVersion < 0x17)
				{
					MWLOG(LEV_WARN, MOD_CAL, L"MSE SET: SIGN_ALGO_SHA256_RSA_PKCS not supported on pre V1.7 cards");
					throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
				}
				ucAlgo = 0x08;
				break;
			case SIGN_ALGO_SHA1_RSA_PSS:
				ALLOWED_APPLET17_ONLY;
				ucAlgo = 0x10;
				break;
			case SIGN_ALGO_SHA256_RSA_PSS:
				ALLOWED_APPLET17_ONLY
				ucAlgo = 0x20;
				break;
			case SIGN_ALGO_SHA256_ECDSA:
				ALLOWED_POST_APPLET17_ONLY;
				ucAlgo = 0x01;
				break;
			case SIGN_ALGO_SHA384_ECDSA:
				ALLOWED_POST_APPLET17_ONLY;
				ucAlgo = 0x02;
				break;
			case SIGN_ALGO_SHA512_ECDSA:
				ALLOWED_POST_APPLET17_ONLY;
				ucAlgo = 0x04;
				break;
			case SIGN_ALGO_SHA3_256_ECDSA:
				ALLOWED_POST_APPLET17_ONLY;
				ucAlgo = 0x08;
				break;
			case SIGN_ALGO_SHA3_384_ECDSA:
				ALLOWED_POST_APPLET17_ONLY;
				ucAlgo = 0x10;
				break;
			case SIGN_ALGO_SHA3_512_ECDSA:
				ALLOWED_POST_APPLET17_ONLY;
				ucAlgo = 0x20;
				break;
			case SIGN_ALGO_ECDSA_RAW:
				ALLOWED_POST_APPLET17_ONLY;
				ucAlgo = 0x40;
				break;
			default:
				throw CMWEXCEPTION(EIDMW_ERR_ALGO_BAD);
		}

		oData.Append(0x04);
		oData.Append(0x80);
		oData.Append(ucAlgo);
		oData.Append(0x84);
		oData.Append((unsigned char)key.ulKeyRef);
		CByteArray oResp = SendAPDU(0x00, 0x22, 0x41, 0xB6, oData);
		if (ShouldSelectApplet(0x22, getSW12(oResp)))
		{
			if (SelectApplet())
			{
				m_selectAppletMode = ALW_SELECT_APPLET;
				oResp = SendAPDU(0x00, 0x22, 0x41, 0xB6, oData);
			}
		}
		getSW12(oResp, 0x9000);
	}

	CByteArray CCard::SignInternal(const tPrivKey & key, unsigned long algo,
		const CByteArray & oData, const tPin * pPin)
	{
		std::string csReadPin1;
		std::string csReadPin2;
		std::string & csPin1 = csReadPin1;
		std::string & csPin2 = csReadPin2;
		if (pPin != NULL)
		{
			AskPin(PIN_OP_VERIFY, *pPin, csPin1, csPin2, &key);
		}
		CAutoLock autolock(this);

		// For V1.7 cards, the Belpic dir (3F00DF00) has to be selected
		if (m_ucAppletVersion >= 0x17)
		{
			SelectFile("3F00DF00");
		}
		else 
		{
			if (m_selectAppletMode == ALW_SELECT_APPLET)
			{
				SelectApplet();
			}
		}
		SetSecurityEnv(key, algo, oData.Size());

		// Pretty unique for smart cards: first MSE SET, then verify PIN
		// (needed for the nonrep key/pin, but also usable for the auth key/pin)
		if (pPin != NULL)
		{
			unsigned long ulRemaining = 0;

			bool bOK = PinCmd(PIN_OP_VERIFY, *pPin, csPin1, csPin2, ulRemaining, &key);
			if (!bOK)
			{
				m_ulRemaining[pPin->ulIndex] = ulRemaining;
				throw CMWEXCEPTION(ulRemaining == 0 ? EIDMW_ERR_PIN_BLOCKED : EIDMW_ERR_PIN_BAD);
			}
		}

		// PSO: Compute Digital Signature
		CByteArray oResp = SendAPDU(0x00, 0x2A, 0x9E, 0x9A, oData);
		unsigned long ulSW12 = getSW12(oResp);

		if (ulSW12 != 0x9000) {
			throw CMWEXCEPTION(m_poPCSC->SW12ToErr(ulSW12));
		}

		// Remove SW1-SW2 from the response
		oResp.Chop(2);
		return oResp;
	}

	bool CCard::ShouldSelectApplet(unsigned char ins, unsigned long ulSW12)
	{
		if (m_selectAppletMode != TRY_SELECT_APPLET)
			return false;
		if (ins == 0xA4)
			return ulSW12 == 0x6A82 || ulSW12 == 0x6A86;

		return ulSW12 == 0x6A82 || ulSW12 == 0x6A86 || ulSW12 == 0x6D00;
	}

	bool CCard::SelectApplet()
	{
		return BeidCardSelectApplet(m_poPCSC, m_hCard);
	}
/*
	tBelpicDF CCard::getDF(const std::string & csPath,
		unsigned long &ulOffset)
	{
		ulOffset = 0;
		if (csPath.substr(0, 4) == "3F00")
		{
			ulOffset = 4;
		}
		if (ulOffset < csPath.size())
		{
			std::string csPartialPath = csPath.substr(ulOffset, 4);
			if (csPartialPath == "DF00")
				return BELPIC_DF;

			//if (csPartialPath == "DF01" && m_ucAppletVersion >= 0x20)
			//      return ID_DF; // this AID doesn't exist for V1 cards
			//this AID doesn't exist for applet v1.7
		}
		return UNKNOWN_DF;
	}*/
	
	unsigned long CCard::Get6CDelay()
	{
		return m_ul6CDelay;
	}

	unsigned int CCard::GetPrivKeySize()
	{
		if (m_ucAppletVersion >= 0x18)
		{
			// TODO: The below may be anything between 256
			// and 521. Figure out what the right size is
			// from the actual card, rather than hardcoding.
			// For now this will work, however.
			return 256;
		}
		if (m_ucAppletVersion >= 0x17)
		{
			return 2048;
		}
		else
		{
			return 1024;
		}
	}

	unsigned char CCard::GetAppletVersion()
	{
		return m_ucAppletVersion;
	}

	unsigned char CCard::PinUsage2Pinpad(const tPin & Pin, const tPrivKey * pKey)
	{
		DlgPinUsage dlgUsage = PinUsage2Dlg(Pin, pKey);
		unsigned char ucPinpadUsage = EIDMW_PP_TYPE_UNKNOWN;

		switch (dlgUsage)
		{
		case DLG_PIN_AUTH:
			ucPinpadUsage = EIDMW_PP_TYPE_AUTH;
			break;
		case DLG_PIN_SIGN:
			ucPinpadUsage = EIDMW_PP_TYPE_SIGN;
			break;
		case DLG_PIN_READ_EF:
			ucPinpadUsage = EIDMW_PP_TYPE_READ_EF;
			break;
		default:
			break;
		}

		return ucPinpadUsage;
	}

	void CCard::AskPin(tPinOperation operation, const tPin & Pin,
		std::string & csPin1, std::string & csPin2, const tPrivKey * pKey)
	{
		bool bAskPIN = csPin1.empty();
		bool bUsePinpad = bAskPIN ? m_poPinpad->UsePinpad(operation) : false;

		// If no Pin(s) provided and it's no Pinpad reader -> ask Pins
		if (bAskPIN && !bUsePinpad)
		{
			showPinDialog(operation, Pin, csPin1, csPin2, pKey);
		}
	}

	bool CCard::AskPinRetry(tPinOperation operation, const tPin & Pin,
		unsigned long ulRemaining, const tPrivKey * pKey)
	{
		bool bRetry = false;
		bool bUsePinpad = m_poPinpad->UsePinpad(operation);

		// Bad PIN: show a dialog to ask the user to try again
		// PIN blocked: show a dialog to tell the user
		if (!bUsePinpad)
		{
#ifdef NO_DIALOGS
			return false;
#endif
			DlgPinUsage usage = PinUsage2Dlg(Pin, pKey);
			DlgRet dlgret = DlgBadPin(usage, utilStringWiden(Pin.csLabel).c_str(), ulRemaining);
			if (0 != ulRemaining && DLG_RETRY == dlgret)
			{
				bRetry = true;
			}
		}
		return bRetry;
	}

	/*bool CCard::PinCmd(tPinOperation operation, const tPin & Pin, const std::string & csPin1,
		const std::string & csPin2, unsigned long &ulRemaining, const tPrivKey * pKey)
	{
		tPin beidPin = Pin;

		// There's a path in the EF(AODF) for the PINs, but it's
		// not necessary, so we can save a Select File command
		//beidPin.csPath = "";

		// Encoding is Global Platform, there is/was no way to encode
		// this in PKCS15 AODF so it says/said erroneously "BCD encoding".
		// this is allready corrected in the pkcs15 parser
		//beidPin.encoding = PIN_ENC_GP;
		return PinCmd_2(operation, beidPin, csPin1, csPin2, ulRemaining, pKey);
	}*/

	bool CCard::PinCmd(tPinOperation operation, const tPin & Pin,
		const std::string & csPin1,
		const std::string & csPin2,
		unsigned long &ulRemaining,
		const tPrivKey * pKey)
	{
		// No standard for Logoff, so each card has to implement
		// it's own command here.
		// it doesn't use the PIN, so we call it directly
		// if (operation == PIN_OP_LOGOFF)
		//	return LogOff();

		bool bRet = false;

		std::string csReadPin1, csReadPin2;
		const std::string * pcsPin1 = &csPin1;
		const std::string * pcsPin2 = &csPin2;
		bool bAskPIN = csPin1.empty();
		bool bUsePinpad = bAskPIN ? m_poPinpad->UsePinpad(operation) : false;

	bad_pin:
		//If no Pin(s) provided and it's no Pinpad reader -> ask Pins
		if (bAskPIN && !bUsePinpad)
		{
			showPinDialog(operation, Pin, csReadPin1, csReadPin2, pKey);
			pcsPin1 = &csReadPin1;
			pcsPin2 = &csReadPin2;
		}

		CByteArray oPinBuf = MakePinBuf(Pin, *pcsPin1, bUsePinpad);

		if (operation != PIN_OP_VERIFY)
			oPinBuf.Append(MakePinBuf(Pin, *pcsPin2, bUsePinpad));

		CByteArray oAPDU = MakePinCmd(operation, Pin);	// add CLA, INS, P1, P2

		oAPDU.Append((unsigned char)oPinBuf.Size());	// add P3
		oAPDU.Append(oPinBuf);

		CByteArray oResp;

		// Don't remove these brackets!!
		{
			CAutoLock autolock(this);

			// There's a path in the EF(AODF) for the PINs, but it's
			// not necessary, so we can save a Select File command
			/* Select the path where the Pin is, if necessary
			if (!Pin.csPath.empty() && !bSelected && Pin.csPath != "3F00")
			{
				SelectFile(Pin.csPath);
				bSelected = true;
			}*/
			// Send the command
			if (csPin1.empty() && bUsePinpad)
			{
				oResp = m_poPinpad->PinCmd(operation, Pin, PinUsage2Pinpad(Pin, pKey), oAPDU, ulRemaining);
			}
			else
			{
				oResp = SendAPDU(oAPDU);
				oAPDU.SecureClearContents();
			}
		}

		unsigned long ulSW12 = getSW12(oResp);

		if (ulSW12 == 0x9000)
			bRet = true;
		else if (ulSW12 == 0x6983)
			ulRemaining = 0;
		else if (ulSW12 / 16 == 0x63C)
			ulRemaining = ulSW12 % 16;
		else
			throw CMWEXCEPTION(m_poPCSC->SW12ToErr(ulSW12));

#ifndef NO_DIALOGS
		// Bad PIN: show a dialog to ask the user to try again
		// PIN blocked: show a dialog to tell the user
		if (bAskPIN && !bRet)
		{
			DlgPinUsage usage = PinUsage2Dlg(Pin, pKey);
			DlgRet dlgret = DlgBadPin(usage, utilStringWiden(Pin.csLabel).c_str(), ulRemaining);
			if (0 != ulRemaining && DLG_RETRY == dlgret)
				goto bad_pin;
		}
#endif
		
		// If PIN command OK and no SSO, then state that we have now
		// verified this PIN, this info is needed in the Sign() method
		if (bRet && !m_poPCSC->m_bSSO)
		{
			bool bFound = false;

			for (size_t i = 0; i < m_verifiedPINs.size() && !bFound; i++)
			{
				bFound = (m_verifiedPINs[i] == Pin.ulIndex);
			}
			if (!bFound)
			{
				m_verifiedPINs.push_back(Pin.ulIndex);
			}
		}
		return bRet;
	}


	CByteArray CCard::VerifyAndSign(const tPrivKey & key, const tPin & Pin,
		unsigned long algo, const CByteArray & oData)
	{
		CByteArray retBytes;

		try
		{
			retBytes = SignInternal(key, algo, oData, &Pin);
		}
		catch (CMWException & e)
		{
			if ((unsigned)e.GetError() == EIDMW_ERR_PIN_BAD)
			{
				MWLOG(LEV_INFO, MOD_CAL, L"     Couldn't sign, asking PIN and trying again");
				// Bad PIN: show a dialog to ask the user to try again
				bool retry = AskPinRetry(PIN_OP_VERIFY, Pin, m_ulRemaining[Pin.ulIndex], &key);
				if (retry)
					retBytes = VerifyAndSign(key, Pin, algo, oData);
				else
					throw e;
			}
			else if ((unsigned)e.GetError() == EIDMW_ERR_PIN_BLOCKED)
			{
				MWLOG(LEV_WARN, MOD_CAL, L"     PIN blocked");
				// PIN blocked: show a dialog to tell the user
				AskPinRetry(PIN_OP_VERIFY, Pin, 0, &key);
				throw e;
			}
			else
				throw e;
		}
		return retBytes;
	}


	CByteArray CCard::Sign(const tPrivKey & key, const tPin & Pin,
		unsigned long algo, const CByteArray & oData)
	{
		// If SSO (Single Sign-On) is false and we didn't verify the
		// PIN yet, then we do this first without trying if it's
		// realy needed.
		if (!m_poPCSC->m_bSSO)
		{
			bool bFound = false;

			for (size_t i = 0; i < m_verifiedPINs.size() && !bFound; i++)
			{
				bFound = (m_verifiedPINs[i] == Pin.ulIndex);
			}
			if (!bFound)
			{
				MWLOG(LEV_INFO, MOD_CAL, L"     No SSO: ask PIN and sign (key: ID=0x%0x, algo=0x%0x, "
					L"%d bytes input)", key.ulID, algo, oData.Size());
				return SignInternal(key, algo, oData, &Pin);
			}
		}
		// First try to sign.
		// If this returns a "Security conditions not satisfied"
		// then first do a Pin verify and then try again
		MWLOG(LEV_INFO, MOD_CAL, L"     Trying to Sign (key: ID=0x%0x, algo=0x%0x, "
			L"%d bytes input)", key.ulID, algo, oData.Size());
		try
		{
			return SignInternal(key, algo, oData);
		}
		catch (CMWException & e)
		{
			if ((unsigned)e.GetError() == EIDMW_ERR_NOT_AUTHENTICATED)
			{
				MWLOG(LEV_INFO, MOD_CAL, L"     Couldn't sign, asking PIN and trying again");
				return VerifyAndSign(key, Pin, algo, oData);
			}
			else
				throw e;
		}
	}


	void CCard::SelectFile(const std::string & csPath)
	{
		CByteArray oResp;

		unsigned long ulPathLen = (unsigned long)csPath.size();

		if (ulPathLen % 4 != 0 || ulPathLen == 0)
			throw CMWEXCEPTION(EIDMW_ERR_BAD_PATH);

		ulPathLen /= 2;

		unsigned char ucP2 = 0x0C;

		CAutoLock autolock(this);

		if (m_selectAppletMode == ALW_SELECT_APPLET)
		{
			SelectApplet();
			oResp = SelectByPath(csPath);
		}
		else
		{
			// First try to select the file by ID, assuming we're in the correct DF
			CByteArray oPath(ulPathLen);

			oPath.Append(Hex2Byte(csPath, ulPathLen - 2));
			oPath.Append(Hex2Byte(csPath, ulPathLen - 1));

			// Select File
			oResp = SendAPDU(0x00, 0xA4, 0x02, ucP2, oPath);
			unsigned long ulSW12 = getSW12(oResp);

			if (ulSW12 == 0x6A82 || ulSW12 == 0x6A86)
			{
				if (ulPathLen == 2)
					throw CMWEXCEPTION(m_poPCSC->SW12ToErr(ulSW12));

				// The file wasn't found in this DF, so let's select by full path
				oResp = SelectByPath(csPath);
			}
			else
			{
				getSW12(oResp, 0x9000);
			}
		}
	}



	/**
	 * The Belpic card doesn't support select by path (only
	 * select by File ID or by AID) , so we make a loop with
	 * 'select by file' commands.
	 * E.g. if path = AAAABBBBCCCC the we do
	 *   Select(AAAA)
	 *   Select(BBBB)
	 *   Select(CCCC)
	 */
	CByteArray CCard::SelectByPath(const std::string & csPath)
	{
		CByteArray oResp;
		unsigned long ulOffset;

		//two ANSI chars per byte value that will by send to the card
		//ulPathLen is the path length in bytes, not in ANSI chars
		unsigned long ulPathLen = (unsigned long)csPath.size() / 2;

		//Do a loop of "Select File by file ID (DF or EF)" commands
		//ulOffset is the offset in bytes, not in ANSI chars
		for (ulOffset = 0; ulOffset < ulPathLen; ulOffset += 2)
		{
			CByteArray oCmd(6);

			//select file by relative path APDU
			const unsigned char Cmd[] = { 0x00, 0xA4, 0x02, 0x0C, 0x02 };
			oCmd.Append(Cmd, sizeof(Cmd));

			//add the correct path
			oCmd.Append(Hex2Byte(csPath, ulOffset));
			oCmd.Append(Hex2Byte(csPath, ulOffset + 1));

			//send the APDU
			oResp = SendAPDU(oCmd);
			unsigned long ulSW12 = getSW12(oResp);


			// in case of file not found, or wrong P1/P2 parameter
			//try to select the applet and try again
			if ((ulSW12 == 0x6A82 || ulSW12 == 0x6A86) && m_selectAppletMode == TRY_SELECT_APPLET)
			{
				// The file still wasn't found, so let's first try to select the applet
				if (SelectApplet())
				{
					m_selectAppletMode = ALW_SELECT_APPLET;
					oResp = SendAPDU(oCmd);
				}
			}
			getSW12(oResp, 0x9000);
		}

		// Normally we should put here the FCI, but since Belpic cards
		// don't return it, we just return the path that can be used
		// later on to return the harcoded FCI for that file.
		return CByteArray((unsigned char *)csPath.c_str(), (unsigned long)csPath.size());
	}

	CByteArray CCard::ReadBinary(unsigned long ulOffset, unsigned long ulLen)
	{
		// Read Binary
		return SendAPDU(0x00, 0xB0, (unsigned char)(ulOffset / 256), (unsigned char)(ulOffset % 256), (unsigned char)(ulLen));
	}

	CByteArray CCard::UpdateBinary(unsigned long ulOffset, const CByteArray & oData)
	{
		// Update Binary
		return SendAPDU(0x00, 0xD6, (unsigned char)(ulOffset / 256), (unsigned char)(ulOffset % 256), oData);
	}

	DlgPinOperation CCard::PinOperation2Dlg(tPinOperation operation)
	{
		switch (operation)
		{
		case PIN_OP_CHANGE:
			return DLG_PIN_OP_CHANGE;
		default:
			return DLG_PIN_OP_VERIFY;
		}
	}

	CByteArray CCard::MakePinCmd(tPinOperation operation, const tPin & Pin)
	{
		CByteArray oCmd(5 + 32);

		oCmd.Append(0x00);//CLA

		switch (operation)
		{
		case PIN_OP_VERIFY:
			oCmd.Append(0x20);
			break;
		case PIN_OP_CHANGE:
			oCmd.Append(0x24);
			break;
		default:
			throw CMWEXCEPTION(EIDMW_ERR_PIN_OPERATION);
		}
		oCmd.Append(0x00);	// P1
		oCmd.Append((unsigned char)Pin.ulAuthID);	// P2

		return oCmd;
	}

	CByteArray CCard::MakePinBuf(const tPin & Pin, const std::string & csPin, bool bEmptyPin)
	{
		CByteArray oBuf(16);
		unsigned long i;

		unsigned long ulPinLen = bEmptyPin ? 0 : (unsigned long)csPin.size();

		if (!bEmptyPin)
		{
			// Test if it's a valid PIN value
			if (Pin.ulMinLen > 0 && ulPinLen < Pin.ulMinLen)
			{
				MWLOG(LEV_WARN, MOD_CAL, L"PIN length is %d, should be at least %d", ulPinLen, Pin.ulMinLen);
				throw CMWEXCEPTION(EIDMW_ERR_PIN_FORMAT);
			}
			if (Pin.ulMaxLen > 0 && ulPinLen > Pin.ulMaxLen)
			{
				MWLOG(LEV_WARN, MOD_CAL, L"PIN length is %d, should be at most %d", ulPinLen, Pin.ulMaxLen);
				throw CMWEXCEPTION(EIDMW_ERR_PIN_FORMAT);
			}
		}

		for (i = 0; i < ulPinLen; i++)
		{
			if (!IsDigit(csPin[i]))
			{
				MWLOG(LEV_WARN, MOD_CAL, L"The PIN contains non-digit values");
				throw CMWEXCEPTION(EIDMW_ERR_PIN_FORMAT);
			}
		}

		//all our supported cards only use PIN_ENC_GP
//		switch (Pin.encoding)
//		{
//		case PIN_ENC_ASCII:
//			for (i = 0; i < ulPinLen; i++)
//				oBuf.Append((unsigned char)csPin[i]);
//			for (; i < Pin.ulStoredLen; i++)
//				oBuf.Append(Pin.ucPadChar);
//			break;
//		case PIN_ENC_GP:
			oBuf.Append((unsigned char)(0x20 + ulPinLen));
			// Falls through
//		case PIN_ENC_BCD:
			i = 0;
			while (i < ulPinLen)
			{
				unsigned char uc = (unsigned char)(16 * (csPin[i] - '0'));
				i++;
				if (i < ulPinLen)
				{
					uc += (unsigned char)(csPin[i] - '0');
				}
				else
				{
					uc += (unsigned char)(0xFF % 16);
				}
				i++;
				oBuf.Append(uc);
			}
			while (oBuf.Size() < Pin.ulStoredLen)
			{
				oBuf.Append((unsigned char)0xFF);
			}
//			break;
//		default:
//			throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
//		}

		return oBuf;
	}

	////////////////////////////////////////////////////////////////:

	CAutoLock::CAutoLock(CCard * poCard) :m_poCard(poCard), m_poPCSC(NULL), m_hCard(0)
	{
		m_poCard->Lock();
	}

	CAutoLock::CAutoLock(CPCSC * poPCSC, SCARDHANDLE hCard) :m_poCard(NULL), m_poPCSC(poPCSC), m_hCard(hCard)
	{
		poPCSC->BeginTransaction(hCard);
	}

	CAutoLock::~CAutoLock()
	{
		try {
			if (m_poCard)
				m_poCard->Unlock();
			else
				m_poPCSC->EndTransaction(m_hCard);
		} catch (CMWException&) {
		}
	}

}
