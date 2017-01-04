
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
#include "log.h"
#include "cert.h"
#include "mw_util.h"
#include "tlvbuffer.h"
#include "thread.h"

#ifndef WIN32
#define strcpy_s(a,b,c)         strcpy((a),(c))
#define sprintf_s(a,b,c,d)    sprintf((a),(c),(d))
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
}

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
	catch(CMWException e)
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
	gnFFReaders = 0;
#endif
	memset(gpSlot, 0, sizeof(gpSlot));
	ret = cal_init_slots();
	if (ret)
		log_trace(WHERE, "E: p11_init_slots() returns %d", ret);

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
		nReaders = oReadersInfo->ReaderCount();
		//get readernames
		for (i = 0; i < nReaders; i++)
		{
			//initialize login state to not logged in by SO nor user
			gpSlot[i].login_type = -1;
			std::string reader = oReadersInfo->ReaderName(i);
			strcpy_n((unsigned char *) gpSlot[i].name,
				 (const char *) reader.c_str(),
				 (unsigned int) reader.size(), (char) '\x00');
		}
	}
	catch(CMWException e)
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

	ret = cal_update_token(hSlot, &status);

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
		log_trace(WHERE, "E: Invalid slot (%d)", hSlot);
		return (CKR_SLOT_ID_INVALID);
	}

	std::string reader = pSlot->name;

	ret = cal_update_token(hSlot, &status);
	if (ret != CKR_OK)
		goto cleanup;

	if ((status == P11_CARD_REMOVED) || (status == P11_CARD_NOT_PRESENT))
	{
		ret = CKR_TOKEN_NOT_PRESENT;
		goto cleanup;
	}

	pInfo->firmwareVersion.major = 1;
	pInfo->firmwareVersion.minor = 0;

	//TODO token recognized  CKR_TOKEN_NOT_RECOGNIZED
	try
	{
		// Take the last 16 hex chars of the serialnr.
		// For BE eID cards, the serial nr. is 32 hex chars long,
		// and the first one are the same for all cards
		CReader & oReader = oCardLayer->getReader(reader);
		std::string oSerialNr = oReader.GetSerialNr();
		size_t serialNrLen = oSerialNr.size();
		size_t snoffset = serialNrLen > 16 ? serialNrLen - 16 : 0;
		size_t snlen =
			serialNrLen - snoffset >
			16 ? 16 : serialNrLen - snoffset;
		//printf("off = %d, len = %d\n", snoffset, snlen);
		strcpy_n(pInfo->serialNumber, oSerialNr.c_str() + snoffset,
			 snlen, ' ');
		strcpy_n(pInfo->label, oReader.GetCardLabel().c_str(), 32,
			 ' ');
		if (oReader.IsPinpadReader())
			pInfo->flags = CKF_PROTECTED_AUTHENTICATION_PATH;
		pInfo->firmwareVersion.major = oReader.GetAppletVersion();
	}
	catch(CMWException e)
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
		log_trace(WHERE, "E: Invalid slot (%d)", hSlot);
		return (CKR_SLOT_ID_INVALID);
	}

	std::string szReader = pSlot->name;
	unsigned long algos = 0;
	unsigned int n = 0;

	ret = cal_update_token(hSlot, &status);
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
		algos = oReader.GetSupportedAlgorithms();
	}
	catch(CMWException e)
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
		}
	}

	if ((info) && (info->type))
	{
		if (info->flags & CKF_SIGN)
		{
			ret = cal_update_token(hSlot, &status);
			if (ret != CKR_OK)
			{
				return (ret);
			}

			if ((status == P11_CARD_REMOVED)
			    || (status == P11_CARD_NOT_PRESENT))
			{
				return (CKR_TOKEN_NOT_PRESENT);
			}

			try
			{
				P11_SLOT *pSlot = p11_get_slot(hSlot);

				if (pSlot == NULL)
				{
					log_trace(WHERE,
						  "E: Invalid slot(%d)",
						  hSlot);
					return (CKR_SLOT_ID_INVALID);
				}
				std::string szReader = pSlot->name;

				CReader & oReader =
					oCardLayer->getReader(szReader);
				pInfo->ulMinKeySize = pInfo->ulMaxKeySize =
					(CK_ULONG) oReader.GetRSAKeySize();
			}
			catch(CMWException e)
			{
				return (cal_translate_error
					(WHERE, e.GetError()));
			}
			catch( ...)
			{
				log_trace(WHERE,
					  "E: unknown exception thrown");
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
	ret = cal_update_token(hSlot, &status);
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
		log_trace(WHERE, "E: Invalid slot (%d)", hSlot);
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
		log_trace(WHERE, "E: Invalid slot (%d)", hSlot);
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
		catch(CMWException e)
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
	CK_ATTRIBUTE PRV_KEY[] = BEID_TEMPLATE_PRV_KEY;
	CK_ATTRIBUTE PUB_KEY[] = BEID_TEMPLATE_PUB_KEY;
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

		/* add all certificate objects from card */
		for (certCounter = 0; certCounter < oReader.CertCount();
		     certCounter++)
		{
			CertId = (CK_ULONG) oReader.GetCert(certCounter).ulID;
			//      sprintf_s(clabel,sizeof(clabel), "Certificate %d (%s)", i+1, oReader.GetCert(i).csLabel.c_str());
			sprintf_s(clabel, sizeof(clabel), "%s",
				  oReader.GetCert(certCounter).csLabel.
				  c_str());

			ret = p11_add_slot_object(pSlot, CERTIFICATE,
						  sizeof(CERTIFICATE) /
						  sizeof(CK_ATTRIBUTE),
						  CK_TRUE, CKO_CERTIFICATE,
						  CertId, CK_FALSE, &hObject);
			if (ret != CKR_OK)
				goto cleanup;
			pObject = p11_get_slot_object(pSlot, hObject);

			ret = p11_set_attribute_value(pObject->pAttr,
						      pObject->count,
						      CKA_CERTIFICATE_TYPE,
						      (CK_VOID_PTR) &
						      certType,
						      sizeof(CK_ULONG));
			if (ret != CKR_OK)
				goto cleanup;
			ret = p11_set_attribute_value(pObject->pAttr,
						      pObject->count,
						      CKA_LABEL,
						      (CK_VOID_PTR) clabel,
						      (CK_ULONG)
						      strlen(clabel));
			if (ret != CKR_OK)
				goto cleanup;

			//only add keys that have a matching cert
			for (keyCounter = 0;
			     keyCounter < oReader.PrivKeyCount();
			     keyCounter++)
			{

				/***************/
				/* Private key */

				/***************/
				tPrivKey key = oReader.GetPrivKey(keyCounter);

				KeyId = (CK_ULONG) key.ulID;

				if (KeyId == CertId)
				{
					//      sprintf_s(clabel,sizeof(clabel), "Private Key %d (%s)", i+1, key.csLabel.c_str());
					sprintf_s(clabel, sizeof(clabel),
						  "%s", key.csLabel.c_str());

					ret = p11_add_slot_object(pSlot,
								  PRV_KEY,
								  sizeof
								  (PRV_KEY) /
								  sizeof
								  (CK_ATTRIBUTE),
								  CK_TRUE,
								  CKO_PRIVATE_KEY,
								  KeyId,
								  CK_TRUE,
								  &hObject);
					if (ret != CKR_OK)
						goto cleanup;

					//put some other attribute items allready so the key can be used for signing
					pObject =
						p11_get_slot_object(pSlot,
								    hObject);

					//type = (CK_ULONG) oReader.GetPrivKey(i).;
					//TODO fixed set to RSA
					ret = p11_set_attribute_value
						(pObject->pAttr,
						 pObject->count, CKA_LABEL,
						 (CK_VOID_PTR) clabel,
						 (CK_ULONG) strlen(clabel));
					if (ret != CKR_OK)
						goto cleanup;

					ret = p11_set_attribute_value
						(pObject->pAttr,
						 pObject->count, CKA_KEY_TYPE,
						 (CK_VOID_PTR) & keytype,
						 sizeof(CK_KEY_TYPE));
					if (ret != CKR_OK)
						goto cleanup;

					//TODO if (ulKeyUsage & SIGN)
					{
						ret = p11_set_attribute_value
							(pObject->pAttr,
							 pObject->count,
							 CKA_SIGN,
							 (CK_VOID_PTR) &
							 btrue,
							 sizeof(btrue));
						if (ret != CKR_OK)
							goto cleanup;
					}

					//TODO error in cal, size is in bits allready
					modsize = key.ulKeyLenBytes * 8;
					ret = p11_set_attribute_value
						(pObject->pAttr,
						 pObject->count,
						 CKA_MODULUS_BITS,
						 (CK_VOID_PTR) & modsize,
						 sizeof(CK_ULONG));
					if (ret != CKR_OK)
						goto cleanup;
					ret = p11_set_attribute_value
						(pObject->pAttr,
						 pObject->count,
						 CKA_EXTRACTABLE,
						 (CK_VOID_PTR) & bfalse,
						 sizeof(bfalse));
					if (ret != CKR_OK)
						goto cleanup;
					ret = p11_set_attribute_value
						(pObject->pAttr,
						 pObject->count, CKA_DERIVE,
						 (CK_VOID_PTR) & bfalse,
						 sizeof(bfalse));
					if (ret != CKR_OK)
						goto cleanup;

					/**************************************************/
					/* Public key corresponding to private key object */

					/**************************************************/
					ret = p11_add_slot_object(pSlot,
								  PUB_KEY,
								  sizeof
								  (PUB_KEY) /
								  sizeof
								  (CK_ATTRIBUTE),
								  CK_TRUE,
								  CKO_PUBLIC_KEY,
								  KeyId,
								  CK_FALSE,
								  &hObject);
					if (ret != CKR_OK)
						goto cleanup;

					pObject =
						p11_get_slot_object(pSlot,
								    hObject);

					//      sprintf_s(clabel,sizeof(clabel), "Public Key %d (%s)", i+1, key.csLabel.c_str());
					sprintf_s(clabel, sizeof(clabel),
						  "%s", key.csLabel.c_str());
					ret = p11_set_attribute_value
						(pObject->pAttr,
						 pObject->count, CKA_LABEL,
						 (CK_VOID_PTR) clabel,
						 (CK_ULONG) strlen(clabel));
					if (ret != CKR_OK)
						goto cleanup;
					ret = p11_set_attribute_value
						(pObject->pAttr,
						 pObject->count, CKA_KEY_TYPE,
						 (CK_VOID_PTR) & keytype,
						 sizeof(CK_KEY_TYPE));
					if (ret != CKR_OK)
						goto cleanup;
					ret = p11_set_attribute_value
						(pObject->pAttr,
						 pObject->count,
						 CKA_MODULUS_BITS,
						 (CK_VOID_PTR) & modsize,
						 sizeof(CK_ULONG));
					if (ret != CKR_OK)
						goto cleanup;
					ret = p11_set_attribute_value
						(pObject->pAttr,
						 pObject->count, CKA_DERIVE,
						 (CK_VOID_PTR) & bfalse,
						 sizeof(bfalse));
					if (ret != CKR_OK)
						goto cleanup;
				}
			}
		}
	}
	catch(CMWException e)
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
		log_trace(WHERE, "E: Invalid slot (%d)", hSlot);
		return (CKR_SLOT_ID_INVALID);
	}

	std::string szReader = pSlot->name;

	memset(cpin, 0, sizeof(cpin));
	if (pin && (l_pin < sizeof(cpin)))
		memcpy(cpin, pin, l_pin);

	std::string csPin = cpin;
	unsigned long ulRemaining = 0;
	unsigned long ulPinIdx = 0;

	try
	{
		CReader & oReader = oCardLayer->getReader(szReader);
		tPin tpin = oReader.GetPin(ulPinIdx);

		if (!oReader.
		    PinCmd(PIN_OP_VERIFY, tpin, csPin, "", ulRemaining))
		{
			if (ulRemaining == 0)
				ret = CKR_PIN_LOCKED;
			else
				ret = CKR_PIN_INCORRECT;
		}
	}
	catch(CMWException e)
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
		log_trace(WHERE, "E: Invalid slot (%d)", hSlot);
		return (CKR_SLOT_ID_INVALID);
	}

	std::string szReader = pSlot->name;
	//CReader &oReader = oCardLayer->getReader(szReader);

	/*TODO ??? oReader. */

	return (ret);
}

#undef WHERE




#define WHERE "cal_change_pin()"
CK_RV cal_change_pin(CK_SLOT_ID hSlot, int l_oldpin, CK_CHAR_PTR oldpin,
		     int l_newpin, CK_CHAR_PTR newpin)
{
	CK_RV ret = CKR_OK;
	P11_SLOT *pSlot = NULL;

	pSlot = p11_get_slot(hSlot);
	if (pSlot == NULL)
	{
		log_trace(WHERE, "E: Invalid slot (%d)", hSlot);
		return (CKR_SLOT_ID_INVALID);
	}

	try
	{
		std::string csPin = "";
		std::string csNewPin = "";
		std::string szReader = pSlot->name;

		CReader & oReader = oCardLayer->getReader(szReader);

		if (oldpin != NULL)
		{
			csPin = (char *) oldpin;
		}
		if (newpin != NULL)
		{
			csNewPin = (char *) newpin;
		}
		unsigned long ulRemaining = 0;

		tPin tpin = oReader.GetPin(0);

		if (!oReader.
		    PinCmd(PIN_OP_CHANGE, tpin, csPin, csNewPin, ulRemaining))
		{
			if (ulRemaining == 0)
				ret = CKR_PIN_LOCKED;
			else
				ret = CKR_PIN_INCORRECT;
		}
	}
	catch(CMWException e)
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
		log_trace(WHERE, "E: Invalid slot (%d)", hSlot);
		return (CKR_SLOT_ID_INVALID);
	}

	szReader = pSlot->name;
	try
	{
		CReader & oReader = oCardLayer->getReader(szReader);
		oATR = oReader.GetATR();
		oCardData = oReader.GetInfo();

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
					     strlen(BEID_OBJECTID_CARDDATA));
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
					     strlen(BEID_OBJECTID_CARDDATA));
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
					     strlen(BEID_OBJECTID_CARDDATA));
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
					     strlen(BEID_OBJECTID_CARDDATA));
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
					     strlen(BEID_OBJECTID_CARDDATA));
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
					     strlen(BEID_OBJECTID_CARDDATA));
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
					     strlen(BEID_OBJECTID_CARDDATA));
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
					     strlen(BEID_OBJECTID_CARDDATA));
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
						     sizeof(ID_DATA) /
						     sizeof(CK_ATTRIBUTE),
						     CK_TRUE, CKO_DATA,
						     CK_FALSE, &hObject,
						     (CK_VOID_PTR) plabel,
						     (CK_ULONG)
						     strlen(plabel),
						     (CK_VOID_PTR) data.
						     GetBytes(),
						     (CK_ULONG) data.Size(),
						     (CK_VOID_PTR)
						     BEID_OBJECTID_CARDDATA,
						     (CK_ULONG)
						     strlen
						     (BEID_OBJECTID_CARDDATA));
			if (ret != CKR_OK)
				goto cleanup;

			data.ClearContents();
			data.Append(oCardData.GetByte(23));
			data.Append(oCardData.GetByte(24));
			plabel = BEID_LABEL_DATA_ApplIntVersion;
			ret = p11_add_slot_ID_object(pSlot, ID_DATA,
						     sizeof(ID_DATA) /
						     sizeof(CK_ATTRIBUTE),
						     CK_TRUE, CKO_DATA,
						     CK_FALSE, &hObject,
						     (CK_VOID_PTR) plabel,
						     (CK_ULONG)
						     strlen(plabel),
						     (CK_VOID_PTR) data.
						     GetBytes(),
						     (CK_ULONG) data.Size(),
						     (CK_VOID_PTR)
						     BEID_OBJECTID_CARDDATA,
						     (CK_ULONG)
						     strlen
						     (BEID_OBJECTID_CARDDATA));
			if (ret != CKR_OK)
				goto cleanup;

			data.ClearContents();
			data.Append(oCardData.GetByte(25));
			data.Append(oCardData.GetByte(26));
			plabel = BEID_LABEL_DATA_PKCS15Version;
			ret = p11_add_slot_ID_object(pSlot, ID_DATA,
						     sizeof(ID_DATA) /
						     sizeof(CK_ATTRIBUTE),
						     CK_TRUE, CKO_DATA,
						     CK_FALSE, &hObject,
						     (CK_VOID_PTR) plabel,
						     (CK_ULONG)
						     strlen(plabel),
						     (CK_VOID_PTR) data.
						     GetBytes(),
						     (CK_ULONG) data.Size(),
						     (CK_VOID_PTR)
						     BEID_OBJECTID_CARDDATA,
						     (CK_ULONG)
						     strlen
						     (BEID_OBJECTID_CARDDATA));
			if (ret != CKR_OK)
				goto cleanup;
		} else
		{
			plabel = BEID_LABEL_DATA_ApplVersion;
			ret = p11_add_slot_ID_object(pSlot, ID_DATA,
						     sizeof(ID_DATA) /
						     sizeof(CK_ATTRIBUTE),
						     CK_TRUE, CKO_DATA,
						     CK_FALSE, &hObject,
						     (CK_VOID_PTR) plabel,
						     (CK_ULONG)
						     strlen(plabel),
						     (CK_VOID_PTR) & oByte,
						     (CK_ULONG) 1,
						     (CK_VOID_PTR)
						     BEID_OBJECTID_CARDDATA,
						     (CK_ULONG)
						     strlen
						     (BEID_OBJECTID_CARDDATA));
			if (ret != CKR_OK)
				goto cleanup;

			data.ClearContents();
			data.Append(oCardData.GetByte(22));
			data.Append(oCardData.GetByte(23));
			plabel = BEID_LABEL_DATA_GlobOSVersion;
			ret = p11_add_slot_ID_object(pSlot, ID_DATA,
						     sizeof(ID_DATA) /
						     sizeof(CK_ATTRIBUTE),
						     CK_TRUE, CKO_DATA,
						     CK_FALSE, &hObject,
						     (CK_VOID_PTR) plabel,
						     (CK_ULONG)
						     strlen(plabel),
						     (CK_VOID_PTR) data.
						     GetBytes(),
						     (CK_ULONG) data.Size(),
						     (CK_VOID_PTR)
						     BEID_OBJECTID_CARDDATA,
						     (CK_ULONG)
						     strlen
						     (BEID_OBJECTID_CARDDATA));
			if (ret != CKR_OK)
				goto cleanup;

			oByte = oCardData.GetByte(24);
			plabel = BEID_LABEL_DATA_ApplIntVersion;
			ret = p11_add_slot_ID_object(pSlot, ID_DATA,
						     sizeof(ID_DATA) /
						     sizeof(CK_ATTRIBUTE),
						     CK_TRUE, CKO_DATA,
						     CK_FALSE, &hObject,
						     (CK_VOID_PTR) plabel,
						     (CK_ULONG)
						     strlen(plabel),
						     (CK_VOID_PTR) & oByte,
						     (CK_ULONG) 1,
						     (CK_VOID_PTR)
						     BEID_OBJECTID_CARDDATA,
						     (CK_ULONG)
						     strlen
						     (BEID_OBJECTID_CARDDATA));
			if (ret != CKR_OK)
				goto cleanup;

			oByte = oCardData.GetByte(25);
			plabel = BEID_LABEL_DATA_PKCS1Support;
			ret = p11_add_slot_ID_object(pSlot, ID_DATA,
						     sizeof(ID_DATA) /
						     sizeof(CK_ATTRIBUTE),
						     CK_TRUE, CKO_DATA,
						     CK_FALSE, &hObject,
						     (CK_VOID_PTR) plabel,
						     (CK_ULONG)
						     strlen(plabel),
						     (CK_VOID_PTR) & oByte,
						     (CK_ULONG) 1,
						     (CK_VOID_PTR)
						     BEID_OBJECTID_CARDDATA,
						     (CK_ULONG)
						     strlen
						     (BEID_OBJECTID_CARDDATA));
			if (ret != CKR_OK)
				goto cleanup;

			oByte = oCardData.GetByte(26);
			plabel = BEID_LABEL_DATA_KeyExchangeVersion;
			ret = p11_add_slot_ID_object(pSlot, ID_DATA,
						     sizeof(ID_DATA) /
						     sizeof(CK_ATTRIBUTE),
						     CK_TRUE, CKO_DATA,
						     CK_FALSE, &hObject,
						     (CK_VOID_PTR) plabel,
						     (CK_ULONG)
						     strlen(plabel),
						     (CK_VOID_PTR) & oByte,
						     (CK_ULONG) 1,
						     (CK_VOID_PTR)
						     BEID_OBJECTID_CARDDATA,
						     (CK_ULONG)
						     strlen
						     (BEID_OBJECTID_CARDDATA));
			if (ret != CKR_OK)
				goto cleanup;
		}


		oByte = oCardData.GetByte(27);
		plabel = BEID_LABEL_DATA_ApplLifeCycle;
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
					     strlen(BEID_OBJECTID_CARDDATA));
		if (ret != CKR_OK)
			goto cleanup;

		//              data.ClearContents();
		//              data = oCardData.GetBytes(28);
		//              plabel = BEID_LABEL_DATA_FILE;
		//              ret = p11_add_slot_ID_object(pSlot, ID_DATA, sizeof(ID_DATA)/sizeof(CK_ATTRIBUTE), CK_TRUE, CKO_DATA, 0, CK_FALSE, &hObject,
		//              (CK_VOID_PTR)plabel, (CK_ULONG)strlen(plabel),(CK_VOID_PTR) data.GetBytes(),(CK_ULONG)data.Size());
		//              if (ret) goto cleanup;
		//
	}
	catch(CMWException e)
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

	CK_ULONG hObject = 0;

	pSlot = p11_get_slot(hSlot);
	if (pSlot == NULL)
	{
		log_trace(WHERE, "E: Invalid slot (%d)", hSlot);
		return (CKR_SLOT_ID_INVALID);
	}

	szReader = pSlot->name;
	try
	{
		CReader & oReader = oCardLayer->getReader(szReader);
		switch (dataType)
		{
			case CACHED_DATA_TYPE_ALL_DATA:
			case CACHED_DATA_TYPE_ID:
				oFileData = oReader.ReadFile(BEID_FILE_ID);

				plabel = BEID_LABEL_DATA_FILE;
				pobjectID = (char *) BEID_OBJECTID_ID;
				/* XXX the const-ness of pobjectID and plabel should
				 * ideally not be cast away, but it goes pretty deep.
				 * Not Now(TM). */
				ret = p11_add_slot_ID_object(pSlot, ID_DATA,
							     sizeof(ID_DATA) /
							     sizeof
							     (CK_ATTRIBUTE),
							     CK_TRUE,
							     CKO_DATA,
							     CK_FALSE,
							     &hObject,
							     (CK_VOID_PTR)
							     plabel,
							     (CK_ULONG)
							     strlen(plabel),
							     (CK_VOID_PTR)
							     oFileData.
							     GetBytes(),
							     (CK_ULONG)
							     oFileData.Size(),
							     (CK_VOID_PTR)
							     pobjectID,
							     (CK_ULONG)
							     strlen
							     (pobjectID));
				if (ret)
					goto cleanup;

				oTLVBuffer.ParseTLV(oFileData.GetBytes(),
						    oFileData.Size());

				nrOfItems =
					sizeof(ID_LABELS) /
					sizeof(BEID_DATA_LABELS_NAME);

				for (i = 0; i < nrOfItems; i++)
				{
					ulLen = sizeof(cBuffer);
					memset(cBuffer, 0, ulLen);
					if(oTLVBuffer.FillUTF8Data(ID_LABELS[i].tag, cBuffer, &ulLen)) {
						plabel = ID_LABELS[i].name;
						ret = p11_add_slot_ID_object(pSlot, ID_DATA, sizeof(ID_DATA) / sizeof(CK_ATTRIBUTE),
									     CK_TRUE, CKO_DATA, CK_FALSE, &hObject, (CK_VOID_PTR)plabel,
									     (CK_ULONG)strlen(plabel), (CK_VOID_PTR)cBuffer, ulLen,
									     (CK_VOID_PTR) pobjectID, (CK_ULONG)strlen(pobjectID));
						if (ret)
							goto cleanup;
					}
				}
				if (dataType != CACHED_DATA_TYPE_ALL_DATA)
				{
					break;
				}
			case CACHED_DATA_TYPE_ADDRESS:
				oFileData =
					oReader.ReadFile(BEID_FILE_ADDRESS);
				plabel = BEID_LABEL_ADDRESS_FILE;
				pobjectID = BEID_OBJECTID_ADDRESS;
				ret = p11_add_slot_ID_object(pSlot, ID_DATA, sizeof(ID_DATA) / sizeof(CK_ATTRIBUTE), CK_TRUE, CKO_DATA,
							     CK_FALSE, &hObject, (CK_VOID_PTR)plabel, (CK_ULONG)strlen(plabel),
							     (CK_VOID_PTR)oFileData.GetBytes(), (CK_ULONG)oFileData.Size(),
							     (CK_VOID_PTR)pobjectID, (CK_ULONG)strlen(pobjectID));
				if (ret)
					goto cleanup;
				oTLVBufferAddress.ParseTLV(oFileData.GetBytes(),
							   oFileData.Size());
				nrOfItems =
					sizeof(ADDRESS_LABELS) /
					sizeof(BEID_DATA_LABELS_NAME);
				for (i = 0; i < nrOfItems; i++)
				{
					ulLen = sizeof(cBuffer);
					memset(cBuffer, 0, ulLen);
					if(oTLVBufferAddress.FillUTF8Data(ADDRESS_LABELS[i].tag, cBuffer, &ulLen)) {
						plabel = ADDRESS_LABELS[i].name;
						ret = p11_add_slot_ID_object(pSlot, ID_DATA, sizeof(ID_DATA) / sizeof(CK_ATTRIBUTE), CK_TRUE,
									     CKO_DATA, CK_FALSE, &hObject, (CK_VOID_PTR)plabel,
									     (CK_ULONG)strlen(plabel), (CK_VOID_PTR)cBuffer, ulLen,
									     (CK_VOID_PTR)pobjectID, (CK_ULONG)strlen(pobjectID));
						if (ret)
							goto cleanup;
					}
				}
				if (dataType != CACHED_DATA_TYPE_ALL_DATA)
				{
					break;
				}
			case CACHED_DATA_TYPE_PHOTO:
				plabel = BEID_LABEL_PHOTO;
				pobjectID = BEID_OBJECTID_PHOTO;
				oFileData = oReader.ReadFile(BEID_FILE_PHOTO);
				ret = p11_add_slot_ID_object(pSlot, ID_DATA,
							     sizeof(ID_DATA) /
							     sizeof
							     (CK_ATTRIBUTE),
							     CK_TRUE,
							     CKO_DATA,
							     CK_FALSE,
							     &hObject,
							     (CK_VOID_PTR)
							     plabel,
							     (CK_ULONG)
							     strlen(plabel),
							     (CK_VOID_PTR)
							     oFileData.
							     GetBytes(),
							     (CK_ULONG)
							     oFileData.Size(),
							     (CK_VOID_PTR)
							     pobjectID,
							     (CK_ULONG)
							     strlen
							     (BEID_OBJECTID_PHOTO));
				if (ret)
					goto cleanup;
				if (dataType != CACHED_DATA_TYPE_ALL_DATA)
				{
					break;
				}
			case CACHED_DATA_TYPE_RNCERT:
				oFileData =
					oReader.ReadFile(BEID_FILE_CERT_RRN);
				plabel = BEID_LABEL_CERT_RN;
				pobjectID = BEID_OBJECTID_RNCERT;
				ret = p11_add_slot_ID_object(pSlot, ID_DATA,
							     sizeof(ID_DATA) /
							     sizeof
							     (CK_ATTRIBUTE),
							     CK_TRUE,
							     CKO_DATA,
							     CK_FALSE,
							     &hObject,
							     (CK_VOID_PTR)
							     plabel,
							     (CK_ULONG)
							     strlen(plabel),
							     (CK_VOID_PTR)
							     oFileData.
							     GetBytes(),
							     (CK_ULONG)
							     oFileData.Size(),
							     (CK_VOID_PTR)
							     pobjectID,
							     (CK_ULONG)
							     strlen
							     (BEID_OBJECTID_RNCERT));
				if (ret)
					goto cleanup;
				if (dataType != CACHED_DATA_TYPE_ALL_DATA)
				{
					break;
				}
			case CACHED_DATA_TYPE_SIGN_DATA_FILE:
				plabel = BEID_LABEL_SGN_RN;
				oFileData =
					oReader.ReadFile(BEID_FILE_ID_SIGN);
				ret = p11_add_slot_ID_object(pSlot, ID_DATA,
							     sizeof(ID_DATA) /
							     sizeof
							     (CK_ATTRIBUTE),
							     CK_TRUE,
							     CKO_DATA,
							     CK_FALSE,
							     &hObject,
							     (CK_VOID_PTR)
							     plabel,
							     (CK_ULONG)
							     strlen(plabel),
							     (CK_VOID_PTR)
							     oFileData.
							     GetBytes(),
							     (CK_ULONG)
							     oFileData.Size(),
							     (CK_VOID_PTR)
							     BEID_OBJECTID_SIGN_DATA_FILE,
							     (CK_ULONG)
							     strlen
							     (BEID_OBJECTID_SIGN_DATA_FILE));
				if (ret)
					goto cleanup;
				if (dataType != CACHED_DATA_TYPE_ALL_DATA)
				{
					break;
				}
			case CACHED_DATA_TYPE_SIGN_ADDRESS_FILE:
				plabel = BEID_LABEL_SGN_ADDRESS;
				oFileData =
					oReader.
					ReadFile(BEID_FILE_ADDRESS_SIGN);
				ret = p11_add_slot_ID_object(pSlot, ID_DATA,
							     sizeof(ID_DATA) /
							     sizeof
							     (CK_ATTRIBUTE),
							     CK_TRUE,
							     CKO_DATA,
							     CK_FALSE,
							     &hObject,
							     (CK_VOID_PTR)
							     plabel,
							     (CK_ULONG)
							     strlen(plabel),
							     (CK_VOID_PTR)
							     oFileData.
							     GetBytes(),
							     (CK_ULONG)
							     oFileData.Size(),
							     (CK_VOID_PTR)
							     BEID_OBJECTID_SIGN_ADDRESS_FILE,
							     (CK_ULONG)
							     strlen
							     (BEID_OBJECTID_SIGN_ADDRESS_FILE));
				if (ret)
					goto cleanup;
				if (dataType != CACHED_DATA_TYPE_ALL_DATA)
				{
					break;
				}
			default:
				break;
		}
	}
	catch(CMWException e)
	{
		return (cal_translate_error(WHERE, e.GetError()));
	}
	catch( ...)
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
	CByteArray oCertData;
	tCert cert;
	tPrivKey key;

	std::string szReader;
	P11_SLOT *pSlot = NULL;

	memset(&certinfo, 0, sizeof(T_CERT_INFO));
	pSlot = p11_get_slot(hSlot);
	if (pSlot == NULL)
	{
		log_trace(WHERE, "E: Invalid slot (%d)", hSlot);
		return (CKR_SLOT_ID_INVALID);
	}

	szReader = pSlot->name;

	ret = cal_update_token(hSlot, &status);
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



	if (pCertObject != NULL)
	{
		try
		{
			CReader & oReader = oCardLayer->getReader(szReader);

			cert = oReader.GetCertByID(*pID);

			//bValid duidt aan if cert met deze ID
			if (cert.bValid)
				oCertData = oReader.ReadFile(cert.csPath);
			else
			{
				return (CKR_DEVICE_ERROR);
			}

			if (cert_get_info
			    (oCertData.GetBytes(), oCertData.Size(),
			     &certinfo) < 0)
			{
				// ASN.1 parser failed. Assume hardware failure.
				ret = CKR_DEVICE_ERROR;
				goto cleanup;
			}

			ret = p11_set_attribute_value(pCertObject->pAttr,
						      pCertObject->count,
						      CKA_SUBJECT,
						      (CK_VOID_PTR) certinfo.
						      subject,
						      (CK_ULONG) certinfo.
						      l_subject);
			if (ret != CKR_OK)
				goto cleanup;
			ret = p11_set_attribute_value(pCertObject->pAttr,
						      pCertObject->count,
						      CKA_ISSUER,
						      (CK_VOID_PTR) certinfo.
						      issuer,
						      (CK_ULONG) certinfo.
						      l_issuer);
			if (ret != CKR_OK)
				goto cleanup;
			ret = p11_set_attribute_value(pCertObject->pAttr,
						      pCertObject->count,
						      CKA_SERIAL_NUMBER,
						      (CK_VOID_PTR) certinfo.
						      serial,
						      (CK_ULONG) certinfo.
						      l_serial);
			if (ret != CKR_OK)
				goto cleanup;
			//use real length from decoder here instead of lg from cal
			ret = p11_set_attribute_value(pCertObject->pAttr,
						      pCertObject->count,
						      CKA_VALUE,
						      (CK_VOID_PTR) oCertData.
						      GetBytes(),
						      (CK_ULONG) certinfo.
						      lcert);
			if (ret != CKR_OK)
				goto cleanup;
			//TODO Check this in the cal if we can be sure that the certificate can be trusted and not be modified on the card
			ret = p11_set_attribute_value(pCertObject->pAttr,
						      pCertObject->count,
						      CKA_TRUSTED,
						      (CK_VOID_PTR) & btrue,
						      sizeof(btrue));
			if (ret != CKR_OK)
				goto cleanup;

			pCertObject->state = P11_CACHED;

			key = oReader.GetPrivKeyByID(*pID);

			if (pPrivKeyObject != NULL)
			{
				ret = p11_set_attribute_value(pPrivKeyObject->
							      pAttr,
							      pPrivKeyObject->
							      count,
							      CKA_SENSITIVE,
							      (CK_VOID_PTR) &
							      btrue,
							      sizeof(btrue));
				if (ret != CKR_OK)
					goto cleanup;
				ret = p11_set_attribute_value(pPrivKeyObject->
							      pAttr,
							      pPrivKeyObject->
							      count,
							      CKA_DECRYPT,
							      (CK_VOID_PTR) &
							      bfalse,
							      sizeof(bfalse));
				if (ret != CKR_OK)
					goto cleanup;
				ret = p11_set_attribute_value(pPrivKeyObject->
							      pAttr,
							      pPrivKeyObject->
							      count,
							      CKA_SIGN_RECOVER,
							      (CK_VOID_PTR) &
							      bfalse,
							      sizeof
							      (CK_BBOOL));
				if (ret != CKR_OK)
					goto cleanup;
				ret = p11_set_attribute_value(pPrivKeyObject->
							      pAttr,
							      pPrivKeyObject->
							      count,
							      CKA_UNWRAP,
							      (CK_VOID_PTR) &
							      bfalse,
							      sizeof
							      (CK_BBOOL));
				if (ret != CKR_OK)
					goto cleanup;
				ret = p11_set_attribute_value(pPrivKeyObject->
							      pAttr,
							      pPrivKeyObject->
							      count,
							      CKA_SUBJECT,
							      (CK_VOID_PTR)
							      certinfo.
							      subject,
							      (CK_ULONG)
							      certinfo.
							      l_subject);
				if (ret != CKR_OK)
					goto cleanup;
				ret = p11_set_attribute_value(pPrivKeyObject->
							      pAttr,
							      pPrivKeyObject->
							      count,
							      CKA_MODULUS,
							      (CK_VOID_PTR)
							      certinfo.mod,
							      (CK_ULONG)
							      certinfo.l_mod);
				if (ret != CKR_OK)
					goto cleanup;
				ret = p11_set_attribute_value(pPrivKeyObject->
							      pAttr,
							      pPrivKeyObject->
							      count,
							      CKA_PUBLIC_EXPONENT,
							      (CK_VOID_PTR)
							      certinfo.exp,
							      (CK_ULONG)
							      certinfo.l_exp);
				if (ret != CKR_OK)
					goto cleanup;
				pPrivKeyObject->state = P11_CACHED;
			}
			if (pPubKeyObject != NULL)
			{
				ret = p11_set_attribute_value(pPubKeyObject->
							      pAttr,
							      pPubKeyObject->
							      count,
							      CKA_SENSITIVE,
							      (CK_VOID_PTR) &
							      btrue,
							      sizeof(btrue));
				if (ret != CKR_OK)
					goto cleanup;
				ret = p11_set_attribute_value(pPubKeyObject->
							      pAttr,
							      pPubKeyObject->
							      count,
							      CKA_VERIFY,
							      (CK_VOID_PTR) &
							      btrue,
							      sizeof(btrue));
				if (ret != CKR_OK)
					goto cleanup;
				ret = p11_set_attribute_value(pPubKeyObject->
							      pAttr,
							      pPubKeyObject->
							      count,
							      CKA_ENCRYPT,
							      (CK_VOID_PTR) &
							      bfalse,
							      sizeof(bfalse));
				if (ret != CKR_OK)
					goto cleanup;
				ret = p11_set_attribute_value(pPubKeyObject->
							      pAttr,
							      pPubKeyObject->
							      count, CKA_WRAP,
							      (CK_VOID_PTR) &
							      bfalse,
							      sizeof
							      (CK_BBOOL));
				if (ret != CKR_OK)
					goto cleanup;

				ret = p11_set_attribute_value(pPubKeyObject->
							      pAttr,
							      pPubKeyObject->
							      count,
							      CKA_SUBJECT,
							      (CK_VOID_PTR)
							      certinfo.
							      subject,
							      (CK_ULONG)
							      certinfo.
							      l_subject);
				if (ret != CKR_OK)
					goto cleanup;
				ret = p11_set_attribute_value(pPubKeyObject->
							      pAttr,
							      pPubKeyObject->
							      count,
							      CKA_MODULUS,
							      (CK_VOID_PTR)
							      certinfo.mod,
							      certinfo.l_mod);
				if (ret != CKR_OK)
					goto cleanup;
				ret = p11_set_attribute_value(pPubKeyObject->
							      pAttr,
							      pPubKeyObject->
							      count,
							      CKA_VALUE,
							      (CK_VOID_PTR)
							      certinfo.pkinfo,
							      certinfo.
							      l_pkinfo);
				if (ret != CKR_OK)
					goto cleanup;
				ret = p11_set_attribute_value(pPubKeyObject->
							      pAttr,
							      pPubKeyObject->
							      count,
							      CKA_PUBLIC_EXPONENT,
							      (CK_VOID_PTR)
							      certinfo.exp,
							      certinfo.l_exp);
				if (ret != CKR_OK)
					goto cleanup;
				//TODO test if we can set the trusted flag...
				ret = p11_set_attribute_value(pPubKeyObject->
							      pAttr,
							      pPubKeyObject->
							      count,
							      CKA_TRUSTED,
							      (CK_VOID_PTR) &
							      btrue,
							      sizeof(btrue));
				if (ret != CKR_OK)
					goto cleanup;

				pPubKeyObject->state = P11_CACHED;
			}
		}
		catch(CMWException e)
		{
			cert_free_info(&certinfo);
			return (cal_translate_error(WHERE, e.GetError()));
		}
		catch( ...)
		{
			log_trace(WHERE, "E: unkown exception thrown");
			cert_free_info(&certinfo);
			return (CKR_FUNCTION_FAILED);
		}
	}
	if (ret != 0)
	{
		cert_free_info(&certinfo);
		return (CKR_DEVICE_ERROR);
	}

      cleanup:
	cert_free_info(&certinfo);
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
		log_trace(WHERE, "E: Invalid slot (%d)", hSlot);
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
		tPrivKey key = oReader.GetPrivKeyByID(pSignData->id);

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
			default:
				ret = CKR_MECHANISM_INVALID;
				goto cleanup;
		}

		oDataOut = oReader.Sign(key, algo, oData);
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
			case P11_CARD_OTHER:	//other card has been inserted
			default:
				//        return (CKR_TOKEN_NOT_PRESENT);
				return (CKR_DEVICE_REMOVED);
				//        return (CKR_SESSION_HANDLE_INVALID);
		}
	}

	try
	{
		//previous state is STILL_PRESENT so get new state to see if this has changed
		ret = cal_update_token(pSession->hslot, &status);
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
	catch(CMWException e)
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
CK_RV cal_update_token(CK_SLOT_ID hSlot, int *pStatus)
{
	P11_OBJECT *pObject = NULL;
	CK_RV ret = CKR_OK;

	//int status;
	unsigned int i = 0;
	P11_SLOT *pSlot = NULL;

#ifdef PKCS11_FF
	//our fake firefox slots should not return CKR_SLOT_ID_INVALID, or firefox will ignore them
	if ((hSlot >= (CK_ULONG) (p11_get_nreaders() - 1))
	    && (hSlot <= (CK_ULONG) cal_getgnFFReaders()))
	{
		*pStatus = P11_CARD_NOT_PRESENT;
		return ret;
	}
#endif

	pSlot = p11_get_slot(hSlot);
	if (pSlot == NULL)
	{
		log_trace(WHERE, "E: Invalid slot (%d)", hSlot);
		return (CKR_SLOT_ID_INVALID);
	}

	try
	{
		std::string reader = pSlot->name;
		CReader & oReader = oCardLayer->getReader(reader);
		*pStatus = cal_map_status(oReader.Status(true));

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
			if ((*pStatus == P11_CARD_OTHER)
			    || (*pStatus == P11_CARD_INSERTED))
			{
				//(re)initialize objects
#ifdef PKCS11_FF
				ret = cal_init_objects(pSlot);
				if (ret != CKR_OK)
				{
					log_trace(WHERE,
						  "E: cal_init_objects() returned %s",
						  log_map_error(ret));
				}
#endif
			}
		} else
		{
			if (oReader.GetCardType() == CARD_UNKNOWN)
			{
				return (CKR_TOKEN_NOT_RECOGNIZED);
			}
		}
	}
	catch(CMWException e)
	{
		return (cal_translate_error(WHERE, e.GetError()));
	}
	catch( ...)
	{
		log_trace(WHERE, "E: unkown exception thrown");
		return (CKR_SESSION_HANDLE_INVALID);
	}
	return ret;
}

#undef WHERE


/*
#define WHERE "cal_wait_for_slot_event()"
CK_RV cal_wait_for_slot_event(int block)
{
CK_RV ret = CKR_OK;

try
{
if (block){
oReadersInfo->CheckTheReaderEvents(TIMEOUT_INFINITE);
}
else{
oReadersInfo->CheckTheReaderEvents(0);
}
}
catch (CMWException e)
{
CLEANUP(cal_translate_error(WHERE, e.GetError()));
}
catch (...)
{
log_trace(WHERE, "E: unkown exception thrown");
CLEANUP(CKR_FUNCTION_FAILED);
}
cleanup:

return(ret);
}
#undef WHERE
*/

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
				//incase the upnp reader detected a reader event, we report it,
				if (i == (p11_get_nreaders() - 1))
				{
					//the '\\Pnp\\Notification' reader reported an event, check if number of readers is higher
					//so the list of readers can be adjusted
					//other reader's events will be ignored as the reader list will get refreshed
					if (oReadersInfo->IsReaderInserted(i))	//-1 as we don't count the pnp reader
					{
						if (gnFFReaders == 0)
						{
							gnFFReaders =
								p11_get_nreaders
								() + 1;
						} else
						{
							gnFFReaders++;
						}
						*ph = gnFFReaders - 1;
					}
					//if this is the only reader change, report the reader removal as a change of the upnp slot
					else if (*ph == -1)
					{
						*ph = i;
					}
					ret = CKR_OK;
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
							if (oReadersInfo->
							    CardPresent(i))
								pSlot->ievent
									= 1;
							else
								pSlot->ievent
									= -1;
						}
					}
				}
			}
		}
	}
	catch(CMWException e)
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
			CReadersInfo *pNewReadersInfo =
				new CReadersInfo(oCardLayer->ListReaders());
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
			oReadersInfo =
				new CReadersInfo(oCardLayer->ListReaders());
		}
		//new reader list, so please stop the scardgetstatuschange that is waiting on the old list
		//oCardLayer->CancelActions();
	}
	catch(CMWException e)
	{
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
		log_trace(WHERE, "E: p11_init_slots() returns %d", ret);

	return (ret);
}

#undef WHERE

CK_RV cal_translate_error(const char *WHERE, long err)
{
	log_trace(WHERE, "E: MiddlewareException thrown: 0x%0x", err);

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
			return (CKR_FUNCTION_FAILED);
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

		/** A card is present but we can't connect in a normal way (e.g. SIS card) */
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
	oReadersInfo->GetReaderStates(txReaderStates, MAX_READERS,
				      &ulnReaders);

	try
	{
		if (block)
		{
			p11_unlock();
/*#ifdef PKCS11_FF
			while ((p11_get_init() == BEIDP11_INITIALIZED)
			       && (lret == SCARD_E_TIMEOUT))
			{
				lret = oCardLayer->
					GetStatusChange(TIMEOUT_POLL,
							txReaderStates,
							ulnReaders);
			}
#else*/
			oCardLayer->GetStatusChange(TIMEOUT_INFINITE,
						    txReaderStates,
						    ulnReaders);
//#endif
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
				CLEANUP(CKR_FUNCTION_FAILED);
			}
		} else
		{
			oCardLayer->GetStatusChange(0, txReaderStates,
						    ulnReaders);
		}
	}
	catch(CMWException e)
	{
		if (block)
		{
			p11_lock();
		}
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

void cal_free_reader_states(SCARD_READERSTATEA * txReaderStates,
			    unsigned long ulnReaders)
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
