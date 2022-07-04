
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
#if !defined(__BEIDLABELS__)
#define __BEIDLABELS__

/* This file contains the label names that are defined for the LABEL attributes 
 * of the pkcs#11 objects that can get returned by this pkcs#11 SDK
 * These label names do not reside on the card 
 */

	//      { BEID_FIELD_TAG_ID_Version,                    "Version"},
#define BEID_ID_DATA_LABELS  {  \
	{ BEID_FIELD_TAG_ID_CardNr,				"card_number"},  \
	{ BEID_FIELD_TAG_ID_ChipNr,				"chip_number"},  \
	{ BEID_FIELD_TAG_ID_ValidityBeginDate,	"validity_begin_date"},  \
	{ BEID_FIELD_TAG_ID_ValidityEndDate,	"validity_end_date"},  \
	{ BEID_FIELD_TAG_ID_IssuingMunicipality,"issuing_municipality"},  \
	{ BEID_FIELD_TAG_ID_NationalNr,			"national_number"},  \
	{ BEID_FIELD_TAG_ID_Surname,			"surname"},  \
	{ BEID_FIELD_TAG_ID_FirstNames,			"firstnames"},  \
	{ BEID_FIELD_TAG_ID_FirstLetterOfThirdGivenName,		"first_letter_of_third_given_name"},  \
	{ BEID_FIELD_TAG_ID_Nationality,		"nationality"},  \
	{ BEID_FIELD_TAG_ID_LocationOfBirth,	"location_of_birth"},  \
	{ BEID_FIELD_TAG_ID_DateOfBirth,		"date_of_birth"},  \
	{ BEID_FIELD_TAG_ID_Gender,				"gender"},  \
	{ BEID_FIELD_TAG_ID_Nobility,			"nobility"},  \
	{ BEID_FIELD_TAG_ID_DocumentType,		"document_type"},  \
	{ BEID_FIELD_TAG_ID_SpecialStatus,		"special_status"},  \
	{ BEID_FIELD_TAG_ID_PhotoHash,			"photo_hash"},  \
	{ BEID_FIELD_TAG_ID_Duplicata,			"duplicata"},  \
	{ BEID_FIELD_TAG_ID_SpecialOrganization,"special_organization"},  \
	{ BEID_FIELD_TAG_ID_MemberOfFamily,		"member_of_family"}, \
	{ BEID_FIELD_TAG_ID_DateAndCountryOfProtection,		"date_and_country_of_protection"},  \
	{ BEID_FIELD_TAG_ID_WorkPermitType,		"work_permit_mention"}, \
	{ BEID_FIELD_TAG_ID_Vat1,				"employer_vat_1"}, \
	{ BEID_FIELD_TAG_ID_Vat2,				"employer_vat_2"}, \
	{ BEID_FIELD_TAG_ID_RegionalFileNumber, "regional_file_number"}, \
	{ BEID_FIELD_TAG_ID_BasicKeyHash,		"basic_key_hash"}, \
	{ BEID_FIELD_TAG_ID_BrexitMention1,		"brexit_mention_1"}, \
	{ BEID_FIELD_TAG_ID_BrexitMention2,		"brexit_mention_2"}, \
	{ BEID_FIELD_TAG_ID_CardAMention1,		"cardA_mention_1"}, \
	{ BEID_FIELD_TAG_ID_CardAMention2,		"cardA_mention_2"} \
}

	//{ BEID_FIELD_TAG_ADDR_Version,                        "Address_Version"},
#define BEID_ADDRESS_DATA_LABELS  {  \
	{ BEID_FIELD_TAG_ADDR_Street,			"address_street_and_number"},  \
	{ BEID_FIELD_TAG_ADDR_ZipCode,			"address_zip"},  \
	{ BEID_FIELD_TAG_ADDR_Municipality,		"address_municipality"}  \
}

#define BEID_ID_RECORD_DATA_LABELS  {  \
	{ BEID_FIELD_TAG_ID_CardNr,							"record_card_number"},  \
	{ BEID_FIELD_TAG_ID_ChipNr,							"record_chip_number"},  \
	{ BEID_FIELD_TAG_ID_ValidityBeginDate,				"record_validity_begin_date"},  \
	{ BEID_FIELD_TAG_ID_ValidityEndDate,				"record_validity_end_date"},  \
	{ BEID_FIELD_TAG_ID_IssuingMunicipality,			"record_issuing_municipality"},  \
	{ BEID_FIELD_TAG_ID_NationalNr,						"record_national_number"},  \
	{ BEID_FIELD_TAG_ID_Surname,						"record_surname"},  \
	{ BEID_FIELD_TAG_ID_FirstNames,						"record_firstnames"},  \
	{ BEID_FIELD_TAG_ID_FirstLetterOfThirdGivenName,	"record_first_letter_of_third_given_name"},  \
	{ BEID_FIELD_TAG_ID_Nationality,					"record_nationality"},  \
	{ BEID_FIELD_TAG_ID_LocationOfBirth,				"record_location_of_birth"},  \
	{ BEID_FIELD_TAG_ID_DateOfBirth,					"record_date_of_birth"},  \
	{ BEID_FIELD_TAG_ID_Gender,							"record_gender"},  \
	{ BEID_FIELD_TAG_ID_Nobility,						"record_nobility"},  \
	{ BEID_FIELD_TAG_ID_DocumentType,					"record_document_type"},  \
	{ BEID_FIELD_TAG_ID_SpecialStatus,					"record_special_status"},  \
	{ BEID_FIELD_TAG_ID_PhotoHash,						"record_photo_hash"},  \
	{ BEID_FIELD_TAG_ID_Duplicata,						"record_duplicata"},  \
	{ BEID_FIELD_TAG_ID_SpecialOrganization,			"record_special_organization"},  \
	{ BEID_FIELD_TAG_ID_MemberOfFamily,					"record_member_of_family"}, \
	{ BEID_FIELD_TAG_ID_DateAndCountryOfProtection,		"record_date_and_country_of_protection"},  \
	{ BEID_FIELD_TAG_ID_WorkPermitType,					"record_work_permit_mention"}, \
	{ BEID_FIELD_TAG_ID_Vat1,							"record_employer_vat_1"}, \
	{ BEID_FIELD_TAG_ID_Vat2,							"record_employer_vat_2"}, \
	{ BEID_FIELD_TAG_ID_RegionalFileNumber,				"record_regional_file_number"}, \
	{ BEID_FIELD_TAG_ID_BasicKeyHash,					"record_basic_key_hash"}, \
	{ BEID_FIELD_TAG_ID_BrexitMention1,					"record_brexit_mention_1"}, \
	{ BEID_FIELD_TAG_ID_BrexitMention2,					"record_brexit_mention_2"} \
}
//	{ BEID_FIELD_TAG_ID_CardAMention1,					"record_cardA_mention_1"}, \
//	{ BEID_FIELD_TAG_ID_CardAMention2,					"record_cardA_mention_2"} \

	//{ BEID_FIELD_TAG_ADDR_Version,							"Address_Version"},
#define BEID_ADDRESS_RECORD_DATA_LABELS  {  \
	{ BEID_FIELD_TAG_ADDR_Street,						"record_address_street_and_number"},  \
	{ BEID_FIELD_TAG_ADDR_ZipCode,						"record_address_zip"},  \
	{ BEID_FIELD_TAG_ADDR_Municipality,					"record_address_municipality"}  \
}

#define BEID_TOKENINFO_LABELS  {  \
	{ BEID_FIELD_TAG_TOKINF_graph,						"tokeninfo_graph_perso_version"},  \
	{ BEID_FIELD_TAG_TOKINF_elec,						"tokeninfo_elec_perso_version"},  \
	{ BEID_FIELD_TAG_TOKINF_elecint,					"tokeninfo_elec_perso_int_version"}  \
}

#define BEID_LABEL_DATA_SerialNr				"carddata_serialnumber"
#define BEID_LABEL_DATA_CompCode				"carddata_comp_code"
#define BEID_LABEL_DATA_OSNr					"carddata_os_number"
#define BEID_LABEL_DATA_OSVersion				"carddata_os_version"
#define BEID_LABEL_DATA_SoftMaskNumber			"carddata_soft_mask_number"
#define BEID_LABEL_DATA_SoftMaskVersion			"carddata_soft_mask_version"
#define BEID_LABEL_DATA_ApplVersion				"carddata_appl_version"
#define BEID_LABEL_DATA_GlobOSVersion			"carddata_glob_os_version"
#define BEID_LABEL_DATA_ApplIntVersion			"carddata_appl_int_version"
#define BEID_LABEL_DATA_PKCS1Support			"carddata_pkcs1_support"
#define BEID_LABEL_DATA_ApplLifeCycle			"carddata_appl_lifecycle"
#define BEID_LABEL_DATA_KeyExchangeVersion		"carddata_key_exchange_version"
#define BEID_LABEL_DATA_PKCS15Version			"carddata_pkcs15_version"
#define BEID_LABEL_DATA_Signature				"carddata_signature"
#define BEID_LABEL_DATA_PinAuth					"carddata_pin_counter"

#define BEID_LABEL_ATR							"ATR"

//personalisation version numbers from tokeninfo file (4 bytes)
#define BEID_LABEL_PersoVersions				"perso_versions"

#define BEID_LABEL_KEY_CARD						"Card"

#define BEID_OBJECTID_ID						"id"
#define BEID_OBJECTID_ADDRESS					"address"
#define BEID_OBJECTID_PHOTO						"photo"
#define BEID_OBJECTID_CARDDATA					"carddata"
#define BEID_OBJECTID_RNCERT					"rncert"
#define BEID_OBJECTID_SIGN_DATA_FILE			"sign_data_file"
#define BEID_OBJECTID_SIGN_ADDRESS_FILE			"sign_address_file"
#define BEID_OBJECTID_BASIC_KEY_FILE			"basic_key_file"
#define BEID_OBJECTID_TOKENINFO					"tokeninfo"
//to give the hidden record objects their own (hidden/undocumented) CKA_OBJECT_ID
//this "record" CKA_OBJECT_ID is not meant to (and can not) be used to retrieve all hidden objects
#define BEID_OBJECTID_RECORD					"record"


#define BEID_LABEL_DATA_FILE		"DATA_FILE"
#define BEID_LABEL_ADDRESS_FILE		"ADDRESS_FILE"
#define BEID_LABEL_PHOTO			"PHOTO_FILE"
#define BEID_LABEL_CARD_DATA		"CARD_DATA"
#define BEID_LABEL_CERT_RN			"CERT_RN_FILE"
//#define BEID_LABEL_CERT_RNCA          "CERT_RNCA_FILE"
#define BEID_LABEL_SGN_RN			"SIGN_DATA_FILE"
#define BEID_LABEL_SGN_ADDRESS		"SIGN_ADDRESS_FILE"
#define BEID_LABEL_BASIC_KEY		"BASIC_KEY_FILE"

#endif
