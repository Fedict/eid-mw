#ifdef WIN32
#include <win32.h>
#pragma pack(push, cryptoki, 1)
#include "pkcs11.h"
#pragma pack(pop, cryptoki)
#include <io.h>
#else
#include "unix.h"
#include "pkcs11.h"
#include <unistd.h>
#endif
#include <testlib.h>
#include <eid-viewer/oslayer.h>
#include <eid-viewer/macros.h>
#include <stdlib.h>
#include "common.h"


static enum eid_vwr_states curstate;
static bool flags[STATE_COUNT];
static bool have_keyhash = false;
static void (*orig_bindata)(const EID_CHAR*, const unsigned char*,int) = NULL;

static void mybindata (const EID_CHAR* label, const unsigned char *data, int datalen){
	orig_bindata(label, data, datalen);
	if (EID_STRCMP(label, TEXT(""))){
		have_keyhash = true;
	}
}

static void pinop_result(enum eid_vwr_pinops p, enum eid_vwr_result res) {
	verbose_assert(p == EID_VWR_PINOP_TEST);
	verbose_assert(res == EID_VWR_RES_SUCCESS);
}
static void newstate(enum eid_vwr_states s) {
	curstate = s;
	flags[s] = true;
	switch(curstate){
	case STATE_LIBOPEN:
		printf("library is open\n");		
		break;
	case STATE_CALLBACKS:
		printf("callback defined\n");		
		break;
	case STATE_NO_TOKEN:
		printf("no file or card\n");
		break;
	case STATE_NO_READER:
		printf("no reader foud (yet?)(child no token)\n");
		break;
	case STATE_READY:
		printf("ready to get token(child no token)\n");
		break;
	case STATE_TOKEN:
		printf("token found\n");
		break;
	case STATE_TOKEN_ID:
		printf("token found, reading id data\n");
		break;
	case STATE_TOKEN_CERTS:
		printf("token found, reading certificates data\n");
		break;
	case STATE_TOKEN_WAIT:
		printf("token found, waiting on action\n");
		break;
	case STATE_TOKEN_IDLE:
		printf("no action given durin token wait(child of wait)\n");
		break;
	case STATE_TOKEN_PINOP:
		printf("executing a pin operation\n");
		break;
	case STATE_TOKEN_SERIALIZE:
		printf("saving data to a file\n");
		break;
	case STATE_TOKEN_CHALLENGE:
		printf("performing a challenge");
		break;
	case STATE_TOKEN_ERROR:
		printf("error occured with card\n");
		break;	
	case STATE_FILE:
		printf("source is defined as file\n");
		break;
	case STATE_FILE_READING:
		printf("Reading the file\n");
		break;
	case STATE_FILE_WAIT:
		printf("done parsing file\n");
		break;
	case STATE_CARD_INVALID:
		printf("card read succesfull but deamed data invalid\n");
		break;
	default:
		exit(TEST_RV_SKIP);
	}
}
static void clearflags(){
	for (int i=0;i<STATE_COUNT;i++){
		flags[i]=false;
	}
}
TEST_FUNC(states) {
	struct eid_vwr_ui_callbacks* cb;
	cursrc = EID_VWR_SRC_UNKNOWN;
	if(!can_confirm()) {
		printf("Cannot do eID viewer tests without confirmation dialogs...\n");
		return TEST_RV_SKIP;
	}
	if(!have_robot()) {
		printf("no robot");
		return TEST_RV_SKIP;
	}
	robot_remove_card();
	robot_remove_reader();
	cb = createcbs();
	orig_bindata = cb->newbindata;
	cb->newbindata = mybindata;
	verbose_assert(cb != NULL);
	cb->pinop_result = pinop_result;
	cb->newstate = newstate;
	verbose_assert(eid_vwr_createcallbacks(cb) == 0);
	
#ifndef WIN32

	SLEEP(20);
	verbose_assert(flags[STATE_CALLBACKS]);
	verbose_assert(flags[STATE_NO_TOKEN]);
	newstate(curstate);
	verbose_assert(curstate == STATE_NO_READER);
	clearflags();
#endif

	robot_insert_reader();
	SLEEP(20);
	verbose_assert(curstate == STATE_READY);
	clearflags();
	
	newstate(curstate);
	eid_vwr_be_deserialize( "./67.06.30-296.59.eid");

	SLEEP(5);
	newstate(curstate);
	verbose_assert(flags[STATE_FILE]);
	verbose_assert(flags[STATE_FILE_READING]);
	verbose_assert(curstate == STATE_FILE_WAIT);
	eid_vwr_close_file ();
	SLEEP(10);
	newstate(curstate);

#ifdef WIN32
	robot_remove_reader();
	SLEEP(5);
	robot_insert_reader();
	SLEEP(5);
#endif
	verbose_assert(flags[STATE_NO_TOKEN]);
	verbose_assert(flags[STATE_NO_READER]);
	newstate(curstate);
	verbose_assert(curstate == STATE_READY);
	clearflags();

#ifndef WIN32
	if (is_manual_robot()){
		printf("wrong card test : do you want to continue? y/n\n");
		char character = 'n';
		if (scanf("%c",&character) != EOF && character == 'y')
		{
			robot_insert_card();
			SLEEP(10);
			newstate(curstate);
			verbose_assert(flags[STATE_TOKEN]);
			verbose_assert(flags[STATE_TOKEN_ID]);
			verbose_assert(curstate == STATE_TOKEN_ERROR);
			clearflags();
			robot_remove_card();
			SLEEP(10);
			verbose_assert(flags[STATE_NO_TOKEN]);
			verbose_assert(flags[STATE_READY]);
			clearflags();		
		}
		SLEEP(3);
	}
#endif

	printf("right card test\n");
	newstate(curstate);
	robot_insert_card();
	SLEEP(40);
	newstate(curstate);
	verbose_assert(flags[STATE_TOKEN]);
	verbose_assert(flags[STATE_TOKEN_ID]);
	//verbose_assert(flags[STATE_TOKEN_CERTS]);
	verbose_assert(flags[STATE_TOKEN_WAIT]);
	verbose_assert(curstate == STATE_TOKEN_IDLE);
	clearflags();
	
#ifndef WIN32
	if(!can_enter_pin(0)) {
		printf("Cannot do PIN tests without PIN code...\n");
		exit(TEST_RV_SKIP);
	}
	eid_vwr_pinop(EID_VWR_PINOP_TEST);
	SLEEP(5);
	verbose_assert(flags[STATE_TOKEN_PINOP]);
#endif
	clearflags();
	SLEEP(5);
	
#ifdef WIN32
	const EID_CHAR* name = L"test.xml";
#else 
	const EID_CHAR* name = "test.xml";
#endif
	eid_vwr_be_serialize(name);
	SLEEP(5);
	newstate(curstate);
	verbose_assert(flags[STATE_TOKEN_SERIALIZE]);
#ifdef WIN32
	verbose_assert(INVALID_FILE_ATTRIBUTES != GetFileAttributes(name));
	if (_wremove(name) != 0) {
		printf("problem removing file");
		return TEST_RV_SKIP;
	}
#else
	verbose_assert(access(name, F_OK) == 0);
	if (remove(name) != 0) {
		printf("problem removing file");
		return TEST_RV_SKIP;
	}
#endif 
	
	clearflags();
	SLEEP(5);
	/*
	printf("test for challenge(only run with a card v1.8)\n");
	if(have_keyhash){
		const unsigned char* challenge = "123456789012345678901234567890123456789012345678";
		int lenght = 48;
		eid_vwr_challenge(challenge, lenght);
		SLEEP(5);
		verbose_assert(flags[STATE_TOKEN_CHALLENGE]);
		clearflags();
		SLEEP(5);
	}
	*/
	eid_vwr_be_set_invalid();
	SLEEP(5);
	newstate(curstate);
	verbose_assert(curstate == STATE_CARD_INVALID);
		
	SLEEP(10);
	return TEST_RV_OK;
}
