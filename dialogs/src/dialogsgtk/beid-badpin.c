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
#include <locale.h>
#include "config.h"
#include "parent.h"

#define EXIT_OK			0
#define EXIT_ERROR		2

enum { MSG_INCORRECT_PIN_CODE=1, MSG_N_ATTEMPTS_LEFT, MSG_LAST_ATTEMPT };
char* beid_messages[4][4]={
                                    "en",   "beID: Incorrect PIN Code",    	"You have entered an incorrect PIN code.\nPlease note that you have only %d attempts left before your PIN is blocked.", 					"You have entered an incorrect PIN code.\nPlease note that at the next incorrect entry your PIN code will be blocked.",
                                    "nl",   "beID: Foutive PINcode",    	"U hebt een foutive PIN code ingegeven.\nGelieve te noteren dat u nog slechts %d pogingen hebt alvorens uw PIN code geblokkeerd wordt.", 	"U hebt een foutive PIN code ingegeven.\nGelieve te noteren dat bij de volgende incorrecte ingave uw PIN code geblokkeerd wordt.",
                                    "fr",   "beID: Code PIN incorrect",    	"Vous avez entré un code PIN incorrect.\nVeuillez noter qu'il ne vous reste plus que %d tentatives avant que votre PIN soit bloqué", 		"Vous avez entré un code PIN incorrect.\nVieullez noter qu'a la prochaine entree incorrecte votre code PIN sera bloqué",
                                    "de",   "beID: Incorrect PIN Code",    	"You have entered an incorrect PIN code.\nPlease note that you have only %d attempts left before your PIN is blocked.", 					"You have entered an incorrect PIN code.\nPlease note that at the next incorrect entry your PIN code will be blocked."
                              };

#include "beid-i18n.h"

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

    gtk_init(&argc,&argv);										// initialize gtk+
	
	// create new message dialog with CANCEL button in standard places, in center of user's screen
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	char message[1024];
	if((argc==2) && (argv[1]!=NULL) && (strlen(argv[1])==1) && isdigit(*(argv[1])))
	{
		int attempts=atoi(argv[1]);
		if(attempts>1)
			snprintf(message,sizeof(message)-2,_MSG_(MSG_N_ATTEMPTS_LEFT),attempts);
		else
			snprintf(message,sizeof(message)-2,_MSG_(MSG_LAST_ATTEMPT));
	}
	else
	{
		fprintf(stderr,"Incorrect Parameter for <number of attempts left>\n");
		exit(EXIT_ERROR);
	}
	
    dialog=gtk_message_dialog_new(NULL,GTK_DIALOG_MODAL,GTK_MESSAGE_WARNING,GTK_BUTTONS_OK,message);
	gtk_dialog_set_default_response(GTK_DIALOG(dialog),GTK_RESPONSE_OK);
    gtk_window_set_title(GTK_WINDOW(dialog),_MSG_(MSG_INCORRECT_PIN_CODE));
    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);

	// show all these widgets, and run the dialog as a modal dialog until it is closed by the user
	//////////////////////////////////////////////////////////////////////////////////////////////    

    gtk_widget_show_all(GTK_WIDGET(dialog));
    switch(gtk_dialog_run(GTK_DIALOG(dialog)))
	{
		case GTK_RESPONSE_OK:					// if the use chose OK
			printf("OK\n");
			return_value=EXIT_OK;
		break;

		default:								// otherwise
			printf("ERROR\n");
			return_value=EXIT_ERROR;
		break;
	}

	// properly dispose of the dialog (which disposes of all it's children), and exit with specific return value
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

	gtk_widget_destroy(dialog);
	exit(return_value);
}

