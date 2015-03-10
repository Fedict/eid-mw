#ifndef EID_VWR_GLIB_UTIL_H
#define EID_VWR_GLIB_UTIL_H

/* 
 * g_hash_table_contains is new in glib 2.32. If we're using an older
 * glib version, emulate its behaviour with g_hash_table_lookup_extended
 */
#include <glib.h>
#if GLIB_VERSION_CUR_STABLE < G_ENCODE_VERSION(2, 32)
#define g_hash_table_contains(t, k) g_hash_table_lookup_extended(t, k, NULL, NULL)
#endif

#endif
