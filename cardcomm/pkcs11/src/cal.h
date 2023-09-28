
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
#include "beidlabels.h"

#ifdef __cplusplus
extern "C"
{
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
	{  CKM_ECDSA,				256,  521,  CKF_HW | CKF_SIGN  }, \
	{  CKM_ECDSA_SHA3_256,		256,  521,  CKF_HW | CKF_SIGN  }, \
	{  CKM_ECDSA_SHA3_384,		256,  521,  CKF_HW | CKF_SIGN  }, \
	{  CKM_ECDSA_SHA3_512,		256,  521,  CKF_HW | CKF_SIGN  }  \
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
	{ CKA_OBJECT_ID,		( CK_VOID_PTR )    NULL, 0 }   \
}

	typedef struct BEID_DATA_LABELS_NAME
	{
		unsigned char tag;
		const char *name;
	} BEID_DATA_LABELS_NAME;

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
