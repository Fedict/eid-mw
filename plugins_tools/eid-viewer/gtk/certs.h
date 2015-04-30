#ifndef EID_VWR_GTK_CERTS_H
#define EID_VWR_GTK_CERTS_H

#include <gtk/gtk.h>
#include <certhelpers.h>

void add_certificate(char* label, void* data, int len);
void clear_certdata();
void certs_init();
GtkTreeModel* certificates_get_model();

#endif
