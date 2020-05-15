
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
#include <iostream>
#include <map>
#include <array>
#include "pkcs15.h"
#include "pkcs15parser.h"
#include "card.h"

namespace eIDMW
{

	const static tCert CertInvalid = { false, "", 0, 0, 0, 0, false, false, "" };

	const static tPrivKey KeyAuthBeidV11 = { true, "Authentication", 0, 2, 4,   130, 128, RSA };
	const static tPrivKey KeySignBeidV11 = { true, "Signature",      1, 3, 512, 131, 128, RSA };
	const static tPrivKey KeyAuthBeidV17 = { true, "Authentication", 0, 2, 4,   130, 256, RSA };
	const static tPrivKey KeySignBeidV17 = { true, "Signature",      1, 3, 512, 131, 256, RSA };
	const static tPrivKey KeyAuthBeidV18 = { true, "Authentication", 0, 2, 4,   130, 48,  EC };
	const static tPrivKey KeySignBeidV18 = { true, "Signature",      1, 3, 512, 131, 48,  EC };
	const static tPrivKey PrivKeyInvalid = { false, "", 0, 0, 0, 0, 0, RSA };
	std::map<unsigned char, std::array<tPrivKey, 2> > keymap;

	const std::string defaultEFTokenInfo = "3F00DF005032";

	const std::string AODFPath = "3F00DF005034";
	const std::string PrKDFPath = "3F00DF005035";
	const std::string CDFPath = "3F00DF005037";

	CPKCS15::CPKCS15(void) :m_poParser(NULL), m_poCard(NULL)
	{
		if(keymap.empty()) {
			std::array<tPrivKey, 2> v11;
			v11[0] = KeyAuthBeidV11;
			v11[1] = KeySignBeidV11;
			keymap[0x01] = v11;
			std::array<tPrivKey, 2> v17;
			v17[0] = KeyAuthBeidV17;
			v17[1] = KeySignBeidV17;
			keymap[0x17] = v17;
			std::array<tPrivKey, 2> v18;
			v18[0] = KeyAuthBeidV18;
			v18[1] = KeySignBeidV18;
			keymap[0x18] = v18;
		}
		Clear();
	}

	CPKCS15::~CPKCS15(void)
	{
	}

	void CPKCS15::Clear(CCard *poCard)
	{
		m_poCard = poCard;

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
	}

	void CPKCS15::SetCard(CCard *poCard)
	{
		m_poCard = poCard;
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
		return keymap[m_poCard->GetAppletVersion()].size();
	}

	tPrivKey CPKCS15::GetPrivKey(unsigned long ulIndex)
	{
		if(keymap.find(m_poCard->GetAppletVersion()) != keymap.end()) {
			return keymap[m_poCard->GetAppletVersion()][ulIndex];
		}
		return PrivKeyInvalid;
	}

	tPrivKey CPKCS15::GetPrivKeyByID(unsigned long ulID)
	{
		unsigned char vers = m_poCard->GetAppletVersion();
		for (std::array<tPrivKey, 2>::const_iterator ik = keymap[vers].begin();
			ik != keymap[vers].end(); ++ik) {
			if (ik->ulID == ulID) return *ik;
		}
		return PrivKeyInvalid;
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
