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
#include <stdio.h>
#include <stdlib.h>
#include "beid_p11.h"
#include "p11.h"
#include "cal.h"
#include "log.h"
#include "util.h"

#define MAX_SZ_READER 1024

/*******************
   GLOBAL
********************/
#ifdef __cplusplus
extern "C" {
#endif

//CARD_TYPE g_CardTypeTable[] = CARD_TYPE_TABLE;

//SCARDCONTEXT   hSC;

//DWORD        cchReaders = SCARD_AUTOALLOCATE;
//DWORD          cchReaders = MAX_SZ_READER; //fixed max size
//unsigned char  szReaders[MAX_SZ_READER];

unsigned int   nReaders;
P11_SLOT       gpSlot[MAX_SLOTS];
//to keep track of init
//imagine different threads call init -> last close should clean the global data
unsigned int   gRefCount = 0;

P11_SESSION *gpSessions = NULL;
unsigned int nSessions = 0;

#ifdef __cplusplus
   } //extern "C"
#endif

P11_SLOT * p11_get_slot(unsigned int h)
{
   //thanks to Adobe, handles have to start from 0!!!
   if (h >= nReaders)
	 {
#ifdef PKCS11_FF
		 //return the upnp reader, as FF is probably asking info about the detected/removed reader
		 if(nReaders > 0)
				h = nReaders-1; 
		 else
			 h = 0;
#else
      return NULL; //invalid handle
#endif
	 }
   return &gpSlot[h];
}

int p11_get_session(unsigned int h, P11_SESSION **ppSession)
{
int ret = 0;

if ( (h == 0) || (h > nSessions) )
   return (CKR_SESSION_HANDLE_INVALID); //invalid handle

*ppSession = &gpSessions[h-1];

ret = cal_validate_session(*ppSession);

return (ret);
}

P11_OBJECT *p11_get_slot_object(P11_SLOT *pSlot, unsigned int h)
{
   if ( (h < 1) || (h > pSlot->nobjects) )
      return (NULL); //invalid handle

   //internally we start from 0
   return (&pSlot->pobjects[h-1]);
}


int p11_get_nreaders()
{
   return nReaders;
}


#define WHERE "p11_get_free_session()"
CK_RV p11_get_free_session(CK_SESSION_HANDLE_PTR phSession, P11_SESSION **ppSession)
{
int ret = 0;
unsigned int index = 0;
unsigned int size = 0;
unsigned int diff = 0;
P11_SESSION *gpSessions_realloc;

*ppSession = NULL;
// search for free entry in session table  
for (index=0; index < nSessions; index++)
   {            
   if (gpSessions[index].inuse == 0)
      break;
   }

//enlarge session table if no free entry was found
if (index == nSessions)
   {
   size = nSessions * sizeof(P11_SESSION);
   diff = SESSION_TAB_STEP_SIZE * sizeof(P11_SESSION);

   gpSessions_realloc = realloc(gpSessions, size + diff);
   if (gpSessions_realloc == NULL)
   {
	   log_trace(WHERE, "E: unable to allocate memory for session table, %d bytes\n", size + diff);
	   return(CKR_HOST_MEMORY);
   }
   else
   {
	   gpSessions = gpSessions_realloc;
   }

   memset(&gpSessions[index], 0, diff);
   nSessions += SESSION_TAB_STEP_SIZE;
   }


gpSessions[index].inuse = 1;

*ppSession = &gpSessions[index];
*phSession = index+1;

return (ret);
}
#undef WHERE

#define WHERE "p11_close_sessions_finalize()"
CK_RV p11_close_sessions_finalize()
{
	CK_RV r, ret;
	P11_SESSION *pSession;
	P11_SLOT *pSlot;
	unsigned int i;

	ret = CKR_OK;
	for(i=0;i<nSessions; i++) {
		if((pSession = &gpSessions[i])) {
			if(pSession->inuse) {
				pSlot = p11_get_slot(pSession->hslot);
				// don't overwrite previous errors
				if((r = p11_close_session(pSlot, pSession)) != CKR_OK) {
					ret = r;
				}
			}
		}
	}

	return ret;
}
#undef WHERE

#define WHERE "p11_close_session()"
CK_RV p11_close_session(P11_SLOT* pSlot, P11_SESSION* pSession)
{
	CK_RV ret = CKR_OK;

	if (pSlot == NULL)
		return ret;

	if (pSlot->nsessions > 0)
		pSlot->nsessions--;

	if ((pSlot->nsessions < 1) && (pSlot->login_type >= 0) )
	{
		cal_logout(pSession->hslot);
		pSlot->login_type = -1;
	}

	//disconnect this session to device
	ret = cal_disconnect(pSession->hslot);
	//clear data so it can be reused
	if(pSession->Operation[P11_OPERATION_FIND].active) {
		p11_clean_finddata(pSession->Operation[P11_OPERATION_FIND].pData);
		free(pSession->Operation[P11_OPERATION_FIND].pData);
		pSession->Operation[P11_OPERATION_FIND].pData = NULL;
		pSession->Operation[P11_OPERATION_FIND].active = 0;
	}
	if(pSession->Operation[P11_OPERATION_DIGEST].active) {
		free(pSession->Operation[P11_OPERATION_DIGEST].pData);
		pSession->Operation[P11_OPERATION_DIGEST].pData = NULL;
		pSession->Operation[P11_OPERATION_DIGEST].active = 0;
	}
	if(pSession->Operation[P11_OPERATION_SIGN].active) {
		free(pSession->Operation[P11_OPERATION_SIGN].pData);
		pSession->Operation[P11_OPERATION_SIGN].pData = NULL;
		pSession->Operation[P11_OPERATION_SIGN].active = 0;
	}
	pSession->state = 0;
	pSession->inuse = 0;
	pSession->flags = 0;
	pSession->hslot = 0;
	pSession->pdNotify = NULL;
	pSession->pfNotify = NULL;

	return ret;
}
#undef WHERE

#define WHERE "p11_close_all_sessions()"
CK_RV p11_close_all_sessions(CK_SLOT_ID slotID)
{
int ret = 0;
unsigned int i = 0;
P11_SLOT    *pSlot = NULL;
P11_SESSION *pSession = NULL;

pSlot = p11_get_slot(slotID);
if (pSlot == NULL)
   {
   log_trace(WHERE, "E: Invalid slot (%d)", slotID);
   ret = CKR_SLOT_ID_INVALID;
   goto cleanup;
   }

if (pSlot->nsessions == 0)
   {
   ret = CKR_OK;
   goto cleanup;
   }

//walk through all sessions and clean the ones related to this slot
for (i=0; (i < nSessions) && (pSession = &gpSessions[i]) ;i++)
   {
   if ( (pSession->inuse) && (pSession->hslot == slotID) )
      {
      ret = p11_close_session(pSlot, pSession);
      }
   }

cleanup:

return (ret);
}
#undef WHERE



#define WHERE "p11_invalidate_sessions()"
CK_RV p11_invalidate_sessions(CK_SLOT_ID hSlot, int status)
{
int ret = 0;
unsigned int i = 0;
P11_SESSION *pSession = NULL;

//walk through all sessions and invalidate the ones related to this slot
for (i=0; (i < nSessions) && (pSession = &gpSessions[i]) ;i++)
   {
   if ( (pSession->inuse) && (pSession->hslot == hSlot) )
      pSession->state = status;
   }

return (ret);
}
#undef WHERE



#define WHERE "p11_new_slot_object()"
CK_RV p11_new_slot_object(P11_SLOT *pSlot, CK_ULONG *phObject)
{
CK_RV ret = CKR_OK;
unsigned int index = 0;
unsigned int size = 0;
unsigned int diff = 0;

//p11_lock();

// search for free entry in object table  
for (index=0; index < pSlot->nobjects; index++)
   {            
   //check for consistency in slot objects
   if (pSlot->pobjects == NULL) 
      {
      log_trace(WHERE, "E: inconsistency for object list in slot!");
      return (CKR_GENERAL_ERROR);
      }

   if (pSlot->pobjects[index].inuse == 0)
      break;
   }

//enlarge session table if no free entry was found
if (index == pSlot->nobjects)
   {
   size = pSlot->nobjects * sizeof(P11_OBJECT);
   diff = OBJECT_TAB_STEP_SIZE * sizeof(P11_OBJECT);

   if ((pSlot->pobjects = realloc(pSlot->pobjects, size+diff)) == NULL)
      {
      log_trace(WHERE, "E: unable to allocate memory for slot object table, %d bytes\n", size+diff);
      return(CKR_HOST_MEMORY);      
      }      
    
  memset(&(pSlot->pobjects[index])/* + size*/, 0, diff);
  pSlot->nobjects += OBJECT_TAB_STEP_SIZE;
  }

//set flag inuse so nobody else will get this handle
pSlot->pobjects[index].inuse = 1;

//handle is array el + 1 //handles start from 1
*phObject = index + 1;

return (ret);
}
#undef WHERE




#define WHERE "p11_get_attribute_value()"
CK_RV p11_get_attribute_value(CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount, CK_ATTRIBUTE_TYPE type, CK_VOID_PTR *ppVoid, CK_ULONG *len)
{
CK_ATTRIBUTE_PTR pAttr = NULL;
unsigned int i = 0;

for (i=0; (i < ulCount) && (pAttr = &pTemplate[i]); i++)
   {
   if (pAttr->type == type)
      {
      *ppVoid = pAttr->pValue;
      *len = pAttr->ulValueLen;
      return(CKR_OK);
      }
   }

*ppVoid = NULL;
*len = 0;

return (CKR_ATTRIBUTE_TYPE_INVALID);
}
#undef WHERE



#define WHERE "p11_set_attribute_value()"
CK_RV p11_set_attribute_value(CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount, CK_ATTRIBUTE_TYPE type, CK_VOID_PTR pVoid, CK_ULONG len)
{
CK_ATTRIBUTE_PTR pAttr = NULL;
unsigned int i = 0;

if (len > MAX_ATTRIBUTE_SIZE)
   return (CKR_ARGUMENTS_BAD);

//search for attribute to set value
for (i=0; (i < ulCount) && (pAttr = &pTemplate[i]); i++)
   {
   if (pAttr->type == type)
      {
      //if there is allready an attribute of this type, free the memory and overwrite the value
      if ( (pAttr->ulValueLen > 0) && (pAttr->pValue != NULL) )
         free(pAttr->pValue);

      pAttr->pValue = NULL;
      pAttr->pValue = malloc(len);
      if (pAttr->pValue == NULL)
         {
         log_trace(WHERE, "E: allocation error for attribute value (len=%d)", len);
         return(CKR_HOST_MEMORY);
         }
         
      memcpy(pAttr->pValue, pVoid, len);
      pAttr->ulValueLen = len;
      return(CKR_OK);
      }
   }

return (CKR_ATTRIBUTE_TYPE_INVALID);
}
#undef WHERE




#define WHERE "p11_copy_object()"
CK_RV p11_copy_object(CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount, CK_ATTRIBUTE_PTR pObject)
{
CK_RV ret = CKR_OK;
unsigned int i;

//check values are within limits
if (ulCount > MAX_OBJECT_SIZE)
   return (CKR_ARGUMENTS_BAD);

for (i=0; i < ulCount; i++)
   {

   //clear pointer to value and allocate new space for this.
	 if(pObject[i].pValue != NULL)
	 {
		 free(pObject[i].pValue);
	 }

   //copy complete attribute
   memcpy(&pObject[i], &pTemplate[i], sizeof(CK_ATTRIBUTE));
	 pObject[i].pValue = NULL;

   if ( pTemplate[i].ulValueLen > MAX_ATTRIBUTE_SIZE)
      return (CKR_ARGUMENTS_BAD);

   if (pTemplate[i].ulValueLen)
      {
      pObject[i].pValue = malloc(pTemplate[i].ulValueLen);
      if (pObject[i].pValue == NULL)
         return (CKR_HOST_MEMORY);
      memcpy(pObject[i].pValue, pTemplate[i].pValue, pTemplate[i].ulValueLen);
      }
   }

return (ret);
}
#undef WHERE





#define WHERE "p11_add_slot_object()"
CK_RV p11_add_slot_object(P11_SLOT *pSlot, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount, CK_BBOOL bToken, CK_ULONG type, CK_ULONG id,  CK_BBOOL bPrivate, CK_ULONG *phObject)
{
CK_RV ret = CKR_OK;
P11_OBJECT *pObject = NULL;
//unsigned int hObject = 0;

*phObject = 0;

ret = p11_new_slot_object(pSlot, phObject);
if ((ret != 0) || (*phObject == 0))
   {
   log_trace(WHERE, "E: could not add new slot object during init of objects");
   return(ret);
   }

pObject = p11_get_slot_object(pSlot, *phObject);

//add room for attributes as in template
pObject->pAttr = (CK_ATTRIBUTE_PTR) malloc(ulCount * sizeof(CK_ATTRIBUTE));
if (pObject->pAttr == NULL)
   {
   log_trace(WHERE, "E: alloc error for attribute");
   return (CKR_HOST_MEMORY);
   }
memset(pObject->pAttr, 0, ulCount * sizeof(CK_ATTRIBUTE));

//set the size of the object attributes
pObject->count = ulCount;

//copy the template to the new object
ret = p11_copy_object(pTemplate, ulCount, pObject->pAttr);
if (ret)
   {
   log_trace(WHERE, "E: p11_copy_object() returned %d", ret);
   goto cleanup;
   }

//CKA_TOKEN
ret = p11_set_attribute_value(pObject->pAttr, ulCount, CKA_TOKEN, (CK_VOID_PTR) &bToken, sizeof(CK_BBOOL));
if (ret)
   {
   log_trace(WHERE, "E: p11_set_attribute_value(CKA_TOKEN) returned %d", ret);
   goto cleanup;
   }

//CKA_CLASS
ret = p11_set_attribute_value(pObject->pAttr, ulCount, CKA_CLASS, (CK_VOID_PTR) &type, sizeof(CK_ULONG));
if (ret)
   {
   log_trace(WHERE, "E: p11_set_attribute_value(CKA_CLASS) returned %d", ret);
   goto cleanup;
   }

//CKA_ID
ret = p11_set_attribute_value(pObject->pAttr, ulCount, CKA_ID, (CK_VOID_PTR) &id, sizeof(CK_ULONG));
if (ret)
   {
   log_trace(WHERE, "E: p11_set_attribute_value(CKA_ID) returned %d", ret);
   goto cleanup;
   }
 
//CKA_PRIVATE
ret = p11_set_attribute_value(pObject->pAttr, ulCount, CKA_PRIVATE, (CK_VOID_PTR) &bPrivate, sizeof(CK_BBOOL));
if (ret)
   {
   log_trace(WHERE, "E: p11_set_attribute_value(CKA_PRIVATE) returned %d", ret);
   goto cleanup;
   }

cleanup:

return (ret);
}
#undef WHERE 





#define WHERE "p11_add_slot_ID_object()"
CK_RV p11_add_slot_ID_object(P11_SLOT *pSlot, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount, CK_BBOOL bToken,
						   CK_ULONG type, CK_BBOOL bPrivate, CK_ULONG *phObject,
						   CK_VOID_PTR plabel, CK_ULONG labelLen, CK_VOID_PTR pvalue, CK_ULONG valueLen, CK_VOID_PTR pobjectID, CK_ULONG objectIDLen)
{
CK_RV ret = CKR_OK;
P11_OBJECT *pObject = NULL;
//unsigned int hObject = 0;

*phObject = 0;

ret = p11_new_slot_object(pSlot, phObject);
if ((ret != CKR_OK) || (*phObject == 0))
   {
   log_trace(WHERE, "E: could not add new slot object during init of objects");
   return(ret);
   }

pObject = p11_get_slot_object(pSlot, *phObject);

//add room for attributes as in template
pObject->pAttr = (CK_ATTRIBUTE_PTR) malloc(ulCount * sizeof(CK_ATTRIBUTE));
if (pObject->pAttr == NULL)
   {
   log_trace(WHERE, "E: alloc error for attribute");
   return (CKR_HOST_MEMORY);
   }
memset(pObject->pAttr, 0, ulCount * sizeof(CK_ATTRIBUTE));

//set the size of the object attributes
pObject->count = ulCount;

//copy the template to the new object
ret = p11_copy_object(pTemplate, ulCount, pObject->pAttr);
if (ret)
   {
   log_trace(WHERE, "E: p11_copy_object() returned %d", ret);
   goto cleanup;
   }

//CKA_TOKEN
ret = p11_set_attribute_value(pObject->pAttr, ulCount, CKA_TOKEN, (CK_VOID_PTR) &bToken, sizeof(CK_BBOOL));
if (ret)
   {
   log_trace(WHERE, "E: p11_set_attribute_value(CKA_TOKEN) returned %d", ret);
   goto cleanup;
   }

//CKA_CLASS
ret = p11_set_attribute_value(pObject->pAttr, ulCount, CKA_CLASS, (CK_VOID_PTR) &type, sizeof(CK_ULONG));
if (ret)
   {
   log_trace(WHERE, "E: p11_set_attribute_value(CKA_CLASS) returned %d", ret);
   goto cleanup;
   }
 
//CKA_PRIVATE
ret = p11_set_attribute_value(pObject->pAttr, ulCount, CKA_PRIVATE, (CK_VOID_PTR) &bPrivate, sizeof(CK_BBOOL));
if (ret)
   {
   log_trace(WHERE, "E: p11_set_attribute_value(CKA_PRIVATE) returned %d", ret);
   goto cleanup;
   }

//CKA_LABEL
ret = p11_set_attribute_value(pObject->pAttr, pObject->count, CKA_LABEL, plabel, labelLen);
if (ret)
   {
   log_trace(WHERE, "E: p11_set_attribute_value(CKA_LABEL) returned %d", ret);
   goto cleanup;
   }

//CKA_VALUE
ret = p11_set_attribute_value(pObject->pAttr, pObject->count, CKA_VALUE, pvalue, valueLen);
if (ret)
   {
   log_trace(WHERE, "E: p11_set_attribute_value(CKA_VALUE) returned %d", ret);
   goto cleanup;
   }

//CKA_VALUE_LEN
ret = p11_set_attribute_value(pObject->pAttr, pObject->count, CKA_VALUE_LEN, &valueLen, sizeof(CK_ULONG));
if (ret)
   {
   log_trace(WHERE, "E: p11_set_attribute_value(CKA_VALUE_LEN) returned %d", ret);
   goto cleanup;
   }

//CKA_OBJECT_ID
ret = p11_set_attribute_value(pObject->pAttr, pObject->count, CKA_OBJECT_ID, pobjectID, objectIDLen);
if (ret)
   {
   log_trace(WHERE, "E: p11_set_attribute_value(CKA_OBJECT_ID) returned %d", ret);
   goto cleanup;
   }

pObject->state=P11_CACHED;

cleanup:

return (ret);
}
#undef WHERE 


void p11_clean_object(P11_OBJECT *pObject)
{
	if (pObject == NULL)
		return;
	if(pObject->count > MAX_OBJECT_SIZE)
		return;
	//remove attributes from object
	if (pObject->pAttr != NULL)
	{
		CK_ATTRIBUTE_PTR pAttributes = pObject->pAttr;
		CK_ULONG i=0;
		for(;i<pObject->count;i++)
		{
			if( pAttributes[i].pValue != NULL)
			{
				free(pAttributes[i].pValue);
			}
		}
		free(pObject->pAttr);
		pObject->pAttr = NULL;
	}
	pObject->count = 0;

	//set used flag to 0 so it can be reused.
	//we don't clean the object itself
	pObject->inuse = 0;
	pObject->state = 0;

	return;
}

void p11_clean_attributelist(CK_ATTRIBUTE_PTR pAttrList, CK_ULONG ulCount)
{
	CK_ULONG i;

	if(pAttrList == NULL)
		return;

	for (i=0; i < ulCount; i++)
	{	
			if(pAttrList[i].pValue != NULL)
			{
				free(pAttrList[i].pValue);
			}
	}
	free(pAttrList);
}

void p11_clean_finddata(P11_FIND_DATA* pFindData)
{
	if(pFindData == NULL)
		return;
	p11_clean_attributelist(pFindData->pSearch,pFindData->size);
}


#define WHERE "p11_find_slot_object()"
CK_RV p11_find_slot_object(P11_SLOT *pSlot, CK_ULONG type, CK_ULONG id,  P11_OBJECT **ppObject)
{
CK_RV ret = CKR_OK;
P11_OBJECT *pObject = NULL;
unsigned int h = 0;
CK_VOID_PTR  p = NULL;
CK_ULONG     l = 0;

*ppObject = NULL;

for (h=1; h <= pSlot->nobjects; h++)
   {
   pObject = p11_get_slot_object(pSlot, h);
   if (pObject == NULL)
      {
      //no object found with specified attributes
      ret = -1;
      goto cleanup;
      }

   ret = p11_get_attribute_value(pObject->pAttr, pObject->count, CKA_ID, &p, &l);
   if ( (ret != 0) || ( l!= sizeof(CK_ULONG)) || (memcmp(p, &id, sizeof(CK_ULONG)) != 0) )
      {
      continue;
      }
   ret = p11_get_attribute_value(pObject->pAttr, pObject->count, CKA_CLASS, &p, &l);
   if ( (ret != 0) || ( l!= sizeof(CK_ULONG)) || (memcmp(p, &type, sizeof(CK_ULONG)) != 0) )
      {
      continue;
      }
   *ppObject = pObject;
   return (CKR_OK);
   }

cleanup:

return (ret);
}
#undef WHERE 



#define WHERE "p11_attribute_present()"
int p11_attribute_present(CK_ATTRIBUTE_TYPE type, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount)
{
unsigned int i = 0;

for (i=0; i < ulCount; i++)
   {
   if (type == pTemplate[i].type)
      return(1);
   }

return (0);
}
#undef WHERE





