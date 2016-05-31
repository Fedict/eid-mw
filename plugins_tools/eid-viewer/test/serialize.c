#include <unix.h>
#include <pkcs11.h>
#include <testlib.h>
#include "oslayer.h"

TEST_FUNC(serialize) {
	struct eid_vwr_ui_callbacks* cb;
	void* data;
	int len;

	if(!have_robot()) {
		printf("Need a robot\n");
		return TEST_RV_SKIP;
	}
	if(!can_confirm()) {
		printf("Need the ability to read data from the card\n");
		return TEST_RV_SKIP;
	}
	robot_insert_card();
	cb = createcbs();
	verbose_assert(cb != NULL);
	verbose_assert(eid_vwr_createcallbacks(cb) == 0);
	verbose_assert(eid_vwr_serialize(&data, &len) == 0);
	robot_remove_card();
	verbose_assert(eid_vwr_serialize(&data, &len) != 0);
	return TEST_RV_OK;
}
