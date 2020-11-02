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

#include "win32.h"
#pragma pack(push, cryptoki, 1)
#include "pkcs11.h"
#pragma pack(pop, cryptoki)

#include <time.h>

#include "stdafx.h"
#include <stdio.h>
#include "../testlib2.h"
#include "logtest.h"
#include "gmain.h"


_TCHAR* eid_robot_style = NULL;
_TCHAR* eid_dialogs_style = NULL;
_TCHAR* eid_builtin_reader = NULL;

/*
 * Main function
 */
//#ifdef WIN32
//int _tmain(int argc, _TCHAR* argv[]){
typedef	struct {
	int result;
	bool doTest;
	char* testDescription;
	int (*test_function_ptr)(void);
} eIDTest;

//int main()
//{
int	_tmain(int argc, _TCHAR* argv[]){

	eIDTest eIDTests[] = {
		{0,true, "states", &states }
	};
	
	return gmain(argc, argv, eIDTests);
}
