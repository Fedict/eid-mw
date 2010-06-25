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
#include <sys/stat.h>
#include <fcntl.h>


#define BEID_SECURE_PIN_DIALOG	"beid-secure-pin-dialog"

#define EXIT_OK		0
#define EXIT_CANCEL 1
#define EXIT_ERROR	2

// struct holding all the runtime data, so we can use callbacks without global variables
/////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
	GtkWidget  *dialog;
} PinDialogInfo;

// event handler for delete-event. always approves the deletion
///////////////////////////////////////////////////////////////
static gboolean on_delete_event( GtkWidget *widget, GdkEvent* event, gpointer pindialog)
{
    return TRUE;
}

void get_pid_path(char* path, size_t max_path_len)
{
	char default_tempdir[]="/tmp";
    char* tmpdir=getenv("TMPDIR");
    if(tmpdir==NULL)
        tmpdir=default_tempdir;
    snprintf(path,max_path_len,"%s/%s-%d.pid",tmpdir,BEID_SECURE_PIN_DIALOG,getuid());

}

int get_running_fd(char* progid)
{
	char 	tmppath[1024],tmpstr[32];
	pid_t	pid=0;
	int 	pidfile;

	get_pid_path(tmppath,sizeof(tmppath));
	
	pidfile=open(tmppath,O_RDONLY);
	if(pidfile>0)
	{
		size_t nread;
		if(nread=read(pidfile,tmpstr,sizeof(tmpstr)-2)>0)
		{
			tmpstr[nread]='\0';
			pid=atol(tmpstr);
			if(kill(pid,0))			// if not still running
			{
				unlink(tmppath);	// remove the pid file
				pid=0;				// and return "nothing found"
			}
		}
		close(pidfile);
	}

	return pid;
}

int set_running(char* progid)
{
	char 	tmppath[1024],tmpstr[32];
	pid_t	pid=0;
	int 	pidfile,result=0;

	get_pid_path(tmppath,sizeof(tmppath));
	pidfile=open(tmppath,O_WRONLY | O_CREAT | O_EXCL);
	if(pidfile>0)
	{
		size_t pidlen=0;
		snprintf(tmpstr,sizeof(tmpstr),"%d",getpid());
		pidlen=strlen(tmpstr);
		if(write(pidfile,tmpstr,pidlen)!=pidlen)
			result=1;
		close(pidfile);
	}
	return result;
}

int main(int argc, char* argv[])
{
	int 			return_value=EXIT_ERROR;
	PinDialogInfo 	pindialog;									// this struct contains all objects

	fprintf(stderr,"--- gtk_init()\n");
    gtk_init(&argc,&argv);										// initialize gtk+
	
	// create new message dialog with CANCEL button in standard places, in center of user's screen
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	fprintf(stderr,"--- creating dialog\n");
    pindialog.dialog=gtk_message_dialog_new(NULL,GTK_DIALOG_MODAL,GTK_MESSAGE_QUESTION,GTK_BUTTONS_CANCEL,argv[1]);
	gtk_dialog_set_default_response(GTK_DIALOG(pindialog.dialog),GTK_RESPONSE_OK);
    gtk_window_set_title(GTK_WINDOW(pindialog.dialog),"beid PIN Request");
    gtk_window_set_position(GTK_WINDOW(pindialog.dialog), GTK_WIN_POS_CENTER);
    g_signal_connect (pindialog.dialog, "delete-event", G_CALLBACK (on_delete_event),&pindialog);

	// show all these widgets, and run the dialog as a modal dialog until it is closed by the user
	//////////////////////////////////////////////////////////////////////////////////////////////    

	fprintf(stderr,"--- showing dialog\n");
    gtk_widget_show_all(GTK_WIDGET(pindialog.dialog));
	
	fprintf(stderr,"--- modal dialog\n");
    switch(gtk_dialog_run(GTK_DIALOG(pindialog.dialog)))
	{
		case GTK_RESPONSE_CANCEL:				// if the use chose CANCEL
			return_value=EXIT_CANCEL;
		break;

		default:								// otherwise
			return_value=EXIT_ERROR;			// output nothing and return CANCEL
		break;
	}

	// properly dispose of the dialog (which disposes of all it's children), and exit with specific return value
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

	fprintf(stderr,"--- dispose of dialog\n");
	gtk_widget_destroy(pindialog.dialog);
	exit(return_value);
}
