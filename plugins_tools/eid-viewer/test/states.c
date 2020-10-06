#include <unix.h>
#include <pkcs11.h>
#include <testlib.h>
#include <eid-viewer/oslayer.h>
#include <eid-viewer/macros.h>
#include <stdlib.h>
#include <pthread.h>
#include "common.h"

pthread_barrier_t barrier;

static enum eid_vwr_states curstate;
static bool flags[STATE_COUNT];
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
		printf("token found reading id data\n");
		break;
	case STATE_TOKEN_CERTS:
		printf("token found reading certificates data\n");
		break;
	case STATE_TOKEN_WAIT:
		printf("token found and waiting on action\n");
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
	int i;
	pthread_barrier_init(&barrier, NULL, 2);
	cursrc = EID_VWR_SRC_UNKNOWN;
	if(!can_confirm()) {
		printf("Cannot do eID viewer tests without confirmation dialogs...\n");
		return TEST_RV_SKIP;
	}
	if(!have_robot()) {
		return TEST_RV_SKIP;
	}
	cb = createcbs();
	verbose_assert(cb != NULL);
	cb->newstate = newstate;
	verbose_assert(eid_vwr_createcallbacks(cb) == 0);
	
	robot_remove_reader();
	SLEEP(10);
	verbose_assert(flags[STATE_CALLBACKS]);
	verbose_assert(flags[STATE_NO_TOKEN]);
	verbose_assert(curstate == STATE_NO_READER);
	clearflags();
	
	robot_insert_reader();
	SLEEP(10);
	verbose_assert(curstate == STATE_READY);
	clearflags();
	
	newstate(curstate);
	eid_vwr_be_deserialize(SRCDIR "/67.06.30-296.59.eid");
	SLEEP(5);
	newstate(curstate);
	verbose_assert(flags[STATE_FILE]);
	verbose_assert(flags[STATE_FILE_READING]);
	verbose_assert(curstate == STATE_FILE_WAIT);
	eid_vwr_close_file ();
	SLEEP(3);
	newstate(curstate);
	verbose_assert(flags[STATE_NO_TOKEN]);
	verbose_assert(flags[STATE_NO_READER]);
	verbose_assert(curstate == STATE_READY);
	clearflags();
	
	if (is_manual_robot()){
		printf("wrong card test : do you want to continue?\n");
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
			SLEEP(5);
			verbose_assert(flags[STATE_NO_TOKEN]);
			verbose_assert(flags[STATE_READY]);
			clearflags();		
		}
		SLEEP(3);
	}
	
	printf("right card test\n");
	newstate(curstate);
	robot_insert_card();
	SLEEP(10);
	newstate(curstate);
	verbose_assert(flags[STATE_TOKEN]);
	verbose_assert(flags[STATE_TOKEN_ID]);
	//verbose_assert(flags[STATE_TOKEN_CERTS]);
	verbose_assert(flags[STATE_TOKEN_WAIT]);
	verbose_assert(curstate == STATE_TOKEN_IDLE);
	clearflags();
	
	eid_vwr_be_set_invalid();
	SLEEP(5);
	newstate(curstate);
	verbose_assert(curstate == STATE_CARD_INVALID);
	
	SLEEP(10);
	return TEST_RV_OK;
}
