
/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2010 FedICT.
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

#include "pkcs15.h"
#include "pkcs15parser.h"
#include "card.h"

namespace eIDMW
{

	const static tCert CertInvalid = { false, "", 0, 0, 0, 0, false, false, "" };
	const static tPrivKey PrivKeyInvalid = { false, "", 0, 0, 0, 0, 0, RSA };
	const unsigned long CARD_KEY_ID = 1;

	static tPrivKey KeysBeidV11[2] = {	{true, "Authentication", 0, 2, 4,   130, 128, RSA},
										{true, "Signature",      1, 3, 512, 131, 128, RSA} };
	static tPrivKey KeysBeidV17[2] = {	{true, "Authentication", 0, 2, 4,   130, 256, RSA},
										{true, "Signature",      1, 3, 512, 131, 256, RSA} };
	static tPrivKey KeysBeidV18[3] = {	{true, "Authentication", 0, 2, 4,   130, 48,  EC},
										{true, "Signature",      1, 3, 512, 131, 48,  EC},
										{true, "Card",   0, CARD_KEY_ID, 2, 129, 48,  EC} };
	const std::string defaultEFTokenInfo = "3F00DF005032";

	const std::string AODFPath = "3F00DF005034";
	const std::string PrKDFPath = "3F00DF005035";
	const std::string CDFPath = "3F00DF005037";

	CPKCS15::CPKCS15(void) :m_poParser(NULL), m_poCard(NULL)
	{
		Clear();
	}

	CPKCS15::~CPKCS15(void)
	{
	}

	void CPKCS15::Clear(CCard *poCard)
	{
		SetCard(poCard);

		// clear everything
		//m_csSerial = "";
		//m_csLabel = "";

		m_oPins.clear();
		m_oCertificates.clear();

		m_xPin.setDefault();
		m_xPrKey.setDefault();
		m_xCert.setDefault();

		m_xAODF.setDefault(AODFPath);
		m_xCDF.setDefault(CDFPath);
		m_xPrKDF.setDefault(PrKDFPath);

		m_poKeysBeid = NULL;
		m_ulKeyCount = 0;
	}

	void CPKCS15::SetCard(CCard *poCard)
	{
		m_poCard = poCard;
		SetKeys();
	}

	void CPKCS15::SetKeys(void)
	{
		if (m_poCard != NULL)
		{
			unsigned char ucAppletVersion = m_poCard->GetAppletVersion();
			switch (ucAppletVersion)
			{
			case 0x11:
				m_poKeysBeid = KeysBeidV11;
				m_ulKeyCount = 2;
				break;
			case 0x17:
				m_poKeysBeid = KeysBeidV17;
				m_ulKeyCount = 2;
				break;
			case 0x18:
				m_poKeysBeid = KeysBeidV18;
				m_ulKeyCount = 3;
				break;
			default:
				m_poKeysBeid = NULL;
				m_ulKeyCount = 0;
				break;
			}
		}
	}

	//serial number is already retrieved by Get Card Data in the CCard constructor: m_oCardData = SendAPDU(0x80, 0xE4, 0x00, 0x00, 0x1C);
/*	std::string CPKCS15::GetSerialNr()
	{
		if (m_csSerial == "")
		{
#ifdef VERBOSE
			std::cerr << "CPKCS15::GetSerialNr" << std::endl;
#endif
			if (!m_xTokenInfo.isRead) ReadLevel2(TOKENINFO);
		}

		return m_csSerial;
	}*/

	//this is the applet label "BELPIC", hardcoded now
/*	std::string CPKCS15::GetCardLabel()
	{
		// check if we know it already
		if (m_csLabel == "") {
			if (!m_xTokenInfo.isRead) ReadLevel2(TOKENINFO);
		}
		return m_csLabel;
	}
*/

	unsigned long CPKCS15::CertCount()
	{
		if (!m_xCDF.isRead) ReadLevel3(CDF);
		return (unsigned long)m_oCertificates.size();
	}

	tCert CPKCS15::GetCert(unsigned long ulIndex)
	{
		if (!m_xCDF.isRead) ReadLevel3(CDF);
		if (ulIndex >= m_oCertificates.size())
			throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);
		return m_oCertificates.at(ulIndex);
	}

	tCert CPKCS15::GetCertByID(unsigned long ulID)
	{
		if (!m_xCDF.isRead) ReadLevel3(CDF);
		for (std::vector<tCert>::const_iterator ic = m_oCertificates.begin();
			ic != m_oCertificates.end(); ++ic) {
			if (ic->ulID == ulID) return *ic;
		}
		return CertInvalid;
	}

	unsigned long CPKCS15::PrivKeyCount()
	{
		return m_ulKeyCount;
	}

	tPrivKey CPKCS15::GetPrivKey(unsigned long ulIndex)
	{
		if ( (m_poKeysBeid != NULL) && (ulIndex <= m_ulKeyCount) )
		{
			return m_poKeysBeid[ulIndex];
		}
		return PrivKeyInvalid;
	}

	tPrivKey CPKCS15::GetPrivKeyByID(unsigned long ulID)
	{
		if (m_poKeysBeid != NULL)
		{
			for (unsigned long ulCounter = 0; ulCounter <= m_ulKeyCount; ulCounter++)
			{
				if (m_poKeysBeid[ulCounter].ulID == ulID)
				{
					return m_poKeysBeid[ulCounter];
				}
			}
		}
		return PrivKeyInvalid;
	}

	unsigned long CPKCS15::GetCardKeyID()
	{
		return CARD_KEY_ID;
	}

	void CPKCS15::ReadLevel3(tPKCSFileName name) {

		switch (name) {
		case CDF:
			ReadFile(&m_xCDF, 2);
			// parse 
			m_oCertificates = m_poParser->ParseCdf(m_xCDF.byteArray);
			break;
		default:
			// error: this method can only be called with CDF or PRKDF
			return;
		}
	}

	void CPKCS15::ReadFile(tPKCSFile* pFile, int upperLevel) {
		if (pFile->path == "") {
			//path should be set
			return;
		}
		pFile->byteArray = m_poCard->ReadFile(pFile->path);
		pFile->isRead = true;
	}

}
