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
 * Required interaction: none.
 */

#include <stdio.h>
#include <MainTest.h>
#include "logtest.h"

#define NUMBER_OF_TESTS 10

int main() {
	int i;
	int testCounter = 0;
	char* testDescription[NUMBER_OF_TESTS];
	int result[NUMBER_OF_TESTS];

	initLog();

	testDescription[testCounter] = "Test multiple finalize/initialize sessions in different threads";
	result[testCounter] = test_finalize_initialize();
	testCounter++;

	for (i = 0; i < testCounter; i++)
	{
		testlog(LVL_INFO,"\nTest %d %s \n", testCounter, testDescription[i]);
		testlog(LVL_INFO,"Result : %d \n", result[i]);
	}
	
	endLog();
	// Wait for user to end this test
	//getchar();
}

