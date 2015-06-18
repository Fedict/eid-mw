#ifndef EID_VWR_GLIB_UTIL_H
#define EID_VWR_GLIB_UTIL_H

/* 
 * g_hash_table_contains is new in glib 2.32. If we're using an older
 * glib version, emulate its behaviour with g_hash_table_lookup_extended
 */
#include <glib.h>
#if !GLIB_CHECK_VERSION(2, 32, 0)
#define g_hash_table_contains(t, k) g_hash_table_lookup_extended(t, k, NULL, NULL)
#endif

#ifndef G_GNUC_BEGIN_IGNORE_DEPRECATIONS
#define G_GNUC_BEGIN_IGNORE_DEPRECATIONS
#define G_GNUC_END_IGNORE_DEPRECATIONS
#endif

#endif
