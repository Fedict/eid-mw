#include <unix.h>
#include <pkcs11.h>
#include <testlib.h>
#include <eid-viewer/oslayer.h>
#include <eid-viewer/macros.h>
#include <stdlib.h>
#include "common.h"

TEST_FUNC(cardevent) {
	struct eid_vwr_ui_callbacks* cb;
	int i;

	cursrc = EID_VWR_SRC_UNKNOWN;
	if(!have_robot()) {
		printf("Cannot do card event tests without a card...\n");
		return TEST_RV_SKIP;
	}
	if(!can_confirm()) {
		printf("Cannot do eID viewer tests without confirmation dialogs...\n");
		return TEST_RV_SKIP;
	}
	robot_remove_card();
	cb = createcbs();
	verbose_assert(cb != NULL);
	verbose_assert(eid_vwr_createcallbacks(cb) == 0);
	SLEEP(1);
	verbose_assert(cursrc == EID_VWR_SRC_NONE);
	robot_insert_card();
	printf("Card inserted, waiting 20 seconds for card event...\n");
	for(i=0; i<20; i++) {
		SLEEP(1);
		if(cursrc != EID_VWR_SRC_NONE) {
			i+=10;
		}
		eid_vwr_poll();
		printf(".");
	}
	verbose_assert(cursrc == EID_VWR_SRC_CARD);
	return TEST_RV_OK;
}
