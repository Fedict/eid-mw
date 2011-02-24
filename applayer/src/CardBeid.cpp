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
#include "CardBeid.h"
#include "TLVBuffer.h"
#include "Util.h"
#include "cryptoFwkBeid.h"
#include "CardBeidDef.h"
#include "CardFile.h"
#include "APLCertif.h"

namespace eIDMW
{

/*****************************************************************************************
---------------------------------------- APL_EidFile_ID -----------------------------------------
*****************************************************************************************/
APL_EidFile_ID::APL_EidFile_ID(APL_EIDCard *card):APL_CardFile(card,BEID_FILE_ID,NULL)
{
}

APL_EidFile_ID::~APL_EidFile_ID()
{
}

tCardFileStatus APL_EidFile_ID::VerifyFile()
{
	if(!m_card)
		return CARDFILESTATUS_ERROR;

	APL_EIDCard *pcard=dynamic_cast<APL_EIDCard *>(m_card);
	tCardFileStatus filestatus;

	APL_Certif *CertRrn = pcard->getRrn();
	
	if(!CertRrn)
		return CARDFILESTATUS_ERROR_RRN;

	//If the status of the RRN file is not OK, 
	//The rrn status is return
	filestatus=CertRrn->getFileStatus();
	if(filestatus!=CARDFILESTATUS_OK)
		return filestatus;

	//Get the status of the certificate RRN
	APL_CertifStatus rrnstatus=CertRrn->getStatus();
	if((rrnstatus==APL_CERTIF_STATUS_TEST || rrnstatus==APL_CERTIF_STATUS_ISSUER) && !pcard->getAllowTestCard())
		return CARDFILESTATUS_ERROR_TEST; 

	if(rrnstatus==APL_CERTIF_STATUS_DATE && !pcard->getAllowBadDate())
		return CARDFILESTATUS_ERROR_DATE; 

	//We test the oid of the RRN 
	//except for test card because, test card may have a bad oid
	if(!pcard->isTestCard())
		if(!m_cryptoFwk->VerifyOidRrn(CertRrn->getData()))
			return CARDFILESTATUS_ERROR_RRN;

	APL_EidFile_IDSign *sign=pcard->getFileIDSign();

	//If the status of the IDSign file is not OK, 
	//The IDSign status is return
	filestatus=sign->getStatus(true);
	if(filestatus!=CARDFILESTATUS_OK)
		return filestatus;

	if(!m_cryptoFwk->VerifySignatureSha1(m_data,sign->getData(),CertRrn->getData()))
		return CARDFILESTATUS_ERROR_SIGNATURE;

	//If this is not a test card, the rrn status must be OK, unless we return an error
	//For a test card, the status could be something else (for ex ISSUER)
	if(!pcard->isTestCard())
	{
		if(rrnstatus!=APL_CERTIF_STATUS_VALID 
			&& rrnstatus!=APL_CERTIF_STATUS_VALID_CRL
			&& rrnstatus!=APL_CERTIF_STATUS_VALID_OCSP)
			return CARDFILESTATUS_ERROR_CERT; 
	}

	return CARDFILESTATUS_OK;
}

void APL_EidFile_ID::EmptyFields()
{
	m_DocumentVersion.clear();
	m_LogicalNumber.clear();
	m_ChipNumber.clear();
	m_ValidityBeginDate.clear();
	m_ValidityEndDate.clear();
	m_IssuingMunicipality.clear();
	m_NationalNumber.clear();
	m_Surname.clear();
	m_FirstName1.clear();
	m_FirstName2.clear();
	m_Nationality.clear();
	m_LocationOfBirth.clear();
	m_DateOfBirth.clear();
	m_Gender.clear();
	m_Nobility.clear();
	m_DocumentType.clear();
	m_SpecialStatus.clear();
	m_PhotoHash.ClearContents();
}

bool APL_EidFile_ID::MapFields()
{
	char cBuffer[250];
	unsigned char ucBuffer[250];
	unsigned long ulLen=0;
	CTLVBuffer oTLVBuffer;
    oTLVBuffer.ParseTLV(m_data.GetBytes(), m_data.Size());

	//IDVersion
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillBinaryStringData(BEID_FIELD_TAG_ID_Version, cBuffer, &ulLen);
	m_DocumentVersion.assign(cBuffer);

	//CardNr
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(BEID_FIELD_TAG_ID_CardNr, cBuffer, &ulLen);
	m_LogicalNumber.assign(cBuffer, 0, ulLen);

	//ChipNr
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillBinaryStringData(BEID_FIELD_TAG_ID_ChipNr, cBuffer, &ulLen);
	m_ChipNumber.assign(cBuffer);

	//ValidityBeginDate
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(BEID_FIELD_TAG_ID_ValidityBeginDate, cBuffer, &ulLen);
	m_ValidityBeginDate.assign(cBuffer, 0, ulLen);

	//ValidityEndDate
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(BEID_FIELD_TAG_ID_ValidityEndDate, cBuffer, &ulLen);
	m_ValidityEndDate.assign(cBuffer, 0, ulLen);

	//IssuingMunicipality
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillUTF8Data(BEID_FIELD_TAG_ID_IssuingMunicipality, cBuffer, &ulLen);
	m_IssuingMunicipality.assign(cBuffer, 0, ulLen);

	//NationalNr
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(BEID_FIELD_TAG_ID_NationalNr, cBuffer, &ulLen);
	m_NationalNumber.assign(cBuffer, 0, ulLen);

	//Surname
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillUTF8Data(BEID_FIELD_TAG_ID_Surname, cBuffer, &ulLen);
	m_Surname.assign(cBuffer, 0, ulLen);

	//FirstName_1
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillUTF8Data(BEID_FIELD_TAG_ID_FirstName_1, cBuffer, &ulLen);
	m_FirstName1.assign(cBuffer, 0, ulLen);

	//FirstName_2
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillUTF8Data(BEID_FIELD_TAG_ID_FirstName_2, cBuffer, &ulLen);
	m_FirstName2.assign(cBuffer, 0, ulLen);

	//Nationality
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillUTF8Data(BEID_FIELD_TAG_ID_Nationality, cBuffer, &ulLen);
	m_Nationality.assign(cBuffer, 0, ulLen);

	//LocationOfBirth
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillUTF8Data(BEID_FIELD_TAG_ID_LocationOfBirth, cBuffer, &ulLen);
	m_LocationOfBirth.assign(cBuffer, 0, ulLen);

	//DateOfBirth
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillUTF8Data(BEID_FIELD_TAG_ID_DateOfBirth, cBuffer, &ulLen);
	m_DateOfBirth.assign(cBuffer, 0, ulLen);

	//Gender
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(BEID_FIELD_TAG_ID_Gender, cBuffer, &ulLen);
	m_Gender.assign(cBuffer, 0, ulLen);

	//Nobility
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillUTF8Data(BEID_FIELD_TAG_ID_Nobility, cBuffer, &ulLen);
	m_Nobility.assign(cBuffer, 0, ulLen);

	//DocumentType
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(BEID_FIELD_TAG_ID_DocumentType, cBuffer, &ulLen);
	m_DocumentType.assign(cBuffer, 0, ulLen);

	//SpecialStatus
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(BEID_FIELD_TAG_ID_SpecialStatus, cBuffer, &ulLen);
	m_SpecialStatus.assign(cBuffer, 0, ulLen);

	//PhotoHash
    ulLen = sizeof(cBuffer);
	memset(ucBuffer,0,ulLen);
	oTLVBuffer.FillBinaryData(BEID_FIELD_TAG_ID_PhotoHash, ucBuffer, &ulLen);
	m_PhotoHash.ClearContents();
	m_PhotoHash.Append(ucBuffer,ulLen);

	//Duplicata
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(BEID_FIELD_TAG_ID_Duplicata, cBuffer, &ulLen);
	m_Duplicata.assign(cBuffer, 0, ulLen);

	//SpecialOrganization
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(BEID_FIELD_TAG_ID_SpecialOrganization, cBuffer, &ulLen);
	m_SpecialOrganization.assign(cBuffer, 0, ulLen);

	//MemberOfFamily
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillUTF8Data(BEID_FIELD_TAG_ID_MemberOfFamily, cBuffer, &ulLen);
	m_MemberOfFamily.assign(cBuffer, 0, ulLen);

	return true;
}

bool APL_EidFile_ID::ShowData()
{
	APL_EIDCard *pcard=dynamic_cast<APL_EIDCard *>(m_card);

	bool bAllowTest=pcard->getAllowTestCard();
	bool bAllowBadDate=pcard->getAllowBadDate();

	tCardFileStatus status=getStatus(true,&bAllowTest,&bAllowBadDate);
	if(status==CARDFILESTATUS_OK)
		return true;

	//If the autorisation changed, we read the card again
	if((status==CARDFILESTATUS_ERROR_TEST && pcard->getAllowTestCard())
		|| (status==CARDFILESTATUS_ERROR_DATE && pcard->getAllowBadDate()))

		status=LoadData(true);

	if(status==CARDFILESTATUS_OK)
		return true;

	return false;
}

const char *APL_EidFile_ID::getDocumentVersion()
{
	if(ShowData())
		return m_DocumentVersion.c_str();

	return "";
}

const char *APL_EidFile_ID::getDocumentType()
{
	if(ShowData())
		return m_DocumentType.c_str();

	return "";
}

const char *APL_EidFile_ID::getFirstName1()
{
	if(ShowData())
		return m_FirstName1.c_str();

	return "";
}

const char *APL_EidFile_ID::getFirstName2()
{
	if(ShowData())
		return m_FirstName2.c_str();

	return "";
}

const char *APL_EidFile_ID::getSurname()
{
	if(ShowData())
		return m_Surname.c_str();

	return "";
}

const char *APL_EidFile_ID::getGender()
{
	if(ShowData())
		return m_Gender.c_str();

	return "";
}

const char *APL_EidFile_ID::getDateOfBirth()
{
	if(ShowData())
		return m_DateOfBirth.c_str();

	return "";
}

const char *APL_EidFile_ID::getLocationOfBirth()
{
	if(ShowData())
		return m_LocationOfBirth.c_str();

	return "";
}

const char *APL_EidFile_ID::getNobility()
{
	if(ShowData())
		return m_Nobility.c_str();

	return "";
}

const char *APL_EidFile_ID::getNationality()
{
	if(ShowData())
		return m_Nationality.c_str();

	return "";
}

const char *APL_EidFile_ID::getNationalNumber()
{
	if(ShowData())
		return m_NationalNumber.c_str();

	return "";
}

const char *APL_EidFile_ID::getDuplicata()
{
	if(ShowData())
		return m_Duplicata.c_str();

	return "";
}

const char *APL_EidFile_ID::getSpecialOrganization()
{
	if(ShowData())
		return m_SpecialOrganization.c_str();

	return "";
}

const char *APL_EidFile_ID::getMemberOfFamily()
{
	if(ShowData())
		return m_MemberOfFamily.c_str();

	return "";
}

const char *APL_EidFile_ID::getLogicalNumber()
{
	if(ShowData())
		return m_LogicalNumber.c_str();

	return "";
}

const char *APL_EidFile_ID::getChipNumber()
{
	if(ShowData())
		return m_ChipNumber.c_str();

	return "";
}

const char *APL_EidFile_ID::getValidityBeginDate()
{
	if(ShowData())
		return m_ValidityBeginDate.c_str();

	return "";
}

const char *APL_EidFile_ID::getValidityEndDate()
{
	if(ShowData())
		return m_ValidityEndDate.c_str();

	return "";
}

const char *APL_EidFile_ID::getIssuingMunicipality()
{
	if(ShowData())
		return m_IssuingMunicipality.c_str();

	return "";
}

const char *APL_EidFile_ID::getSpecialStatus()
{
	if(ShowData())
		return m_SpecialStatus.c_str();

	return "";
}

const CByteArray& APL_EidFile_ID::getPhotoHash()
{
	if(ShowData())
		return m_PhotoHash;

	return EmptyByteArray;
}
/*****************************************************************************************
---------------------------------------- APL_EidFile_IDSign -----------------------------------------
*****************************************************************************************/
APL_EidFile_IDSign::APL_EidFile_IDSign(APL_EIDCard *card):APL_CardFile(card,BEID_FILE_ID_SIGN,NULL)
{
}

APL_EidFile_IDSign::~APL_EidFile_IDSign()
{
}

/**
  * No verification here.
  * This file is needed for other verifications. 
  * If it is corrupted, these verifications will failed.
  */
tCardFileStatus APL_EidFile_IDSign::VerifyFile()
{
	return CARDFILESTATUS_OK;
}


/*****************************************************************************************
---------------------------------------- APL_EidFile_Address -----------------------------------------
*****************************************************************************************/
APL_EidFile_Address::APL_EidFile_Address(APL_EIDCard *card):APL_CardFile(card,BEID_FILE_ADDRESS,NULL)
{
}

APL_EidFile_Address::~APL_EidFile_Address()
{
}

tCardFileStatus APL_EidFile_Address::VerifyFile()
{
	if(!m_card)
		return CARDFILESTATUS_ERROR;
	
	APL_EIDCard *pcard=dynamic_cast<APL_EIDCard *>(m_card);
	tCardFileStatus filestatus;

	APL_Certif *CertRrn=pcard->getRrn();

	if(!CertRrn)
		return CARDFILESTATUS_ERROR_RRN;

	//If the status of the RRN file is not OK, 
	//The rrn status is return
	filestatus=CertRrn->getFileStatus();
	if(filestatus!=CARDFILESTATUS_OK)
		return filestatus;

	//Get the status of the certificate RRN
	APL_CertifStatus rrnstatus=CertRrn->getStatus();
	if((rrnstatus==APL_CERTIF_STATUS_TEST || rrnstatus==APL_CERTIF_STATUS_ISSUER) && !pcard->getAllowTestCard())
		return CARDFILESTATUS_ERROR_TEST; 

	if(rrnstatus==APL_CERTIF_STATUS_DATE && !pcard->getAllowBadDate())
		return CARDFILESTATUS_ERROR_DATE; 

	//We test the oid of the RRN 
	//except for test card because, test card may have a bad oid
	if(!pcard->isTestCard())
		if(!m_cryptoFwk->VerifyOidRrn(CertRrn->getData()))
			return CARDFILESTATUS_ERROR_RRN;

	APL_EidFile_AddressSign *sign=pcard->getFileAddressSign();

	//If the status of the IDSign file is not OK, 
	//The IDSign status is return
	filestatus=sign->getStatus(true);
	if(filestatus!=CARDFILESTATUS_OK)
		return filestatus;

	APL_EidFile_IDSign *idsign=pcard->getFileIDSign();

	CByteArray dataToSign;					//To check the signature,
	dataToSign.Append(m_data);				//we need to remove the null byte
	dataToSign.TrimRight();					//at the end of the address file
	dataToSign.Append(idsign->getData());	//then, we add the signature of the id file

	if(!m_cryptoFwk->VerifySignatureSha1(dataToSign,sign->getData(),CertRrn->getData()))
		return CARDFILESTATUS_ERROR_SIGNATURE;

	//If this is not a test card, the rrn status must be OK, unless we return an error
	//For a test card, the status could be something else (for ex ISSUER)
	if(!pcard->isTestCard())
	{
		if(rrnstatus!=APL_CERTIF_STATUS_VALID 
			&& rrnstatus!=APL_CERTIF_STATUS_VALID_CRL
			&& rrnstatus!=APL_CERTIF_STATUS_VALID_OCSP)
			return CARDFILESTATUS_ERROR_CERT; 
	}

	return CARDFILESTATUS_OK;
}

bool APL_EidFile_Address::ShowData()
{
	APL_EIDCard *pcard=dynamic_cast<APL_EIDCard *>(m_card);

	bool bAllowTest=pcard->getAllowTestCard();
	bool bAllowBadDate=pcard->getAllowBadDate();

	tCardFileStatus status=getStatus(true,&bAllowTest,&bAllowBadDate);
	if(status==CARDFILESTATUS_OK)
		return true;

	//If the autorisation changed, we read the card again
	if((status==CARDFILESTATUS_ERROR_TEST && pcard->getAllowTestCard())
		|| (status==CARDFILESTATUS_ERROR_DATE && pcard->getAllowBadDate()))

		status=LoadData(true);

	if(status==CARDFILESTATUS_OK)
		return true;

	return false;
}

void APL_EidFile_Address::EmptyFields()
{
	m_AddressVersion.clear();
	m_Street.clear();
	m_ZipCode.clear();
	m_Municipality.clear();
	m_Country.clear();
}

bool APL_EidFile_Address::MapFields()
{
	char cBuffer[250];
	unsigned long ulLen=0;

	CTLVBuffer oTLVBuffer;
    oTLVBuffer.ParseTLV(m_data.GetBytes(), m_data.Size());

	//AddressVersion
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillBinaryStringData(BEID_FIELD_TAG_ADDR_Version, cBuffer, &ulLen);
	m_AddressVersion.assign(cBuffer);

	//Street
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillUTF8Data(BEID_FIELD_TAG_ADDR_Street, cBuffer, &ulLen);
	m_Street.assign(cBuffer, 0, ulLen);

	//ZipCode
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(BEID_FIELD_TAG_ADDR_ZipCode, cBuffer, &ulLen);
	m_ZipCode.assign(cBuffer, 0, ulLen);

	//Municipality
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillUTF8Data(BEID_FIELD_TAG_ADDR_Municipality, cBuffer, &ulLen);
	m_Municipality.assign(cBuffer, 0, ulLen);

	//Country
	m_Country.assign("be");

	return true;
}

const char *APL_EidFile_Address::getAddressVersion()
{
	if(ShowData())
		return m_AddressVersion.c_str();

	return "";
}

const char *APL_EidFile_Address::getStreet()
{
	if(ShowData())
		return m_Street.c_str();

	return "";
}

const char *APL_EidFile_Address::getZipCode()
{
	if(ShowData())
		return m_ZipCode.c_str();

	return "";
}

const char *APL_EidFile_Address::getMunicipality()
{
	if(ShowData())
		return m_Municipality.c_str();

	return "";
}

const char *APL_EidFile_Address::getCountry()
{
	if(ShowData())
		return m_Country.c_str();

	return "";
}

/*****************************************************************************************
---------------------------------------- APL_EidFile_AddressSign -----------------------------------------
*****************************************************************************************/
APL_EidFile_AddressSign::APL_EidFile_AddressSign(APL_EIDCard *card):APL_CardFile(card,BEID_FILE_ADDRESS_SIGN,NULL)
{
}

APL_EidFile_AddressSign::~APL_EidFile_AddressSign()
{
}

/**
  * No verification here.
  * This file is needed for other verifications. 
  * If it is corrupted, these verifications will failed.
  */
tCardFileStatus APL_EidFile_AddressSign::VerifyFile()
{
	return CARDFILESTATUS_OK;
}

/*****************************************************************************************
---------------------------------------- APL_EidFile_Photo -----------------------------------------
*****************************************************************************************/
APL_EidFile_Photo::APL_EidFile_Photo(APL_EIDCard *card):APL_CardFile(card,BEID_FILE_PHOTO,NULL)
{
}

APL_EidFile_Photo::~APL_EidFile_Photo()
{
}

tCardFileStatus APL_EidFile_Photo::VerifyFile()
{
	if(!m_card)
		return CARDFILESTATUS_ERROR;

	APL_EIDCard *pcard=dynamic_cast<APL_EIDCard *>(m_card);

	//The hash for the photo is in the ID file
	const CByteArray &hash=pcard->getFileID()->getPhotoHash();

	//If the status of the ID file is not OK, the hash is not valid.
	//The id status is return
	tCardFileStatus idstatus=pcard->getFileID()->getStatus();
	if(idstatus!=CARDFILESTATUS_OK)
		return idstatus;

	//We check if the hash correspond to the photo
	if(!m_cryptoFwk->VerifyHashSha1(m_data,hash))
		return CARDFILESTATUS_ERROR_HASH;

	return CARDFILESTATUS_OK;
}

/*****************************************************************************************
---------------------------------------- APL_EidFile_TokenInfo -----------------------------------------
*****************************************************************************************/
APL_EidFile_TokenInfo::APL_EidFile_TokenInfo(APL_EIDCard *card):APL_CardFile(card,BEID_FILE_TOKENINFO,NULL)
{
}

APL_EidFile_TokenInfo::~APL_EidFile_TokenInfo()
{
}

tCardFileStatus APL_EidFile_TokenInfo::VerifyFile()
{
	return CARDFILESTATUS_OK;
}

void APL_EidFile_TokenInfo::EmptyFields()
{
	m_GraphicalPersonalisation.clear();
	m_ElectricalPersonalisation.clear();
	m_ElectricalPersonalisationInterface.clear();
}

bool APL_EidFile_TokenInfo::MapFields()
{
	char buffer[50];

	//Graphical Personalisation
	sprintf_s(buffer,sizeof(buffer),"%02X", m_data.GetByte(BEID_FIELD_BYTE_TOKENINFO_GraphicalPersonalisation));
	m_GraphicalPersonalisation=buffer;

	//Electrical Personalisation
	sprintf_s(buffer,sizeof(buffer),"%02X", m_data.GetByte(BEID_FIELD_BYTE_TOKENINFO_ElectricalPersonalisation));
	m_ElectricalPersonalisation=buffer;

	//Electrical Personalisation Interface
	sprintf_s(buffer,sizeof(buffer),"%02X", m_data.GetByte(BEID_FIELD_BYTE_TOKENINFO_ElectricalPersonalisationInterface));
	m_ElectricalPersonalisationInterface=buffer;

	return true;

}

const char *APL_EidFile_TokenInfo::getGraphicalPersonalisation()
{
	if(ShowData())
		return m_GraphicalPersonalisation.c_str();

	return "";
}

const char *APL_EidFile_TokenInfo::getElectricalPersonalisation()
{
	if(ShowData())
		return m_ElectricalPersonalisation.c_str();

	return "";
}

const char *APL_EidFile_TokenInfo::getElectricalPersonalisationInterface()
{
	if(ShowData())
		return m_ElectricalPersonalisationInterface.c_str();

	return "";
}

/*****************************************************************************************
---------------------------------------- APL_EidFile_RRN -----------------------------------------
*****************************************************************************************/
APL_EidFile_RRN::APL_EidFile_RRN(APL_EIDCard *card):APL_CardFile_Certificate(card,BEID_FILE_RRN,NULL)
{
}

APL_EidFile_RRN::~APL_EidFile_RRN()
{
}

tCardFileStatus APL_EidFile_RRN::VerifyFile()
{
	APL_EIDCard *pcard=dynamic_cast<APL_EIDCard *>(m_card);
	tCardFileStatus tempstatus;

	//We try to find the issuer in the existing store (should be the root)
	APL_Certif *issuer=pcard->getCertificates()->findIssuer(&m_data);

	if(issuer==NULL) //If rrn is not issued by the root
	{
		//We check if RRNCA exist on the card
		APL_EidFile_RRNCA *rrnca=pcard->getFileRRNCA();
		tempstatus=rrnca->getStatus(true);

		//If the file doesn't exist
		if(tempstatus==CARDFILESTATUS_ERROR_NOFILE)
		{
			const unsigned char *const *pissuer;
			int i=0;

			//we look in the hard coded rrnca to find the issuer
			for(pissuer=_beid_rrnca_certs;*pissuer!=NULL;pissuer++)
			{
				CByteArray issuer_data(*pissuer,_beid_rrnca_certs_size[i++]);

				if(m_cryptoFwk->isIssuer(m_data,issuer_data))
				{
					pcard->setFileRRNCA(&issuer_data);
					return CARDFILESTATUS_OK;
				}
			}

			//No issuer found => Probaly a test card
			//The content of the file is ok, the validation of the file will be done later
			return CARDFILESTATUS_OK; 
		}
		else
		{
			//The rrn ca status is return
			return tempstatus;
		}
	}
	else
	{
		//Make RRNCA fit to the issuer
		pcard->setFileRRNCA(&issuer->getData());
	}

	return CARDFILESTATUS_OK;
}

/*****************************************************************************************
---------------------------------------- APL_EidFile_RRNCA -----------------------------------------
*****************************************************************************************/
APL_EidFile_RRNCA::APL_EidFile_RRNCA(APL_EIDCard *card,const CByteArray *data):APL_CardFile_Certificate(card,BEID_FILE_RRNCA,data)
{
}

APL_EidFile_RRNCA::~APL_EidFile_RRNCA()
{
}

/**
  * No verification here.
  * This file is needed for other verifications. 
  * If it is corrupted, these verifications will failed.
  */
tCardFileStatus APL_EidFile_RRNCA::VerifyFile()
{
	return CARDFILESTATUS_OK;
}


}
