
/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2014 FedICT.
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

/*
Takes care of
 - communication with the reader/smart card
       - via the Privacy service, if running
       - directly via PCSC otherwise
 - pinpad handling (loading of pinpad libs, making pinpad commands, ...)
*/

#ifndef PCSC_H
#define PCSC_H

#include "common/eiderrors.h"
#include "common/bytearray.h"
#include "common/mwexception.h"
#include "cardlayerconst.h"
#include "internalconst.h"

#include <winscard.h>

#ifndef WIN32

#include "wintypes.h"
//#include "reader.h"

// fix for gc issue #47 - compatible with pcsc-lite-1.6.4
// but still works on older pcsc-lite versions which *do* define SCARD_READERSTATE_A themselves
#ifndef SCARD_READERSTATE_A
#define SCARD_READERSTATE_A SCARD_READERSTATE
#endif

#ifndef SCARD_READERSTATEA
#define SCARD_READERSTATEA SCARD_READERSTATE_A
#endif
// needed for pcsclite version earlier than 1.4
#ifndef SCARD_E_NO_READERS_AVAILABLE

#define SCARD_E_NO_READERS_AVAILABLE 0x8010002E	/** Cannot find smart card reader */
#endif
#ifndef SCARD_PROTOCOL_UNDEFINED
#define SCARD_PROTOCOL_UNDEFINED         0x00
#endif
#ifndef SCARD_E_COMM_DATA_LOST
#define SCARD_E_COMM_DATA_LOST           ((DWORD)0x8010002FL)
#endif
#ifndef SCARD_E_NO_ACCESS
#define SCARD_E_NO_ACCESS                ((DWORD)0x80100027)
#endif
#ifndef SCARD_ATTR_VENDOR_IFD_VERSION
#define SCARD_ATTR_VENDOR_IFD_VERSION	 ((DWORD)0x00010102)
#endif

#endif

#define IOCTL_SMARTCARD_SET_CARD_TYPE	SCARD_CTL_CODE(2060)

//#include <winscard.h>

namespace eIDMW
{

// Copied from PCSC
#define EIDMW_STATE_CHANGED   0x00000002
#define EIDMW_STATE_PRESENT   0x00000020

/**
 * Provides an abstraction of the PCSC SCARD_READERSTATE struct.
 * The ulCurrentState and ulEventState correspond to the
 * dwCurrentState resp. dwEventState in the SCARD_READERSTATE struct,
 * except that the SCARD_STATE_CHANGED is only set when a card
 * insert/removal occurred (as opposed to Windows' PCSC).
 */
	typedef struct
	{
		std::string csReader;
		unsigned long ulCurrentState;	// the state when we last checked
		unsigned long ulEventState;	// the state after the new check
	} tReaderInfo;

	class CPCSC
	{
public:
		CPCSC(void);
		~CPCSC(void);

		bool m_bSSO; // force Single Sign-On
		unsigned long m_ulConnectionDelay;

		void EstablishContext();

		void ReleaseContext();
		void Cancel();

	/**
	 * We can't return a string because the output is a "multistring",
	 * which means a multiple strings separated by a 0x00 and ended
	 * by 2 0x00 bytes.
	 */
		CByteArray ListReaders();

		//created a new function for pkcs11, as we don't want to change the CSP's behaviour
		//this is temporary, pkcs11 will move away from using this cardlayer
		long GetTheStatusChange(unsigned long ulTimeout, SCARD_READERSTATEA * txReaderStates, unsigned long ulReaderCount);


		long Status(const std::string & csReader, bool &bCardStillPresent);

		SCARDHANDLE Connect(const std::string & csReader, unsigned long ulShareMode = SCARD_SHARE_SHARED,
				    unsigned long ulPreferredProtocols = SCARD_PROTOCOL_T0 );
		void Disconnect(SCARDHANDLE hCard, tDisconnectMode disconnectMode);

		CByteArray GetATR(SCARDHANDLE hCard);
		CByteArray GetIFDVersion(SCARDHANDLE hCard);

	/**
	 * Returns true if the same card is still present,
	 * false if the card has been removed (and perhaps
	 * the same or antoher card has been inserted).
	 */
		bool Status(SCARDHANDLE hCard);

		CByteArray Transmit(SCARDHANDLE hCard, const CByteArray & oCmdAPDU, long *plRetVal, void *pSendPci = NULL, void *pRecvPci = NULL);
		void Recover(SCARDHANDLE hCard, unsigned long *pulLockCount);
		CByteArray Control(SCARDHANDLE hCard, unsigned long ulControl, const CByteArray & oCmd, unsigned long ulMaxResponseSize = CTRL_BUF_LEN);

		void BeginTransaction(SCARDHANDLE hCard);
		void EndTransaction(SCARDHANDLE hCard);

		//unsigned long GetContext();
		SCARDCONTEXT GetContext();

		long SW12ToErr(unsigned long ulSW12);
		long PcscToErr(unsigned long lRet);

private:	

		//unsigned long m_hContext;
		SCARDCONTEXT m_hContext;

		friend class CPinpad;

		int m_iTimeoutCount;

		unsigned long m_ulCardTxDelay;	//delay before each transmission to a smartcard; in millie-seconds, default 1

	};

}
#endif
