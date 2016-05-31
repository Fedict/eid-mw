#ifndef EID_VWR_GTK_VERIFY_H
#define EID_VWR_GTK_VERIFY_H

#include <gtk/gtk.h>

void add_verify_data(char* label, void* data, int len);
gboolean data_verifies();

#endif
