#include <unix.h>
#include <pkcs11.h>
#include <testlib.h>
#include "common.h"
#include <eid-viewer/oslayer.h>

static void(*nsd)(const EID_CHAR*, const EID_CHAR*) = NULL;
static int have_found_cardnumber = 0;

static void newstringdata(const EID_CHAR* label, const EID_CHAR* data) {
	nsd(label, data);
	if(!strcmp(label, "card_number")) {
		verbose_assert(strcmp(data, "592000155079") == 0);
		have_found_cardnumber = 1;
	}
}

TEST_FUNC(deserialize) {
	struct eid_vwr_ui_callbacks* cb;

	cb = createcbs();
	if(!have_robot()) {
		printf("E: Need a robot to remove the card...\n");
		return TEST_RV_SKIP;
	}
	robot_remove_card();
	verbose_assert(cb != NULL);
	nsd = cb->newstringdata;
	cb->newstringdata = newstringdata;
	verbose_assert(eid_vwr_createcallbacks(cb) == 0);
	printf("opening " SRCDIR "/67.06.30-296.59.eid");
	eid_vwr_be_deserialize(SRCDIR "/67.06.30-296.59.eid");
	SLEEP(1);
	verbose_assert(cursrc == EID_VWR_SRC_FILE);
	verbose_assert(have_found_cardnumber == 1);
	return TEST_RV_OK;
}
