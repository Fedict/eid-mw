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

extern HANDLE                  ghSvcStopEvent;
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

#define dlopen(lib,h) LoadLibrary(lib)
#define dlsym(h, function) GetProcAddress(h, function)
#define dlclose(h) FreeLibrary(h)
#ifdef WIN64
#define PKCS11_LIB L"..\\_Binaries35\\x64\\Debug\\beid35pkcs11D.dll"
#else
#define PKCS11_LIB L"beidpkcs11.dll"
#endif
#define RTLD_LAZY	1
#define RTLD_NOW	2
#define RTLD_GLOBAL 4

#else
#include <opensc/pkcs11.h>
#include <dlfcn.h>
#include <unistd.h>
#define PKCS11_LIB "/usr/local/lib/libbeidpkcs11.so" 
#endif
#include <stdlib.h>


DWORD CertProp();
CK_RV getcertificates(HWND hTextEdit,CK_FUNCTION_LIST_PTR functions);
CK_RV WaitForCardEvent(HWND hTextEdit, CK_FUNCTION_LIST_PTR functions);
BOOL ImportCertificates(BYTE* pbserialNumber,DWORD serialNumberLen,BYTE* pbcertificateData,DWORD dwcertificateDataLen);
BOOL StoreUserCerts (PCCERT_CONTEXT pCertContext, unsigned char KeyUsageBits, BYTE* pbserialNumber,DWORD dwserialNumberLen);
BOOL StoreAuthorityCerts(PCCERT_CONTEXT pCertContext, unsigned char KeyUsageBits);
BOOL ProviderNameCorrect (PCCERT_CONTEXT pCertContext );

#endif /* CERTPROP_H_ */
