#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>

#include "thread.h"
#include "photo.h"
#include "gtk_globals.h"

#include <openssl/sha.h>

static struct photo_info pi;

/* Clear the photo element on the identity tab */
static void clearphoto(char* label G_GNUC_UNUSED) {
	GtkWidget* image = GTK_WIDGET(gtk_builder_get_object(builder, "photo"));
	g_object_set_threaded(G_OBJECT(image), "stock", "gtk-file", NULL);
	g_object_set_threaded(G_OBJECT(image), "sensitive", (void*)FALSE, NULL);
	if(pi.raw)
		free(pi.raw);
	if(pi.hash)
		free(pi.hash);
	if(pi.pixbuf)
		g_object_unref(pi.pixbuf);
	memset(&pi, 0, sizeof(pi));
}

/* Display the photo on the identity tab */
void displayphoto(char* label G_GNUC_UNUSED, void* data, int length) {
	GtkWidget* image = GTK_WIDGET(gtk_builder_get_object(builder, "photo"));
	/* To initialize a GdkPixbuf, we need a GInputStream, so create
	 * a memory-backed GInputStream in order to read the photo data
	 */
	GInputStream *mstream = G_INPUT_STREAM(g_memory_input_stream_new_from_data(data, length, NULL));
	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_stream(mstream, NULL, NULL);
	pi.pixbuf = GDK_PIXBUF(g_object_ref(pixbuf));
	g_input_stream_close(mstream, NULL, NULL);

	g_hash_table_insert(touched_labels, g_strdup("PHOTO_HASH"), clearphoto);
	g_object_set_threaded(G_OBJECT(image), "pixbuf", pixbuf, g_object_unref);
	g_object_set_threaded(G_OBJECT(image), "sensitive", (void*)TRUE, NULL);

	/* Store the data for later reference */
	pi.raw = malloc(length);
	memcpy(pi.raw, data, length);
	pi.plen = length;
}

/* Called when we receive the photo hash. Store for later reference */
void photohash(char* label G_GNUC_UNUSED, void* data, int length) {
	pi.hlen = length;
	pi.hash = malloc(length);
	memcpy(pi.hash, data, length);
}

/* Called by the data validation method. */
const struct photo_info* photo_get_data() {
	return &pi;
}
