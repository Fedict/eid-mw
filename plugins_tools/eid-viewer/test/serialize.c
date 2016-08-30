#include <unix.h>
#include <pkcs11.h>
#include <testlib.h>
#include "common.h"
#include <eid-viewer/oslayer.h>

TEST_FUNC(serialize) {
	struct eid_vwr_ui_callbacks* cb;

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
	eid_vwr_be_serialize(TEXT("test.xml"));
	robot_remove_card();
	eid_vwr_be_serialize(TEXT("test.xml"));
	return TEST_RV_OK;
}
