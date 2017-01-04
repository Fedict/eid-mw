
/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2011-2012 FedICT.
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

#ifndef basetest_h
#define basetest_h
	
#include <stdio.h>
#include <ctype.h>
#ifdef WIN32
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
//#ifdef _DEBUG
//  #define PKCS11_LIB TEXT("beidpkcs11D.dll")
//#else
#define PKCS11_LIB TEXT("beidpkcs11.dll")
//#endif
	
#define RTLD_LAZY	1
#define RTLD_NOW	2
#define RTLD_GLOBAL 4
	
#else /*  */
#define HMODULE void*
#include "rsaref220/unix.h"
#include "rsaref220/pkcs11.h"
	
#pragma pack(push, cryptoki, 1)
#include "rsaref220/pkcs11.h"
#pragma pack(pop, cryptoki)
	
#include <dlfcn.h>
#include <unistd.h>
#include <string.h>
	
#ifdef __APPLE__
#define PKCS11_LIB "/usr/local/lib/libbeidpkcs11.dylib"
#else /*  */
#define PKCS11_LIB "libbeidpkcs11.so.0"
#endif /*  */
#define TEXT(x) x
#define _getch() getchar()
#endif /*  */
#include <stdlib.h>
	
#endif /*  */
