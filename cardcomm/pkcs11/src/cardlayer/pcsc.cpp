
/* ****************************************************************************

* eID Middleware Project.
* Copyright (C) 2010-2017 FedICT.
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
#ifdef UNICODE
#undef UNICODE
#endif

#include "pcsc.h"
#include "internalconst.h"
#include "common/configuration.h"
#include "common/mwexception.h"
#include "common/log.h"
#include "common/thread.h"
#include "common/util.h"
#include <exception>
//#include <Winsvc.h>

#define EID_RECOVER_RETRIES	10

namespace eIDMW
{
	static SCARD_IO_REQUEST m_ioSendPci;
	static SCARD_IO_REQUEST m_ioRecvPci;

	                 CPCSC::CPCSC()
	{
		CConfig config;

		        m_ulCardTxDelay =
			config.
			GetLong(CConfig::
				EIDMW_CONFIG_PARAM_GENERAL_CARDTXDELAY);
		        m_hContext = 0;
		        m_iTimeoutCount = 0;
		        m_iListReadersCount = 0;
	}

	CPCSC::  ~CPCSC(void)
	{
		ReleaseContext();
	}

	void CPCSC::EstablishContext()
	{
		if (m_hContext == 0)
		{
			SCARDCONTEXT hCtx = 0;
			long lRet =
				SCardEstablishContext(SCARD_SCOPE_USER, NULL,
						      NULL, &hCtx);
			MWLOG(LEV_DEBUG, MOD_CAL,
			      L"    SCardEstablishContext(): 0x%0x", lRet);
			if (SCARD_S_SUCCESS != lRet)
				throw CMWEXCEPTION(PcscToErr(lRet));

			m_hContext = hCtx;
		}
	}

	void CPCSC::ReleaseContext()
	{
		if (m_hContext != 0)
		{
			//              SCardCancel(m_hContext);
			SCardReleaseContext(m_hContext);
			m_hContext = 0;
		}
	}

	void CPCSC::Cancel()
	{
		if (m_hContext != 0)
		{
			SCardCancel(m_hContext);
		}
	}

	CByteArray CPCSC::ListReaders()
	{
		char csReaders[1024];
		DWORD dwReadersLen = sizeof(csReaders);

		long lRet =
			SCardListReaders(m_hContext, NULL, csReaders,
					 &dwReadersLen);
		if (SCARD_S_SUCCESS != lRet || m_iListReadersCount < 6)
		{
			MWLOG(LEV_DEBUG, MOD_CAL,
			      L"    SCardListReaders(): 0x%0x", lRet);
			m_iListReadersCount++;
		}
		if (SCARD_S_SUCCESS == lRet)
		{
			return CByteArray((unsigned char *) csReaders,
					  dwReadersLen);
		} else if ((long) SCARD_E_NO_READERS_AVAILABLE == lRet)
		{
			return CByteArray();
		} else
		{
			ReleaseContext();
			throw CMWEXCEPTION(PcscToErr(lRet));
		}
	}

	/*
	   static char *state2string(char *buf, unsigned long state)
	   {
	   sprintf(buf, "%0x = %0x 0000", state, state / 0x10000);
	   if (state & SCARD_STATE_UNPOWERED)
	   strcat(buf, " | SCARD_STATE_UNPOWERED");
	   if (state & SCARD_STATE_MUTE)
	   strcat(buf, " | SCARD_STATE_MUTE");
	   if (state & SCARD_STATE_INUSE)
	   strcat(buf, " | SCARD_STATE_INUSE");
	   if (state & SCARD_STATE_EXCLUSIVE)
	   strcat(buf, " | SCARD_STATE_EXCLUSIVE");
	   if (state & SCARD_STATE_ATRMATCH)
	   strcat(buf, " | SCARD_STATE_ATRMATCH");
	   if (state & SCARD_STATE_PRESENT)
	   strcat(buf, " | SCARD_STATE_PRESENT");
	   if (state & SCARD_STATE_EMPTY)
	   strcat(buf, " | SCARD_STATE_EMPTY");
	   if (state & SCARD_STATE_UNAVAILABLE)
	   strcat(buf, " | SCARD_STATE_UNAVAILABLE");
	   if (state & SCARD_STATE_UNKNOWN)
	   strcat(buf, " | SCARD_STATE_UNKNOWN");
	   if (state & SCARD_STATE_CHANGED)
	   strcat(buf, " | SCARD_STATE_CHANGED");
	   if (state & SCARD_STATE_IGNORE)
	   strcat(buf, " | SCARD_STATE_IGNORE");
	   if (state == SCARD_STATE_UNAWARE)
	   strcat(buf, " | SCARD_STATE_UNAWARE");

	   return buf;
	   }
	   char csCurrState[200];
	   char csNextState[200];
	 */

	bool CPCSC::GetStatusChange(unsigned long ulTimeout,
				    tReaderInfo * pReaderInfos,
				    unsigned long ulReaderCount)
	{
		bool bChanged = false;

		SCARD_READERSTATEA txReaderStates[MAX_READERS];
		DWORD tChangedState[MAX_READERS];

		// Convert from tReaderInfo[] -> SCARD_READERSTATE array
		for (DWORD i = 0; i < ulReaderCount; i++)
		{
			//initialize all members with zero to prevent issues with remote card readers
			memset(&txReaderStates[i], 0, sizeof(SCARD_READERSTATEA));
			txReaderStates[i].szReader = pReaderInfos[i].csReader.c_str();
			txReaderStates[i].dwCurrentState = pReaderInfos[i].ulEventState;
			txReaderStates[i].cbAtr = 0;
			txReaderStates[i].pvUserData = 0;
		}

	      wait_again:
		long lRet = SCardGetStatusChange(m_hContext, ulTimeout, txReaderStates, ulReaderCount);
		if ((long) SCARD_E_TIMEOUT != lRet)
		{
			if (SCARD_S_SUCCESS != lRet)
				throw CMWEXCEPTION(PcscToErr(lRet));

			// On Windows, often/always the SCARD_STATE_CHANGED is always set,
			// and in case of a remove/insert or insert/remove, you have to do a
			// second SCardGetStatusChange() to get the final reader state.
			for (DWORD i = 0; i < ulReaderCount; i++)
			{
#ifdef WIN32
				// There's a SCARD_STATE_EMPTY and a SCARD_STATE_PRESENT flag.
				// So we take the exor of the current and the event state for
				// both flags; if the exor isn't 0 then at least 1 of the flags
				// changed value
				DWORD exor1 = (txReaderStates[i].dwCurrentState & (SCARD_STATE_EMPTY | SCARD_STATE_PRESENT))
					^ (txReaderStates[i].dwEventState & (SCARD_STATE_EMPTY | SCARD_STATE_PRESENT));
				bool bUnpowered = false;	// Ignore this state

				//((txReaderStates[i].dwCurrentState & SCARD_STATE_UNPOWERED) == 0) &&
				//((txReaderStates[i].dwEventState & SCARD_STATE_UNPOWERED) != 0);
				tChangedState[i] = ((exor1 == 0) && !bUnpowered) ? 0 : SCARD_STATE_CHANGED;
#else
				tChangedState[i] =
					txReaderStates[i].
					dwEventState & SCARD_STATE_CHANGED;
#endif
				bChanged |= (tChangedState[i] != 0);
			}

#ifdef WIN32
			if (bChanged)
			{
				for (DWORD i = 0; i < ulReaderCount; i++)
				{
					// take previous state, reset bits that are not supported as input
					txReaderStates[i].dwCurrentState = (txReaderStates[i].dwEventState & ~SCARD_STATE_CHANGED & ~SCARD_STATE_UNKNOWN);
					txReaderStates[i].pvUserData = 0;
				}
				long lRet = SCardGetStatusChange(m_hContext, 0, txReaderStates, ulReaderCount);
				if (SCARD_S_SUCCESS != lRet
				    && SCARD_E_TIMEOUT != lRet)
					throw CMWEXCEPTION(PcscToErr(lRet));
			}
#endif

			// Update the event states in pReaderInfos
			for (DWORD i = 0; i < ulReaderCount; i++)
			{
				pReaderInfos[i].ulCurrentState = pReaderInfos[i].ulEventState;
				// Clear and SCARD_STATE_CHANGED flag, and use tChangedState instead
				pReaderInfos[i].ulEventState = (txReaderStates[i].dwEventState & ~SCARD_STATE_CHANGED) | tChangedState[i];
			}

			// Sometimes, it seems we're getting here even without a status change,
			// so in this case we'll just go waiting again, if there's no timeout
			if (!bChanged)
			{
				unsigned long ulDelay = ulTimeout > 250 ? 250 : ulTimeout;
				if (ulTimeout != TIMEOUT_INFINITE)
					ulTimeout -= ulDelay;
				if (ulTimeout != 0)
				{
					CThread::SleepMillisecs(ulDelay);
					goto wait_again;
				}
			}
		}

		return bChanged;
	}

	bool CPCSC::Status(const std::string & csReader)
	{
		SCARD_READERSTATEA xReaderState;

		//initialize all members with zero's in order to prevent failures with remote card readers
		memset(&xReaderState, 0, sizeof(SCARD_READERSTATEA));
		xReaderState.szReader = csReader.c_str();
		xReaderState.dwCurrentState = 0;
		xReaderState.cbAtr = 0;

		long lRet = SCardGetStatusChange(m_hContext, 0, &xReaderState, 1);
		if (SCARD_S_SUCCESS != lRet)
		{
			MWLOG(LEV_ERROR, MOD_CAL, L"    SCardGetStatusChange returned: 0x%0x", lRet);
			throw CMWEXCEPTION(PcscToErr(lRet));
		}

		return (xReaderState.dwEventState & SCARD_STATE_PRESENT) == SCARD_STATE_PRESENT;
	}

	SCARDHANDLE CPCSC::Connect(const std::string & csReader,
				   unsigned long ulShareMode,
				   unsigned long ulPreferredProtocols)
	{
		DWORD dwProtocol;

		//      DWORD dwCounter = 0;
		SCARDHANDLE hCard = 0;

		dwProtocol = 1;

		//    MWLOG(LEV_DEBUG, MOD_CAL, L"    Calling connect: %0x, %ls, 0x%0x, %0x\n", m_hContext, utilStringWiden(csReader).c_str(), ulShareMode, ulPreferredProtocols);

		long lRet = SCardConnect(m_hContext, csReader.c_str(),
					 ulShareMode, ulPreferredProtocols,
					 &hCard, &dwProtocol);

		/*      if (SCARD_S_SUCCESS != lRet)
		   {
		   long eidError = PcscToErr(lRet);
		   while ( ((EIDMW_ERR_CANT_CONNECT == eidError) || (EIDMW_ERR_CARD_COMM != eidError)) && (dwCounter < 10) )
		   {
		   lRet = SCardConnect(m_hContext, csReader.c_str(),
		   ulShareMode, ulPreferredProtocols, &hCard, &dwProtocol);
		   eidError = PcscToErr(lRet);
		   dwCounter++;
		   CThread::SleepMillisecs(200);
		   }
		   }
		 */
		MWLOG(LEV_DEBUG, MOD_CAL, L"    SCardConnect(%ls): 0x%0x",
		      utilStringWiden(csReader).c_str(), lRet);

		if ((long) SCARD_E_NO_SMARTCARD == lRet)
			hCard = 0;
		else if (SCARD_S_SUCCESS != lRet)
			throw CMWEXCEPTION(PcscToErr(lRet));

		else
		{
			m_ioSendPci.dwProtocol = dwProtocol;
			m_ioSendPci.cbPciLength = sizeof(SCARD_IO_REQUEST);
			m_ioRecvPci.dwProtocol = dwProtocol;
			m_ioRecvPci.cbPciLength = sizeof(SCARD_IO_REQUEST);

			// If you do an SCardTransmit() too fast after an SCardConnect(),
			// some cards/readers will return an error (e.g. 0x801002f)
			CThread::SleepMillisecs(200);
		}

		return hCard;
	}

	void CPCSC::Disconnect(SCARDHANDLE hCard,
			       tDisconnectMode disconnectMode)
	{
		DWORD dwDisposition =
			disconnectMode ==
			DISCONNECT_RESET_CARD ? SCARD_RESET_CARD :
			SCARD_LEAVE_CARD;

		long lRet = SCardDisconnect(hCard, dwDisposition);

		MWLOG(LEV_DEBUG, MOD_CAL,
		      L"    SCardDisconnect(0x%0x): 0x%0x ; mode: %d", hCard,
		      lRet, dwDisposition);
		if (SCARD_S_SUCCESS != lRet)
			throw CMWEXCEPTION(PcscToErr(lRet));
	}

	CByteArray CPCSC::GetATR(SCARDHANDLE hCard)
	{
		DWORD dwReaderLen = 0;
		DWORD dwState, dwProtocol;
		unsigned char tucATR[64];
		DWORD dwATRLen = sizeof(tucATR);

		long lRet = SCardStatus(hCard, NULL, &dwReaderLen,
					&dwState, &dwProtocol, tucATR,
					&dwATRLen);
		MWLOG(LEV_DEBUG, MOD_CAL, L"    SCardStatus(0x%0x): 0x%0x",
		      hCard, lRet);
		if (SCARD_S_SUCCESS != lRet)
			throw CMWEXCEPTION(PcscToErr(lRet));

		return CByteArray(tucATR, dwATRLen);
	}

	CByteArray CPCSC::GetIFDVersion(SCARDHANDLE hCard)
	{
		unsigned char tucIFDVers[4] = { 0, 0, 0, 0 };
		DWORD dwIFDVersLen = sizeof(tucIFDVers);

		long lRet =
			SCardGetAttrib(hCard, SCARD_ATTR_VENDOR_IFD_VERSION,
				       tucIFDVers, &dwIFDVersLen);

		MWLOG(LEV_DEBUG, MOD_CAL, L"    SCardGetAttrib(0x%0x): 0x%0x",
		      hCard, lRet);

		return CByteArray(tucIFDVers, dwIFDVersLen);
	}

	bool CPCSC::Status(SCARDHANDLE hCard)
	{
		DWORD dwReaderLen = 0;
		DWORD dwState, dwProtocol;
		unsigned char tucATR[64];
		DWORD dwATRLen = sizeof(tucATR);
		static int iStatusCount = 0;

		long lRet = SCardStatus(hCard, NULL, &dwReaderLen,
					&dwState, &dwProtocol, tucATR,
					&dwATRLen);

		if (iStatusCount < 5 || SCARD_S_SUCCESS != lRet)
		{
			iStatusCount++;
			MWLOG(LEV_DEBUG, MOD_CAL, L"    SCardStatus(0x%0x): 0x%0x", hCard, lRet);
		}

		return SCARD_S_SUCCESS == lRet;
	}

	CByteArray CPCSC::Transmit(SCARDHANDLE hCard,
				   const CByteArray & oCmdAPDU,
				   long *plRetVal, void *pSendPci,
				   void *pRecvPci)
	{
		unsigned char tucRecv[APDU_BUF_LEN];

		memset(tucRecv, 0, sizeof(tucRecv));
		DWORD dwRecvLen = sizeof(tucRecv);

		unsigned char ucINS =
			oCmdAPDU.Size() >= 4 ? oCmdAPDU.GetByte(1) : 0;
		unsigned long ulLen = ucINS == 0xA4
			|| ucINS == 0x22 ? 0xFFFFFFFF : 5;

		SCARD_IO_REQUEST *pioSendPci = (pSendPci != NULL) ? (SCARD_IO_REQUEST *) pSendPci : &m_ioSendPci;
		SCARD_IO_REQUEST *pioRecvPci = (pRecvPci != NULL) ? (SCARD_IO_REQUEST *) pRecvPci : &m_ioRecvPci;

		MWLOG(LEV_DEBUG, MOD_CAL, L"      SCardTransmit(%ls)", oCmdAPDU.ToWString(true, true, 0, ulLen).c_str());
		//MWLOG(LEV_DEBUG, MOD_CAL, L"      SCardTransmit pioSendPci (dwProtocol = 0X%x, cbPciLength = 0x%x)", pioSendPci->dwProtocol, pioSendPci->cbPciLength);
		//MWLOG(LEV_DEBUG, MOD_CAL, L"      SCardTransmit pioRecvPci (dwProtocol = 0X%x, cbPciLength = 0x%x)", pioRecvPci->dwProtocol, pioRecvPci->cbPciLength);

		// Very strange: sometimes an SCardTransmit() returns a communications
		// error or a SW12 = 6D 00 error.
		// It occurs with most readers (some more then others) and depends heavily
		// on the type of card (e.g. nearly always with the test Kids card).
		// It seems to be fixed when adding a delay before sending something to the card...
		CThread::SleepMillisecs(m_ulCardTxDelay);

#ifdef __APPLE__
		int iRetryCount = 0;

	      try_again:
#endif
		long lRet = SCardTransmit(hCard,
					  pioSendPci, oCmdAPDU.GetBytes(),
					  (DWORD) oCmdAPDU.Size(),
					  pioRecvPci, tucRecv, &dwRecvLen);

		*plRetVal = lRet;
		if (SCARD_S_SUCCESS != lRet)
		{
#ifdef __APPLE__
			if (SCARD_E_SHARING_VIOLATION == lRet
			    && iRetryCount < 3)
			{
				iRetryCount++;
				CThread::SleepMillisecs(500);
				goto try_again;
			}
#endif
			MWLOG(LEV_DEBUG, MOD_CAL,
			      L"        SCardTransmit(): 0x%0x", lRet);
			throw CMWEXCEPTION(PcscToErr(lRet));
		}
		// Don't log the full response for privacy reasons, only SW1-SW2
		//MWLOG(LEV_DEBUG, MOD_CAL, L"        SCardTransmit(): %ls", CByteArray(tucRecv, (unsigned long) dwRecvLen).ToWString(true, true, 0, (unsigned long) dwRecvLen).c_str() );
		MWLOG(LEV_DEBUG, MOD_CAL,
		      L"        SCardTransmit(): SW12 = %02X %02X",
		      tucRecv[dwRecvLen - 2], tucRecv[dwRecvLen - 1]);
		//check response, and add 25 ms delay when error was returned

		if ((tucRecv[dwRecvLen - 2] != 0x90)
		    && (tucRecv[dwRecvLen - 1] != 0x00)
		    && (tucRecv[dwRecvLen - 2] != 0x61))
		{
			CThread::SleepMillisecs(25);
		}

		return CByteArray(tucRecv, (unsigned long) dwRecvLen);
	}



	void CPCSC::Recover(SCARDHANDLE hCard, unsigned long *pulLockCount)
	{
		//try to recover when the card is not responding (properly) anymore

		DWORD ap = 0;
		int i = 0;
		long lRet = SCARD_F_INTERNAL_ERROR;

		MWLOG(LEV_WARN, MOD_CAL,
		      L"Card is not responding properly, trying to recover...");

		for (i = 0;
		     (i < EID_RECOVER_RETRIES) && (lRet != SCARD_S_SUCCESS);
		     i++)
		{
			if (i != 0)
				CThread::SleepMillisecs(1000);

			lRet = SCardReconnect(hCard, SCARD_SHARE_SHARED,
					      SCARD_PROTOCOL_T0,
					      SCARD_RESET_CARD, &ap);
			if (lRet != SCARD_S_SUCCESS)
			{
				MWLOG(LEV_DEBUG, MOD_CAL,
				      L"        [%d] SCardReconnect errorcode: [0x%02X]",
				      i, lRet);
				continue;
			}
			// transaction is lost after an SCardReconnect()
			if (*pulLockCount > 0)
			{
				lRet = SCardBeginTransaction(hCard);
				if (lRet != SCARD_S_SUCCESS)
				{
					MWLOG(LEV_DEBUG, MOD_CAL,
					      L"        [%d] SCardBeginTransaction errorcode: [0x%02X]",
					      i, lRet);
					if (i == (EID_RECOVER_RETRIES - 1))
					{
						*pulLockCount = 0;	//failed starting a new transaction
					}
					continue;
				}
				*pulLockCount = 1;
			}

			MWLOG(LEV_INFO, MOD_CAL,
			      L"        Card recovered in loop %d", i);
		}
	}


	CByteArray CPCSC::Control(SCARDHANDLE hCard, unsigned long ulControl,
				  const CByteArray & oCmd,
				  unsigned long ulMaxResponseSize)
	{
		MWLOG(LEV_DEBUG, MOD_CAL,
		      L"      SCardControl(ctrl=0x%0x, %ls)", ulControl,
		      oCmd.ToWString(true, true, 0, 5).c_str());

		unsigned char *pucRecv = new unsigned char[ulMaxResponseSize];

		if (pucRecv == NULL)
			throw CMWEXCEPTION(EIDMW_ERR_MEMORY);
		DWORD dwRecvLen = ulMaxResponseSize;

#ifndef __OLD_PCSC_API__
		long lRet = SCardControl(hCard, ulControl,
					 oCmd.GetBytes(), (DWORD) oCmd.Size(),
					 pucRecv, dwRecvLen, &dwRecvLen);
#else
		long lRet = SCardControl((SCARDHANDLE) hCard,
					 oCmd.GetBytes(), (DWORD) oCmd.Size(),
					 pucRecv, &dwRecvLen);
#endif
		if (SCARD_S_SUCCESS != lRet)
		{
			MWLOG(LEV_DEBUG, MOD_CAL,
			      L"        SCardControl() err: 0x%0x", lRet);
			delete[]pucRecv;
			throw CMWEXCEPTION(PcscToErr(lRet));
		}

		if (dwRecvLen == 2)
		{
			MWLOG(LEV_DEBUG, MOD_CAL,
			      L"        SCardControl(): 2 bytes returned: 0x%02X%02X",
			      pucRecv[0], pucRecv[1]);
		} else
			MWLOG(LEV_DEBUG, MOD_CAL,
			      L"        SCardControl(): %02d bytes returned",
			      dwRecvLen);

		CByteArray oResp(pucRecv, (unsigned long) dwRecvLen);

		delete[]pucRecv;

		return oResp;
	}

	void CPCSC::BeginTransaction(SCARDHANDLE hCard)
	{
		long lRet = SCardBeginTransaction(hCard);

		MWLOG(LEV_DEBUG, MOD_CAL,
		      L"    SCardBeginTransaction(0x%0x): 0x%0x", hCard,
		      lRet);
		if (SCARD_S_SUCCESS != lRet)
			throw CMWEXCEPTION(PcscToErr(lRet));
	}

	void CPCSC::EndTransaction(SCARDHANDLE hCard)
	{
		long lRet = SCardEndTransaction(hCard, SCARD_LEAVE_CARD);

		MWLOG(LEV_DEBUG, MOD_CAL,
		      L"    SCardEndTransaction(0x%0x): 0x%0x", hCard, lRet);
	}

	long CPCSC::SW12ToErr(unsigned long ulSW12)
	{
		long lRet = EIDMW_ERR_CARD;

		switch (ulSW12)
		{
			case 0x9000:
				lRet = EIDMW_OK;
				break;
			case 0x6982:
				lRet = EIDMW_ERR_NOT_AUTHENTICATED;
				break;
			case 0x6B00:
				lRet = EIDMW_ERR_BAD_P1P2;
				break;
			case 0x6A86:
				lRet = EIDMW_ERR_BAD_P1P2;
				break;
			case 0x6986:
				lRet = EIDMW_ERR_CMD_NOT_ALLOWED;
				break;
			case 0x6A82:
				lRet = EIDMW_ERR_FILE_NOT_FOUND;
				break;
			case 0x6400:
				lRet = EIDMW_ERR_NOT_AUTHENTICATED;
				break;
			case 0x6581:
				lRet = EIDMW_ERR_EEPROM;
				break;
			case 0x6700:
				lRet = EIDMW_ERR_LEN_BAD;
				break;
			case 0x6985:
				lRet = EIDMW_ERR_BAD_COND;
				break;
			case 0x6D00:
				lRet = EIDMW_ERR_CMD_NOT_AVAIL;
				break;
			case 0x6E00:
				lRet = EIDMW_ERR_CLASS_BAD;
				break;
		}

		return lRet;
	}

	//unsigned long CPCSC::GetContext()
	SCARDCONTEXT CPCSC::GetContext()
	{
		return m_hContext;
	}

	long CPCSC::PcscToErr(unsigned long lPcscErr)
	{
		long lRet = EIDMW_ERR_CARD;

		switch (lPcscErr)
		{
			case SCARD_E_CANCELLED:
				lRet = EIDMW_ERR_CANCELLED;
				break;
			case SCARD_E_PROTO_MISMATCH:
			case SCARD_E_COMM_DATA_LOST:
			case SCARD_F_COMM_ERROR:
				lRet = EIDMW_ERR_CARD_COMM;
				break;
			case SCARD_E_INSUFFICIENT_BUFFER:
				lRet = EIDMW_ERR_PARAM_RANGE;
				break;
			case SCARD_E_INVALID_PARAMETER:
				lRet = EIDMW_ERR_PARAM_BAD;
				break;
			case SCARD_W_REMOVED_CARD:
				lRet = EIDMW_ERR_NO_CARD;
				break;
			case SCARD_E_NO_ACCESS:
				lRet = EIDMW_ERR_CMD_NOT_ALLOWED;
				break;
			case SCARD_W_UNRESPONSIVE_CARD:
			case SCARD_W_UNPOWERED_CARD:
			case SCARD_W_UNSUPPORTED_CARD:
				lRet = EIDMW_ERR_CANT_CONNECT;
				break;
			case SCARD_E_NO_SERVICE:
			case SCARD_E_SERVICE_STOPPED:
				lRet = EIDMW_ERR_NO_READER;
				break;
			case SCARD_W_RESET_CARD:
				lRet = EIDMW_ERR_CARD_RESET;
				break;
			case SCARD_E_SHARING_VIOLATION:
				lRet = EIDMW_ERR_CARD_SHARING;
				break;
			case SCARD_E_NOT_TRANSACTED:
				lRet = EIDMW_ERR_NOT_TRANSACTED;
				break;
		}

		return lRet;
	}

	long CPCSC::GetTheStatusChange(unsigned long ulTimeout,
				       SCARD_READERSTATEA * txReaderStates,
				       unsigned long ulReaderCount)
	{
		long lRet;

		do
		{
			lRet = SCardGetStatusChange(m_hContext,
						    ulTimeout, txReaderStates,
						    ulReaderCount);
			if ((long) SCARD_E_TIMEOUT != lRet)
			{
				if (SCARD_S_SUCCESS != lRet)
				{
					MWLOG(LEV_DEBUG, MOD_CAL, L"    SCardGetStatusChange returns: 0x%0x", lRet);
					throw CMWEXCEPTION(PcscToErr(lRet));
				}
			}
		}
		while ((lRet == SCARD_E_TIMEOUT)
		       && (ulTimeout == TIMEOUT_INFINITE));

		return lRet;
	}

/*	long CPCSC::PCSCServiceRunning(bool* pRunning)
	{
#ifndef WIN32
		return 0;
#else
		SC_HANDLE hSCManager = 0;
		SC_HANDLE hService = 0;
		DWORD dwOSVersion = GetVersion();
		DWORD dwCorrectedOSVersion = 0;
		DWORD dwError = 0;
		DWORD dwBytesNeeded = 0;
		SERVICE_STATUS_PROCESS serviceStatusInfo;

		dwOSVersion &= 0x0000FFFF;
		//swap the version bytes
		dwCorrectedOSVersion = ((dwOSVersion&0x000000FF)<<8)|(((dwOSVersion&0x0000FF00)>>8));
		//version number win8 is 6.2
		if(dwCorrectedOSVersion >= 0x0602)
		{
			hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
			if(hSCManager == NULL)
			{
				dwError = GetLastError();
				MWLOG(LEV_WARN, MOD_CAL, L"OpenSCManager returned NULL, err = %.08x", dwError);
				return dwError;
			}
			hService = OpenService(hSCManager,"SCardSvr",SERVICE_QUERY_STATUS);//|SERVICE_START//SERVICE_ALL_ACCESS
			if(hService == NULL)
			{
				dwError = GetLastError();//ERROR_ACCESS_DENIED 5
				MWLOG(LEV_WARN, MOD_CAL, L"OpenService returned NULL, err = %.08x", dwError);
				return dwError;
			}
			//check if service is stopped
			if (!QueryServiceStatusEx( hService, SC_STATUS_PROCESS_INFO, (LPBYTE) &serviceStatusInfo,
				sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded ) )
			{
				dwError = GetLastError();
				MWLOG(LEV_WARN, MOD_CAL,L"QueryServiceStatusEx failed err = %.08x", dwError);
				CloseServiceHandle(hService); 
				CloseServiceHandle(hSCManager);
				return dwError; 
			}

			if( serviceStatusInfo.dwCurrentState == SERVICE_RUNNING )
			{
				MWLOG(LEV_INFO, MOD_CAL,L"SCardSvr is already running");
				CloseServiceHandle(hService); 
				CloseServiceHandle(hSCManager);
				*pRunning = true;
				return 0; 
			}
			else
			{
				MWLOG(LEV_INFO, MOD_CAL,L"SCardSvr is not running");
				CloseServiceHandle(hService); 
				CloseServiceHandle(hSCManager);
				*pRunning = false;
				return 0; 
			}
		}
		//OS before 8
		*pRunning = true;
		return 0;
#endif
	}

	void CPCSC::StartPCSCService()
	{
#ifndef WIN32
		return;
#else
		SC_HANDLE hSCManager = 0;
		SC_HANDLE hService = 0;
		DWORD dwOSVersion = GetVersion();
		DWORD dwCorrectedOSVersion = 0;
		DWORD dwError;
		DWORD dwBytesNeeded = 0;
		SERVICE_STATUS_PROCESS serviceStatusInfo;
		DWORD dwRetries = 0;

		dwOSVersion &= 0x0000FFFF;
		//swap the version bytes
		dwCorrectedOSVersion = ((dwOSVersion&0x000000FF)<<8)|(((dwOSVersion&0x0000FF00)>>8));
		//version number win8 is 6.2
		if(dwCorrectedOSVersion >= 0x0602)
		{
			hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
			if(hSCManager == NULL)
			{
				dwError = GetLastError();
				MWLOG(LEV_WARN, MOD_CAL, L"OpenSCManager returned NULL, err = %.08x", dwError);
				return;
			}
			hService = OpenService(hSCManager,"SCardSvr",SERVICE_QUERY_STATUS);//|SERVICE_START//SERVICE_ALL_ACCESS
			if(hService == NULL)
			{
				dwError = GetLastError();//ERROR_ACCESS_DENIED 5
				MWLOG(LEV_WARN, MOD_CAL, L"OpenService returned NULL, err = %.08x", dwError);
				return;
			}
			//check if service is stopped
			if (!QueryServiceStatusEx( hService, SC_STATUS_PROCESS_INFO, (LPBYTE) &serviceStatusInfo,
				sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded ) )
			{
				dwError = GetLastError();
				MWLOG(LEV_WARN, MOD_CAL,L"QueryServiceStatusEx failed err = %.08x", dwError);
				CloseServiceHandle(hService); 
				CloseServiceHandle(hSCManager);
				return; 
			}

			if( (serviceStatusInfo.dwCurrentState != SERVICE_STOPPED) && (serviceStatusInfo.dwCurrentState != SERVICE_STOP_PENDING) )
			{
				MWLOG(LEV_INFO, MOD_CAL,L"SCardSvr is already running");
				CloseServiceHandle(hService); 
				CloseServiceHandle(hSCManager);
				return; 
			}
			else
			{
				while( (serviceStatusInfo.dwCurrentState == SERVICE_STOP_PENDING) & (dwRetries < 6) )
				{
					CThread::SleepMillisecs(250);
					//service is stopping, wait for it to stop
					//check if service is stopped
					if (!QueryServiceStatusEx( hService, SC_STATUS_PROCESS_INFO, (LPBYTE) &serviceStatusInfo,
						sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded ) )
					{
						dwError = GetLastError();
						MWLOG(LEV_WARN, MOD_CAL,L"QueryServiceStatusEx failed err = %.08x", dwError);
						CloseServiceHandle(hService); 
						CloseServiceHandle(hSCManager);
						return; 
					}
					dwRetries++;
				}
				if(dwRetries == 6)
				{
					MWLOG(LEV_WARN, MOD_CAL,L"SERVICE_STOP_PENDING still in progress");
					CloseServiceHandle(hService); 
					CloseServiceHandle(hSCManager);
				}
				//service is stopped, start it
				if (!StartService( hService, 0, NULL) )
				{
					dwError = GetLastError();
					MWLOG(LEV_ERROR, MOD_CAL,L"StartService failed err = %.08x", dwError);
					CloseServiceHandle(hService); 
					CloseServiceHandle(hSCManager);
					return; 
				}
				dwRetries = 0;
				do
				{
					CThread::SleepMillisecs(250);
					//service is starting, wait for it to be running
					if (!QueryServiceStatusEx( hService, SC_STATUS_PROCESS_INFO, (LPBYTE) &serviceStatusInfo,
						sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded ) )
					{
						dwError = GetLastError();
						MWLOG(LEV_WARN, MOD_CAL,L"QueryServiceStatusEx failed err = %.08x", dwError);
						CloseServiceHandle(hService); 
						CloseServiceHandle(hSCManager);
						return; 
					}
					dwRetries++;
				}
				while( (serviceStatusInfo.dwCurrentState == SERVICE_START_PENDING) & (dwRetries < 6) );
			}
		}
#endif
	}
	*/
}				//end namespace
