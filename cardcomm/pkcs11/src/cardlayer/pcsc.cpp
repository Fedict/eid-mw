
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

#define EID_RECOVER_RETRIES	10

namespace eIDMW
{
	static SCARD_IO_REQUEST m_ioSendPci;
	static SCARD_IO_REQUEST m_ioRecvPci;

	CPCSC::CPCSC()
	{
		CConfig config;
		m_bSSO = CConfig::GetLong(CConfig::EIDMW_CONFIG_PARAM_SECURITY_SINGLESIGNON) != 0;
		m_ulConnectionDelay = CConfig::GetLong(CConfig::EIDMW_CONFIG_PARAM_GENERAL_CARDCONNDELAY);
		m_ulCardTxDelay = config.GetLong(CConfig::EIDMW_CONFIG_PARAM_GENERAL_CARDTXDELAY);
		m_hContext = 0;
		m_iTimeoutCount = 0;
	}

	CPCSC::~CPCSC(void)
	{
		ReleaseContext();
	}

	void CPCSC::EstablishContext()
	{
		if (m_hContext == 0)
		{
			SCARDCONTEXT hCtx = 0;
			long lRet = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &hCtx);
			MWLOG(LEV_DEBUG, MOD_CAL, L"    SCardEstablishContext(): 0x%0x", lRet);
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

		long lRet = SCardListReaders(m_hContext, NULL, csReaders, &dwReadersLen);
		if (SCARD_S_SUCCESS != lRet)
		{
			MWLOG(LEV_DEBUG, MOD_CAL, L"    SCardListReaders() returned error: 0x%0x", lRet);

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
		SCARDHANDLE hCard = 0;
		dwProtocol = 1;

		long lRet = SCardConnect(m_hContext, csReader.c_str(), (uint32_t)ulShareMode, (uint32_t)ulPreferredProtocols, &hCard, &dwProtocol);

		MWLOG(LEV_DEBUG, MOD_CAL, L"    SCardConnect(%ls): 0x%0x", utilStringWiden(csReader).c_str(), lRet);

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

	void CPCSC::Disconnect(SCARDHANDLE hCard, tDisconnectMode disconnectMode)
	{
		DWORD dwDisposition = disconnectMode == DISCONNECT_RESET_CARD ? SCARD_RESET_CARD : SCARD_LEAVE_CARD;

		long lRet = SCardDisconnect(hCard, dwDisposition);

		MWLOG(LEV_DEBUG, MOD_CAL, L"    SCardDisconnect(0x%0x): 0x%0x ; mode: %d", hCard, lRet, dwDisposition);
		if (SCARD_S_SUCCESS != lRet)
			throw CMWEXCEPTION(PcscToErr(lRet));
	}

	CByteArray CPCSC::GetATR(SCARDHANDLE hCard)
	{
		DWORD dwReaderLen = 0;
		DWORD dwState, dwProtocol;
		unsigned char tucATR[64];
		DWORD dwATRLen = sizeof(tucATR);

		long lRet = SCardStatus(hCard, NULL, &dwReaderLen, &dwState, &dwProtocol, tucATR, &dwATRLen);
		MWLOG(LEV_DEBUG, MOD_CAL, L"    SCardStatus(0x%0x): 0x%0x", hCard, lRet);
		if (SCARD_S_SUCCESS != lRet)
			throw CMWEXCEPTION(PcscToErr(lRet));

		return CByteArray(tucATR, dwATRLen);
	}

	CByteArray CPCSC::GetIFDVersion(SCARDHANDLE hCard)
	{
		unsigned char tucIFDVers[4] = { 0, 0, 0, 0 };
		DWORD dwIFDVersLen = sizeof(tucIFDVers);

		long lRet = SCardGetAttrib(hCard, SCARD_ATTR_VENDOR_IFD_VERSION, tucIFDVers, &dwIFDVersLen);

		MWLOG(LEV_DEBUG, MOD_CAL, L"    SCardGetAttrib(0x%0x): 0x%0x", hCard, lRet);

		return CByteArray(tucIFDVers, dwIFDVersLen);
	}

	bool CPCSC::Status(SCARDHANDLE hCard)
	{
		DWORD dwReaderLen = 0;
		DWORD dwState, dwProtocol;
		unsigned char tucATR[64];
		DWORD dwATRLen = sizeof(tucATR);
		static int iStatusCount = 0;

		long lRet = SCardStatus(hCard, NULL, &dwReaderLen, &dwState, &dwProtocol, tucATR, &dwATRLen);

		if (iStatusCount < 5 || SCARD_S_SUCCESS != lRet)
		{
			iStatusCount++;
			MWLOG(LEV_DEBUG, MOD_CAL, L"    SCardStatus(0x%0x): 0x%0x", hCard, lRet);
		}

		return SCARD_S_SUCCESS == lRet;
	}

	CByteArray CPCSC::Transmit(SCARDHANDLE hCard, const CByteArray & oCmdAPDU, long *plRetVal, void *pSendPci, void *pRecvPci)
	{
		unsigned char tucRecv[APDU_BUF_LEN];

		memset(tucRecv, 0, sizeof(tucRecv));
		DWORD dwRecvLen = sizeof(tucRecv);

		unsigned char ucINS = oCmdAPDU.Size() >= 4 ? oCmdAPDU.GetByte(1) : 0;
		unsigned long ulLen = ucINS == 0xA4 || ucINS == 0x22 ? 0xFFFFFFFF : 5;

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
		CThread::SleepMillisecs((int)m_ulCardTxDelay);

#ifdef __APPLE__
		int iRetryCount = 0;

	      try_again:
#endif
		long lRet = SCardTransmit(hCard, pioSendPci, oCmdAPDU.GetBytes(), (DWORD) oCmdAPDU.Size(), pioRecvPci, tucRecv, &dwRecvLen);

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
			MWLOG(LEV_DEBUG, MOD_CAL, L"        SCardTransmit(): 0x%0x", lRet);
			throw CMWEXCEPTION(PcscToErr(lRet));
		}
		// Don't log the full response for privacy reasons, only SW1-SW2
		//MWLOG(LEV_DEBUG, MOD_CAL, L"        SCardTransmit(): %ls", CByteArray(tucRecv, (unsigned long) dwRecvLen).ToWString(true, true, 0, (unsigned long) dwRecvLen).c_str() );
		MWLOG(LEV_DEBUG, MOD_CAL, L"        SCardTransmit(): SW12 = %02X %02X", tucRecv[dwRecvLen - 2], tucRecv[dwRecvLen - 1]);
		//check response, and add 25 ms delay when error was returned

		if ((tucRecv[dwRecvLen - 2] != 0x90) && (tucRecv[dwRecvLen - 1] != 0x00) && (tucRecv[dwRecvLen - 2] != 0x61))
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

		MWLOG(LEV_WARN, MOD_CAL, L"Card is not responding properly, trying to recover...");

		for (i = 0; (i < EID_RECOVER_RETRIES) && (lRet != SCARD_S_SUCCESS); i++)
		{
			if (i != 0)
				CThread::SleepMillisecs(1000);

			lRet = SCardReconnect(hCard, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0, SCARD_RESET_CARD, &ap);
			if (lRet != SCARD_S_SUCCESS)
			{
				MWLOG(LEV_DEBUG, MOD_CAL, L"        [%d] SCardReconnect errorcode: [0x%02X]", i, lRet);
				continue;
			}
			// transaction is lost after an SCardReconnect()
			if (*pulLockCount > 0)
			{
				lRet = SCardBeginTransaction(hCard);
				if (lRet != SCARD_S_SUCCESS)
				{
					MWLOG(LEV_DEBUG, MOD_CAL,  L"        [%d] SCardBeginTransaction errorcode: [0x%02X]", i, lRet);
					if (i == (EID_RECOVER_RETRIES - 1))
					{
						*pulLockCount = 0;	//failed starting a new transaction
					}
					continue;
				}
				*pulLockCount = 1;
			}

			MWLOG(LEV_INFO, MOD_CAL, L"        Card recovered in loop %d", i);
		}
	}


	CByteArray CPCSC::Control(SCARDHANDLE hCard, unsigned long ulControl, const CByteArray & oCmd, unsigned long ulMaxResponseSize)
	{
		MWLOG(LEV_DEBUG, MOD_CAL, L"      SCardControl(ctrl=0x%0x, %ls)", ulControl, oCmd.ToWString(true, true, 0, 5).c_str());

		unsigned char *pucRecv = new unsigned char[ulMaxResponseSize];

		if (pucRecv == NULL)
			throw CMWEXCEPTION(EIDMW_ERR_MEMORY);
		DWORD dwRecvLen = (DWORD)ulMaxResponseSize;

#ifndef __OLD_PCSC_API__
		long lRet = SCardControl(hCard, (uint32_t)ulControl, oCmd.GetBytes(), (uint32_t)oCmd.Size(), pucRecv, (uint32_t)dwRecvLen, (uint32_t *)&dwRecvLen);
#else
		long lRet = SCardControl((SCARDHANDLE) hCard, oCmd.GetBytes(), (uint32_t) oCmd.Size(), pucRecv, (uint32_t *)&dwRecvLen);
#endif
		if (SCARD_S_SUCCESS != lRet)
		{
			MWLOG(LEV_DEBUG, MOD_CAL, L"        SCardControl() err: 0x%0x", lRet);
			delete[]pucRecv;
			throw CMWEXCEPTION(PcscToErr(lRet));
		}

		if (dwRecvLen == 2)
		{
			MWLOG(LEV_DEBUG, MOD_CAL, L"        SCardControl(): 2 bytes returned: 0x%02X%02X", pucRecv[0], pucRecv[1]);
		} else
			MWLOG(LEV_DEBUG, MOD_CAL, L"        SCardControl(): %02d bytes returned", dwRecvLen);

		CByteArray oResp(pucRecv, (unsigned long) dwRecvLen);

		delete[]pucRecv;

		return oResp;
	}

	void CPCSC::BeginTransaction(SCARDHANDLE hCard)
	{
		long lRet = SCardBeginTransaction(hCard);

		MWLOG(LEV_DEBUG, MOD_CAL, L"    SCardBeginTransaction(0x%0x): 0x%0x", hCard, lRet);
		if (SCARD_S_SUCCESS != lRet)
			throw CMWEXCEPTION(PcscToErr(lRet));
	}

	void CPCSC::EndTransaction(SCARDHANDLE hCard)
	{
		long lRet = SCardEndTransaction(hCard, SCARD_LEAVE_CARD);

		MWLOG(LEV_DEBUG, MOD_CAL, L"    SCardEndTransaction(0x%0x): 0x%0x", hCard, lRet);
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
			case SCARD_E_UNKNOWN_READER:
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

	long CPCSC::GetTheStatusChange(unsigned long ulTimeout, SCARD_READERSTATEA * txReaderStates, unsigned long ulReaderCount)
	{
		long lRet;

		do
		{
			lRet = SCardGetStatusChange(m_hContext, (uint32_t)ulTimeout, txReaderStates, (uint32_t)ulReaderCount);
			MWLOG(LEV_DEBUG, MOD_CAL, L"    SCardGetStatusChange: current status 0x%0x, event status 0x%0x", txReaderStates->dwCurrentState, txReaderStates->dwEventState);
			if ((long) SCARD_E_TIMEOUT != lRet)
			{
				if (SCARD_S_SUCCESS != lRet)
				{
					MWLOG(LEV_DEBUG, MOD_CAL, L"    SCardGetStatusChange returns: 0x%0x", lRet);
					throw CMWEXCEPTION(PcscToErr(lRet));
				}
			}
		}
		while ((lRet == SCARD_E_TIMEOUT) && (ulTimeout == TIMEOUT_INFINITE));

		return lRet;
	}

}				//end namespace
