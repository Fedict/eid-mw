
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
#if !defined(__SMARTCARD__)
#define __SMARTCARD__

//#include "winscard.h"
#include "beid_p11.h"
#include "util.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define CLEANUP(a)   { ret = (a); goto cleanup; }


#define MAX_SLOTS 10

#define MAX_SLOT_NAME			128
#define MAX_ATR_LENGTH			128
#define MAX_LABEL_LENGTH		32

#define MAX_SESSIONS          1000
#define MAX_RW_SESSIONS       1000

//max values for object (nr of attribute*sizeof(attribute) in object)
//max size for single attribute (e.g. certificate is around 2k)
#define MAX_OBJECT_SIZE       32000
#define MAX_ATTRIBUTE_SIZE    64000

//max digest size, we don't check the max size among available mechanisms in token, so digest can be done token unrelated.
#define MAX_DIGEST_SIZE       64

#define CARD_TYPE_BEID				1
#define CARD_TYPE_TELESEC			2
#define CARD_TYPE_TELESEC_30	   3
#define CARD_TYPE_GEMPLUS			100

#define CARD_TYPE_NOTSUPPORTED		   -100


#define CACHED_DATA_TYPE_ID			0x001
#define CACHED_DATA_TYPE_ADDRESS		0x002
#define CACHED_DATA_TYPE_PHOTO			0x004
#define CACHED_DATA_TYPE_CARDDATA		0x008
#define CACHED_DATA_TYPE_RNCERT			0x010
#define CACHED_DATA_TYPE_SIGN_DATA_FILE		0x020
#define CACHED_DATA_TYPE_SIGN_ADDRESS_FILE	0x040
#define CACHED_DATA_TYPE_ALL_DATA		0x07F	//All the above are set
#define CACHED_DATA_TYPE_CDF			0x080
#define CACHED_DATA_TYPE_AUTH_CERT		0x100
#define CACHED_DATA_TYPE_SIGN_CERT		0x200
#define CACHED_DATA_TYPE_CA_CERT		0x400
#define CACHED_DATA_TYPE_ROOT_CERT		0x800

#define CARD_FUNCTION_NOT_IMPLEMENTED  -101
#define CARD_FUNCTION_NOT_SUPPORTED		-102

#define SIGN_TYPE_NONREP		1
#define SIGN_TYPE_DIGSIG		2

#define SESSION_TAB_STEP_SIZE 10
#define OBJECT_TAB_STEP_SIZE 3

#define P11_SESSION_INVALID   -1000
#define P11_SESSION_VALID      1
#define P11_SLOT_OBJECT_ERROR -1001

#define P11_PIN_INCORRECT     -1
#define P11_PIN_BLOCKED       -2

#define P11_CACHED            1


#define P11_CARD_INSERTED           0
#define P11_CARD_NOT_PRESENT        1
#define P11_CARD_STILL_PRESENT      2
#define P11_CARD_REMOVED            3
#define P11_CARD_OTHER              4
#define P11_CARD_UNKNOWN_STATE	    5

#define P11_EVENT_NONE              0
#define P11_EVENT_INSERT            1
#define P11_EVENT_REMOVE           -1

	typedef struct CARD_TYPE
	{
		char *atr;
		char *label;
		int type;
	} CARD_TYPE;

//extern CARD_TYPE g_CardTypeTable[];

#define CARD_TYPE_TABLE { \
	{ "3B9813400AA503010101AD1311",						"BE eid",						CARD_TYPE_BEID },		      \
	{ "3B9894400AA503010101AD1310",						"BE eid",						CARD_TYPE_BEID },		      \
	{ "3B989440FFA503010101AD1310",						"BE eid",						CARD_TYPE_BEID },		      \
	{ "3BBA96008131865D0064057B0203318090007D",		"Telesec",				   	CARD_TYPE_TELESEC},	      \
	{ "3BBA96008131865D0064056002033180900066",		"Telesec",				   	CARD_TYPE_TELESEC },       \
	{ "3BA70040148065A214010137",						   "?Gemplus GPK4000",        CARD_TYPE_GEMPLUS },       \
	{ "3B7D94000080318065B08301029083009000",			"?Gemplus XPresso 32K",    CARD_TYPE_NOTSUPPORTED },  \
	{ "3BBF96008131FE5D00640411030131C073F701D00090007D", "Telesec TCOS 3.0",  CARD_TYPE_TELESEC_30 },    \
	NULL \
}

#if 0
	typedef struct P11_TOKEN
	{
		SCARDHANDLE hdl;
		int isT1;
		int type;
		char a_cType[MAX_LABEL_LENGTH];
		char atr[MAX_ATR_LENGTH];
	} P11_TOKEN;
#endif

	typedef struct P11_OBJECT
	{
		int inuse;
		int state;
		CK_ATTRIBUTE_PTR pAttr;
		CK_ULONG count;
	} P11_OBJECT;


	typedef struct P11_SLOT
	{
		char name[MAX_SLOT_NAME];
		int login_type;
		unsigned int nsessions;
		unsigned int connect;
		int ievent;  //0:nothing   1:inserted    2:removed
//P11_TOKEN      token;
		P11_OBJECT *pobjects;
		unsigned int nobjects;
		void *pReader;	//CReader
		CK_ULONG ulCardDataCached;
	} P11_SLOT;

//pReader = &oReader;


#define P11_NUM_OPERATIONS    3
#define P11_OPERATION_FIND    0
#define P11_OPERATION_DIGEST  1
#define P11_OPERATION_SIGN    2

#define P11_READDATA_ASK				0
#define P11_READDATA_ALLOWED		1
#define P11_READDATA_REFUSED    2
#define P11_READDATA_ALWAYS			3

	typedef struct P11_OPERATION
	{
		int type;
		int active;
		void *pData;
	} P11_OPERATION;

	typedef struct P11_SESSION
	{
		int inuse;
		CK_SLOT_ID hslot;
		CK_FLAGS flags;
		CK_VOID_PTR pdNotify;
		CK_NOTIFY pfNotify;
		int state;
//int               nObjects;
//P11_OBJECT        **ppObjects;
		P11_OPERATION Operation[P11_NUM_OPERATIONS];
		CK_BYTE bReadDataAllowed;
	} P11_SESSION;


	typedef struct P11_MECHANISM_INFO
	{
		CK_MECHANISM_TYPE type;
		CK_ULONG ulMinKeySize;
		CK_ULONG ulMaxKeySize;
		CK_FLAGS flags;
	} P11_MECHANISM_INFO;


	typedef struct P11_FIND_DATA
	{
		CK_ATTRIBUTE_PTR pSearch;
		CK_ULONG size;
		CK_OBJECT_HANDLE hCurrent;
	} P11_FIND_DATA;


	typedef struct P11_DIGEST_DATA
	{
		int update;
		void *phash;
		unsigned int l_hash;
	} P11_DIGEST_DATA;

	typedef struct P11_SIGN_DATA
	{
		int update;
		CK_MECHANISM_TYPE mechanism;
		CK_OBJECT_HANDLE hKey;
		CK_ULONG l_sign;
		CK_ULONG id;
		void *phash;
		unsigned int l_hash;
		char *pbuf;
		unsigned int lbuf;
	} P11_SIGN_DATA;


//void memwash(char *p_in, unsigned int len);
	int p11_get_token_type(char *atr, char *a_cType);

#if 0
	int p11_init();
	int p11_close();
	int p11_init_slots();
	int p11_connect(P11_SLOT * pReader);
	int p11_disconnect(P11_SLOT * pSlot);
	int p11_change_pin(P11_SLOT * pSlot, int l_oldpin, char *oldpin,
			   int l_newpin, char *newpin);
	int p11_logon(P11_SLOT * pSlot, int l_pin, char *pin,
		      int sec_messaging);
	int p11_logout(P11_SLOT * pSlot);
	int p11_sign(P11_SLOT * pSlot, P11_SIGN_DATA * pSignData,
		     unsigned char *in, int l_in, unsigned char *out,
		     unsigned int *l_out);
	int p11_read_certificate(P11_SLOT * pSlot, int *l_cert,
				 char **pp_cert);
	CK_RV p11_get_mechanism_list(P11_SLOT * pSlot,
				     CK_MECHANISM_TYPE_PTR pMechanismList,
				     CK_ULONG_PTR pulCount);
	CK_RV p11_get_mechanism_info(P11_SLOT * pSlot, CK_MECHANISM_TYPE type,
				     CK_MECHANISM_INFO_PTR pInfo);
	CK_RV p11_get_token_info(CK_SLOT_ID slotID, CK_TOKEN_INFO_PTR pInfo);
	int p11_init_objects(P11_SLOT * pSlot);
	int p11_read_object(P11_SLOT * pSlot, P11_OBJECT * pObject);
#endif

	P11_SLOT *p11_get_slot(unsigned int h);
	int p11_get_session(unsigned int h, P11_SESSION ** ppSession);
	P11_OBJECT *p11_get_slot_object(P11_SLOT * pSlot, unsigned int h);
	int p11_get_nreaders();

	CK_RV p11_close_sessions_finalize();
	CK_RV p11_close_session(P11_SLOT *, P11_SESSION *);
	CK_RV p11_close_all_sessions(CK_SLOT_ID slotID);
	CK_RV p11_get_free_session(CK_SESSION_HANDLE_PTR phSession,
				   P11_SESSION ** ppSession);
	CK_RV p11_get_attribute_value(CK_ATTRIBUTE_PTR pTemplate,
				      CK_ULONG ulCount,
				      CK_ATTRIBUTE_TYPE type,
				      CK_VOID_PTR * ppVoid, CK_ULONG * len);
	CK_RV p11_set_attribute_value(CK_ATTRIBUTE_PTR pTemplate,
				      CK_ULONG ulCount,
				      CK_ATTRIBUTE_TYPE type,
				      CK_VOID_PTR pVoid, CK_ULONG len);
	CK_RV p11_copy_object(CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount,
			      CK_ATTRIBUTE_PTR pObject);
	CK_RV p11_add_slot_ID_object(P11_SLOT * pSlot,
				     CK_ATTRIBUTE_PTR pTemplate,
				     CK_ULONG ulCount, CK_BBOOL bToken,
				     CK_ULONG type, CK_BBOOL bPrivate,
				     CK_ULONG * phObject, CK_VOID_PTR plabel,
				     CK_ULONG labelLen, CK_VOID_PTR pvalue,
				     CK_ULONG valueLen, CK_VOID_PTR pobjectID,
				     CK_ULONG objectIDLen);
	CK_RV p11_add_slot_object(P11_SLOT * pSlot,
				  CK_ATTRIBUTE_PTR pTemplate,
				  CK_ULONG ulCount, CK_BBOOL bToken,
				  CK_ULONG type, CK_ULONG id,
				  CK_BBOOL bPrivate, CK_ULONG * phObject);
	void p11_clean_object(P11_OBJECT * pObject);
	void p11_clean_attributelist(CK_ATTRIBUTE_PTR pAttr,
				     CK_ULONG ulCount);
	void p11_clean_finddata(P11_FIND_DATA * pFindData);
	CK_RV p11_find_slot_object(P11_SLOT * pSlot, CK_ULONG type,
				   CK_ULONG id, P11_OBJECT ** pphObject);
	int p11_attribute_present(CK_ATTRIBUTE_TYPE type,
				  CK_ATTRIBUTE_PTR pTemplate,
				  CK_ULONG ulCount);
	CK_RV p11_invalidate_sessions(CK_SLOT_ID hSlot, int status);

#ifdef __cplusplus
}
#endif


#endif
