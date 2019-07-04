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
//--module C:\develop\proj\eidmw\eidmw\_Binaries\Debug\beidpkcs11.dll -t -l 

#include <stdlib.h>
#include <string.h>
#include "beid_p11.h"
#include "util.h"
#include "pkcs11log.h"
#include "p11.h"
#include "cal.h"

#ifndef WIN32
//linux config file
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#endif

#define LOG_MAX_REC  10

extern CK_FUNCTION_LIST pkcs11_function_list;
//extern void *logmutex;

//static int g_final = 0; /* Belpic */

//  CMutex gMutex;
//  CMutex SlotMutex[MAX_READERS];

/* If there's no card reader present, this function used to
* return CKR_DEVICE_ERROR, which causes Mozilla and Firefox
* to crash.
* So now we let this function (and C_GetInfo() as well)
* always return OK, and do the real initialization when
* C_GetSlotList() is called, this function is logically
* called immediately after C_Initialize() and/or C_GetInfo().
*/

#define WHERE "C_Initialize()"
CK_RV C_Initialize(CK_VOID_PTR pReserved)
{
	CK_RV ret = CKR_OK;
	CK_C_INITIALIZE_ARGS_PTR p_args;
	unsigned char initial_state = p11_get_init();

	log_init(DEFAULT_LOG_FILE, LOG_LEVEL_PKCS11_NONE);

	log_trace(WHERE, "I: enter pReserved = %p",pReserved);
	if (p11_get_init() != BEIDP11_NOT_INITIALIZED)
	{
		ret = CKR_CRYPTOKI_ALREADY_INITIALIZED;
		log_trace(WHERE, "I: Module is allready initialized");
	}
	else
	{
		//g_init = BEIDP11_INITIALIZED;
		p11_set_init(BEIDP11_INITIALIZING);
		if (pReserved != NULL)
		{
			p_args = (CK_C_INITIALIZE_ARGS *)pReserved;

			if(p_args->pReserved != NULL)
			{
				ret = CKR_ARGUMENTS_BAD;
				goto cleanup;
			}
			if(	(p_args->CreateMutex == NULL) || (p_args->DestroyMutex == NULL) || \
				(p_args->LockMutex == NULL) || (p_args->UnlockMutex == NULL)	)
			{
				log_trace(WHERE, "S: use supplied locking mechanism");
				//If some, but not all, of the supplied function pointers to C_Initialize are non-NULL_PTR, 
				//then C_Initialize should return with the value CKR_ARGUMENTS_BAD.
				if(!((p_args->CreateMutex == NULL) && (p_args->DestroyMutex == NULL) && \
					(p_args->LockMutex == NULL) && (p_args->UnlockMutex == NULL)))
				{
					ret = CKR_ARGUMENTS_BAD;
					goto cleanup;
				}
			}
			log_trace(WHERE, "S: p11_init_lock");
			p11_init_lock(p_args);
		}
		cal_init();
		p11_set_init(BEIDP11_INITIALIZED);
		log_trace(WHERE, "S: Initialize this PKCS11 Module");
		log_trace(WHERE, "S: =============================");
/*#ifdef PKCS11_FF
	cal_init_pcsc();
#endif*/
	}

cleanup:
	log_trace(WHERE, "I: leave, ret = %i",ret);
	if (ret != CKR_OK) {
		p11_set_init(initial_state);
	}
	return ret;
}
#undef WHERE



#define WHERE "C_Finalize()"
CK_RV C_Finalize(CK_VOID_PTR pReserved)
{
	CK_RV ret = CKR_OK;
	log_trace(WHERE, "I: enter");

	if (p11_get_init() != BEIDP11_INITIALIZED)
	{
		log_trace(WHERE, "I: leave, CKR_CRYPTOKI_NOT_INITIALIZED");
		return (CKR_CRYPTOKI_NOT_INITIALIZED);
	}		

	if (pReserved != NULL)
	{
		log_trace(WHERE, "I: leave, CKR_ARGUMENTS_BAD");
		return (CKR_ARGUMENTS_BAD);
	}

	p11_lock();

	//g_final = 0; /* Belpic */
	p11_set_init(BEIDP11_DEINITIALIZING);

	p11_close_sessions_finalize();

	cal_close();

	/* Release and destroy the mutex */
	// mutex might still be in use by C_waitforslotlist

	p11_free_lock();

	p11_set_init(BEIDP11_NOT_INITIALIZED);
	// util_clean_lock(&logmutex);
	log_trace(WHERE, "I: p11_free_lock()");
	log_trace(WHERE, "I: leave, ret = %i",ret);
	return ret;
}
#undef WHERE




#define WHERE "C_GetInfo()"
CK_RV C_GetInfo(CK_INFO_PTR pInfo)
{
	CK_RV ret = CKR_OK;
	log_trace(WHERE, "I: enter");
	if (pInfo == NULL_PTR)
	{
		ret = CKR_ARGUMENTS_BAD;
		goto cleanup;
	}

	log_trace(WHERE, "S: C_GetInfo()");

	pInfo->cryptokiVersion.major = 2;
	pInfo->cryptokiVersion.minor = 40;//0x28
	strcpy_n(pInfo->manufacturerID,  "Belgium Government",  sizeof(pInfo->manufacturerID), ' ');
	pInfo->flags = 0;
	strcpy_n(pInfo->libraryDescription, "Belgium eID PKCS#11 interface v2", sizeof(pInfo->libraryDescription), ' ');
	pInfo->libraryVersion.major = 5;
	pInfo->libraryVersion.minor = 0;

cleanup:
	log_trace(WHERE, "I: leave, ret = %i",ret);
	return ret;
}       
#undef WHERE



#define WHERE "C_GetFunctionList()"
CK_RV C_GetFunctionList(CK_FUNCTION_LIST_PTR_PTR ppFunctionList)
{
	log_trace(WHERE, "I: enter");
	log_trace(WHERE, "S: C_GetFunctionList()");

	if (ppFunctionList == NULL_PTR)
	{
		log_trace(WHERE, "I: leave, CKR_ARGUMENTS_BAD");
		return CKR_ARGUMENTS_BAD;
	}

	*ppFunctionList = &pkcs11_function_list;

	log_trace(WHERE, "I: leave, CKR_OK");
	return CKR_OK;
}
#undef WHERE



#define WHERE "C_GetSlotList()"
CK_RV C_GetSlotList(CK_BBOOL       tokenPresent,  /* only slots with token present */
	CK_SLOT_ID_PTR pSlotList,     /* receives the array of slot IDs */
	CK_ULONG_PTR   pulCount)      /* receives the number of slots */
{

	P11_SLOT *pSlot;
	CK_RV ret = CKR_OK;
	int h;
	CK_ULONG c = 0; 
	static int l=0;

	log_trace(WHERE, "I: enter");

	if (p11_get_init() != BEIDP11_INITIALIZED)
	{
		log_trace(WHERE, "I: leave, CKR_CRYPTOKI_NOT_INITIALIZED");
		return (CKR_CRYPTOKI_NOT_INITIALIZED);
	}

	p11_lock();
	log_trace(WHERE, "I: p11_lock() acquired");

	if (++l<LOG_MAX_REC)
		log_trace(WHERE, "S: C_GetSlotList()");

	if (pulCount == NULL_PTR)
	{
		ret = CKR_ARGUMENTS_BAD;
		goto cleanup;
	}

	if(pSlotList == NULL){
		ret = cal_refresh_readers();
	}
	//init slots allready done
	//update info on tokens in slot, could be removed if thread keeps track of these token states
	//BUG in Adobe Acrobat reader: adobe asks for slots with pSlotList = NULL, so only nr of slots will be returned. This is ok.
	//a second time Adobe calls this, pSlotList still is NULL, so the array with SlotIDs cannot be returned, again, nr of slots is returned.
	//Adobe just assumes that the first slot has ID=0 !!! and uses this ID=0 for all further actions.
	//to overcome this problem, we start our SlotIDs from 0 and not 1 !!!

	log_trace(WHERE, "I: h=0");

	for (h=0; h < p11_get_nreaders(); h++)
	{
		log_trace(WHERE, "I: h=%i",h);
		pSlot = p11_get_slot(h);

		if (l < LOG_MAX_REC) 
			log_trace(WHERE, "I: slot[%d]: %s", h, pSlot->name);

		if (tokenPresent == CK_TRUE)
		{
			int pPresent = 0;
			ret = cal_token_present(h, &pPresent);
			if(ret != CKR_OK && ret != CKR_TOKEN_NOT_RECOGNIZED)
			{
				goto cleanup;
			}
			ret = CKR_OK; // CKR_TOKEN_NOT_RECOGNIZED is useless as a return value from this function, and we just ensured that nothing else is possible
			if (pPresent)
			{
				log_trace(WHERE, "I: cal_token_present");
				c++;
				if ((pSlotList != NULL_PTR) && (c <= *pulCount) )
					pSlotList[c-1] =  h;
			}
			continue;
		}
		else
		{
			//get all slots
			c++;
			if ((pSlotList != NULL_PTR) && (c <= *pulCount) )
			{
				pSlotList[c-1] =  h;
			}
			continue;
		}
	} //end for

	//if more slots are found than can be returned in slotlist, return buffer too smal 
	if ((c > *pulCount) && (pSlotList != NULL_PTR) )
		ret = CKR_BUFFER_TOO_SMALL;

	//number of slots should always be returned.
	*pulCount = c;

cleanup:   
	log_trace(WHERE, "I: p11_unlock()");
	p11_unlock();
	log_trace(WHERE, "I: leave, ret = %i",ret);
	return ret;
}
#undef WHERE


#define WHERE "C_GetSlotInfo()"
CK_RV C_GetSlotInfo(CK_SLOT_ID slotID, CK_SLOT_INFO_PTR pInfo)
{         
	CK_RV ret;
	P11_SLOT *slot;
	static int l=0;
	int isPresent = 0;

	log_trace(WHERE, "I: enter");

	if (p11_get_init() != BEIDP11_INITIALIZED)
	{
		log_trace(WHERE, "I: leave, CKR_CRYPTOKI_NOT_INITIALIZED");
		return (CKR_CRYPTOKI_NOT_INITIALIZED);
	}		

	p11_lock();

	if (++l < LOG_MAX_REC)  
		log_trace(WHERE, "S: C_GetSlotInfo(slot %d)", slotID);

	if (pInfo == NULL_PTR) 
	{
		log_trace(WHERE, "E: pInfo = NULL");
		CLEANUP(CKR_ARGUMENTS_BAD);
	}

	slot = p11_get_slot(slotID);
	if (slot == NULL)
	{
		log_trace(WHERE, "E: p11_get_slot(%d) returns null", slotID);
		CLEANUP(CKR_SLOT_ID_INVALID);
	}

	//fill in slot info
	strcpy_n(pInfo->slotDescription, slot->name, 64, ' ');
	strcpy_n(pInfo->manufacturerID, "_ID_", 32, ' ');
	pInfo->flags = CKF_REMOVABLE_DEVICE | CKF_HW_SLOT;
	pInfo->hardwareVersion.major = 1;
	pInfo->hardwareVersion.minor = 0;
	pInfo->firmwareVersion.major = 1;
	pInfo->firmwareVersion.minor = 0;

	//check if token is present
	ret = (cal_token_present(slotID, &isPresent));

	//don't mind that the token is not recognized, just report that a token is present
	if ( (ret == CKR_TOKEN_NOT_RECOGNIZED) || (ret == CKR_TOKEN_NOT_PRESENT) )
	{
		ret = CKR_OK;
		isPresent = 0;
	} 
	else if (ret != CKR_OK)
	{
		goto cleanup;
	}
	if (isPresent)
	{
		pInfo->flags |= CKF_TOKEN_PRESENT;
	}

cleanup:
	p11_unlock();
	log_trace(WHERE, "I: leave, ret = %i",ret);
	return ret;
}
#undef WHERE



#define WHERE "C_GetTokenInfo()"
CK_RV C_GetTokenInfo(CK_SLOT_ID slotID, CK_TOKEN_INFO_PTR pInfo)
{
	CK_RV ret;
	log_trace(WHERE, "I: enter");

	if (p11_get_init() != BEIDP11_INITIALIZED)
	{
		log_trace(WHERE, "I: leave, CKR_CRYPTOKI_NOT_INITIALIZED");
		return (CKR_CRYPTOKI_NOT_INITIALIZED);
	}		

	p11_lock();

	log_trace(WHERE, "S: C_GetTokenInfo(slot %d)", slotID);
	if (pInfo == NULL_PTR) 
	{
		log_trace(WHERE, "E: pInfo = NULL");
		CLEANUP(CKR_ARGUMENTS_BAD);
	}

	ret = cal_get_token_info(slotID, pInfo);
	if (ret != CKR_OK)
	{
		log_trace(WHERE, "E: p11_get_token_info returns %d", ret);
		goto cleanup;
	}

cleanup:        
	p11_unlock();
	log_trace(WHERE, "I: leave, ret = %i",ret);
	return ret;
}
#undef WHERE



#define WHERE "C_GetMechanismList()"
CK_RV C_GetMechanismList(CK_SLOT_ID slotID,
	CK_MECHANISM_TYPE_PTR pMechanismList,
	CK_ULONG_PTR pulCount)
{
	CK_RV ret;
	log_trace(WHERE, "I: enter");

	if (p11_get_init() != BEIDP11_INITIALIZED)
	{
		log_trace(WHERE, "I: leave, CKR_CRYPTOKI_NOT_INITIALIZED");
		return (CKR_CRYPTOKI_NOT_INITIALIZED);
	}		

	p11_lock();

	log_trace(WHERE, "S: C_GetMechanismList(slot %d)", slotID);

	ret = cal_get_mechanism_list(slotID, pMechanismList,pulCount);
	if (ret != CKR_OK)
	{
		log_trace(WHERE, "E: cal_get_mechanism_list(slotid=%d) returns %s", slotID, log_map_error(ret));
		goto cleanup;
	}

cleanup:

	p11_unlock();
	log_trace(WHERE, "I: leave, ret = %i",ret);
	return ret;
}
#undef WHERE


#define WHERE "C_GetMechanismInfo()"
CK_RV C_GetMechanismInfo(CK_SLOT_ID slotID,
	CK_MECHANISM_TYPE type,
	CK_MECHANISM_INFO_PTR pInfo)
{
	CK_RV ret;
	log_trace(WHERE, "I: enter");

	if (p11_get_init() != BEIDP11_INITIALIZED)
	{
		log_trace(WHERE, "I: leave, CKR_CRYPTOKI_NOT_INITIALIZED");
		return (CKR_CRYPTOKI_NOT_INITIALIZED);
	}		

	p11_lock();

	log_trace(WHERE, "S: C_GetMechanismInfo(slot %d)", slotID);

	if (pInfo == NULL_PTR)
	{
		CLEANUP(CKR_ARGUMENTS_BAD);
	}

	ret = cal_get_mechanism_info(slotID, type, pInfo);
	if (ret != CKR_OK)
	{
		log_trace(WHERE, "E: p11_get_mechanism_info(slotid=%d) returns %d", slotID, ret);
		goto cleanup;
	}

cleanup:        
	p11_unlock();
	log_trace(WHERE, "I: leave, ret = %i",ret);
	return ret;
}
#undef WHERE





#define WHERE "C_InitToken()"
CK_RV C_InitToken(CK_SLOT_ID slotID,
	CK_CHAR_PTR pPin,
	CK_ULONG ulPinLen,
	CK_CHAR_PTR pLabel)
{
	log_trace(WHERE, "I: CKR_FUNCTION_NOT_SUPPORTED");
	log_trace(WHERE, "S: C_InitToken(slot %d)", slotID);
	return (CKR_FUNCTION_NOT_SUPPORTED);
}
#undef WHERE


#define WHERE "C_WaitForSlotEvent("
CK_RV C_WaitForSlotEvent(CK_FLAGS flags,   /* blocking/nonblocking flag */
	CK_SLOT_ID_PTR pSlot,  /* location that receives the slot ID */
	CK_VOID_PTR pReserved) /* reserved.  Should be NULL_PTR */

{
	CK_RV ret = CKR_OK;
#ifdef _WIN32
	int h;
	P11_SLOT *p11Slot = NULL;
	int i = 0;
#endif
	CK_BBOOL locked = CK_FALSE;

	log_trace(WHERE, "I: enter");

	//need to check initialization before lock, as lock might be in progress of being set up
	if (p11_get_init() != BEIDP11_INITIALIZED)
	{
		log_trace(WHERE, "I: leave, CKR_CRYPTOKI_NOT_INITIALIZED");
		return (CKR_CRYPTOKI_NOT_INITIALIZED);
	}	

	p11_lock();

	//check again, in case c_finalize got the lock right before we did
	//(then c_finalize will give us a chance to fall through, right before he resets the lock))
	if (p11_get_init() != BEIDP11_INITIALIZED)
	{
		log_trace(WHERE, "I: leave, CKR_CRYPTOKI_NOT_INITIALIZED");
		p11_unlock();
		return (CKR_CRYPTOKI_NOT_INITIALIZED);
	}	

	locked = CK_TRUE;

	log_trace(WHERE, "S: C_WaitForSlotEvent(flags = 0x%0x)", flags);

	/* Doesn't seem to work on Linux: if you insert a card then Mozilla freezes
	 * until you remove the card. 
	 * So we might have to return "not supported" in which case Ff 1.5 defaults
	 * to polling in the main thread, like before. */
#ifndef _WIN32
	CLEANUP(CKR_FUNCTION_NOT_SUPPORTED);
#else

	//first check if no events are set for slots in previous run
	//this could happen if more cards are inserted/removed at the same time
	for (i=0; i < p11_get_nreaders(); i++)
	{
		p11Slot = p11_get_slot(i);
		if(p11Slot == NULL)
			CLEANUP(CKR_FUNCTION_FAILED);
		if (p11Slot->ievent != P11_EVENT_NONE)
		{
#ifdef PKCS11_FF
			//in case the upnp reader caused the event, return a slotnumber higher then the highest slotnumber in the current slotList
			if (i == (p11_get_nreaders() - 1))
			{
				i = p11_get_nreaders();
			}
#endif
			*pSlot = i;
			//clear event
			p11Slot->ievent = P11_EVENT_NONE;
			CLEANUP(CKR_OK);
		}
	}

	if (flags & CKF_DONT_BLOCK)
	{
		ret = cal_wait_for_slot_event(0);//0 means don't block
	}
	else
	{
		ret = cal_wait_for_slot_event(1);//1 means block, lock will get released here

		//ret is 0x30 when SCardGetStatusChange gets cancelled 
		if ((p11_get_init() == BEIDP11_NOT_INITIALIZED ) || 
			(p11_get_init() == BEIDP11_DEINITIALIZING) || 
			(ret == CKR_CRYPTOKI_NOT_INITIALIZED) )
		{
			log_trace(WHERE, "I: CKR_CRYPTOKI_NOT_INITIALIZED");
			p11_unlock();
			return(CKR_CRYPTOKI_NOT_INITIALIZED);
		}
	}
	if(ret != CKR_OK)
		goto cleanup;

	ret = cal_get_slot_changes(&h);

	if (ret == CKR_OK)
		*pSlot = h;
#endif // WIN32
    
cleanup:
	if(locked == CK_TRUE)
		p11_unlock();

	log_trace(WHERE, "I: leave, ret = %i",ret);
	return ret;
}
#undef WHERE




CK_FUNCTION_LIST pkcs11_function_list = {
	{ 2, 20 },
	C_Initialize,
	C_Finalize,
	C_GetInfo,
	C_GetFunctionList,
	C_GetSlotList,
	C_GetSlotInfo,
	C_GetTokenInfo,
	C_GetMechanismList,
	C_GetMechanismInfo,
	C_InitToken,
	C_InitPIN,
	C_SetPIN,
	C_OpenSession,
	C_CloseSession,
	C_CloseAllSessions,
	C_GetSessionInfo,
	C_GetOperationState,
	C_SetOperationState,
	C_Login,
	C_Logout,
	C_CreateObject,
	C_CopyObject,
	C_DestroyObject,
	C_GetObjectSize,
	C_GetAttributeValue,
	C_SetAttributeValue,
	C_FindObjectsInit,
	C_FindObjects,
	C_FindObjectsFinal,
	C_EncryptInit,
	C_Encrypt,
	C_EncryptUpdate,
	C_EncryptFinal,
	C_DecryptInit,
	C_Decrypt,
	C_DecryptUpdate,
	C_DecryptFinal,
	C_DigestInit,
	C_Digest,
	C_DigestUpdate,
	C_DigestKey,
	C_DigestFinal,
	C_SignInit,
	C_Sign,
	C_SignUpdate,
	C_SignFinal,
	C_SignRecoverInit,
	C_SignRecover,
	C_VerifyInit,
	C_Verify,
	C_VerifyUpdate,
	C_VerifyFinal,
	C_VerifyRecoverInit,
	C_VerifyRecover,
	C_DigestEncryptUpdate,
	C_DecryptDigestUpdate,
	C_SignEncryptUpdate,
	C_DecryptVerifyUpdate,
	C_GenerateKey,
	C_GenerateKeyPair,
	C_WrapKey,
	C_UnwrapKey,
	C_DeriveKey,
	C_SeedRandom,
	C_GenerateRandom,
	C_GetFunctionStatus,
	C_CancelFunction,
	C_WaitForSlotEvent
};
