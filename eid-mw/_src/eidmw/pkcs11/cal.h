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
#if !defined(__CAL__)
#define __CAL__

#include "beid_p11.h"
//#include "CardLayer.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PKCS11_FF
	int cal_getgnFFReaders(void);
	void cal_setgnFFReaders(int newgnFFReaders);
	void cal_incgnFFReaders(void);
#endif

#define CAL_MECHANISM_TABLE  { \
	{  CKM_MD5,                128, 128  , CKF_DIGEST         }, \
	{  CKM_SHA_1,              160, 160  , CKF_DIGEST         }, \
	{  CKM_SHA256,             256, 256  , CKF_DIGEST         }, \
	{  CKM_SHA384,             384, 384  , CKF_DIGEST         }, \
	{  CKM_SHA512,             512, 512  , CKF_DIGEST         }, \
	{  CKM_RIPEMD160,          160, 160  , CKF_DIGEST         }, \
	{  CKM_RSA_PKCS,           1024, 1024, CKF_HW | CKF_SIGN  }, \
	{  CKM_MD5_RSA_PKCS,       1024, 1024, CKF_HW | CKF_SIGN  }, \
	{  CKM_SHA1_RSA_PKCS,      1024, 1024, CKF_HW | CKF_SIGN  }, \
	{  CKM_SHA256_RSA_PKCS,    1024, 1024, CKF_HW | CKF_SIGN  }, \
	{  CKM_SHA384_RSA_PKCS,    1024, 1024, CKF_HW | CKF_SIGN  }, \
	{  CKM_SHA512_RSA_PKCS,    1024, 1024, CKF_HW | CKF_SIGN  }, \
	{  CKM_RIPEMD160_RSA_PKCS, 1024, 1024, CKF_HW | CKF_SIGN  }  \
}


#define BEID_TEMPLATE_PRV_KEY  {  \
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
	{ CKA_EXTRACTABLE,			( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_PUBLIC_EXPONENT,  ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_PRIVATE,          ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_PRIVATE_EXPONENT, ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_PRIME_1,          ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_PRIME_2,          ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_EXPONENT_1,       ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_EXPONENT_2,       ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_COEFFICIENT,      ( CK_VOID_PTR ) NULL, 0 },   \
	{ CKA_DERIVE,						( CK_VOID_PTR ) NULL, 0 }    \
}

#define BEID_TEMPLATE_PUB_KEY  {  \
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
	{ CKA_VALUE,						( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_MODULUS,          ( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_MODULUS_BITS,     ( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_PUBLIC_EXPONENT,  ( CK_VOID_PTR ) NULL, 0 },  \
	{ CKA_DERIVE,						( CK_VOID_PTR ) NULL, 0 }   \
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
	{ CKA_OBJECT_ID,				( CK_VOID_PTR )    NULL, 0 }	 \
}

	//	{ BEID_FIELD_TAG_ID_Version,			"Version"},
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
	{ BEID_FIELD_TAG_ID_MemberOfFamily,		"member_of_family"}  \
}

	//{ BEID_FIELD_TAG_ADDR_Version,			"Address_Version"},
#define BEID_ADDRESS_DATA_LABELS  {  \
	{ BEID_FIELD_TAG_ADDR_Street,			"address_street_and_number"},  \
	{ BEID_FIELD_TAG_ADDR_ZipCode,			"address_zip"},  \
	{ BEID_FIELD_TAG_ADDR_Municipality,		"address_municipality"}  \
}

#define BEID_LABEL_DATA_SerialNr				"carddata_serialnumber"
#define BEID_LABEL_DATA_CompCode				"carddata_comp_code"
#define BEID_LABEL_DATA_OSNr						"carddata_os_number"
#define BEID_LABEL_DATA_OSVersion				"carddata_os_version"
#define BEID_LABEL_DATA_SoftMaskNumber	"carddata_soft_mask_number"
#define BEID_LABEL_DATA_SoftMaskVersion	"carddata_soft_mask_version"
#define BEID_LABEL_DATA_ApplVersion			"carddata_appl_version"
#define BEID_LABEL_DATA_GlobOSVersion		"carddata_glob_os_version"
#define BEID_LABEL_DATA_ApplIntVersion	"carddata_appl_int_version"
#define BEID_LABEL_DATA_PKCS1Support		"carddata_pkcs1_support"
#define BEID_LABEL_DATA_ApplLifeCycle		"carddata_appl_lifecycle"
#define BEID_LABEL_DATA_KeyExchangeVersion	"carddata_key_exchange_version"
#define BEID_LABEL_DATA_Signature				"carddata_signature"

#define BEID_LABEL_ATR									"ATR"

#define BEID_OBJECTID_ID									"id"		
#define BEID_OBJECTID_ADDRESS							"address"
#define BEID_OBJECTID_PHOTO								"photo"
#define BEID_OBJECTID_CARDDATA						"carddata"
#define BEID_OBJECTID_RNCERT							"rncert"
#define BEID_OBJECTID_SIGN_DATA_FILE			"sign_data_file"
#define BEID_OBJECTID_SIGN_ADDRESS_FILE		"sign_address_file"

typedef struct BEID_DATA_LABELS_NAME {
	unsigned char	tag;
	char*			name;
}BEID_DATA_LABELS_NAME;

#define BEID_LABEL_DATA_FILE		"DATA_FILE"
#define BEID_LABEL_ADDRESS_FILE		"ADDRESS_FILE"
#define BEID_LABEL_PHOTO			"PHOTO_FILE"
#define BEID_LABEL_CARD_DATA		"CARD_DATA"
#define BEID_LABEL_CERT_RN			"CERT_RN_FILE"
//#define BEID_LABEL_CERT_RNCA		"CERT_RNCA_FILE"

#define BEID_LABEL_SGN_RN			"SIGN_DATA_FILE"
#define BEID_LABEL_SGN_ADDRESS	"SIGN_ADDRESS_FILE"



int cal_init();
int cal_close();
int cal_clean_slots(void);
int cal_init_slots(void);
int cal_token_present(CK_SLOT_ID hSlot);
CK_RV cal_get_token_info(CK_SLOT_ID hSlot, CK_TOKEN_INFO_PTR pInfo);
CK_RV cal_get_mechanism_list(CK_SLOT_ID hSlot, CK_MECHANISM_TYPE_PTR pMechanismList, CK_ULONG_PTR pulCount);
int cal_connect(CK_SLOT_ID hSlot);
int cal_disconnect(CK_SLOT_ID hSlot);
int cal_init_objects(CK_SLOT_ID hSlot);
CK_RV cal_get_mechanism_info(CK_SLOT_ID hSlot, CK_MECHANISM_TYPE type, CK_MECHANISM_INFO_PTR pInfo);
int cal_logon(CK_SLOT_ID hSlot, size_t l_pin, CK_CHAR_PTR pin, int sec_messaging);
int cal_logout(CK_SLOT_ID hSlot);
int cal_change_pin(CK_SLOT_ID hSlot, int l_oldpin, CK_CHAR_PTR oldpin, int l_newpin, CK_CHAR_PTR newpin);
int cal_get_card_data(CK_SLOT_ID hSlot);
int cal_read_ID_files(CK_SLOT_ID hSlot, CK_BYTE dataType);
int cal_read_object(CK_SLOT_ID hSlot, P11_OBJECT *pObject);
int cal_sign(CK_SLOT_ID hSlot, P11_SIGN_DATA *pSignData, unsigned char* in, unsigned long l_in, unsigned char *out, unsigned long *l_out);
int cal_validate_session(P11_SESSION *pSession);
int cal_update_token(CK_SLOT_ID hSlot);
CK_RV cal_wait_for_slot_event(int block);
CK_RV cal_wait_for_the_slot_event(int block);
CK_RV cal_get_slot_changes(int *ph);
int cal_refresh_readers();

#ifdef __cplusplus
  }
#endif


#endif

