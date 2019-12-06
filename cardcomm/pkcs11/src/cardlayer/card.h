
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

/**
 * Subclasses of this class implement functionality for a specific
 * type of card (e.g. BE eID, PT eID, ...)
 * This subclasses can either part of the CAL or be separate libraries
 * ('plugins'), see CardFactory.cpp for more info.
 */
#ifndef CARD_H
#define CARD_H

#include "p15objects.h"
#include "pcsc.h"
#include "internalconst.h"
#include "common/bytearray.h"
#include "common/mwexception.h"
#include "common/hash.h"
#include "common/util.h"
#include "pinpad.h"
#include "dialogs/dialogs.h"
#include "pkcs15.h"

#include <stddef.h>
#include <stdint.h>
#ifdef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
#include "beid_fuzz.h"
#endif

namespace eIDMW
{
	/*typedef enum
	{
		BELPIC_DF,
		ID_DF,
		UNKNOWN_DF,
	} tBelpicDF;*/

	typedef enum
	{
		//BEID_PINS_USE_CARDHOLDER_PIN,		//1 PIN :  (PINcardholder (01) in applets before 1.8)
		//BEID_PINS_USE_AUTH_PIN,			//3 PINS: PINauthenticate, PINnonrepudiation, PINreadEF will always use PINauthenticate ID (01)
		BEID_PINS_USE_ONE_PIN,				//The above cases should have the same flow in the code
		BEID_PINS_USE_SEPARATE_PINS			//3 PINS: PINauthenticate, PINnonrepudiation, PINreadEF will use their own IDs
	} tPinUsage;

	typedef enum {
		BEID_PIN_READ_EF,
		BEID_PIN_AUTH,
		BEID_PIN_NONREP,
	} tPinObjective;

	const static tPin PinInvalid = { false, "", 0, 0, 0, 0, 0 };
	const static tPin PinBeid = { true, "Basic PIN", 1, 0, 4, 12, 8 };
	const static tPin PinBeidAuth = { true, "Authentication PIN", 1, 0, 4, 12, 8 };
	const static tPin PinBeidSign = { true, "Signature PIN", 4, 1, 4, 12, 8 };
	const static tPin PinBeidEF = { true, "Data Access PIN", 5, 2, 4, 12, 8 };

	class CCard
	{
	public:

		CCard(SCARDHANDLE hCard, CPCSC * poPCSC, CPinpad * poPinpad,
			tSelectAppletMode selectAppletMode, tCardType cardType);
		~CCard(void);

		/** Disconnect from the card, optionally resetting it */
		void Disconnect(tDisconnectMode disconnectMode = DISCONNECT_LEAVE_CARD);

		/** Call SCardStatus() to get the ATR, and return that. */
		CByteArray GetATR();

		/** Return the serial number of the card reader */
		CByteArray GetIFDVersion();

		/** Return true if there is definitely a card in the reader, false otherwise */
		bool Status();

		/** Return true if the card reader has a PIN pad */
		bool IsPinpadReader();
		std::string GetPinpadPrefix();
		/** Return the type of the card. */
		tCardType GetType() { return m_cardType; };
		/** Convert the return value of GetSerialNrBytes() to
			an std::string, and cache it for further usage */
		std::string GetSerialNr();

		/** Returns the OR-ing of all supported crypto algorithms */
		unsigned long GetCardSupportedAlgorithms();

		/* Sign data. If necessary, a PIN will be asked */
		CByteArray CardSign(const tPrivKey & key, unsigned long algo, const CByteArray & oData);

		/** Start a transaction on the card. Can be called
			recursively, maintains a counter */
		void Lock();
		/** End a transaction on the card, as started with
			Lock(). */
		void Unlock();

		//void SelectApplication(const CByteArray & oAID);

		CByteArray ReadFile(const std::string & csPath, unsigned long ulOffset = 0, unsigned long ulMaxLen = FULL_FILE);

		CByteArray ReadRecord(const std::string & csPath, unsigned char ulRecordID);

		tPin GetPinFor(tPinObjective obj);
		tPin GetPinByKeyID(unsigned long id);
		unsigned long PinStatus(const tPin & Pin);
		bool PinCmd(tPinOperation operation, const tPin & Pin, const std::string & csPin1,
			const std::string & csPin2, unsigned long &ulRemaining, const tPrivKey * pKey = NULL);

		DlgPinUsage PinUsage2Dlg(const tPin & Pin, const tPrivKey * pKey);

		unsigned long GetSupportedAlgorithms();

		CByteArray Sign(const tPrivKey & key, const tPin & Pin, unsigned long algo, const CByteArray & oData);

		/** Send a case 1 or case 2 commands (no data is sent to the card),
			if you know it's case 1 then preferably set bDataIsReturned
			to false. */
		CByteArray SendAPDU(unsigned char ucCLA, unsigned char ucINS, unsigned char ucP1, unsigned char ucP2, unsigned long ulOutLen);

		/** Send a case 3 or case 4 commands (data is sent to the card),
		if you know it's case 1 then preferably set bDataIsReturned
		to false */
		CByteArray SendAPDU(unsigned char ucCLA, unsigned char ucINS, unsigned char ucP1, unsigned char ucP2, const CByteArray & oData);
		CByteArray SendAPDU(const CByteArray & oCmdAPDU);

		SCARDHANDLE m_hCard;

		CByteArray GetSerialNrBytes();

		std::string GetCardLabel();

		/* Read the file indicated by 'csPath'.
		 * This path can be absolute, relative or empty
		* (in which case the currenlty selected file is read)
		* If too much bytes are specified by ulMaxLen, no
		* exception is throw, the function just returns the
		* number of bytes that are available. */
		CByteArray ReadCardFile(const std::string & csPath, unsigned long ulOffset = 0, unsigned long ulMaxLen = FULL_FILE);

		/* Read record with Type bRecordID the file indicated by 'csPath'.
		 * This path can be absolute, relative or empty
		* (in which case the currenlty selected file is read)
		*/
		CByteArray ReadRecordFromFile(const std::string & csPath, unsigned char bRecordID);

		//--- P15 functions
		unsigned long PinCount();

		unsigned long CertCount();

		/** ulIndex ranges from 0 to CertCount() - 1 */
		tCert GetCert(unsigned long ulIndex);

		/** If bValid == false, then no PIN with this ID was found */
		tCert GetCertByID(unsigned long ulID);

		unsigned long PrivKeyCount();

		/** ulIndex ranges from 0 to PrivKeyCount() - 1 */
		tPrivKey GetPrivKey(unsigned long ulIndex);

		/** If bValid == false, then no PIN with this ID was found */
		tPrivKey GetPrivKeyByID(unsigned long ulID);

		/** Return the output of the GET_CARD_DATA command to
		the eID card (or nothing if it's not an eID card) */
		/** Returns 3 bytes:
		 *   - the appletversion (1 byte): 0x10, 0x11, 0x20
		 *   - the global OS version (2 bytes) */
		CByteArray GetInfo();

		bool LogOff();

		unsigned int GetPrivKeySize();
		unsigned char GetAppletVersion();

	protected:
		// How long to wait (msec) before re-sending an APDU when SW12 = 6CXX is returned
		unsigned long Get6CDelay();

		unsigned char Hex2Byte(char cHex);
		unsigned char Hex2Byte(const std::string & csHex, unsigned long ulIdx);
		bool IsDigit(char c);
		std::vector < unsigned long >m_verifiedPINs;

		CByteArray ReadBinary(unsigned long ulOffset, unsigned long ulLen);
		CByteArray UpdateBinary(unsigned long ulOffset, const CByteArray & oData);
		unsigned char PinUsage2Pinpad(const tPin & Pin, const tPrivKey * pKey);
		DlgPinOperation PinOperation2Dlg(tPinOperation operation);

		CByteArray MakePinCmd(tPinOperation operation, const tPin & Pin);
		CByteArray MakePinBuf(const tPin & Pin, const std::string & csPin, bool bEmptyPin);

		CByteArray VerifyAndSign(const tPrivKey & key, const tPin & Pin,
			unsigned long algo, const CByteArray & oData);
		void AskPin(tPinOperation operation, const tPin & Pin, std::string & csPin1,
			std::string & csPin2, const tPrivKey * pKey);
		bool AskPinRetry(tPinOperation operation, const tPin & Pin, unsigned long ulRemaining,
			const tPrivKey * pKey);

		/** Return true if the serial number is present in oData, false otherwise */
		bool SerialNrPresent(const CByteArray & oData);

		/** If ulExpected is provided and differs from the return code, an MWException is thrown */
		unsigned long getSW12(const CByteArray & oRespAPDU, unsigned long ulExpected = 0);

		bool ShouldSelectApplet(unsigned char ins, unsigned long ulSW12);
		bool SelectApplet();

//		tBelpicDF getDF(const std::string & csPath, unsigned long &ulOffset);
		void SelectFile(const std::string & csPath);
		CByteArray SelectByPath(const std::string & csPath);

		void showPinDialog(tPinOperation operation, const tPin & Pin, std::string & csPin1,
			std::string & csPin2, const tPrivKey * pKey);

		void SetSecurityEnv(const tPrivKey & key, unsigned long algo, unsigned long ulInputLen);
		CByteArray SignInternal(const tPrivKey & key, unsigned long algo, const CByteArray & oData,
			const tPin * pPin = NULL);

		CPCSC *m_poPCSC;
		CPinpad *m_poPinpad;
		tCardType m_cardType;
		unsigned long m_ulLockCount;
		bool m_bSerialNrString;
		tSelectAppletMode m_selectAppletMode;
		unsigned long m_ulRemaining[3];
		tPinUsage m_pinUsage;
		CByteArray m_oCardData;
		CByteArray m_oSerialNr;
		unsigned char m_ucAppletVersion;
		unsigned long m_ul6CDelay;

#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4251)	// m_csSerialNr does not need to have dll-interface
#endif
		std::string m_csSerialNr;
#ifdef WIN32
#pragma warning(pop)
#endif

	private:
		// No copies allowed
		CCard(const CCard & oCard);
		CCard & operator =(const CCard & oCard);

		CPKCS15 m_oPKCS15;

	};

	class CAutoLock
	{
	public:
		CAutoLock(CCard * poCard);

		/** Warning: no lock counter is kept (as opposed as for
		 *  the CAutoLock(CCard *poCard) ctor; so make sure this
		 *  object gets out of scope before making a new one for
		 *  the same card handle! */
		CAutoLock(CPCSC * poPCSC, SCARDHANDLE hCard);

		~CAutoLock();

	private:
		CCard * m_poCard;
		CPCSC *m_poPCSC;
		SCARDHANDLE m_hCard;
	};

	CCard *BeidCardGetInstance(const char *csReader,
		SCARDHANDLE hCard, CPCSC * poPCSC, CPinpad * poPinpad);

	CCard *UnknownCardGetInstance(const char *csReader,
		SCARDHANDLE hCard, CPCSC * poPCSC, CPinpad * poPinpad);
}

#endif
