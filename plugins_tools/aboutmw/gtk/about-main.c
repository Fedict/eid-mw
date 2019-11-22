/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2014 FedICT.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */

#define _GNU_SOURCE

#include <config.h>

#define _GNU_SOURCE

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/utsname.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "about_glade.h"
#include "now.h"

#include "gettext.h"

#if GTK_CHECK_VERSION(3, 96, 0)
#define GtkClipboard GdkClipboard
#define gtk_clipboard_get(a) gdk_display_get_clipboard(gdk_display_get_default())
#define gtk_clipboard_set_text(c, t, l) gdk_clipboard_set_text(c, t)
#define gtk_init(a, b) gtk_init()

void gtk_widget_show_all_ll(GtkWidget *widget, gpointer data G_GNUC_UNUSED) {
	if(GTK_IS_CONTAINER(widget)) {
		gtk_container_foreach(GTK_CONTAINER(widget), gtk_widget_show_all_ll, NULL);
	}
	gtk_widget_show(widget);
}

void gtk_widget_show_all(GtkWidget *widget) {
	gtk_widget_show_all_ll(widget, NULL);
}
#endif

#ifndef _
#define _(s) gettext(s)
#endif

static enum _bits {
	BITS_UNKNOWN,
	BITS_32,
	BITS_64,
	BITS_FOREIGN,
} bitness;

void check_pcsc(GtkWidget* top G_GNUC_UNUSED, GtkListStore* data) {
	FILE* f = popen("pidof pcscd", "r");
	GtkTreeIter iter;
	char pid[6];
	char* tmp;

	gtk_list_store_append(data, &iter);
	if(!f) {
		gtk_list_store_set(data, &iter, 0, _("PCSC daemon status"), 1, _("(check failed)"), -1);
		return;
	}
	if(feof(f)) {
		gtk_list_store_set(data, &iter, 0, _("PCSC daemon status"), 1, _("(not running)"), -1);
		goto exit;
	}
	pid[5]='\0';
	if(fgets(pid, 5, f)==NULL) {
		gtk_list_store_set(data, &iter, 0, _("PCSC daemon status"), 1, _("(not running)"), -1);
		goto exit;
	}
	if((tmp = strchr(pid, '\n'))) {
		*tmp = '\0';
	}
	if(strlen(pid)==0) {
		gtk_list_store_set(data, &iter, 0, _("PCSC daemon status"), 1, _("(not running)"), -1);
		goto exit;
	}
	tmp = g_strdup_printf(_("running; pid: %s"), pid);
	gtk_list_store_set(data, &iter, 0, _("PCSC daemon status"), 1, tmp, -1);
	g_free(tmp);
exit:
	pclose(f);
}

void do_viewer(GtkWidget* top G_GNUC_UNUSED, GtkListStore* data) {
	FILE* f = popen("which eid-viewer", "r");
	GtkTreeIter iter;
	char tmp[PATH_MAX];
	char* loc;

	gtk_list_store_append(data, &iter);
	if(!f) {
		gtk_list_store_set(data, &iter, 0, _("eID Viewer location"), 1, _("(not found)"), -1);
		return;
	}
	tmp[PATH_MAX-1]='\0';
	if(fgets(tmp, PATH_MAX, f) == NULL) {
		gtk_list_store_set(data, &iter, 0, _("eID Viewer location"), 1, _("(not found)"), -1);
		goto exit;
	}
	if((loc = strrchr(tmp, '\n')) != NULL) {
		*loc = '\0';
	}
	if(strlen(tmp) == 0) {
		gtk_list_store_set(data, &iter, 0, _("eID Viewer location"), 1, _("(not found)"), -1);
		goto exit;
	}
	gtk_list_store_set(data, &iter, 0, _("eID Viewer location"), 1, tmp, -1);
exit:
	pclose(f);
}

void do_files(GtkWidget* top, GtkListStore* data) {
	GtkWidget* dialog;
	struct stat st;
	struct {
		enum _bits bitness;
		char* loc;
	} locs[] = {
		{ BITS_32, "/usr/lib/libbeidpkcs11.so.0" },
		{ BITS_32, "/usr/lib/i386-linux-gnu/libbeidpkcs11.so.0" },
		{ BITS_64, "/usr/lib64/libbeidpkcs11.so.0" },
		{ BITS_64, "/usr/lib/x86_64-linux-gnu/libbeidpkcs11.so.0" },
		{ bitness, LIBDIR "/libbeidpkcs11.so.0" },
	};
	unsigned int i;
	gboolean found32 = FALSE;
	gboolean found64 = FALSE;
	gboolean foundforeign = FALSE;
	GtkTreeIter iter;

	for(i=0;i<(sizeof(locs) / sizeof(locs[0]));i++) {
		if(stat(locs[i].loc, &st) < 0) {
			switch(errno) {
			case ENOENT:
				/* file does not exist */
				break;
			default:
				dialog = gtk_message_dialog_new(GTK_WINDOW(top), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "stat: %s", strerror(errno));
				gtk_dialog_run(GTK_DIALOG(dialog));
				gtk_widget_destroy(dialog);
				break;
			}
		} else {
			gchar* str;
			switch(locs[i].bitness) {
				case BITS_32:
					if(found32) continue;
					str = _("32-bit PKCS#11 location");
					found32 = TRUE;
					break;
				case BITS_64:
					if(found64) continue;
					str = _("64-bit PKCS#11 location");
					found64 = TRUE;
					break;
				default:
					str = _("PKCS#11 location");
					foundforeign = TRUE;
					break;
			}

			gtk_list_store_append(data, &iter);
			gtk_list_store_set(data, &iter, 0, str, 1, locs[i].loc, -1);
		}
	}
	switch(bitness) {
	case BITS_32:
	case BITS_64:
		if(!found32) {
			gtk_list_store_append(data, &iter);
			gtk_list_store_set(data, &iter, 0, _("32-bit PKCS#11 location"), 1, _("(not found)"), -1);
		}
		if(!found64) {
			gtk_list_store_append(data, &iter);
			gtk_list_store_set(data, &iter, 0, _("64-bit PKCS#11 location"), 1, _("(not found)"), -1);
		}
		break;
	default:
		if(!foundforeign) {
			gtk_list_store_append(data, &iter);
			gtk_list_store_set(data, &iter, 0, _("PKCS#11 location"), 1, _("(not found)"), -1);
		}
		break;
	}
}

void copyline_simple(GtkTreeModel* model, GtkTreePath *path G_GNUC_UNUSED, GtkTreeIter *iter, gchar** text) {
	gchar *old = *text;
	gchar *value;

	gtk_tree_model_get(model, iter, 1, &value, -1);
	if(*text == NULL) {
		*text = g_strdup_printf("%s", value);
	} else {
		// should not happen, but better safe than sorry...
		*text = g_strdup_printf("%s\n%s", old, value);
		g_free(old);
	}
}

void copyline_detail(GtkTreeModel* model, GtkTreePath *path G_GNUC_UNUSED, GtkTreeIter *iter, gchar** text) {
	gchar *old = *text;
	gchar *name, *value;
	
	gtk_tree_model_get(model, iter, 0, &name, 1, &value, -1);
	if(*text == NULL) {
		*text = g_strdup_printf("%s: %s", name, value);
	} else {
		*text = g_strdup_printf("%s\n%s: %s", old, name, value);
		g_free(old);
	}
}

void copy2clip(GtkTreeView* tv) {
	GtkClipboard* clip = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	GtkTreeSelection* sel = gtk_tree_view_get_selection(tv);
	GtkTreeSelectionForeachFunc copyline = (GtkTreeSelectionForeachFunc)copyline_detail;
	gchar* text = NULL;
	gint rowcount = gtk_tree_selection_count_selected_rows(sel);

	if(rowcount == 1) {
		copyline = (GtkTreeSelectionForeachFunc)copyline_simple;
	}

	gtk_tree_selection_selected_foreach(sel, copyline, &text);
	if(!text) return;
	gtk_clipboard_set_text(clip, text, strlen(text));
}

void copy2prim(GtkTreeSelection* sel, gpointer user_data G_GNUC_UNUSED) {
	GtkClipboard* clip = gtk_clipboard_get(GDK_SELECTION_PRIMARY);
	gchar* text = NULL;
	gint rowcount = gtk_tree_selection_count_selected_rows(sel);

	if(rowcount != 1) {
		return;
	}
	gtk_tree_selection_selected_foreach(sel, (GtkTreeSelectionForeachFunc)copyline_simple, &text);
	gtk_clipboard_set_text(clip, text, strlen(text));
}

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
		if(!strncmp(undat.machine, "arm", 3)) {
			values = strdup(undat.machine);
		} else {
			asprintf(&values, _("Unknown (%s)"), undat.machine);
		}
		gtk_list_store_set(data, &iter, 0, _("System architecture"), 1, values, -1);
		free(values);
	}

	gtk_list_store_append(data, &iter);
	asprintf(&values, "%s %s %s %s %s", undat.sysname, undat.nodename, undat.release, undat.version, undat.machine);
	gtk_list_store_set(data, &iter, 0, "uname", 1, values, -1);
	free(values);
}

char* get_lsb_info(char opt) {
	char cmd[] = "lsb_release -. -s";
	char *rv, *loc;
	FILE *f;

	*(strchr(cmd, '.')) = opt;
	f=popen(cmd, "r");
	if(!f) {
		return strdup(_("(unknown)"));
	}
	rv = malloc(80);
	rv[79]='\0';
	rv[0]='\0';
	if(fgets(rv, 79, f) == NULL) {
		free(rv);
		rv = strdup(_("(unknown)"));
		goto exit;
	}
	if(strlen(rv) == 0) {
		free(rv);
		rv = strdup(_("(unknown)"));
		goto exit;
	}
	if((loc = strrchr(rv, '\n')) != NULL) {
		*loc = '\0';
	}
exit:
	pclose(f);
	return rv;
}

void do_distro(GtkWidget* top G_GNUC_UNUSED, GtkListStore* data) {
	GtkTreeIter iter;
	char *dat;

	dat = get_lsb_info('i');
	gtk_list_store_append(data, &iter);
	gtk_list_store_set(data, &iter, 0, _("Distribution"), 1, dat, -1);
	free(dat);

	dat = get_lsb_info('r');
	gtk_list_store_append(data, &iter);
	gtk_list_store_set(data, &iter, 0, _("Distribution version"), 1, dat, -1);
	free(dat);

	dat = get_lsb_info('c');
	gtk_list_store_append(data, &iter);
	gtk_list_store_set(data, &iter, 0, _("Distribution codename"), 1, dat, -1);
	free(dat);
}

int main(int argc, char** argv) {
	GtkBuilder* builder;
	GtkWidget *window, *treeview, *button;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn* col;
	GtkTreeIter iter;
	GtkListStore *store;
	GtkAccelGroup *group;
	GtkTreeSelection* sel;
	gchar *tmp, *loc;

	bindtextdomain("about-eid-mw", DATAROOTDIR "/locale");
	textdomain("about-eid-mw");

	gtk_init(&argc, &argv);

	builder = gtk_builder_new();
	gtk_builder_add_from_string(builder, ABOUT_GLADE_STRING, strlen(ABOUT_GLADE_STRING), NULL);
	window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
	group = gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(window), group);

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

	check_pcsc(window, store);
	do_viewer(window, store);
	do_distro(window, store);
	do_uname(window, store);
	do_files(window, store);

	renderer = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes(_("Item"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col);
	col = gtk_tree_view_column_new_with_attributes(_("Value"), renderer, "text", 1, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col);

	sel = GTK_TREE_SELECTION(gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview)));
	gtk_tree_selection_set_mode(sel, GTK_SELECTION_MULTIPLE);
	g_signal_connect(G_OBJECT(sel), "changed", G_CALLBACK(copy2prim), NULL);

	g_signal_connect(G_OBJECT(window), "delete-event", gtk_main_quit, NULL);
	button = GTK_WIDGET(gtk_builder_get_object(builder, "quitbtn"));
	g_signal_connect(G_OBJECT(button), "clicked", gtk_main_quit, NULL);
	gtk_widget_add_accelerator(button, "clicked", group, GDK_KEY_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(button, "clicked", group, GDK_KEY_w, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	button = GTK_WIDGET(gtk_builder_get_object(builder, "copybtn"));
	g_signal_connect_swapped(G_OBJECT(button), "clicked", G_CALLBACK(copy2clip), treeview);
	gtk_widget_add_accelerator(button, "clicked", group, GDK_KEY_c, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

	gtk_widget_show_all(window);

	gtk_main();

	return 0;
}
