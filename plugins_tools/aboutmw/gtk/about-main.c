#include <config.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/utsname.h>

#include <gtk/gtk.h>

#include "about_glade.h"
#include "now.h"

#ifndef _
#define _(s) (s)
#endif

static enum {
	BITS_UNKNOWN,
	BITS_32,
	BITS_64,
	BITS_FOREIGN,
} bitness;

void do_uname(GtkWidget* top, GtkListStore* data) {
	GtkTreeIter iter;
	struct utsname undat;
	char* values;

	if(uname(&undat) < 0) {
		GtkWidget* dialog = gtk_message_dialog_new(GTK_WINDOW(top), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "uname: %s", strerror(errno));
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
	}

	gtk_list_store_append(data, &iter);
	if(!strcmp(undat.machine, "x86_64")) {
		bitness = BITS_64;
		gtk_list_store_set(data, &iter, 0, _("System architecture"), 1, _("64-bit PC"), -1);
	} else if(undat.machine[0] == 'i' && undat.machine[2] == '8' && undat.machine[3] == '6') {
		bitness = BITS_32;
		gtk_list_store_set(data, &iter, 0, _("System architecture"), 1, _("32-bit PC"), -1);
	} else {
		bitness = BITS_FOREIGN;
		asprintf(&values, _("Unknown (%s)"), undat.machine);
		gtk_list_store_set(data, &iter, 0, _("System architecture"), 1, values, -1);
		free(values);
	}

	gtk_list_store_append(data, &iter);
	asprintf(&values, "%s %s %s %s %s", undat.sysname, undat.nodename, undat.release, undat.version, undat.machine);
	gtk_list_store_set(data, &iter, 0, _("uname"), 1, values, -1);
	free(values);
}

int main(int argc, char** argv) {
	GtkBuilder* builder;
	GtkWidget *window, *treeview;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn* col;
	GtkTreeIter iter;
	GtkListStore *store;
	gchar *tmp, *loc;

	gtk_init(&argc, &argv);

	builder = gtk_builder_new();
	gtk_builder_add_from_string(builder, ABOUT_GLADE_STRING, strlen(ABOUT_GLADE_STRING), NULL);
	window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
	gtk_widget_show_all(window);

	g_signal_connect(G_OBJECT(window), "delete-event", gtk_main_quit, NULL);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(builder, "quitbtn")), "clicked", gtk_main_quit, NULL);

	treeview = GTK_WIDGET(gtk_builder_get_object(builder, "treeview"));

	store = GTK_LIST_STORE(gtk_builder_get_object(builder, "infodata"));

	tmp = g_strdup(PACKAGE_VERSION);
	loc = strchr(tmp, '-');
	*loc = '\0';
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, 0, _("Middleware version"), 1, tmp, -1);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, 0, _("Middleware build ID"), 1, loc+1, -1);

	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, 0, _("Middleware build date"), 1, EID_NOW_STRING, -1);

	do_uname(window, store);

	renderer = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes(_("Item"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col);
	col = gtk_tree_view_column_new_with_attributes(_("Value"), renderer, "text", 1, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col);

	gtk_main();
}
