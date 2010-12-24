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
#if !defined(__CAL__)
#define __CAL__

#include "beid_p11.h"
//#include "CardLayer.h"

#ifdef __cplusplus
extern "C" {
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
	{ CKA_MODIFIABLE,       ( CK_VOID_PTR )    NULL, 0 }   \
}

	//	{ BEID_FIELD_TAG_ID_Version,			"Version"},
#define BEID_ID_DATA_LABELS  {  \
	{ BEID_FIELD_TAG_ID_CardNr,				"CardNr"},  \
	{ BEID_FIELD_TAG_ID_ChipNr,				"ChipNr"},  \
	{ BEID_FIELD_TAG_ID_ValidityBeginDate,	"Validity_Begin_Date"},  \
	{ BEID_FIELD_TAG_ID_ValidityEndDate,	"Validity_End_Date"},  \
	{ BEID_FIELD_TAG_ID_IssuingMunicipality,"Issuing_Municipality"},  \
	{ BEID_FIELD_TAG_ID_NationalNr,			"National_Nr"},  \
	{ BEID_FIELD_TAG_ID_Surname,			"Surname"},  \
	{ BEID_FIELD_TAG_ID_FirstNames,		"FirstNames"},  \
	{ BEID_FIELD_TAG_ID_FirstLetterOfThirdGivenName,		"First_Letter_Of_Third_Given_Name"},  \
	{ BEID_FIELD_TAG_ID_Nationality,		"Nationality"},  \
	{ BEID_FIELD_TAG_ID_LocationOfBirth,	"Location_Of_Birth"},  \
	{ BEID_FIELD_TAG_ID_DateOfBirth,		"Date_Of_Birth"},  \
	{ BEID_FIELD_TAG_ID_Gender,				"Gender"},  \
	{ BEID_FIELD_TAG_ID_Nobility,			"Nobility"},  \
	{ BEID_FIELD_TAG_ID_DocumentType,		"Document_Type"},  \
	{ BEID_FIELD_TAG_ID_SpecialStatus,		"Special_Status"},  \
	{ BEID_FIELD_TAG_ID_PhotoHash,			"Photo_Hash"},  \
	{ BEID_FIELD_TAG_ID_Duplicata,			"Duplicata"},  \
	{ BEID_FIELD_TAG_ID_SpecialOrganization,"Special_Organization"},  \
	{ BEID_FIELD_TAG_ID_MemberOfFamily,		"Member_Of_Family"}  \
}

	//{ BEID_FIELD_TAG_ADDR_Version,			"Address_Version"},
#define BEID_ADDRESS_DATA_LABELS  {  \
	{ BEID_FIELD_TAG_ADDR_Street,			"Address_Street_Number"},  \
	{ BEID_FIELD_TAG_ADDR_ZipCode,			"Address_Zip"},  \
	{ BEID_FIELD_TAG_ADDR_Municipality,		"Address_Municipality"}  \
}

#define BEID_FIELD_TAG_DATA_SerialNr				"CardData_SerialNumber"
#define BEID_FIELD_TAG_DATA_CompCode				"CardData_CompCode"
#define BEID_FIELD_TAG_DATA_OSNr						"CardData_OSNumber"
#define BEID_FIELD_TAG_DATA_OSVersion				"CardData_OSVersion"
#define BEID_FIELD_TAG_DATA_SoftMaskNumber	"CardData_SoftMaskNumber"
#define BEID_FIELD_TAG_DATA_SoftMaskVersion	"CardData_SoftMaskVersion"
#define BEID_FIELD_TAG_DATA_ApplVersion			"CardData_ApplVersion"
#define BEID_FIELD_TAG_DATA_GlobOSVersion		"CardData_GlobOSVersion"
#define BEID_FIELD_TAG_DATA_ApplIntVersion	"CardData_ApplIntVersion"
#define BEID_FIELD_TAG_DATA_PKCS1Support		"CardData_PKCS1Support"
#define BEID_FIELD_TAG_DATA_ApplLifeCycle		"CardData_ApplLifeCycle"
#define BEID_FIELD_TAG_DATA_KeyExchangeVersion	"CardData_KeyExchangeVersion"
#define BEID_FIELD_TAG_DATA_Signature				"CardData_Signature"

#define BEID_FIELD_TAG_ATR									"ATR"

typedef struct BEID_DATA_LABELS_NAME {
	unsigned char	tag;
	char*			name;
}BEID_DATA_LABELS_NAME;

#define BEID_FIELD_TAG_DATA_FILE		"DATA_FILE"
#define BEID_FIELD_TAG_ADDRESS_FILE		"ADDRESS_FILE"
#define BEID_FIELD_TAG_PHOTO			"PHOTO"
#define BEID_FIELD_TAG_CARD_DATA		"CARD_DATA"
#define BEID_FIELD_TAG_CERT_RN			"CERT_RN_FILE"
#define BEID_FIELD_TAG_CERT_RNCA		"CERT_RNCA_FILE"

#define BEID_FIELD_TAG_SGN_RN			"SGN_DATA_FILE"
#define BEID_FIELD_TAG_SGN_ADDRESS	"SGN_ADDRESS_FILE"



int cal_init();
int cal_close();
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
int cal_read_ID_files(CK_SLOT_ID hSlot);
int cal_read_object(CK_SLOT_ID hSlot, P11_OBJECT *pObject);
int cal_sign(CK_SLOT_ID hSlot, P11_SIGN_DATA *pSignData, unsigned char* in, unsigned long l_in, unsigned char *out, unsigned long *l_out);
int cal_validate_session(P11_SESSION *pSession);
int cal_update_token(CK_SLOT_ID hSlot);
int cal_wait_for_slot_event(int block, int *cardevent, int *ph);


#ifdef __cplusplus
  }
#endif


#endif

