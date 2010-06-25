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
#include <limits.h>
#include <errno.h>

#define BEID_SECURE_PIN_DIALOG	"beid-secure-pin-dialog"
#define	MIN_CMDLINE_PATH_BYTES 14

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

int write_pid(char* pid_path)
{
	int 	pid_fd,result=-1;
	char 	tmp_str[32];

	do { pid_fd=open(pid_path,O_WRONLY | O_EXCL | O_CREAT | O_TRUNC); } while (pid_fd==-1 && errno==EINTR);
	if(pid_fd>0)
	{
		int nwritten=0;
		snprintf(tmp_str,sizeof(tmp_str),"%d",getpid());
		do { nwritten=write(pid_fd,tmp_str,strlen(tmp_str)); } while (nwritten==-1 && errno==EINTR);
		if(nwritten==strlen(tmp_str))
			result=0;
		close(pid_fd);
	}

	return result;
}

pid_t read_pid(char* pid_path)
{
	char	tmp_str[16];
	int 	pid_fd;
	pid_t	pid;

	do { pid_fd=open(pid_path,O_RDONLY); } while (pid_fd==-1 && errno==EINTR);
	if(pid_fd>0)
	{
		size_t nread=0;
		do { nread=read(pid_fd,tmp_str,sizeof(tmp_str)-2); } while (pid_fd==-1 && errno==EINTR);
		if(nread>0)						// if something read
		{
			tmp_str[nread]='\0';			// 0-terminate to make valid C string
			pid=strtoul(tmp_str,NULL,10);	// convert to pid_t
		}

		close(pid_fd);
		return pid;
	}
	return -1;
}
	

int read_proc_cmdline(char* cmdline, size_t max_cmd_size, pid_t pid)
{
	char 	tmp_str[32];
	ssize_t nread=0;
	
	if(snprintf(tmp_str,sizeof(tmp_str)-2,"/proc/%d/cmdline",pid) >= MIN_CMDLINE_PATH_BYTES)
	{
		int proc_fd;
		do { proc_fd=open(tmp_str,O_RDONLY); } while (proc_fd==-1 && errno==EINTR);

		if(proc_fd>0)
		{
			do { nread=read(proc_fd,cmdline,max_cmd_size); } while (proc_fd==-1 && errno==EINTR);
			if(nread>0)
				tmp_str[nread]='\0';			// but make sure the whole is properly terminated
			close(proc_fd);
			return 0;
		}
	}

	return -1;
}


int terminate(pid_t target)
{
	char 	tmp_str[PATH_MAX];
	int 	attempts;

	if((kill(target,0)==0) && (read_proc_cmdline(tmp_str,sizeof(tmp_str),target)==0) && (strstr(tmp_str,BEID_SECURE_PIN_DIALOG)==0))			// if it contains our command-line, it's a former instance
	{
		for(attempts=0;attempts<3 && kill(target,0)==0;attempts++)
		{
			kill(target,SIGTERM);									// tell it to make room for the living
			sleep(100000);											// give it 100ms to do so
		}

		for(attempts=0;attempts<3 && kill(target,0)==0;attempts++)
		{
			kill(target,SIGKILL);									// insist
			sleep(100000);											// give it 100ms to do so
		}
	}
}

int singleton_lock(char* pid_path)
{

	int	 	pid_fd			=write_pid(pid_path);
	int		abort			=0;

	while((pid_fd=write_pid(pid_path))<0 && !abort)		// while we can't acquire exclusive lock
	{
		pid_t interloper=read_pid(pid_path);			// read pid from interloper

		if(interloper>0)
			terminate(interloper);						// terminate it
		else
		{
			if(errno==ENOENT)							// .. because pid file was deleted 	
				continue;								// keep on trying

			if(unlink(pid_path)==-1)					// other reason for not being able to read; try to unlink the pid file
			{
				if(errno!=ENOENT)						// if failed for other reason than already gone.
					abort=1;							// we won't be able to get the lock, ever, abort.
			}
		}
	}

	if(pid_fd>0)
	{
		close(pid_fd);
		return 0;
	}

	return -1;
}

int singleton_unlock(char* pid_path)
{
	return unlink(pid_path);
}
	
int main(int argc, char* argv[])
{
	char			pid_path[PATH_MAX];
	int 			return_value=EXIT_ERROR;
	PinDialogInfo 	pindialog;									// this struct contains all objects

	get_pid_path(pid_path,sizeof(pid_path));					// get path to pid file
	if(singleton_lock(pid_path)==-1)							// establish pid lock
		exit(1);												// or abort if this is not possible

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

	singleton_unlock(pid_path);					// unlock the pid file lock
	exit(return_value);
}
