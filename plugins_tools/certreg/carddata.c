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
#include "stdafx.h"
#include "certreg.h"


CK_RV WaitForCardEvent(HWND hTextEdit, CK_FUNCTION_LIST_PTR functions, DWORD *pAutoFlags)
{
	CK_RV retVal = CKR_OK; 
	CK_FLAGS flags = 0;
	CK_SLOT_ID slotID;
	CK_SLOT_INFO slotInfo;
	CK_TOKEN_INFO tokenInfo;
	CK_ULONG ulPreviousCount = 0;
	CK_ULONG ulCount = 0;
	CK_ULONG ulCounter = 0;
	CK_SLOT_ID_PTR pSlotList = NULL;
	CK_ULONG_PTR pCardPresentList = NULL;
	//CK_BYTE_PTR pserialNumberList;
	//CK_ULONG_PTR pserialNumberLenList;
								//CK_ULONG certContextLen = 5;
	PCCERT_CONTEXT** pCertContextArray = NULL;

	retVal = functions->C_GetSlotList(FALSE, NULL_PTR, &ulCount);
	ulPreviousCount = ulCount;

	//pserialNumberList = (CK_BYTE*)malloc(ulCount*sizeof(CK_BYTE));
	//memset(pserialNumberList,0,ulCount*sizeof(CK_BYTE));
	//pserialNumberLenList = (CK_ULONG*)malloc(ulCount*sizeof(CK_ULONG));
	//allocate space for the array of certificate pointers per slot


	retVal = functions->C_GetSlotList(FALSE, NULL_PTR, &ulCount);

	if ((retVal == CKR_OK) && (ulCount > 0)) {
		SendMessage(hTextEdit, EM_REPLACESEL, 0, (LPARAM)"Readers found: \r\n");
		//we retrieved the number of readers, now allocate memory for them
		pSlotList = (CK_SLOT_ID_PTR) malloc(ulCount*sizeof(CK_SLOT_ID));
		pCardPresentList = (CK_ULONG_PTR)malloc(ulCount*sizeof(CK_SLOT_ID));
		pCertContextArray = (PCCERT_CONTEXT** ) calloc (ulCount, sizeof(PCCERT_CONTEXT*) );

		retVal = functions->C_GetSlotList(FALSE, pSlotList, &ulCount);
		if ((retVal == CKR_OK) && (pSlotList != NULL) && (pCardPresentList != NULL) &&(pCertContextArray != NULL) && (ulCount > 0)) {


			//ulCount readers found, now fill in the certificate context array,
			//and register all certificates found
			while(ulCounter < ulCount)
			{
				/* Get slot information*/
				retVal = functions->C_GetSlotInfo(pSlotList[ulCounter], &slotInfo);
				if(retVal != CKR_OK){}
				else
				{
					slotInfo.slotDescription[63] = 0;
					//do not display the PnP slot
					if(strncmp((const char*)slotInfo.slotDescription,"\\\\?PnP?\\Notification",20) != 0)
					{
						SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"- ");
						SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)slotInfo.slotDescription);
						//SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"\r\n");

						/* Get token information*/
						retVal = functions->C_GetTokenInfo(pSlotList[ulCounter], &tokenInfo);
						if (retVal == CKR_TOKEN_NOT_PRESENT) {
							pCardPresentList[ulCounter] = 0;
							SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"  No Card Found \r\n");
						}
						else if (retVal==CKR_OK)
						{
							pCardPresentList[ulCounter] = 1;
							tokenInfo.label[31]=0;
							SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"  Card Found: ");
							SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)tokenInfo.label);
							SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"\r\n");
							//allocate space for 5 certificate context pointers
							pCertContextArray[ulCounter] = (PCCERT_CONTEXT*) calloc (5,sizeof(PCCERT_CONTEXT));
							if(pCertContextArray[ulCounter] != NULL)
							{
								if(*pAutoFlags & AUTO_REGISTER)
									retVal = HandleNewCardFound(hTextEdit, functions, ulCounter, pSlotList,pCertContextArray[ulCounter], 5);						
							}
							else
							{
								SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"ERROR:  Out of memory\r\n");
							}
						}
					}
				}
				ulCounter++;
			}//end of while
		}
	}

	//as long as the readercount didn't change; keep the current slotlist
	//TODO: match the entire slotList, not just checking its size
	while(ulCount == ulPreviousCount)
	{
		/* Block and wait for a slot event */
		retVal = functions->C_WaitForSlotEvent(flags, &slotID, NULL_PTR);
		if(retVal != CKR_OK)
		{
			SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"WARNING: C_WaitForSlotEvent returned an error \r\n");
			return retVal;

			//	printError()
			//		char errormessage[100];
			//		_snprintf(errormessage,100,"C_GetAttributeValue returned 0x%0.8x\r\n",retval);
			//		SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"\r\n");
		}
		/* Check if we already know that slot */
		retVal = functions->C_GetSlotInfo(slotID, &slotInfo);
		if(retVal != CKR_OK){}

		ulCounter = 0;
		while(ulCounter < ulCount)
		{
			if(pSlotList[ulCounter] == slotID)
			{
				// Get token information
				retVal = functions->C_GetTokenInfo(slotID, &tokenInfo);
				if( (retVal == CKR_TOKEN_NOT_PRESENT) && (pCardPresentList[ulCounter] == 1) )
				{
					SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"Card removed\r\n");
					pCardPresentList[ulCounter] = 0;
					//token removed, so remove its certificates
					if(*pAutoFlags & AUTO_REMOVE)
						retVal = HandleCardRemoved(hTextEdit, functions, pCertContextArray[ulCounter], 5);
					//free the allocated space of the certificate context pointers
					if(pCertContextArray[ulCounter] != NULL)
					{
						free (pCertContextArray[ulCounter]);
						pCertContextArray[ulCounter] = NULL;
					}
				}
				else
				{
					if(pCardPresentList[ulCounter] == 0)
					{
						SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"Card inserted\r\n");
						pCardPresentList[ulCounter] = 1;
						//allocate space for 5 certificate context pointers
						pCertContextArray[ulCounter] = (PCCERT_CONTEXT*)calloc (5,sizeof(PCCERT_CONTEXT));
						if(pCertContextArray[ulCounter] != NULL)
						{
							//token added, so add its certificates
							if(*pAutoFlags & AUTO_REGISTER)
								retVal = HandleNewCardFound(hTextEdit, functions, ulCounter, pSlotList,
								pCertContextArray[ulCounter], 5);
						}
						else
						{
							SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"ERROR:  Out of memory\r\n");
						}
					}					
				}			
				break;
			}
			ulCounter++;
		}

		if(ulCounter == ulCount)
		{
			//a new reader is detected
			SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"New reader detected \r\n");
			// Get token information
			//				retVal = functions->C_GetTokenInfo(slotID, &tokenInfo);
			//				if (retVal == CKR_TOKEN_NOT_PRESENT) 
			//				{
			//					SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"  No Card Found \r\n");
			//				}
			//				else
			//				{
			//					SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"Card Found: \r\n");
			//				}
		}
		retVal = functions->C_GetSlotList(FALSE, NULL_PTR, &ulCount);
	}// end of while
	if(pSlotList != NULL)
		free(pSlotList);
	if(pCardPresentList != NULL)
		free(pCardPresentList);

	if(pCertContextArray != NULL)
	{
		for(ulCounter = 0;ulCounter < ulPreviousCount;ulCounter++ )
		{
			if(pCertContextArray[ulCounter] != NULL)
			{
				free(pCertContextArray[ulCounter]);
				pCertContextArray[ulCounter] = NULL;
			}
		}
		free(pCertContextArray);
	}
	return retVal;
}

CK_RV HandleNewCardFound(HWND hTextEdit, CK_FUNCTION_LIST_PTR functions,
									CK_ULONG ulCounter, CK_SLOT_ID_PTR pSlotList,
									PCCERT_CONTEXT*	ppCertContext, CK_ULONG certContextLen)
{
	CK_RV retVal = CKR_OK; 
	CK_SESSION_HANDLE session_handle;

	retVal = (functions->C_OpenSession)(pSlotList[ulCounter], CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session_handle);
	if (retVal == CKR_OK)
	{
		CK_ULONG ulserialNumberLen = 20;
		CK_BYTE_PTR serialNumber = (CK_BYTE_PTR)malloc (ulserialNumberLen*sizeof(CK_BYTE));

		if(serialNumber != NULL)
		{
			//get serial number of the card
			retVal =  GetSerialNumber(hTextEdit, functions, &session_handle, serialNumber,&ulserialNumberLen);
			if (retVal == CKR_OK)
			{	
				//register certificates of the card		
				retVal = GetAndRegisterCertificates(hTextEdit, functions, &session_handle, serialNumber, ulserialNumberLen, ppCertContext, certContextLen);
			}
		}
		retVal = (functions->C_CloseSession) (session_handle);			
	}
	return retVal;
}

CK_RV HandleCardRemoved(HWND hTextEdit, CK_FUNCTION_LIST_PTR functions,
									PCCERT_CONTEXT*	ppCertContext, CK_ULONG ulcertContextLen)
{
	//CK_RV retVal;
	CK_ULONG ulCounter = 0;

	if(ppCertContext != NULL)
	{
		for(;ulCounter<ulcertContextLen;ulCounter++)
		{
			if(ppCertContext[ulCounter] != NULL)
			{
				//this function also frees the ppCertContext[ulCounter] if the certificate is deleted
				DeleteIfUserCert(hTextEdit,ppCertContext[ulCounter]);
			}
		}
	}

	return CKR_OK;
}





CK_RV GetSerialNumber(HWND hTextEdit, CK_FUNCTION_LIST_PTR functions, CK_SESSION_HANDLE *psession_handle,
											CK_BYTE* pbserialNumber,CK_ULONG *pulserialNumberLen)
{
	CK_RV retval = CKR_OK; 
	CK_ULONG ulObjectCount=1;

	CK_ULONG ulData = CKO_DATA;
	CK_ATTRIBUTE serialNrAttr[2] = {{CKA_CLASS,&ulData,sizeof(CK_ULONG)},{CKA_LABEL,"carddata_serialnumber", 21}};

	CK_ATTRIBUTE attr_value_templ = {CKA_VALUE,(CK_VOID_PTR)pbserialNumber,*pulserialNumberLen};
	CK_OBJECT_HANDLE hKey;

	retval = (functions->C_FindObjectsInit)(*psession_handle, serialNrAttr, 2); 
	if (retval == CKR_OK)
	{
		retval = (functions->C_FindObjects)(*psession_handle, &hKey,1,&ulObjectCount); 
		if (ulObjectCount > 0)
		{
			//get the serial number value
			attr_value_templ.ulValueLen = *pulserialNumberLen;
			retval = (functions->C_GetAttributeValue)(*psession_handle,hKey,&attr_value_templ,1);
			if (retval == CKR_OK)
			{
				*pulserialNumberLen = attr_value_templ.ulValueLen;
			}
			else
			{
				//failed getting the serial number's value
				*pulserialNumberLen = 0;
			}
		}
		retval = (functions->C_FindObjectsFinal)(*psession_handle); 
		//ReturnedSuccesfull(retval,&trv, "C_FindObjectsFinal", "test_getprivatekeys" );
	}
	return retval;
} 


CK_RV GetAndRegisterCertificates(HWND hTextEdit, CK_FUNCTION_LIST_PTR functions, CK_SESSION_HANDLE *psession_handle,
													 CK_BYTE* pbserialNumber,CK_ULONG ulserialNumberLen,
													 PCCERT_CONTEXT*	ppCertContext, CK_ULONG ulcertContextLen) 
{
	CK_RV retval = CKR_OK; 
	CK_ULONG ulObjectCount=1;

	CK_ULONG ulCertificate = CKO_CERTIFICATE;
	CK_ATTRIBUTE attributes[1] = {CKA_CLASS,&ulCertificate,sizeof(CK_ULONG)};

	CK_BYTE label[100];
	CK_ULONG ullabelLen = 100;
	CK_BYTE value[10240];
	CK_ULONG ulvalueLen = 10239;
	CK_BYTE_PTR pbcertificateData = NULL;
	CK_ULONG dwcertificateDataLen = 0;

	CK_ATTRIBUTE attr_label_templ = {CKA_LABEL,(CK_VOID_PTR)label,ullabelLen};
	CK_ATTRIBUTE attr_value_templ = {CKA_VALUE,(CK_VOID_PTR)value,ulvalueLen};
	CK_OBJECT_HANDLE hKey;


	retval = (functions->C_FindObjectsInit)(*psession_handle, attributes, 1); 
	if (retval == CKR_OK)
	{
		CK_ULONG certCount = 0;
		while (ulObjectCount > 0)
		{
			retval = (functions->C_FindObjects)(*psession_handle, &hKey,1,&ulObjectCount); 
			if (ulObjectCount > 0)
			{
				attr_label_templ.ulValueLen = 100;
				retval = (functions->C_GetAttributeValue)(*psession_handle,hKey,&attr_label_templ,1);
				if (retval == CKR_OK)
				{
					pbcertificateData = (CK_BYTE_PTR)attr_label_templ.pValue;										
					pbcertificateData[attr_label_templ.ulValueLen] = 0;
					SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"certificate found: ");
					SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)pbcertificateData);
				}
				attr_value_templ.ulValueLen = 10239;
				retval = (functions->C_GetAttributeValue)(*psession_handle,hKey,&attr_value_templ,1);
				if ((retval == CKR_OK) && (ulcertContextLen > certCount) )
				{
					pbcertificateData = (CK_BYTE_PTR)attr_value_templ.pValue;
					dwcertificateDataLen = attr_value_templ.ulValueLen; 

					if( ImportCertificate(pbserialNumber,ulserialNumberLen,pbcertificateData,dwcertificateDataLen, &(ppCertContext[certCount])) == TRUE)
					{
						certCount++;

						SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"   - registered\r\n");
					}
					else
					{
						SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"   - registration failed\r\n");
					}
					//testlog(LVL_INFO,"key LABEL value = %s\n", pbcertificateData);
				}
				else
				{
					char errormessage[100];
					_snprintf(errormessage,100,"C_GetAttributeValue returned 0x%0.8x\r\n",retval);
					SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"\r\n");
					SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"ERROR: failed getting attribute value \r\n");
					SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)errormessage);
				}
			}
		}//end of while
		retval = (functions->C_FindObjectsFinal)(*psession_handle); 
	}
	return retval;
} 


CK_RV RemoveCertificates(HWND hTextEdit, CK_FUNCTION_LIST_PTR functions, CK_SESSION_HANDLE *psession_handle,
													 CK_BYTE* pbserialNumber,CK_ULONG ulserialNumberLen) 
{
	CK_RV retval = CKR_OK; 
	/*
	CK_ULONG ulObjectCount=1;

	CK_ULONG ulCertificate = CKO_CERTIFICATE;
	CK_ATTRIBUTE attributes[1] = {CKA_CLASS,&ulCertificate,sizeof(CK_ULONG)};

	CK_BYTE label[100];
	CK_ULONG ullabelLen = 100;
	CK_BYTE value[10240];
	CK_ULONG ulvalueLen = 10239;
	CK_BYTE_PTR pbcertificateData = NULL;
	CK_ULONG dwcertificateDataLen = 0;

	CK_ATTRIBUTE attr_label_templ = {CKA_LABEL,(CK_VOID_PTR)label,ullabelLen};
	CK_ATTRIBUTE attr_value_templ = {CKA_VALUE,(CK_VOID_PTR)value,ulvalueLen};
	CK_OBJECT_HANDLE hKey;


	retval = (functions->C_FindObjectsInit)(*psession_handle, attributes, 1); 
	if (retval == CKR_OK)
	{
		while (ulObjectCount > 0)
		{
			retval = (functions->C_FindObjects)(*psession_handle, &hKey,1,&ulObjectCount); 
			if (ulObjectCount > 0)
			{
				attr_label_templ.ulValueLen = 100;
				retval = (functions->C_GetAttributeValue)(*psession_handle,hKey,&attr_label_templ,1);
				if (retval == CKR_OK)
				{
					pbcertificateData = attr_label_templ.pValue;										
					pbcertificateData[attr_label_templ.ulValueLen] = 0;
					SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"certificate found: ");
					SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)pbcertificateData);
				}
				attr_value_templ.ulValueLen = 10239;
				retval = (functions->C_GetAttributeValue)(*psession_handle,hKey,&attr_value_templ,1);
				if (retval == CKR_OK)
				{
					pbcertificateData = attr_value_templ.pValue;
					dwcertificateDataLen = attr_value_templ.ulValueLen; 
					if( ImportCertificates(pbserialNumber,ulserialNumberLen,pbcertificateData,dwcertificateDataLen) == TRUE)
					{
						SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"   - registered\r\n");
					}
					else
					{
						SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"   - registration failed\r\n");
					}
					//testlog(LVL_INFO,"key LABEL value = %s\n", pbcertificateData);
				}
				else
				{
					char errormessage[100];
					_snprintf(errormessage,100,"C_GetAttributeValue returned 0x%0.8x\r\n",retval);
					SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"\r\n");
					SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"ERROR: failed getting attribute value \r\n");
					SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)errormessage);
				}
			}
		}//end of while
		retval = (functions->C_FindObjectsFinal)(*psession_handle); 
	}*/
	return retval;
} 



CK_RV getcertificates(HWND hTextEdit, CK_FUNCTION_LIST_PTR functions) {

	//	CK_FUNCTION_LIST_PTR functions;
	CK_RV retval = CKR_OK; 
	CK_SESSION_HANDLE session_handle;
	CK_ULONG slot_count = 0;
	CK_SLOT_ID_PTR slotIds;
	CK_ULONG slotIdx;
	CK_ULONG ulObjectCount=1;

	//	retval = (functions->C_Initialize) (NULL);
	//	if (retval == CKR_OK)
	//	{		
	retval = (functions->C_GetSlotList) (CK_TRUE, 0, &slot_count);
	if (retval == CKR_OK)
	{
		if(slot_count == 0)
		{
			SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"No eID Card found:\r\n");
		}
		else
		{
			SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"eID card found \r\n");
			SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"Retrieving certificates.. \r\n");
		}
		slotIds = (CK_SLOT_ID_PTR)malloc(slot_count * sizeof(CK_SLOT_INFO));
		//check failure

		retval = (functions->C_GetSlotList) (CK_TRUE, slotIds, &slot_count);
		if (retval == CKR_OK)
		{
			for (slotIdx = 0; slotIdx < slot_count; slotIdx++) 
			{
				CK_SLOT_ID slotId = slotIds[slotIdx];
				CK_ULONG ulData = CKO_DATA;
				CK_ATTRIBUTE serialNrAttr[2] = {{CKA_CLASS,&ulData,sizeof(CK_ULONG)},{CKA_LABEL,"carddata_serialnumber", 21}};

				CK_ULONG ulCertificate = CKO_CERTIFICATE;
				CK_ATTRIBUTE attributes[1] = {CKA_CLASS,&ulCertificate,sizeof(CK_ULONG)};

				CK_BYTE label[100];
				CK_ULONG ullabelLen = 100;

				CK_BYTE value[10240];
				CK_ULONG ulvalueLen = 10239;
				CK_BYTE_PTR pbserialNumber = NULL;
				CK_BYTE_PTR pbcertificateData = NULL;
				CK_ULONG dwserialNumberLen = 0;
				CK_ULONG dwcertificateDataLen = 0;

				CK_ATTRIBUTE attr_label_templ = {CKA_LABEL,(CK_VOID_PTR)label,ullabelLen};
				CK_ATTRIBUTE attr_value_templ = {CKA_VALUE,(CK_VOID_PTR)value,ulvalueLen};
				CK_OBJECT_HANDLE hKey;

				retval = (functions->C_OpenSession)(slotId, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session_handle);
				if (retval == CKR_OK)
				{
					retval = (functions->C_FindObjectsInit)(session_handle, serialNrAttr, 2); 
					if (retval == CKR_OK)
					{
						//enable search
						ulObjectCount = 1;
						while (ulObjectCount > 0)
						{
							retval = (functions->C_FindObjects)(session_handle, &hKey,1,&ulObjectCount); 
							//ReturnedSuccesfull(retval,&trv, "C_FindObjects", "test_getprivatekeys" );
							if (ulObjectCount > 0)
							{
								attr_value_templ.ulValueLen = 10239;
								retval = (functions->C_GetAttributeValue)(session_handle,hKey,&attr_value_templ,1);
								if (retval == CKR_OK)
								{
									pbserialNumber = (CK_BYTE_PTR)malloc (attr_value_templ.ulValueLen + 1);
									if (pbserialNumber != NULL)
									{
										memcpy(pbserialNumber, attr_value_templ.pValue, attr_value_templ.ulValueLen);
										pbserialNumber[attr_value_templ.ulValueLen] = 0;
										dwserialNumberLen = attr_value_templ.ulValueLen;
										SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"card serial number read\r\n");
									}
									else
									{
										SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"ERROR: malloc failed\r\n");
									}
									//testlog(LVL_INFO,"key LABEL value = %s\n", pbcertificateData);
								}
							}
						}
						retval = (functions->C_FindObjectsFinal)(session_handle); 
						//ReturnedSuccesfull(retval,&trv, "C_FindObjectsFinal", "test_getprivatekeys" );
					}

					//in order to start searching
					ulObjectCount = 1;

					retval = (functions->C_FindObjectsInit)(session_handle, attributes, 1); 
					if (retval == CKR_OK)
					{
						while (ulObjectCount > 0)
						{
							retval = (functions->C_FindObjects)(session_handle, &hKey,1,&ulObjectCount); 
							//ReturnedSuccesfull(retval,&trv, "C_FindObjects", "test_getprivatekeys" );
							if (ulObjectCount > 0)
							{
								attr_label_templ.ulValueLen = 100;
								retval = (functions->C_GetAttributeValue)(session_handle,hKey,&attr_label_templ,1);
								if (retval == CKR_OK)
								{
									pbcertificateData = (CK_BYTE_PTR)attr_label_templ.pValue;										
									pbcertificateData[attr_label_templ.ulValueLen] = 0;
									SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"certificate found: ");
									SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)pbcertificateData);
								}
								attr_value_templ.ulValueLen = 10239;
								retval = (functions->C_GetAttributeValue)(session_handle,hKey,&attr_value_templ,1);
								if (retval == CKR_OK)
								{
									PCCERT_CONTEXT	pCertContext = NULL;
									pbcertificateData = (CK_BYTE_PTR)attr_value_templ.pValue;
									dwcertificateDataLen = attr_value_templ.ulValueLen; 
									
									if( ImportCertificate(pbserialNumber,dwserialNumberLen,pbcertificateData,dwcertificateDataLen, &pCertContext) == TRUE)
									{
										SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"   - registered\r\n");
									}
									else
									{
										SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"   - registration failed\r\n");
									}
									if(pCertContext != NULL)
										CertFreeCertificateContext(pCertContext);
									//testlog(LVL_INFO,"key LABEL value = %s\n", pbcertificateData);
								}
								else
								{
									char errormessage[100];
									_snprintf(errormessage,100,"C_GetAttributeValue returned 0x%0.8x\r\n",retval);
									SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"\r\n");
									SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"ERROR: failed getting attribute value \r\n");
									SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)errormessage);
								}
							}
						}
						retval = (functions->C_FindObjectsFinal)(session_handle); 
						//ReturnedSuccesfull(retval,&trv, "C_FindObjectsFinal", "test_getprivatekeys" );
					}
				}
				retval = (functions->C_CloseSession) (session_handle);
				//ReturnedSuccesfull(retval,&trv, "C_CloseSession", "test_getprivatekeys" );

				if (pbserialNumber != NULL)
					free (pbserialNumber);
			}
		}
	}
	//		retval = (functions->C_Finalize) (NULL_PTR);
	//ReturnedSuccesfull(retval,&trv, "C_Finalize", "test_getprivatekeys" );
	//	}
	return retval;
} 



