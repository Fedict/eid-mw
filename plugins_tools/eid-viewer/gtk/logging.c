#include <string.h>

#include <glib.h>

#include <oslayer.h>

#include "gtk_globals.h"
#include "logging.h"
#include "prefs.h"

struct log_message {
	enum eid_vwr_loglevel l;
	gchar* msg;
};

static gboolean append_logline(gpointer ptr) {
	struct log_message *msg = (struct log_message*)ptr;
	if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(
					gtk_builder_get_object(builder, "mi_help_log")))) {
		if(msg->l >= gtk_combo_box_get_active(GTK_COMBO_BOX(
						gtk_builder_get_object(builder, "loglvl")))) {
			GtkTextView* tv = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "logtext"));
			GtkTextBuffer* buf = gtk_text_view_get_buffer(tv);
			GtkTextMark* mark = gtk_text_buffer_get_mark(buf, "logaddpos");
			GtkTextIter it;

			gtk_text_buffer_get_iter_at_mark(buf, &it, mark);
			gtk_text_buffer_insert(buf, &it, msg->msg, -1);
			gtk_text_view_scroll_to_mark(tv, mark, 0.0, FALSE, 0.0, 0.0);
		}
	}
	g_free(msg->msg);
	g_free(msg);
	return FALSE;
}

static void glib_message_redirect(const gchar* log_domain,
			   GLogLevelFlags log_level,
			   const gchar* message,
			   gpointer user_data) {
	enum eid_vwr_loglevel l;
	gchar* tmp;
	struct log_message* msg;

	g_log_default_handler(log_domain, log_level, message, user_data);
	switch(log_level) {
		case G_LOG_LEVEL_DEBUG:
			l = EID_VWR_LOG_DETAIL;
			tmp = "D: ";
			break;
		case G_LOG_LEVEL_INFO:
		case G_LOG_LEVEL_MESSAGE:
			l = EID_VWR_LOG_NORMAL;
			tmp = "N: ";
			break;
		case G_LOG_LEVEL_WARNING:
		case G_LOG_LEVEL_CRITICAL:
		case G_LOG_LEVEL_ERROR:
			l = EID_VWR_LOG_COARSE;
			tmp = "C: ";
			break;
		default:
			l = EID_VWR_LOG_COARSE;
			tmp = "U: ";
			break;
	}
	tmp = g_strdup_printf("%s%s\n", tmp, message);
	msg = g_new0(struct log_message, 1);
	msg->l = l;
	msg->msg = tmp;
	g_main_context_invoke(NULL, append_logline, msg);
}

static void clear_log(GtkButton* but G_GNUC_UNUSED, gpointer user_data G_GNUC_UNUSED) {
	GtkTextBuffer* buf = GTK_TEXT_BUFFER(gtk_text_view_get_buffer(
				GTK_TEXT_VIEW(gtk_builder_get_object(builder, "logtext"))));
	gtk_text_buffer_set_text(buf, "", -1);
}

static void copy_log(GtkButton* but G_GNUC_UNUSED, gpointer user_data G_GNUC_UNUSED) {
	GtkTextBuffer* buf = GTK_TEXT_BUFFER(gtk_text_view_get_buffer(
				GTK_TEXT_VIEW(gtk_builder_get_object(builder, "logtext"))));
	GtkTextIter start, end;
	gtk_text_buffer_get_start_iter(buf, &start);
	gtk_text_buffer_get_end_iter(buf, &end);
	gchar* text = gtk_text_buffer_get_text(buf, &start, &end, TRUE);
	GtkClipboard* clip = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	gtk_clipboard_set_text(clip, text, strlen(text));
}

static void switch_logtab(GtkCheckMenuItem* item, gpointer user_data G_GNUC_UNUSED) {
	gboolean active = gtk_check_menu_item_get_active(item);
	if(!active) {
		clear_log(NULL, NULL);
	}
	gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "logtab")), active);
}

static void reallog(enum eid_vwr_loglevel l, const char* line, va_list ap) {
	GLogLevelFlags gtklog;
	switch(l) {
		case EID_VWR_LOG_DETAIL:
			gtklog = G_LOG_LEVEL_DEBUG;
			break;
		case EID_VWR_LOG_NORMAL:
			gtklog = G_LOG_LEVEL_MESSAGE;
			break;
		case EID_VWR_LOG_COARSE:
			gtklog = G_LOG_LEVEL_WARNING;
			break;
	}
	g_logv(NULL, gtklog, line, ap);
}

void uilog(enum eid_vwr_loglevel l, const char* line, ...) {
	va_list ap, ac;
	va_start(ap, line);
	va_copy(ac, ap);
	reallog(l, line, ac);
	va_end(ac);
	va_end(ap);
}

logfunc log_init() {
	GObject* object;
	GtkTextBuffer* buf;
	GtkTextIter it;
	GSettings* sets = get_prefs();

	object = G_OBJECT(gtk_builder_get_object(builder, "mi_help_log"));
	g_signal_connect(object, "toggled", G_CALLBACK(switch_logtab), object);
	g_settings_bind(sets, "showlog", object, "active", 0);
	switch_logtab(GTK_CHECK_MENU_ITEM(object), NULL);

	object = G_OBJECT(gtk_builder_get_object(builder, "logclearbut"));
	g_signal_connect(object, "clicked", G_CALLBACK(clear_log), NULL);

	object = G_OBJECT(gtk_builder_get_object(builder, "logcopybut"));
	g_signal_connect(object, "clicked", G_CALLBACK(copy_log), NULL);

	object = G_OBJECT(gtk_builder_get_object(builder, "loglvl"));
	g_settings_bind(sets, "loglevel", object, "active", 0);

	buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(builder, "logtext")));
	gtk_text_buffer_get_end_iter(buf, &it);
	gtk_text_buffer_create_mark(buf, "logaddpos", &it, FALSE);

	g_log_set_default_handler(glib_message_redirect, NULL);

	return reallog;
}
