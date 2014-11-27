#include <config.h>

#include <string.h>

#include <gtk/gtk.h>
#include <about_glade.h>

#ifndef _
#define _(s) (s)
#endif

int main(int argc, char** argv) {
	GtkBuilder* builder;
	GtkWidget *window, *treeview;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn* col;
	GtkTreeIter iter;
	GtkListStore *store;

	gtk_init(&argc, &argv);

	builder = gtk_builder_new();
	gtk_builder_add_from_string(builder, ABOUT_GLADE_STRING, strlen(ABOUT_GLADE_STRING), NULL);
	window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
	gtk_widget_show_all(window);

	g_signal_connect(G_OBJECT(window), "delete-event", gtk_main_quit, NULL);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(builder, "quitbtn")), "clicked", gtk_main_quit, NULL);

	treeview = GTK_WIDGET(gtk_builder_get_object(builder, "treeview"));

	store = GTK_LIST_STORE(gtk_builder_get_object(builder, "infodata"));

	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, 0, _("Middleware version"), 1, PACKAGE_VERSION, -1);

	renderer = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes(_("Item"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col);
	col = gtk_tree_view_column_new_with_attributes(_("Value"), renderer, "text", 1, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col);

	gtk_main();
}
