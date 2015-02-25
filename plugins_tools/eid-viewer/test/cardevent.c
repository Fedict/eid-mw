#include <unix.h>
#include <pkcs11.h>
#include <testlib.h>
#include "oslayer.h"
#include <stdlib.h>
#include "common.h"

TEST_FUNC(cardevent) {
	struct eid_vwr_ui_callbacks* cb;

	cursrc = EID_VWR_SRC_UNKNOWN;
	if(!have_robot()) {
		printf("Cannot do card event tests without a card...\n");
		return TEST_RV_SKIP;
	}
	robot_remove_card();
	cb = createcbs();
	verbose_assert(cb != NULL);
	verbose_assert(eid_vwr_createcallbacks(cb) != 0);
	sleep(1);
	verbose_assert(cursrc == EID_VWR_SRC_NONE);
	robot_insert_card();
	printf("Card inserted, waiting 10 seconds for card event...\n");
	sleep(10);
	verbose_assert(cursrc == EID_VWR_SRC_CARD);
	return TEST_RV_OK;
}
