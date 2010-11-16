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

/*
* Integration test for the PKCS#11 library.
* Required interaction: PIN entry.
*/

#include <stdio.h>
#include <MainTest.h>
#include "logtest.h"

#define NUMBER_OF_TESTS 100
//#define USER_INTERACTION

int main() {
	int i=0;
	int testCounter = 0;
	char* testDescription[NUMBER_OF_TESTS];
	testRet result[NUMBER_OF_TESTS];

	initLog();

	while (i<NUMBER_OF_TESTS)
	{
		testDescription[i] = NULL;
		i++;
	}
	testDescription[testCounter] = "Test multiple finalize/initialize sessions in multiple threads with different args";
	result[testCounter] = test_finalize_initialize();
	testCounter++;
	testDescription[testCounter] = "Test multiple finalize/initialize sessions in a single thread";
	result[testCounter] = test_finalize_initialize_st();
	testCounter++;
	testDescription[testCounter] = "Test initialize when bad parameters are supplied";
	result[testCounter] = test_initialize_ownmutex();
	testCounter++;
	result[testCounter] = test_initialize_preserved();
	testCounter++;
	result[testCounter] = test_finalize_preserved();
	testCounter++;
	testDescription[testCounter] = "Test C_getinfo results in a single thread";
	result[testCounter] = test_getinfo();
	testCounter++;
	testDescription[testCounter] = "Test C_getslotlist in single and two call usage";
	result[testCounter] = test_getslotlist();
	testCounter++;
	testDescription[testCounter] = "Test C_getslotlist when insufficient memory is reserved";
	result[testCounter] = test_getslotlist_multiple_slots();
	testCounter++;
	testDescription[testCounter] = "Tests opening and closing of a session in a single thread";
	result[testCounter] = test_open_close_session();
	testCounter++;
	result[testCounter] = test_open_close_session_info();
	testCounter++;
	result[testCounter] = test_open_close_session_bad_params();
	testCounter++;
	result[testCounter] = test_open_close_session_limits();
	testCounter++;
	testDescription[testCounter] = "Tests waiting for slot event";
	result[testCounter] = test_waitforslotevent_noblock();
	testCounter++;
#ifdef USER_INTERACTION
	result[testCounter] = test_waitforslotevent_userinteraction();
	testCounter++;
#endif
	result[testCounter] = test_waitforslotevent_whilefinalize();
	testCounter++;
	testDescription[testCounter] = "Tests the retrieval of slot and token info";
	result[testCounter] = test_get_token_info();
	testCounter++;
	testDescription[testCounter] = "Shows info on the mechanisms supported by the card";
	result[testCounter] = test_getmechanisms();
	testCounter++;
	testDescription[testCounter] = "tests getting the keys from the card";
	result[testCounter] = test_getkeys();
	testCounter++;
	testDescription[testCounter] = "tests the return values of the getattributes function";
	result[testCounter] = test_getkeys_retvals();
	testCounter++;
	testDescription[testCounter] = "tests getting all the objects from the card";
	result[testCounter] = test_getallobjects();
	testCounter++;
	testDescription[testCounter] = "tests getting object's attributes from the card";
	result[testCounter] = test_getattributevalue_all();
	testCounter++;
	testDescription[testCounter] = "tests getting the lastname object's attributes from the card";
	result[testCounter] = test_getattributevalue_lastname();
	testCounter++;]
	testDescription[testCounter] = "tests getting the private object's from the card without logging in";
	result[testCounter] = test_findPrivateKeyWithoutLoginShouldFail();
	testCounter++;

#ifdef USER_INTERACTION
	testDescription[testCounter] = "tests signing with the card";
	result[testCounter] = test_sign();
	testCounter++;
#endif
	
	//testlog(LVL_NOLEVEL,"\n\n_______________________________________________\n");
	for (i = 0; i < testCounter; i++)
	{
		if (testDescription[i] != NULL)
		{
			testlog(LVL_NOLEVEL,"\n_______________________________________________\n");
			testlog(LVL_NOLEVEL,"\nTest %d %s \n", i, testDescription[i]);
		}
		if(result[i].pkcs11rv != CKR_OK)
		{
			testlog(LVL_NOLEVEL,"FAILED : Result = 0x%.8x \n", result[i].pkcs11rv);
		}
		else
		{
			switch(result[i].basetestrv)
			{
			case TEST_PASSED:
				testlog(LVL_NOLEVEL,"PASSED\n");
				break;
			case TEST_SKIPPED:
				testlog(LVL_NOLEVEL,"SKIPPED\n");
				break;
			case TEST_WARNING:
				testlog(LVL_NOLEVEL,"WARNING\n");
				break;
			case TEST_ERROR:
			case TEST_FAILED:
				testlog(LVL_NOLEVEL,"FAILED : Result = 0x%.8x \n", result[i].pkcs11rv);
				break;
			default:
				testlog(LVL_NOLEVEL,"UNKNOWN : Result = 0x%.8x \n", result[i].pkcs11rv);
				break;
			};
		}
	}

	testlog(LVL_NOLEVEL,"\n===============================================\n");
	//short summary
	for (i = 0; i < testCounter; i++)
	{
		if(result[i].pkcs11rv != CKR_OK)
		{
			testlog(LVL_NOLEVEL," F ");
		}
		else
		{
			switch(result[i].basetestrv)
			{
			case TEST_PASSED:
				testlog(LVL_NOLEVEL,"P");
				break;
			case TEST_SKIPPED:
				testlog(LVL_NOLEVEL," S ");
				break;
			case TEST_WARNING:
				testlog(LVL_NOLEVEL," W ");
				break;
			case TEST_ERROR:
				testlog(LVL_NOLEVEL," F ");
				break;
			default:
				testlog(LVL_NOLEVEL," X ");
				break;
			};
		}
	}
	testlog(LVL_NOLEVEL,"\n===============================================\n");

	endLog();
	// Wait for user to end this test
	getchar();
}

