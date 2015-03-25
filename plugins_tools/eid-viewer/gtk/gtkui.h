#ifndef EID_VWR_GTKUI_H
#define EID_VWR_GTKUI_H

#include <gtk/gtk.h>
#include "conversions.h"

void file_open(GtkMenuItem* open, gpointer user_data);
void file_save(GtkMenuItem* open, gpointer format);
void file_close(GtkMenuItem* close, gpointer user_data);
void file_prefs(GtkMenuItem* prefs, gpointer user_data);
void file_print(GtkMenuItem* print, gpointer user_data);
void translate(GtkMenuItem* langitem, gpointer to_lang);
void showabout(GtkMenuItem* about, gpointer user_data);
void showurl(GtkMenuItem* item, gpointer which);
void showlog(GtkMenuItem* item, gpointer user_data);
void pinop(GtkWidget* item, gpointer which);
enum eid_vwr_langs get_curlang();
void show_cert_image(const char* name, void* data, int len);

#endif
