#ifndef EID_VWR_GTK_THREAD_H
#define EID_VWR_GTK_THREAD_H

#include <glib-object.h>

void g_object_set_threaded(GObject* obj, const gchar* name, void* value, gboolean do_free);

#endif
