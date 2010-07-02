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
#include <gtk/gtkenums.h>
#include <gdk/gdkkeysyms.h>
#include <glib.h>
#include <stdlib.h>
#include <glib/gi18n.h>
#include <libintl.h>
#include <locale.h>
#include "config.h"

#define _(String) gettext (String)

#define EXIT_OK			0
#define EXIT_ERROR		2

// event handler for delete-event. always approves the deletion
///////////////////////////////////////////////////////////////
static gboolean on_delete_event( GtkWidget *widget, GdkEvent* event, gpointer pindialog)
{
    return TRUE;
}

int main(int argc, char* argv[])
{
	int 		return_value=EXIT_ERROR;
	GtkWidget*	dialog;

	setlocale(LC_MESSAGES,"");
    bindtextdomain(PACKAGE,LOCALEDIR);
    textdomain(PACKAGE);

    gtk_init(&argc,&argv);										// initialize gtk+
	
	// create new message dialog with CANCEL button in standard places, in center of user's screen
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	char message[1024];
	if((argc==2) && (argv[1]!=NULL) && (strlen(argv[1])==1) && isdigit(*(argv[1])))
	{
		int attempts=atoi(argv[1]);
		if(attempts>1)
			snprintf(message,sizeof(message)-2,_("You have entered an incorrect PIN code.\nPlease note that you have only %d attempts left before your PIN is blocked."),attempts);
		else
			snprintf(message,sizeof(message)-2,_("You have entered an incorrect PIN code.\nPlease note that at the next incorrect entry your PIN code will be blocked."));
	}
	else
	{
		fprintf(stderr,"Incorrect Parameter for <number of attempts left>\n");
		exit(1);
	}
	
    dialog=gtk_message_dialog_new(NULL,GTK_DIALOG_MODAL,GTK_MESSAGE_WARNING,GTK_BUTTONS_OK,message);
	gtk_dialog_set_default_response(GTK_DIALOG(dialog),GTK_RESPONSE_OK);
    gtk_window_set_title(GTK_WINDOW(dialog),_("beID: Incorrect PIN Code"));
    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);

	// show all these widgets, and run the dialog as a modal dialog until it is closed by the user
	//////////////////////////////////////////////////////////////////////////////////////////////    

    gtk_widget_show_all(GTK_WIDGET(dialog));
    switch(gtk_dialog_run(GTK_DIALOG(dialog)))
	{
		case GTK_RESPONSE_OK:					// if the use chose OK
			return_value=EXIT_OK;
		break;

		default:								// otherwise
			return_value=EXIT_ERROR;			// output nothing and return CANCEL
		break;
	}

	// properly dispose of the dialog (which disposes of all it's children), and exit with specific return value
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

	gtk_widget_destroy(dialog);
	exit(return_value);
}
