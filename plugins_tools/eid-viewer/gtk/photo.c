#include <gtk/gtk.h>
#include <string.h>

#include "thread.h"
#include "photo.h"
#include "gtk_globals.h"

#include <openssl/sha.h>

static unsigned char sha1_detected[SHA_DIGEST_LENGTH];
static unsigned char sha1_asserted[SHA_DIGEST_LENGTH];

static void clearphoto(char* label) {
	GtkWidget* image = GTK_WIDGET(gtk_builder_get_object(builder, "photo"));
	g_object_set_threaded(G_OBJECT(image), "stock", "gtk-file", NULL);
	g_object_set_threaded(G_OBJECT(image), "sensitive", (void*)FALSE, NULL);
	memset(sha1_detected, 0, SHA_DIGEST_LENGTH);
	memset(sha1_asserted, 0, SHA_DIGEST_LENGTH);
}

void displayphoto(char* label G_GNUC_UNUSED, void* data, int length) {
	GtkWidget* image = GTK_WIDGET(gtk_builder_get_object(builder, "photo"));
	GInputStream *mstream = G_INPUT_STREAM(g_memory_input_stream_new_from_data(data, length, NULL));
	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_stream(mstream, NULL, NULL);
	g_input_stream_close(mstream, NULL, NULL);

	g_hash_table_insert(touched_labels, g_strdup("PHOTO_HASH"), clearphoto);
	g_object_set_threaded(G_OBJECT(image), "pixbuf", pixbuf, g_object_unref);
	g_object_set_threaded(G_OBJECT(image), "sensitive", (void*)TRUE, NULL);

	SHA1(data, length, sha1_detected);
}

void photohash(char* label G_GNUC_UNUSED, void* data, int length) {
	if(length == SHA_DIGEST_LENGTH) {
		memcpy(sha1_asserted, data, SHA_DIGEST_LENGTH);
	}
}

int photo_is_valid() {
	char sha1_empty[SHA_DIGEST_LENGTH];
	memset(sha1_empty, 0, SHA_DIGEST_LENGTH);
	if(!memcmp(sha1_detected, sha1_empty, SHA_DIGEST_LENGTH) || !memcmp(sha1_asserted, sha1_empty, SHA_DIGEST_LENGTH)) {
		return -1;
	}
	return !memcmp(sha1_detected, sha1_asserted, SHA_DIGEST_LENGTH);
}
