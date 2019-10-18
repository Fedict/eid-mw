
/* ****************************************************************************

* eID Middleware Project.
* Copyright (C) 2008-2017 FedICT.
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
#include "pinpad.h"
#include "pcsc.h"
#include "pinpad2.h"
#include "common/log.h"
#include "common/util.h"
#include "common/configuration.h"
#include "assert.h"

// Make sure PP_DUMP_CMDS is not defined in a release!
#define PP_DUMP_CMDS
#undef PP_DUMP_CMDS

namespace eIDMW
{

	CPinpad::CPinpad(void):m_poPCSC(NULL), m_hCard(0), m_usReaderFirmVers(0), m_bNewCard(true),
		m_bUsePinpadLib(false), m_ulLangCode(0), m_bCanVerifyUnlock(false), m_bCanChangeUnlock(false),
		m_ioctlVerifyStart(0), m_ioctlVerifyFinish(0), m_ioctlVerifyDirect(0), m_ioctlChangeStart(0),
		m_ioctlChangeFinish(0), m_ioctlChangeDirect(0), m_bCanUsePPDU(false)
	{
	}

	void CPinpad::Init(CPCSC* poPCSC, SCARDHANDLE hCard, const std::string & csReader, const std::string & csPinpadPrefix)
	{
		m_poPCSC = poPCSC;
		m_hCard = hCard;
		m_csReader = csReader;

		// We only unload the pinpad lib that is currently
		// loaded if another type of card is inserted then
		// the card before the current one.
		if (csPinpadPrefix != m_csPinpadPrefix)
			UnloadPinpadLib();

		m_csPinpadPrefix = csPinpadPrefix;
	}

	void CPinpad::Init(CPCSC * poPCSC, SCARDHANDLE hCard, const std::string & csReader,
			   const std::string & csPinpadPrefix, CByteArray usReaderFirmVers)
	{
		this->Init(poPCSC, hCard, csReader, csPinpadPrefix);

		m_usReaderFirmVers = usReaderFirmVers.GetByte(3) * 256 + usReaderFirmVers.GetByte(2);
	}

	bool CPinpad::UsePinpad(tPinOperation operation)
	{
		if (m_bNewCard)
		{
			m_bUsePinpadLib = m_oPinpadLib.Load((unsigned long) m_poPCSC->m_hContext, m_hCard, m_csReader,
				     m_csPinpadPrefix, GetLanguage());

			// The GemPC pinpad reader does a "Verify PIN" with empty buffer in an attempt
			// to get and display the remainings attempts. But the BE eID card takes this
			// empty buffer to be a bad PIN, so you quickly end up with a blocked card.
			// Therefore, we don't allow this reader to be used as a pinpad reader..
			if (!m_bUsePinpadLib && m_csReader.find("Gemplus GemPC Pinpad") == 0)
			{
				return false;
			}
			GetFeatureList();
		}

		switch (operation)
		{
			case PIN_OP_VERIFY:
				return m_bCanVerifyUnlock;
			case PIN_OP_CHANGE:
				return m_bCanChangeUnlock;
			default:
				return false;
		}
	}

	// See par 4.1.11.3 bmFormatString description
	unsigned char CPinpad::ToFormatString(const tPin & pin)
	{
		//all our supported cards only use PIN_ENC_GP

//		switch (pin.encoding)
//		{
//			case PIN_ENC_ASCII:
//				return 0x00 | 0x00 | 0x00 | 0x02;
//			case PIN_ENC_BCD:
//				return 0x00 | 0x00 | 0x00 | 0x01;
//			case PIN_ENC_GP:
				// READER FIX:
				// The SPR532 reader wants this value to be as for BCD
				const char *csReader = m_csReader.c_str();

				if ((m_usReaderFirmVers != 0x0000) && (m_usReaderFirmVers < 0x0506) && (strstr(csReader, "SPRx32 USB") != NULL))
				{
					return 0x00 | 0x00 | 0x00 | 0x01;
				}
				return 0x80 | 0x08 | 0x00 | 0x01;
//		}
//		return 0;
	}

	// See par 4.1.11.4 bmPINBlockString description
	unsigned char CPinpad::ToPinBlockString(const tPin & pin)
	{
		//all our supported cards only use PIN_ENC_GP
//		switch (pin.encoding)
//		{
//			case PIN_ENC_ASCII:
//				return (unsigned char) pin.ulStoredLen;
//			case PIN_ENC_BCD:
//				return (unsigned char) pin.ulStoredLen;
//			case PIN_ENC_GP:
				return 0x40 | (unsigned char) (pin.ulStoredLen - 1);
//		}
//		return (unsigned char) pin.ulStoredLen;
	}

	// See par 4.1.11.5 bmPINLengthFormat
	unsigned char CPinpad::ToPinLengthFormat(const tPin & pin)
	{
		//all our supported cards only use PIN_ENC_GP
//		switch (pin.encoding)
//		{
//			case PIN_ENC_ASCII:
//				return 0x00 | 0x00;
//			case PIN_ENC_BCD:
//				return 0x00 | 0x00;
//			case PIN_ENC_GP:
				return 0x00 | 0x04;
//		}
//		return 0x00;
	}

	unsigned char CPinpad::GetMaxPinLen(const tPin & pin)
	{
		unsigned char ucRes = (unsigned char)(pin.ulMaxLen);

		// READER FIX:
		// Limitation of the GemPC reader: at most 8 PIN digits
		const char *csReader = m_csReader.c_str();

		if (strstr(csReader, "Gemplus GemPC Pinpad") == csReader)
		{
			ucRes = (ucRes > 8) ? 8 : ucRes;
		}
		return ucRes;
	}

	CByteArray CPinpad::PinCmd(tPinOperation operation, const tPin & pin, unsigned char ucPinType,
				   const CByteArray & oAPDU, unsigned long &ulRemaining)
	{
		if (!UsePinpad(operation))
			throw CMWEXCEPTION(EIDMW_ERR_PIN_OPERATION);

		CByteArray oResp;

		if (operation == PIN_OP_VERIFY)
		{
			oResp = PinCmd1(operation, pin, ucPinType, oAPDU, ulRemaining);
		}
		else
		{
			oResp = PinCmd2(operation, pin, ucPinType, oAPDU, ulRemaining);
		}

		if (oResp.Size() != 2)
		{
			MWLOG(LEV_ERROR, MOD_CAL, L"pinpad reader returned %ls\n", oResp.ToWString().c_str());
			return EIDMW_ERR_UNKNOWN;	// should never happen
		}

		const unsigned char *pucSW12 = oResp.GetBytes();

		if (pucSW12[0] == 0x64 && pucSW12[1] == 0x00)
			throw CMWEXCEPTION(EIDMW_ERR_TIMEOUT);

		if (pucSW12[0] == 0x64 && pucSW12[1] == 0x01)
			throw CMWEXCEPTION(EIDMW_ERR_PIN_CANCEL);

		if (pucSW12[0] == 0x64 && pucSW12[1] == 0x02)
			throw CMWEXCEPTION(EIDMW_NEW_PINS_DIFFER);

		if (pucSW12[0] == 0x64 && pucSW12[1] == 0x03)
			throw CMWEXCEPTION(EIDMW_WRONG_PIN_FORMAT);

		if (pucSW12[0] == 0x6B && pucSW12[1] == 0x80)
			throw CMWEXCEPTION(EIDMW_PINPAD_ERR);

		return oResp;
	}

	/** For operations involving 1 PIN */
	CByteArray CPinpad::PinCmd1(tPinOperation operation, const tPin & pin, unsigned char ucPinType,
				    const CByteArray & oAPDU, unsigned long &ulRemaining)
	{
		EIDMW_PP_VERIFY_CCID xVerifyCmd;
		unsigned long ulVerifyCmdLen;

		memset(&xVerifyCmd, 0, sizeof(xVerifyCmd));
		xVerifyCmd.bTimerOut = 30;
		xVerifyCmd.bTimerOut2 = 30;
		xVerifyCmd.bmFormatString = ToFormatString(pin);
		xVerifyCmd.bmPINBlockString = ToPinBlockString(pin);
		xVerifyCmd.bmPINLengthFormat = ToPinLengthFormat(pin);
		xVerifyCmd.wPINMaxExtraDigit[0] = GetMaxPinLen(pin);
		xVerifyCmd.wPINMaxExtraDigit[1] = (unsigned char) pin.ulMinLen;
		xVerifyCmd.bEntryValidationCondition = 0x02;
		xVerifyCmd.bNumberMessage = 0x01;
		//ToUchar2(m_ulLangCode, xVerifyCmd.wLangId);
		xVerifyCmd.wLangId[0] = (unsigned char) ((m_ulLangCode & 0xff00) / 256);
		xVerifyCmd.wLangId[1] = (unsigned char) (m_ulLangCode & 0xff);
		xVerifyCmd.bMsgIndex = 0;
		ToUchar4(oAPDU.Size(), xVerifyCmd.ulDataLength);
		memcpy(xVerifyCmd.abData, oAPDU.GetBytes(), oAPDU.Size());
		ulVerifyCmdLen = sizeof(xVerifyCmd) - PP_APDU_MAX_LEN + oAPDU.Size();

		CByteArray oCmd((unsigned char *) &xVerifyCmd, ulVerifyCmdLen);

		if (m_bCanUsePPDU)
		{
			if (m_ioctlVerifyDirect)
			{
				return PinpadPPDU(FEATURE_VERIFY_PIN_DIRECT, oCmd, operation, ucPinType, pin.csLabel, true);
			} 
			else	//m_bCanUsePPDU can only be true if either m_ioctlVerifyDirect or m_ioctlVerifyStart are set
			{
				PinpadPPDU(FEATURE_VERIFY_PIN_START, oCmd, operation, ucPinType, pin.csLabel, false);
				return PinpadPPDU(FEATURE_VERIFY_PIN_FINISH, CByteArray(), operation, ucPinType, "", true);
			}
		} else
		{
			if (m_ioctlVerifyDirect)
			{
				return PinpadControl(m_ioctlVerifyDirect, oCmd, operation, ucPinType, pin.csLabel, true);
			} 
			else
			{
				PinpadControl(m_ioctlVerifyStart, oCmd, operation, ucPinType, pin.csLabel, false);
				return PinpadControl(m_ioctlVerifyFinish, CByteArray(), operation, ucPinType, "", true);
			}
		}
	}

	/** For operations involving 2 PINs */
	CByteArray CPinpad::PinCmd2(tPinOperation operation, const tPin & pin, unsigned char ucPinType,
				    const CByteArray & oAPDU, unsigned long &ulRemaining)
	{
		EIDMW_PP_CHANGE_CCID xChangeCmd;
		unsigned long ulChangeCmdLen;

		memset(&xChangeCmd, 0, sizeof(xChangeCmd));
		xChangeCmd.bTimerOut = 30;
		xChangeCmd.bTimerOut2 = 30;
		xChangeCmd.bmFormatString = ToFormatString(pin);
		xChangeCmd.bmPINBlockString = ToPinBlockString(pin);
		xChangeCmd.bmPINLengthFormat = ToPinLengthFormat(pin);
		xChangeCmd.bInsertionOffsetOld = 0x00;
		xChangeCmd.bInsertionOffsetNew = (unsigned char) pin.ulStoredLen;
		xChangeCmd.wPINMaxExtraDigit[0] = GetMaxPinLen(pin);
		xChangeCmd.wPINMaxExtraDigit[1] = (unsigned char) pin.ulMinLen;
		xChangeCmd.bConfirmPIN = 0x03;
		xChangeCmd.bEntryValidationCondition = 0x02;
		xChangeCmd.bNumberMessage = 0x03;
		//ToUchar2(m_ulLangCode, xChangeCmd.wLangId);
		xChangeCmd.wLangId[0] = (unsigned char) ((m_ulLangCode & 0xff00) / 256);
		xChangeCmd.wLangId[1] = (unsigned char) (m_ulLangCode & 0xff);
		xChangeCmd.bMsgIndex1 = 0x00;
		xChangeCmd.bMsgIndex2 = 0x01;
		xChangeCmd.bMsgIndex3 = 0x02;
		ToUchar4(oAPDU.Size(), xChangeCmd.ulDataLength);
		memcpy(xChangeCmd.abData, oAPDU.GetBytes(), oAPDU.Size());
		ulChangeCmdLen = sizeof(xChangeCmd) - PP_APDU_MAX_LEN + oAPDU.Size();

		CByteArray oCmd((unsigned char *) &xChangeCmd, ulChangeCmdLen);

		if (m_bCanUsePPDU)
		{
			if (m_ioctlChangeDirect)
			{
				return PinpadPPDU(FEATURE_MODIFY_PIN_DIRECT, oCmd, operation, ucPinType, pin.csLabel, true);
			}
			else	//m_bCanUsePPDU can only be true if either m_ioctlChangeDirect or m_ioctlChangeStart are set
			{
				PinpadPPDU(FEATURE_MODIFY_PIN_START, oCmd, operation, ucPinType, pin.csLabel, false);
				return PinpadPPDU(FEATURE_MODIFY_PIN_FINISH, CByteArray(), operation, ucPinType, "", true);
			}
		}
		else
		{
			if (m_ioctlChangeDirect)
			{
				return PinpadControl(m_ioctlChangeDirect, oCmd,
					operation, ucPinType,
					pin.csLabel, true);
			}
			else
			{
				PinpadControl(m_ioctlChangeStart, oCmd, operation,
					ucPinType, pin.csLabel, false);
				return PinpadControl(m_ioctlChangeFinish,
					CByteArray(), operation,
					ucPinType, "", true);
			}
		}

	}

	void CPinpad::UnloadPinpadLib()
	{
		m_bNewCard = true;
		m_bCanVerifyUnlock = false;
		m_bCanChangeUnlock = false;
		m_bUsePinpadLib = false;
		m_bCanUsePPDU = false;
		m_oPinpadLib.Unload();
	}

	CByteArray CPinpad::PinpadControl(unsigned long ulControl, const CByteArray & oCmd, tPinOperation operation,
					  unsigned char ucPintype, const std::string & csPinLabel, bool bShowDlg)
	{
		unsigned char pinpadOperation = PinOperation2Lib(operation);

#ifdef PP_DUMP_CMDS
		printf("PP ctrl (%svia pinpad lib): 0x%0x\n",
		       (m_bUsePinpadLib ? "" : "not "), ulControl);
		printf("PP IN: %s\n",
		       oCmd.ToString(true, false, 0, 0xFFFFFFFF).c_str());
#endif

		BEID_DIALOGHANDLE dlgHandle;
		bool bCloseDlg = bShowDlg;

		if (bShowDlg)
		{
			bCloseDlg = m_oPinpadLib.ShowDlg(pinpadOperation, ucPintype, csPinLabel, m_csReader, &dlgHandle);
		}

		CByteArray oResp;

		try
		{
			if (!m_bUsePinpadLib)
			{
				oResp = m_poPCSC->Control(m_hCard, ulControl, oCmd);
			} else
			{
				m_ulLangCode = 0;//forget the previously know language, so the register is checked once more
				GetLanguage();
				MWLOG(LEV_INFO, MOD_CAL,L"PinpadControl using pinpadlib with lang=0x%u\n",m_ulLangCode);
				oResp = m_oPinpadLib.PinCmd(m_hCard, ulControl, oCmd,ucPintype, pinpadOperation,m_ulLangCode);
			}
		}
		catch( ...)
		{
			if (bCloseDlg)
				m_oPinpadLib.CloseDlg(dlgHandle);
			throw;
		}
		if (bCloseDlg)
			m_oPinpadLib.CloseDlg(dlgHandle);

#ifdef PP_DUMP_CMDS
		printf("PP OUT: %s\n",
		       oResp.ToString(true, false, 0, 0xFFFFFFFF).c_str());
#endif

		return oResp;
	}

#define CHECK_FEATURE(feature, featureID, iotcl) \
	if (feature[0] == featureID) \
	iotcl = 256 * (256 * ((256 * feature[2]) + feature[3]) + feature[4]) + feature[5];

	void CPinpad::GetFeatureList()
	{
		m_bCanVerifyUnlock = false;
		m_bCanChangeUnlock = false;
		m_bCanUsePPDU = false;
		m_ioctlVerifyStart = m_ioctlVerifyFinish = m_ioctlVerifyDirect = 0;
		m_ioctlChangeStart = m_ioctlChangeFinish = m_ioctlChangeDirect = 0;

		try
		{
			CByteArray oFeatures = PinpadControl(CCID_IOCTL_GET_FEATURE_REQUEST, CByteArray(), PIN_OP_VERIFY, 0, "", false);

			// Example of a feature list: 06 04 00 31 20 30 07 04 00 31 20 34
			// Which means:
			//  - IOCTL for CCID_VERIFY_DIRECT = 0x00312030
			//  - IOCTL for CCID_CHANGE_DIRECT = 0x00312034


			unsigned long ulFeatureLen = oFeatures.Size();

			MWLOG(LEV_DEBUG, MOD_CAL, L"CPinpad::GetFeatureList() oFeatures.size = %lu\n", ulFeatureLen);

			//if(ulFeatureLen > 0)
			//{
			//      const unsigned char *pucFeatures = oFeatures.GetBytes();
			//      for (unsigned long i = 0; i < ulFeatureLen; i+=6)
			//      {
			//              MWLOG(LEV_DEBUG, MOD_CAL, L"feature %d is 0x%02u 0x%02u 0x%02u 0x%02u 0x%02u 0x%02u",i/6,pucFeatures[i],pucFeatures[i+1],pucFeatures[i+2],pucFeatures[i+3],pucFeatures[i+4],pucFeatures[i+5]);
			//      }
			//}

			if (((ulFeatureLen % 6) == 0) && (ulFeatureLen > 0))
			{
				const unsigned char *pucFeatures = oFeatures.GetBytes();
				ulFeatureLen /= 6;
				MWLOG(LEV_DEBUG, MOD_CAL, L"checking features");
				for (unsigned long i = 0; i < ulFeatureLen; i++)
				{
					CHECK_FEATURE(pucFeatures, CCID_VERIFY_START, m_ioctlVerifyStart)
					CHECK_FEATURE(pucFeatures, CCID_VERIFY_FINISH, m_ioctlVerifyFinish)
					CHECK_FEATURE(pucFeatures, CCID_VERIFY_DIRECT, m_ioctlVerifyDirect)
					CHECK_FEATURE(pucFeatures, CCID_CHANGE_START, m_ioctlChangeStart)
					CHECK_FEATURE(pucFeatures, CCID_CHANGE_FINISH, m_ioctlChangeFinish)
					CHECK_FEATURE(pucFeatures, CCID_CHANGE_DIRECT, m_ioctlChangeDirect)
					pucFeatures += 6;
				}
			} else
			{
				//MWLOG(LEV_DEBUG, MOD_CAL, L"(ulFeatureLen mod 6) is 0, trying GetPPDUFeatureList\n");
				GetPPDUFeatureList();
			}
		}
		catch(const CMWException & e)
		{
			// very likely CCID_IOCTL_GET_FEATURE_REQUEST isn't supported
			// by this reader -> try via PPDU
			//MWLOG(LEV_DEBUG, MOD_CAL, L"CPinpad::GetFeatureList() threw exception, trying GetPPDUFeatureList\n");
			GetPPDUFeatureList();

			e.GetError();
		}
		m_bCanVerifyUnlock = (m_ioctlVerifyStart && m_ioctlVerifyFinish) || m_ioctlVerifyDirect;
		m_bCanChangeUnlock = (m_ioctlChangeStart && m_ioctlChangeFinish) || m_ioctlChangeDirect;

		if (m_bCanVerifyUnlock || m_bCanChangeUnlock)
		{
			m_ulLangCode = GetLanguage();
		}

		m_bNewCard = false;
	}

	unsigned char CPinpad::PinOperation2Lib(tPinOperation operation)
	{
		switch (operation)
		{
			case PIN_OP_VERIFY:
				return EIDMW_PP_OP_VERIFY;
			case PIN_OP_CHANGE:
				return EIDMW_PP_OP_CHANGE;
				// Add others when needed
			default:
				throw CMWEXCEPTION(EIDMW_ERR_CHECK);
		}
	}

	unsigned long CPinpad::GetLanguage()
	{
		if (m_ulLangCode == 0)
		{
			m_ulLangCode = PP_LANG_EN;
			std::wstring csLang = CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_LANGUAGE);
			if (csLang == L"nl")
				m_ulLangCode = PP_LANG_NL;
			else if (csLang == L"fr")
				m_ulLangCode = PP_LANG_FR;
			else if (csLang == L"de")
				m_ulLangCode = PP_LANG_DE;
		}
		return m_ulLangCode;
	}

	void CPinpad::GetPPDUFeatureList()
	{
		CByteArray oResp;
		long lRetVal = 0;
		unsigned int counter = 0;

		unsigned char cmd[] = { 0xFF, 0xC2, 0x01, 0x00, 0x00 };
		CByteArray oCmd(cmd, sizeof(cmd));

		//add friendlynames of readers that support PPDU over transmit here

		if ((m_csReader.find("VASCO DIGIPASS 870") == 0) ||
		    (m_csReader.find("VASCO DIGIPASS 875") == 0) ||
		    (m_csReader.find("VASCO DIGIPASS 920") == 0) ||
			(m_csReader.find("VASCO DIGIPASS 840") == 0) ||
			(m_csReader.find("VASCO DIGIPASS 876") == 0) ||    
		    (m_csReader.find("Gemalto ING Shield Pro") == 0) ||
			(m_csReader.find("ETSWW eKrypto PINPhab") == 0) ||
			(m_csReader.find("ETSWW eKrypto PINPad") == 0) ||
			(m_csReader.find("DIOSS pinpad") == 0))
		{
			oResp = m_poPCSC->Transmit(m_hCard, oCmd, &lRetVal);
			for (; counter < (oResp.Size() - 2); counter++)
			{
				switch (oResp.GetByte(counter))
				{
					case FEATURE_MODIFY_PIN_START:
						m_ioctlChangeStart = true;
						m_bCanUsePPDU = true;
						break;
	
					case FEATURE_MODIFY_PIN_FINISH:
						m_ioctlChangeFinish = true;
						break;

					case FEATURE_VERIFY_PIN_START:
						m_ioctlVerifyStart = true;
						m_bCanUsePPDU = true;
						break;

					case FEATURE_VERIFY_PIN_FINISH:
						m_ioctlVerifyFinish = true;
						break;

					case FEATURE_VERIFY_PIN_DIRECT:
						m_ioctlVerifyDirect = true;
						m_bCanUsePPDU = true;
						break;

					case FEATURE_MODIFY_PIN_DIRECT:
						m_ioctlChangeDirect = true;
						m_bCanUsePPDU = true;
						break;

					default:
						break;
				}
			}
		}
	}

	CByteArray CPinpad::PinpadPPDU(BYTE cbControl, const CByteArray & oCmd, tPinOperation operation,
				       unsigned char ucPintype, const std::string & csPinLabel, bool bShowDlg)
	{
		unsigned char pinpadOperation = PinOperation2Lib(operation);

		assert(oCmd.Size() <= 255);
		BYTE oCmdLen = (BYTE) oCmd.Size();
		long lRetVal = 0;

#ifdef PP_DUMP_CMDS
		printf("PP PPDU (%svia pinpad lib): 0x%0x\n",
		       (m_bUsePinpadLib ? "" : "not "), cbControl);
		printf("PP IN: %s\n",
		       oCmd.ToString(true, false, 0, 0xFFFFFFFF).c_str());
#endif

		BEID_DIALOGHANDLE dlgHandle;
		bool bCloseDlg = bShowDlg;

		CByteArray oResp;
		unsigned char ucTransmit[] = { 0xFF, 0xC2, 0x01 };
		CByteArray oTransmit(ucTransmit, sizeof(ucTransmit));

		oTransmit.Append(cbControl);
		oTransmit.Append(oCmdLen);
		if (oCmdLen > 0)
		{
			oTransmit.Append(oCmd);
		}

		if (bShowDlg)
		{
			bCloseDlg = m_oPinpadLib.ShowDlg(pinpadOperation, ucPintype, csPinLabel, m_csReader, &dlgHandle);
		}
		oResp = m_poPCSC->Transmit(m_hCard, oTransmit, &lRetVal);

		if (bCloseDlg)
		{
			m_oPinpadLib.CloseDlg(dlgHandle);
		}
#ifdef PP_DUMP_CMDS
		printf("PP OUT: %s\n",
		       oResp.ToString(true, false, 0, 0xFFFFFFFF).c_str());
#endif

		return oResp;
	}
}

/**************************** logs *******************

1. SPR532 (driver 2.14_2.11, firmware 5.05)

Verify:
1E 1E 01 47 04 08 04 02 01 CD CD 00 00 00 00 0D
00 00 00 00 20 00 01 08 20 FF FF FF FF FF FF FF

Change:
1E 1E 01 47 04 00 08 0C 04 03 02 03 CD CD 00 00
00 00 00 00 15 00 00 00 00 24 00 01 10 20 FF FF
FF FF FF FF FF 20 FF FF FF FF FF FF FF

Note: bmFormatString must be 01 (instead of 89) !!!


2. GemPC pinpad

Verify:
1E 1E 89 47 04 08 04 02 01 CD CD 00 00 00 00 0D
00 00 00 00 20 00 01 08 20 FF FF FF FF FF FF FF

Change:
1E 1E 89 47 04 00 08 08 04 03 02 03 CD CD 00 00
00 00 00 00 15 00 00 00 00 24 00 01 10 20 FF FF
FF FF FF FF FF 20 FF FF FF FF FF FF FF
00
Note: wPINMaxExtraDigit[0] must be at most 8 (instead of 12) !!

******************************************************/
