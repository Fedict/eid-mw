#include <stdarg.h>
#include <stdlib.h>
#include "thread.h"

struct gost_helper {
	GObject* obj;
	const gchar* name;
	void* value;
	void(*free)(void*);
};

static gboolean gost_helper(gpointer user_data) {
	struct gost_helper* help = (struct gost_helper*) user_data;
	g_object_set(help->obj, help->name, help->value, NULL);
	if(help->free != NULL) {
		help->free(help->value);
	}
	free(help);
	return FALSE;
}

static gboolean gosdt_helper(gpointer user_data) {
	struct gost_helper* help = (struct gost_helper*) user_data;
	if(help->free) {
		g_object_set_data_full(help->obj, help->name, help->value, help->free);
	} else {
		g_object_set_data(help->obj, help->name, help->value);
	}
	free(help);
	return FALSE;
}

static gboolean gostv_helper(gpointer user_data) {
	struct gost_helper* help = (struct gost_helper*) user_data;
	g_object_set_property(help->obj, help->name, (GValue*)help->value);
	if(help->free) {
		help->free(help->value);
	}
	free(help);
	return FALSE;
}

/* Helper function to allow properties to be set transparently from
 * background threads */
void g_object_set_threaded(GObject* obj, const gchar* property, void* value, void(*freefunc)(void*)) {
	struct gost_helper* help = malloc(sizeof(struct gost_helper));
	help->obj = obj;
	help->name = property;
	help->value = value;
	help->free = freefunc;
	g_main_context_invoke(NULL, gost_helper, help);
}

void g_object_set_threaded_gvalue(GObject* obj, const gchar* property, GValue* value, void(*freefunc)(void*)) {
	struct gost_helper* help = malloc(sizeof(struct gost_helper));
	help->obj = obj;
	help->name = property;
	help->value = value;
	help->free = freefunc;
	g_main_context_invoke(NULL, gostv_helper, help);
}

void g_object_set_data_threaded(GObject* obj, const gchar* key, void* value, void(*freefunc)(void*)) {
	struct gost_helper *help = malloc(sizeof(struct gost_helper));
	help->obj = obj;
	help->name = key;
	help->value = value;
	help->free = freefunc;
	g_main_context_invoke(NULL, gosdt_helper, help);
}
