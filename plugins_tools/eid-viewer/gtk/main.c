#include <config.h>
#include <gtk/gtk.h>
#include <assert.h>

#include "oslayer.h"
#include "viewer_glade.h"
#include "gettext.h"
#include "binops.h"
#include "gtk_globals.h"

#include "gtkui.h"

#ifndef _
#define _(s) gettext(s)
#endif

typedef void(*bindisplayfunc)(void*, int);
typedef void(*clearfunc)(char*);

static GHashTable* binhash;

static void uilog(eid_vwr_level, line) {
	// TODO
}

static void stringclear(char* l) {
	GtkLabel* label = GTK_LABEL(gtk_builder_get_object(builder, l));
	// Should only appear in the hash table if we successfully found it
	// earlier...
	assert(label != NULL);
	gtk_label_set_text(label, "-");
}

static void newstringdata(char* l, char* data) {
	GtkLabel* label = GTK_LABEL(gtk_builder_get_object(builder, l));
	clearfunc f = stringclear;
	if(!label) {
		char* msg = g_strdup_printf(_("Could not display label '%s', data '%s': no GtkLabel found for data"), label, data);
		uilog(EID_VWR_LOG_DETAIL, msg);
		g_free(msg);
		return;
	}
	g_hash_table_insert(touched_labels, label, f);
	gtk_label_set_text(label, data);
}

static void newbindata(char* label, void* data, int datalen) {
	bindisplayfunc func;

	if(!g_hash_table_contains(binhash, label)) {
		char* msg = g_strdup_printf(_("Could not display binary data with label '%s': not found in hashtable"), label);
	}
	func = (bindisplayfunc)g_hash_table_lookup(binhash, label);
	func(data, datalen);
	return;
}

static void cleardata(gpointer key, gpointer value, gpointer user_data G_GNUC_UNUSED) {
	clearfunc func = (clearfunc)value;
	char* k = key;
	func(k);
}

static void newsrc(enum eid_vwr_source src) {
	g_hash_table_foreach(touched_labels, cleardata, NULL);
	// TODO: update display so we see which source we're using
}

static gboolean poll(gpointer user_data G_GNUC_UNUSED) {
	eid_vwr_poll();
	return TRUE;
}

int main(int argc, char** argv) {
	GtkWidget *window;
	GObject* signaltmp;
	GtkAccelGroup *group;
	struct eid_vwr_ui_callbacks* cb;

	bindtextdomain("eid-viewer", DATAROOTDIR "/locale");
	textdomain("eid-viewer");

	gtk_init(&argc, &argv);
	builder = gtk_builder_new();
	gtk_builder_add_from_string(builder, VIEWER_GLADE_STRING, strlen(VIEWER_GLADE_STRING), NULL);

	window = GTK_WIDGET(gtk_builder_get_object(builder, "mainwin"));
	group = gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(window), group);

	touched_labels = g_hash_table_new(g_str_hash, g_str_equal);
	binhash = g_hash_table_new(g_str_hash, g_str_equal);

	cb = eid_vwr_cbstruct();
	cb->newsrc = newsrc;
	cb->newstringdata = newstringdata;
	cb->newbindata = newbindata;
	cb->log = uilog;
	eid_vwr_createcallbacks(cb);

	g_signal_connect(G_OBJECT(window), "delete-event", gtk_main_quit, NULL);
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "mi_file_open"));
	g_signal_connect(signaltmp, "activate", G_CALLBACK(file_open), NULL);
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "mi_file_saveas_xml"));
	g_signal_connect(signaltmp, "activate", G_CALLBACK(file_save), "xml");
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "mi_file_saveas_csv"));
	g_signal_connect(signaltmp, "activate", G_CALLBACK(file_save), "csv");
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "mi_file_close"));
	g_signal_connect(signaltmp, "activate", G_CALLBACK(file_close), NULL);
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "mi_file_prefs"));
	g_signal_connect(signaltmp, "activate", G_CALLBACK(file_prefs), NULL);
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "mi_file_print"));
	g_signal_connect(signaltmp, "activate", G_CALLBACK(file_print), NULL);
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "mi_file_quit"));
	g_signal_connect(signaltmp, "activate", G_CALLBACK(gtk_main_quit), NULL);
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "mi_lang_de"));
	g_signal_connect(signaltmp, "activate", G_CALLBACK(translate), "de");
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "mi_lang_en"));
	g_signal_connect(signaltmp, "activate", G_CALLBACK(translate), "en");
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "mi_lang_fr"));
	g_signal_connect(signaltmp, "activate", G_CALLBACK(translate), "fr");
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "mi_lang_nl"));
	g_signal_connect(signaltmp, "activate", G_CALLBACK(translate), "nl");
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "mi_help_about"));
	g_signal_connect(signaltmp, "activate", G_CALLBACK(showabout), NULL);
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "mi_help_faq"));
	g_signal_connect(signaltmp, "activate", G_CALLBACK(showurl), "faq");
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "mi_help_test"));
	g_signal_connect(signaltmp, "activate", G_CALLBACK(showurl), "test");
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "mi_help_log"));
	g_signal_connect(signaltmp, "activate", G_CALLBACK(showlog), NULL);

	g_timeout_add_seconds(1, poll, NULL);

	gtk_widget_show_all(window);

	gtk_main();
}
