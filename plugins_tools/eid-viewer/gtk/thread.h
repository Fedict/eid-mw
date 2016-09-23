#ifndef EID_VWR_GTK_THREAD_H
#define EID_VWR_GTK_THREAD_H

#include <glib-object.h>

void g_object_set_threaded(GObject * obj, const gchar * name, void *value,
			   void (*freefunc) (void *));
void g_object_set_threaded_gvalue(GObject *obj, const gchar * property,
				  GValue * value, void(*freefunc)(void*));
void g_object_set_data_threaded(GObject * obj, const gchar * name,
				void *value, void (*freefunc) (void *));

#endif
