#include "prefs.h"
#include <eid-viewer/oslayer.h>

static GSettings* prefs = NULL;

/* Returns the GSettings* pointer to our preferences */
GSettings* get_prefs() {
	if(G_UNLIKELY(!prefs)) {
		prefs = g_settings_new("be.belgium.eid.eid-viewer");
	}
	return prefs;
}
