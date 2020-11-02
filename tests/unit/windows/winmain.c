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
#include "testlib.h"
#include "logtest.h"


_TCHAR* eid_robot_style = NULL;
_TCHAR*	eid_dialogs_style = NULL;
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
	clock_t startTime = clock();
	clock_t duration = 0;
	int msecDuration = 0;

	int i = 0;
	int nrofTests = 0;
	errno_t error;

	eIDTest eIDTests[] = {
		{0,true, "eject cardreaders before using them", &eject},
		{0,true, "initialize_finalize",&init_finalize},
		{0,true, "getinfo",&getinfo},
		{0,true, "slotlist",&slotlist},
		{0,true, "slotinfo",&slotinfo},
		{0,true, "tkinfo",&tkinfo},
		{0,true, "double_init",&double_init},
		{0,true, "slotevent",&slotevent},
		{0,true, "mechlist",&mechlist},
		{0,true, "mechinfo",&mechinfo},
		{0,true, "sessions",&sessions},
		{0,true, "sessions_nocard",&sessions_nocard},
		{0,true, "sessioninfo",&sessioninfo},
		{0,true, "slogin",&slogin},
		{0,true, "sbadlogin",&sbadlogin},
		{0,true, "nonsensible",&nonsensible},
		{0,true, "objects",&objects},
		{0,true, "readdata",&readdata},
		{0,true, "readdata_limited", &readdata_limited},
		{0,true, "readdata_sequence", &readdata_sequence},
		{0,true, "digest",&digest},
		{0,true, "sign", &sign},
		{0,true, "sign_state",&sign_state},
		{0,true, "decode_photo",&decode_photo},
		{0,true, "ordering",&ordering},
		{0,true, "wrong_init", &wrong_init},
		{0,true, "login_state", &login_state}
	};
	nrofTests = (sizeof(eIDTests) / sizeof(eIDTest));

	if(argc >=2)
		eid_robot_style = argv[1];

	if(argc >=3)
		eid_dialogs_style = argv[2];

	if(argc >=4)
		eid_builtin_reader = argv[3];

	if(argc >= 5) {
		for (i = 0; i < nrofTests; i++) {
			eIDTests[i].doTest = false;
			for (int j = 4; j < argc; j++) {
				if (strcmp(argv[j], eIDTests[i].testDescription) == 0) {
					eIDTests[i].doTest = true;
					j = argc;
				}
			}
		}
	}

	error = initLog();
	if(error != 0)
	{
		printf("Can't open logfile");
		return -1;
	}
	for(i=0; i < nrofTests; i++)
	{
		if (!eIDTests[i].doTest)
			continue;
		printf("test %d: %s\n", i, eIDTests[i].testDescription);
		eIDTests[i].result = eIDTests[i].test_function_ptr();
	}

	int success = 0;
	int failed = 0;
	int skipped = 0;

	//testlog(LVL_NOLEVEL,"\n\n_______________________________________________\n");
	for (i = 0; i < nrofTests; i++)
	{
		if(!eIDTests[i].doTest)
			continue;
		if (eIDTests[i].testDescription != NULL)
		{
			testlog(LVL_NOLEVEL,"_______________________________________________\n");
			testlog(LVL_NOLEVEL,"Test %d %s \n", i, eIDTests[i].testDescription);
		}
		switch(eIDTests[i].result) {
			case TEST_RV_OK:
				success++;
				testlog(LVL_NOLEVEL,"OK\n");
				break;
			case TEST_RV_SKIP:
				skipped++;
				testlog(LVL_NOLEVEL, "SKIPPED\n");
				break;
			case TEST_RV_FAIL:
				failed++;
				testlog(LVL_NOLEVEL, "FAILED\n");
				break;
		}
		/*
		else
		{
			switch(eIDTests[i].result.basetestrv)
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
				testlog(LVL_NOLEVEL,"FAILED : Result = 0x%.8x \n", eIDTests[i].result.pkcs11rv);
				break;
			default:
				testlog(LVL_NOLEVEL,"UNKNOWN : Result = 0x%.8x \n", eIDTests[i].result.pkcs11rv);
				break;
			};
		}*/
		//testlog(LVL_NOLEVEL,"\n_______________________________________________\n");
	}

	printf("Totals: successful %d, skipped %d, failed %d\n", success, skipped, failed);
	duration = clock() - startTime;

	msecDuration = (duration * 1000) / CLOCKS_PER_SEC;
	printf("Duration: %d,%d seconds", msecDuration/1000, msecDuration%1000);

	testlog(LVL_NOLEVEL,"\n===============================================\n");

	//short summary
	/*for (i = 0; i < nrofTests; i++)
	{
		if(eIDTests[i].result.pkcs11rv != CKR_OK)
		{
			testlog(LVL_NOLEVEL," F ");
		}
		else
		{
			switch(eIDTests[i].result.basetestrv)
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
			case TEST_FAILED:
				testlog(LVL_NOLEVEL," F ");
				break;
			default:
				testlog(LVL_NOLEVEL," X ");
				break;
			};
		}
	}*/
	testlog(LVL_NOLEVEL,"\n===============================================\n");

	endLog();

	return failed;
}
