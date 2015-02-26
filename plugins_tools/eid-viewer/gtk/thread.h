#ifndef EID_VWR_GTK_THREAD_H
#define EID_VWR_GTK_THREAD_H

#include <glib-object.h>

void g_object_set_threaded(GObject* obj, const gchar* first_property_name, ...);

#endif
