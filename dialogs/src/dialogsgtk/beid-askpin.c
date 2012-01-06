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
#include "config.h"
#include "parent.h"


#define MIN_PIN_LENGTH 4
#define MAX_PIN_LENGTH 12

#define EXIT_OK		0
#define EXIT_CANCEL 1
#define EXIT_ERROR	2

enum { MSG_PIN_CODE_REQUIRED=1, MSG_PLEASE_ENTER_PIN };
char* beid_messages[4][3]={
									"en",	"beID: PIN Code Required",		"The application\n[%s]\nrequests your eID PIN code.",
									"nl",	"beID: PINcode Vereist", 		"Het programma\n[%s]\nvraagt uw eID PINcode",
									"fr",	"beID: Code PIN Necessaire",	"l'application\n[%s]\nvous demande votre code PIN eID",
									"de",	"beID: PIN Code Required",		"Die Anwendung\n[%s]\nfragt um Ihren eID PIN-code" 
							  };

#include "beid-i18n.h"

// struct holding all the runtime data, so we can use callbacks without global variables
/////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
	GtkWidget  *dialog,*pinLabel,*table,*pinFrame,*backspace,*clear,*eventBox,*digits[10];
	GtkButton  *okbutton,*cancelbutton;	
	char	   	pin[MAX_PIN_LENGTH+1];
	gchar		bullet[6];
} PinDialogInfo;

// draw number of current theme's "invisible char" corresponding to number of digits in pin
///////////////////////////////////////////////////////////////////////////////////////////
void update_pin_label(PinDialogInfo *pindialog)
{
	int i;
	gchar tmp[MAX_PIN_LENGTH*6];
		  tmp[0]='\0';
	for(i=0;i<strlen(pindialog->pin);i++)
		g_strlcat(tmp,pindialog->bullet,sizeof(tmp));
	gtk_label_set_text(GTK_LABEL(pindialog->pinLabel),tmp);
}

// if MIN_PIN_LENGTH or more digits have been entered, enable the OK button
///////////////////////////////////////////////////////////////////////////
void update_ok_button(PinDialogInfo *pindialog)
{
	if(strlen(pindialog->pin)>=MIN_PIN_LENGTH)
	{
		gtk_dialog_set_response_sensitive(GTK_DIALOG(pindialog->dialog),GTK_RESPONSE_OK, TRUE);
		gtk_dialog_set_default_response(GTK_DIALOG(pindialog->dialog),GTK_RESPONSE_OK);
		gtk_widget_grab_focus(pindialog->okbutton);
		
	}
	else
	{
		gtk_dialog_set_response_sensitive(GTK_DIALOG(pindialog->dialog), GTK_RESPONSE_OK, FALSE);
		gtk_dialog_set_default_response(GTK_DIALOG(pindialog->dialog),GTK_RESPONSE_CANCEL);
		gtk_widget_grab_focus(pindialog->cancelbutton);
	}
}


// called when pin changed, updates pin label and OK button status
//////////////////////////////////////////////////////////////////
void pin_changed(PinDialogInfo *pindialog)
{
	update_pin_label(pindialog);
	update_ok_button(pindialog);
}

// add one digit at the end of the current pin
//////////////////////////////////////////////
static void add_digit(PinDialogInfo* pindialog, int digit)
{
	if(strlen(pindialog->pin)<MAX_PIN_LENGTH)
	{
		char tmp[MAX_PIN_LENGTH+1];
		snprintf(tmp,MAX_PIN_LENGTH+1,"%s%1d",pindialog->pin,digit);
		strcpy(pindialog->pin,tmp);
		pin_changed(pindialog);
	}
}

// remove one digit from the current pin
////////////////////////////////////////
static void backspace(PinDialogInfo* pindialog)
{
	if(strlen(pindialog->pin)>0)
	{
		pindialog->pin[strlen(pindialog->pin)-1]='\0';
		pin_changed(pindialog);
	}
}

// remove current pin entirely
///////////////////////////////////////
static void clear(PinDialogInfo* pindialog)
{
	pindialog->pin[0]='\0';
	pin_changed(pindialog);
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

// event handler for key presses on physical keyboard.
// handles digits and backspace by itself, passes all other keystrokes to default handler
/////////////////////////////////////////////////////////////////////////////////////////
gboolean on_key_press(GtkWidget * window, GdkEventKey*	pKey, gpointer _pindialog)
{
	PinDialogInfo* pindialog=(PinDialogInfo*)_pindialog;

   	if(pKey->type == GDK_KEY_PRESS)
	{
		guint32 ucChar=gdk_keyval_to_unicode(pKey->keyval);

		if(g_unichar_isdigit(ucChar))
		{
			add_digit(pindialog,g_unichar_digit_value(ucChar));
			return TRUE;
		}
		else if(pKey->keyval==GDK_BackSpace)
		{
			backspace(pindialog);
			return TRUE;
		}
	}
	return FALSE;
}

// initialise the bullet field, using a temporary get_entry to obtain the theme's current "invisible char"
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void pindialog_init(PinDialogInfo* pindialog)
{
	GtkWidget* entry=gtk_entry_new();
	gunichar invis=gtk_entry_get_invisible_char(GTK_ENTRY(entry));
	gtk_widget_destroy(entry);
	gint bullet_size=g_unichar_to_utf8(invis,pindialog->bullet);
	pindialog->bullet[bullet_size]='\0';
}

int main(int argc, char* argv[])
{
	int				return_value;
	PinDialogInfo 	pindialog;									// this struct contains all objects
	GdkColor 		color;
	char			caller_path[1024];

    gtk_init(&argc,&argv);										// initialize gtk+

	// create new message dialog with CANCEL and OK buttons in standard places, in center of user's screen
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if(get_parent_path(caller_path, sizeof(caller_path)-2)>0)
    {
        char message[2048];
		pindialog_init(&pindialog);									// setup PinDialogInfo structure
        snprintf(message, sizeof(message)-2, _MSG_(MSG_PLEASE_ENTER_PIN), caller_path);
        pindialog.dialog=gtk_message_dialog_new(NULL,GTK_DIALOG_MODAL,GTK_MESSAGE_QUESTION,GTK_BUTTONS_NONE,message);
    }
    else
    {
        fprintf(stderr,"Failed To Determine Parent Process. Aborting.\n");
        exit(EXIT_ERROR);
    }

	pindialog.cancelbutton=gtk_dialog_add_button(pindialog.dialog,GTK_STOCK_CANCEL,	GTK_RESPONSE_CANCEL);	
	pindialog.okbutton	  =gtk_dialog_add_button(pindialog.dialog,GTK_STOCK_OK,		GTK_RESPONSE_OK);	

	gtk_dialog_set_default_response(GTK_DIALOG(pindialog.dialog),GTK_RESPONSE_OK);
    gtk_window_set_title(GTK_WINDOW(pindialog.dialog),_MSG_(MSG_PIN_CODE_REQUIRED));
    gtk_window_set_position(GTK_WINDOW(pindialog.dialog), GTK_WIN_POS_CENTER);
    g_signal_connect (pindialog.dialog,"delete-event",G_CALLBACK(on_delete_event),&pindialog);

	// create on-screen numeric keypad, connect the digit and action keys to their event handlers
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	pindialog.table=gtk_table_new(4,3,TRUE);	// table of 4 rows, 3 columns

	// digit 0
	pindialog.digits[0]=gtk_button_new_with_label("0");
	gtk_table_attach(GTK_TABLE(pindialog.table),pindialog.digits[0],1,2,3,4,(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),2,2);
	GTK_WIDGET_UNSET_FLAGS(pindialog.digits[0],GTK_CAN_FOCUS);
//	gtk_widget_set_can_focus(pindialog.digits[0],FALSE);
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
		GTK_WIDGET_UNSET_FLAGS(pindialog.digits[i],GTK_CAN_FOCUS);
//		gtk_widget_set_can_focus(pindialog.digits[i],FALSE);
		g_signal_connect(pindialog.digits[i],"clicked",G_CALLBACK(on_key_digit),(gpointer)&pindialog);
	}

	// backspace button
	pindialog.backspace=gtk_button_new();
	gtk_container_add(GTK_CONTAINER(pindialog.backspace),gtk_image_new_from_stock(GTK_STOCK_GO_BACK,GTK_ICON_SIZE_SMALL_TOOLBAR));
	gtk_table_attach(GTK_TABLE(pindialog.table),pindialog.backspace,0,1,3,4,(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),2,2);
	GTK_WIDGET_UNSET_FLAGS(pindialog.backspace,GTK_CAN_FOCUS);
//	gtk_widget_set_can_focus(pindialog.backspace,FALSE);
	g_signal_connect(pindialog.backspace,"clicked",G_CALLBACK(on_key_backspace),(gpointer)&pindialog);

	// clear button
	pindialog.clear=gtk_button_new();
	gtk_container_add(GTK_CONTAINER(pindialog.clear),gtk_image_new_from_stock(GTK_STOCK_CLEAR,GTK_ICON_SIZE_SMALL_TOOLBAR));
	gtk_table_attach(GTK_TABLE(pindialog.table),pindialog.clear,2,3,3,4,(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),2,2);
	GTK_WIDGET_UNSET_FLAGS(pindialog.clear,GTK_CAN_FOCUS);
//	gtk_widget_set_can_focus(pindialog.clear,FALSE);
	g_signal_connect(pindialog.clear,"clicked",G_CALLBACK(on_key_clear),(gpointer)&pindialog);

	// create special label with opaque background
	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	pindialog.pinLabel=gtk_label_new("");
	pindialog.eventBox=gtk_event_box_new();
	pindialog.pinFrame=gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(pindialog.pinFrame),GTK_SHADOW_ETCHED_IN);
    gdk_color_parse("white",&color);
	gtk_widget_modify_bg(pindialog.eventBox,GTK_STATE_NORMAL,&color);

	// add all these objects to the dialog
	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	gtk_container_add(GTK_CONTAINER(pindialog.eventBox),pindialog.pinLabel);
	gtk_container_add(GTK_CONTAINER(pindialog.pinFrame),pindialog.eventBox);
   	gtk_container_set_border_width(GTK_CONTAINER(pindialog.dialog),10);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pindialog.dialog)->vbox),pindialog.pinFrame, TRUE, TRUE,2);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pindialog.dialog)->vbox),pindialog.table, FALSE, FALSE,2);

	// capture key presses at dialog level (since we have no real entry fields)
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	g_signal_connect(pindialog.dialog,"key-press-event",G_CALLBACK (on_key_press),&pindialog);

	// reset the PIN to the empty string and update the dialog state to an empty PIN
 	///////////////////////////////////////////////////////////////////////////////////////////////////////////	
	pindialog.pin[0]='\0'; 
	pin_changed(&pindialog);

	// show all these widgets, and run the dialog as a modal dialog until it is closed by the user
	//////////////////////////////////////////////////////////////////////////////////////////////    

    gtk_widget_show_all(GTK_WIDGET(pindialog.dialog));
	
    switch(gtk_dialog_run(GTK_DIALOG(pindialog.dialog)))
	{
		case GTK_RESPONSE_OK:					// if the use chose OK
			printf("%s\n",pindialog.pin);		// output the PIN to stdout
			return_value=EXIT_OK;
		break;

		case GTK_RESPONSE_CANCEL:
			printf("CANCEL\n");					// output CANCEL
			return_value=EXIT_OK;
		break;

		default:								// otherwise
			printf("ERROR\n");					// output ERROR
			return_value=EXIT_ERROR;
		break;
	}

	// properly dispose of the dialog (which disposes of all it's children), and exit with specific return value
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

	gtk_widget_destroy(pindialog.dialog);
	exit(return_value);
}
