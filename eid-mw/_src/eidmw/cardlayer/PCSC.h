/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
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

#include "../common/eidErrors.h"
#include "../common/ByteArray.h"
#include "../common/MWException.h"
#include "CardLayerConst.h"
#include "InternalConst.h"

#include <winscard.h>

#ifndef WIN32

#include "wintypes.h"
#include "Reader.h"
#ifndef SCARD_READERSTATEA
#define SCARD_READERSTATEA SCARD_READERSTATE_A
#endif
// needed for pcsclite version earlier than 1.4
#ifndef SCARD_E_NO_READERS_AVAILABLE
#define SCARD_E_NO_READERS_AVAILABLE 0x8010002E /** Cannot find smart card reader */
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
typedef struct  {
    std::string csReader;
    unsigned long ulCurrentState; // the state when we last checked
    unsigned long ulEventState;   // the state after the new check
} tReaderInfo;

class EIDMW_CAL_API CPCSC
{
public:
    CPCSC(void);
    ~CPCSC(void);

	void EstablishContext();

	void ReleaseContext();

	/**
	 * We can't return a string because the output is a "multistring",
	 * which means a multiple strings separated by a 0x00 and ended
	 * by 2 0x00 bytes.
	 */
	CByteArray ListReaders();

	/** Returns true if something changed */
	bool GetStatusChange(unsigned long ulTimeout,
		tReaderInfo *pReaderInfos, unsigned long ulReaderCount);

	bool Status(const std::string &csReader);

    unsigned long Connect(const std::string &csReader,
		unsigned long ulShareMode = SCARD_SHARE_SHARED,
		unsigned long ulPreferredProtocols = SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1);
    void Disconnect(unsigned long hCard, tDisconnectMode disconnectMode);

	CByteArray GetATR(unsigned long hCard);
	CByteArray GetIFDVersion(unsigned long hCard);

	/**
	 * Returns true if the same card is still present,
	 * false if the card has been removed (and perhaps
	 * the same or antoher card has been inserted).
	 */
	bool Status(unsigned long hCard);

	CByteArray Transmit(unsigned long hCard, const CByteArray &oCmdAPDU,
		void *pSendPci = NULL, void *pRecvPci = NULL);
	CByteArray Control(unsigned long hCard, unsigned long ulControl,
		const CByteArray &oCmd, unsigned long ulMaxResponseSize = CTRL_BUF_LEN);

    void BeginTransaction(unsigned long hCard);
    void EndTransaction(unsigned long hCard);

	//unsigned long GetContext();
	SCARDCONTEXT GetContext();

    long SW12ToErr(unsigned long ulSW12);

private:
	long PcscToErr(unsigned long lRet);

    //unsigned long m_hContext;
	SCARDCONTEXT m_hContext;

	friend class CPinpad;

	int m_iTimeoutCount;
	int m_iListReadersCount;

    unsigned long m_ulCardTxDelay;          //delay before each transmission to a smartcard; in millie-seconds, default 1

};

}
#endif
