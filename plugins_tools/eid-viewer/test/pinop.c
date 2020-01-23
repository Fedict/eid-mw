#include <unix.h>
#include <pkcs11.h>
#include <testlib.h>
#include <eid-viewer/oslayer.h>
#include <eid-viewer/macros.h>
#include <stdlib.h>
#include <pthread.h>
#include "common.h"

pthread_barrier_t barrier;

static void pinop_result(enum eid_vwr_pinops p, enum eid_vwr_result res) {
	verbose_assert(p == EID_VWR_PINOP_TEST);
	verbose_assert(res == EID_VWR_RES_SUCCESS);
	exit(TEST_RV_OK);
}

static enum eid_vwr_states curstate;

static void newstate(enum eid_vwr_states s) {
	curstate = s;
	switch(curstate) {
	case STATE_TOKEN_WAIT:
		if(!can_enter_pin(0)) {
			printf("Cannot do PIN tests without PIN code...\n");
			exit(TEST_RV_SKIP);
		}
		eid_vwr_pinop(EID_VWR_PINOP_TEST);
		pthread_barrier_wait(&barrier);
	break;
	case STATE_CARD_INVALID:
		fprintf("E: could not read token: card data invalid");
		exit(TEST_RV_FAIL);
	default:
		// nothing to do here
		break;
	}
}

TEST_FUNC(pinop) {
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
	robot_insert_card();
	cb = createcbs();
	verbose_assert(cb != NULL);
	cb->pinop_result = pinop_result;
	cb->newstate = newstate;
	verbose_assert(eid_vwr_createcallbacks(cb) == 0);
	SLEEP(1);
	printf("Card inserted, waiting 20 seconds for card event...\n");
	for(i=0; i<20; i++) {
		SLEEP(1);
		if(cursrc != EID_VWR_SRC_NONE) {
			i+=20;
		}
		printf(".");
	}
	if(cursrc != EID_VWR_SRC_CARD) {
		printf("No card found...\n");
		return TEST_RV_SKIP;
	}
	pthread_barrier_wait(&barrier);
	SLEEP(30);
	return TEST_RV_FAIL;
}
