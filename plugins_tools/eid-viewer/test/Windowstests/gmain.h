#pragma once
#include "win32.h"
#pragma pack(push, cryptoki, 1)
#include "pkcs11.h"
#pragma pack(pop, cryptoki)

#include <time.h>

#include "stdafx.h"
#include <stdio.h>
#include "../testlib2.h"
#include "logtest.h"
#include <test\testlib.c>

typedef	struct {
	int result;
	bool doTest;
	char* testDescription;
	int (*test_function_ptr)(void);
} eIDTest;

int gmain(int argc, _TCHAR* argv[] , eIDTest eIDTests[]) {
	clock_t startTime = clock();
	clock_t duration = 0;
	int msecDuration = 0;

	int i = 0;
	int nrofTests = 0;
	errno_t error;
	nrofTests = (sizeof(eIDTests) / sizeof(eIDTest));

	if (argc >= 2)
		eid_robot_style = argv[1];

	if (argc >= 3)
		eid_dialogs_style = argv[2];

	if (argc >= 4)
		eid_builtin_reader = argv[3];

	if (argc >= 5) {
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
	if (error != 0)
	{
		printf("Can't open logfile");
		return -1;
	}
	for (i = 0; i < nrofTests; i++)
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
		if (!eIDTests[i].doTest)
			continue;
		if (eIDTests[i].testDescription != NULL)
		{
			testlog(LVL_NOLEVEL, "_______________________________________________\n");
			testlog(LVL_NOLEVEL, "Test %d %s \n", i, eIDTests[i].testDescription);
		}
		switch (eIDTests[i].result) {
		case TEST_RV_OK:
			success++;
			testlog(LVL_NOLEVEL, "OK\n");
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
	printf("Duration: %d,%d seconds", msecDuration / 1000, msecDuration % 1000);

	testlog(LVL_NOLEVEL, "\n===============================================\n");

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
	testlog(LVL_NOLEVEL, "\n===============================================\n");

	endLog();
	return failed;
}