#include <config.h>
#include <gtk/gtk.h>
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>

#include "oslayer.h"
#include "viewer_glade.h"
#include "gettext.h"
#include "gtk_globals.h"

#include "gtkui.h"
#include "thread.h"
#include "photo.h"
#include "verify.h"
#include "certs.h"
#include "state.h"
#include "glib_util.h"
#include "logging.h"
#include "prefs.h"

#ifndef _
#define _(s) gettext(s)
#endif

typedef void(*bindisplayfunc)(const char*, void*, int);
typedef void(*clearfunc)(char*);

static GHashTable* binhash;
static guint statusbar_context = 0;

extern char** environ;

struct statusupdate {
	gboolean spin;
	char* text;
};

static gboolean realstatus(gpointer data) {
	GtkStatusbar* sb = GTK_STATUSBAR(gtk_builder_get_object(builder, "statusbar"));
	GtkSpinner* spinner = GTK_SPINNER(gtk_builder_get_object(builder, "busy_spinner"));
	struct statusupdate *update = (struct statusupdate*)data;

	if(G_UNLIKELY(!statusbar_context)) {
		statusbar_context = gtk_statusbar_get_context_id(sb, "useless");
	}
	gtk_statusbar_remove_all(sb, statusbar_context);
	gtk_widget_set_visible(GTK_WIDGET(spinner), update->spin);
	g_object_set(G_OBJECT(spinner), "active", update->spin, NULL);
	if(update->text) {
		gtk_statusbar_push(sb, statusbar_context, update->text);
		g_free(update->text);
	}
	g_free(update);

	return FALSE;
}

static void uistatus(gboolean spin, char* data, ...) {
	va_list ap, ac;
	struct statusupdate* update = g_new0(struct statusupdate, 1);

	if(data != NULL) {
		va_start(ap, data);
		va_copy(ac, ap);
		update->text = g_strdup_vprintf(data, ac);
		va_end(ac);
		va_end(ap);
	}
	update->spin = spin;
	g_main_context_invoke(NULL, realstatus, update);
}

static void newstate(enum eid_vwr_states s) {
	switch(s) {
		case STATE_LIBOPEN:
		case STATE_CALLBACKS:
			uistatus(TRUE, _("Initializing"));
			return;
		case STATE_READY:
			uistatus(FALSE, _("Ready to read identity card."));
			return;
		case STATE_TOKEN:
			uistatus(TRUE, _("Card available"));
			return;
		case STATE_TOKEN_WAIT:
			uistatus(FALSE, _(""));
			return;
		case STATE_TOKEN_ID:
			uistatus(TRUE, _("Reading identity."));
			return;
		case STATE_TOKEN_CERTS:
			uistatus(TRUE, _("Reading certificates from card"));
			return;
		case STATE_TOKEN_PINOP:
			uistatus(TRUE, _("Performing a PIN operation"));
			return;
		case STATE_FILE:
			uistatus(TRUE, _("Reading data from file"));
		default:
			return;
	}
}

static void stringclear(const char* l) {
	GtkLabel* label = GTK_LABEL(gtk_builder_get_object(builder, l));
	// Should only appear in the hash table if we successfully found it
	// earlier...
	assert(label != NULL);
	g_object_set_threaded(G_OBJECT(label), "label", "-", NULL);
	g_object_set_threaded(G_OBJECT(label), "sensitive", FALSE, NULL);
}

static void newstringdata(const char* l, char* data) {
	GtkLabel* label = GTK_LABEL(gtk_builder_get_object(builder, l));
	if(!label) {
		char* msg = g_strdup_printf(_("Could not display label '%s', data '%s': no GtkLabel found for data"), l, data);
		uilog(EID_VWR_LOG_DETAIL, msg);
		g_free(msg);
		return;
	}
	if(!data || strlen(data) == 0) {
		stringclear(l);
		return;
	}
	g_hash_table_insert(touched_labels, g_strdup(l), stringclear);
	g_object_set_threaded(G_OBJECT(label), "label", g_strdup(data), g_free);
	g_object_set_threaded(G_OBJECT(label), "sensitive", (void*)TRUE, NULL);
}

static void newbindata(const char* label, void* data, int datalen) {
	bindisplayfunc func;
	gchar* msg;

	if(!g_hash_table_contains(binhash, label)) {
		msg = g_strdup_printf(_("Could not display binary data with label '%s': not found in hashtable"), label);
		uilog(EID_VWR_LOG_DETAIL, msg);
		free(msg);
		return;
	}
	func = (bindisplayfunc)g_hash_table_lookup(binhash, label);
	func(label, data, datalen);
	return;
}

static void cleardata(gpointer key, gpointer value, gpointer user_data G_GNUC_UNUSED) {
	clearfunc func = (clearfunc)value;
	char* k = key;
	func(k);
}

static void newsrc(enum eid_vwr_source src) {
	clear_certdata();
	g_hash_table_foreach(touched_labels, cleardata, NULL);
	// TODO: update display so we see which source we're using
}

static gboolean poll(gpointer user_data G_GNUC_UNUSED) {
	eid_vwr_poll();
	return TRUE;
}

static void* threadmain(void* data G_GNUC_UNUSED) {
	eid_vwr_be_mainloop();
}

enum eid_vwr_langs langfromenv() {
	char* p;
	char* all = NULL;
	char* msg = NULL;
	char* lang = NULL;
	int i;

	for(i=0, p=environ[0];p != NULL; i++, p=environ[i]) {
		if(!strncmp(p, "LC_ALL=", 7)) {
			all = p+7;
		}
		if(!strncmp(p, "LC_MESSAGES=", 12)) {
			msg = p+12;
		}
		if(!strncmp(p, "LANG=", 5)) {
			lang = p+5;
		}
	}
	if(all != NULL) {
		p = all;
	} else if(msg != NULL) {
		p = msg;
	} else if(lang != NULL) {
		p = lang;
	} else {
		p = "";
	}
	if(!strncmp(p, "de", 2)) {
		return EID_VWR_LANG_DE;
	}
	if(!strncmp(p, "fr", 2)) {
		return EID_VWR_LANG_FR;
	}
	if(!strncmp(p, "nl", 2)) {
		return EID_VWR_LANG_NL;
	}
	return EID_VWR_LANG_EN;
}

static void connect_signals(GtkWidget* window) {
	GObject* signaltmp;

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
	g_signal_connect(signaltmp, "activate", G_CALLBACK(translate), "de_BE.UTF-8");
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "mi_lang_en"));
	g_signal_connect(signaltmp, "activate", G_CALLBACK(translate), "en_US.UTF-8");
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "mi_lang_fr"));
	g_signal_connect(signaltmp, "activate", G_CALLBACK(translate), "fr_BE.UTF-8");
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "mi_lang_nl"));
	g_signal_connect(signaltmp, "activate", G_CALLBACK(translate), "nl_BE.UTF-8");
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "mi_help_about"));
	g_signal_connect(signaltmp, "activate", G_CALLBACK(showabout), NULL);
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "mi_help_faq"));
	g_signal_connect(signaltmp, "activate", G_CALLBACK(showurl), "faq");
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "mi_help_test"));
	g_signal_connect(signaltmp, "activate", G_CALLBACK(showurl), "test");
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "pintestbut"));
	g_signal_connect(signaltmp, "clicked", G_CALLBACK(pinop), (void*)EID_VWR_PINOP_TEST);
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "pinchangebut"));
	g_signal_connect(signaltmp, "clicked", G_CALLBACK(pinop), (void*)EID_VWR_PINOP_CHG);

	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "cert_paned"));
	g_settings_bind(get_prefs(), "cert-paned-pos", signaltmp, "position", 0);
}

static void show_date_state(char* label, void* data, int length) {
	gchar* labelname = g_strndup(label, strstr(label, ":") - label);
	GtkWidget* l = GTK_WIDGET(gtk_builder_get_object(builder, labelname));
	g_free(labelname);
}

static void bindata_init() {
	binhash = g_hash_table_new(g_str_hash, g_str_equal);

	g_hash_table_insert(binhash, "PHOTO_FILE", displayphoto);
	g_hash_table_insert(binhash, "photo_hash", photohash);
	g_hash_table_insert(binhash, "SIGN_DATA_FILE", add_verify_data);
	g_hash_table_insert(binhash, "SIGN_ADDRESS_FILE", add_verify_data);
	g_hash_table_insert(binhash, "ADDRESS_FILE", add_verify_data);
	g_hash_table_insert(binhash, "DATA_FILE", add_verify_data);
	g_hash_table_insert(binhash, "CERT_RN_FILE", add_certificate);
	g_hash_table_insert(binhash, "Authentication", add_certificate);
	g_hash_table_insert(binhash, "CA", add_certificate);
	g_hash_table_insert(binhash, "Root", add_certificate);
	g_hash_table_insert(binhash, "Signature", add_certificate);
	g_hash_table_insert(binhash, "certvalfromval:past", show_date_state);
	g_hash_table_insert(binhash, "certvaltilval:future", show_date_state);
}

static void update_info_detail(GtkTreeModel* model, GtkTreePath *path, GtkTreeIter* iter, gpointer data G_GNUC_UNUSED) {
	gchar *from, *to, *use, *certdata;
	gboolean past, future;

	gtk_tree_model_get(model, iter,
			CERT_COL_VALIDFROM, &from,
			CERT_COL_VALIDTO, &to,
			CERT_COL_USE, &use,
			CERT_COL_DESC, &certdata,
			CERT_COL_VALIDFROM_PAST, &past,
			CERT_COL_VALIDTO_FUTURE, &future,
			-1);
	newstringdata("certvalfromval", from);
	newstringdata("certvaltilval", to);
	newstringdata("certuseval", use);
	newstringdata("certdata", certdata);
	newbindata("certvalfromval:past", &past, sizeof(gboolean));
	newbindata("certvaltilval:future", &future, sizeof(gboolean));
}

static void update_info(GtkTreeSelection* sel, gpointer user_data G_GNUC_UNUSED) {
	gtk_tree_selection_selected_foreach(sel, update_info_detail, NULL);
}

static void setup_treeview() {
	GtkTreeView* tv = GTK_TREE_VIEW(gtk_builder_get_object(builder, "tv_cert"));
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *col;
	GtkTreeSelection* sel = gtk_tree_view_get_selection(tv);

	gtk_tree_view_set_model(tv, certificates_get_model());
	renderer = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes("label", renderer, "text", 0, NULL);
	gtk_tree_view_append_column(tv, col);

	g_signal_connect(G_OBJECT(sel), "changed", G_CALLBACK(update_info), NULL);
}

int main(int argc, char** argv) {
	GtkWidget *window;
	GtkAccelGroup *group;
	struct eid_vwr_ui_callbacks* cb;
	pthread_t thread;
	GError* err = NULL;

	/* The GNU implementation of setlocale() ignores whatever we
	 * specify if the LANGUAGE environment variable has a value, so
	 * ensure that it doesn't
	 */
	putenv("LANGUAGE=");
	bindtextdomain("eid-viewer", DATAROOTDIR "/locale");
	textdomain("eid-viewer");

	convert_set_lang(langfromenv());

	gtk_init(&argc, &argv);
	builder = gtk_builder_new();
	if(gtk_builder_add_from_string(builder, VIEWER_GLADE_STRING, strlen(VIEWER_GLADE_STRING), &err) == 0) {
		g_critical("Could not parse Glade XML: %s", err->message);
		exit(EXIT_FAILURE);
	}

	window = GTK_WIDGET(gtk_builder_get_object(builder, "mainwin"));
	group = gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(window), group);

	touched_labels = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);

	bindata_init();
	connect_signals(window);
	setup_treeview();

	sm_init();

	cb = eid_vwr_cbstruct();
	cb->newsrc = newsrc;
	cb->newstringdata = newstringdata;
	cb->newbindata = newbindata;
	cb->log = log_init();
	cb->newstate = newstate;
	eid_vwr_createcallbacks(cb);

	pthread_create(&thread, NULL, threadmain, NULL);

	gtk_widget_show(window);

	gtk_main();
}
