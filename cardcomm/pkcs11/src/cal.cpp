
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
#include "bytearray.h"
#include "eiderrors.h"
#include "beid_p11.h"
#include "cardlayer.h"
#include "readersinfo.h"
#include "p11.h"
#include "util.h"
#include "cal.h"
#include "pkcs11log.h"
#include "cert.h"
#include "mw_util.h"
#include "tlvbuffer.h"
#include "thread.h"
#ifdef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
#include "beid_fuzz.h"
#endif
#ifndef WIN32
#define strcpy_s(a,b,c)         strcpy((a),(c))
#define sprintf_s(a,b,c,d)    sprintf((a),(c),(d))

//linux config file
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#endif


using namespace eIDMW;

CCardLayer *oCardLayer;
CReadersInfo *oReadersInfo;

extern "C"
{
	extern unsigned int gRefCount;
	extern unsigned int nReaders;
	extern bool gSlotsChanged;
	extern P11_SLOT gpSlot[MAX_SLOTS];
	//local functions
	CK_RV cal_translate_error(const char *WHERE, long err);
	int cal_map_status(tCardStatus calstatus);
}

#ifdef PKCS11_FF
/*
static int gnFFReaders;
int cal_getgnFFReaders(void)
{
	return gnFFReaders;
}

void cal_setgnFFReaders(int newgnFFReaders)
{
	gnFFReaders = newgnFFReaders;
}

void cal_incgnFFReaders(void)
{
	gnFFReaders++;
}

void cal_re_establish_context(void)
{
	oCardLayer->PCSCReEstablishContext();
}*/

#endif

void cal_free_reader_states(SCARD_READERSTATEA * txReaderStates,
			    unsigned long ulnReaders);

/*
long cal_check_pcsc(CK_BBOOL* pRunning)
{
	//allthough both CK_BBOOL and bool are 1 byte at the moment
	bool pbRunning = false;
	long error = 0;
	error = oCardLayer->PCSCServiceRunning(&pbRunning);
	if (pbRunning == true)
		*pRunning = CK_TRUE;
	else
		*pRunning = CK_FALSE;
	return error;
}*/

void cal_wait(int millisecs)
{
	CThread::SleepMillisecs(millisecs);
}

/*#define WHERE "cal_init_pcsc()"
void cal_init_pcsc()
{
	oCardLayer->StartPCSCService();
}
#undef WHERE*/

#define WHERE "cal_init()"
CK_RV cal_init()
{
	CK_RV ret = CKR_OK;

	if (gRefCount > 0)
		return 0;

	try
	{
		oCardLayer = new CCardLayer();
		oReadersInfo = new CReadersInfo(oCardLayer->ListReaders());
	}
	catch(CMWException &e)
	{
		return (cal_translate_error(WHERE, e.GetError()));
	}
	catch( ...)
	{
		log_trace(WHERE, "E: unkown exception thrown");
		return (CKR_FUNCTION_FAILED);
	}

	//init slots and token in slots
#ifdef PKCS11_FF
//	gnFFReaders = 0;
#endif
	memset(gpSlot, 0, sizeof(gpSlot));
	ret = cal_init_slots();
	if (ret)
		log_trace(WHERE, "E: p11_init_slots() returns %lu", ret);

	return (ret);
}

#undef WHERE


#define WHERE "cal_close()"
void cal_close()
{
	//Reference count countdown, clean if 0
	//if (--gRefCount > 0)
	//   return (0);

	if (oCardLayer)
		delete(oCardLayer);
	if (oReadersInfo)
		delete(oReadersInfo);

	oCardLayer = NULL;
	oReadersInfo = NULL;

	cal_clean_slots();

	return;
}

#undef WHERE

#define WHERE "cal_clean_slots()"
void cal_clean_slots()
{
	unsigned int i;
	CK_SLOT_ID hSlot = 0;
	P11_SLOT *pSlot = NULL;
	P11_OBJECT *pObject = NULL;

	for (; hSlot < MAX_SLOTS; hSlot++)
	{
		pSlot = p11_get_slot(hSlot);
		if (pSlot == NULL)
		{
			//slot not in use
			break;
		}
		//clean objects
		for (i = 1; i <= pSlot->nobjects; i++)
		{
			pObject = p11_get_slot_object(pSlot, i);
			p11_clean_object(pObject);
			//if (pObject != NULL)
			// pObject->state = 0;
		}
		if (pSlot->pobjects != NULL)
		{
			free(pSlot->pobjects);
			pSlot->pobjects = NULL;
			pSlot->ulCardDataCached = 0;
		}
	}
	return;
}

#undef WHERE



#define WHERE "cal_init_slots()"
CK_RV cal_init_slots(void)
{
	CK_RV ret = CKR_OK;
	unsigned int i;

	try
	{
		nReaders = (unsigned int)(oReadersInfo->ReaderCount());
		//get readernames
		for (i = 0; i < nReaders; i++)
		{
			//initialize login state to not logged in by SO nor user
			gpSlot[i].logged_in = CK_FALSE;
			std::string reader = oReadersInfo->ReaderName(i);
			strcpy_n((unsigned char *) gpSlot[i].name,
				 (const char *) reader.c_str(),
				 (unsigned int) reader.size(), (char) '\x00');
		}
	}
	catch(CMWException &e)
	{
		return (cal_translate_error(WHERE, e.GetError()));
	}
	catch( ...)
	{
		log_trace(WHERE, "E: unkown exception thrown");
		return (CKR_FUNCTION_FAILED);
	}

	return (ret);
}

#undef WHERE


#define WHERE "cal_token_present()"
CK_RV cal_token_present(CK_SLOT_ID hSlot, int *pPresent)
{
	CK_RV ret = CKR_OK;
	int status = P11_CARD_NOT_PRESENT;

	ret = cal_update_token(hSlot, &status, 0);

	switch (status)
	{
		case P11_CARD_INSERTED:
		case P11_CARD_STILL_PRESENT:
		case P11_CARD_OTHER:
			*pPresent = 1;
			break;
		case P11_CARD_NOT_PRESENT:
		case P11_CARD_REMOVED:
		case P11_CARD_UNKNOWN_STATE:
		default:
			*pPresent = 0;
	}

	return (ret);
}

#undef WHERE




#define WHERE "cal_get_token_info()"
CK_RV cal_get_token_info(CK_SLOT_ID hSlot, CK_TOKEN_INFO_PTR pInfo)
{
	CK_RV ret = CKR_OK;
	int status;
	P11_SLOT *pSlot = NULL;

	pInfo->flags = 0;

	pSlot = p11_get_slot(hSlot);
	if (pSlot == NULL)
	{
		log_trace(WHERE, "E: Invalid slot (%lu)", hSlot);
		return (CKR_SLOT_ID_INVALID);
	}

	std::string reader = pSlot->name;

	ret = cal_update_token(hSlot, &status, 0);
	if (ret != CKR_OK)
		goto cleanup;

	if ((status == P11_CARD_REMOVED) || (status == P11_CARD_NOT_PRESENT))
	{
		ret = CKR_TOKEN_NOT_PRESENT;
		goto cleanup;
	}

	pInfo->firmwareVersion.major = 1;
	pInfo->firmwareVersion.minor = 0;

	try
	{
		// Take the last 16 hex chars of the serialnr.
		// For BE eID cards, the serial nr. is 32 hex chars long,
		// and the first one are the same for all cards
		CReader & oReader = oCardLayer->getReader(reader);
		CCard* poCard = oReader.GetCard();

		std::string oSerialNr = poCard->GetSerialNr();
		size_t serialNrLen = oSerialNr.size();
		size_t snoffset = serialNrLen > 16 ? serialNrLen - 16 : 0;
		size_t snlen = serialNrLen - snoffset > 16 ? 16 : serialNrLen - snoffset;
		//printf("off = %d, len = %d\n", snoffset, snlen);
		strcpy_n(pInfo->serialNumber, oSerialNr.c_str() + snoffset, snlen, ' ');
		strcpy_n(pInfo->label, poCard->GetCardLabel().c_str(), 32,  ' ');
		if (poCard->IsPinpadReader())
			pInfo->flags = CKF_PROTECTED_AUTHENTICATION_PATH;
		pInfo->firmwareVersion.major = poCard->GetAppletVersion();
	}
	catch(CMWException &e)
	{
		return (cal_translate_error(WHERE, e.GetError()));
	}
	catch( ...)
	{
		log_trace(WHERE, "E: unkown exception thrown");
		return (CKR_FUNCTION_FAILED);
	}

	strcpy_n(pInfo->manufacturerID, "Belgium Government", 32, ' ');
	strcpy_n(pInfo->model, "Belgium eID", 16, ' ');
	// strcpy_n(pInfo->label, "Belgian eID", 16, ' '); // FRANK TEST

	/* Take the last 16 chars of the serial number (if the are more then 16).
	   _Assuming_ that the serial number is a Big Endian counter, this will
	   assure that the serial within each type of card will be unique in pkcs11
	   (at least for the first 16^16 cards :-) */
	//if (sn_start < 0)
	//   sn_start = 0;

	pInfo->ulMaxSessionCount = MAX_SESSIONS;	//CK_EFFECTIVELY_INFINITE;
	pInfo->ulSessionCount = pSlot->nsessions;
	pInfo->ulMaxRwSessionCount = MAX_RW_SESSIONS;
	pInfo->ulRwSessionCount = 0;	/* FIXME */
	pInfo->ulTotalPublicMemory = CK_UNAVAILABLE_INFORMATION;
	pInfo->ulFreePublicMemory = CK_UNAVAILABLE_INFORMATION;
	pInfo->ulTotalPrivateMemory = CK_UNAVAILABLE_INFORMATION;
	pInfo->ulFreePrivateMemory = CK_UNAVAILABLE_INFORMATION;
	pInfo->hardwareVersion.major = 1;
	pInfo->hardwareVersion.minor = 0;

	pInfo->ulMaxPinLen = 12;
	pInfo->ulMinPinLen = 4;
	strcpy_s((char *) pInfo->utcTime, sizeof(pInfo->utcTime),
		 "20080101000000");

	pInfo->flags |= CKF_WRITE_PROTECTED | CKF_TOKEN_INITIALIZED | CKF_USER_PIN_INITIALIZED;	// check for pin change capabilitypInfo->flags |= /*CKF_LOGIN_REQUIRED |  CKF_USER_PIN_INITIALIZED |*/; //CAL does logon, so no CKF_LOGIN_REQUIRED nor CKF_USER_PIN_INITIALIZED
#ifdef NO_DIALOGS
	pInfo->flags |= CKF_LOGIN_REQUIRED;	// no dialogs, so we ask the calling program to ask for PIN
#endif
      cleanup:

	return (ret);
}

#undef WHERE




#define WHERE "cal_get_mechanism_list()"
CK_RV cal_get_mechanism_list(CK_SLOT_ID hSlot,
			     CK_MECHANISM_TYPE_PTR pMechanismList,
			     CK_ULONG_PTR pulCount)
{
	CK_RV ret = CKR_OK;
	int status;
	P11_SLOT *pSlot = NULL;

	pSlot = p11_get_slot(hSlot);
	if (pSlot == NULL)
	{
		log_trace(WHERE, "E: Invalid slot (%lu)", hSlot);
		return (CKR_SLOT_ID_INVALID);
	}

	std::string szReader = pSlot->name;
	unsigned long algos = 0;
	unsigned int n = 0;

	ret = cal_update_token(hSlot, &status, 0);
	if (ret != CKR_OK)
	{
		return (ret);
	}

	if ((status == P11_CARD_REMOVED) || (status == P11_CARD_NOT_PRESENT))
	{
		return (CKR_TOKEN_NOT_PRESENT);
	}

	try
	{
		CReader & oReader = oCardLayer->getReader(szReader);
		CCard* poCard = oReader.GetCard();
		algos = poCard->GetCardSupportedAlgorithms();
	}
	catch(CMWException &e)
	{
		return (cal_translate_error(WHERE, e.GetError()));
	}
	catch( ...)
	{
		log_trace(WHERE, "E: unkown exception thrown");
		return (CKR_FUNCTION_FAILED);
	}

	if (pMechanismList == NULL)
	{
		*pulCount = 6;	//for 6 hash algos

		if (algos & SIGN_ALGO_RSA_PKCS)
			*pulCount += 1;
		if (algos & SIGN_ALGO_MD5_RSA_PKCS)
			*pulCount += 1;
		if (algos & SIGN_ALGO_SHA1_RSA_PKCS)
			*pulCount += 1;
		if (algos & SIGN_ALGO_SHA256_RSA_PKCS)
			*pulCount += 1;
		if (algos & SIGN_ALGO_SHA384_RSA_PKCS)
			*pulCount += 1;
		if (algos & SIGN_ALGO_SHA512_RSA_PKCS)
			*pulCount += 1;
		if (algos & SIGN_ALGO_RIPEMD160_RSA_PKCS)
			*pulCount += 1;
		if (algos & SIGN_ALGO_SHA1_RSA_PSS)
			*pulCount += 1;
		if (algos & SIGN_ALGO_SHA256_RSA_PSS)
			*pulCount += 1;
		if (algos & SIGN_ALGO_SHA256_ECDSA)
			*pulCount += 1;
		if (algos & SIGN_ALGO_SHA384_ECDSA)
			*pulCount += 1;
		if (algos & SIGN_ALGO_SHA512_ECDSA)
			*pulCount += 1;
		if (algos & SIGN_ALGO_ECDSA_RAW)
			*pulCount += 1;
		// TODO: also add SHA3 mechanisms -- PKCS#11 v2.40 does not yet support those, though; PKCS#11 v3 will, but is not released yet.
		return (CKR_OK);
	}

	/* hash algos */
	if (n++ < *pulCount)
		pMechanismList[n - 1] = CKM_MD5;
	else
		return (CKR_BUFFER_TOO_SMALL);

	if (n++ < *pulCount)
		pMechanismList[n - 1] = CKM_SHA_1;
	else
		return (CKR_BUFFER_TOO_SMALL);

	if (n++ < *pulCount)
		pMechanismList[n - 1] = CKM_SHA256;
	else
		return (CKR_BUFFER_TOO_SMALL);

	if (n++ < *pulCount)
		pMechanismList[n - 1] = CKM_SHA384;
	else
		return (CKR_BUFFER_TOO_SMALL);

	if (n++ < *pulCount)
		pMechanismList[n - 1] = CKM_SHA512;
	else
		return (CKR_BUFFER_TOO_SMALL);

	if (n++ < *pulCount)
		pMechanismList[n - 1] = CKM_RIPEMD160;
	else
		return (CKR_BUFFER_TOO_SMALL);

	/* sign algos */
	if (algos & SIGN_ALGO_RSA_PKCS)
	{
		if (n++ < *pulCount)
			pMechanismList[n - 1] = CKM_RSA_PKCS;
		else
			return (CKR_BUFFER_TOO_SMALL);
	}
	if (algos & SIGN_ALGO_MD5_RSA_PKCS)
	{
		if (n++ < *pulCount)
			pMechanismList[n - 1] = CKM_MD5_RSA_PKCS;
		else
			return (CKR_BUFFER_TOO_SMALL);
	}
	if (algos & SIGN_ALGO_SHA1_RSA_PKCS)
	{
		if (n++ < *pulCount)
			pMechanismList[n - 1] = CKM_SHA1_RSA_PKCS;
		else
			return (CKR_BUFFER_TOO_SMALL);
	}
	if (algos & SIGN_ALGO_SHA256_RSA_PKCS)
	{
		if (n++ < *pulCount)
			pMechanismList[n - 1] = CKM_SHA256_RSA_PKCS;
		else
			return (CKR_BUFFER_TOO_SMALL);
	}
	if (algos & SIGN_ALGO_SHA384_RSA_PKCS)
	{
		if (n++ < *pulCount)
			pMechanismList[n - 1] = CKM_SHA384_RSA_PKCS;
		else
			return (CKR_BUFFER_TOO_SMALL);
	}
	if (algos & SIGN_ALGO_SHA512_RSA_PKCS)
	{
		if (n++ < *pulCount)
			pMechanismList[n - 1] = CKM_SHA512_RSA_PKCS;
		else
			return (CKR_BUFFER_TOO_SMALL);
	}
	if (algos & SIGN_ALGO_RIPEMD160_RSA_PKCS)
	{
		if (n++ < *pulCount)
			pMechanismList[n - 1] = CKM_RIPEMD160_RSA_PKCS;
		else
			return (CKR_BUFFER_TOO_SMALL);
	}
	if (algos & SIGN_ALGO_SHA1_RSA_PSS)
	{
		if (n++ < *pulCount)
			pMechanismList[n - 1] = CKM_SHA1_RSA_PKCS_PSS;
		else
			return (CKR_BUFFER_TOO_SMALL);
	}
	if (algos & SIGN_ALGO_SHA256_RSA_PSS)
	{
		if (n++ < *pulCount)
			pMechanismList[n - 1] = CKM_SHA256_RSA_PKCS_PSS;
		else
			return (CKR_BUFFER_TOO_SMALL);
	}
	if (algos & SIGN_ALGO_SHA256_ECDSA)
	{
		if (n++ < *pulCount)
			pMechanismList[n - 1] = CKM_ECDSA_SHA256;
		else
			return (CKR_BUFFER_TOO_SMALL);
	}
	if (algos & SIGN_ALGO_SHA384_ECDSA)
	{
		if (n++ < *pulCount)
			pMechanismList[n - 1] = CKM_ECDSA_SHA384;
		else
			return (CKR_BUFFER_TOO_SMALL);
	}
	if (algos & SIGN_ALGO_SHA512_ECDSA)
	{
		if (n++ < *pulCount)
			pMechanismList[n - 1] = CKM_ECDSA_SHA512;
		else
			return (CKR_BUFFER_TOO_SMALL);
	}
	if (algos & SIGN_ALGO_ECDSA_RAW)
	{
		if (n++ < *pulCount)
			pMechanismList[n - 1] = CKM_ECDSA;
		else
			return (CKR_BUFFER_TOO_SMALL);
	}
	return (ret);
}

#undef WHERE



#define WHERE "cal_get_mechanism_info()"
CK_RV cal_get_mechanism_info(CK_SLOT_ID hSlot, CK_MECHANISM_TYPE type,
			     CK_MECHANISM_INFO_PTR pInfo)
{
	CK_RV ret = CKR_OK;
	P11_MECHANISM_INFO table[] = CAL_MECHANISM_TABLE;
	P11_MECHANISM_INFO *info = NULL;
	int status;
	int i;

	if (pInfo == NULL_PTR)
	{
		ret = CKR_ARGUMENTS_BAD;
		goto cleanup;
	}
	//look for type in table
	for (i = 0; i < (int)(sizeof(table) / sizeof(table[0])); i++)
	{
		if (table[i].type == type)
		{
			info = &table[i];
			break;
		}
	}

	if ((info) && (info->type))
	{
		if (info->flags & CKF_SIGN)
		{
			ret = cal_update_token(hSlot, &status, 0);
			if (ret != CKR_OK)
			{
				return (ret);
			}

			if ((status == P11_CARD_REMOVED) || (status == P11_CARD_NOT_PRESENT))
			{
				return (CKR_TOKEN_NOT_PRESENT);
			}

			try
			{
				P11_SLOT *pSlot = p11_get_slot(hSlot);

				if (pSlot == NULL)
				{
					log_trace(WHERE, "E: Invalid slot(%lu)", hSlot);
					return (CKR_SLOT_ID_INVALID);
				}
				std::string szReader = pSlot->name;

				CReader & oReader = oCardLayer->getReader(szReader);
				CCard* poCard = oReader.GetCard();
				pInfo->ulMinKeySize = pInfo->ulMaxKeySize = (CK_ULONG)poCard->GetPrivKeySize();
			}
			catch(CMWException &e)
			{
				return (cal_translate_error
					(WHERE, e.GetError()));
			}
			catch( ...)
			{
				log_trace(WHERE, "E: unknown exception thrown");
				return (CKR_FUNCTION_FAILED);
			}
		} else
		{
			pInfo->ulMinKeySize = info->ulMinKeySize;
			pInfo->ulMaxKeySize = info->ulMaxKeySize;
		}
		pInfo->flags = info->flags;
	} else
		ret = CKR_MECHANISM_INVALID;

      cleanup:

	return (ret);
}

#undef WHERE




#define WHERE "cal_connect()"
CK_RV cal_connect(CK_SLOT_ID hSlot)
{
	CK_RV ret = CKR_OK;
	int status;
	P11_SLOT *pSlot = NULL;

	//connect to token
	ret = cal_update_token(hSlot, &status, 0);
	if (ret != CKR_OK)
		goto cleanup;

	if ((status == P11_CARD_REMOVED) || (status == P11_CARD_NOT_PRESENT))
	{
		ret = CKR_TOKEN_NOT_PRESENT;
		goto cleanup;
	}

	pSlot = p11_get_slot(hSlot);
	if (pSlot == NULL)
	{
		log_trace(WHERE, "E: Invalid slot (%lu)", hSlot);
		return (CKR_SLOT_ID_INVALID);
	}

	pSlot->connect++;

      cleanup:

	return (ret);
}

#undef WHERE



#define WHERE "cal_disconnect()"
CK_RV cal_disconnect(CK_SLOT_ID hSlot)
{
	CK_RV ret = 0;
	P11_SLOT *pSlot = NULL;

	pSlot = p11_get_slot(hSlot);
	if (pSlot == NULL)
	{
		log_trace(WHERE, "E: Invalid slot (%lu)", hSlot);
		return (CKR_SLOT_ID_INVALID);
	}

	if (pSlot->connect > 0)
		pSlot->connect--;

	if (pSlot->connect < 1)
	{
		pSlot->connect = 0;
		std::string szreader = pSlot->name;
		try
		{
			CReader & oReader = oCardLayer->getReader(szreader);
			oReader.Disconnect();
		}
		catch(CMWException &e)
		{
			return (cal_translate_error(WHERE, e.GetError()));
		}
		catch( ...)
		{
			log_trace(WHERE, "E: unkown exception thrown");
			return (CKR_FUNCTION_FAILED);
		}
	}
	return ret;
}

#undef WHERE


#define WHERE "cal_init_objects()"
CK_RV cal_init_objects(P11_SLOT * pSlot)
{
	CK_RV ret = CKR_OK;
	CK_ATTRIBUTE PRV_KEY_RSA[] = BEID_TEMPLATE_PRV_KEY_RSA;
	CK_ATTRIBUTE PUB_KEY_RSA[] = BEID_TEMPLATE_PUB_KEY_RSA;
	CK_ATTRIBUTE PRV_KEY_EC[] = BEID_TEMPLATE_PRV_KEY_EC;
	CK_ATTRIBUTE PUB_KEY_EC[] = BEID_TEMPLATE_PUB_KEY_EC;
	CK_ATTRIBUTE CERTIFICATE[] = BEID_TEMPLATE_CERTIFICATE;
	CK_ULONG hObject = 0;
	P11_OBJECT *pObject = NULL;
	CK_KEY_TYPE keytype = CKK_RSA;
	CK_BBOOL btrue = CK_TRUE;
	CK_BBOOL bfalse = CK_FALSE;
	CK_ULONG modsize = 0;	/* TODO read from pkcs15 */
	CK_ULONG CertId = 0;
	CK_ULONG KeyId = 0;
	unsigned int certCounter = 0;
	unsigned int keyCounter = 0;
	char clabel[128];
	CK_CERTIFICATE_TYPE certType = CKC_X_509;

	//check if the object list is initialized, and if so, return with OK
	if (pSlot->ulCardDataCached & CACHED_DATA_TYPE_CDF)
		return CKR_OK;

	//this function will initialize objects as they are valid for the token
	//this function does not read the actual values but enables an application to
	//search for an attribute
	//attributes are only read from the token as needed and thereafter they are cached
	//they remain valid as long as the connection with the smartcard remains valid
	//an application might check the PCKS#15 on the card to initialize the PKCS#11 objects here
	//or can decide to do this statically

	//mapping between these PKCS11 objects and "real" beid objects is done through CLASS type and ID
	// e.g. CKA_CLASS=CKO_PRIVATE_KEY and CK_ID=0   => this object is representative for the authentication key on the card.
	//      CKA_CLASS=CKO_PUBKEY and CK_ID=0        => this object is represents the public key found in the authentication certificate
	//                                                 to read this public key, we read the certificate and extract the public key components

	//set attribute template, CKA_TOKEN to true, fill CKA_CLASS type, ID value and CKA_PRIVATE flag

	std::string szReader = pSlot->name;
	try
	{
		CReader & oReader = oCardLayer->getReader(szReader);
		CCard* poCard = oReader.GetCard();

		// add all certificate objects from card 
		for (certCounter = 0; certCounter < poCard->CertCount(); certCounter++)
		{
			CertId = (CK_ULONG)poCard->GetCert(certCounter).ulID;

			sprintf_s(clabel, sizeof(clabel), "%s", poCard->GetCert(certCounter).csLabel.c_str());

			ret = p11_add_slot_object(pSlot, CERTIFICATE, sizeof(CERTIFICATE) / sizeof(CK_ATTRIBUTE), CK_TRUE, CKO_CERTIFICATE, CertId, CK_FALSE, &hObject);
			if (ret != CKR_OK)
				goto cleanup;
			pObject = p11_get_slot_object(pSlot, hObject);

			ret = p11_set_attribute_value(pObject->pAttr, pObject->count, CKA_CERTIFICATE_TYPE, (CK_VOID_PTR)& certType, sizeof(CK_ULONG));
			if (ret != CKR_OK)
				goto cleanup;
			ret = p11_set_attribute_value(pObject->pAttr, pObject->count, CKA_LABEL, (CK_VOID_PTR)clabel, (CK_ULONG)strlen(clabel));
			if (ret != CKR_OK)
				goto cleanup;
		}

		// add all key objects from card 
		// also add keys without a matching certificate (i.e. the card key used to authenticate the v1.8 EC card)
		for (keyCounter = 0; keyCounter < poCard->PrivKeyCount(); keyCounter++)
		{
			/***************/
			/* Private key */
			/***************/
			tPrivKey key = poCard->GetPrivKey(keyCounter);
			keytype = (key.keyType == RSA ? CKK_RSA : CKK_EC);

			KeyId = (CK_ULONG)key.ulID;

			sprintf_s(clabel, sizeof(clabel), "%s", key.csLabel.c_str());

			if (key.keyType == RSA) {
				ret = p11_add_slot_object(pSlot, PRV_KEY_RSA, sizeof(PRV_KEY_RSA) / sizeof(CK_ATTRIBUTE), CK_TRUE, CKO_PRIVATE_KEY, KeyId, CK_TRUE, &hObject);
				if (ret != CKR_OK)
					goto cleanup;
			}
			else if (key.keyType == EC) {
				ret = p11_add_slot_object(pSlot, PRV_KEY_EC, sizeof(PRV_KEY_EC) / sizeof(CK_ATTRIBUTE), CK_TRUE, CKO_PRIVATE_KEY, KeyId, CK_TRUE, &hObject);
				if (ret != CKR_OK)
					goto cleanup;
			}
			else {
				//skip this key, we only support RSA and EC
				continue;
			}

			//put some other attribute items allready so the key can be used for signing or challenging
			pObject = p11_get_slot_object(pSlot, hObject);

			ret = p11_set_attribute_value(pObject->pAttr, pObject->count, CKA_LABEL, (CK_VOID_PTR)clabel, (CK_ULONG)strlen(clabel));
			if (ret != CKR_OK)
				goto cleanup;

			ret = p11_set_attribute_value(pObject->pAttr, pObject->count, CKA_KEY_TYPE, (CK_VOID_PTR)& keytype, sizeof(CK_KEY_TYPE));
			if (ret != CKR_OK)
				goto cleanup;

			//TODO if (ulKeyUsage & SIGN)
			{
				ret = p11_set_attribute_value(pObject->pAttr, pObject->count, CKA_SIGN, (CK_VOID_PTR)& btrue, sizeof(btrue));
				if (ret != CKR_OK)
					goto cleanup;
			}

			//TODO error in cal, size is in bits allready
			if (key.keyType == RSA) {
				modsize = key.ulKeyLenBytes * 8;
				ret = p11_set_attribute_value(pObject->pAttr, pObject->count, CKA_MODULUS_BITS, (CK_VOID_PTR)& modsize, sizeof(CK_ULONG));
				if (ret != CKR_OK)
					goto cleanup;
			}
			//in case keyType == EC, do not pre-fill the CKA_EC_PARAMS with { 0x06, 0x05, 0x2b, 0x81, 0x04, 0x00, 0x22 }
			//we'll read the EC curve from the matching certificate later
			/*else { // if (key.keyType == EC)
				unsigned char ECCurve[] = { 0x06, 0x05, 0x2b, 0x81, 0x04, 0x00, 0x22 };
				ret = p11_set_attribute_value(pObject->pAttr, pObject->count, CKA_EC_PARAMS, (CK_VOID_PTR)& ECCurve, sizeof(ECCurve));
				if (ret != CKR_OK)
					goto cleanup;
			}*/
			ret = p11_set_attribute_value(pObject->pAttr, pObject->count, CKA_EXTRACTABLE, (CK_VOID_PTR)& bfalse, sizeof(bfalse));
			if (ret != CKR_OK)
				goto cleanup;
			ret = p11_set_attribute_value(pObject->pAttr, pObject->count, CKA_DERIVE, (CK_VOID_PTR)& bfalse, sizeof(bfalse));
			if (ret != CKR_OK)
				goto cleanup;

			/**************************************************/
			/* Public key corresponding to private key object */

			/**************************************************/

			if (key.keyType == RSA) {
				ret = p11_add_slot_object(pSlot, PUB_KEY_RSA, sizeof(PUB_KEY_RSA) / sizeof(CK_ATTRIBUTE), CK_TRUE, CKO_PUBLIC_KEY, KeyId, CK_FALSE, &hObject);
				if (ret != CKR_OK)
					goto cleanup;
			}
			else { //if (key.keyType == EC) 
				ret = p11_add_slot_object(pSlot, PUB_KEY_EC, sizeof(PUB_KEY_EC) / sizeof(CK_ATTRIBUTE), CK_TRUE, CKO_PUBLIC_KEY, KeyId, CK_FALSE, &hObject);
				if (ret != CKR_OK)
					goto cleanup;
			}

			pObject = p11_get_slot_object(pSlot, hObject);

			sprintf_s(clabel, sizeof(clabel), "%s", key.csLabel.c_str());
			ret = p11_set_attribute_value(pObject->pAttr, pObject->count, CKA_LABEL, (CK_VOID_PTR)clabel, (CK_ULONG)strlen(clabel));
			if (ret != CKR_OK)
				goto cleanup;
			ret = p11_set_attribute_value(pObject->pAttr, pObject->count, CKA_KEY_TYPE, (CK_VOID_PTR)& keytype, sizeof(CK_KEY_TYPE));
			if (ret != CKR_OK)
				goto cleanup;
			if (key.keyType == RSA) {
				ret = p11_set_attribute_value(pObject->pAttr, pObject->count, CKA_MODULUS_BITS, (CK_VOID_PTR)& modsize, sizeof(CK_ULONG));
				if (ret != CKR_OK)
					goto cleanup;
			}
			//in case keyType == EC, do not pre-fill the CKA_EC_PARAMS with { 0x06, 0x05, 0x2b, 0x81, 0x04, 0x00, 0x22 }
			//we'll read the EC curve from the matching certificate later
			/*else {// if (key.keyType == EC)
				unsigned char ECCurve[] = { 0x06, 0x05, 0x2b, 0x81, 0x04, 0x00, 0x22 };
				ret = p11_set_attribute_value(pObject->pAttr, pObject->count, CKA_EC_PARAMS, (CK_VOID_PTR)& ECCurve, sizeof(ECCurve));
				if (ret != CKR_OK)
					goto cleanup;
			}*/
			ret = p11_set_attribute_value(pObject->pAttr, pObject->count, CKA_DERIVE, (CK_VOID_PTR)& bfalse, sizeof(bfalse));
			if (ret != CKR_OK)
				goto cleanup;
			//}
		}
	}
	catch(CMWException &e)
	{
		return (cal_translate_error(WHERE, e.GetError()));
	}
	catch( ...)
	{
		log_trace(WHERE, "E: unkown exception thrown");
		return (CKR_FUNCTION_FAILED);
	}

      cleanup:
	pSlot->ulCardDataCached |= CACHED_DATA_TYPE_CDF;
	return (ret);
}

#undef WHERE



#define WHERE "cal_logon()"
CK_RV cal_logon(CK_SLOT_ID hSlot, size_t l_pin, CK_CHAR_PTR pin,
		int sec_messaging)
{
	CK_RV ret = CKR_OK;
	char cpin[20];
	P11_SLOT *pSlot = NULL;

	pSlot = p11_get_slot(hSlot);
	if (pSlot == NULL)
	{
		log_trace(WHERE, "E: Invalid slot (%lu)", hSlot);
		return (CKR_SLOT_ID_INVALID);
	}

	std::string szReader = pSlot->name;

	memset(cpin, 0, sizeof(cpin));
	if (pin && (l_pin < sizeof(cpin)))
		memcpy(cpin, pin, l_pin);

	std::string csPin = cpin;
	unsigned long ulRemaining = 0;

	try
	{
		CReader& oReader = oCardLayer->getReader(szReader);
		CCard* poCard = oReader.GetCard();

		if (!poCard->PinCmd(PIN_OP_VERIFY, PinBeid, csPin, "", ulRemaining))
		{
			if (ulRemaining == 0)
				ret = CKR_PIN_LOCKED;
			else
				ret = CKR_PIN_INCORRECT;
		}
	}
	catch(CMWException &e)
	{
		return (cal_translate_error(WHERE, e.GetError()));
	}
	catch( ...)
	{
		log_trace(WHERE, "E: unkown exception thrown");
		return (CKR_FUNCTION_FAILED);
	}

	return (ret);
}

#undef WHERE


#define WHERE "cal_logout()"
CK_RV cal_logout(CK_SLOT_ID hSlot)
{
	CK_RV ret = CKR_OK;
	P11_SLOT *pSlot = NULL;

	pSlot = p11_get_slot(hSlot);
	if (pSlot == NULL)
	{
		log_trace(WHERE, "E: Invalid slot (%lu)", hSlot);
		return (CKR_SLOT_ID_INVALID);
	}
	
	try
	{
		std::string szReader = pSlot->name;
		CReader &oReader = oCardLayer->getReader(szReader);
		CCard* poCard = oReader.GetCard();

		if (!poCard->LogOff())
		{
			//can only get here if we're not a BEID card
			log_trace(WHERE, "E: PIN_OP_LOGOFF failed");
			ret = CKR_FUNCTION_FAILED;
		}
	}
	catch (CMWException &e)
	{
		return (cal_translate_error(WHERE, e.GetError()));
	}
	catch (...)
	{
		log_trace(WHERE, "E: unkown exception thrown");
		return (CKR_FUNCTION_FAILED);
	}
	return (ret);
}

#undef WHERE




#define WHERE "cal_change_pin()"
CK_RV cal_change_pin(CK_SLOT_ID hSlot, CK_ULONG pinref, CK_ULONG l_oldpin, CK_CHAR_PTR oldpin,
		     CK_ULONG l_newpin, CK_CHAR_PTR newpin)
{
	CK_RV ret = CKR_OK;
	P11_SLOT *pSlot = NULL;

	pSlot = p11_get_slot(hSlot);
	if (pSlot == NULL)
	{
		log_trace(WHERE, "E: Invalid slot (%lu)", hSlot);
		return (CKR_SLOT_ID_INVALID);
	}

	try
	{
		std::string csPin = "";
		std::string csNewPin = "";
		std::string szReader = pSlot->name;

		CReader & oReader = oCardLayer->getReader(szReader);
		CCard* poCard = oReader.GetCard();

		if (oldpin != NULL)
		{
			csPin = (char *) oldpin;
		}
		if (newpin != NULL)
		{
			csNewPin = (char *) newpin;
		}
		unsigned long ulRemaining = 0;

		if (!(poCard->PinCmd(PIN_OP_CHANGE, PinBeid, csPin, csNewPin, ulRemaining)))
		{
			if (ulRemaining == 0)
				ret = CKR_PIN_LOCKED;
			else
				ret = CKR_PIN_INCORRECT;
		}
	}
	catch(CMWException &e)
	{
		return (cal_translate_error(WHERE, e.GetError()));
	}
	catch( ...)
	{
		log_trace(WHERE, "E: unkown exception thrown");
		return (CKR_FUNCTION_FAILED);
	}

	return (ret);
}

#undef WHERE




#define WHERE "cal_get_card_data()"
//we already know the unsigned data
CK_RV cal_get_card_data(CK_SLOT_ID hSlot)
{
	CK_RV ret = 0;
	CByteArray oATR;
	CByteArray oAPDU(5);
	unsigned char oByte;
	CByteArray oCardData;

	std::string szReader;
	//      char cBuffer[250];
	//      unsigned char ucBuffer[250];
	const char *plabel = NULL;
	CTLVBuffer oTLVBuffer;
	P11_SLOT *pSlot = NULL;
	CK_ATTRIBUTE ID_DATA[] = BEID_TEMPLATE_ID_DATA;

	CK_ULONG hObject = 0;

	pSlot = p11_get_slot(hSlot);
	if (pSlot == NULL)
	{
		log_trace(WHERE, "E: Invalid slot (%lu)", hSlot);
		return (CKR_SLOT_ID_INVALID);
	}

	szReader = pSlot->name;
	try
	{
		CReader& oReader = oCardLayer->getReader(szReader);
		CCard* poCard = oReader.GetCard();

		oATR = poCard->GetATR();
		oCardData = poCard->GetInfo();

		plabel = BEID_LABEL_ATR;
		ret = p11_add_slot_ID_object(pSlot, ID_DATA,
					     sizeof(ID_DATA) /
					     sizeof(CK_ATTRIBUTE), CK_TRUE,
					     CKO_DATA, CK_FALSE, &hObject,
					     (CK_VOID_PTR) plabel,
					     (CK_ULONG) strlen(plabel),
					     (CK_VOID_PTR) oATR.GetBytes(),
					     (CK_ULONG) oATR.Size(),
					     (CK_VOID_PTR)
					     BEID_OBJECTID_CARDDATA,
					     (CK_ULONG)
					     strlen(BEID_OBJECTID_CARDDATA),
						 CK_FALSE);
		if (ret != CKR_OK)
			goto cleanup;

		plabel = BEID_LABEL_CARD_DATA;
		ret = p11_add_slot_ID_object(pSlot, ID_DATA,
					     sizeof(ID_DATA) /
					     sizeof(CK_ATTRIBUTE), CK_TRUE,
					     CKO_DATA, CK_FALSE, &hObject,
					     (CK_VOID_PTR) plabel,
					     (CK_ULONG) strlen(plabel),
					     (CK_VOID_PTR) oCardData.
					     GetBytes(),
					     (CK_ULONG) oCardData.Size(),
					     (CK_VOID_PTR)
					     BEID_OBJECTID_CARDDATA,
					     (CK_ULONG)
					     strlen(BEID_OBJECTID_CARDDATA),
						 CK_FALSE);
		if (ret != CKR_OK)
			goto cleanup;

		CByteArray data = CByteArray(oCardData.GetBytes(), 16);

		plabel = BEID_LABEL_DATA_SerialNr;
		ret = p11_add_slot_ID_object(pSlot, ID_DATA,
					     sizeof(ID_DATA) /
					     sizeof(CK_ATTRIBUTE), CK_TRUE,
					     CKO_DATA, CK_FALSE, &hObject,
					     (CK_VOID_PTR) plabel,
					     (CK_ULONG) strlen(plabel),
					     (CK_VOID_PTR) data.GetBytes(),
					     (CK_ULONG) data.Size(),
					     (CK_VOID_PTR)
					     BEID_OBJECTID_CARDDATA,
					     (CK_ULONG)
					     strlen(BEID_OBJECTID_CARDDATA),
						 CK_FALSE);
		if (ret != CKR_OK)
			goto cleanup;

		oByte = oCardData.GetByte(16);
		plabel = BEID_LABEL_DATA_CompCode;
		ret = p11_add_slot_ID_object(pSlot, ID_DATA,
					     sizeof(ID_DATA) /
					     sizeof(CK_ATTRIBUTE), CK_TRUE,
					     CKO_DATA, CK_FALSE, &hObject,
					     (CK_VOID_PTR) plabel,
					     (CK_ULONG) strlen(plabel),
					     (CK_VOID_PTR) & oByte,
					     (CK_ULONG) 1,
					     (CK_VOID_PTR)
					     BEID_OBJECTID_CARDDATA,
					     (CK_ULONG)
					     strlen(BEID_OBJECTID_CARDDATA),
						 CK_FALSE);
		if (ret != CKR_OK)
			goto cleanup;

		oByte = oCardData.GetByte(17);
		plabel = BEID_LABEL_DATA_OSNr;
		ret = p11_add_slot_ID_object(pSlot, ID_DATA,
					     sizeof(ID_DATA) /
					     sizeof(CK_ATTRIBUTE), CK_TRUE,
					     CKO_DATA, CK_FALSE, &hObject,
					     (CK_VOID_PTR) plabel,
					     (CK_ULONG) strlen(plabel),
					     (CK_VOID_PTR) & oByte,
					     (CK_ULONG) 1,
					     (CK_VOID_PTR)
					     BEID_OBJECTID_CARDDATA,
					     (CK_ULONG)
					     strlen(BEID_OBJECTID_CARDDATA),
						 CK_FALSE);
		if (ret != CKR_OK)
			goto cleanup;

		oByte = oCardData.GetByte(18);
		plabel = BEID_LABEL_DATA_OSVersion;
		ret = p11_add_slot_ID_object(pSlot, ID_DATA,
					     sizeof(ID_DATA) /
					     sizeof(CK_ATTRIBUTE), CK_TRUE,
					     CKO_DATA, CK_FALSE, &hObject,
					     (CK_VOID_PTR) plabel,
					     (CK_ULONG) strlen(plabel),
					     (CK_VOID_PTR) & oByte,
					     (CK_ULONG) 1,
					     (CK_VOID_PTR)
					     BEID_OBJECTID_CARDDATA,
					     (CK_ULONG)
					     strlen(BEID_OBJECTID_CARDDATA),
						 CK_FALSE);
		if (ret != CKR_OK)
			goto cleanup;

		oByte = oCardData.GetByte(19);
		plabel = BEID_LABEL_DATA_SoftMaskNumber;
		ret = p11_add_slot_ID_object(pSlot, ID_DATA,
					     sizeof(ID_DATA) /
					     sizeof(CK_ATTRIBUTE), CK_TRUE,
					     CKO_DATA, CK_FALSE, &hObject,
					     (CK_VOID_PTR) plabel,
					     (CK_ULONG) strlen(plabel),
					     (CK_VOID_PTR) & oByte,
					     (CK_ULONG) 1,
					     (CK_VOID_PTR)
					     BEID_OBJECTID_CARDDATA,
					     (CK_ULONG)
					     strlen(BEID_OBJECTID_CARDDATA),
						 CK_FALSE);
		if (ret != CKR_OK)
			goto cleanup;

		oByte = oCardData.GetByte(20);
		plabel = BEID_LABEL_DATA_SoftMaskVersion;
		ret = p11_add_slot_ID_object(pSlot, ID_DATA,
					     sizeof(ID_DATA) /
					     sizeof(CK_ATTRIBUTE), CK_TRUE,
					     CKO_DATA, CK_FALSE, &hObject,
					     (CK_VOID_PTR) plabel,
					     (CK_ULONG) strlen(plabel),
					     (CK_VOID_PTR) & oByte,
					     (CK_ULONG) 1,
					     (CK_VOID_PTR)
					     BEID_OBJECTID_CARDDATA,
					     (CK_ULONG)
					     strlen(BEID_OBJECTID_CARDDATA),
						 CK_FALSE);
		if (ret != CKR_OK)
			goto cleanup;

		//spec of applet 1.7 is not compatible with that of applet 1.1,
		//so we check the appletversion here, and fill in the remaining fields according to it
		oByte = oCardData.GetByte(21);

		if (oByte == 0x11)
		{
			data.ClearContents();
			data.Append(oCardData.GetByte(21));
			data.Append(oCardData.GetByte(22));
			plabel = BEID_LABEL_DATA_ApplVersion;
			ret = p11_add_slot_ID_object(pSlot, ID_DATA,
						     sizeof(ID_DATA) / sizeof(CK_ATTRIBUTE),
						     CK_TRUE, CKO_DATA,
						     CK_FALSE, &hObject,
						     (CK_VOID_PTR) plabel,
						     (CK_ULONG) strlen(plabel),
						     (CK_VOID_PTR) data.GetBytes(),
						     (CK_ULONG) data.Size(),
						     (CK_VOID_PTR) BEID_OBJECTID_CARDDATA,
						     (CK_ULONG) strlen(BEID_OBJECTID_CARDDATA),
							 CK_FALSE);
			if (ret != CKR_OK)
				goto cleanup;

			data.ClearContents();
			data.Append(oCardData.GetByte(23));
			data.Append(oCardData.GetByte(24));
			plabel = BEID_LABEL_DATA_ApplIntVersion;
			ret = p11_add_slot_ID_object(pSlot, ID_DATA,
						     sizeof(ID_DATA) / sizeof(CK_ATTRIBUTE),
						     CK_TRUE, CKO_DATA,
						     CK_FALSE, &hObject,
						     (CK_VOID_PTR) plabel,
						     (CK_ULONG) strlen(plabel),
						     (CK_VOID_PTR) data.GetBytes(),
						     (CK_ULONG) data.Size(),
						     (CK_VOID_PTR) BEID_OBJECTID_CARDDATA,
						     (CK_ULONG) strlen(BEID_OBJECTID_CARDDATA),
							 CK_FALSE);
			if (ret != CKR_OK)
				goto cleanup;

			data.ClearContents();
			data.Append(oCardData.GetByte(25));
			data.Append(oCardData.GetByte(26));
			plabel = BEID_LABEL_DATA_PKCS15Version;
			ret = p11_add_slot_ID_object(pSlot, ID_DATA,
						     sizeof(ID_DATA) / sizeof(CK_ATTRIBUTE),
						     CK_TRUE, CKO_DATA,
						     CK_FALSE, &hObject,
						     (CK_VOID_PTR) plabel,
						     (CK_ULONG) strlen(plabel),
						     (CK_VOID_PTR) data.GetBytes(),
						     (CK_ULONG) data.Size(),
						     (CK_VOID_PTR) BEID_OBJECTID_CARDDATA,
						     (CK_ULONG) strlen(BEID_OBJECTID_CARDDATA),
							 CK_FALSE);
			if (ret != CKR_OK)
				goto cleanup;
		} else
		{
			plabel = BEID_LABEL_DATA_ApplVersion;
			ret = p11_add_slot_ID_object(pSlot, ID_DATA,
						     sizeof(ID_DATA) / sizeof(CK_ATTRIBUTE),
						     CK_TRUE, CKO_DATA,
						     CK_FALSE, &hObject,
						     (CK_VOID_PTR) plabel,
						     (CK_ULONG) strlen(plabel),
						     (CK_VOID_PTR) & oByte,
						     (CK_ULONG) 1,
						     (CK_VOID_PTR) BEID_OBJECTID_CARDDATA,
						     (CK_ULONG) strlen(BEID_OBJECTID_CARDDATA),
							 CK_FALSE);
			if (ret != CKR_OK)
				goto cleanup;

			data.ClearContents();
			data.Append(oCardData.GetByte(22));
			data.Append(oCardData.GetByte(23));
			plabel = BEID_LABEL_DATA_GlobOSVersion;
			ret = p11_add_slot_ID_object(pSlot, ID_DATA,
						     sizeof(ID_DATA) / sizeof(CK_ATTRIBUTE),
						     CK_TRUE, CKO_DATA,
						     CK_FALSE, &hObject,
						     (CK_VOID_PTR) plabel,
						     (CK_ULONG) strlen(plabel),
						     (CK_VOID_PTR) data.GetBytes(),
						     (CK_ULONG) data.Size(),
						     (CK_VOID_PTR) BEID_OBJECTID_CARDDATA,
						     (CK_ULONG) strlen(BEID_OBJECTID_CARDDATA),
							 CK_FALSE);
			if (ret != CKR_OK)
				goto cleanup;

			oByte = oCardData.GetByte(24);
			plabel = BEID_LABEL_DATA_ApplIntVersion;
			ret = p11_add_slot_ID_object(pSlot, ID_DATA,
						     sizeof(ID_DATA) / sizeof(CK_ATTRIBUTE),
						     CK_TRUE, CKO_DATA,
						     CK_FALSE, &hObject,
						     (CK_VOID_PTR) plabel,
						     (CK_ULONG) strlen(plabel),
						     (CK_VOID_PTR) & oByte,
						     (CK_ULONG) 1,
						     (CK_VOID_PTR) BEID_OBJECTID_CARDDATA,
						     (CK_ULONG) strlen(BEID_OBJECTID_CARDDATA),
							 CK_FALSE);
			if (ret != CKR_OK)
				goto cleanup;

			oByte = oCardData.GetByte(25);
			plabel = BEID_LABEL_DATA_PKCS1Support;
			ret = p11_add_slot_ID_object(pSlot, ID_DATA,
						     sizeof(ID_DATA) / sizeof(CK_ATTRIBUTE),
						     CK_TRUE, CKO_DATA,
						     CK_FALSE, &hObject,
						     (CK_VOID_PTR) plabel,
						     (CK_ULONG) strlen(plabel),
						     (CK_VOID_PTR) & oByte,
						     (CK_ULONG) 1,
						     (CK_VOID_PTR) BEID_OBJECTID_CARDDATA,
						     (CK_ULONG) strlen(BEID_OBJECTID_CARDDATA),
							 CK_FALSE);
			if (ret != CKR_OK)
				goto cleanup;

			oByte = oCardData.GetByte(26);
			plabel = BEID_LABEL_DATA_KeyExchangeVersion;
			ret = p11_add_slot_ID_object(pSlot, ID_DATA,
						     sizeof(ID_DATA) / sizeof(CK_ATTRIBUTE),
						     CK_TRUE, CKO_DATA,
						     CK_FALSE, &hObject,
						     (CK_VOID_PTR) plabel,
						     (CK_ULONG) strlen(plabel),
						     (CK_VOID_PTR) & oByte,
						     (CK_ULONG) 1,
						     (CK_VOID_PTR) BEID_OBJECTID_CARDDATA,
						     (CK_ULONG) strlen(BEID_OBJECTID_CARDDATA),
							 CK_FALSE);
			if (ret != CKR_OK)
				goto cleanup;
		}


		oByte = oCardData.GetByte(27);
		plabel = BEID_LABEL_DATA_ApplLifeCycle;
		ret = p11_add_slot_ID_object(pSlot, ID_DATA,
					     sizeof(ID_DATA) / sizeof(CK_ATTRIBUTE),
					     CK_TRUE, CKO_DATA,
					     CK_FALSE, &hObject,
					     (CK_VOID_PTR) plabel,
					     (CK_ULONG) strlen(plabel),
					     (CK_VOID_PTR) & oByte,
					     (CK_ULONG) 1,
					     (CK_VOID_PTR) BEID_OBJECTID_CARDDATA,
					     (CK_ULONG) strlen(BEID_OBJECTID_CARDDATA),
						 CK_FALSE);
		if (ret != CKR_OK)
			goto cleanup;

		if(oCardData.Size() > 30) {
			oByte = oCardData.GetByte(28);
			plabel = BEID_LABEL_DATA_PinAuth;
			ret = p11_add_slot_ID_object(pSlot, ID_DATA,
						     sizeof(ID_DATA) / sizeof(CK_ATTRIBUTE),
						     CK_TRUE, CKO_DATA,
						     CK_FALSE, &hObject,
						     (CK_VOID_PTR) plabel,
						     (CK_ULONG) strlen(plabel),
						     (CK_VOID_PTR) & oByte,
						     (CK_ULONG) 1,
						     (CK_VOID_PTR) BEID_OBJECTID_CARDDATA,
						     (CK_ULONG) strlen(BEID_OBJECTID_CARDDATA),
							 CK_FALSE);
			if (ret != CKR_OK)
				goto cleanup;
		}
		//              data.ClearContents();
		//              data = oCardData.GetBytes(28);
		//              plabel = BEID_LABEL_DATA_FILE;
		//              ret = p11_add_slot_ID_object(pSlot, ID_DATA, sizeof(ID_DATA)/sizeof(CK_ATTRIBUTE), CK_TRUE, CKO_DATA, 0, CK_FALSE, &hObject,
		//              (CK_VOID_PTR)plabel, (CK_ULONG)strlen(plabel),(CK_VOID_PTR) data.GetBytes(),(CK_ULONG)data.Size());
		//              if (ret) goto cleanup;
		//
	}
	catch(CMWException &e)
	{
		return (cal_translate_error(WHERE, e.GetError()));
	}
	catch( ...)
	{
		log_trace(WHERE, "E: unkown exception thrown");
		return (CKR_FUNCTION_FAILED);
	}

	if (ret != 0)
	{
		return (CKR_DEVICE_ERROR);
	}

      cleanup:
	return (ret);
}

#undef WHERE


#define WHERE "cal_read_and_store_file_records()"
CK_RV cal_read_and_store_record(P11_SLOT *pSlot, CK_ULONG ulDataType, CK_BYTE bRecordID, CK_UTF8CHAR* plabel, CK_ULONG ulLabelLen)
{
	CK_RV ret = CKR_OK;

	std::string szReader;
	CByteArray oRecordData;
	CK_ATTRIBUTE ID_DATA[] = BEID_TEMPLATE_ID_DATA;
	
	const char *pobjectID = BEID_OBJECTID_RECORD;
	CK_ULONG hObject = 0;
	unsigned char* pucBuffer = NULL;
	CK_ULONG ulBufferLen = 0;

	if (pSlot == NULL)
	{
		log_trace(WHERE, "E: Invalid slot pSlot == NULL");
		return (CKR_SLOT_ID_INVALID);
	}
	szReader = pSlot->name;
	try
	{
		CReader & oReader = oCardLayer->getReader(szReader);
		CCard* poCard = oReader.GetCard();
		switch (ulDataType)
		{
		case CACHED_DATA_TYPE_ID:
			oRecordData = poCard->ReadRecordFromFile(BEID_FILE_ID, bRecordID);
			break;
		case CACHED_DATA_TYPE_ADDRESS:
			oRecordData = poCard->ReadRecordFromFile(BEID_FILE_ADDRESS, bRecordID);
			break;
		default:
			//unsupported ulDataType given, not fatal, but also not useable
			log_trace(WHERE, "E: wrong ulDataType given (%lu), recordID = %x", ulDataType, bRecordID);
			return CKR_OK;
		}

		pucBuffer = oRecordData.GetBytes();
		ulBufferLen = oRecordData.Size();
		//parse TLV (one record, so i.e. just skip first 2 bytes (Type and Length are always 1 byte))
		//and store it into an object
		if (ulBufferLen > 2)
		{
			pucBuffer += 2;
			ulBufferLen -= 2;
		}
		else
		{
			//no data returned (at most the status words), so do not add a slot object
			ret = CKR_OK;
			goto cleanup;
		}

		ret = p11_add_slot_ID_object(pSlot, //the slot where to object belongs to
			ID_DATA, sizeof(ID_DATA) / sizeof(CK_ATTRIBUTE), //the object attribute template
			CK_TRUE,	//is this a token object?
			CKO_DATA,	//the object class
			CK_FALSE,	//is this a private object?
			&hObject,	//pointer to store the object's handle
			(CK_VOID_PTR)plabel, ulLabelLen,	//the CKA_LABEL attribute (e.g. "surname")
			(CK_VOID_PTR)pucBuffer, ulBufferLen,		//the value of the CKA_LABEL attribute (e.g. your surname as it is found on the token)
			(CK_VOID_PTR)pobjectID, (CK_ULONG)strlen(pobjectID), //the CKA_OBJECT_ID attribute
			CK_TRUE); //the hidden flag (hidden objects are only returned when searching for their Label)

		if (ret)
		{
			goto cleanup;
		}

		if (ret != 0)
		{
			return (CKR_DEVICE_ERROR);
		}

	cleanup:
		return (ret);


	}
	catch (CMWException &e)
	{
		return (cal_translate_error(WHERE, e.GetError()));
	}
	catch (...)
	{
		//ret = -1;
		log_trace(WHERE, "E: unknown exception thrown");
		return (CKR_FUNCTION_FAILED);
	}

}
#undef WHERE

#define WHERE "cal_read_ID_files()"
CK_RV cal_read_ID_files(CK_SLOT_ID hSlot, CK_ULONG dataType)
{
	CK_RV ret = CKR_OK;
	CByteArray oFileData;

	std::string szReader;
	char cBuffer[256];

	//      unsigned char ucBuffer[250];
	const char *plabel = NULL;
	const char *pobjectID = NULL;
	unsigned long ulLen = 0;
	CTLVBuffer oTLVBuffer;
	CTLVBuffer oTLVBufferAddress;	//need second buffer object, as memory is only freed when this object is destructed
	P11_SLOT *pSlot = NULL;
	CK_ATTRIBUTE ID_DATA[] = BEID_TEMPLATE_ID_DATA;
	BEID_DATA_LABELS_NAME ID_LABELS[] = BEID_ID_DATA_LABELS;
	BEID_DATA_LABELS_NAME ADDRESS_LABELS[] = BEID_ADDRESS_DATA_LABELS;
	int i = 0;
	int nrOfItems = 0;
	unsigned char ucAppletVersion = 0;

	CK_ULONG hObject = 0;

//	FILE* BEIDfile = fopen("F:\\idFile.dat", "rb");
//	BYTE buffer[4096];
//	int dataSize = 0;

	pSlot = p11_get_slot(hSlot);
	if (pSlot == NULL)
	{
		log_trace(WHERE, "E: Invalid slot (%lu)", hSlot);
		return (CKR_SLOT_ID_INVALID);
	}

	szReader = pSlot->name;
	try
	{
		CReader & oReader = oCardLayer->getReader(szReader);
		CCard* poCard = oReader.GetCard();
		switch (dataType)
		{
		case CACHED_DATA_TYPE_ALL_DATA:
		case CACHED_DATA_TYPE_ID:
			oFileData = poCard->ReadCardFile(BEID_FILE_ID);

			//				dataSize = fread((void *)buffer,1,4096, BEIDfile);
			//				fclose(BEIDfile);
			//				oFileData.Append(buffer, dataSize);

			plabel = BEID_LABEL_DATA_FILE;
			pobjectID = (char *)BEID_OBJECTID_ID;
			/* XXX the const-ness of pobjectID and plabel should
			 * ideally not be cast away, but it goes pretty deep.
			 * Not Now(TM). */
			ret = p11_add_slot_ID_object(pSlot, ID_DATA,
				sizeof(ID_DATA) / sizeof(CK_ATTRIBUTE),
				CK_TRUE,
				CKO_DATA,
				CK_FALSE,
				&hObject,
				(CK_VOID_PTR)plabel, (CK_ULONG)strlen(plabel),
				(CK_VOID_PTR)oFileData.GetBytes(), (CK_ULONG)oFileData.Size(),
				(CK_VOID_PTR)pobjectID, (CK_ULONG)strlen(pobjectID),
				CK_FALSE);
			if (ret)
				goto cleanup;

			oTLVBuffer.ParseTLV(oFileData.GetBytes(), oFileData.Size());

			nrOfItems = sizeof(ID_LABELS) / sizeof(BEID_DATA_LABELS_NAME);

			for (i = 0; i < nrOfItems; i++)
			{
				ulLen = sizeof(cBuffer);
				memset(cBuffer, 0, ulLen);
				if (oTLVBuffer.FillUTF8Data(ID_LABELS[i].tag, cBuffer, &ulLen)) {
					plabel = ID_LABELS[i].name;
					ret = p11_add_slot_ID_object(pSlot, ID_DATA, sizeof(ID_DATA) / sizeof(CK_ATTRIBUTE),
						CK_TRUE, CKO_DATA, CK_FALSE, &hObject, (CK_VOID_PTR)plabel,
						(CK_ULONG)strlen(plabel), (CK_VOID_PTR)cBuffer, ulLen,
						(CK_VOID_PTR)pobjectID, (CK_ULONG)strlen(pobjectID), CK_FALSE);
					if (ret)
						goto cleanup;
				}
			}
			if (dataType != CACHED_DATA_TYPE_ALL_DATA)
			{
				break;
			}
			/* Falls through */
		case CACHED_DATA_TYPE_ADDRESS:
			oFileData = poCard->ReadCardFile(BEID_FILE_ADDRESS);
			plabel = BEID_LABEL_ADDRESS_FILE;
			pobjectID = BEID_OBJECTID_ADDRESS;
			ret = p11_add_slot_ID_object(pSlot, ID_DATA, sizeof(ID_DATA) / sizeof(CK_ATTRIBUTE), CK_TRUE, CKO_DATA,
				CK_FALSE, &hObject, (CK_VOID_PTR)plabel, (CK_ULONG)strlen(plabel),
				(CK_VOID_PTR)oFileData.GetBytes(), (CK_ULONG)oFileData.Size(),
				(CK_VOID_PTR)pobjectID, (CK_ULONG)strlen(pobjectID), CK_FALSE);
			if (ret)
				goto cleanup;
			oTLVBufferAddress.ParseTLV(oFileData.GetBytes(),
				oFileData.Size());
			nrOfItems = sizeof(ADDRESS_LABELS) / sizeof(BEID_DATA_LABELS_NAME);
			for (i = 0; i < nrOfItems; i++)
			{
				ulLen = sizeof(cBuffer);
				memset(cBuffer, 0, ulLen);
				if (oTLVBufferAddress.FillUTF8Data(ADDRESS_LABELS[i].tag, cBuffer, &ulLen)) {
					plabel = ADDRESS_LABELS[i].name;
					ret = p11_add_slot_ID_object(pSlot, ID_DATA, sizeof(ID_DATA) / sizeof(CK_ATTRIBUTE), CK_TRUE,
						CKO_DATA, CK_FALSE, &hObject, (CK_VOID_PTR)plabel,
						(CK_ULONG)strlen(plabel), (CK_VOID_PTR)cBuffer, ulLen,
						(CK_VOID_PTR)pobjectID, (CK_ULONG)strlen(pobjectID), CK_FALSE);
					if (ret)
						goto cleanup;
				}
			}
			if (dataType != CACHED_DATA_TYPE_ALL_DATA)
			{
				break;
			}
			/* Falls through */
		case CACHED_DATA_TYPE_PHOTO:
			plabel = BEID_LABEL_PHOTO;
			pobjectID = BEID_OBJECTID_PHOTO;
			oFileData = poCard->ReadCardFile(BEID_FILE_PHOTO);
			ret = p11_add_slot_ID_object(pSlot, ID_DATA,
				sizeof(ID_DATA) / sizeof(CK_ATTRIBUTE),
				CK_TRUE,
				CKO_DATA,
				CK_FALSE,
				&hObject,
				(CK_VOID_PTR)plabel, (CK_ULONG)strlen(plabel),
				(CK_VOID_PTR)oFileData.GetBytes(), (CK_ULONG)oFileData.Size(),
				(CK_VOID_PTR)pobjectID, (CK_ULONG)strlen(BEID_OBJECTID_PHOTO),
				CK_FALSE);
			if (ret)
				goto cleanup;
			if (dataType != CACHED_DATA_TYPE_ALL_DATA)
			{
				break;
			}
			/* Falls through */
		case CACHED_DATA_TYPE_RNCERT:
			oFileData = poCard->ReadCardFile(BEID_FILE_CERT_RRN);
			plabel = BEID_LABEL_CERT_RN;
			pobjectID = BEID_OBJECTID_RNCERT;
			ret = p11_add_slot_ID_object(pSlot, ID_DATA,
				sizeof(ID_DATA) / sizeof(CK_ATTRIBUTE),
				CK_TRUE,
				CKO_DATA,
				CK_FALSE,
				&hObject,
				(CK_VOID_PTR)plabel, (CK_ULONG)strlen(plabel),
				(CK_VOID_PTR)oFileData.GetBytes(), (CK_ULONG)oFileData.Size(),
				(CK_VOID_PTR)pobjectID, (CK_ULONG)strlen(BEID_OBJECTID_RNCERT),
				CK_FALSE);
			if (ret)
				goto cleanup;
			if (dataType != CACHED_DATA_TYPE_ALL_DATA)
			{
				break;
			}
			/* Falls through */
		case CACHED_DATA_TYPE_SIGN_DATA_FILE:
			plabel = BEID_LABEL_SGN_RN;
			oFileData = poCard->ReadCardFile(BEID_FILE_ID_SIGN);
			ret = p11_add_slot_ID_object(pSlot, ID_DATA,
				sizeof(ID_DATA) / sizeof(CK_ATTRIBUTE),
				CK_TRUE,
				CKO_DATA,
				CK_FALSE,
				&hObject,
				(CK_VOID_PTR)plabel, (CK_ULONG)strlen(plabel),
				(CK_VOID_PTR)oFileData.GetBytes(), (CK_ULONG)oFileData.Size(),
				(CK_VOID_PTR)BEID_OBJECTID_SIGN_DATA_FILE,
				(CK_ULONG)strlen(BEID_OBJECTID_SIGN_DATA_FILE),
				CK_FALSE);
			if (ret)
				goto cleanup;
			if (dataType != CACHED_DATA_TYPE_ALL_DATA)
			{
				break;
			}
			/* Falls through */
		case CACHED_DATA_TYPE_SIGN_ADDRESS_FILE:
			plabel = BEID_LABEL_SGN_ADDRESS;
			oFileData = poCard->ReadCardFile(BEID_FILE_ADDRESS_SIGN);
			ret = p11_add_slot_ID_object(pSlot, ID_DATA,
				sizeof(ID_DATA) / sizeof(CK_ATTRIBUTE),
				CK_TRUE,
				CKO_DATA,
				CK_FALSE,
				&hObject,
				(CK_VOID_PTR)plabel, (CK_ULONG)strlen(plabel),
				(CK_VOID_PTR)oFileData.GetBytes(),
				(CK_ULONG)oFileData.Size(),
				(CK_VOID_PTR)BEID_OBJECTID_SIGN_ADDRESS_FILE, (CK_ULONG)strlen(BEID_OBJECTID_SIGN_ADDRESS_FILE),
				CK_FALSE);
			if (ret)
				goto cleanup;
			if (dataType != CACHED_DATA_TYPE_ALL_DATA)
			{
				break;
			}
			/* Falls through */
		case CACHED_DATA_TYPE_BASIC_KEY_FILE:
			ucAppletVersion = poCard->GetAppletVersion();

			if (ucAppletVersion >= 0x18)
			{
				plabel = BEID_LABEL_BASIC_KEY;
				pobjectID = BEID_OBJECTID_BASIC_KEY_FILE;
				oFileData = poCard->ReadCardFile(BEID_FILE_BASIC_KEY);
				ret = p11_add_slot_ID_object(pSlot, ID_DATA,
					sizeof(ID_DATA) / sizeof(CK_ATTRIBUTE),
					CK_TRUE,
					CKO_DATA,
					CK_FALSE,
					&hObject,
					(CK_VOID_PTR)plabel, (CK_ULONG)strlen(plabel),
					(CK_VOID_PTR)oFileData.GetBytes(), (CK_ULONG)oFileData.Size(),
					(CK_VOID_PTR)pobjectID, (CK_ULONG)strlen(BEID_OBJECTID_BASIC_KEY_FILE),
					CK_FALSE);
				if (ret)
					goto cleanup;
			}
			if (dataType != CACHED_DATA_TYPE_ALL_DATA)
			{
				break;
			}
			/* Falls through */
		default:
			break;
		}
	}
	catch (CMWException &e)
	{
		return (cal_translate_error(WHERE, e.GetError()));
	}
	catch (...)
	{
		//ret = -1;
		log_trace(WHERE, "E: unknown exception thrown");
		return (CKR_FUNCTION_FAILED);
	}

	if (ret != 0)
	{
		return (CKR_DEVICE_ERROR);
	}

	cleanup:
	return (ret);
}

#undef WHERE



#define WHERE "cal_read_object()"
CK_RV cal_read_object(CK_SLOT_ID hSlot, P11_OBJECT * pObject)
{
	CK_RV ret = CKR_OK;
	int status;
	CK_ULONG *pID = NULL;
	CK_ULONG *pClass = NULL;
	CK_ULONG len = 0;
	CK_BBOOL btrue = CK_TRUE;
	CK_BBOOL bfalse = CK_FALSE;
	P11_OBJECT *pCertObject = NULL;
	P11_OBJECT *pPubKeyObject = NULL;
	P11_OBJECT *pPrivKeyObject = NULL;
	T_CERT_INFO certinfo;
	T_KEY_INFO keyinfo;
	CByteArray oCertData;
	tCert cert;
	tPrivKey key;

	std::string szReader;
	P11_SLOT *pSlot = NULL;

	memset(&certinfo, 0, sizeof(T_CERT_INFO));
	memset(&keyinfo, 0, sizeof(T_KEY_INFO));

	pSlot = p11_get_slot(hSlot);
	if (pSlot == NULL)
	{
		log_trace(WHERE, "E: Invalid slot (%lu)", hSlot);
		return (CKR_SLOT_ID_INVALID);
	}

	szReader = pSlot->name;

	ret = cal_update_token(hSlot, &status, 0);
	if (ret != CKR_OK)
		goto cleanup;

	if ((status == P11_CARD_REMOVED) || (status == P11_CARD_NOT_PRESENT))
	{
		ret = CKR_TOKEN_NOT_PRESENT;
		goto cleanup;
	}

	ret = p11_get_attribute_value(pObject->pAttr, pObject->count,
				      CKA_CLASS, (CK_VOID_PTR *) & pClass,
				      &len);
	if (ret != CKR_OK)
		goto cleanup;

	//do not refresh the ID files here
	if (*pClass == CKO_DATA)
		goto cleanup;

	//read ID of object we want to read from token
	ret = p11_get_attribute_value(pObject->pAttr, pObject->count, CKA_ID,
				      (CK_VOID_PTR *) & pID, &len);
	if (ret != CKR_OK)
		goto cleanup;

	//get the object related to this ID

	p11_find_slot_object(pSlot, CKO_PRIVATE_KEY, *pID, &pPrivKeyObject);

	p11_find_slot_object(pSlot, CKO_PUBLIC_KEY, *pID, &pPubKeyObject);

	p11_find_slot_object(pSlot, CKO_CERTIFICATE, *pID, &pCertObject);

	try
	{
		CReader & oReader = oCardLayer->getReader(szReader);
		CCard* poCard = oReader.GetCard();

		if (pCertObject != NULL)
		{

			cert = poCard->GetCertByID(*pID);

			//bValid duidt aan if cert met deze ID
			if (cert.bValid)
				oCertData = poCard->ReadCardFile(cert.csPath);
			else
			{
				log_trace(WHERE, "E: cert.bValid is false");
				return (CKR_DEVICE_ERROR);
			}

			//at least 64K Bytes as size (unsigned int) will suffice for cert length
			if (cert_get_info(oCertData.GetBytes(), (unsigned int)(oCertData.Size()), &certinfo) < 0)
			{
				// ASN.1 parser failed. Assume hardware failure.
				log_trace(WHERE, "E: cert_get_info failed");
				ret = CKR_DEVICE_ERROR;
				goto cleanup;
			}

			ret = p11_set_attribute_value(pCertObject->pAttr, pCertObject->count, CKA_SUBJECT, (CK_VOID_PTR)certinfo.subject, (CK_ULONG)certinfo.l_subject);
			if (ret != CKR_OK)
				goto cleanup;

			ret = p11_set_attribute_value(pCertObject->pAttr, pCertObject->count, CKA_ISSUER, (CK_VOID_PTR)certinfo.issuer, (CK_ULONG)certinfo.l_issuer);
			if (ret != CKR_OK)
				goto cleanup;

			ret = p11_set_attribute_value(pCertObject->pAttr, pCertObject->count, CKA_SERIAL_NUMBER, (CK_VOID_PTR)certinfo.serial, (CK_ULONG)certinfo.l_serial);
			if (ret != CKR_OK)
				goto cleanup;
			//use real length from decoder here instead of lg from cal
			ret = p11_set_attribute_value(pCertObject->pAttr, pCertObject->count, CKA_VALUE, (CK_VOID_PTR)oCertData.GetBytes(), (CK_ULONG)certinfo.lcert);
			if (ret != CKR_OK)
				goto cleanup;
			//TODO Check this in the cal if we can be sure that the certificate can be trusted and not be modified on the card
			ret = p11_set_attribute_value(pCertObject->pAttr, pCertObject->count, CKA_TRUSTED, (CK_VOID_PTR)& btrue, sizeof(btrue));
			if (ret != CKR_OK)
				goto cleanup;

			pCertObject->state = P11_CACHED;

			key = poCard->GetPrivKeyByID(*pID);

			if (pPrivKeyObject != NULL)
			{
				ret = p11_set_attribute_value(pPrivKeyObject->pAttr, pPrivKeyObject->count, CKA_SENSITIVE, (CK_VOID_PTR)& btrue, sizeof(btrue));
				if (ret != CKR_OK)
					goto cleanup;

				ret = p11_set_attribute_value(pPrivKeyObject->pAttr, pPrivKeyObject->count, CKA_DECRYPT, (CK_VOID_PTR)& bfalse, sizeof(bfalse));
				if (ret != CKR_OK)
					goto cleanup;

				ret = p11_set_attribute_value(pPrivKeyObject->pAttr, pPrivKeyObject->count, CKA_SIGN_RECOVER, (CK_VOID_PTR)& bfalse, sizeof(CK_BBOOL));
				if (ret != CKR_OK)
					goto cleanup;

				ret = p11_set_attribute_value(pPrivKeyObject->pAttr, pPrivKeyObject->count, CKA_UNWRAP, (CK_VOID_PTR)& bfalse, sizeof(CK_BBOOL));
				if (ret != CKR_OK)
					goto cleanup;

				ret = p11_set_attribute_value(pPrivKeyObject->pAttr, pPrivKeyObject->count, CKA_SUBJECT, (CK_VOID_PTR)certinfo.subject, (CK_ULONG)certinfo.l_subject);
				if (ret != CKR_OK)
					goto cleanup;

				if (key.keyType == RSA)
				{
					if (certinfo.l_mod > 0)
						ret = p11_set_attribute_value(pPrivKeyObject->pAttr, pPrivKeyObject->count, CKA_MODULUS, (CK_VOID_PTR)certinfo.mod, (CK_ULONG)certinfo.l_mod);
					if (ret != CKR_OK)
						goto cleanup;

					if (certinfo.l_exp > 0)
						ret = p11_set_attribute_value(pPrivKeyObject->pAttr, pPrivKeyObject->count, CKA_PUBLIC_EXPONENT, (CK_VOID_PTR)certinfo.exp, (CK_ULONG)certinfo.l_exp);
					if (ret != CKR_OK)
						goto cleanup;
				}
				else
				{
					if (certinfo.l_curve > 0)
						ret = p11_set_attribute_value(pPrivKeyObject->pAttr, pPrivKeyObject->count, CKA_EC_PARAMS, (CK_VOID_PTR)certinfo.curve, (CK_ULONG)certinfo.l_curve);
					if (ret != CKR_OK)
						goto cleanup;
				}
				pPrivKeyObject->state = P11_CACHED;
			}
			if (pPubKeyObject != NULL)
			{
				ret = p11_set_attribute_value(pPubKeyObject->pAttr, pPubKeyObject->count, CKA_SENSITIVE, (CK_VOID_PTR)& btrue, sizeof(btrue));
				if (ret != CKR_OK)
					goto cleanup;

				ret = p11_set_attribute_value(pPubKeyObject->pAttr, pPubKeyObject->count, CKA_VERIFY, (CK_VOID_PTR)& btrue, sizeof(btrue));
				if (ret != CKR_OK)
					goto cleanup;

				ret = p11_set_attribute_value(pPubKeyObject->pAttr, pPubKeyObject->count, CKA_ENCRYPT, (CK_VOID_PTR)& bfalse, sizeof(bfalse));
				if (ret != CKR_OK)
					goto cleanup;

				ret = p11_set_attribute_value(pPubKeyObject->pAttr, pPubKeyObject->count, CKA_WRAP, (CK_VOID_PTR)& bfalse, sizeof(CK_BBOOL));
				if (ret != CKR_OK)
					goto cleanup;

				ret = p11_set_attribute_value(pPubKeyObject->pAttr, pPubKeyObject->count, CKA_SUBJECT, (CK_VOID_PTR)certinfo.subject, (CK_ULONG)certinfo.l_subject);
				if (ret != CKR_OK)
					goto cleanup;

				if (key.keyType == RSA)
				{
					if (certinfo.l_mod > 0)
						ret = p11_set_attribute_value(pPubKeyObject->pAttr, pPubKeyObject->count, CKA_MODULUS, (CK_VOID_PTR)certinfo.mod, certinfo.l_mod);
					if (ret != CKR_OK)
						goto cleanup;

					if (certinfo.l_exp > 0)
						ret = p11_set_attribute_value(pPubKeyObject->pAttr, pPubKeyObject->count, CKA_PUBLIC_EXPONENT, (CK_VOID_PTR)certinfo.exp, certinfo.l_exp);
					if (ret != CKR_OK)
						goto cleanup;

					if (certinfo.l_pkinfo > 0)
						ret = p11_set_attribute_value(pPubKeyObject->pAttr, pPubKeyObject->count, CKA_VALUE, (CK_VOID_PTR)certinfo.pkinfo, certinfo.l_pkinfo);
					if (ret != CKR_OK)
						goto cleanup;
				}
				else
				{
					if (certinfo.l_curve > 0)
						ret = p11_set_attribute_value(pPubKeyObject->pAttr, pPubKeyObject->count, CKA_EC_PARAMS, (CK_VOID_PTR)certinfo.curve, (CK_ULONG)certinfo.l_curve);
					if (ret != CKR_OK)
						goto cleanup;

					if (certinfo.l_pkinfo > 0)
						ret = p11_set_attribute_value(pPubKeyObject->pAttr, pPubKeyObject->count, CKA_EC_POINT, (CK_VOID_PTR)certinfo.pkinfo, certinfo.l_pkinfo);
					if (ret != CKR_OK)
						goto cleanup;
				}

				//TODO test if we can set the trusted flag...
				ret = p11_set_attribute_value(pPubKeyObject->pAttr, pPubKeyObject->count, CKA_TRUSTED, (CK_VOID_PTR)& btrue, sizeof(btrue));
				if (ret != CKR_OK)
					goto cleanup;

				pPubKeyObject->state = P11_CACHED;
			}
		}
		else if (pPubKeyObject != NULL)// no matching cert found, check if a public key was found (card key)
		{
		//verify if its the card public key
		if(*pID != poCard->GetCardKeyID())
			goto cleanup;
		
		//read file
		CByteArray oCardKeyData = poCard->ReadCardFile(BEID_FILE_BASIC_KEY);

		//parse file
		if (key_get_info(oCardKeyData.GetBytes(), (unsigned int)oCardKeyData.Size(), &keyinfo) < 0)
		{
			// ASN.1 parser failed. Assume hardware failure.
			log_trace(WHERE, "E: key_get_info failed");
			ret = CKR_DEVICE_ERROR;
			goto cleanup;
		}

		//store data in attributes
		key = poCard->GetPrivKeyByID(*pID); //get the matching private key to check the keytype

		if (key.keyType == EC)
		{
			if (keyinfo.l_curve > 0)
				ret = p11_set_attribute_value(pPubKeyObject->pAttr, pPubKeyObject->count, CKA_EC_PARAMS, (CK_VOID_PTR)keyinfo.curve, (CK_ULONG)keyinfo.l_curve);
			if (ret != CKR_OK)
				goto cleanup;

			if (keyinfo.l_pkinfo > 0)
				ret = p11_set_attribute_value(pPubKeyObject->pAttr, pPubKeyObject->count, CKA_EC_POINT, (CK_VOID_PTR)keyinfo.pkinfo, keyinfo.l_pkinfo);
			if (ret != CKR_OK)
				goto cleanup;

			pPubKeyObject->state = P11_CACHED;
		}
		}
	}
	catch (CMWException &e)
	{
		cert_free_info(&certinfo);
		key_free_info(&keyinfo);
		return (cal_translate_error(WHERE, e.GetError()));
	}
	catch (...)
	{
		log_trace(WHERE, "E: unkown exception thrown");
		cert_free_info(&certinfo);
		key_free_info(&keyinfo);
		return (CKR_FUNCTION_FAILED);
	}

	if (ret != 0)
	{
		log_trace(WHERE, "E: ret is 0x%0lx", ret);
		cert_free_info(&certinfo);
		key_free_info(&keyinfo);
		return (CKR_DEVICE_ERROR);
	}

 cleanup:
	cert_free_info(&certinfo);
	key_free_info(&keyinfo);
	return (ret);
}

#undef WHERE


#define WHERE "cal_challenge()"
CK_RV cal_challenge(CK_SLOT_ID hSlot, P11_SIGN_DATA * pSignData, unsigned char *in,
	unsigned long l_in, unsigned char *out, unsigned long *l_out)
{
	CK_RV ret = 0;
	CByteArray oData(in, l_in);
	CByteArray oDataOut;
	P11_SLOT *pSlot = NULL;

	pSlot = p11_get_slot(hSlot);
	if (pSlot == NULL)
	{
		log_trace(WHERE, "E: Invalid slot (%lu)", hSlot);
		return (CKR_SLOT_ID_INVALID);
	}
	std::string szReader = pSlot->name;

	//the caller is responsible for filling in/ checking the length
	try
	{
		CReader & oReader = oCardLayer->getReader(szReader);
		CCard* poCard = oReader.GetCard();

		if (pSignData->mechanism != CKM_ECDSA)
		{
			ret = CKR_MECHANISM_INVALID;
			goto cleanup;
		}

		oDataOut = poCard->CardChallenge(oData);
	}
	catch (CMWException & e)
	{
		return (cal_translate_error(WHERE, e.GetError()));
	}
	catch (...)
	{
		log_trace(WHERE, "E: unkown exception thrown");
		return (CKR_FUNCTION_FAILED);
	}

	*l_out = oDataOut.Size();
	memcpy(out, oDataOut.GetBytes(), *l_out);

cleanup:

	return (ret);
}

#undef WHERE


#define WHERE "cal_sign()"
CK_RV cal_sign(CK_SLOT_ID hSlot, P11_SIGN_DATA * pSignData, unsigned char *in,
	       unsigned long l_in, unsigned char *out, unsigned long *l_out)
{
	CK_RV ret = 0;
	CByteArray oData(in, l_in);
	CByteArray oDataOut;
	unsigned long algo;
	P11_SLOT *pSlot = NULL;

	pSlot = p11_get_slot(hSlot);
	if (pSlot == NULL)
	{
		log_trace(WHERE, "E: Invalid slot (%lu)", hSlot);
		return (CKR_SLOT_ID_INVALID);
	}
	std::string szReader = pSlot->name;

	//the caller is responsible for filling in/ checking the length
	/*
	   if (out == NULL)
	   {
	   //get length of signature
	   *l_out = 128;
	   return(CKR_OK);
	   }
	   if (*l_out < 128)
	   return(CKR_BUFFER_TOO_SMALL);
	 */
	try
	{
		CReader & oReader = oCardLayer->getReader(szReader);
		CCard* poCard = oReader.GetCard();
		tPrivKey key = poCard->GetPrivKeyByID(pSignData->id);

		switch (pSignData->mechanism)
		{
			case CKM_RSA_PKCS:
				algo = SIGN_ALGO_RSA_PKCS;
				break;
			case CKM_MD5:
			case CKM_MD5_RSA_PKCS:
				algo = SIGN_ALGO_MD5_RSA_PKCS;
				break;
			case CKM_SHA_1:
			case CKM_SHA1_RSA_PKCS:
				algo = SIGN_ALGO_SHA1_RSA_PKCS;
				break;
			case CKM_SHA256:
			case CKM_SHA256_RSA_PKCS:
				algo = SIGN_ALGO_SHA256_RSA_PKCS;
				break;
			case CKM_SHA384:
			case CKM_SHA384_RSA_PKCS:
				algo = SIGN_ALGO_SHA384_RSA_PKCS;
				break;
			case CKM_SHA512:
			case CKM_SHA512_RSA_PKCS:
				algo = SIGN_ALGO_SHA512_RSA_PKCS;
				break;
			case CKM_RIPEMD160:
			case CKM_RIPEMD160_RSA_PKCS:
				algo = SIGN_ALGO_RIPEMD160_RSA_PKCS;
				break;
			case CKM_SHA1_RSA_PKCS_PSS:
				algo = SIGN_ALGO_SHA1_RSA_PSS;
				break;
			case CKM_SHA256_RSA_PKCS_PSS:
				algo = SIGN_ALGO_SHA256_RSA_PSS;
				break;
			case CKM_ECDSA_SHA256:
				algo = SIGN_ALGO_SHA256_ECDSA;
				break;
			case CKM_ECDSA_SHA384:
				algo = SIGN_ALGO_SHA384_ECDSA;
				break;
			case CKM_ECDSA_SHA512:
				algo = SIGN_ALGO_SHA512_ECDSA;
				break;
			case CKM_ECDSA:
				algo = SIGN_ALGO_ECDSA_RAW;
				break;
			default:
				ret = CKR_MECHANISM_INVALID;
				goto cleanup;
		}

		oDataOut = poCard->CardSign(key, algo, oData);
	}
	catch(CMWException & e)
	{
		return (cal_translate_error(WHERE, e.GetError()));
	}
	catch( ...)
	{
		log_trace(WHERE, "E: unkown exception thrown");
		return (CKR_FUNCTION_FAILED);
	}

	*l_out = oDataOut.Size();
	memcpy(out, oDataOut.GetBytes(), *l_out);

      cleanup:

	return (ret);
}

#undef WHERE


#define WHERE "cal_validate_session()"
CK_RV cal_validate_session(P11_SESSION * pSession)
{
	CK_RV ret = CKR_OK;
	int status;

	if (pSession->inuse == 0)
	{
		return (CKR_SESSION_HANDLE_INVALID);
	}
	//check status flag in session first
	if (pSession->state != P11_CARD_STILL_PRESENT)
	{
		//state allready invalid for this session so we can return here
		switch (pSession->state)
		{
			case P11_CARD_INSERTED:	//card is inserted (Opensession allready called connect(update) so state PRESENT is not expected anymore)
			case P11_CARD_NOT_PRESENT:	//card is not present
			case P11_CARD_REMOVED:	//card is removed
			case P11_CARD_OTHER:	//another card has been inserted, replacing the previous one
			default:
				//        return (CKR_TOKEN_NOT_PRESENT);
				return (CKR_DEVICE_REMOVED);
				//        return (CKR_SESSION_HANDLE_INVALID);
		}
	}

	try
	{
		//previous state is STILL_PRESENT so get new state to see if this has changed
		ret = cal_update_token(pSession->hslot, &status, 0);
		if (ret != CKR_OK)
		{
			return (ret);
		}

		if (status == P11_CARD_STILL_PRESENT)
			ret = 0;
		else
		{
			//      if ( (status == P11_CARD_REMOVED) || (status == P11_CARD_NOT_PRESENT) )
			ret = CKR_DEVICE_REMOVED;
			//      else
			////         ret = CKR_SESSION_HANDLE_INVALID;
			//         ret = CKR_OK;
		}
	}
	catch(CMWException &e)
	{
		return (cal_translate_error(WHERE, e.GetError()));
	}
	catch( ...)
	{
		log_trace(WHERE, "E: unkown exception thrown");
		return (CKR_GENERAL_ERROR);
	}
	return (ret);
}

#undef WHERE




#define WHERE "cal_update_token()"
CK_RV cal_update_token(CK_SLOT_ID hSlot, int *pStatus, int bPresenceOnly)
{
	P11_OBJECT *pObject = NULL;
	CK_RV ret = CKR_OK;

	//int status;
	unsigned int i = 0;
	P11_SLOT *pSlot = NULL;

	pSlot = p11_get_slot(hSlot);
	if (pSlot == NULL)
	{
		log_trace(WHERE, "E: Invalid slot (%lu)", hSlot);
		return (CKR_SLOT_ID_INVALID);
	}

	try
	{
		std::string reader = pSlot->name;
		CReader & oReader = oCardLayer->getReader(reader);
		CCard* poCard = NULL;
		//we get an error thrown here when the cardobject has not been created yet

		*pStatus = cal_map_status(oReader.Status(true, bPresenceOnly ? true : false));
		
		poCard = oReader.GetCard();

		if ( (*pStatus == P11_CARD_INSERTED) || (*pStatus == P11_CARD_STILL_PRESENT)  || (*pStatus == P11_CARD_OTHER) )
		{
			if (!bPresenceOnly && (poCard->GetType() == CARD_UNKNOWN))
			{
				log_trace(WHERE, "oReader.GetCardType() == CARD_UNKNOWN");
				return (CKR_TOKEN_NOT_RECOGNIZED);
			}
		}

		if (*pStatus != P11_CARD_STILL_PRESENT)
		{
			//clean objects
			for (i = 1; i <= pSlot->nobjects; i++)
			{
				pObject = p11_get_slot_object(pSlot, i);
				p11_clean_object(pObject);
				//if (pObject != NULL)
				// pObject->state = 0;
			}
			pSlot->ulCardDataCached = 0;

			//invalidate sessions
			p11_invalidate_sessions(hSlot, *pStatus);

			//if Present, other => init objects
			if ((*pStatus == P11_CARD_OTHER) || (*pStatus == P11_CARD_INSERTED))
			{
				//(re)initialize objects
#ifdef PKCS11_FF
				ret = cal_init_objects(pSlot);
				if (ret != CKR_OK)
				{
					log_trace(WHERE, "E: cal_init_objects() returned %s", log_map_error(ret));
				}
#endif
			}

		}
	}
	catch(CMWException &e)
	{
		if (e.GetError() == EIDMW_ERR_NO_CARD)
		{
			//token not present is a status, not an error
			*pStatus = P11_CARD_NOT_PRESENT;
			ret = CKR_OK;
			log_trace(WHERE, "I: No card present");
		} 
		else 
		{
			ret = cal_translate_error(WHERE, e.GetError());
		}
		return ret;
	}
	catch( ...)
	{
		log_trace(WHERE, "E: unkown exception thrown");
		return (CKR_SESSION_HANDLE_INVALID);
	}
	return ret;
}

#undef WHERE


#define WHERE "cal_get_slot_changes()"
CK_RV cal_get_slot_changes(int *ph)
{
	int first = 1;
	P11_SLOT *pSlot = NULL;
	CK_RV ret = CKR_NO_EVENT;

	*ph = -1;

	try
	{
		for (int i = 0; i < p11_get_nreaders(); i++)
		{
			if (oReadersInfo->ReaderStateChanged(i))
			{
				//return first reader that changed state
				//there could be more than one reader that changed state,
				//keep these events in the slotlist
#ifdef PKCS11_FF
				//in case the upnp reader detected a reader event, we report it with a slotID that is above the highest slotID in the current slotList
				if (i == (p11_get_nreaders() - 1))
				{
					if(first)
					{ 
						*ph = p11_get_nreaders();
						ret = CKR_OK;
					}
					else {
						pSlot = p11_get_slot(i);
						if (pSlot)
						{
							pSlot->ievent = 2;
						}
					}
				} else
#endif
				{
					if (first)
					{
						*ph = i;
						first = 0;
						ret = CKR_OK;
					} else
					{
						pSlot = p11_get_slot(i);
						if (pSlot)
						{
							if (oReadersInfo->CardPresent(i))
								pSlot->ievent = 1;
							else
								pSlot->ievent = 2;
						}
					}
				}
			}
		}
	}
	catch(CMWException &e)
	{
		return (cal_translate_error(WHERE, e.GetError()));
	}
	catch( ...)
	{
		log_trace(WHERE, "E: unkown exception thrown");
		return (CKR_FUNCTION_FAILED);
	}
	return ret;
}

#undef WHERE

#define WHERE "cal_map_status()"
int cal_map_status(tCardStatus calstatus)
{
	switch (calstatus)
	{
		case CARD_INSERTED:
			return (P11_CARD_INSERTED);
		case CARD_NOT_PRESENT:
			return (P11_CARD_NOT_PRESENT);
		case CARD_STILL_PRESENT:
			return (P11_CARD_STILL_PRESENT);
		case CARD_REMOVED:
			return (P11_CARD_REMOVED);
		case CARD_OTHER:
			return (P11_CARD_OTHER);
		case CARD_UNKNOWN_STATE:
			return (P11_CARD_UNKNOWN_STATE);
		default:
			return (-1);
	}
}

#undef WHERE

#define WHERE "cal_refresh_readers()"
CK_RV cal_refresh_readers()
{
	CK_RV ret = CKR_OK;

	try
	{
		if (oReadersInfo)
		{
			//check if readerlist changed?
			CReadersInfo *pNewReadersInfo = new CReadersInfo(oCardLayer->ListReaders());
			if (pNewReadersInfo->SameList(oReadersInfo) == TRUE)
			{
				//same reader list as before, so we keep the readers' status
				delete(pNewReadersInfo);
				return CKR_OK;
			} else
			{
				delete(oReadersInfo);
				oReadersInfo = pNewReadersInfo;
			}
		} else
		{
			oReadersInfo = new CReadersInfo(oCardLayer->ListReaders());
		}
		//new _reader list, so please stop the scardgetstatuschange that is waiting on the old list
		log_trace(WHERE, "I: stopping the scardgetstatuschange that is waiting on the old list");
		oCardLayer->CancelActions();
		log_trace(WHERE, "I: called oCardLayer->CancelActions()");
	}
	catch(CMWException &e)
	{
		log_trace(WHERE, "E: CMWException exception thrown: 0x%8lx", e.GetError());
		return (cal_translate_error(WHERE, e.GetError()));
	}
	catch( ...)
	{
		log_trace(WHERE, "E: unknown exception thrown");
		return (CKR_FUNCTION_FAILED);
	}

	//init slots and token in slots
	memset(gpSlot, 0, sizeof(gpSlot));
	ret = cal_init_slots();
	if (ret)
		log_trace(WHERE, "E: p11_init_slots() returns %lu", ret);

	return (ret);
}

#undef WHERE

CK_RV cal_translate_error(const char *WHERE, long err)
{
	log_trace(WHERE, "E: MiddlewareException thrown: 0x%0lx", err);

	switch (err)
	{
		case EIDMW_OK:
			return (CKR_OK);
			break;

		/** A function parameter has an unexpected value (general) */
		case EIDMW_ERR_PARAM_BAD:
			return (CKR_FUNCTION_FAILED);
			break;	//return(CKR_ARGUMENTS_BAD);

		/** A function parameter exceeded the allowed range */
		case EIDMW_ERR_PARAM_RANGE:
			return (CKR_FUNCTION_FAILED);
			break;	//return(CKR_ARGUMENTS_BAD);

		/** Bad file path (invalid characters, length no multiple of 4) */
		case EIDMW_ERR_BAD_PATH:
			return (CKR_FUNCTION_FAILED);
			break;

		/** Unknown/unsupported algorithm */
		case EIDMW_ERR_ALGO_BAD:
			return (CKR_MECHANISM_INVALID);
			break;

		/** Invalid/unsupported PIN operation */
		case EIDMW_ERR_PIN_OPERATION:
			return (CKR_FUNCTION_FAILED);
			break;

		/** PIN not allowed for this card (invalid characters, too short/long) */
		case EIDMW_ERR_PIN_FORMAT:
			return (CKR_FUNCTION_FAILED);
			break;
			/* the action was cancelled */
		case EIDMW_ERR_CANCELLED:
			return (CKR_FUNCTION_CANCELED);
			break;
			// Card errors

		/** Generic card error */
		case EIDMW_ERR_CARD:
			return (CKR_DEVICE_ERROR);
			break;

		/** Not Authenticated (no PIN specified) */
		case EIDMW_ERR_NOT_AUTHENTICATED:
			return (CKR_USER_NOT_LOGGED_IN);
			break;

		/** This command is not supported by this card */
		case EIDMW_ERR_NOT_SUPPORTED:
			return (CKR_DEVICE_ERROR);
			break;

		/** Bad PIN */
		case EIDMW_ERR_PIN_BAD:
			return (CKR_PIN_INCORRECT);
			break;

		/** PIN blocked */
		case EIDMW_ERR_PIN_BLOCKED:
			return (CKR_PIN_LOCKED);
			break;

		/** No card present or card has been removed */
		case EIDMW_ERR_NO_CARD:
			return (CKR_TOKEN_NOT_PRESENT);
			break;

		/** Bad parameter P1 or P2 */
		case EIDMW_ERR_BAD_P1P2:
			return (CKR_DEVICE_ERROR);
			break;

		/** Command not allowed */
		case EIDMW_ERR_CMD_NOT_ALLOWED:
			return (CKR_DEVICE_ERROR);
			break;

		/** File not found */
		case EIDMW_ERR_FILE_NOT_FOUND:
			return (CKR_DEVICE_ERROR);
			break;

		/** Unable to read applet version from the card */
		case EIDMW_ERR_APPLET_VERSION_NOT_FOUND:
			return (CKR_DEVICE_ERROR);
			break;

		/** Card not activated */
		case EIDMW_ERR_NOT_ACTIVATED:
			return (CKR_DEVICE_ERROR);
			break;

			// Reader errors

		/** Error communicating with the card */
		case EIDMW_ERR_CARD_COMM:
			return (CKR_DEVICE_ERROR);
			break;

		/** No reader has been found */
		case EIDMW_ERR_NO_READER:
			return (CKR_DEVICE_ERROR);
			break;

		/** The pinpad reader returned an error */
		case EIDMW_ERR_PINPAD:
			return (CKR_DEVICE_ERROR);
			break;

		/** A card is present but we can't connect in a normal way*/
		case EIDMW_ERR_CANT_CONNECT:
			return (CKR_DEVICE_ERROR);
			break;

			// Internal errors (caused by the middleware)

		/** An internal limit has been reached */
		case EIDMW_ERR_LIMIT:
			return (CKR_DEVICE_ERROR);
			break;

		/** An internal check failed */
		case EIDMW_ERR_CHECK:
			return (CKR_DEVICE_ERROR);
			break;

		/** The PCSC library could not be located */
		case EIDMW_ERR_PCSC_LIB:
			return (CKR_DEVICE_ERROR);
			break;

		/** An attempt to resolve a Z-lib address failed */
		case EIDMW_ERR_ZLIB_RESOLVE:
			return (CKR_GENERAL_ERROR);
			break;

		/** And unknown error occurred */
		case EIDMW_ERR_UNKNOWN:
			return (CKR_GENERAL_ERROR);
			break;

		/** The pinpad reader received a wrong/unknown value */
		case EIDMW_PINPAD_ERR:
			return (CKR_DEVICE_ERROR);
			break;

		/** Dynamic library couldn't be loaded (found found at the specified location) */
		case EIDMW_CANT_LOAD_LIB:
			return (CKR_GENERAL_ERROR);
			break;

		/** Memory error */
		case EIDMW_ERR_MEMORY:
			return (CKR_HOST_MEMORY);
			break;

		/** Couldn't delete cache file(s) */
		case EIDMW_ERR_DELETE_CACHE:
			return (CKR_GENERAL_ERROR);
			break;

		/** Error getting or writing config data */
		case EIDMW_CONF:
			return (CKR_GENERAL_ERROR);
			break;


			// User errors/events

		/** User pressed Cancel in PIN dialog */
		case EIDMW_ERR_PIN_CANCEL:
			return (CKR_FUNCTION_CANCELED);
			break;

		/** Pinpad timeout */
		case EIDMW_ERR_TIMEOUT:
			return (CKR_FUNCTION_CANCELED);
			break;

		/** The new PINs that were entered differ */
		case EIDMW_NEW_PINS_DIFFER:

		/** A PIN with invalid length or format was entered */
		case EIDMW_WRONG_PIN_FORMAT:
			return (CKR_FUNCTION_FAILED);
			break;

			// Parser errors

		/** Could not find expected elements in parsed ASN.1 vector */
		case EIDMW_WRONG_ASN1_FORMAT:
			return (CKR_FUNCTION_FAILED);
			break;

			// I/O errors
			// errors modelled on the definitions in errno.h

		/** File could not be opened */
		case EIDMW_FILE_NOT_OPENED:
			return (CKR_FUNCTION_FAILED);
			break;

		/** Search permission is denied for a component of the path prefix of path. */
		case EIDMW_PERMISSION_DENIED:
			return (CKR_FUNCTION_FAILED);
			break;

		/** A loop exists in symbolic links encountered during resolution of the path argument. */

		/** A component of path does not name an existing file or path is an empty string.*/

		/** A component of the path prefix is not a directory. */

		/** The length of the path argument exceeds {PATH_MAX} or a pathname component is longer than {NAME_MAX}. */
		case EIDMW_INVALID_PATH:
			return (CKR_FUNCTION_FAILED);
			break;

		/** {OPEN_MAX} file descriptors are currently open in the calling process. */

		/** Too many files are currently open in the system.*/
		case EIDMW_TOO_MANY_OPENED_FILES:
			return (CKR_FUNCTION_FAILED);
			break;

		/** The argument of closedir or readdir does not refer to an open directory stream. */
		case EIDMW_DIR_NOT_OPENED:
			return (CKR_FUNCTION_FAILED);
			break;

		/** Interrupted by a signal */
		case EIDMW_INTERRUPTION:
			return (CKR_FUNCTION_FAILED);
			break;

		/** One of the values in the structure to be returned cannot be represented correctly. */
		case EIDMW_OVERFLOW:
			return (CKR_FUNCTION_FAILED);
			break;

		/** An I/O error occurred while reading from the file system.*/
		case EIDMW_ERROR_IO:
			return (CKR_FUNCTION_FAILED);
			break;

		/** Call of the Logger after destruct time */
		case EIDMW_ERR_LOGGER_APPLEAVING:
			return (CKR_FUNCTION_FAILED);
			break;

			// SDK error

		/** The document type is unknown for this card */
		case EIDMW_ERR_DOCTYPE_UNKNOWN:
			return (CKR_FUNCTION_FAILED);
			break;

		/** The card type asked doesn't correspond with the real card type */
		case EIDMW_ERR_CARDTYPE_BAD:
			return (CKR_FUNCTION_FAILED);
			break;

		/** This card type is unknown */
		case EIDMW_ERR_CARDTYPE_UNKNOWN:
			return (CKR_TOKEN_NOT_RECOGNIZED);
			break;

		/** This Certificate has no issuer (=root) */
		case EIDMW_ERR_CERT_NOISSUER:
			return (CKR_FUNCTION_FAILED);
			break;

		/** No release of SDK object has been done before closing the application */
		case EIDMW_ERR_RELEASE_NEEDED:
			return (CKR_FUNCTION_FAILED);
			break;

			// Errors in system calls

		/** a system call returned an error */
		case EIDMW_ERR_SYSTEM:
			return (CKR_GENERAL_ERROR);
			break;

		/** a signal function returned an error */
		case EIDMW_ERR_SIGNAL:
			return (CKR_FUNCTION_FAILED);
			break;

		default:
			return (CKR_GENERAL_ERROR);
	}
}

#define WHERE "cal_wait_for_slot_event()"
CK_RV cal_wait_for_slot_event(int block)
{
	CK_RV ret = CKR_OK;

	if (oReadersInfo->IsFirstTime())
	{
		ret = cal_wait_for_the_slot_event(0);
		if (ret != CKR_OK)
			return ret;
		oReadersInfo->SetFirstTime(FALSE);
	}
	ret = cal_wait_for_the_slot_event(block);
	return ret;
}

#undef WHERE

#define WHERE "cal_wait_for_the_slot_event()"
CK_RV cal_wait_for_the_slot_event(int block)
{
	SCARD_READERSTATEA txReaderStates[MAX_READERS];
	CK_RV ret = CKR_OK;

#ifdef PKCS11_FF
	long lret = SCARD_E_TIMEOUT;
#endif
	unsigned long ulnReaders = 0;

	memset(txReaderStates, 0, sizeof(txReaderStates));
	oReadersInfo->GetReaderStates(txReaderStates, MAX_READERS, &ulnReaders);

	try
	{
		if (block)
		{
			p11_unlock();

			oCardLayer->GetStatusChange(TIMEOUT_INFINITE, txReaderStates, ulnReaders);

			log_trace(WHERE, "I: status change received");
			p11_lock();
			if (p11_get_init() != BEIDP11_INITIALIZED)
			{
				log_trace(WHERE,
					  "I: leave, p11_get_init returned false");
				CLEANUP(CKR_CRYPTOKI_NOT_INITIALIZED);
			}
			if (oReadersInfo->IsFirstTime())
			{
				//we never call GetStatusChange with blocked flag when its the first time,
				//if we get here, C_getslotlist must have reset oReadersInfo, in which case
				//all reader states we have here are obsolete
				CLEANUP(CKR_NO_EVENT);
			}
		} else
		{
			oCardLayer->GetStatusChange(0, txReaderStates, ulnReaders);
		}
	}
	catch(CMWException &e)
	{
		if (block)
		{
			p11_lock();
		}
		log_trace(WHERE, "E: CMWException exception thrown 0x%0lx", e.GetError());
		CLEANUP(cal_translate_error(WHERE, e.GetError()));
	}
	catch( ...)
	{
		if (block)
		{
			p11_lock();
		}
		log_trace(WHERE, "E: unkown exception thrown");
		CLEANUP(CKR_FUNCTION_FAILED);
	}

	oReadersInfo->UpdateReaderStates(txReaderStates, ulnReaders);

      cleanup:
	cal_free_reader_states(txReaderStates, ulnReaders);
	return (ret);
}

#undef WHERE

void cal_free_reader_states(SCARD_READERSTATEA * txReaderStates, unsigned long ulnReaders)
{
	// Free the memory allocated for the reader names
	for (DWORD i = 0; i < ulnReaders; i++)
	{
		if (txReaderStates[i].szReader != NULL)
		{
			free((void *) (txReaderStates[i].szReader));
			txReaderStates[i].szReader = NULL;
		}
	}
	return;
}
