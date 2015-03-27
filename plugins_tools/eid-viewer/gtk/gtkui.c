#include <config.h>
#include "gtkui.h"
#include "gettext.h"
#include "gtk_globals.h"
#include "gtk_main.h"
#include "oslayer.h"
#include "state.h"

#include <locale.h>
#include <stdlib.h>

#ifdef _
#error _ must not be defined
#endif
#define _(s) gettext(s)

#define GEN_FUNC(n, d) \
void n(GtkMenuItem* item, gpointer user_data) { \
	GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder, "mainwin")); \
	GtkWidget* dlg = gtk_message_dialog_new(window, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, d " (not yet implemented)", (gchar*)user_data); \
	gtk_dialog_run(GTK_DIALOG(dlg)); \
	gtk_widget_destroy(dlg); \
}

static enum eid_vwr_langs curlang = EID_VWR_LANG_NONE;

void show_cert_image(const char* label, void *data, int len) {
	GdkPixbuf *buf = GDK_PIXBUF(data);
	GtkImage *ci = GTK_IMAGE(gtk_builder_get_object(builder, "certimage"));

	gtk_image_set_from_pixbuf(ci, buf);
}

void showabout(GtkMenuItem* about, gpointer user_data G_GNUC_UNUSED) {
	GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder, "mainwin"));
	const char *authors[] = { "Wouter Verhelst", "Frederik Vernelen", NULL };
	const char *artists[] = { "Frank Mariën", NULL };
	gtk_show_about_dialog(window,
			"program-name", _("eID Viewer"),
			"version", PACKAGE_VERSION,
			"website", "http://eid.belgium.be/",
			"authors", authors,
			"copyright", _("Copyright 2015 Fedict"),
			"comments", _("View data on the Belgian eID card"),
#if (HAVE_GTK == 3)
			"license-type", GTK_LICENSE_LGPL_3_0,
#else
			"license", _("This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.\n\nThis program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.\n\nYou should have received a copy of the GNU General Public License along with this program.  If not, see http://www.gnu.org/licenses/."),
			"wrap-license", TRUE,
#endif
			"artists", artists,
			NULL);
}

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

void file_open(GtkMenuItem* item, gpointer user_data) {
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
	gtk_file_filter_set_name(filter, _("XML eID files"));
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

	filter = gtk_file_filter_new();
	gtk_file_filter_add_pattern(filter, "*.csv");
	gtk_file_filter_set_name(filter, _("CSV eID files"));
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

	preview = gtk_image_new();
	gtk_file_chooser_set_preview_widget(GTK_FILE_CHOOSER(dialog), preview);
	g_signal_connect(G_OBJECT(dialog), "update-preview", G_CALLBACK(update_preview), preview);

	res = gtk_dialog_run(GTK_DIALOG(dialog));
	if(res == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		sm_handle_event(EVENT_OPEN_FILE, filename, g_free, NULL);
	}
	gtk_widget_destroy(dialog);
}

void file_save(GtkMenuItem* item, gpointer user_data) {
	GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder, "mainwin"));
	GtkWidget* dialog = gtk_file_chooser_dialog_new(
			strcmp(user_data, "xml") ? _("Save CSV eID file") : _("Save XML eID file"), window, GTK_FILE_CHOOSER_ACTION_SAVE,
			_("_Cancel"), GTK_RESPONSE_CANCEL,
			_("_Save"), GTK_RESPONSE_ACCEPT,
			NULL);
	gchar* filename;
	gint res;
	GtkFileFilter* filter;

	filter = gtk_file_filter_new();
	gtk_file_filter_add_pattern(filter, "*");
	gtk_file_filter_set_name(filter, _("All files"));
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

	filter = gtk_file_filter_new();
	if(strcmp(user_data, "xml")) {
		gtk_file_filter_add_pattern(filter, "*.csv");
		gtk_file_filter_set_name(filter, _("CSV eID files"));
	} else {
		gtk_file_filter_add_pattern(filter, "*.eid");
		gtk_file_filter_set_name(filter, _("XML eID files"));
	}
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
	gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), filter);

	res = gtk_dialog_run(GTK_DIALOG(dialog));
	if(res == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		sm_handle_event(EVENT_SERIALIZE, filename, g_free, NULL);
	}
	gtk_widget_destroy(dialog);
}

void file_close(GtkMenuItem* item, gpointer user_data) {
	sm_handle_event(EVENT_CLOSE_FILE, NULL, NULL, NULL);
}

void pinop(GtkWidget* button, gpointer which) {
	enum eid_vwr_pinops op = (enum eid_vwr_pinops) which;

	eid_vwr_pinop(op);
}

enum eid_vwr_langs get_curlang() {
	if(curlang != EID_VWR_LANG_NONE) {
		return curlang;
	}
	return langfromenv();
}

void translate(GtkMenuItem* item, gpointer target) {
	enum eid_vwr_langs lang = EID_VWR_LANG_EN;
	GtkWidget *widget;
	if(!strncmp(target, "de", 2)) {
		lang = EID_VWR_LANG_DE;
	} else if(!strncmp(target, "fr", 2)) {
		lang = EID_VWR_LANG_FR;
	} else if(!strncmp(target, "nl", 2)) {
		lang = EID_VWR_LANG_NL;
	}
	convert_set_lang(lang);
	curlang = lang;
	setlocale(LC_MESSAGES, target);

	// TODO: it would be more efficient if the below was parsed from the
	// Glad XML data rather than be re-encoded here...
#define TSL_LABEL(l, t) { widget = GTK_WIDGET(gtk_builder_get_object(builder, l)); gtk_label_set_text(GTK_LABEL(widget), t); }
	TSL_LABEL("tab_id", _("Identity"));
	TSL_LABEL("tab_cardpin", _("Card/PIN"));
	TSL_LABEL("tab_certs", _("Certificates"));
	TSL_LABEL("nametit", _("Name:"));
	TSL_LABEL("giventit", _("Given names:"));
	TSL_LABEL("pobtit", _("Place of birth:"));
	TSL_LABEL("dobtit", _("Date of birth:"));
	TSL_LABEL("sextit", _("Sex:"));
	TSL_LABEL("natnumtit", _("National number:"));
	TSL_LABEL("nattit", _("Nationality:"));
	TSL_LABEL("tittit", _("Title:"));
	TSL_LABEL("spectit", _("Special status:"));
	TSL_LABEL("streettit", _("Street:"));
	TSL_LABEL("posttit", _("Postal code:"));
	TSL_LABEL("munictit", _("Municipality:"));
	TSL_LABEL("cardnumtit", _("Card number:"));
	TSL_LABEL("issuetit", _("Place of issue:"));
	TSL_LABEL("chipnumtit", _("Chip number:"));
	TSL_LABEL("cardvalfromtit", _("Valid from:"));
	TSL_LABEL("cardvaltiltit", _("Valid until:"));
	TSL_LABEL("certvalfromtit", _("Valid from:"));
	TSL_LABEL("certvaltiltit", _("Valid until:"));
	TSL_LABEL("certusetit", _("Use:"));
	TSL_LABEL("certtrusttit", _("Trust:"));
#undef TSL_LABEL
}

GEN_FUNC(file_prefs, "set preferences")
GEN_FUNC(showurl, "show %s url")
