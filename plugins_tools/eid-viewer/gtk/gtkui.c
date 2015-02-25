#include "gtkui.h"
#include "gtk_globals.h"

#ifndef _
#define _(s) gettext(s)
#endif

void showabout(GtkMenuItem* about, gpointer user_data G_GNUC_UNUSED) {
	GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder, "mainwin"));
	gtk_show_about_dialog(window, "program-name", _("eID Viewer"), NULL);
}

#define GEN_FUNC(n, d) \
void n(GtkMenuItem* item, gpointer user_data) { \
	GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder, "mainwin")); \
	GtkWidget* dlg = gtk_message_dialog_new(window, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, d " (not yet implemented)", user_data); \
	gtk_dialog_run(GTK_DIALOG(dlg)); \
	gtk_widget_destroy(dlg); \
}

GEN_FUNC(file_open, "open file")
GEN_FUNC(file_save, "save %s file")
GEN_FUNC(file_close, "close file")
GEN_FUNC(file_prefs, "set preferences")
GEN_FUNC(file_print, "print")
GEN_FUNC(translate, "translate to %s")
GEN_FUNC(showurl, "show %s url")
GEN_FUNC(showlog, "show log tab")
GEN_FUNC(testpin, "test pin")
GEN_FUNC(changepin, "change pin")
