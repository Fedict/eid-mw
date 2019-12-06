/* ****************************************************************************
 * eID Middleware Project.
 * Copyright (C) 2008-2011 FedICT.
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

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <glib.h>
#include <stdlib.h>
#include <glib/gi18n.h>
#include <locale.h>
#include "config.h"
#include "parent.h"
#include "gettext.h"

#define EXIT_OK			0
#define EXIT_CANCEL		1
#define EXIT_ERROR		2

#if GTK_CHECK_VERSION(3, 96, 0)
void gtk_widget_show_all_ll(GtkWidget *widget, gpointer data G_GNUC_UNUSED) {
	if(GTK_IS_CONTAINER(widget)) {
		gtk_container_foreach(GTK_CONTAINER(widget), gtk_widget_show_all_ll, NULL);
	}
	gtk_widget_show(widget);
}

void gtk_widget_show_all(GtkWidget *widget) {
	gtk_widget_show_all_ll(widget, NULL);
}
#define gtk_init(a, b) gtk_init();
#endif

int main(int argc, char *argv[]) {
        int return_value;
        GtkWidget *dialog;
        char caller_path[1024];

        gtk_init(&argc, &argv); // initialize gtk+

	/* initialize gettext */
	putenv("LANGUAGE=");
	bindtextdomain("dialogs-beid", DATAROOTDIR "/locale");
	textdomain("dialogs-beid");

        // create new message dialog with CANCEL button in standard places, in center of user's screen
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        if (get_parent_path(caller_path, sizeof(caller_path) - 2) > 0) {
                dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION,
                                                GTK_BUTTONS_OK_CANCEL,
                                                gettext("The application [%s] wants to access the eID card. Do you want to accept it?"), caller_path);
        } else {
                fprintf(stderr, "Failed To Determine Parent Process. Aborting.\n");
                exit(EXIT_ERROR);
        }

        gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
        gtk_window_set_title(GTK_WINDOW(dialog), gettext("beID: Card Access"));
        gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);

        // show all these widgets, and run the dialog as a modal dialog until it is closed by the user
        //////////////////////////////////////////////////////////////////////////////////////////////    

        gtk_widget_show_all(GTK_WIDGET(dialog));
        switch (gtk_dialog_run(GTK_DIALOG(dialog))) {
                case GTK_RESPONSE_OK:
                        printf("OK\n");
                        return_value = EXIT_OK;
                        break;

                case GTK_RESPONSE_CANCEL:
                        printf("CANCEL\n");
                        return_value = EXIT_OK;
                        break;

                default:
                        printf("ERROR\n");
                        return_value = EXIT_ERROR;
                        break;
        }

        // properly dispose of the dialog (which disposes of all it's children), and exit with specific return value
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////

        gtk_widget_destroy(dialog);
        exit(return_value);
}
