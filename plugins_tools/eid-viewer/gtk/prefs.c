#include "prefs.h"
#include <oslayer.h>

static GSettings* prefs = NULL;

GSettings* get_prefs() {
	if(G_UNLIKELY(!prefs)) {
		prefs = g_settings_new("be.belgium.eid.eid-viewer");
	}
	return prefs;
}
