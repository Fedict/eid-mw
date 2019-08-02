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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "beid_p11.h"
#include "p11.h"
#include "pkcs11log.h"
#include "util.h"
#include "cal.h"

#define WHERE "C_OpenSession()"
CK_RV C_OpenSession(CK_SLOT_ID            slotID,        /* the slot's ID */
	CK_FLAGS              flags,         /* defined in CK_SESSION_INFO */
	CK_VOID_PTR           pApplication,  /* pointer passed to callback */
	CK_NOTIFY             Notify,        /* notification callback function */
	CK_SESSION_HANDLE_PTR phSession)     /* receives new session handle */
{
	CK_RV ret;
	P11_SLOT* pSlot = NULL;
	P11_SESSION *pSession = NULL;

	//   CAutoMutex(&g_oSlotMutex);
	log_trace(WHERE, "I: enter");

	if (p11_get_init() != BEIDP11_INITIALIZED)
	{
		log_trace(WHERE, "I: leave, CKR_CRYPTOKI_NOT_INITIALIZED");
		return (CKR_CRYPTOKI_NOT_INITIALIZED);
	}		


	p11_lock();

	log_trace(WHERE, "S: C_OpenSession (slot %lu)", slotID);

	if (!(flags & CKF_SERIAL_SESSION)) 
	{
		ret = CKR_SESSION_PARALLEL_NOT_SUPPORTED;
		goto cleanup;
	}

	//XXX check this
	/*   if (flags & ~(CKF_SERIAL_SESSION | CKF_RW_SESSION))
	{
	ret = CKR_ARGUMENTS_BAD;
	goto cleanup;
	}*/

	pSlot = p11_get_slot(slotID);
	if (pSlot == NULL)
	{
		log_trace(WHERE, "E: p11_get_slot(%lu) returns null", slotID);
		ret = CKR_SLOT_ID_INVALID;
		goto cleanup;
	}

	/* Check that no conflictions sessions exist */
	/* RO session when SO session exists is not allowed */
	if ( !(flags & CKF_RW_SESSION) && (pSlot->login_type == CKU_SO) && (pSlot->logged_in == CK_TRUE))
	{
		log_trace(WHERE, "E: R/W Session exists for slot %d", slotID);
		ret = CKR_SESSION_READ_WRITE_SO_EXISTS;
		goto cleanup;
	}

	//get a free session object reserve it by setting inuse flag
	ret = p11_get_free_session(phSession, &pSession);
	if (ret != CKR_OK)
	{
		log_trace(WHERE, "E: p11_get_free_session() returns %lu", ret);
		goto cleanup;
	}

	//connect to card if present
	ret = cal_connect(slotID);
	if (ret != CKR_OK)
	{
		log_trace(WHERE, "E: cal_connect(slot %lu) failed", slotID);
		//release session so it can be reused
		pSession->inuse = 0;
		goto cleanup;
	}

	pSession->hslot = slotID;
	pSession->flags = flags;
	pSession->pdNotify = pApplication;
	pSession->pfNotify = Notify;
	//initial state 
	pSession->state = P11_CARD_STILL_PRESENT;
//	pSession->bReadDataAllowed = P11_READDATA_ASK;

	/* keep the nr of sessions for this slot */
	pSlot->nsessions++;

	log_trace(WHERE, "S: Open session (slot %lu: hsession = %lu )", slotID, *phSession);

cleanup:
	p11_unlock();
	log_trace(WHERE, "I: leave, ret = %lu",ret);
	return ret;
}
#undef WHERE



#define WHERE "C_CloseSession()"
CK_RV C_CloseSession(CK_SESSION_HANDLE hSession)
{
	P11_SESSION *pSession = NULL;
	P11_SLOT *pSlot = NULL;
	CK_RV ret;
	log_trace(WHERE, "I: enter");

	if (p11_get_init() != BEIDP11_INITIALIZED)
	{
		log_trace(WHERE, "I: leave, CKR_CRYPTOKI_NOT_INITIALIZED");
		return (CKR_CRYPTOKI_NOT_INITIALIZED);
	}	

	p11_lock();

	log_trace(WHERE, "S: C_CloseSession (session %lu)", hSession);

	//get session, of pSession is found, regardless the ret value, we can clean it up
	p11_get_session(hSession, &pSession);
	if (pSession == NULL)
	{
		ret = CKR_SESSION_HANDLE_INVALID;
		log_trace(WHERE, "E: Invalid session handle (%lu)", hSession);
		goto cleanup;
	}

	//get slot, if not exist, we allow to close session anyway
	pSlot = p11_get_slot(pSession->hslot);
	if (pSlot == NULL)
	{
		log_trace(WHERE, "W: Invalid slot (%lu) for session (%lu)", pSession->hslot, hSession);
		ret = CKR_OK;
	}
	else
	{
		ret = p11_close_session(pSlot, pSession);
	}

cleanup:
	p11_unlock();
	log_trace(WHERE, "I: leave, ret = %lu",ret);
	return ret;
}
#undef WHERE




#define WHERE "C_CloseAllSessions()"
CK_RV C_CloseAllSessions(CK_SLOT_ID slotID) /* the token's slot */
{
	CK_RV ret;
	log_trace(WHERE, "I: enter");

	if (p11_get_init() != BEIDP11_INITIALIZED)
	{
		log_trace(WHERE, "I: leave, CKR_CRYPTOKI_NOT_INITIALIZED");
		return (CKR_CRYPTOKI_NOT_INITIALIZED);
	}		

	p11_lock();

	log_trace(WHERE, "S: C_CloseAllSessions(slot %lu)", slotID);

	ret = p11_close_all_sessions(slotID);

	p11_unlock();
	log_trace(WHERE, "I: leave, ret = %lu",ret);
	return ret;
}
#undef WHERE



#define WHERE "C_GetSessionInfo()"
CK_RV C_GetSessionInfo(CK_SESSION_HANDLE hSession,  /* the session's handle */
	CK_SESSION_INFO_PTR pInfo)   /* receives session information */
{
	CK_RV ret;
	P11_SESSION *pSession = NULL;
	P11_SLOT *pSlot = NULL;
	CK_TOKEN_INFO tokeninfo;
	log_trace(WHERE, "I: enter");

	if (p11_get_init() != BEIDP11_INITIALIZED)
	{
		log_trace(WHERE, "I: leave, CKR_CRYPTOKI_NOT_INITIALIZED");
		return (CKR_CRYPTOKI_NOT_INITIALIZED);
	}		

	p11_lock();

	log_trace(WHERE, "S: C_GetSessionInfo(session %lu)", hSession);

	if (pInfo == NULL_PTR) 
	{
		ret = CKR_ARGUMENTS_BAD;
		goto cleanup;
	}

	ret = p11_get_session(hSession, &pSession);
	if (ret)
	{
		log_trace(WHERE, "E: Invalid session handle (%lu) (%s)", hSession, log_map_error(ret));
		goto cleanup;
	}

	pInfo->slotID = pSession->hslot;
	pInfo->flags = pSession->flags;
	pInfo->ulDeviceError = 0;

	pSlot = p11_get_slot(pSession->hslot);
	if (pSlot == NULL)
	{
		log_trace(WHERE, "E: slot not found for session %lu", hSession);
		ret = CKR_SESSION_HANDLE_INVALID;
		goto cleanup;
	}

	if (pSlot->logged_in == CK_TRUE)
	{
		//SO only can create RW_SO sessions
		if (pSlot->login_type == CKU_SO)
		{
			pInfo->state = CKS_RW_SO_FUNCTIONS;
		}
		//USER can create RW or RO sessions
		else if (pSlot->login_type == CKU_USER)
		{
			pInfo->state = (pSession->flags & CKF_RW_SESSION) ? CKS_RW_USER_FUNCTIONS : CKS_RO_USER_FUNCTIONS;
		}
	}
	//if login not required => we can also get USER sessions without being logged on
	else 
	{
		ret = cal_get_token_info(pSession->hslot, &tokeninfo);
		if ( (ret == CKR_OK) && !(tokeninfo.flags & CKF_LOGIN_REQUIRED) )
			pInfo->state = (pSession->flags & CKF_RW_SESSION)? CKS_RW_USER_FUNCTIONS : CKS_RO_USER_FUNCTIONS;
		else
			pInfo->state = (pSession->flags & CKF_RW_SESSION) ? CKS_RW_PUBLIC_SESSION : CKS_RO_PUBLIC_SESSION;
	}

cleanup:
	p11_unlock();
	log_trace(WHERE, "I: leave, ret = %lu",ret);
	return ret;
}
#undef WHERE 


#define WHERE "C_GetOperationState()"
CK_RV C_GetOperationState(CK_SESSION_HANDLE hSession,             /* the session's handle */
	CK_BYTE_PTR       pOperationState,      /* location receiving state */
	CK_ULONG_PTR      pulOperationStateLen) /* location receiving state length */
{
	log_trace(WHERE, "S: C_GetOperationState(sesssion %lu)", hSession);
	return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE



#define WHERE "C_SetOperationState()"
CK_RV C_SetOperationState(CK_SESSION_HANDLE hSession,            /* the session's handle */
	CK_BYTE_PTR      pOperationState,      /* the location holding the state */
	CK_ULONG         ulOperationStateLen,  /* location holding state length */
	CK_OBJECT_HANDLE hEncryptionKey,       /* handle of en/decryption key */
	CK_OBJECT_HANDLE hAuthenticationKey)   /* handle of sign/verify key */
{
	log_trace(WHERE, "S: C_SetOperationState(session %lu)", hSession);
	return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE


#define WHERE "C_Login()"
CK_RV C_Login(CK_SESSION_HANDLE hSession,  /* the session's handle */
	CK_USER_TYPE      userType,  /* the user type */
	CK_CHAR_PTR       pPin,      /* the user's PIN */
	CK_ULONG          ulPinLen)  /* the length of the PIN */
{
	CK_RV ret;
	P11_SESSION *pSession = NULL;
	P11_SLOT *pSlot = NULL;
	CK_TOKEN_INFO tokeninfo;

	//return(CKR_OK);
	log_trace(WHERE, "I: enter");

	if (p11_get_init() != BEIDP11_INITIALIZED)
	{
		log_trace(WHERE, "I: leave, CKR_CRYPTOKI_NOT_INITIALIZED");
		return (CKR_CRYPTOKI_NOT_INITIALIZED);
	}		

	p11_lock();

	memset(&tokeninfo, 0, sizeof(CK_TOKEN_INFO));

	log_trace(WHERE, "S: Login (session %lu)", hSession);

	if (userType != CKU_USER && userType != CKU_SO)
	{
		ret = CKR_USER_TYPE_INVALID;
		goto cleanup;
	}

	ret = p11_get_session(hSession, &pSession);
	if (ret)
	{
		log_trace(WHERE, "E: Invalid session handle (%lu)", hSession);
		goto cleanup;
	}

	pSlot = p11_get_slot(pSession->hslot);
	if (pSlot == NULL)
	{
		log_trace(WHERE, "E: Slot not found for session %lu", hSession);
		ret = CKR_SESSION_HANDLE_INVALID;
		goto cleanup;
	}

	if (pSlot->logged_in == CK_TRUE)
	{
		// Needed for Acrobat, in case you want to sign with a 2nd card
		ret = CKR_OK; //CKR_USER_ALREADY_LOGGED_IN;
		goto cleanup;
	}

	/*   ret = cal_get_token_info(pSlot, &tokeninfo);
	if (ret != CKR_OK)
	{
	log_trace(WHERE, "E: could not find tokeninfo for slot %d", pSession->hslot);
	goto cleanup;
	}

	if ( !(tokeninfo.flags & CKF_USER_PIN_INITIALIZED) )
	{
	ret = CKR_USER_PIN_NOT_INITIALIZED;
	goto cleanup;
	}*/

	ret = cal_logon(pSession->hslot, ulPinLen, pPin, 0);
	if (ret == CKR_OK)
	{
		pSlot->login_type = userType;
		pSlot->logged_in = CK_TRUE;
	}		

cleanup:
	p11_unlock();
	log_trace(WHERE, "I: leave, ret = %lu",ret);
	return ret;
}
#undef WHERE



#define WHERE "C_Logout()"
CK_RV C_Logout(CK_SESSION_HANDLE hSession) /* the session's handle */
{
	CK_RV ret = CKR_OK;
	P11_SESSION *pSession = NULL;
	P11_SLOT *pSlot = NULL;
	log_trace(WHERE, "I: enter");

	if (p11_get_init() != BEIDP11_INITIALIZED)
	{
		log_trace(WHERE, "I: leave, CKR_CRYPTOKI_NOT_INITIALIZED");
		return (CKR_CRYPTOKI_NOT_INITIALIZED);
	}		

	p11_lock();

	log_trace(WHERE, "S: Logout (session %lu)", hSession);

	ret = p11_get_session(hSession, &pSession);
	if (ret)
	{
		log_trace(WHERE, "E: Invalid session handle (%lu)", hSession);
		goto cleanup;
	}

	pSlot = p11_get_slot(pSession->hslot);
	if (pSlot == NULL)
	{
		log_trace(WHERE, "E: Slot not found for session %lu", hSession);
		ret = CKR_SESSION_HANDLE_INVALID;
		goto cleanup;
	}

	if (pSlot->logged_in == CK_TRUE)
	{
		pSlot->logged_in = CK_FALSE;
		ret = cal_logout(pSession->hslot);
	}
	else
		ret = CKR_USER_NOT_LOGGED_IN;

	/* TODO  cleanup all active operations (see standard) */
	/* TODO: invalidate all private objects */
	/* TODO: destroy all private session objects (we only have private token objects and they are unreadable anyway) */

cleanup:
	p11_unlock();
	log_trace(WHERE, "I: leave, ret = %lu",ret);
	return ret;
}
#undef WHERE




#define WHERE "C_InitPIN()"
CK_RV C_InitPIN(CK_SESSION_HANDLE hSession,
	CK_CHAR_PTR pPin,
	CK_ULONG ulPinLen)
{
	log_trace(WHERE, "S: C_InitPIN (session %lu)", hSession);
	return (CKR_FUNCTION_NOT_SUPPORTED);
}
#undef WHERE


#define WHERE "C_SetPIN()"
CK_RV C_SetPIN(CK_SESSION_HANDLE hSession,
	CK_CHAR_PTR pOldPin,
	CK_ULONG ulOldLen,
	CK_CHAR_PTR pNewPin,
	CK_ULONG ulNewLen)
{
	CK_RV ret;
	P11_SESSION *pSession = NULL;
	log_trace(WHERE, "I: enter");

	if (p11_get_init() != BEIDP11_INITIALIZED)
	{
		log_trace(WHERE, "I: leave, CKR_CRYPTOKI_NOT_INITIALIZED");
		return (CKR_CRYPTOKI_NOT_INITIALIZED);
	}		

	p11_lock();

	log_trace(WHERE, "S: C_SetPIN(session %lu)", hSession);

	ret = p11_get_session(hSession, &pSession);
	if (ret)
	{
		log_trace(WHERE, "E: Invalid session handle (%lu)", hSession);
		goto cleanup;
	}

	ret = cal_change_pin(pSession->hslot, BEID_PIN_AUTH, ulOldLen, pOldPin, ulNewLen, pNewPin);
cleanup:
	p11_unlock();
	log_trace(WHERE, "I: leave, ret = %lu",ret);
	return ret;
}
#undef WHERE
