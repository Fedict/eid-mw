/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2013 FedICT.
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
#include "beidcard.h"
#include "../common/src/log.h"

#ifdef __APPLE__
#include "unknowncard.h"
#endif

using namespace eIDMW;

static const unsigned char APPLET_AID[] = {0xA0,0x00,0x00,0x00,0x30,0x29,0x05,0x70,0x00,0xAD,0x13,0x10,0x01,0x01,0xFF};
static const unsigned char BELPIC_AID[] = {0xA0,0x00,0x00,0x01,0x77,0x50,0x4B,0x43,0x53,0x2D,0x31,0x35};
//static const unsigned char ID_AID[] =     {0xA0,0x00,0x00,0x01,0x77,0x49,0x64,0x46,0x69,0x6C,0x65,0x73};

static const tFileInfo DEFAULT_FILE_INFO = {-1, -1, -1};
static const tFileInfo PREFS_FILE_INFO_V1 = {-1, -1, 1};
static const tFileInfo PREFS_FILE_INFO_V2 = {-1, -1, 0x85};

// If we want to 'hardcode' this plugin internally in the CAL, this function
// can't be present because it's the same for all plugins
#ifndef CARDPLUGIN_IN_CAL
CCard *GetCardInstance(unsigned long ulVersion, const char *csReader,
	SCARDHANDLE hCard, CContext *poContext, CPinpad *poPinpad)
{
	return BeidCardGetInstance(ulVersion, csReader, hCard, poContext, poPinpad);
}
#endif

static bool BeidCardSelectApplet(CContext *poContext, SCARDHANDLE hCard)
{
	long lRetVal = 0; 
	unsigned char tucSelectApp[] = {0x00, 0xA4, 0x04, 0x00};
	CByteArray oCmd(40);
	oCmd.Append(tucSelectApp, sizeof(tucSelectApp));
	oCmd.Append((unsigned char) sizeof(APPLET_AID));
	oCmd.Append(APPLET_AID, sizeof(APPLET_AID));

	CByteArray oResp = poContext->m_oPCSC.Transmit(hCard, oCmd, &lRetVal);

	return (oResp.Size() == 2 && (oResp.GetByte(0) == 0x61 || oResp.GetByte(0) == 0x90));
}

CCard *BeidCardGetInstance(unsigned long ulVersion, const char *csReader,
	SCARDHANDLE hCard, CContext *poContext, CPinpad *poPinpad)
{
	CCard *poCard = NULL;

	if ((ulVersion % 100) == (PLUGIN_VERSION % 100))
	{
		try {
			bool bNeedToSelectApplet = false;
			CByteArray oData;
			CByteArray oCmd(40);
			unsigned char tucSelectApp[] = {0x00, 0xA4, 0x04, 0x0C};
			oCmd.Append(tucSelectApp, sizeof(tucSelectApp));
			oCmd.Append((unsigned char) sizeof(BELPIC_AID));
			oCmd.Append(BELPIC_AID, sizeof(BELPIC_AID));
			long lRetVal;
			// Don't remove these brackets, CAutoLock dtor must be called!
			//{
			//don't use autolock when card might be reset
				//CAutoLock oAutLock(&poContext->m_oPCSC, hCard);
				unsigned long ulLockCount = 1;
				poContext->m_oPCSC.BeginTransaction(hCard);
				oData = poContext->m_oPCSC.Transmit(hCard, oCmd, &lRetVal);
				if (lRetVal == SCARD_E_COMM_DATA_LOST || lRetVal == SCARD_E_NOT_TRANSACTED)
				{
					poContext->m_oPCSC.Recover(hCard, &ulLockCount);
					
					bNeedToSelectApplet = BeidCardSelectApplet(poContext, hCard);
					if (bNeedToSelectApplet)// try again to select the belpic app
						oData = poContext->m_oPCSC.Transmit(hCard, oCmd,&lRetVal);
				}
				if (oData.Size() == 2 && oData.GetByte(0) == 0x6A &&
					(oData.GetByte(1) == 0x82 || oData.GetByte(1) == 0x86))
				{
					// Perhaps the applet is no longer selected; so try to select it
					// first; and if successfull then try to select the Belpic AID again
					bNeedToSelectApplet = BeidCardSelectApplet(poContext, hCard);
					if (bNeedToSelectApplet)
						oData = poContext->m_oPCSC.Transmit(hCard, oCmd,&lRetVal);
				}

				bool bIsBeidCard = oData.Size() == 2 && oData.GetByte(0) == 0x90 && oData.GetByte(1) == 0x00;

				if (bIsBeidCard)
					poCard = new CBeidCard(hCard, poContext, poPinpad, oData,
					bNeedToSelectApplet ? ALW_SELECT_APPLET : TRY_SELECT_APPLET);
#ifdef __APPLE__
				else {
					// On Mac, if an unknown asynchronous card is inserted,
					// we don't return NULL but a CUnknownCard instance.
					// Reason: if we return NULL then the SISCardPlugin who
					// will be consulted next in card of a ACR38U reader
					// causes the reader/driver to get in a strange state
					// (if no SIS card is present) and if then a CUnknownCard
					// is instantiated, it will throw an exception if e.g.
					// SCardStatus() is called.
					// Remark: this trick won't work if synchronous card
					// (other then the SIS card is inserted).
					if(ulLockCount)
					{
						poContext->m_oPCSC.EndTransaction(hCard);
					}
					return new CUnknownCard(hCard, poContext, poPinpad, CByteArray());
				}
#endif
			//}
			if(ulLockCount)
			{
				poContext->m_oPCSC.EndTransaction(hCard);
			}
		}
		catch(...)
		{
			//printf("Exception in cardPluginBeid.CardGetInstance()\n");
		}
	}

	return poCard;
}

CBeidCard::CBeidCard(SCARDHANDLE hCard, CContext *poContext,
	CPinpad *poPinpad, const CByteArray & oData, tSelectAppletMode selectAppletMode) :
CPkiCard(hCard, poContext, poPinpad)
{
		m_cardType = CARD_BEID;
    try {
		m_ucCLA = 0x80;
        m_oCardData = SendAPDU(0xE4, 0x00, 0x00, 0x1C);
        m_ucCLA = 0x00;

		if (m_oCardData.Size() < 23)
		{
			throw CMWEXCEPTION(EIDMW_ERR_APPLET_VERSION_NOT_FOUND);
		}

		m_oCardData.Chop(2); // remove SW12 = '90 00'

		m_oSerialNr = CByteArray(m_oCardData.GetBytes(), 16);

		m_ucAppletVersion = m_oCardData.GetByte(21);

		m_ul6CDelay = 0;
		if (m_oCardData.GetByte(22) == 0x00 && m_oCardData.GetByte(23) == 0x01)
			m_ul6CDelay = 50;

		m_selectAppletMode = selectAppletMode;
	}
    catch(CMWException e)
    {
		MWLOG(LEV_CRIT, MOD_CAL, L"Failed to get CardData: 0x%0x", e.GetError());
        Disconnect(DISCONNECT_LEAVE_CARD);
    }
    catch(...)
    {
        MWLOG(LEV_CRIT, MOD_CAL, L"Failed to get CardData");
        Disconnect(DISCONNECT_LEAVE_CARD);
    }
}

CBeidCard::~CBeidCard(void)
{
}

tCardType CBeidCard::GetType()
{
    return CARD_BEID;
}

CByteArray CBeidCard::GetSerialNrBytes()
{
    return m_oSerialNr;
}

CByteArray CBeidCard::GetInfo()
{
    return m_oCardData;
}

std::string CBeidCard::GetPinpadPrefix()
{
	return "beidpp";
}

unsigned long CBeidCard::PinStatus(const tPin & Pin)
{
    // This command isn't supported on V1 cards
    if (m_oCardData.GetByte(21) < 0x20)
        return PIN_STATUS_UNKNOWN;

    try
    {
        m_ucCLA = 0x80;
        CByteArray oResp = SendAPDU(0xEA, 0x00, (unsigned char) Pin.ulPinRef, 1);
        m_ucCLA = 0x00;

        getSW12(oResp, 0x9000);

        return oResp.GetByte(0);
    }
    catch(...)
    {
        m_ucCLA = 0x00;
        throw;
    }
}

DlgPinUsage CBeidCard::PinUsage2Dlg(const tPin & Pin, const tPrivKey *pKey)
{
	DlgPinUsage usage = DLG_PIN_UNKNOWN;

	if (pKey != NULL)
	{
		if (pKey->ulID == 2)
			usage = DLG_PIN_AUTH;
		else if (pKey->ulID == 3)
			usage = DLG_PIN_SIGN;
	}
	else if (Pin.ulID == 2)
		usage = DLG_PIN_SIGN;
	else
		usage = DLG_PIN_AUTH;

	return usage;
}

void CBeidCard::showPinDialog(tPinOperation operation, const tPin & Pin, std::string & csPin1, std::string & csPin2,	const tPrivKey *pKey)
{
#ifndef NO_DIALOGS
	// Convert params
	wchar_t wsPin1[PIN_MAX_LENGTH+1];
	wchar_t wsPin2[PIN_MAX_LENGTH+1];
	DlgPinOperation pinOperation = PinOperation2Dlg(operation);
	DlgPinUsage usage = PinUsage2Dlg(Pin, pKey);
	DlgPinInfo pinInfo = {Pin.ulMinLen, Pin.ulMaxLen, PIN_FLAG_DIGITS};

	// The actual call
	DlgRet ret;
	std::wstring wideLabel = utilStringWiden(Pin.csLabel);

	if(operation==PIN_OP_CHANGE)
		ret=DlgAskPins(pinOperation,usage,wideLabel.c_str(),pinInfo,wsPin1,PIN_MAX_LENGTH+1,pinInfo,wsPin2,PIN_MAX_LENGTH+1);
	else
		ret=DlgAskPin(pinOperation,usage,wideLabel.c_str(),pinInfo,wsPin1,PIN_MAX_LENGTH+1);

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
#endif
}

bool CBeidCard::PinCmd(tPinOperation operation, const tPin & Pin,
        const std::string & csPin1, const std::string & csPin2,
        unsigned long & ulRemaining, const tPrivKey *pKey)
{
    tPin beidPin = Pin;
    // There's a path in the EF(AODF) for the PINs, but it's
    // not necessary, so we can save a Select File command
    beidPin.csPath = "";
	// Encoding is Global Platform, there is/was no way to encode
	// this in PKCS15 AODF so it says/said erroneously "BCD encoding".
	beidPin.encoding = PIN_ENC_GP;

    return CPkiCard::PinCmd(operation, beidPin, csPin1, csPin2, ulRemaining, pKey);
}

bool CBeidCard::LogOff(const tPin & Pin)
{
    m_ucCLA = 0x80;
	// No PIN has to be specified
    CByteArray oResp = SendAPDU(0xE6, 0x00, 0x00, 0);
    m_ucCLA = 0x00;

	getSW12(oResp, 0x9000);

	return true;
}

unsigned long CBeidCard::GetSupportedAlgorithms()
{
	unsigned long ulAlgos =
		SIGN_ALGO_RSA_PKCS | SIGN_ALGO_MD5_RSA_PKCS | SIGN_ALGO_SHA1_RSA_PKCS;

	if (m_ucAppletVersion >= 0x17)
	{
		ulAlgos |= SIGN_ALGO_SHA256_RSA_PKCS;
		ulAlgos |= SIGN_ALGO_SHA1_RSA_PSS;
		ulAlgos |= SIGN_ALGO_SHA256_RSA_PSS;
	}

	return ulAlgos;
}

CByteArray CBeidCard::Ctrl(long ctrl, const CByteArray & oCmdData)
{
	CAutoLock oAutoLock(this);

	switch(ctrl)
    {
    case CTRL_BEID_GETCARDDATA:
        return m_oCardData;
    case CTRL_BEID_GETSIGNEDCARDDATA:
		if (m_ucAppletVersion < 0x17)
			throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
		else
		{
			if (m_selectAppletMode == ALW_SELECT_APPLET)
				SelectApplet();
			m_ucCLA = 0x80;
			CByteArray oRet = SendAPDU(0xE4, 0x02, 0x00, 0x9C);
			m_ucCLA = 0;
			getSW12(oRet, 0x9000);
			oRet.Chop(2);
			return oRet;
		}
    case CTRL_BEID_GETSIGNEDPINSTATUS:
		// oCmdData must contain:
		// - the pin reference (1 byte)
		if (m_ucAppletVersion < 0x17)
			throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
		else
		{
			if (m_selectAppletMode == ALW_SELECT_APPLET)
				SelectApplet();
			unsigned char ucPinRef = oCmdData.GetByte(0);
			m_ucCLA = 0x80;
			CByteArray oRet = SendAPDU(0xEA, 0x02, ucPinRef, 0x81);
			m_ucCLA = 0;
			if (ShouldSelectApplet(0xEA, getSW12(oRet)))
			{
				if (SelectApplet())
				{
					m_selectAppletMode = ALW_SELECT_APPLET;
					m_ucCLA = 0x80;
					CByteArray oRet = SendAPDU(0xEA, 0x02, ucPinRef, 0x81);
					m_ucCLA = 0;
				}
			}
			getSW12(oRet, 0x9000);
			oRet.Chop(2);
			return oRet;
		}
	case CTRL_BEID_INTERNAL_AUTH:
		// oCmdData must contain:
		// - the key reference (1 byte)
		// - the challenge to be signed (20 bytes)
		if (oCmdData.Size() != 21)
			throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
		else
		{
			if (m_selectAppletMode == ALW_SELECT_APPLET)
				SelectApplet();
			unsigned char ucKeyRef = oCmdData.GetByte(0);
			CByteArray oData(22);
			oData.Append(0x94);
			oData.Append(0x14);
			oData.Append(oCmdData.GetBytes() + 1, 20);
			CByteArray oRet = SendAPDU(0x88, 0x02, ucKeyRef, oData);
			if (ShouldSelectApplet(0x88, getSW12(oRet)))
			{
				if (SelectApplet())
				{
					m_selectAppletMode = ALW_SELECT_APPLET;
					CByteArray oRet = SendAPDU(0x88, 0x02, ucKeyRef, oData);
				}
			}
			getSW12(oRet, 0x9000);
			oRet.Chop(2);
			return oRet;
		}
    default:
        MWLOG(LEV_WARN, MOD_CAL, L"Ctrl(): Unknown CRTL code %d (0x%0x) specified", ctrl, ctrl);
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
    }
}

CP15Correction* CBeidCard::GetP15Correction()
{
	return &p15correction;
}

tFileInfo CBeidCard::ParseFileInfo(CByteArray & oFCI)
{
	// We should never come here
	throw CMWEXCEPTION(EIDMW_ERR_CHECK);
}

void CBeidCard::SetSecurityEnv(const tPrivKey & key, unsigned long algo,
    unsigned long ulInputLen)
{
	// Data = [04 80 <algoref> 84 <keyref>]  (5 bytes)
    CByteArray oData(5);

	oData.Append(0x04);

	oData.Append(0x80);

	unsigned char ucAlgo;
    switch (algo)
    {
    case SIGN_ALGO_RSA_PKCS: ucAlgo = 0x01; break;
    case SIGN_ALGO_SHA1_RSA_PKCS: ucAlgo = 0x02; break;
    case SIGN_ALGO_MD5_RSA_PKCS: ucAlgo = 0x04; break;
		case SIGN_ALGO_SHA256_RSA_PKCS:
			if (m_ucAppletVersion < 0x17)
			{
				MWLOG(LEV_WARN, MOD_CAL, L"MSE SET: SIGN_ALGO_SHA256_RSA_PKCS not supported on pre V1.7 cards");
				throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
			}
			ucAlgo = 0x08;
			break;
    case SIGN_ALGO_SHA1_RSA_PSS:
        if (m_ucAppletVersion < 0x17)
        {
            MWLOG(LEV_WARN, MOD_CAL, L"MSE SET: PSS not supported on pre V1.7 cards");
            throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
        }
        ucAlgo = 0x10;
        break;
		case SIGN_ALGO_SHA256_RSA_PSS:
				if (m_ucAppletVersion < 0x17)
        {
            MWLOG(LEV_WARN, MOD_CAL, L"MSE SET: PSS not supported on pre V1.7 cards");
            throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
        }
        ucAlgo = 0x20;
        break;
    default:
        throw CMWEXCEPTION(EIDMW_ERR_ALGO_BAD);
    }
    oData.Append(ucAlgo);

	oData.Append(0x84);

	oData.Append((unsigned char) key.ulKeyRef);

    CByteArray oResp = SendAPDU(0x22, 0x41, 0xB6, oData);
	if (ShouldSelectApplet(0x22, getSW12(oResp)))
	{
		if (SelectApplet())
		{
			m_selectAppletMode = ALW_SELECT_APPLET;
			oResp = SendAPDU(0x22, 0x41, 0xB6, oData);
		}
	}

	getSW12(oResp, 0x9000);
}

CByteArray CBeidCard::SignInternal(const tPrivKey & key, unsigned long algo,
    const CByteArray & oData, const tPin *pPin)
{
	CAutoLock autolock(this);

	// For V1.7 cards, the Belpic dir has to be selected
	if (m_ucAppletVersion >= 0x17)
		SelectFile(key.csPath);
	else if (m_selectAppletMode == ALW_SELECT_APPLET)
		SelectApplet();

	SetSecurityEnv(key, algo, oData.Size());
 
	// Pretty unique for smart cards: first MSE SET, then verify PIN
    // (needed for the nonrep key/pin, but also usable for the auth key/pin)
    if (pPin != NULL)
    {
        unsigned long ulRemaining = 0;
        bool bOK = PinCmd(PIN_OP_VERIFY, *pPin, "", "", ulRemaining, &key);
        if (!bOK)
			throw CMWEXCEPTION(ulRemaining == 0 ? EIDMW_ERR_PIN_BLOCKED : EIDMW_ERR_PIN_BAD);
    }

    // PSO: Compute Digital Signature
    CByteArray oResp = SendAPDU(0x2A, 0x9E, 0x9A, oData);
	unsigned long ulSW12 = getSW12(oResp);
	if (ulSW12 != 0x9000)
		throw CMWEXCEPTION(m_poContext->m_oPCSC.SW12ToErr(ulSW12));

	// Remove SW1-SW2 from the response
	oResp.Chop(2);

	return oResp;
}

bool CBeidCard::ShouldSelectApplet(unsigned char ins, unsigned long ulSW12)
{
	if (m_selectAppletMode != TRY_SELECT_APPLET)
		return false;

	if (ins == 0xA4)
		return ulSW12 == 0x6A82 || ulSW12 == 0x6A86;
	return ulSW12 == 0x6A82 || ulSW12 == 0x6A86 || ulSW12 == 0x6D00;
}

bool CBeidCard::SelectApplet()
{
	return BeidCardSelectApplet(m_poContext, m_hCard);
}

tBelpicDF CBeidCard::getDF(const std::string & csPath, unsigned long & ulOffset)
{
	ulOffset = 0;
	if (csPath.substr(0, 4) == "3F00")
		ulOffset = 4;

	if (ulOffset < csPath.size())
	{
		std::string csPartialPath = csPath.substr(ulOffset, 4);
		if (csPartialPath == "DF00")
			return BELPIC_DF;
		//if (csPartialPath == "DF01" && m_ucAppletVersion >= 0x20)
		//	return ID_DF; // this AID doesn't exist for V1 cards
		//this AID doesn't exist for applet v1.7
	}

	return UNKNOWN_DF;
}

tFileInfo CBeidCard::SelectFile(const std::string & csPath, bool bReturnFileInfo)
{
	CPkiCard::SelectFile(csPath, false);

	// The EF(Preferences) file can be written using the authentication PIN;
	// that's the only exception to the 'read always' - 'write never' ACs.
	if (csPath.substr(csPath.size() - 4, 4) == "4039")
	{
		if (m_ucAppletVersion < 0x17)
			return PREFS_FILE_INFO_V1;
		else
			return PREFS_FILE_INFO_V2;
	}
	else
		return DEFAULT_FILE_INFO;
}

/**
 * The Belpic card doesn't support select by path (only
 * select by File ID or by AID) , so we make a loop with
 * 'select by file' commands.
 * E.g. if path = AAAABBBCCC the we do
 *   Select(AAAA)
 *   Select(BBBB)
 *   Select(CCCC)
 * If the the path contains the Belpic DF (DF00) or
 * the ID DF (DF01) then we select by AID without
 * first selected the MF (3F00) even if it is specified
 * because selection by AID always works.
 */
CByteArray CBeidCard::SelectByPath(const std::string & csPath, bool bReturnFileInfo)
{
	unsigned long ulOffset = 0;
	tBelpicDF belpicDF = getDF(csPath, ulOffset);

	if (belpicDF == UNKNOWN_DF)
	{
		// 1. Do a loop of "Select File by file ID" commands

		unsigned long ulPathLen = (unsigned long) csPath.size() / 2;
		for (ulOffset = 0; ulOffset < ulPathLen; ulOffset += 2)
		{
			CByteArray oPath(ulPathLen);
			oPath.Append(Hex2Byte(csPath, ulOffset));
			oPath.Append(Hex2Byte(csPath, ulOffset + 1));

			CByteArray oResp = SendAPDU(0xA4, 0x02, 0x0C, oPath);
			unsigned long ulSW12 = getSW12(oResp);
			if ((ulSW12 == 0x6A82 || ulSW12 == 0x6A86) && m_selectAppletMode == TRY_SELECT_APPLET)
			{
				// The file still wasn't found, so let's first try to select the applet
				if (SelectApplet())
				{
					m_selectAppletMode = ALW_SELECT_APPLET;
					oResp = SendAPDU(0xA4, 0x02, 0x0C, oPath);
				}
			}
			getSW12(oResp, 0x9000);
		}
	}
	else
	{
		// 2.a Select the BELPIC DF or the ID DF by AID

		CByteArray oAID(20);
//		if (belpicDF == BELPIC_DF)
			oAID.Append(BELPIC_AID, sizeof(BELPIC_AID));
//		else
//			oAID.Append(ID_AID, sizeof(ID_AID));

		CByteArray oResp = SendAPDU(0xA4, 0x04, 0x0C, oAID);
        unsigned long ulSW12 = getSW12(oResp);
		if ((ulSW12 == 0x6A82 || ulSW12 == 0x6A86) && m_selectAppletMode == TRY_SELECT_APPLET)
		{
			// The file still wasn't found, so let's first try to select the applet
			if (SelectApplet())
			{
				m_selectAppletMode = ALW_SELECT_APPLET;
				oResp = SendAPDU(0xA4, 0x04, 0x0C, oAID);
			}
		}
		getSW12(oResp, 0x9000);

		// 2.b Select the file inside the DF, if needed

		ulOffset += 4;
		if (ulOffset + 4 == csPath.size())
		{
			CByteArray oPath(2);
			oPath.Append(Hex2Byte(csPath, ulOffset / 2));
			oPath.Append(Hex2Byte(csPath, ulOffset / 2 + 1));

			CByteArray oResp = SendAPDU(0xA4, 0x02, 0x0C, oPath);
			unsigned long ulSW12 = getSW12(oResp);
			if (ulSW12 != 0x9000)
				throw CMWEXCEPTION(m_poContext->m_oPCSC.SW12ToErr(ulSW12));
		}
	}

	// Normally we should put here the FCI, but since Belpic cards
	// don't return it, we just return the path that can be used
	// later on to return the harcoded FCI for that file.
	return CByteArray((unsigned char *) csPath.c_str(), (unsigned long) csPath.size());
}

unsigned long CBeidCard::Get6CDelay()
{
	return m_ul6CDelay;
}

tCacheInfo CBeidCard::GetCacheInfo(const std::string &csPath)
{
	tCacheInfo dontCache = {DONT_CACHE, 0};
/*	tCacheInfo simpleCache = {SIMPLE_CACHE, 0};
	tCacheInfo certCache = {CERT_CACHE, 0};
	tCacheInfo check16Cache = {CHECK_16_CACHE, 0}; // Check 16 bytes at offset 0
	tCacheInfo checkSerial = {CHECK_SERIAL, 0}; // Check if the card serial nr is present

	// csPath -> file ID
	unsigned int uiFileID = 0;
	unsigned long ulLen = (unsigned long) (csPath.size() / 2);
	if (ulLen >= 2)
		uiFileID = 256 * Hex2Byte(csPath, ulLen - 2) + Hex2Byte(csPath, ulLen - 1);

	switch(uiFileID)
	{
	case 0x2F00: // EF(DIR)
		return m_ucAppletVersion < 0x20 ? simpleCache : dontCache;
	case 0x5031: // EF(ODF)
	case 0x5032: // EF(TokenInfo)
	case 0x5034: // EF(AODF)
	case 0x5035: // EF(PrKDF)
	case 0x5037: // EF(CDF)
	case 0x503C: // EF(Cert#8) (RRN)
	case 0x503D: // EF(Cert#9) (ID CA)
	case 0x4035: // EF(ID#Photo)
#ifdef CAL_EMULATION  // the EF(ID#RN) of the emulated test cards have the same serial nr
	case 0x4031: // EF(ID#RN)
#endif
		return simpleCache;
#ifndef CAL_EMULATION
	case 0x4031: // EF(ID#RN)
		return checkSerial;
#endif	
	case 0x4032: // EF(SGN#RN)
	case 0x4034: // EF(SGN#Adress)
		return check16Cache;
	case 0x5038: // EF(Cert#2) (authentication)
	case 0x5039: // EF(Cert#3) (non repudiation)
	case 0x503A: // EF(Cert#4) (Citizen CA)
	case 0x503B: // EF(Cert#6) (root)
		return certCache;
	}
*/
	return dontCache;
}
