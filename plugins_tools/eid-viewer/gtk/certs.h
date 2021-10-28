#ifndef EID_VWR_GTK_CERTS_H
#define EID_VWR_GTK_CERTS_H

#include <gtk/gtk.h>
#include <eid-viewer/certhelpers.h>

void add_certificate(char *label, void *data, int len);
void clear_certdata();
void certs_init();
GtkTreeModel *certificates_get_model();
void certexport(GtkMenuItem * mi, gpointer user_data);
void certdetail(GtkMenuItem * mi, gpointer user_data);
void validate_all(gpointer event_source, gpointer user_data);
const void* perform_http_request(char *url, long *retlen, void** handle);

#endif
