
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
#if !defined(__CAL__)
#define __CAL__

#include <stdio.h>
#include "beid_p11.h"
//#include "CardLayer.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef PKCS11_FF
/*	int cal_getgnFFReaders(void);
	void cal_setgnFFReaders(int newgnFFReaders);
	void cal_incgnFFReaders(void);
	void cal_re_establish_context(void);*/
#endif

#define CAL_MECHANISM_TABLE { \
	{  CKM_MD5,					128, 128  , CKF_DIGEST         }, \
	{  CKM_SHA_1,			    160, 160  , CKF_DIGEST         }, \
	{  CKM_SHA256,				256, 256  , CKF_DIGEST         }, \
	{  CKM_SHA384,				384, 384  , CKF_DIGEST         }, \
	{  CKM_SHA512,				512, 512  , CKF_DIGEST         }, \
	{  CKM_RIPEMD160,			160, 160  , CKF_DIGEST         }, \
	{  CKM_RSA_PKCS,			1024, 2048, CKF_HW | CKF_SIGN  }, \
	{  CKM_MD5_RSA_PKCS,		1024, 2048, CKF_HW | CKF_SIGN  }, \
	{  CKM_SHA1_RSA_PKCS,		1024, 2048, CKF_HW | CKF_SIGN  }, \
	{  CKM_SHA256_RSA_PKCS,		1024, 2048, CKF_HW | CKF_SIGN  }, \
	{  CKM_SHA384_RSA_PKCS,		1024, 2048, CKF_HW | CKF_SIGN  }, \
	{  CKM_SHA512_RSA_PKCS,		1024, 2048, CKF_HW | CKF_SIGN  }, \
	{  CKM_RIPEMD160_RSA_PKCS,	1024, 2048, CKF_HW | CKF_SIGN  }, \
	{  CKM_SHA1_RSA_PKCS_PSS,	2048, 2048, CKF_HW | CKF_SIGN  }, \
	{  CKM_SHA256_RSA_PKCS_PSS,	2048, 2048, CKF_HW | CKF_SIGN  }, \
	{  CKM_ECDSA_SHA256,		256,  521,  CKF_HW | CKF_SIGN  }, \
	{  CKM_ECDSA_SHA384,		256,  521,  CKF_HW | CKF_SIGN  }, \
	{  CKM_ECDSA_SHA512,		256,  521,  CKF_HW | CKF_SIGN  }, \
	{  CKM_ECDSA,				256,  521,  CKF_HW | CKF_SIGN  }  \
}


#define BEID_TEMPLATE_PRV_KEY_RSA  {  \
	{ CKA_CLASS,            ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_ID,               ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_SUBJECT,          ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_LABEL,            ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_TOKEN,            ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_KEY_TYPE,         ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_SENSITIVE,        ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_SIGN,             ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_SIGN_RECOVER,     ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_DECRYPT,          ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_UNWRAP,           ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_MODULUS,          ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_MODULUS_BITS,     ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_EXTRACTABLE,		( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_PUBLIC_EXPONENT,  ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_PRIVATE,          ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_PRIVATE_EXPONENT, ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_PRIME_1,          ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_PRIME_2,          ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_EXPONENT_1,       ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_EXPONENT_2,       ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_COEFFICIENT,      ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_DERIVE,			( CK_VOID_PTR ) NULL, 0 }    \
}

#define BEID_TEMPLATE_PUB_KEY_RSA  {  \
	{ CKA_CLASS,            ( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_ID,               ( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_SUBJECT,          ( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_LABEL,            ( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_TOKEN,            ( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_KEY_TYPE,         ( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_PRIVATE,          ( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_TRUSTED,          ( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_SENSITIVE,        ( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_ENCRYPT,          ( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_VERIFY,           ( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_WRAP,             ( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_VALUE,			( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_MODULUS,          ( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_MODULUS_BITS,     ( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_PUBLIC_EXPONENT,  ( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_DERIVE,			( CK_VOID_PTR ) NULL, 0 }   \
}

#define BEID_TEMPLATE_PRV_KEY_EC  {  \
	{ CKA_CLASS,            ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_ID,               ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_SUBJECT,          ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_LABEL,            ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_TOKEN,            ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_KEY_TYPE,         ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_SENSITIVE,        ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_SIGN,             ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_SIGN_RECOVER,     ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_DECRYPT,          ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_UNWRAP,           ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_EC_PARAMS,        ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_EXTRACTABLE,		( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_PRIVATE,          ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_DERIVE,			( CK_VOID_PTR ) NULL, 0 }    \
}

#define BEID_TEMPLATE_PUB_KEY_EC  {  \
	{ CKA_CLASS,            ( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_ID,               ( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_SUBJECT,          ( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_LABEL,            ( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_TOKEN,            ( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_KEY_TYPE,         ( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_PRIVATE,          ( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_TRUSTED,          ( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_SENSITIVE,        ( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_ENCRYPT,          ( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_VERIFY,           ( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_WRAP,             ( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_EC_PARAMS,        ( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_EC_POINT,			( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_DERIVE,			( CK_VOID_PTR ) NULL, 0 }   \
}

#define BEID_TEMPLATE_CERTIFICATE  {  \
	{ CKA_CLASS,            ( CK_VOID_PTR )    NULL, 0 },  \
	{ CKA_ID,               ( CK_VOID_PTR )    NULL, 0 },  \
	{ CKA_SUBJECT,          ( CK_VOID_PTR )    NULL, 0 },  \
	{ CKA_ISSUER,           ( CK_VOID_PTR )    NULL, 0 },  \
	{ CKA_TOKEN,            ( CK_VOID_PTR )    NULL, 0 },  \
	{ CKA_TRUSTED,          ( CK_VOID_PTR )    NULL, 0 },  \
	{ CKA_CERTIFICATE_TYPE, ( CK_VOID_PTR )    NULL, 0 },  \
	{ CKA_PRIVATE,          ( CK_VOID_PTR )    NULL, 0 },  \
	{ CKA_SERIAL_NUMBER,    ( CK_VOID_PTR )    NULL, 0 },  \
	{ CKA_VALUE,            ( CK_VOID_PTR )    NULL, 0 },  \
	{ CKA_LABEL,            ( CK_VOID_PTR )    NULL, 0 },  \
	{ 0,                    ( CK_VOID_PTR )    NULL, 0 }   \
}

#define BEID_TEMPLATE_ID_DATA  {  \
	{ CKA_CLASS,            ( CK_VOID_PTR )    NULL, 0 },  \
	{ CKA_TOKEN,            ( CK_VOID_PTR )    NULL, 0 },  \
	{ CKA_PRIVATE,          ( CK_VOID_PTR )    NULL, 0 },  \
	{ CKA_VALUE,            ( CK_VOID_PTR )    NULL, 0 },  \
	{ CKA_VALUE_LEN,        ( CK_VOID_PTR )    NULL, 0 },  \
	{ CKA_LABEL,            ( CK_VOID_PTR )    NULL, 0 },  \
	{ CKA_MODIFIABLE,       ( CK_VOID_PTR )    NULL, 0 },  \
	{ CKA_OBJECT_ID,		( CK_VOID_PTR )    NULL, 0 }	 \
}

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
}

	//{ BEID_FIELD_TAG_ADDR_Version,							"Address_Version"},
#define BEID_ADDRESS_RECORD_DATA_LABELS  {  \
	{ BEID_FIELD_TAG_ADDR_Street,						"record_address_street_and_number"},  \
	{ BEID_FIELD_TAG_ADDR_ZipCode,						"record_address_zip"},  \
	{ BEID_FIELD_TAG_ADDR_Municipality,					"record_address_municipality"}  \
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

#define BEID_OBJECTID_ID						"id"
#define BEID_OBJECTID_ADDRESS					"address"
#define BEID_OBJECTID_PHOTO						"photo"
#define BEID_OBJECTID_CARDDATA					"carddata"
#define BEID_OBJECTID_RNCERT					"rncert"
#define BEID_OBJECTID_SIGN_DATA_FILE			"sign_data_file"
#define BEID_OBJECTID_SIGN_ADDRESS_FILE			"sign_address_file"
#define BEID_OBJECTID_BASIC_KEY_FILE			"basic_key_file"
//to give the hidden record objects their own (hidden/undocumented) CKA_OBJECT_ID
//this "record" CKA_OBJECT_ID is not meant to (and can not) be used to retrieve all hidden objects
#define BEID_OBJECTID_RECORD					"record"

	typedef struct BEID_DATA_LABELS_NAME
	{
		unsigned char tag;
		const char *name;
	} BEID_DATA_LABELS_NAME;

#define BEID_LABEL_DATA_FILE		"DATA_FILE"
#define BEID_LABEL_ADDRESS_FILE		"ADDRESS_FILE"
#define BEID_LABEL_PHOTO			"PHOTO_FILE"
#define BEID_LABEL_CARD_DATA		"CARD_DATA"
#define BEID_LABEL_CERT_RN			"CERT_RN_FILE"
//#define BEID_LABEL_CERT_RNCA          "CERT_RNCA_FILE"
#define BEID_LABEL_SGN_RN			"SIGN_DATA_FILE"
#define BEID_LABEL_SGN_ADDRESS		"SIGN_ADDRESS_FILE"
#define BEID_LABEL_BASIC_KEY		"BASIC_KEY_FILE"

	typedef enum {
		BEID_PIN_READ_EF,
		BEID_PIN_AUTH,
		BEID_PIN_NONREP,
	} tPinObjective;

	CK_RV cal_init(void);
//void cal_init_pcsc();
//	long cal_check_pcsc(CK_BBOOL * pRunning);
	void cal_wait(int millisecs);
	void cal_close(void);
	void cal_clean_slots(void);
	CK_RV cal_init_slots(void);
	CK_RV cal_token_present(CK_SLOT_ID hSlot, int *pPresent);
	CK_RV cal_get_token_info(CK_SLOT_ID hSlot, CK_TOKEN_INFO_PTR pInfo);
	CK_RV cal_get_mechanism_list(CK_SLOT_ID hSlot,
				     CK_MECHANISM_TYPE_PTR pMechanismList,
				     CK_ULONG_PTR pulCount);
	CK_RV cal_connect(CK_SLOT_ID hSlot);
	CK_RV cal_disconnect(CK_SLOT_ID hSlot);
	CK_RV cal_init_objects(P11_SLOT * pSlot);
	CK_RV cal_get_mechanism_info(CK_SLOT_ID hSlot, CK_MECHANISM_TYPE type,
				     CK_MECHANISM_INFO_PTR pInfo);
	CK_RV cal_logon(CK_SLOT_ID hSlot, size_t l_pin, CK_CHAR_PTR pin,
			int sec_messaging);
	CK_RV cal_logout(CK_SLOT_ID hSlot);
	CK_RV cal_change_pin(CK_SLOT_ID hSlot, CK_ULONG pinref, CK_ULONG l_oldpin,
			     CK_CHAR_PTR oldpin, CK_ULONG l_newpin,
			     CK_CHAR_PTR newpin);
	CK_RV cal_get_card_data(CK_SLOT_ID hSlot);
	CK_RV cal_read_and_store_record(P11_SLOT* pSlot, CK_ULONG ulDataType, CK_BYTE bRecordID, CK_UTF8CHAR* plabel, CK_ULONG ulLabelLen);
	CK_RV cal_read_ID_files(CK_SLOT_ID hSlot, CK_ULONG dataType);
	CK_RV cal_read_object(CK_SLOT_ID hSlot, P11_OBJECT * pObject);
	CK_RV cal_sign(CK_SLOT_ID hSlot, P11_SIGN_DATA * pSignData,
		       unsigned char *in, unsigned long l_in,
		       unsigned char *out, unsigned long *l_out);
	CK_RV cal_challenge(CK_SLOT_ID hSlot, P11_SIGN_DATA * pSignData, unsigned char *in,
		unsigned long l_in, unsigned char *out, unsigned long *l_out);
	CK_RV cal_validate_session(P11_SESSION * pSession);
	CK_RV cal_update_token(CK_SLOT_ID hSlot, int *pStatus, int bPresenceOnly);
	CK_RV cal_wait_for_slot_event(int block);
	CK_RV cal_wait_for_the_slot_event(int block);
	CK_RV cal_get_slot_changes(int *ph);
	CK_RV cal_refresh_readers(void);

#ifdef __cplusplus
}
#endif


#endif
