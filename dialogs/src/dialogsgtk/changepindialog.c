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
#include <string.h>
#include <stdlib.h>
#include <glib/gi18n.h>

#define MIN_PIN_LENGTH 4
#define MAX_PIN_LENGTH 12

#define EXIT_OK		0
#define EXIT_CANCEL 1
#define EXIT_ERROR	2

// struct holding all the runtime data, so we can use callbacks without global variables
/////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
	GtkWidget  *dialog,*pinLabel,*table,*pinFrame,*backspace,*clear,*eventBox,*digits[10];
	GtkWidget  *newPinsTable, *originalPinLabel, *newPin0Label, *newPin1Label, *originalPinEntry, *newPin0Entry, *newPin1Entry;
} PinDialogInfo;

// if MIN_PIN_LENGTH or more digits have been entered, enable the OK button
///////////////////////////////////////////////////////////////////////////


int entries_are_valid(PinDialogInfo* pindialog)
{
	gchar* original_pin	=gtk_entry_get_text(GTK_ENTRY(pindialog->originalPinEntry));
	gchar* new_pin0		=gtk_entry_get_text(GTK_ENTRY(pindialog->newPin0Entry));
	gchar* new_pin1		=gtk_entry_get_text(GTK_ENTRY(pindialog->newPin1Entry));

	if(strlen(original_pin)<MIN_PIN_LENGTH || strlen(new_pin0)<MIN_PIN_LENGTH || strlen(new_pin1)<MIN_PIN_LENGTH)
		return 0;
}


void update_ok_button(PinDialogInfo *pindialog)
{
/*	if(strlen(pindialog->newPin1Entry)>=MIN_PIN_LENGTH)
	{
		GtkWidget* okButton=gtk_dialog_get_widget_for_response(GTK_DIALOG(pindialog->dialog),GTK_RESPONSE_OK);
		gtk_dialog_set_response_sensitive(GTK_DIALOG(pindialog->dialog),GTK_RESPONSE_OK, TRUE);
		gtk_dialog_set_default_response(GTK_DIALOG(pindialog->dialog),GTK_RESPONSE_OK);
		gtk_widget_grab_focus(okButton);
		
	}
	else
	{
		gtk_dialog_set_response_sensitive(GTK_DIALOG(pindialog->dialog), GTK_RESPONSE_OK, FALSE);
		gtk_dialog_set_default_response(GTK_DIALOG(pindialog->dialog),GTK_RESPONSE_REJECT);
		gtk_widget_grab_focus(pindialog->pinLabel);
	} */
}


// called when pin changed, updates OK button status
////////////////////////////////////////////////////

void pins_changed(GtkEntry *entry, gpointer _pindialog)
{
	PinDialogInfo* pindialog=(PinDialogInfo*)_pindialog;
	update_ok_button(pindialog);
}

static void add_digit(PinDialogInfo* pindialog, int digit)
{
/*	if(strlen(pindialog->pin)<MAX_PIN_LENGTH)
	{
		char tmp[MAX_PIN_LENGTH+1];
		snprintf(tmp,MAX_PIN_LENGTH+1,"%s%1d",pindialog->pin,digit);
		strcpy(pindialog->pin,tmp);
		pin_changed(pindialog);
	} */
}

// remove one digit from the current pin
////////////////////////////////////////
static void backspace(PinDialogInfo* pindialog)
{
/*	if(strlen(pindialog->pin)>0)
	{
		pindialog->pin[strlen(pindialog->pin)-1]='\0';
		pin_changed(pindialog);
	} */
}

// remove current pin entirely
///////////////////////////////////////
static void clear(PinDialogInfo* pindialog)
{
//	pin_changed(pindialog);
}

// event handler for delete-event. always approves the deletion
///////////////////////////////////////////////////////////////
static gboolean on_delete_event( GtkWidget *widget, GdkEvent* event, gpointer pindialog)
{
    return TRUE;
}

// event handler for all numerical buttons on the virtual keypad
//////////////////////////////////////////////////////////////// 
static void on_key_digit(GtkWidget* kie, gpointer _pindialog)
{
	PinDialogInfo* pindialog=(PinDialogInfo*)_pindialog;
	add_digit(pindialog,atoi(((char*)gtk_button_get_label(GTK_BUTTON(kie)))));
}

// event handler for backspace button on virtual keypad
/////////////////////////////////////////////////////////////////
static void on_key_backspace(GtkWidget* kie, gpointer _pindialog)
{
	PinDialogInfo* pindialog=(PinDialogInfo*)_pindialog;
	backspace(pindialog);
}

// event handler for clear button on virtual keypad
///////////////////////////////////////////////////
static void on_key_clear(GtkWidget* kie, gpointer _pindialog)
{
	PinDialogInfo* pindialog=(PinDialogInfo*)_pindialog;
	clear(pindialog);
}

int main(int argc, char* argv[])
{
	int 			return_value=EXIT_ERROR;
	PinDialogInfo 	pindialog;									// this struct contains all objects
	GdkColor 		color;					

	fprintf(stderr,"--- gtk_init()\n");
    gtk_init(&argc,&argv);										// initialize gtk+

	// create new message dialog with CANCEL and OK buttons in standard places, in center of user's screen
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	fprintf(stderr,"--- creating dialog\n");
    pindialog.dialog=gtk_message_dialog_new(NULL,GTK_DIALOG_MODAL,GTK_MESSAGE_QUESTION,GTK_BUTTONS_OK_CANCEL,argv[1]);
	gtk_dialog_set_default_response(GTK_DIALOG(pindialog.dialog),GTK_RESPONSE_OK);
    gtk_window_set_title(GTK_WINDOW(pindialog.dialog),"beid PIN Request");
    gtk_window_set_position(GTK_WINDOW(pindialog.dialog), GTK_WIN_POS_CENTER);
    g_signal_connect (pindialog.dialog, "delete-event", G_CALLBACK (on_delete_event),&pindialog);

	// create on-screen numeric keypad, connect the digit and action keys to their event handlers
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	pindialog.table=gtk_table_new(4,3,TRUE);	// table of 4 rows, 3 columns

	// digit 0
	pindialog.digits[0]=gtk_button_new_with_label("0");
	gtk_table_attach(GTK_TABLE(pindialog.table),pindialog.digits[0],1,2,3,4,(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),2,2);
	gtk_widget_set_can_focus(pindialog.digits[0],FALSE);
	g_signal_connect(pindialog.digits[0],"clicked",G_CALLBACK(on_key_digit),(gpointer)&pindialog);

	// digits 1 to 9	
	int i;
	for(i=1;i<=9;i++)
	{
		char label[2];
		snprintf(label,2,"%1d",i);
		pindialog.digits[i]=gtk_button_new_with_label(label);
		int col=(i-1)%3;
		int row=(i-1)/3;
		gtk_table_attach(GTK_TABLE(pindialog.table),pindialog.digits[i],col,col+1,row,row+1,(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),2,2);
		gtk_widget_set_can_focus(pindialog.digits[i],FALSE);
		g_signal_connect(pindialog.digits[i],"clicked",G_CALLBACK(on_key_digit),(gpointer)&pindialog);
	}

	// backspace button
	pindialog.backspace=gtk_button_new();
	gtk_container_add(GTK_CONTAINER(pindialog.backspace),gtk_image_new_from_stock(GTK_STOCK_GO_BACK,GTK_ICON_SIZE_SMALL_TOOLBAR));
	gtk_table_attach(GTK_TABLE(pindialog.table),pindialog.backspace,0,1,3,4,(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),2,2);
	gtk_widget_set_can_focus(pindialog.backspace,FALSE);
	g_signal_connect(pindialog.backspace,"clicked",G_CALLBACK(on_key_backspace),(gpointer)&pindialog);

	// clear button
	pindialog.clear=gtk_button_new();
	gtk_container_add(GTK_CONTAINER(pindialog.clear),gtk_image_new_from_stock(GTK_STOCK_CLEAR,GTK_ICON_SIZE_SMALL_TOOLBAR));
	gtk_table_attach(GTK_TABLE(pindialog.table),pindialog.clear,2,3,3,4,(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),2,2);
	gtk_widget_set_can_focus(pindialog.clear,FALSE);
	g_signal_connect(pindialog.clear,"clicked",G_CALLBACK(on_key_clear),(gpointer)&pindialog);

	// create original, new, and verify new pin entry fields with labels, in a table
	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	pindialog.newPinsTable		=gtk_table_new(3,2,TRUE);    // table of 4 rows, 3 columns

	pindialog.originalPinLabel	=gtk_label_new("Current PIN:");
	pindialog.newPin0Label		=gtk_label_new("New PIN:");
	pindialog.newPin1Label		=gtk_label_new("New PIN (again):");

	pindialog.originalPinEntry	=gtk_entry_new();
	pindialog.newPin0Entry		=gtk_entry_new();
	pindialog.newPin1Entry		=gtk_entry_new();

	gtk_table_attach(GTK_TABLE(pindialog.newPinsTable),pindialog.originalPinLabel,0,1,0,1,(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),2,2);
	gtk_table_attach(GTK_TABLE(pindialog.newPinsTable),pindialog.newPin0Label,	  0,1,1,2,(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),2,2);
	gtk_table_attach(GTK_TABLE(pindialog.newPinsTable),pindialog.newPin1Label,	  0,1,2,3,(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),2,2);

	gtk_table_attach(GTK_TABLE(pindialog.newPinsTable),pindialog.originalPinEntry,1,2,0,1,(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),2,2);
	gtk_table_attach(GTK_TABLE(pindialog.newPinsTable),pindialog.newPin0Entry,1,2,1,2,(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),2,2);
	gtk_table_attach(GTK_TABLE(pindialog.newPinsTable),pindialog.newPin1Entry,1,2,2,3,(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),2,2);

	g_signal_connect(pindialog.originalPinEntry,"changed",G_CALLBACK(pins_changed),(gpointer)&pindialog);
	g_signal_connect(pindialog.newPin0Entry,"changed",G_CALLBACK(pins_changed),(gpointer)&pindialog);
	g_signal_connect(pindialog.newPin1Entry,"changed",G_CALLBACK(pins_changed),(gpointer)&pindialog);

	// add all these objects to the dialog
	///////////////////////////////////////////////////////////////////////////////////////////////////////////

   	gtk_container_set_border_width(GTK_CONTAINER(pindialog.dialog),10);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pindialog.dialog)->vbox),pindialog.newPinsTable, TRUE, TRUE,2);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pindialog.dialog)->vbox),pindialog.table, FALSE, FALSE,2);

	// show all these widgets, and run the dialog as a modal dialog until it is closed by the user
	//////////////////////////////////////////////////////////////////////////////////////////////    

	fprintf(stderr,"--- showing dialog\n");
    gtk_widget_show_all(GTK_WIDGET(pindialog.dialog));
	
	fprintf(stderr,"--- modal dialog\n");
    switch(gtk_dialog_run(GTK_DIALOG(pindialog.dialog)))
	{
		case GTK_RESPONSE_OK:					// if the use chose OK
		//	printf("%s\n",pindialog.pin);		// output the PIN to stdout
			return_value=EXIT_OK;				// and return OK
		break;

		default:								// otherwise
			return_value=EXIT_CANCEL;			// output nothing and return CANCEL
		break;
	}

	// properly dispose of the dialog (which disposes of all it's children), and exit with specific return value
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

	fprintf(stderr,"--- dispose of dialog\n");
	gtk_widget_destroy(pindialog.dialog);
	exit(return_value);
}
