#ifndef EID_VWR_GTK_PREFS_H
#define EID_VWR_GTK_PREFS_H

#include <config.h>

#if HAVE_GIO
#include <gio/gio.h>

GSettings *get_prefs() G_GNUC_PURE;
#else
#define get_prefs() NULL
#define g_settings_bind(s, k, o, p, f) { }

#endif

#endif
