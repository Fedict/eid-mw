#ifndef EID_VWR_GTK_CERTS_H
#define EID_VWR_GTK_CERTS_H

#include <gtk/gtk.h>

void add_certificate(char* label, void* data, int len);
GtkTreeModel* certificates_get_model();

#endif
