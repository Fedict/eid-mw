
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

#pragma once

#include "reader.h"
#include "readersinfo.h"
#include "context.h"

namespace eIDMW
{

	class CCardLayer
	{
public:

	/**
	 * Calling the CAL (Card Abstraction Layer) starts with the
	 * constructing a CCardLayer object.
	 * Calling the constructor doesn't cause a connection to the smart
	 * card resource manager yet (no SCardEstablishContext() yet)
	 */
		CCardLayer(void);

		    ~CCardLayer(void);

	/**
	 * Release the connection to the smart card resource manager (a
	 * call to SCardReleaseContext(); this is also done in the destructor.
	 * WARNING: this will cause all CReadersInfo and CReader object to
	 * become unusable. Calling afterwards one of the functions below will
	 * cause a re-connection to the smart card resource manager.
	 */
		void ForceRelease();

		/* Terminates all outstanding actions within the resource manager context
		 * (a call to SCardCancel)
		 */
		void CancelActions();

		/* reestablish the pcsc context */
		void PCSCReEstablishContext();

		/* check if the smartcard service is running */
		long PCSCServiceRunning(bool * pRunning);

		/* check if the smartcard service is running, and if not, try to start it */
		//void StartPCSCService();

		/* capture all status changes into txReaderStates
		 * (a call to SCardGetStatusChange)
		 */
		long GetStatusChange(unsigned long ulTimeout,
				     SCARD_READERSTATEA * txReaderStates,
				     unsigned long ulReaderCount);

	/**
	 * Return the list of all available readers, plus info on the
	 * presence of cards in those readers.
	 */
		CReadersInfo ListReaders();

	/**
	 * Get a CReader object for the reader with name csReaderName;
	 * no connection is made yet to the card that might be present
	 * in the reader.
	 * The list of csReaderNames can be found with ListReaers().
	 * If csReader == "", then the first found reader is taken
	 */






		         
			 
			 CReader & getReader(const std::string &
					     csReaderName);

private:
		// No copies allowed
		      CCardLayer(const CCardLayer & oCardLayer);
		      CCardLayer & operator =(const CCardLayer & oCardLayer);
		      std::string * GetDefaultReader();

		CContext m_oContext;

#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4251)	//m_szDefaultReaderName does not need a dll interface
#endif
		         std::string m_szDefaultReaderName;
#ifdef WIN32
#pragma warning(pop)
#endif

		unsigned long m_ulReaderCount;
		CReader *m_tpReaders[MAX_READERS];
	};

}
