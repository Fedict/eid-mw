
/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2012 FedICT.
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

#include <string>
#include <cstring>
#include "pcsc.h"

namespace eIDMW
{

	class CCardLayer;

	class CReadersInfo
	{
public:

	/**
	 * Creates an empty object
	 */
		CReadersInfo();

	/**
	 * Return the amount of readers, this value never changes.
	 */
		unsigned long ReaderCount();

	/**
	 * Return the reader name, this value never changes.
	 * - ulIndex ranges from 0 to (but not included) ReaderCount.
	 */
	std::string ReaderName(unsigned long ulIndex);

	/**
	 * Whether or not the state of the reader has changed since the last Update()
	 * - ulIndex ranges from 0 to (but not included) ReaderCount.
	 */
		bool ReaderStateChanged(unsigned long ulIndex);	// card inserted or removed

	/**
	 * Whether or not the last Update() found a card in the reader
	 * - ulIndex ranges from 0 to (but not included) ReaderCount.
	 */
		bool CardPresent(unsigned long ulIndex);

		/* Fill in the txReaderStates array */
		bool GetReaderStates(SCARD_READERSTATEA * txReaderStates,
				     unsigned long length,
				     unsigned long *ulnReaders);

		/*update m_tInfos with the new reader states */
		bool UpdateReaderStates(SCARD_READERSTATEA * txReaderStates,
					unsigned long ulnReaders);

		/* free allocated memory in txReaderStates */
		void FreeReaderStates(SCARD_READERSTATEA * txReaderStates,
				      unsigned long ulnReaders);

		bool IsFirstTime(void);
		void SetFirstTime(bool firstTime);

		bool SameList(CReadersInfo * newReadersInfo);
		void ClearChanged(unsigned long reader);

private:
		CReadersInfo(const CByteArray & oReaders);

		bool bFirstTime;
		unsigned long m_ulReaderCount;
		tReaderInfo m_tInfos[MAX_READERS];

		friend class CCardLayer;	// calls the CReadersInfo constructor

	};

}
