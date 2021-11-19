#include <config.h>
#include "gtkui.h"
#include "gettext.h"
#include "gtk_globals.h"
#include "gtk_main.h"
#include <eid-viewer/oslayer.h>
#include "state.h"
#include "certs.h"
#include "logging.h"
#include "thread.h"
#include <eid-util/labels.h>

#include <libxml/xmlreader.h>
#include "viewer_glade.h"

#include <locale.h>
#include <stdlib.h>

gboolean is_foreigner = FALSE;

#ifdef _
#error _ must not be defined
#endif
#define _(s) gettext(s)
#define N_(s) gettext_noop(s)

#define GEN_FUNC(n, d) \
void n(GtkWidget* item G_GNUC_UNUSED, gpointer user_data) { \
	GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder, "mainwin")); \
	GtkWidget* dlg = gtk_message_dialog_new(window, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, d " (not yet implemented)", (gchar*)user_data); \
	gtk_dialog_run(GTK_DIALOG(dlg)); \
	gtk_widget_destroy(dlg); \
}

#if !GTK_CHECK_VERSION(3,22,0)
#define gtk_show_uri_on_window(parent,uri,timestamp,error) gtk_show_uri(gtk_widget_get_screen(GTK_WIDGET(parent)),uri,timestamp,error)
#endif
#if GTK_CHECK_VERSION(3, 96, 0)
#define gtk_image_set_from_icon_name(i, n, s) gtk_image_set_from_icon_name(i, n)
#endif

static enum eid_vwr_langs curlang = EID_VWR_LANG_NONE;

/* Hide the certificate state icon */
static void clear_cert_image(char* label) {
	GtkImage *img = GTK_IMAGE(gtk_builder_get_object(builder, label));
	gtk_image_set_from_icon_name(img, "image-missing", GTK_ICON_SIZE_BUTTON);
}

/* Show the correct certificate state icon. TODO: make this match what the
 * certificate validation returns */
void show_cert_image(const char* label, void *data, int len G_GNUC_UNUSED) {
	GdkPixbuf *buf = GDK_PIXBUF(data);
	GtkImage *ci = GTK_IMAGE(gtk_builder_get_object(builder, "certimage"));

	g_hash_table_insert(touched_labels, g_strdup(label), clear_cert_image);

	gtk_image_set_from_pixbuf(ci, buf);
}

/* Show an "about" dialog */
void showabout(GtkWidget* about G_GNUC_UNUSED, gpointer user_data G_GNUC_UNUSED) {
	GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder, "mainwin"));
	const char *authors[] = { "Wouter Verhelst", "Frederik Vernelen", NULL };
	const char *artists[] = { "Frank Mariën", NULL };
	gtk_show_about_dialog(window,
			"program-name", _("eID Viewer"),
			"version", PACKAGE_VERSION,
			"website", "http://eid.belgium.be/",
			"authors", authors,
			"copyright", _("Copyright 2015-2016 Fedict\nCopyright 2017-2018 BOSA"),
			"comments", _("View data on the Belgian eID card"),
			"license-type", GTK_LICENSE_LGPL_3_0,
			"artists", artists,
			NULL);
}

/* Show a preview image for the selected file (if any, and if that is a valid .eid file) */
static void update_preview(GtkFileChooser* chooser, gpointer data) {
	GtkWidget* preview;
	char *filename;
	GdkPixbuf *pixbuf;
	GInputStream* mstream;
	struct eid_vwr_preview* prv;

	preview = GTK_WIDGET(data);
	filename = gtk_file_chooser_get_preview_filename(chooser);
	prv = eid_vwr_get_preview(filename);
	g_free(filename);

	if(!prv->have_data) {
		gtk_file_chooser_set_preview_widget_active(chooser, FALSE);
		free(prv);
		return;
	}

	mstream = G_INPUT_STREAM(g_memory_input_stream_new_from_data(prv->imagedata, prv->imagelen, NULL));
	pixbuf = gdk_pixbuf_new_from_stream(mstream, NULL, NULL);
	gtk_image_set_from_pixbuf(GTK_IMAGE(preview), pixbuf);
	g_object_unref(pixbuf);
	free(prv->imagedata);
	free(prv);
	gtk_file_chooser_set_preview_widget_active(chooser, TRUE);
}

GEN_FUNC(open_file_detail, "opening %s")

/* Show an "open file" dialog, and make the backend open it if the user accepted the selection */
void file_open(GtkWidget* item G_GNUC_UNUSED, gpointer user_data G_GNUC_UNUSED) {
	GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder, "mainwin"));
	GtkWidget* dialog = gtk_file_chooser_dialog_new(
			_("Open eID file"), window, GTK_FILE_CHOOSER_ACTION_OPEN,
			_("_Cancel"), GTK_RESPONSE_CANCEL,
			_("_Open"), GTK_RESPONSE_ACCEPT,
			NULL);
	GtkWidget* preview;
	gchar* filename;
	gint res;
	GtkFileFilter* filter;

	filter = gtk_file_filter_new();
	gtk_file_filter_add_pattern(filter, "*");
	gtk_file_filter_set_name(filter, _("All files"));
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

	filter = gtk_file_filter_new();
	gtk_file_filter_add_pattern(filter, "*.eid");
	gtk_file_filter_add_pattern(filter, "*.csv");
	gtk_file_filter_set_name(filter, _("All eID files"));
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
	gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), filter);

	filter = gtk_file_filter_new();
	gtk_file_filter_add_pattern(filter, "*.eid");
	gtk_file_filter_set_name(filter, _("eID files"));
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

	preview = gtk_image_new();
	gtk_file_chooser_set_preview_widget(GTK_FILE_CHOOSER(dialog), preview);
	g_signal_connect(G_OBJECT(dialog), "update-preview", G_CALLBACK(update_preview), preview);

	res = gtk_dialog_run(GTK_DIALOG(dialog));
	if(res == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		eid_vwr_be_deserialize(filename);
		g_free(filename);
	}
	gtk_widget_destroy(dialog);
}

/* Show a "save file" dialog, and make the backend save the XML data there if the user accepted the selection */
void file_save(GtkWidget* item G_GNUC_UNUSED, gpointer user_data G_GNUC_UNUSED) {
	GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder, "mainwin"));
	GtkWidget* dialog = gtk_file_chooser_dialog_new(
			_("Save eID file"), window, GTK_FILE_CHOOSER_ACTION_SAVE,
			_("_Cancel"), GTK_RESPONSE_CANCEL,
			_("_Save"), GTK_RESPONSE_ACCEPT,
			NULL);
	gchar* filename;
	gchar* filename_sugg;
	gint res;
	GtkFileFilter* filter;

	filter = gtk_file_filter_new();
	gtk_file_filter_add_pattern(filter, "*");
	gtk_file_filter_set_name(filter, _("All files"));
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

	filter = gtk_file_filter_new();
	gtk_file_filter_add_pattern(filter, "*.eid");
	gtk_file_filter_set_name(filter, _("eID files"));
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
	gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), filter);

	filename_sugg = g_strdup_printf("%s.eid", gtk_label_get_text(GTK_LABEL(gtk_builder_get_object(builder, "national_number"))));
	gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), filename_sugg);

	res = gtk_dialog_run(GTK_DIALOG(dialog));
	if(res == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		eid_vwr_be_serialize(filename);
		g_free(filename);
	}
	gtk_widget_destroy(dialog);
	g_free(filename_sugg);
}

/* Close the currently-open file */
void file_close(GtkWidget* item G_GNUC_UNUSED, gpointer user_data G_GNUC_UNUSED) {
	eid_vwr_close_file();
}

/* Perform a PIN operation */
void pinop(GtkWidget* button G_GNUC_UNUSED, gpointer which) {
	enum eid_vwr_pinops op = (enum eid_vwr_pinops)(uintptr_t)which;

	eid_vwr_pinop(op);
}

gboolean showdlg(gpointer d) {
	GtkDialog *dlg = GTK_DIALOG(d);
	gtk_dialog_run(dlg);
	gtk_widget_destroy(GTK_WIDGET(dlg));

	return FALSE;
}

void pinop_result(enum eid_vwr_pinops which, enum eid_vwr_result r) {
	static char* msgs[2][3] = {
		{ N_("PIN incorrect"), N_("PIN OK"), N_("Error occurred while trying to test PIN code") },
		{ N_("PIN change failed"), N_("PIN changed"), N_("Error occurred while trying to change PIN code") }
	};
	GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder, "mainwin"));
	GtkWidget* dlg = gtk_message_dialog_new(window, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, gettext(msgs[which][r]), NULL);
	g_main_context_invoke(NULL, showdlg, dlg);
}

/* Get the "current" language, or the default language if none is selected */
enum eid_vwr_langs get_curlang() {
	if(curlang != EID_VWR_LANG_NONE) {
		return curlang;
	}
	return langfromenv();
}

/* Re-translate the user interface if the user switches language.
   TODO: will we still need this? */
static void retranslate_gtkui() {
	xmlTextReaderPtr reader = xmlReaderForMemory(VIEWER_GLADE_STRING, sizeof(VIEWER_GLADE_STRING)-1, "", NULL, 0);
	const xmlChar *curnode = NULL, *curname = NULL;
	int rc;

	if(!reader) {
		printf("whoops\n");
		return;
	}
	while((rc = xmlTextReaderRead(reader)) > 0) {
		curnode = xmlTextReaderConstLocalName(reader);

		if(xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
			continue;
		}

		if(!strcmp((char*)curnode, "object")) {
			if(xmlTextReaderHasAttributes(reader) > 0) {
				xmlChar* val = xmlTextReaderGetAttribute(reader, (const xmlChar*)"id");
				if(val) {
					curname = val;
				}
			}
		}
		if(curname != NULL && !strcmp((char*)curnode, "property")) {
			if(xmlTextReaderHasAttributes(reader) > 0) {
				xmlChar *trans = xmlTextReaderGetAttribute(reader, (const xmlChar*)"translatable");
				xmlChar *prop = xmlTextReaderGetAttribute(reader, (const xmlChar*)"name");
				const xmlChar *label;
				xmlTextReaderRead(reader);
				label = xmlTextReaderConstValue(reader);
				if(trans && !strcmp((char*)trans, "yes")) {
					g_object_set(G_OBJECT(gtk_builder_get_object(builder, (char*)curname)), (char*)prop, _((char*)label), NULL);
				}
			}
		}
	}
}

/* Translate the UI to the target language */
void translate(GtkWidget* item G_GNUC_UNUSED, gpointer target) {
	enum eid_vwr_langs lang = EID_VWR_LANG_EN;
	if(!strncmp(target, "de", 2)) {
		lang = EID_VWR_LANG_DE;
	} else if(!strncmp(target, "fr", 2)) {
		lang = EID_VWR_LANG_FR;
	} else if(!strncmp(target, "nl", 2)) {
		lang = EID_VWR_LANG_NL;
	}
	eid_vwr_convert_set_lang(lang);
	curlang = lang;
	setlocale(LC_MESSAGES, target);

	retranslate_gtkui();
}

/* Initialize the drag-and-drop environment */
static gboolean setup_dnd_real(gpointer foo G_GNUC_UNUSED) {
	GtkWidget* photo = GTK_WIDGET(gtk_builder_get_object(builder, "photobox"));

#if GTK_CHECK_VERSION(3, 96, 0)
	gtk_drag_source_set(photo, GDK_BUTTON1_MASK, NULL, GDK_ACTION_COPY);
#else
	gtk_drag_source_set(photo, GDK_BUTTON1_MASK, NULL, 0, GDK_ACTION_COPY);
#endif
	gtk_drag_source_add_text_targets(photo);

	return FALSE;
}

void setup_dnd(void) {
	g_main_context_invoke(NULL, setup_dnd_real, NULL);
}

/* Disable drag-and-drop when we no longer have data */
static gboolean disable_dnd_real(gpointer foo G_GNUC_UNUSED) {
	GtkWidget* pbox = GTK_WIDGET(gtk_builder_get_object(builder, "photobox"));

	gtk_drag_source_unset(pbox);

	return FALSE;
}

void disable_dnd(void) {
	g_main_context_invoke(NULL, disable_dnd_real, NULL);
}

/* Perform a drag-and-drop operation */
#if GTK_CHECK_VERSION(4, 0, 0)
GdkContentProvider * drag_data_get(GtkDragSource *dragsrc, double x, double y, gpointer user_data) {
	const char *xml = eid_vwr_be_get_xmlform();
	if(!xml) return NULL;
	return gdk_content_provider_new_for_bytes("application/xml", g_bytes_new(xml, strlen(xml)+1));
}
#else
void drag_data_get(GtkWidget* widget G_GNUC_UNUSED, GdkDragContext *ctx G_GNUC_UNUSED, GtkSelectionData *data, guint info G_GNUC_UNUSED, guint time G_GNUC_UNUSED, gpointer user_data G_GNUC_UNUSED) {
	const char* xml = eid_vwr_be_get_xmlform();
	if(!xml) return;
	gtk_selection_data_set_text(data, xml, -1);
}
#endif

void validate_toggle(gpointer event_source, gpointer user_data G_GNUC_UNUSED) {
	// g_object_get_data returns NULL if we haven't set this yet. That's
	// fine, just means we'll drop to the false case which disables things,
	// which is exactly what we want at startup
	GtkWidget* valbut = GTK_WIDGET(gtk_builder_get_object(builder, "validate_now"));
	gtk_widget_set_sensitive(valbut, FALSE);
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(event_source))) {
		validate_all(NULL, NULL);
	} else {
		if(g_object_get_data(G_OBJECT(valbut), "want_active") != NULL) {
			gtk_widget_set_sensitive(valbut, TRUE);
		}
	}
}

void showurl(GtkWidget *item G_GNUC_UNUSED, gpointer user_data) {
	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "mainwin"));
	if(strcmp((gchar*)user_data, "faq") == 0) {
		gtk_show_uri_on_window(GTK_WINDOW(window), _("https://eid.belgium.be/en/question-and-answer"), GDK_CURRENT_TIME, NULL);
	} else {
		gtk_show_uri_on_window(GTK_WINDOW(window), _("https://iamapps.belgium.be/tma/"), GDK_CURRENT_TIME, NULL);
	}
}

void auto_reader(GtkCheckButton *mi, gpointer user_data G_GNUC_UNUSED) {
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(mi))) {
		eid_vwr_be_select_slot(1, 0);
	}
}

static void manual_reader(GtkCheckMenuItem *mi, gpointer slotptr) {
	intptr_t slot = (intptr_t)slotptr;

	if(gtk_check_menu_item_get_active(mi)) {
		eid_vwr_be_select_slot(0, slot);
	}
}

struct rdri {
	unsigned long nreaders;
	slotdesc* slots;
};

static gboolean readers_changed_real(gpointer user_data) {
	unsigned long i;
	static GtkWidget** items = NULL;
	struct rdri* info = (struct rdri*) user_data;

	if(items) {
		for(i=0; items[i]!=NULL; i++) {
			gtk_widget_destroy(GTK_WIDGET(items[i]));
		}
		free(items);
	}
	GtkRadioButton* automatic = GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "reader_auto"));
	if(info->nreaders == 0) {
		gtk_widget_set_sensitive(GTK_WIDGET(automatic), FALSE);
		return FALSE;
	}
	gtk_widget_set_sensitive(GTK_WIDGET(automatic), TRUE);
	GtkBox *box = GTK_BOX(gtk_builder_get_object(builder, "box_cardreaders"));
	items = malloc(sizeof(GtkWidget*) * (info->nreaders + 1));
	for(i=0; i<info->nreaders; i++) {
		items[i] = gtk_radio_button_new_with_label_from_widget(automatic, (char*)info->slots[i].description);
		gtk_container_add(box, items[i]);
		g_signal_connect(G_OBJECT(items[i]), "toggled", G_CALLBACK(manual_reader), (void*)info->slots[i].slot);
		gtk_widget_show(GTK_WIDGET(items[i]));
		free(info->slots[i].description);
	}
	items[info->nreaders] = NULL;
	free(info->slots);
	free(info);
	return FALSE;
}

void readers_changed(unsigned long nreaders, slotdesc* slots) {
	struct rdri* data = malloc(sizeof(struct rdri));
	unsigned long i;
	data->slots = malloc(sizeof(slotdesc) * nreaders);
	memcpy(data->slots, slots, sizeof(slotdesc)*nreaders);
	for(i=0; i<nreaders; i++) {
		if(slots[i].description != NULL) {
			data->slots[i].description = strdup(slots[i].description);
		} else {
			data->slots[i].description = NULL;
		}
	}
	data->nreaders = nreaders;
	g_main_context_invoke(NULL, readers_changed_real, data);
	g_message("readers changed");
}

void update_doctype(char* label G_GNUC_UNUSED, void* data, int length) {
	static char doctype[2];
	char* newtype = (char*)data;
	char b0, b1;
	if(length == 1) {
		b0 = ' ';
		b1 = *newtype;
	} else {
		b0 = newtype[0];
		b1 = newtype[1];
	}
	if(b0 != doctype[0] || b1 != doctype[1]) {
		struct labelnames* toggles = get_foreigner_labels();
		int i;

		doctype[0] = b0; doctype[1] = b1;

		if((doctype[0] == '0' || doctype[0] == ' ') && doctype[1] == '1') {
			is_foreigner = FALSE;
		} else {
			is_foreigner = TRUE;
		}
		for(i=0; i<toggles->len; i++) {
			GValue *show_data, *show_label;
			gchar* titlelabel;
			GObject* obj = gtk_builder_get_object(builder, toggles->label[i]);
			if(!obj) {
				continue;
			}
			titlelabel = g_strdup_printf("title_%s", toggles->label[i]);
			show_data = calloc(sizeof(GValue), 1);
			g_value_init(show_data, G_TYPE_BOOLEAN);
			g_value_set_boolean(show_data, is_foreigner);
			show_label = calloc(sizeof(GValue), 1);
			g_value_init(show_label, G_TYPE_BOOLEAN);
			g_value_set_boolean(show_label, is_foreigner);
			g_object_set_threaded_gvalue(obj, "visible", show_data, free);
			g_object_set_threaded_gvalue(gtk_builder_get_object(builder, titlelabel), "visible", show_label, free);
			g_free(titlelabel);
		}
	}
}
