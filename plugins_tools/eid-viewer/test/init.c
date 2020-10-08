#include <unix.h>
#include <pkcs11.h>
#include <eid-viewer/oslayer.h>
#include <testlib.h>

TEST_FUNC(init) {
	struct eid_vwr_ui_callbacks* cb;

	cb = eid_vwr_cbstruct();
	verbose_assert(cb != NULL);
	verbose_assert(cb->version == 1);
	verbose_assert(cb->newsrc == NULL);
	verbose_assert(cb->newstringdata == NULL);
	verbose_assert(cb->newbindata == NULL);
	verbose_assert(cb->log == NULL);
	verbose_assert(cb->logv == NULL);
	verbose_assert(cb->newstate == NULL);
	verbose_assert(cb->pinop_result == NULL);
	verbose_assert(cb->readers_changed == NULL);
	verbose_assert(cb->challenge_result == NULL);

	return TEST_RV_OK;
}
