#include <gtk/gtk.h>

#include "thread.h"
#include "photo.h"
#include "gtk_globals.h"

void displayphoto(void* data, int length) {
	GtkWidget* image = GTK_WIDGET(gtk_builder_get_object(builder, "photo"));
	GInputStream *mstream = G_INPUT_STREAM(g_memory_input_stream_new_from_data(data, length, NULL));
	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_stream(mstream, NULL, NULL);

	g_object_set_threaded(G_OBJECT(image), "pixbuf", pixbuf, g_object_unref);
}
