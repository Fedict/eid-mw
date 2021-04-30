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
#ifndef WIN32
#include <unix.h>
#include <pkcs11.h>
#include <common/mwexception.cpp>
#include <stdio.h>
#include "testlib.h"
#include <string>

#include <typeinfo>
#include <iostream>
TEST_FUNC(exceptiondata){

	try{
		trow CNotAuthenticatedException(0xe1d00404L)
	}	
	catch(CNotAuthenticatedException n){
		printf("Not authenticated error code: %lu\n", n.get_m_lError);
	}
	catch(CMWException mw){
		if (mw.GetError() != 0xe1d00404L)return TEST_RV_FAIL;
		if (mw.GetLine() != 0)return TEST_RV_FAIL;
		if (mw.GetFile() != "") return TEST_RV_FAIL;
		else return TEST_RV_OK;
	}
 }
