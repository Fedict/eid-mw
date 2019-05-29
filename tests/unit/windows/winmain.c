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

	eIDTest eIDTests[] = { \
//	{{0,TEST_SKIPPED},"Tests C_getinfo results in a single thread",&test_getinfo},
//#ifdef WIN32
//	{{0,TEST_SKIPPED},"Tests multiple finalize/initialize sessions in multiple threads with different args",&test_finalize_initialize},
//	{{0,TEST_SKIPPED},"Tests multiple finalize/initialize sessions in a single thread",&test_finalize_initialize_st},
//	{{0,TEST_SKIPPED},"Tests C_initialize when bad parameters are supplied 1",&test_initialize_ownmutex},
//	{{0,TEST_SKIPPED},"Tests C_initialize when bad parameters are supplied",&test_initialize_preserved},
//	{{0,TEST_SKIPPED},"Tests C_finalize when bad parameters are supplied",&test_finalize_preserved},
//#endif //WIN32
//	{{0,TEST_SKIPPED},"Tests C_getslotlist in single and two call usage",&test_getslotlist},
//	{{0,TEST_SKIPPED},"Tests C_getslotlist when insufficient memory is reserved",&test_getslotlist_multiple_slots},
//	{{0,TEST_SKIPPED},"Tests opening and closing of a session in a single thread",&test_open_close_session},
//	{{0,TEST_SKIPPED},"Tests getting slotID and slotdescription",&test_open_close_session_info},
//	{{0,TEST_SKIPPED},"Tests C_opensession with wrong parameters supplied",&test_open_close_session_bad_params},
//	{{0,TEST_SKIPPED},"Tests how many sessions can be opened",&test_open_close_session_limits},
//#ifdef WIN32
//{{0,TEST_SKIPPED},"tests getting all the objects from the card",&test_logslotevents_blocking},

//  #ifdef USER_INTERACTION
//	{{0,TEST_SKIPPED},"Tests waitforslot event (needs user interaction)",&test_waitforslotevent_userinteraction},//
//	{{0,TEST_SKIPPED},"tests signing with the card",&test_sign},
//		{{0,TEST_SKIPPED},"tests signing sequence with all mechanisms on the card",&test_sign_allmechs},
//	{{0,TEST_SKIPPED},"Tests waiting for slot event blocking",&test_waitforslotevent_blocking},
//  #endif
//	{{0,TEST_SKIPPED},"Tests waiting for slot event non-blocking",&test_waitforslotevent_nonblocking},
//	{{0,TEST_SKIPPED},"Tests waiting for slot event blocking, while another thread calls c_finalize",&test_waitforslotevent_whilefinalize},
//#endif //WIN32
//	{{0,TEST_SKIPPED},"Tests the retrieval of slot and token info",&test_get_token_info},
//	{{0,TEST_SKIPPED},"Shows info on the mechanisms supported by the card",&test_getmechanisms},
//	{{0,TEST_SKIPPED},"tests getting the keys from the card",&test_getkeys},
//	{{0,TEST_SKIPPED},"tests the return values of the getattributes function",&test_getkeys_retvals},
	//{{0,TEST_SKIPPED},"tests getting all the objects from the card",&test_getallobjects},
		{0,"tests initialize/finalize",&init_finalize},
			{0,"tests getinfo",&getinfo},
			//{0,"tests initialize/finalize",&funclist},
			{0,"tests slotlist",&slotlist},
			{0,"tests slotinfo",&slotinfo},
			{0,"tests tkinfo",&tkinfo},
			{0,"tests double_init",&double_init},
			{0,"tests slotevent",&slotevent},
			{0,"tests mechlist",&mechlist},
			{0,"tests mechinfo",&mechinfo},
			{0,"tests sessions",&sessions},
			{0,"tests sessions_nocard",&sessions_nocard},
			{0,"tests sessioninfo",&sessioninfo},
			{0,"tests login",&slogin},
			{0,"tests nonsensible",&nonsensible},
			{0,"tests objects",&objects},
			{0,"tests readdata",&readdata},
			{0,"tests readdata_limited", &readdata_limited},
			{0,"tests readdata_sequence", &readdata_sequence},
			{0,"tests digest",&digest},
			{0,"tests sign", &sign},
			{0,"tests sign_state",&sign_state},
			{0,"tests reading the photo",&decode_photo},
			{0,"tests ordering",&ordering},
			{0,"tests wrong_init", &wrong_init},
			{0,"tests login_state", &login_state},


//		{{0,TEST_SKIPPED},"tests getting all the objects from the card",&test_getallidobjects},
	
//	{{0,TEST_SKIPPED},"tests getting object's attributes from the card",&test_getattributevalue_all},
//	{{0,TEST_SKIPPED},"tests getting object's attributes from the card",&test_getattributevalue_rrn},
	
//	{{0,TEST_SKIPPED},"tests getting the lastname object's attributes from the card",&test_getattributevalue_lastname},	
//#ifdef WIN32
//	{{0,TEST_SKIPPED},"tests adding and removing readers",&test_add_remove_readers},
//	{{0,TEST_SKIPPED},"tests waiting for card and reader events",&test_add_remove_readerevents},
//#endif //WIN32
//	{{0,TEST_SKIPPED},"tests getting the private object's from the card without logging in",&test_findPrivateKeyWithoutLoginShouldFail},	
//	{{0,TEST_SKIPPED},"tests the return value of C_FindObjectsFinal when called without C_FindObjectsInit",&test_findObjectsFinalNotInitialized},
	};

	if(argc >=2)
		eid_robot_style = argv[1];

	if(argc >=3)
		eid_dialogs_style = argv[2];

	if(argc >=4)
		eid_builtin_reader = argv[3];

	error = initLog();
	if(error != 0)
	{
		printf("Can't open logfile");
		return -1;
	}
	nrofTests = (sizeof(eIDTests)/sizeof(eIDTest));

	while (i < nrofTests )
	{
		printf("test %d: %s\n", i, eIDTests[i].testDescription);
		eIDTests[i].result = eIDTests[i].test_function_ptr();
		i++;
	}


	//testlog(LVL_NOLEVEL,"\n\n_______________________________________________\n");
	for (i = 0; i < nrofTests; i++)
	{
		if (eIDTests[i].testDescription != NULL)
		{
			testlog(LVL_NOLEVEL,"_______________________________________________\n");
			testlog(LVL_NOLEVEL,"Test %d %s \n", i, eIDTests[i].testDescription);
		}
		switch(eIDTests[i].result) {
			case TEST_RV_OK:
				testlog(LVL_NOLEVEL,"OK");
				break;
			case TEST_RV_SKIP:
				testlog(LVL_NOLEVEL, "SKIPPED");
				break;
			case TEST_RV_FAIL:
				testlog(LVL_NOLEVEL, "FAILED");
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
	testlog(LVL_NOLEVEL,"press a key to end the test\n");
	// Wait for user to end this test
#ifdef WIN32
	getchar();
#endif
}

