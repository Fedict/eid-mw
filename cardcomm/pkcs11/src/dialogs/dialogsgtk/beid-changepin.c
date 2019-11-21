/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2010 FedICT.
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
#include <locale.h>
#include "config.h"
#include "parent.h"
#include "gettext.h"
#include <ctype.h>

#define MIN_PIN_LENGTH 4
#define MAX_PIN_LENGTH 16

#define EXIT_OK		0
#define EXIT_CANCEL 1
#define EXIT_ERROR	2

#if GTK_CHECK_VERSION(3, 96, 0)
#define gtk_init(a, b) gtk_init()
#define gtk_entry_get_text(e) gtk_entry_buffer_get_text(gtk_entry_get_buffer(e))
#define gtk_box_pack_start(b, c, e, f, p) gtk_box_insert_child_after(b, c, NULL)

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
        GtkWidget *newPinsGrid, *originalPinLabel, *newPin0Label, *newPin1Label, *originalPinEntry,
                *newPin0Entry, *newPin1Entry;
        GtkButton *okbutton, *cancelbutton;
} PinDialogInfo;


// check validity of 3 fields
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int entries_are_valid(PinDialogInfo * pindialog) {
        const gchar *original_pin = gtk_entry_get_text(GTK_ENTRY(pindialog->originalPinEntry));
        const gchar *new_pin0 = gtk_entry_get_text(GTK_ENTRY(pindialog->newPin0Entry));
        const gchar *new_pin1 = gtk_entry_get_text(GTK_ENTRY(pindialog->newPin1Entry));

        // no fields have pins that are too short in them?
        if (strlen(original_pin) < MIN_PIN_LENGTH || strlen(new_pin0) < MIN_PIN_LENGTH
            || strlen(new_pin1) < MIN_PIN_LENGTH)
                return -1;

        // no fields have pins that are too long in them?
        if (strlen(original_pin) > MAX_PIN_LENGTH || strlen(new_pin0) > MAX_PIN_LENGTH
            || strlen(new_pin1) > MAX_PIN_LENGTH)
                return -2;

        // the verify new pin equals the new pin field contents?
        if (strcmp(new_pin0, new_pin1) != 0)
                return -4;

        // the new pin is not the same as the old pin?
        if (strcmp(original_pin, new_pin1) == 0)
                return -5;

        // if no failures above, approve fields
        return 1;
}


// singal handler to attach to GtkEntry, that will limit input to base-10 digits
////////////////////////////////////////////////////////////////////////////////
void insert_only_digits(GtkEntry * entry, const gchar * text, gint length, gint * position,
                        gpointer data) {
        GtkEditable *editable = GTK_EDITABLE(entry);
        int i, count = 0;
        gchar *result = g_new(gchar, length);

        for (i = 0; i < length; i++)
                if (isdigit(text[i]))
                        result[count++] = text[i];

        if (count > 0) {
                g_signal_handlers_block_by_func(G_OBJECT(editable),
                                                G_CALLBACK(insert_only_digits), data);
                gtk_editable_insert_text(editable, result, count, position);
                g_signal_handlers_unblock_by_func(G_OBJECT(editable),
                                                  G_CALLBACK(insert_only_digits), data);
        }

        g_signal_stop_emission_by_name(G_OBJECT(editable), "insert_text");
        g_free(result);
}


// if a reasonable number of digits have been entered in all 3 fields, the new pin equals the new pin's check field, *and* the new pin is not the old pin,
// enable the OK button
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void update_ok_button(PinDialogInfo * pindialog) {
        if (entries_are_valid(pindialog) > 0) {
                gtk_dialog_set_response_sensitive(GTK_DIALOG(pindialog->dialog), GTK_RESPONSE_OK,
                                                  TRUE);
                gtk_dialog_set_default_response(GTK_DIALOG(pindialog->dialog), GTK_RESPONSE_OK);
        } else {
                gtk_dialog_set_response_sensitive(GTK_DIALOG(pindialog->dialog), GTK_RESPONSE_OK,
                                                  FALSE);
                gtk_dialog_set_default_response(GTK_DIALOG(pindialog->dialog), GTK_RESPONSE_CANCEL);
        }
}


// called when pin changed, updates OK button status
////////////////////////////////////////////////////

void pins_changed(GtkEntry * entry, gpointer _pindialog) {
        PinDialogInfo *pindialog = (PinDialogInfo *) _pindialog;

        update_ok_button(pindialog);
}

// event handler for delete-event. always approves the deletion
///////////////////////////////////////////////////////////////
static gboolean on_delete_event(GtkWidget * widget, GdkEvent * event, gpointer pindialog) {
        return TRUE;
}

// main program
///////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[]) {
        int return_value = EXIT_ERROR;
        PinDialogInfo pindialog;
        char caller_path[1024];

        gtk_init(&argc, &argv); // initialize gtk+

	putenv("LANGUAGE=");
	bindtextdomain("dialogs-beid", DATAROOTDIR "/locale");
	textdomain("dialogs-beid");

        // create new message dialog with CANCEL and OK buttons in standard places, in center of user's screen
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if(argc < 2 || (strlen(argv[1]) == 0)) {
		fprintf(stderr, "E: required argument: PIN name");
		exit(EXIT_ERROR);
	}

        if (get_parent_path(caller_path, sizeof(caller_path) - 2) > 0) {
                pindialog.dialog =
                        gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION,
                                               GTK_BUTTONS_NONE,
                                               gettext("Request from Application [%s]:\n\nPlease enter your current eID %s, followed by your new eID %s (twice)"),
                                               caller_path, argv[1], argv[1]);
        } else {
                fprintf(stderr, "Failed To Determine Parent Process. Aborting.\n");
                exit(EXIT_ERROR);
        }

        pindialog.cancelbutton =
                GTK_BUTTON(gtk_dialog_add_button
                           (GTK_DIALOG(pindialog.dialog), gettext("_Cancel"), GTK_RESPONSE_CANCEL));
        pindialog.okbutton =
                GTK_BUTTON(gtk_dialog_add_button
                           (GTK_DIALOG(pindialog.dialog), gettext("_OK"), GTK_RESPONSE_OK));

        gtk_dialog_set_default_response(GTK_DIALOG(pindialog.dialog), GTK_RESPONSE_OK);
        gtk_window_set_title(GTK_WINDOW(pindialog.dialog), gettext("beID: Change PIN Code"));
        gtk_window_set_position(GTK_WINDOW(pindialog.dialog), GTK_WIN_POS_CENTER);
        g_signal_connect(pindialog.dialog, "delete-event", G_CALLBACK(on_delete_event), &pindialog);

        // create original, new, and verify new pin entry fields with labels, in a table
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////

        pindialog.newPinsGrid = gtk_grid_new();
	gtk_grid_set_row_homogeneous(GTK_GRID(pindialog.newPinsGrid), TRUE);
	gtk_grid_set_column_homogeneous(GTK_GRID(pindialog.newPinsGrid), TRUE);

        pindialog.originalPinLabel = gtk_label_new(gettext("Current PIN:"));
        pindialog.newPin0Label = gtk_label_new(gettext("New PIN:"));
        pindialog.newPin1Label = gtk_label_new(gettext("New PIN (again):"));
        pindialog.originalPinEntry = gtk_entry_new();
        pindialog.newPin0Entry = gtk_entry_new();
        pindialog.newPin1Entry = gtk_entry_new();

        // set max lengths
        gtk_entry_set_max_length(GTK_ENTRY(pindialog.originalPinEntry), MAX_PIN_LENGTH);
        gtk_entry_set_max_length(GTK_ENTRY(pindialog.newPin0Entry), MAX_PIN_LENGTH);
        gtk_entry_set_max_length(GTK_ENTRY(pindialog.newPin1Entry), MAX_PIN_LENGTH);

        // disable visibilities
        gtk_entry_set_visibility(GTK_ENTRY(pindialog.originalPinEntry), FALSE);
        gtk_entry_set_visibility(GTK_ENTRY(pindialog.newPin0Entry), FALSE);
        gtk_entry_set_visibility(GTK_ENTRY(pindialog.newPin1Entry), FALSE);

        // put labels and entries in a table
        gtk_grid_attach(GTK_GRID(pindialog.newPinsGrid), pindialog.originalPinLabel, 0, 0, 1, 1);
        gtk_grid_attach(GTK_GRID(pindialog.newPinsGrid), pindialog.newPin0Label, 0, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(pindialog.newPinsGrid), pindialog.newPin1Label, 0, 2, 1, 1);
        gtk_grid_attach(GTK_GRID(pindialog.newPinsGrid), pindialog.originalPinEntry, 1, 0, 1, 1);
        gtk_grid_attach(GTK_GRID(pindialog.newPinsGrid), pindialog.newPin0Entry, 1, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(pindialog.newPinsGrid), pindialog.newPin1Entry, 1, 2, 1, 1);

        // connect signals to filter and read inputs
        g_signal_connect(pindialog.originalPinEntry, "insert_text",
                         G_CALLBACK(insert_only_digits), (gpointer) & pindialog);
        g_signal_connect(pindialog.newPin0Entry, "insert_text", G_CALLBACK(insert_only_digits),
                         (gpointer) & pindialog);
        g_signal_connect(pindialog.newPin1Entry, "insert_text", G_CALLBACK(insert_only_digits),
                         (gpointer) & pindialog);
        g_signal_connect(pindialog.originalPinEntry, "changed", G_CALLBACK(pins_changed),
                         (gpointer) & pindialog);
        g_signal_connect(pindialog.newPin0Entry, "changed", G_CALLBACK(pins_changed),
                         (gpointer) & pindialog);
        g_signal_connect(pindialog.newPin1Entry, "changed", G_CALLBACK(pins_changed),
                         (gpointer) & pindialog);

        // add all these objects to the dialog
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////

#if !GTK_CHECK_VERSION(3, 96, 0)
        gtk_container_set_border_width(GTK_CONTAINER(pindialog.dialog), 10);
#endif
        gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(pindialog.dialog))),
                           pindialog.newPinsGrid, TRUE, TRUE, 2);

        // initial state for OK button
        /////////////////////////////////////////////////////////////////////////////////////////////////////////

        update_ok_button(&pindialog);

        // show all these widgets, and run the dialog as a modal dialog until it is closed by the user
        //////////////////////////////////////////////////////////////////////////////////////////////    

        gtk_widget_show_all(GTK_WIDGET(pindialog.dialog));
        switch (gtk_dialog_run(GTK_DIALOG(pindialog.dialog))) {
                case GTK_RESPONSE_OK:  // if the user chose OK
                        {
                                const char *oldpin =
                                        gtk_entry_get_text(GTK_ENTRY(pindialog.originalPinEntry));
                                const char *newpin =
                                        gtk_entry_get_text(GTK_ENTRY(pindialog.newPin0Entry));

                                printf("%s:%s\n", oldpin, newpin);      // output the PINs to stdout
                                return_value = EXIT_OK; // and return OK
                        }
                        break;

                default:       // otherwise
                        printf("CANCEL\n");
                        return_value = EXIT_OK; // output CANCEL and return ok (cancel is not an error)
                        break;
        }

        // properly dispose of the dialog (which disposes of all it's children), and exit with specific return value
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////

        gtk_widget_destroy(pindialog.dialog);
        exit(return_value);
}
