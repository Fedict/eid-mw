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
#include "readersinfo.h"

namespace eIDMW
{

CReadersInfo::CReadersInfo()
{
	bFirstTime = true;
	m_ulReaderCount = 0;
}

//contains the virtual reader incase of #define PKCS11_V2_20
unsigned long CReadersInfo::ReaderCount()
{
	return m_ulReaderCount;
}

std::string CReadersInfo::ReaderName(unsigned long ulIndex)
{
	if (ulIndex >= m_ulReaderCount)
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);

    return m_tInfos[ulIndex].csReader;
}
/*
bool CReadersInfo::CheckReaderEvents(unsigned long ulTimeout,
	unsigned long ulIndex)
{
	bool bChanged = false;

	if (bFirstTime)
	{
		m_poPCSC->GetStatusChange(0, m_tInfos, m_ulReaderCount);
		bFirstTime = false;
	}

wait_again:
	if (ulIndex == ALL_READERS)
	{
		bChanged = m_poPCSC->GetStatusChange(ulTimeout,	m_tInfos, m_ulReaderCount);
	}
	else
	{
		if (ulIndex >= m_ulReaderCount)
			throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);

		bChanged = m_poPCSC->GetStatusChange(ulTimeout, &m_tInfos[ulIndex], 1);
	}

	// Extra safety: if nothing changed and we should wait forever
	// (for a change), the we go waiting again
	if (!bChanged &&  (ulTimeout == TIMEOUT_INFINITE))
		goto wait_again;

	return bChanged;
}
*/
bool CReadersInfo::IsReaderInserted(unsigned long ulIndex)
{
	unsigned long eventCount = (m_tInfos[ulIndex].ulEventState >> 16) & 0x00FF;
	unsigned long currentCount = (m_tInfos[ulIndex].ulCurrentState >> 16) & 0x00FF;
	if( (strcmp(m_tInfos[ulIndex].csReader.c_str(),"\\\\?PnP?\\Notification") == 0) &&
		(eventCount > currentCount) ){
		return true;
	}
	return false;
}

bool CReadersInfo::ReaderStateChanged(unsigned long ulIndex)
{
	if (ulIndex >= m_ulReaderCount)
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);

	return (m_tInfos[ulIndex].ulEventState != m_tInfos[ulIndex].ulCurrentState);
	//return ( (m_tInfos[ulIndex].ulEventState & (SCARD_STATE_EMPTY | SCARD_STATE_PRESENT)) != (m_tInfos[ulIndex].ulCurrentState & (SCARD_STATE_EMPTY | SCARD_STATE_PRESENT)) );
    //return 0 != (m_tInfos[ulIndex].ulEventState & EIDMW_STATE_CHANGED);
}

bool CReadersInfo::CardPresent(unsigned long ulIndex)
{
	/*if (bFirstTime)
	{
		m_poPCSC->GetStatusChange(0, m_tInfos, m_ulReaderCount);
		bFirstTime = false;
	}*/

	if (ulIndex >= m_ulReaderCount)
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);

    return 0 != (m_tInfos[ulIndex].ulEventState & EIDMW_STATE_PRESENT);
}

/** Constructor */
CReadersInfo::CReadersInfo(const CByteArray & oReaders)
{
	bFirstTime = true;
	m_ulReaderCount = 0;

	//Parse the string reader-list into the array "m_tcsReaders"
	const char *csReaders = (const char *) oReaders.GetBytes();
	size_t i;


#ifdef PKCS11_V2_20
	for (i = 0;csReaders != NULL && csReaders[0] != '\0' && i < MAX_READERS-1; i++, m_ulReaderCount++)
#else
	for (i = 0;csReaders != NULL && csReaders[0] != '\0' && i < MAX_READERS; i++, m_ulReaderCount++)
#endif
	{
		m_tInfos[m_ulReaderCount].csReader = csReaders;
		m_tInfos[m_ulReaderCount].ulCurrentState = 0;
		m_tInfos[m_ulReaderCount].ulEventState = 0;
		csReaders += m_tInfos[m_ulReaderCount].csReader.length() + 1;
	}
#ifdef PKCS11_V2_20
	//Add an extra hidden reader to detect new attached reader events
	m_tInfos[m_ulReaderCount].csReader = "\\\\?PnP?\\Notification";
	m_tInfos[m_ulReaderCount].ulCurrentState = 0;
	m_tInfos[m_ulReaderCount].ulEventState = 0;
	m_ulReaderCount++;
#endif
}

bool CReadersInfo::GetReaderStates(SCARD_READERSTATEA* txReaderStates,
																	 unsigned long length,
																	 unsigned long *ulnReaders)
{
	*ulnReaders = m_ulReaderCount;
	if(length < m_ulReaderCount)
		return false;
	// Convert from tReaderInfo[] -> SCARD_READERSTATE array
	for (DWORD i = 0; i < m_ulReaderCount; i++)
	{
		//copy the reader name, instead of giving the pointer, as C_GetSlotList might free it.
		txReaderStates[i].szReader = (char*)malloc(strlen(m_tInfos[i].csReader.c_str())+1);
		memset((void*)txReaderStates[i].szReader,0,strlen(m_tInfos[i].csReader.c_str())+1);
		memcpy((void*)txReaderStates[i].szReader,(const void*)(m_tInfos[i].csReader.c_str()),strlen(m_tInfos[i].csReader.c_str()));
		txReaderStates[i].dwCurrentState = m_tInfos[i].ulEventState;
	}
	return true;
}

bool CReadersInfo::UpdateReaderStates(SCARD_READERSTATEA* txReaderStates,
																			unsigned long ulnReaders)
{
	if(m_ulReaderCount != ulnReaders)
		return false;

	// Update the event states in m_tInfos
	for (DWORD i = 0; i < m_ulReaderCount; i++)
	{
			m_tInfos[i].ulCurrentState = m_tInfos[i].ulEventState;
			m_tInfos[i].ulEventState = txReaderStates[i].dwEventState & ~SCARD_STATE_CHANGED;
	}

	return true;
}

void CReadersInfo::FreeReaderStates(SCARD_READERSTATEA* txReaderStates,
																			unsigned long ulnReaders)
{
	// Free the memory allocated for the reader names
	for (DWORD i = 0; i < ulnReaders; i++)
	{
		if(txReaderStates[i].szReader != NULL)
		{
			free((void*)(txReaderStates[i].szReader));
			txReaderStates[i].szReader = NULL;
		}
	}

	return;
}

bool CReadersInfo::IsFirstTime(void)
{
	return bFirstTime;
}

void CReadersInfo::SetFirstTime(bool firstTime)
{
	bFirstTime = firstTime;
	return;
}
bool CReadersInfo::SameList(CReadersInfo* newReadersInfo)
{
	if(this->ReaderCount() != newReadersInfo->ReaderCount())
	{
		return false;
	}
	for (DWORD i = 0; i < m_ulReaderCount; i++)
	{
		if ( strcmp((this->ReaderName(i)).c_str(), (newReadersInfo->ReaderName(i)).c_str()) != 0)
		{
			return false;
		}
	}
	return true;
}


}//end namespace