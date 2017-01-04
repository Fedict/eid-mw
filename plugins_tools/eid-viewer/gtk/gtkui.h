#ifndef EID_VWR_GTKUI_H
#define EID_VWR_GTKUI_H

#include <gtk/gtk.h>
#include <eid-viewer/oslayer.h>

void file_open(GtkMenuItem * open, gpointer user_data);
void file_save(GtkMenuItem * open, gpointer format);
void file_close(GtkMenuItem * close, gpointer user_data);
void file_print(GtkMenuItem * print, gpointer user_data);
void translate(GtkMenuItem * langitem, gpointer to_lang);
void showabout(GtkMenuItem * about, gpointer user_data);
void showurl(GtkMenuItem * item, gpointer which);
void showlog(GtkMenuItem * item, gpointer user_data);
void pinop(GtkWidget * item, gpointer which);
void pinop_result(enum eid_vwr_pinops which, enum eid_vwr_result r);
void readers_changed(unsigned long nreaders, slotdesc * slots);
enum eid_vwr_langs get_curlang();
void show_cert_image(const char *name, void *data, int len);
void setup_dnd(void);
void disable_dnd(void);
void drag_data_get(GtkWidget * widget, GdkDragContext * ctx,
		   GtkSelectionData * data, guint info, guint time,
		   gpointer user_data);
void validate_toggle(gpointer event_source, gpointer user_data);
void auto_reader(GtkCheckMenuItem * mi, gpointer user_data);
void update_doctype(char* label, void* data, int length);
extern gboolean is_foreigner;

#endif
