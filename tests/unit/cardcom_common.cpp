/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2014 FedICT.
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
#ifdef WIN32
#include <win32.h>
#pragma pack(push, cryptoki, 1)
#include "pkcs11.h"
#pragma pack(pop, cryptoki)
#else
#include <unix.h>
#include <pkcs11.h>
#endif
#include <stdio.h>
#include "testlib.h"
#include <string>
#include <common/mw_util.h>
#include <typeinfo>
#include <iostream>

TEST_FUNC(scardcom_common){
	std::string tstring = std::string("teststring");
	std::wstring wchecker;
	if (typeid(wstring_From_string(tstring)) !=  typeid(wchecker)){ 
		std::cout << typeid(wstring_From_string(tstring)).name() << std::endl;
		return TEST_RV_FAIL;
	}	

	std::wstring testw = wstring_From_string("teststring");	
	std::string schecker;
	if (typeid(string_From_wstring(testw)) != typeid(schecker)){ 
		std::cout << typeid(string_From_wstring(testw)).name() << std::endl;
		return TEST_RV_FAIL;
	}

	FILE * tfile = fopen("testfile.txt","w+");
	FILE ** fPointer = new FILE *;
	*fPointer = tfile;
	std::wstring wmode = wstring_From_string("a");	
        const wchar_t * mode = wmode.c_str();
	std::wstring wFilename = wstring_From_string(std::string ("testfile.txt"));
	const wchar_t * filename = wFilename.c_str();
	if (_wfopen_s(fPointer, filename, mode) > 0 ){
		printf("could not open file");
		return TEST_RV_FAIL;
	}
	fclose(tfile);
	remove("testfile.txt");
	return TEST_RV_OK;
 }
	
