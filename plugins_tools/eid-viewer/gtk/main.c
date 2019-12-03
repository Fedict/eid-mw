#include <config.h>
#include <gtk/gtk.h>
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <eid-viewer/oslayer.h>
#include "viewer_glade.h"
#include "gettext.h"
#include "gtk_globals.h"

#include "gtkui.h"
#include "thread.h"
#include "photo.h"
#include "certs.h"
#include "state.h"
#include "glib_util.h"
#include "logging.h"
#include "prefs.h"
#include "print.h"
#include "gtk_main.h"

#if GTK_CHECK_VERSION(3, 96, 0)
#define gtk_style_context_get_color(ct, s, cr) gtk_style_context_get_color(ct, cr)
#define gtk_init(a, b) gtk_init()
#endif

#ifndef _
#define _(s) gettext(s)
#endif

typedef void(*bindisplayfunc)(const char*, const void*, int);
typedef void(*clearfunc)(char*);

static GHashTable* binhash;
static guint statusbar_context = 0;

GHashTable* touched_labels;
GtkBuilder* builder;

extern char** environ;

struct statusupdate {
	gboolean spin;
	char* text;
};

/* Helper function for uistatus() */
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

/* Update the status bar and the spinner with a new status message. */
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
	/* Don't change UI elements from a background thread */
	g_main_context_invoke(NULL, realstatus, update);
}

/* Handle "state changed" elements */
static void newstate(enum eid_vwr_states s) {
	GObject *open, *save, *print, *close, *pintest, *pinchg, *validate;
#define want_verify (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "validate_always"))))
	open = gtk_builder_get_object(builder, "mi_file_open");
	save = gtk_builder_get_object(builder, "mi_file_saveas");
	print = gtk_builder_get_object(builder, "mi_file_print");
	close = gtk_builder_get_object(builder, "mi_file_close");
	pintest = gtk_builder_get_object(builder, "pintestbut");
	pinchg = gtk_builder_get_object(builder, "pinchangebut");
	validate = gtk_builder_get_object(builder, "validate_now");

	g_object_set_threaded(open, "sensitive", (void*)FALSE, NULL);
	g_object_set_threaded(close, "sensitive", (void*)FALSE, NULL);
	g_object_set_threaded(print, "sensitive", (void*)FALSE, NULL);
	g_object_set_threaded(save, "sensitive", (void*)FALSE, NULL);
	g_object_set_threaded(pintest, "sensitive", (void*)FALSE, NULL);
	g_object_set_threaded(pinchg, "sensitive", (void*)FALSE, NULL);
	g_object_set_threaded(validate, "sensitive", (void*)FALSE, NULL);
	g_object_set_data_threaded(validate, "want_active", (void*)FALSE, NULL);
	switch(s) {
		case STATE_LIBOPEN:
		case STATE_CALLBACKS:
			uistatus(TRUE, _("Initialising"));
			return;
		case STATE_READY:
			uistatus(FALSE, _("Ready to read identity card"));
			g_object_set_threaded(open, "sensitive", (void*)TRUE, NULL);
			disable_dnd();
			return;
		case STATE_NO_READER:
			uistatus(FALSE, _("No cardreader found"));
			g_object_set_threaded(open, "sensitive", (void*)TRUE, NULL);
			disable_dnd();
			return;
		case STATE_TOKEN:
			uistatus(TRUE, _("Card available"));
			return;
		case STATE_TOKEN_IDLE:
			uistatus(FALSE, "");
			g_object_set_threaded(print, "sensitive", (void*)TRUE, NULL);
			g_object_set_threaded(save, "sensitive", (void*)TRUE, NULL);
			g_object_set_threaded(pintest, "sensitive", (void*)TRUE, NULL);
			g_object_set_threaded(pinchg, "sensitive", (void*)TRUE, NULL);
			g_object_set_data_threaded(validate, "want_active", (void*)TRUE, NULL);
			if(want_verify) {
				validate_all(NULL, NULL);
			} else {
				g_object_set_threaded(validate, "sensitive", (void*)TRUE, NULL);
			}
			setup_dnd();
			return;
		case STATE_TOKEN_ID:
			uistatus(TRUE, _("Reading identity"));
			return;
		case STATE_TOKEN_CERTS:
			uistatus(TRUE, _("Reading certificates"));
			return;
		case STATE_TOKEN_ERROR:
			uistatus(FALSE, _("Failed to read identity data"));
			return;
		case STATE_TOKEN_PINOP:
			uistatus(TRUE, _("Performing a PIN operation"));
			return;
		case STATE_FILE_WAIT:
			uistatus(FALSE, "");
			g_object_set_threaded(print, "sensitive", (void*)TRUE, NULL);
			g_object_set_threaded(close, "sensitive", (void*)TRUE, NULL);
			g_object_set_data_threaded(validate, "want_active", (void*)TRUE, NULL);
			if(want_verify) {
				validate_all(NULL, NULL);
			} else {
				g_object_set_threaded(validate, "sensitive", (void*)TRUE, NULL);
			}
			setup_dnd();
		default:
			return;
	}
}

/* Clear a string element from the UI */
static void stringclear(const char* l) {
	GtkLabel* label = GTK_LABEL(gtk_builder_get_object(builder, l));
	/* Should only appear in the hash table if we successfully found it
	   earlier... */
	assert(label != NULL);
	g_object_set_threaded(G_OBJECT(label), "label", "-", NULL);
	g_object_set_threaded(G_OBJECT(label), "sensitive", FALSE, NULL);
}

/* Add a new string to the UI */
static void newstringdata(const char* l, const char* data) {
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
	/* Remember which elements we've touched, so we can clear them
	 * again later on */
	g_hash_table_insert(touched_labels, g_strdup(l), stringclear);
	g_object_set_threaded(G_OBJECT(label), "label", g_strdup(data), g_free);
	g_object_set_threaded(G_OBJECT(label), "sensitive", (void*)TRUE, NULL);
}

/* Add a new binary data element to the UI */
static void newbindata(const char* label, const unsigned char* data, int datalen) {
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

/* Helper function for newsrc() */
static void cleardata(gpointer key, gpointer value, gpointer user_data G_GNUC_UNUSED) {
	clearfunc func = (clearfunc)value;
	char* k = key;
	func(k);
}

/* Handle the "new source" event from the state machine */
static void newsrc(enum eid_vwr_source src) {
	clear_certdata();
	g_hash_table_foreach(touched_labels, cleardata, NULL);
	if(src == EID_VWR_SRC_CARD) {
		g_object_set_threaded(G_OBJECT(gtk_builder_get_object(builder, "mainwin")), "urgency-hint", (void*)TRUE, NULL);
	}
	// TODO: update display so we see which source we're using
}

/* Figure out what language we should use from environment variables */
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

/* Connect UI event signals to callback functions */
static void connect_signals(GtkWidget* window) {
	GObject* signaltmp;
	GObject* photo;

	g_signal_connect(G_OBJECT(window), "delete-event", gtk_main_quit, NULL);
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "mi_file_open"));
	g_signal_connect(signaltmp, "activate", G_CALLBACK(file_open), NULL);
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "mi_file_saveas"));
	g_signal_connect(signaltmp, "activate", G_CALLBACK(file_save), NULL);
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "mi_file_reader_auto"));
	g_signal_connect(signaltmp, "toggled", G_CALLBACK(auto_reader), NULL);
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "mi_file_close"));
	g_signal_connect(signaltmp, "activate", G_CALLBACK(file_close), NULL);
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "mi_file_print"));
	g_signal_connect(signaltmp, "activate", G_CALLBACK(do_print), NULL);
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
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "mi_cert_detail"));
	g_signal_connect(signaltmp, "activate", G_CALLBACK(certdetail), NULL);
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "mi_cert_export_der"));
	g_signal_connect(signaltmp, "activate", G_CALLBACK(certexport), "DER");
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "mi_cert_export_pem"));
	g_signal_connect(signaltmp, "activate", G_CALLBACK(certexport), "PEM");
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "mi_cert_export_chain"));
	g_signal_connect(signaltmp, "activate", G_CALLBACK(certexport), "chain");
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "pintestbut"));
	g_signal_connect(signaltmp, "clicked", G_CALLBACK(pinop), (void*)EID_VWR_PINOP_TEST);
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "pinchangebut"));
	g_signal_connect(signaltmp, "clicked", G_CALLBACK(pinop), (void*)EID_VWR_PINOP_CHG);
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "photobox"));
	photo = G_OBJECT(gtk_builder_get_object(builder, "photo"));
	g_signal_connect(signaltmp, "drag-data-get", G_CALLBACK(drag_data_get), photo);
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "validate_now"));
	g_signal_connect(signaltmp, "clicked", G_CALLBACK(validate_all), NULL);
	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "validate_always"));
	g_signal_connect(signaltmp, "toggled", G_CALLBACK(validate_toggle), NULL);

	signaltmp = G_OBJECT(gtk_builder_get_object(builder, "cert_paned"));
	g_settings_bind(get_prefs(), "cert-paned-pos", signaltmp, "position", 0);
}

/* Set the colour of the "valid from" and "valid until" elements on the
 * Certificate page to red if they are incorrectly in the future or
 * past. */
static void show_date_state(char* label, void* data, int length G_GNUC_UNUSED) {
	gchar* labelname = g_strndup(label, strstr(label, ":") - label);
	GtkLabel* l = GTK_LABEL(gtk_builder_get_object(builder, labelname));
	PangoAttrList *attrs = pango_attr_list_new();
	PangoAttribute *attr;
	gboolean* is_invalid = (gboolean*)data;

	g_free(labelname);
	if(*is_invalid) {
		attr = pango_attr_foreground_new(G_MAXUINT16, 0, 0);
	} else {
		GdkRGBA color;
		GtkStyleContext *style = gtk_widget_get_style_context(GTK_WIDGET(l));

		gtk_style_context_get_color(style, GTK_STATE_FLAG_NORMAL, &color);
		attr = pango_attr_foreground_new(color.red * G_MAXUINT16, color.green * G_MAXUINT16, color.blue * G_MAXUINT16);
	}
	pango_attr_list_insert(attrs, attr);
	gtk_label_set_attributes(l, attrs);
}

static void toggleclear(const char* l) {
	GtkToggleButton* tb = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, l));
	gtk_toggle_button_set_active(tb, FALSE);
}

static void set_family(char* label G_GNUC_UNUSED, void* data G_GNUC_UNUSED, int length G_GNUC_UNUSED) {
	GtkCheckButton* cb = GTK_CHECK_BUTTON(gtk_builder_get_object(builder, "member_of_family"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb), TRUE);
	g_hash_table_insert(touched_labels, g_strdup("member_of_family"), toggleclear);
}

/* Initialize the hash table for binary data */
static void bindata_init() {
	binhash = g_hash_table_new(g_str_hash, g_str_equal);

	g_hash_table_insert(binhash, "PHOTO_FILE", displayphoto);
	g_hash_table_insert(binhash, "photo_hash", photohash);
	g_hash_table_insert(binhash, "CERT_RN_FILE", add_certificate);
	g_hash_table_insert(binhash, "Authentication", add_certificate);
	g_hash_table_insert(binhash, "CA", add_certificate);
	g_hash_table_insert(binhash, "Root", add_certificate);
	g_hash_table_insert(binhash, "Signature", add_certificate);
	g_hash_table_insert(binhash, "certvalfromval:past", show_date_state);
	g_hash_table_insert(binhash, "certvaltilval:future", show_date_state);
	g_hash_table_insert(binhash, "certimage", show_cert_image);
	g_hash_table_insert(binhash, "document_type_raw", update_doctype);
	g_hash_table_insert(binhash, "member_of_family", set_family);
}

/* Helper function for update_info() */
static void update_info_detail(GtkTreeModel* model, GtkTreePath *path G_GNUC_UNUSED, GtkTreeIter* iter, gpointer data G_GNUC_UNUSED) {
	gchar *from, *to, *use, *certdata, *trustval;
	gboolean past, future;
	GdkPixbuf *image;

	gtk_tree_model_get(model, iter,
			CERT_COL_VALIDFROM, &from,
			CERT_COL_VALIDTO, &to,
			CERT_COL_USE, &use,
			CERT_COL_DESC, &certdata,
			CERT_COL_VALIDFROM_PAST, &past,
			CERT_COL_VALIDTO_FUTURE, &future,
			CERT_COL_IMAGE, &image,
			CERT_COL_VALIDITY, &trustval,
			-1);
	newstringdata("certvalfromval", from);
	newstringdata("certvaltilval", to);
	newstringdata("certuseval", use);
	newstringdata("certdata", certdata);
	newstringdata("certtrustval", trustval);
	newbindata("certimage", (unsigned char*)image, -1);
	newbindata("certvalfromval:past", (unsigned char*)(&past), sizeof(gboolean));
	newbindata("certvaltilval:future", (unsigned char*)(&future), sizeof(gboolean));
}

/* Called when the user changes the selection of the treeview on the
 * certificates tab */
void update_info(GtkTreeSelection* sel, gpointer user_data G_GNUC_UNUSED) {
	gtk_tree_selection_selected_foreach(sel, update_info_detail, NULL);
}

/* Called when the user clicks on the treeview on the certificates tab */
static gboolean show_menu(GtkWidget* widget G_GNUC_UNUSED, GdkEvent* event, gpointer user_data G_GNUC_UNUSED) {
	GtkMenu* menu = GTK_MENU(gtk_builder_get_object(builder, "certmenu"));
	guint button;
	gdk_event_get_button(event, &button);
	if(button == 3) { // RMB click
#if GTK_CHECK_VERSION(3, 22, 0)
		gtk_menu_popup_at_pointer(menu, event);
#else
		gtk_menu_popup(menu, NULL, NULL, NULL, NULL, event->button.button, event->button.time);
#endif
	}
	return FALSE;
}

/* Initialize the treeview on the certificates tab: connect the model,
 * and connect signal handlers */
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
	g_signal_connect(G_OBJECT(tv), "button-press-event", G_CALLBACK(show_menu), NULL);
}

/* Main entry point */
int main(int argc, char** argv) {
	GtkWidget *window;
	GtkAccelGroup *group;
	struct eid_vwr_ui_callbacks* cb;
	GdkPixbuf *logo;
	GError* err = NULL;

	/* The GNU implementation of setlocale() ignores whatever we
	 * specify if the LANGUAGE environment variable has a value, so
	 * ensure that it doesn't
	 */
	putenv("LANGUAGE=");
	bindtextdomain("eid-viewer", DATAROOTDIR "/locale");
	textdomain("eid-viewer");

	eid_vwr_convert_set_lang(langfromenv());

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

	certs_init();

	cb = eid_vwr_cbstruct();
	cb->newsrc = newsrc;
	cb->newstringdata = newstringdata;
	cb->newbindata = newbindata;
	cb->logv = ui_log_init();
	cb->newstate = newstate;
	cb->pinop_result = pinop_result;
	cb->readers_changed = readers_changed;
	eid_vwr_createcallbacks(cb);

	eid_vwr_init_crypto();

	gtk_window_set_default_icon_name("eid-viewer");

	gtk_widget_show(window);

	if(argc > 1) {
		eid_vwr_be_deserialize(argv[1]);
	}

	gtk_main();

	return 0;
}
