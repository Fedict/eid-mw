
/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2009-2010 FedICT.
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

#ifndef CERTREG_H_
#define CERTREG_H_
#include <windows.h>
extern HANDLE ghSvcStopEvent;


#include <stdio.h>
#ifdef WIN32
#include <wincrypt.h>
	
//allign at 1 byte
#pragma pack(push, cryptoki, 1)
#include <win32.h>
#include <pkcs11.h>
#pragma pack(pop, cryptoki)
//back to default allignment
	
#include <windows.h>
#include <conio.h>
#include <tchar.h>
#include <strsafe.h>
	
#define AUTO_REGISTER	0x01
#define AUTO_REMOVE		0x02
	
#define dlopen(lib,h) LoadLibrary(lib)
#define dlsym(h, function) GetProcAddress(h, function)
#define dlclose(h) FreeLibrary(h)
#ifdef WIN64
#define PKCS11_LIB L"..\\_Binaries35\\x64\\Debug\\beid35pkcs11D.dll"
#else /*  */
#define PKCS11_LIB L"beidpkcs11.dll"
#endif /*  */
#define RTLD_LAZY	1
#define RTLD_NOW	2
#define RTLD_GLOBAL 4
	
#else /*  */
#include <opensc/pkcs11.h>
#include <dlfcn.h>
#include <unistd.h>
#define PKCS11_LIB "/usr/local/lib/libbeidpkcs11.so" 
#endif /*  */
#include <stdlib.h>
	
//Function that handles card events in a loop,
//and that exits when a reader event is triggered
	CK_RV HandleCardEvents(HWND hTextEdit,
			       CK_FUNCTION_LIST_PTR functions);
 
//Function called when HandleCardEvents detects a new card
	CK_RV HandleNewCardFound(HWND hTextEdit,
				 CK_FUNCTION_LIST_PTR functions,
				 CK_ULONG ulCounter,
				 CK_SLOT_ID_PTR pSlotList,
				 PCCERT_CONTEXT * pCertContext,
				 CK_ULONG certContextLen);
 
//Get the serial number of the inserted card
	CK_RV GetSerialNumber(HWND hTextEdit, CK_FUNCTION_LIST_PTR functions,
			      CK_SESSION_HANDLE * psession_handle,
			      CK_BYTE * pbserialNumber,
			      CK_ULONG * pulserialNumberLen);
 
//Get the certificates of the inserted card, and register them
	CK_RV GetAndRegisterCertificates(HWND hTextEdit,
					 CK_FUNCTION_LIST_PTR functions,
					 CK_SESSION_HANDLE * psession_handle,
					 CK_BYTE * pbserialNumber,
					 CK_ULONG ulserialNumberLen,
					 PCCERT_CONTEXT * ppCertContext,
					 CK_ULONG ulcertContextLen);
  BOOL ImportCertificate(BYTE * pbserialNumber, DWORD serialNumberLen,
			    BYTE * pbcertificateData,
			    DWORD dwcertificateDataLen,
			    PCCERT_CONTEXT * ppCertContext);
BOOL StoreUserCerts(PCCERT_CONTEXT pCertContext, unsigned char KeyUsageBits,
		     BYTE * pbserialNumber, DWORD dwserialNumberLen);
BOOL StoreAuthorityCerts(PCCERT_CONTEXT pCertContext,
			  unsigned char KeyUsageBits);
BOOL ProviderNameCorrect(PCCERT_CONTEXT pCertContext);
DWORD CertProp();
 
//Function called when HandleCardEvents detects a card is removed
	CK_RV HandleCardRemoved(HWND hTextEdit,
				CK_FUNCTION_LIST_PTR functions,
				PCCERT_CONTEXT * ppCertContext,
				CK_ULONG ulcertContextLen);
 BOOL DeleteIfUserCert(HWND hTextEdit, PCCERT_CONTEXT pCertContext);
 
//function used for manual registration of the certificates
	CK_RV getcertificates(HWND hTextEdit, CK_FUNCTION_LIST_PTR functions);
  
#endif	/* CERTPROP_H_ */
