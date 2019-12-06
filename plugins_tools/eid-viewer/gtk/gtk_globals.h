#ifndef EID_VWR_UI_GTK_H
#define EID_VWR_UI_GTK_H

#include <gtk/gtk.h>

extern GHashTable *touched_labels;
extern GtkBuilder *builder;

#include <gdk-pixbuf/gdk-pixdata.h>

extern const GdkPixdata coat_of_arms_nl;
extern const GdkPixdata coat_of_arms_fr;
extern const GdkPixdata coat_of_arms_de;
extern const GdkPixdata coat_of_arms_en;
extern const GdkPixdata logo_128;
extern const GdkPixdata certificate_large;
extern const GdkPixdata certificate_bad;
extern const GdkPixdata certificate_checked;
extern const GdkPixdata certificate_warn;

#endif
