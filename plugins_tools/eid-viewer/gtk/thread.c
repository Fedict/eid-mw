#include <stdarg.h>
#include <stdlib.h>
#include "thread.h"

struct gost_helper {
	GObject* obj;
	const gchar* firstprop;
	va_list ap;
};

static gboolean gost_helper(gpointer user_data) {
	struct gost_helper* help = (struct gost_helper*) user_data;
	g_object_set_valist(help->obj, help->firstprop, help->ap);
	free(help);
	return FALSE;
}

void g_object_set_threaded(GObject* obj, const gchar* first_property_name, ...) {
	va_list ap;
	struct gost_helper* help = malloc(sizeof(struct gost_helper));
	help->obj = obj;
	help->firstprop = first_property_name;
	va_start(ap, first_property_name);
	va_copy(help->ap, ap);
	g_main_context_invoke(NULL, gost_helper, help);
}
