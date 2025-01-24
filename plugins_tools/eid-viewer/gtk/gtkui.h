#ifndef EID_VWR_GTKUI_H
#define EID_VWR_GTKUI_H

#include <gtk/gtk.h>
#include <eid-viewer/oslayer.h>

void file_open(GtkWidget * open, gpointer user_data);
void file_save(GtkWidget * open, gpointer format);
void file_close(GtkWidget * close, gpointer user_data);
void file_print(GtkWidget * print, gpointer user_data);
void translate(GtkWidget * langitem, gpointer to_lang);
void showabout(GtkWidget * about, gpointer user_data);
void showurl(GtkWidget * item, gpointer which);
void showlog(GtkWidget * item, gpointer user_data);
void pinop(GtkWidget * item, gpointer which);
void pinop_result(enum eid_vwr_pinops which, enum eid_vwr_result r);
void readers_changed(unsigned long nreaders, slotdesc * slots);
enum eid_vwr_langs get_curlang();
void show_cert_image(const char *name, void *data, int len);
void setup_dnd(void);
void disable_dnd(void);
#if GTK_CHECK_VERSION(4, 0, 0)
GdkContentProvider * drag_data_get(GtkDragSource *dragsrc, double x, double y, gpointer user_data);
#else
void drag_data_get(GtkWidget * widget, GdkDragContext * ctx,
		   GtkSelectionData * data, guint info, guint time,
		   gpointer user_data);
#endif
void validate_toggle(gpointer event_source, gpointer user_data);
void auto_reader(GtkWidget * mi, gpointer user_data);
void show_card_eu_start_date(char* doctype);
void update_doctype(char* label, void* data, int length);
extern gboolean is_foreigner;

#endif
