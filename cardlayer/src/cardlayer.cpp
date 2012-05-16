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
#include "cardlayer.h"
#include "cache.h"

namespace eIDMW
{

CCardLayer::CCardLayer(void)
{
	m_ulReaderCount = 0;
	for (unsigned long i = 0; i < MAX_READERS; i++)
		m_tpReaders[i] = NULL;
}

CCardLayer::~CCardLayer(void)
{
	for (unsigned long i = 0; i < MAX_READERS; i++)
	{
		if (m_tpReaders[i] != NULL) {
			delete m_tpReaders[i];
			m_tpReaders[i] = NULL;
		}
	}
}

void CCardLayer::ForceRelease(void)
{
	m_oContext.m_oPCSC.ReleaseContext();
}

void CCardLayer::CancelActions(void)
{
	m_oContext.m_oPCSC.Cancel();
}

void CCardLayer::GetStatusChange(unsigned long ulTimeout,
															 SCARD_READERSTATEA *txReaderStates,
															 unsigned long ulReaderCount)
{
	m_oContext.m_oPCSC.GetTheStatusChange(ulTimeout,txReaderStates,ulReaderCount);
}

/**
 * This is something you typically do just once, unless you
 * want to check if new readers were inserted/removed.
 * Note: PKCS#11 assumes that the number of readers never
 * change, so you have to call this function only in
 * C_GetSlotList().
 */
CReadersInfo CCardLayer::ListReaders()
{
	CReadersInfo theReadersInfo;
	CByteArray oReaders;

	// Do an SCardEstablishContext() if not done yet
	try
	{
		m_oContext.m_oPCSC.EstablishContext();
		oReaders = m_oContext.m_oPCSC.ListReaders();
	}
	catch(CMWException &e)
	{
		unsigned long err = e.GetError();
		if (err == EIDMW_ERR_NO_READER)
			return theReadersInfo;

		throw;
	}

	theReadersInfo = CReadersInfo(oReaders);

	if (oReaders.Size() != 0) 
	{
		m_szDefaultReaderName = (char *) oReaders.GetBytes();
	}

	return theReadersInfo;
}

CReader & CCardLayer::getReader(const std::string &csReaderName)
{
	// Do an SCardEstablishContext() if not done yet
	m_oContext.m_oPCSC.EstablishContext();

	CReader *pRet = NULL;

	// If csReaderName == "", take the default (= first found) reader name
	const std::string *pcsReaderName = (csReaderName.size() == 0) ?
		GetDefaultReader() : &csReaderName;
	if (pcsReaderName->size() == 0)
		throw CMWEXCEPTION(EIDMW_ERR_NO_READER);

	// First check if the reader doesn't exist already
	for (unsigned long i = 0; i < MAX_READERS; i++)
	{
		if (m_tpReaders[i] != NULL) {
			if (m_tpReaders[i]->GetReaderName() == *pcsReaderName)
			{
				pRet =  m_tpReaders[i];
				break;
			}
		}
	}

	// No CReader object for this readername -> make one
	if (pRet == NULL)
	{
		for (unsigned long i = 0; i < MAX_READERS; i++)
		{
			if (m_tpReaders[i] == NULL) {
				pRet = new CReader(*pcsReaderName, &m_oContext);
				m_tpReaders[i] = pRet;
				break;
			}
		}
	}

	// No room in m_tpReaders -> throw an exception
	if (pRet == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_LIMIT);

	return *pRet;
}

std::string * CCardLayer::GetDefaultReader()
{
	std::string *pRet = &m_szDefaultReaderName;

	if (m_szDefaultReaderName.size() == 0)
	{
		CByteArray csReaders = m_oContext.m_oPCSC.ListReaders();
		if (csReaders.Size() != 0)
			m_szDefaultReaderName = (char *) csReaders.GetBytes();
	}

	return pRet;
}

bool CCardLayer::DeleteFromCache(const std::string & csSerialNr)
{
	return CCache::Delete(csSerialNr);
}

}
