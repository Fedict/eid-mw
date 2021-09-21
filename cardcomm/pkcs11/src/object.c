/* ****************************************************************************

* eID Middleware Project.
* Copyright (C) 2008-2013 FedICT.
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
#include <stdlib.h>
#include <string.h>
#include "beid_p11.h"
#include "pkcs11log.h"
#include "util.h"
#include "p11.h"
#include "cal.h"
#include "display.h"

//global variable 
//int eidmw_readpermission = 0;

//function declarations
void SetParseFlagByLabel(CK_ULONG* pFilesToParseFlag,CK_UTF8CHAR_PTR pLabel,CK_ULONG len);
void SetParseFlagByObjectID(CK_ULONG* pFilesToParseFlag,CK_UTF8CHAR_PTR pObjectID,CK_ULONG len);
CK_BBOOL CheckLabelInuse(P11_SLOT* pSlot, CK_UTF8CHAR* pLabel, CK_ULONG labelLen);
CK_RV ReadRecordLabel(P11_SLOT* pSlot, CK_UTF8CHAR* pLabel, CK_ULONG labelLen);

#define WHERE "C_CreateObject()"
CK_RV C_CreateObject(CK_SESSION_HANDLE hSession,    /* the session's handle */
										 CK_ATTRIBUTE_PTR  pTemplate,   /* the object's template */
										 CK_ULONG          ulCount,     /* attributes in template */
										 CK_OBJECT_HANDLE_PTR phObject) /* receives new object's handle. */
{
	log_trace(WHERE, "S: C_CreateObject(): nop");
	return (CKR_FUNCTION_NOT_SUPPORTED);
}
#undef WHERE


#define WHERE "C_CopyObject()"
CK_RV C_CopyObject(CK_SESSION_HANDLE    hSession,    /* the session's handle */
									 CK_OBJECT_HANDLE     hObject,     /* the object's handle */
									 CK_ATTRIBUTE_PTR     pTemplate,   /* template for new object */
									 CK_ULONG             ulCount,     /* attributes in template */
									 CK_OBJECT_HANDLE_PTR phNewObject) /* receives handle of copy */
{
	log_trace(WHERE, "S: C_CopyObject(): nop");
	return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE





#define WHERE "C_DestroyObject()"
CK_RV C_DestroyObject(CK_SESSION_HANDLE hSession,  /* the session's handle */
											CK_OBJECT_HANDLE  hObject)   /* the object's handle */
{
	log_trace(WHERE, "S: C_DestroyObject(): nop");
	return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE




#define WHERE "C_GetObjectSize()"
CK_RV C_GetObjectSize(CK_SESSION_HANDLE hSession,  /* the session's handle */
											CK_OBJECT_HANDLE  hObject,   /* the object's handle */
											CK_ULONG_PTR      pulSize)   /* receives size of object */
{
	log_trace(WHERE, "S: C_GetObjectSize(): nop");
	return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE



#define WHERE "C_GetAttributeValue()"
CK_RV C_GetAttributeValue(CK_SESSION_HANDLE hSession,   /* the session's handle */
													CK_OBJECT_HANDLE  hObject,    /* the object's handle */
													CK_ATTRIBUTE_PTR  pTemplate,  /* specifies attributes, gets values */
													CK_ULONG          ulCount)    /* attributes in template */
{
	/*
	This function returns the values from the object.
	Object is cached so objects are read only once and remain valid until new session is setup with token.
	Objects are allready initialized (but not read) during connection with token.
	*/

    CK_RV status;
    CK_RV ret = 0;
	P11_SESSION *pSession = NULL;
	P11_SLOT    *pSlot    = NULL;
	P11_OBJECT  *pObject  = NULL;
	unsigned int j = 0;
	void  *pValue    = NULL;
	CK_ULONG len = 0;
	log_trace(WHERE, "I: enter");

	if (p11_get_init() != BEIDP11_INITIALIZED)
	{
		log_trace(WHERE, "I: leave, CKR_CRYPTOKI_NOT_INITIALIZED");
		return (CKR_CRYPTOKI_NOT_INITIALIZED);
	}		

	p11_lock();

	log_trace(WHERE, "S: C_GetAttributeValue(hObject=%lu)",hObject);

	ret = p11_get_session(hSession, &pSession);
	if (ret)
	{
		log_trace(WHERE, "E: Invalid session handle (%lu)", hSession);
		goto cleanup;
	}

	pSlot = p11_get_slot(pSession->hslot);
	if (pSlot == NULL)
	{
		log_trace(WHERE, "E: p11_get_slot(%lu) returns null", pSession->hslot);
		ret = CKR_SLOT_ID_INVALID;
		goto cleanup;
	}

	pObject = p11_get_slot_object(pSlot, hObject);
	if (pObject == NULL)
	{
		log_trace(WHERE, "E: slot %lu: object %lu does not exist", pSession->hslot, hObject);
		ret = CKR_OBJECT_HANDLE_INVALID;
		goto cleanup;
	}

	//read object from token if not cached allready
	if (pObject->state != P11_CACHED)
	{
		ret = cal_read_object(pSession->hslot, pObject);
		if (ret != 0)
		{
			log_trace(WHERE, "E: p11_read_object() returned %lu", ret);
			goto cleanup;
		}
	}

	/*   if (pSlot->login_type < 0) //CKU_SO=0; CKU_USER=1
	{
	if (p11_get_attribute_value(pObject->pAttr, pObject->count, CKA_PRIVATE, (CK_VOID_PTR *) &pbPrivate, &len) != CKR_OK)
	{
	log_trace(WHERE, "E: missing CKA_PRIVATE attribute in our object: bad implementation");
	continue;
	}

	if ( (len == sizeof(CK_BBOOL)) && (*pbPrivate == CK_TRUE) )
	{
	log_trace(WHERE, "E: Not allowed to retrieve private objects");
	continue;
	}
	}*/

	//retrieve all objects as listed in template and fill the template
	//action is done for all attributes, even if some attributes give errors or buffer is too small
	//there is however only one return code to return so we have to keep the most important return code.
	for (j = 0; j < ulCount; j++)
	{
		status = p11_get_attribute_value(pObject->pAttr, pObject->count, pTemplate[j].type, (CK_VOID_PTR *) &pValue, &len);
		if (status != CKR_OK)
		{
			log_template("E: C_GetAttributeValue status != CKR_OK", &pTemplate[j], 1);
			log_trace(WHERE, "E: p11_get_attribute_value (object=%lu) returned %s", hObject, log_map_error(status));
			pTemplate[j].ulValueLen = (CK_ULONG) -1;
			ret = status;
			continue;
		}

		if (pTemplate[j].pValue == NULL)
		{
			/* in this case we return the real length of the value */
			pTemplate[j].ulValueLen = len;
			continue;
		}

		if (len > pTemplate[j].ulValueLen)
		{
			pTemplate[j].ulValueLen = (CK_ULONG) -1;
			ret = CKR_BUFFER_TOO_SMALL;
			continue;
		}

		pTemplate[j].ulValueLen = len;
		memcpy(pTemplate[j].pValue, pValue, len);
	}

	if (ulCount != 0)
		log_template("I: Template out:", pTemplate, ulCount);

cleanup:
	p11_unlock();
	return ret;
}
#undef WHERE




#define WHERE "C_SetAttributeValue()"
CK_RV C_SetAttributeValue(CK_SESSION_HANDLE hSession,   /* the session's handle */
													CK_OBJECT_HANDLE  hObject,    /* the object's handle */
													CK_ATTRIBUTE_PTR  pTemplate,  /* specifies attributes and values */
													CK_ULONG          ulCount)    /* attributes in template */
{
	log_trace(WHERE, "S: C_SetAttributeValue(): nop");
	return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE


#define WHERE "C_FindObjectsInit()"
CK_RV C_FindObjectsInit(CK_SESSION_HANDLE hSession,   /* the session's handle */
						CK_ATTRIBUTE_PTR  pTemplate,  /* attribute values to match */
						CK_ULONG          ulCount)    /* attributes in search template */
{
	P11_SESSION *pSession = NULL;
	P11_SLOT    *pSlot    = NULL;
	P11_FIND_DATA *pData = NULL;
	CK_RV ret;
	CK_ULONG      *pclass = NULL;
	CK_ULONG       len = 0;
	CK_BBOOL			addIdObjects = CK_FALSE;
	CK_ULONG			filesToCacheFlag = CACHED_DATA_TYPE_ALL_DATA;
//	CK_BYTE				allowCardRead = P11_DISPLAY_NO;
	log_trace(WHERE, "I: enter");

	if (p11_get_init() != BEIDP11_INITIALIZED)
	{
		log_trace(WHERE, "I: leave, CKR_CRYPTOKI_NOT_INITIALIZED");
		return (CKR_CRYPTOKI_NOT_INITIALIZED);
	}		

	p11_lock();

	log_trace(WHERE, "S: C_FindObjectsInit(session %lu)", hSession);
	if (ulCount == 0)
		log_trace(WHERE, "I: empty template => search all objects");
	else
		log_template("I: Search template:", pTemplate, ulCount);

	ret = p11_get_session(hSession, &pSession);
	if (pSession == NULL)
	{
		log_trace(WHERE, "E: pSession == NULL");
		goto cleanup;
	}
	if (ret)
	{
		log_trace(WHERE, "E: Invalid session (%lu) (%s)", hSession, log_map_error(ret));
		//if (ret == CKR_DEVICE_REMOVED)
		//ret = CKR_SESSION_HANDLE_INVALID;
		//ret = CKR_FUNCTION_FAILED;
		goto cleanup;
	}

	pSlot = p11_get_slot(pSession->hslot);
	if (pSlot == NULL)
	{
		log_trace(WHERE, "E: p11_get_slot(%lu) returns null", pSession->hslot);
		ret = CKR_SLOT_ID_INVALID;
		goto cleanup;
	}

	/* add check here to avoid useless calls to C_FindObjects() */
	/* reason to add this is that e.g. Firefox for every certificate in its store starts a find operation with CKA_CLASS_TYPE */
	/* that is unknown by this implementation */
	/* CKA_CLASS_TYPE we support is only CKO_CERTIFICATE, CKO_PRIVATE_KEY and CKO_PUBLIC_KEY */
	/* Sun-PKCS11 cannot handle CKR_ATTRIBUTE_VALUE_INVALID properly so => initialize search and findObjects will just return 0 matching objects
	in case of CKO_DATA */
	if (ulCount)
	{
		ret = p11_get_attribute_value(pTemplate, ulCount, CKA_CLASS, (CK_VOID_PTR *) &pclass, &len);
		if ( (ret == 0) && (len == sizeof(CK_ULONG) ) )
		{
#ifndef PKCS11_FF
			if( (*pclass == CKO_CERTIFICATE) || (*pclass == CKO_PRIVATE_KEY) || (*pclass == CKO_PUBLIC_KEY) || (*pclass == CKO_SECRET_KEY) )
			{
				ret = cal_init_objects(pSlot);
				if (ret != CKR_OK)
				{
					log_trace(WHERE, "E: cal_init_objects() returned %s.",log_map_error(ret));
				}
			}
#endif
			//CKO_SECRET_KEY is not supported but for SUN-PKCS11 we allow a search that will result in 0 objects
			if ( (*pclass != CKO_CERTIFICATE) && (*pclass != CKO_PRIVATE_KEY) && (*pclass != CKO_PUBLIC_KEY) && (*pclass != CKO_SECRET_KEY) && (*pclass != CKO_DATA))
			{
				log_trace(WHERE, "I: CKA_CLASS (%0lx) not supported by this PKCS11 module", *pclass);
				ret = CKR_ATTRIBUTE_VALUE_INVALID;
				goto cleanup;
			}
			else if (*pclass == CKO_DATA)
			{
				addIdObjects = CK_TRUE;
			}			
		}
		//We only return the CKO_DATA objects when specifically asked for, this to prevent webbrowsers
		//to read the entire carddata, while they only need the certificates. 
		//(e.g. we saw firefox do a C_FindObjectsInit with only the auth cert's CKA_VALUE and its value
		// in the template)
		//else if (len == 0)// no CKA_CLASS attribute in the template
		//{
		//	addIdObjects = CK_TRUE;
		//}
	}
	else
	{
#ifndef PKCS11_FF
		ret = cal_init_objects(pSlot);
		if (ret != CKR_OK)
		{
			log_trace(WHERE, "E: cal_init_objects() returned %s_",log_map_error(ret));
		}
#endif
	}
	//see comment above, We only return the CKO_DATA objects when specifically asked for
	//else
	//{
	//	addIdObjects = CK_TRUE;
	//}



	//is there an active search operation for this session
	if (pSession->Operation[P11_OPERATION_FIND].active)
	{
		log_trace(WHERE, "W: Session %lu: search operation allready exists", hSession);
		ret = CKR_OPERATION_ACTIVE;
		goto cleanup;
	}

/*	if(addIdObjects == CK_TRUE)
	{
		//parse the search template
		CK_UTF8CHAR* pLabel;
		CK_UTF8CHAR* pObjectID;

		ret = p11_get_attribute_value(pTemplate, ulCount, CKA_OBJECT_ID, (CK_VOID_PTR *) &pObjectID, &len);
		if ( (ret == 0) && (len > 0 ) )
		{
			SetParseFlagByObjectID(&filesToCacheFlag,pObjectID,len);
		}
		else //no CKA_OBJECT_ID specified
		{
			ret = p11_get_attribute_value(pTemplate, ulCount, CKA_LABEL, (CK_VOID_PTR *) &pLabel, &len);
			if ( (ret == 0) && (len > 0 ) )
			{
				SetParseFlagByLabel(&filesToCacheFlag,pLabel,len);
			}
		}

		
		if((filesToCacheFlag != CACHED_DATA_TYPE_CARDDATA) && (filesToCacheFlag != CACHED_DATA_TYPE_RNCERT))
		{
			if ((pSession->bReadDataAllowed == P11_READDATA_ASK) & (eidmw_readpermission != P11_READDATA_ALWAYS))
			{
				allowCardRead = AllowCardReading();
				switch(allowCardRead)
				{
				case P11_DISPLAY_YES:
					pSession->bReadDataAllowed = P11_READDATA_ALLOWED;
					break;
				case P11_DISPLAY_ALWAYS:
					pSession->bReadDataAllowed = P11_READDATA_ALLOWED;
					eidmw_readpermission = P11_READDATA_ALWAYS;
					//allowed for as long as this pkcs11 instance exists, put it in some variable
					log_trace(WHERE, "I: Al reading from the card");
					break;
				case P11_DISPLAY_NO:
					//keep asking
				//case P11_DISPLAY_NEVER:
					//pSession->bReadDataAllowed = P11_READDATA_REFUSED;	
				default:							
					log_trace(WHERE, "I: User does not allow reading from the card");
					ret = CKR_FUNCTION_FAILED;
					goto cleanup;
					break;
				}
			}
			else if (pSession->bReadDataAllowed == P11_READDATA_REFUSED)
			{
				log_trace(WHERE, "I: User did not allow reading from the card during this session");
				ret = CKR_FUNCTION_FAILED;
				goto cleanup;
			}
		}
		
	}*/

	/* init search operation */
	pData = (P11_FIND_DATA *)pSession->Operation[P11_OPERATION_FIND].pData;
	if(pData == NULL)
	{
		pSession->Operation[P11_OPERATION_FIND].pData = (P11_FIND_DATA *) malloc (sizeof(P11_FIND_DATA));
		pData = (P11_FIND_DATA *)pSession->Operation[P11_OPERATION_FIND].pData;
		if (pData == NULL)
		{
			log_trace( WHERE, "E: error allocating memory");
			ret = CKR_HOST_MEMORY;
			goto cleanup;
		}
	}

	//first handle = 1
	pData->hCurrent = 1;
	pData->pSearch  = NULL;
	pData->size     = 0;

	//keep search template if at least one entry in the search template
	if (ulCount > 0)
	{
		pData->pSearch = (CK_ATTRIBUTE_PTR) malloc(sizeof(CK_ATTRIBUTE)*ulCount);
		if (pData->pSearch == NULL)
		{
			log_trace(WHERE, "E: error allocating memory for object search template()");
			ret = CKR_HOST_MEMORY;
			goto cleanup;
		}
		memset(pData->pSearch,0,sizeof(CK_ATTRIBUTE)*ulCount);

		ret = p11_copy_object(pTemplate, ulCount, pData->pSearch);
		if (ret)
		{
			log_trace(WHERE, "E: p11_copy_object() returned %lu", ret);
			goto cleanup;
		}
	}

	pData->size = ulCount;

	//set search operation to active state since there can be only one
	pSession->Operation[P11_OPERATION_FIND].active = 1;

	if ( addIdObjects )
	{
		CK_UTF8CHAR* pLabel;
		CK_UTF8CHAR* pObjectID;

		ret = p11_get_attribute_value(pTemplate, ulCount, CKA_OBJECT_ID, (CK_VOID_PTR *)&pObjectID, &len);
		if ((ret == 0) && (len > 0))
		{
			SetParseFlagByObjectID(&filesToCacheFlag, pObjectID, len);
		}
		else //no CKA_OBJECT_ID specified
		{
			ret = p11_get_attribute_value(pTemplate, ulCount, CKA_LABEL, (CK_VOID_PTR *)&pLabel, &len);
			if ((ret == 0) && (len > 0))
			{
				SetParseFlagByLabel(&filesToCacheFlag, pLabel, len);
			}

			//check if a record_ object is being asked for
			if (filesToCacheFlag == CACHED_DATA_TYPE_PER_RECORD)
			{
				//check if the record_ object is already present
				if (CheckLabelInuse(pSlot, pLabel, len) == CK_FALSE)
				{
					ret = ReadRecordLabel(pSlot, pLabel, len);
					{
						if (ret != 0) {
							log_trace(WHERE, "E: ReadRecordLabel() failed with %lu", ret);
							goto cleanup;
						}
					}
				}
				//we have (now or before) read this record from the eID card
				ret = CKR_OK;

				goto cleanup;
			}
		}

		//check if the data isn't cached already
		if(	((filesToCacheFlag != CACHED_DATA_TYPE_ALL_DATA) && ((pSlot->ulCardDataCached & filesToCacheFlag) == FALSE)) ||
			((filesToCacheFlag == CACHED_DATA_TYPE_ALL_DATA) && (pSlot->ulCardDataCached != CACHED_DATA_TYPE_ALL_DATA)) )
		{
			CK_ULONG counter = 0;
			CK_ULONG flagsToCheckListLen = 8;
			CK_ULONG flagsToCheckList[8] = {CACHED_DATA_TYPE_ID,CACHED_DATA_TYPE_ADDRESS,CACHED_DATA_TYPE_PHOTO,
				CACHED_DATA_TYPE_RNCERT,CACHED_DATA_TYPE_SIGN_DATA_FILE,CACHED_DATA_TYPE_SIGN_ADDRESS_FILE, 
				CACHED_DATA_TYPE_BASIC_KEY_FILE, CACHED_DATA_TYPE_TOKENINFO };

			switch(filesToCacheFlag)
			{
			case CACHED_DATA_TYPE_ALL_DATA:
				//cache and parse whatever isn't cached already
				//first check if carddata is cashed already, if not parse and cache it
				if( (pSlot->ulCardDataCached & CACHED_DATA_TYPE_CARDDATA) == 0){
					ret = cal_get_card_data(pSession->hslot);
					if (ret != 0){
						log_trace(WHERE, "E: cal_get_card_data() returned %lu", ret);
						goto cleanup;
					}
				}
				//check which other files are cached already, parse and cache those that aren't
				while(counter < flagsToCheckListLen){
					ret = cal_read_ID_files(pSession->hslot,flagsToCheckList[counter]);
					if (ret != 0){
						log_trace(WHERE, "E: cal_read_ID_files() returned %lu", ret);
						goto cleanup;
					}
					counter++;
				}
				break;
			case CACHED_DATA_TYPE_CARDDATA:
				//cache and parse only the carddata
				ret = cal_get_card_data(pSession->hslot);
				if (ret != 0){
					log_trace(WHERE, "E: cal_get_card_data() returned %lu", ret);
					goto cleanup;
				}
				break;
			default:
				//cache and parse only the requested file type
				ret = cal_read_ID_files(pSession->hslot,filesToCacheFlag);
				if (ret != 0){
					log_trace(WHERE, "E: cal_read_ID_files() returned %lu", ret);
					goto cleanup;
				}
			}
			//remember the file(s) we cashed
			pSlot->ulCardDataCached |= filesToCacheFlag;
		}
	}

	ret = CKR_OK;

cleanup:
	p11_unlock();
	return ret;
}
#undef WHERE


#define WHERE "C_FindObjects()"
CK_RV C_FindObjects(CK_SESSION_HANDLE    hSession,          /* the session's handle */
										CK_OBJECT_HANDLE_PTR phObject,          /* receives object handle array */
										CK_ULONG             ulMaxObjectCount,  /* max handles to be returned */
										CK_ULONG_PTR         pulObjectCount)    /* actual number returned */
{
	/*

	this function finds handles to objects but does not actually reads them.
	this function returns handles to objects that exist on the token.
	PKCS15 defines existance of attributes that should be readable from token

	*/

	CK_RV ret = 0;
	P11_SESSION   *pSession = NULL;
	P11_SLOT      *pSlot = NULL;
	P11_FIND_DATA *pData = NULL;
	P11_OBJECT    *pObject = NULL;
	CK_BBOOL      *pbToken = NULL;
	void          *p = NULL;
	CK_ULONG      *pclass = NULL;
	int           match = 0;
    unsigned long h = 0;
    unsigned int  j = 0;

	CK_ULONG		len = 0;
	CK_BBOOL      bShowHidden = CK_FALSE;

	log_trace(WHERE, "I: enter");

	if (p11_get_init() != BEIDP11_INITIALIZED)
	{
		log_trace(WHERE, "I: leave, CKR_CRYPTOKI_NOT_INITIALIZED");
		return (CKR_CRYPTOKI_NOT_INITIALIZED);
	}		

	p11_lock();

	log_trace(WHERE, "S: p11_get_session(session %lu) enter", hSession);

	ret = p11_get_session(hSession, &pSession);
	log_trace(WHERE, "S: p11_get_session(session %lu) leave", hSession);
	if (pSession == NULL || ret != CKR_OK)
		// if (ret)
	{
		log_trace(WHERE, "E: Invalid session handle (%lu)", hSession);
		goto cleanup;
	}

	if (pSession->Operation[P11_OPERATION_FIND].active == 0)
	{
		log_trace(WHERE, "E: For this session no search operation is initiated");
		ret = CKR_OPERATION_NOT_INITIALIZED;
		goto cleanup;
	}

	//get search template
	pData = (P11_FIND_DATA *) pSession->Operation[P11_OPERATION_FIND].pData;
	if (pData == NULL)
	{
		log_trace(WHERE, "E: Session (%lu): search data not initialized correctly", hSession);
		ret = CKR_OPERATION_NOT_INITIALIZED;
		goto cleanup;
	}

	/* VSC this code was moved to here since Sun-PKCS11 cannot handle CKR_Attribute_value_invalid in C_FindObjectsInit() properly!!! */
	/* here we just return 0 objects in case of class type that is not supported */
	ret = p11_get_attribute_value(pData->pSearch, pData->size, CKA_CLASS, (CK_VOID_PTR *) &pclass, &len);
	if ( (ret == 0) && (len == sizeof(CK_ULONG) ) )
	{
		if ( (*pclass != CKO_CERTIFICATE) && (*pclass != CKO_PRIVATE_KEY) && (*pclass != CKO_PUBLIC_KEY) && (*pclass != CKO_DATA) )
		{
			ret = CKR_OK; //ret = CKR_ATTRIBUTE_VALUE_INVALID;
			*pulObjectCount = 0;
			goto cleanup;
		}
	}

	//check if we have a TOKEN attribute to look for
	//in case of null search template we search for all objects
	//Firefox does not set TOKEN object for CKO_PRIVATE_KEY objects so for the moment we allow looking for them, id has to match anyway.
	len = sizeof(CK_BBOOL);
	if (pData->size > 0)
	{
		ret = p11_get_attribute_value(pData->pSearch, pData->size, CKA_TOKEN, (CK_VOID_PTR *) &pbToken, &len);
		//if ((ret != CKR_OK) || ( (len == sizeof(CK_BBOOL) ) && (*pbToken == CK_FALSE) ) )
		//for the moment if CKA_TOKEN is specified and set to false, we reply that only token objects can be searched for and continue with ok
		if ((ret == CKR_OK) && (len == sizeof(CK_BBOOL)) && (*pbToken == CK_FALSE) ) 
		{
			log_trace(WHERE, "W: only token objects can be searched for");
			*pulObjectCount = 0;
			ret = CKR_OK;
			goto cleanup;
		}
	}

	//check if we have a LABEL attribute to look for
	//if not, we will not return hidden objects, if so we will when their LABEL attributes match
	len = 0;
	if (pData->size > 0)
	{
		ret = p11_get_attribute_value(pData->pSearch, pData->size, CKA_LABEL, (CK_VOID_PTR *)&pbToken, &len);
		//when the type (CKA_LABEL) is found, CKR_OK is returned
		if ((ret == CKR_OK) && (len > 0))
		{
			//when searching for a non-empty label, set showhidden flag (so it is returned in case the data object is hidden)
			log_trace(WHERE, "I: CKA_LABEL in search string, turning hidden flag off");
			bShowHidden = CK_TRUE;
		}
	}

	pSlot = p11_get_slot(pSession->hslot);
	if (pSlot == NULL)
	{
		log_trace(WHERE, "E: p11_get_slot(%lu) returns null", pSession->hslot);
		ret = CKR_SLOT_ID_INVALID;
		goto cleanup;
	}

	*pulObjectCount = 0;

	//for all objects in token, match with search template as long as we need, keep handle to current token object
	for (h = pData->hCurrent; h <= (pSlot->nobjects) && (*pulObjectCount < ulMaxObjectCount); h++, pData->hCurrent++)
	{
		pObject = p11_get_slot_object(pSlot, h);
		if (pObject == NULL)
		{
			log_trace(WHERE, "E: invalid object handle, call C_FindObjectsInit() first");
			ret = CKR_OPERATION_NOT_INITIALIZED;
			goto cleanup;
		}
		if (pObject->inuse == 0)
			continue; //this object is not in use by the token()

		if ( (pObject->hidden == 1) && !bShowHidden)
		{
			//this object is hidden, only return it when specifically asked for (bShowHidden flag enabled)
			continue;
		}

		//if not logged in, objects with missing CKA_PRIVATE or CKA_PRIVATE set to false will be ignored
#if 0 //TODO
		if (pSlot->login_type < 0) //CKU_SO=0; CKU_USER=1
		{
			if (p11_get_attribute_value(pObject->pAttr, pObject->count, CKA_PRIVATE, (CK_VOID_PTR *) &pbPrivate, &len) != CKR_OK)
			{
				log_trace(WHERE, "E: missing CKA_PRIVATE attribute in our object: bad implementation");
				continue;
			}

			if ( (len == sizeof(CK_BBOOL)) && (*pbPrivate == CK_TRUE) )
			{
				log_trace(WHERE, "E: Not allowed to retrieve private objects");
				continue;
			}
		}
#endif
		// Try to match every attribute
		match = 1;
		//if pData->size = 0 => this means that we will search for every object!!! match = 1 so for() is skipped and object is returned
		for (j = 0; j < pData->size; j++)
		{
			//get the value of the attribute from the token object and compare with the search attribute
			if (p11_get_attribute_value(pObject->pAttr, pObject->count, pData->pSearch[j].type, &p, &len) != CKR_OK)
			{
				match = 0;
				break;
			}

			// printf("pData->pSearch[%d].ulValueLen=%d <> len=%d\n",j,pData->pSearch[j].ulValueLen,len);

			if (pData->pSearch[j].ulValueLen != len)
			{
				match = 0;
				break;
			}

			if (memcmp(pData->pSearch[j].pValue, p, len) != 0)
			{
				match = 0;
				break;
			}
		}

		if (match)
		{
			log_trace(WHERE, "I: Slot %lu: Object %lu matches", pSession->hslot, h);
			//put handle to object in list
			phObject[*pulObjectCount] = (CK_OBJECT_HANDLE) h;
			*pulObjectCount +=1;
		}
		else
			log_trace(WHERE, "I: Slot %lu: Object %lu no match with search template", pSession->hslot, h);
	}

	ret = CKR_OK;

cleanup: 
	log_trace(WHERE, "I: leave");
	p11_unlock();
	return ret;
}
#undef WHERE





#define WHERE "C_FindObjectsFinal()"
CK_RV C_FindObjectsFinal(CK_SESSION_HANDLE hSession) /* the session's handle */
{
	P11_SESSION *pSession = NULL;
	P11_FIND_DATA *pData = NULL;
	CK_RV ret;
	log_trace(WHERE, "I: enter");

	if (p11_get_init() != BEIDP11_INITIALIZED)
	{
		log_trace(WHERE, "I: leave, CKR_CRYPTOKI_NOT_INITIALIZED");
		return (CKR_CRYPTOKI_NOT_INITIALIZED);
	}		

	p11_lock();

	log_trace(WHERE, "S: C_FindObjectsFinal(session %lu)", hSession);

	ret = p11_get_session(hSession, &pSession);
	if (pSession == NULL || ret != CKR_OK)
		//omit error card removed here since FireFox has a problem with it.
		// if (ret)
	{
		log_trace(WHERE, "E: Invalid session handle (%lu)", hSession);
		goto cleanup;
	}

	if (pSession->Operation[P11_OPERATION_FIND].active == 0)
	{
		log_trace(WHERE, "I: For this session no search operation is active");
		//we return without problem
		ret = CKR_OPERATION_NOT_INITIALIZED;
		goto cleanup;
	}

	//get search template
	pData = (P11_FIND_DATA *) pSession->Operation[P11_OPERATION_FIND].pData;
	if (pData == NULL)
	{
		log_trace(WHERE, "I: For this session no search operation is active");
		//we return without problem
		ret = CKR_OK;
		goto cleanup;
	}
	//free search template
	p11_clean_finddata(pData);	

	//free find operation data
	free(pData);
	pSession->Operation[P11_OPERATION_FIND].pData = NULL;

	pSession->Operation[P11_OPERATION_FIND].active = 0;

	ret = CKR_OK;

cleanup:
	p11_unlock();
	return ret;
}
#undef WHERE


void SetParseFlagByLabel(CK_ULONG* pFilesToParseFlag,CK_UTF8CHAR_PTR pLabel,CK_ULONG len)
{
	CK_ULONG nrOfItems = 0;
	CK_ULONG counter = 0;
	BEID_DATA_LABELS_NAME ID_LABELS[]=BEID_ID_DATA_LABELS;
	BEID_DATA_LABELS_NAME ADDRESS_LABELS[]=BEID_ADDRESS_DATA_LABELS;

	CK_ULONG carddataLabelsListLen = 14;
	const char* carddataLabelsList[14] = {BEID_LABEL_DATA_SerialNr,BEID_LABEL_DATA_CompCode,BEID_LABEL_DATA_OSNr,
		BEID_LABEL_DATA_OSVersion,BEID_LABEL_DATA_SoftMaskNumber,BEID_LABEL_DATA_SoftMaskVersion,
		BEID_LABEL_DATA_ApplVersion,BEID_LABEL_DATA_GlobOSVersion,BEID_LABEL_DATA_ApplIntVersion,
		BEID_LABEL_DATA_PKCS1Support,BEID_LABEL_DATA_ApplLifeCycle,BEID_LABEL_DATA_KeyExchangeVersion,
		BEID_LABEL_DATA_Signature,BEID_LABEL_ATR};

	//labels from identity data
	nrOfItems = sizeof(ID_LABELS)/sizeof(BEID_DATA_LABELS_NAME);
	while(counter < nrOfItems)
	{
		if(strlen(ID_LABELS[counter].name) == len)
		{
			if(memcmp(ID_LABELS[counter].name,pLabel,len)==0){
				*pFilesToParseFlag=CACHED_DATA_TYPE_ID;
				return;
			}
		}
		counter++;
	}
	//labels from address data
	counter = 0;
	nrOfItems = sizeof(ADDRESS_LABELS)/sizeof(BEID_DATA_LABELS_NAME);
	while(counter < nrOfItems)
	{		
		if(strlen(ADDRESS_LABELS[counter].name) == len)
		{
			if(memcmp(ADDRESS_LABELS[counter].name,pLabel,len)==0){
				*pFilesToParseFlag=CACHED_DATA_TYPE_ADDRESS;
				return;
			}
		}
		counter++;
	}

	//label of the foto file
	if(strlen(BEID_LABEL_PHOTO) == len)
	{
		if(memcmp(BEID_LABEL_PHOTO,pLabel,len)==0){
			*pFilesToParseFlag=CACHED_DATA_TYPE_PHOTO;
			return;
		}
	}
	//label of the RN cert
	if(strlen(BEID_LABEL_CERT_RN) == len)
	{
		if(memcmp(BEID_LABEL_CERT_RN,pLabel,len)==0){
			*pFilesToParseFlag=CACHED_DATA_TYPE_RNCERT;
			return;
		}
	}
	//label of the data signature file
	if(strlen(BEID_LABEL_SGN_RN) == len)
	{
		if(memcmp(BEID_LABEL_SGN_RN,pLabel,len)==0){
			*pFilesToParseFlag=CACHED_DATA_TYPE_SIGN_DATA_FILE;
			return;
		}
	}
	//label of the address signature cert
	if(strlen(BEID_LABEL_SGN_ADDRESS) == len)
	{
		if(memcmp(BEID_LABEL_SGN_ADDRESS,pLabel,len)==0){
			*pFilesToParseFlag=CACHED_DATA_TYPE_SIGN_ADDRESS_FILE;
			return;
		}
	}
	//label of the public basic key
	if (strlen(BEID_LABEL_BASIC_KEY) == len)
	{
		if (memcmp(BEID_LABEL_BASIC_KEY, pLabel, len) == 0) {
			*pFilesToParseFlag = CACHED_DATA_TYPE_BASIC_KEY_FILE;
			return;
		}
	}
	//label of the personalisation versions
	if (strlen(BEID_LABEL_PersoVersions) == len)
	{
		if (memcmp(BEID_LABEL_PersoVersions, pLabel, len) == 0) {
			*pFilesToParseFlag = CACHED_DATA_TYPE_TOKENINFO;
			return;
		}
	}
	//labels from card data
	counter = 0;
	while(counter < carddataLabelsListLen)
	{
		if(strlen(carddataLabelsList[counter]) == len)
		{
			if(memcmp(carddataLabelsList[counter],pLabel,len)==0){
				*pFilesToParseFlag=CACHED_DATA_TYPE_CARDDATA;
				return;
			}
		}
		counter++;
	}
	//label of per record data
	if (len >= 7)
	{
		if (memcmp("record_", pLabel, 7) == 0) 
		{
			*pFilesToParseFlag = CACHED_DATA_TYPE_PER_RECORD;
			return;
		}
	}

	//unknown label
	return;
}


void SetParseFlagByObjectID(CK_ULONG* pFilesToParseFlag,CK_UTF8CHAR_PTR pObjectID,CK_ULONG len)
{
	if(strlen(BEID_OBJECTID_ID)==len){
		if(memcmp(BEID_OBJECTID_ID,pObjectID,len)==0){
			*pFilesToParseFlag=CACHED_DATA_TYPE_ID;
			return;
		}
	}
	if(strlen(BEID_OBJECTID_ADDRESS)==len){
		if(memcmp(BEID_OBJECTID_ADDRESS,pObjectID,len)==0){
			*pFilesToParseFlag=CACHED_DATA_TYPE_ADDRESS;
			return;
		}
	}
	if(strlen(BEID_OBJECTID_PHOTO)==len){
		if(memcmp(BEID_OBJECTID_PHOTO,pObjectID,len)==0){
			*pFilesToParseFlag=CACHED_DATA_TYPE_PHOTO;
			return;
		}
	}
	if(strlen(BEID_OBJECTID_CARDDATA)==len){
		if(memcmp(BEID_OBJECTID_CARDDATA,pObjectID,len)==0){
			*pFilesToParseFlag=CACHED_DATA_TYPE_CARDDATA;
			return;
		}
	}
	if(strlen(BEID_OBJECTID_RNCERT)==len){
		if(memcmp(BEID_OBJECTID_RNCERT,pObjectID,len)==0){
			*pFilesToParseFlag=CACHED_DATA_TYPE_RNCERT;
			return;
		}
	}
	if(strlen(BEID_OBJECTID_SIGN_DATA_FILE)==len){
		if(memcmp(BEID_OBJECTID_SIGN_DATA_FILE,pObjectID,len)==0){
			*pFilesToParseFlag=CACHED_DATA_TYPE_SIGN_DATA_FILE;
			return;
		}
	}
	if(strlen(BEID_OBJECTID_SIGN_ADDRESS_FILE)==len){
		if(memcmp(BEID_OBJECTID_SIGN_ADDRESS_FILE,pObjectID,len)==0){
			*pFilesToParseFlag=CACHED_DATA_TYPE_SIGN_ADDRESS_FILE;
			return;
		}
	}
	if (strlen(BEID_OBJECTID_BASIC_KEY_FILE) == len) {
		if (memcmp(BEID_OBJECTID_BASIC_KEY_FILE, pObjectID, len) == 0) {
			*pFilesToParseFlag = CACHED_DATA_TYPE_BASIC_KEY_FILE;
			return;
		}
	}
	if (strlen(BEID_OBJECTID_TOKENINFO) == len) {
		if (memcmp(BEID_OBJECTID_TOKENINFO, pObjectID, len) == 0) {
			*pFilesToParseFlag = CACHED_DATA_TYPE_TOKENINFO;
			return;
		}
	}
	
	return;
}

/* 
 * To be called from the pkcs#11 API functions C_
 * p11_lock() should be held;
 */
#define WHERE "CheckLabelInuse()"
CK_BBOOL CheckLabelInuse(P11_SLOT* pSlot, CK_UTF8CHAR* pLabel, CK_ULONG labelLen)
{
	CK_ULONG i = 0;
	CK_BBOOL ret = CK_FALSE;
	CK_UTF8CHAR* pObjectLabel;
	CK_ULONG	objectLabelLen = 0;
	P11_OBJECT*	pObject = NULL;

	log_trace(WHERE, "I: enter");

//	if (p11_get_init() != BEIDP11_INITIALIZED)
//	{
//		log_trace(WHERE, "I: leave, CKR_CRYPTOKI_NOT_INITIALIZED");
//		return (CK_FALSE);
//	}

//	p11_lock();
	if (pSlot == NULL)
	{
		log_trace(WHERE, "E: pSlot == NULL");
		ret = CK_FALSE;
		goto cleanup;
	}

	if (pSlot->nobjects == 0)
	{
		log_trace(WHERE, "I: no objects in list, so we will not find the one we are looking for");
		ret = CK_FALSE;
		goto cleanup;
	}

	//for all objects in session, check if the value of CK_LABEL matches pLabel
	for (i = 0; i <= (pSlot->nobjects); i++)
	{
		pObject = p11_get_slot_object(pSlot, i);
		if (pObject == NULL)
		{
			log_trace(WHERE, "E: invalid object handle, call C_FindObjectsInit() first");
			ret = CK_FALSE;
			goto cleanup;
		}
		if (pObject->inuse == 0)
			continue; //this object is not in use by the token()

		//retrieve the CKA_LABEL of the object
		if (p11_get_attribute_value(pObject->pAttr, pObject->count, CKA_LABEL, (CK_VOID_PTR*)&pObjectLabel, &objectLabelLen) == CKR_OK)
		{
			if (objectLabelLen > 0)
			{
				if (labelLen == objectLabelLen)
				{
					if (memcmp(pObjectLabel, pLabel, labelLen) == 0)
					{
						ret = CK_TRUE;
						goto cleanup;
					}
				}
			}
		}
		else
		{
			//couldn't check the attribute, skip this object
			continue;
		}
	}

cleanup:
	log_trace(WHERE, "I: leave");
//	p11_unlock();
	return ret;
}
#undef WHERE

/*
 *Search for the file (on the card) this label belongs to, 
 *then read only the correpsponding record from the card, 
 *and create and store an object that represents it
*/
#define WHERE "ReadRecordLabel()"
CK_RV ReadRecordLabel(P11_SLOT* pSlot, CK_UTF8CHAR* pLabel, CK_ULONG labelLen)
{
	BEID_DATA_LABELS_NAME ID_RECORD_LABELS[] = BEID_ID_RECORD_DATA_LABELS;
	BEID_DATA_LABELS_NAME ADDRESS_RECORD_LABELS[] = BEID_ADDRESS_RECORD_DATA_LABELS;
	int nrOfItems = 0;
	CK_BYTE counter = 0;
	int ulDataType = 0;
	CK_RV ret = 0;

	//retrieve the record number and file (id, address) it resides in
	nrOfItems = sizeof(ID_RECORD_LABELS) / sizeof(BEID_DATA_LABELS_NAME);
	for (counter = 0; counter < nrOfItems; counter++)
	{
		if (strlen(ID_RECORD_LABELS[counter].name) == labelLen)
		{
			if (memcmp(ID_RECORD_LABELS[counter].name, pLabel, labelLen) == 0) {
				ulDataType = CACHED_DATA_TYPE_ID;
				//tags also start at 0, increment by 1 and are 1 BYTE long
				ret = cal_read_and_store_record(pSlot, ulDataType, ID_RECORD_LABELS[counter].tag, pLabel, labelLen);
				goto cleanup;
			}
		}
	}
	counter = 0;
	nrOfItems = sizeof(ADDRESS_RECORD_LABELS) / sizeof(BEID_DATA_LABELS_NAME);
	for (counter = 0; counter < nrOfItems; counter++)
	{
		if (strlen(ADDRESS_RECORD_LABELS[counter].name) == labelLen)
		{
			if (memcmp(ADDRESS_RECORD_LABELS[counter].name, pLabel, labelLen) == 0) {
				ulDataType = CACHED_DATA_TYPE_ADDRESS;
				//tags also start at 0, increment by 1 and are 1 BYTE long
				ret = cal_read_and_store_record(pSlot, ulDataType, ADDRESS_RECORD_LABELS[counter].tag, pLabel, labelLen);
				goto cleanup;
			}
		}
	}

cleanup:
	log_trace(WHERE, "I: leave");
	p11_unlock();
	return ret;
}
#undef WHERE
