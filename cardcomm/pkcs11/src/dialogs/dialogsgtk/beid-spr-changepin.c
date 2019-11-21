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
#include <string.h>
#include <stdlib.h>
#include <glib/gi18n.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "config.h"
#include <locale.h>
#include "parent.h"
#include "gettext.h"

#define EXIT_OK		0
#define EXIT_CANCEL 1
#define EXIT_ERROR	2

#if GTK_CHECK_VERSION(3, 96, 0)
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

// struct holding all the runtime data, so we can use callbacks without global variables
/////////////////////////////////////////////////////////////////////////////////////////
typedef struct {
        GtkWidget *dialog;
} PinDialogInfo;

// event handler for delete-event. always approves the deletion
///////////////////////////////////////////////////////////////
static gboolean on_delete_event(GtkWidget * widget, GdkEvent * event, gpointer pindialog) {
        return TRUE;
}

int main(int argc, char *argv[]) {
        int return_value = EXIT_ERROR;
        PinDialogInfo pindialog;        // this struct contains all dialog objects
        char caller_path[1024];

        gtk_init(&argc, &argv); // initialize gtk+

	putenv("LANGUAGE=");
	bindtextdomain("dialogs-beid", DATAROOTDIR "/locale");
	textdomain("dialogs-beid");

        // create new message dialog with CANCEL button in standard places, in center of user's screen
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        if (get_parent_path(caller_path, sizeof(caller_path) - 2) > 0) {
                if (!((argc == 3) && (argv[1] != NULL) && (strlen(argv[1]) != 0)
		      && (argv[2] != NULL) && (strlen(argv[2]) > 0))) {
                        fprintf(stderr, "Incorrect parameters. Expected: <PIN name> <description of SPR>\n");
                        exit(EXIT_ERROR);
                }
        } else {
                fprintf(stderr, "Failed To Determine Parent Process. Aborting.\n");
                exit(EXIT_ERROR);
        }

        pindialog.dialog =
                gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION,
                                       GTK_BUTTONS_NONE, gettext("Request from Application [%s]:\n\nPlease change your eID %s code on the secure pinpad reader:\n[%s]..."),
                                       caller_path, argv[1], argv[2]);
        gtk_dialog_set_default_response(GTK_DIALOG(pindialog.dialog), GTK_RESPONSE_OK);
        gtk_window_set_title(GTK_WINDOW(pindialog.dialog), gettext("beID: Change PIN Code"));
        gtk_window_set_position(GTK_WINDOW(pindialog.dialog), GTK_WIN_POS_CENTER);
        g_signal_connect(pindialog.dialog, "delete-event", G_CALLBACK(on_delete_event), &pindialog);

        // show all these widgets, and run the dialog as a modal dialog until it is closed by the user
        //////////////////////////////////////////////////////////////////////////////////////////////    

        gtk_widget_show_all(GTK_WIDGET(pindialog.dialog));

        switch (gtk_dialog_run(GTK_DIALOG(pindialog.dialog))) {
                case GTK_RESPONSE_CANCEL:      // if the use chose CANCEL
                        printf("CANCEL\n");
                        return_value = EXIT_OK; // output CANCEL and return OK (cancel is not an error)
                        break;

                default:       // otherwise
                        printf("ERROR\n");
                        return_value = EXIT_ERROR;      // output and return ERROR
                        break;
        }

        // properly dispose of the dialog (which disposes of all it's children), and exit with specific return value
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////

        gtk_widget_destroy(pindialog.dialog);
        exit(return_value);
}
