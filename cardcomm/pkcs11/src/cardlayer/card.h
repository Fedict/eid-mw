
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
 * type of card (e.g. BE eID, PT eID, SIS, ...)
 * This subclasses can either part of the CAL or be separate libraries
 * ('plugins'), see CardFactory.cpp for more info.
 */
#ifndef CARD_H
#define CARD_H

#include "p15objects.h"
#include "context.h"
#include "internalconst.h"
#include "common/bytearray.h"
#include "common/mwexception.h"
#include "common/hash.h"
#include "common/util.h"
#include "pinpad.h"
#include "p15correction.h"
#include "dialogs/dialogs.h"

namespace eIDMW
{
	/** Compatibility version for the entire plugin-relevant API: V1.00 */
	const unsigned long PLUGIN_VERSION = 100;

	class EIDMW_CAL_API CCard
	{
public:
		CCard(SCARDHANDLE hCard, CContext * poContext,
		      CPinpad * poPinpad);
		virtual ~CCard(void);

		/** Find out which card is present and return the appropriate subclass */
		static CCard *Connect(const std::string & csReader,
				      CContext * poContext,
				      CPinpad * poPinpad);

		/** Disconnect from the card, optionally resetting it */
		void Disconnect(tDisconnectMode disconnectMode =
					DISCONNECT_LEAVE_CARD);

		/** Call SCardStatus() to get the ATR, and return that. */
		CByteArray GetATR();

		/** Return the serial number of the card reader */
		CByteArray GetIFDVersion();

		/** Return true if we can use our current cardhandle (m_poCard) with the card, false otherwise */
		bool Status();

		/** Return true if the card reader has a PIN pad */
		virtual bool IsPinpadReader();
		virtual std::string GetPinpadPrefix();
		/** Return the type of the card. */
		tCardType GetType() { return m_cardType; };
		/** Convert the return value of GetSerialNrBytes() to
		    an std::string, and cache it for further usage */
		virtual std::string GetSerialNr();
		/** Return the serial number of the card */
		virtual CByteArray GetSerialNrBytes();
		/** Return a string describing the type of card */
		virtual std::string GetLabel();
		/** Return the output of the GET_CARD_DATA command to
		    the eID card (or nothing if it's not an eID card) */
		virtual CByteArray GetInfo();

		/** Start a transaction on the card. Can be called
		    recursively, maintains a counter */
		void Lock();
		/** End a transaction on the card, as started with
		    Lock(). */
		void Unlock();

		virtual void SelectApplication(const CByteArray & oAID);

		CByteArray ReadFile(const std::string & csPath,
					    unsigned long ulOffset =
					    0, unsigned long ulMaxLen =
					    FULL_FILE, bool bDoNotCache =
					    false);
		virtual tCacheInfo GetCacheInfo(const std::string & csPath);

		virtual CByteArray ReadUncachedFile(const std::string &
						    csPath,
						    unsigned long ulOffset =
						    0,
						    unsigned long ulMaxLen =
						    FULL_FILE) = 0;

		virtual unsigned long PinStatus(const tPin & Pin);
		virtual bool PinCmd(tPinOperation operation, const tPin & Pin,
				    const std::string & csPin1,
				    const std::string & csPin2,
				    unsigned long &ulRemaining,
				    const tPrivKey * pKey = NULL);

		virtual DlgPinUsage PinUsage2Dlg(const tPin & Pin,
						 const tPrivKey * pKey);

		virtual unsigned long GetSupportedAlgorithms();
		virtual unsigned int GetRSAKeySize() = 0;
		virtual unsigned char GetAppletVersion() = 0;

		virtual CByteArray Sign(const tPrivKey & key,
					const tPin & Pin, unsigned long algo,
					const CByteArray & oData);

		/** Send a case 1 or case 2 commands (no data is sent to the card),
		    if you know it's case 1 then preferably set bDataIsReturned
		    to false. */
		CByteArray SendAPDU(unsigned char ucINS,
					    unsigned char ucP1,
					    unsigned char ucP2,
					    unsigned long ulOutLen);

    		/** Send a case 3 or case 4 commands (data is sent to the card),
		    if you know it's case 1 then preferably set bDataIsReturned
		    to false */
		CByteArray SendAPDU(unsigned char ucINS,
					    unsigned char ucP1,
					    unsigned char ucP2,
					    const CByteArray & oData);
		CByteArray SendAPDU(const CByteArray & oCmdAPDU);

		/* retrieve the correction class for PINs, certificates and private keys */
		virtual CP15Correction *GetP15Correction();

		SCARDHANDLE m_hCard;

protected:
		// How long to wait (msec) before re-sending an APDU when SW12 = 6CXX is returned
		virtual unsigned long Get6CDelay();

		virtual unsigned char Hex2Byte(char cHex);
		virtual unsigned char Hex2Byte(const std::string & csHex,
					       unsigned long ulIdx);
		virtual bool IsDigit(char c);

		/** Return true if the serial number is present in oData, false otherwise */
		bool SerialNrPresent(const CByteArray & oData);

	/** If ulExpected is provided and differs from the return code, an MWException is thrown */
		virtual unsigned long getSW12(const CByteArray & oRespAPDU,
					      unsigned long ulExpected = 0);

		CContext *m_poContext;
		CPinpad *m_poPinpad;
		tCardType m_cardType;
		unsigned long m_ulLockCount;
		bool m_bSerialNrString;
#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4251)	// m_csSerialNr does not need to have dll-interface
#endif
		     std::string m_csSerialNr;
#ifdef WIN32
#pragma warning(pop)
#endif

		unsigned char m_ucCLA;

private:
		// No copies allowed
		     CCard(const CCard & oCard);
		      CCard & operator =(const CCard & oCard);
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

}
#endif
