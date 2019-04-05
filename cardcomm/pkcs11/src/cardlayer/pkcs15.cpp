
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
#include "pkcs15.h"
#include "pkcs15parser.h"
#include "card.h"

namespace eIDMW
{

	const static tCert CertInvalid = { false, "", 0, 0, 0, 0, false, false, "" };
	const static tPrivKey PrivKeyInvalid = { false, "", 0, 0, 0, 0, 0, 0, 0, "", 0, false };

	// Hardcoded Beid V1 PINs, keys, certs
	bool            bValid;
	std::string     csLabel;
	bool            bFlags[2];
	unsigned long   ulAuthID; // optional
	unsigned long   ulID;
	bool            bPinFlags[6];
	unsigned long   ulPinType;
	unsigned long   ulMinLen;
	unsigned long   ulStoredLen;
	unsigned long   ulMaxLen;  // optional ?
	unsigned char   ulPinRef;  // optional
	unsigned char   ucPadChar;
	//tPinEncoding    encoding;
	//std::string     csPath;

	const static tPrivKey KeyAuthBeidV1 = { true, "Authentication", 0,2,0,1,0,0,0x82,"3F00DF00", 128,true };
	const static tPrivKey KeySignBeidV1 = { true, "Signature", 0,3,0,1,0,0,0x83,"3F00DF00", 128,true };
	const static tPrivKey KeyAuthBeidV2 = { true, "Authentication", 0,2,0,2,0,0,0x8A,"3F00DF00", 128,true };
	const static tPrivKey KeySignBeidV2 = { true, "Signature", 0,3,0,2,0,0,0x89,"3F00DF00", 128,true };

	const std::string defaultEFTokenInfo = "3F00DF005032";

	const std::string AODFPath = "3F00DF005034";
	const std::string PrKDFPath = "3F00DF005035";
	const std::string CDFPath = "3F00DF005037";

	CPKCS15::CPKCS15(void) :m_poParser(NULL)
	{
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
		m_oPrKeys.clear();

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
		if (!m_xPrKDF.isRead)  ReadLevel3(PRKDF);
		return (unsigned long)m_oPrKeys.size();
	}

	tPrivKey CPKCS15::GetPrivKey(unsigned long ulIndex)
	{
		if (!m_xPrKDF.isRead)  ReadLevel3(PRKDF);
		if (ulIndex >= m_oPrKeys.size())
			throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);
		return m_oPrKeys.at(ulIndex);
	}

	tPrivKey CPKCS15::GetPrivKeyByID(unsigned long ulID)
	{
		if (!m_xPrKDF.isRead)  ReadLevel3(PRKDF);

		for (std::vector<tPrivKey>::const_iterator ik = m_oPrKeys.begin();
			ik != m_oPrKeys.end(); ++ik) {
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
		case PRKDF:
			ReadFile(&m_xPrKDF, 2);
			// parse
			m_oPrKeys = m_poParser->ParsePrkdf(m_xPrKDF.byteArray);
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
