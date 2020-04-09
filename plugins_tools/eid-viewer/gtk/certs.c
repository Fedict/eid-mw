#include <config.h>
#include "certs.h"
#include "prefs.h"
#include "gtk_globals.h"
#include "glib_util.h"
#include "main.h"

#include <string.h>
#include <ctype.h>

#include <gtk/gtk.h>
#include <gtk/logging.h>

#include <curl/curl.h>
#include <proxy.h>

#include <openssl/x509.h>
#include <openssl/x509v3.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#include <locale.h>
#include <gettext.h>
#include <errno.h>
#include <eid-viewer/verify_cert.h>

enum certs {
	Root,
	CA,
	CERT_RN_FILE,
	Authentication,
	Signature,
	CERTS_COUNT,
};

#define _(s) gettext(s)

static GtkTreeStore* certificates = NULL;
static GtkTreeIter* iters[CERTS_COUNT];
static GdkPixbuf* good_certificate;
static GdkPixbuf* bad_certificate;
static GdkPixbuf* warn_certificate;
static GdkPixbuf* unchecked_certificate;
static pxProxyFactory *pf;

/* Returns a GtkTreeIter for the part of the tree model that refers to the
 * certificate with the given name */
static GtkTreeIter* get_iter_for(char* which) {
	GtkTreeIter* parent;
	enum certs w = CERTS_COUNT;

#define char_to_enum(c) if(!strcmp(#c, which)) { w = c; }
	char_to_enum(Root);
	char_to_enum(CA);
	char_to_enum(CERT_RN_FILE);
	char_to_enum(Authentication);
	char_to_enum(Signature);
#undef char_to_enum

	if(w == CERTS_COUNT) {
		/* invalid/unknown certificate */
		return NULL;
	}
	if(iters[w]) {
	       	return iters[w];
	}
	switch(w) {
		case CA:
		case CERT_RN_FILE:
			parent = get_iter_for("Root");
			break;
		case Authentication:
		case Signature:
			parent = get_iter_for("CA");
			break;
		default:
			parent = NULL;
	}
	iters[w] = malloc(sizeof(GtkTreeIter));
	gtk_tree_store_append(certificates, iters[w], parent);
	return iters[w];
}

struct tree_store_data {
	char* which;
	gint* columns;
	GValue* values;
	gint n_values;
	void(*free)(struct tree_store_data*);
};

/* Free the data for the tree store */
static void tst_free_simple(struct tree_store_data* dat) {
	int i;
	free(dat->which);
	free(dat->columns);
	for(i=0;i<dat->n_values; i++) {
		g_value_unset(&(dat->values[i]));
	}
	free(dat->values);
	free(dat);
}

/* Helper function for tst_set() */
static gboolean tst_helper(gpointer user_data) {
	struct tree_store_data* dat = (struct tree_store_data*)user_data;
	GtkTreeIter* iter = get_iter_for(dat->which);

	if(!iter) {
		return FALSE;
	}

	gtk_tree_store_set_valuesv(certificates, iter, dat->columns, dat->values, dat->n_values);
	dat->free(dat);

	return FALSE;
}

/* Set the given data on the tree model */
static void tst_set(char* w, gint* c, GValue* v, gint n) {
	struct tree_store_data* dat = malloc(sizeof(struct tree_store_data));
	dat->which = strdup(w);
	dat->columns = c;
	dat->values = v;
	dat->n_values = n;
	dat->free = tst_free_simple;
	/* GTK+ does not like it when you modify UI elements from a background
	 * thread, so ensure it's called on the main thread instead */
	g_main_context_invoke(NULL, tst_helper, dat);
}

static gboolean trigger_update_info(gpointer user_data G_GNUC_UNUSED) {
	GtkTreeSelection *sel = gtk_tree_view_get_selection(
			GTK_TREE_VIEW(gtk_builder_get_object(builder, "tv_cert")));
	update_info(sel, NULL);

	return FALSE;
}

/* Helper function to initialize the tree store if that hasn't happened yet */
static void ensure_cert() {
	if(G_UNLIKELY(certificates == NULL)) {
		certificates = gtk_tree_store_new(CERT_COL_NCOLS,
				G_TYPE_STRING, // label (short description)
				GDK_TYPE_PIXBUF, // image
				G_TYPE_STRING, // valid from
				G_TYPE_BOOLEAN, // valid from in past?
				G_TYPE_STRING, // valid through 
				G_TYPE_BOOLEAN, // valid through in future?
				G_TYPE_STRING, // use
				G_TYPE_STRING, // validity
				G_TYPE_STRING, // description (multi-line field 
				G_TYPE_BYTE_ARRAY); // data (GByteArray*)
	}
}

static void create_proxy_factory() {
	pf = px_proxy_factory_new();
}

struct recvdata {
	unsigned char* data;
	size_t len;
};

/* CURL helper function to receive data */
static size_t appendmem(char *ptr, size_t size, size_t nmemb, void* data) {
	struct recvdata *str = (struct recvdata*) data;
	size_t realsize = size * nmemb;
	unsigned char *p = realloc(str->data, str->len + realsize);

	if(!p) {
		return 0;
	}
	str->data = p;
	memcpy(str->data + str->len, ptr, realsize);
	str->len += realsize;

	return realsize;
}

static const void* perform_curl_request(char *url, CURL *curl, long *retlen) {
	int i=0;
	char** proxies = px_proxy_factory_get_proxies(pf, url);
	void* retval;
	CURLcode curl_res;
	struct recvdata *dat;

	dat = calloc(sizeof(struct recvdata), 1);
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, appendmem);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, dat);
	do {
		if((curl_res = curl_easy_perform(curl)) != CURLE_OK) {
			uilog(EID_VWR_LOG_COARSE, "Could not perform OCSP request (with proxy: %s): %s",
					proxies[i] ? "none" : proxies[i],
					curl_easy_strerror(curl_res));
		}
		if(proxies[i] == NULL) {
			curl_easy_setopt(curl, CURLOPT_PROXY, "");
		} else {
			if(!strcmp(proxies[i], "direct://")) {
				// skip that
				i++;
				curl_easy_setopt(curl, CURLOPT_PROXY, "");
			} else {
				curl_easy_setopt(curl, CURLOPT_PROXY, proxies[i]);
			}
		}
	} while(proxies[i++] != NULL && curl_res != CURLE_OK);

	for(i=0; proxies[i]; i++) {
		free(proxies[i]);
	}
	free(proxies);

	if(curl_res != CURLE_OK) {
		free(dat->data);
		dat->len = 0;
		dat->data = NULL;
	}
	retval = dat->data;
	*retlen = dat->len;

	free(dat);
	curl_easy_cleanup(curl);
	return retval;
}

static const void* perform_ocsp_request(char* url, void* data, long datlen, long* retlen, void** handle) {
	CURL *curl;
	struct curl_slist *list = NULL;
	const void *retval;

	list = curl_slist_append(list, "Content-Type: application/ocsp-request");
	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_POST, (long)1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, datlen);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
	retval = perform_curl_request(url, curl, retlen);
	*handle = (void*)retval;
	curl_slist_free_all(list);
	return retval;
}

static const void* perform_http_request(char* url, long *retlen, void** handle) {
	CURL *curl = curl_easy_init();
	return (*handle = (void*)perform_curl_request(url, curl, retlen));
}

static enum eid_vwr_result check_cert(char* which) {
	GtkTreeIter *cert_iter = get_iter_for(which);
	GtkTreeIter *ca_iter;
	GByteArray *cert, *ca_cert;
	GValue *val_cert, *val_ca, *val_root;
	GValue *val_tcert, *val_tca, *val_troot;
	int *col_cert, *col_ca, *col_root;
	int *col_tcert, *col_tca, *col_troot;
	enum eid_vwr_result verify_result;

	if(strcmp(which, "CA")==0) {
		ca_iter = get_iter_for("Root");
	} else {
		ca_iter = get_iter_for("CA");
	}

	gtk_tree_model_get(GTK_TREE_MODEL(certificates), cert_iter, CERT_COL_DATA, &cert, -1);
	gtk_tree_model_get(GTK_TREE_MODEL(certificates), ca_iter, CERT_COL_DATA, &ca_cert, -1);

	val_cert = calloc(sizeof(GValue), 1);
	col_cert = malloc(sizeof(int));
	val_tcert = calloc(sizeof(GValue), 1);
	col_tcert = malloc(sizeof(int));

	*col_cert = CERT_COL_IMAGE;
	g_value_init(val_cert, GDK_TYPE_PIXBUF);
	*col_tcert = CERT_COL_VALIDITY;
	g_value_init(val_tcert, G_TYPE_STRING);
	if(strcmp(which, "CERT_RN_FILE") == 0) {
		verify_result = eid_vwr_verify_rrncert(cert->data, cert->len);
	} else if(strcmp(which, "CA") == 0) {
		verify_result = eid_vwr_verify_int_cert(cert->data, cert->len, ca_cert->data, ca_cert->len, perform_http_request, free);
	} else {
		verify_result = eid_vwr_verify_cert(cert->data, cert->len, ca_cert->data, ca_cert->len, perform_ocsp_request, free);
	}

	switch(verify_result) {
		case EID_VWR_RES_SUCCESS:
			g_value_set_instance(val_cert, good_certificate);
			g_value_set_string(val_tcert, _("trusted"));
			break;
		case EID_VWR_RES_FAILED:
			g_value_set_instance(val_cert, bad_certificate);
			g_value_set_string(val_tcert, _("NOT TRUSTED"));
			break;
		case EID_VWR_RES_UNKNOWN:
			g_value_set_instance(val_cert, warn_certificate);
			g_value_set_string(val_tcert, _("validation failed"));
			break;
		default:
			free(val_cert);
			free(col_cert);
			free(val_tcert);
			free(col_tcert);
			return verify_result;
	}
	col_ca = malloc(sizeof(int));
	val_ca = calloc(sizeof(GValue), 1);
	col_tca = malloc(sizeof(int));
	val_tca = calloc(sizeof(GValue), 1);
	col_root = malloc(sizeof(int));
	val_root = calloc(sizeof(GValue), 1);
	col_troot = malloc(sizeof(int));
	val_troot = calloc(sizeof(GValue), 1);
	*col_ca = *col_root = *col_cert;
	*col_tca = *col_troot = *col_tcert;
	g_value_init(val_ca, GDK_TYPE_PIXBUF);
	g_value_copy(val_cert, val_ca);
	g_value_init(val_root, GDK_TYPE_PIXBUF);
	g_value_copy(val_cert, val_root);
	g_value_init(val_tca, G_TYPE_STRING);
	g_value_copy(val_tcert, val_tca);
	g_value_init(val_troot, G_TYPE_STRING);
	g_value_copy(val_tcert, val_troot);
	tst_set(which, col_cert, val_cert, 1);
	tst_set(which, col_tcert, val_tcert, 1);
	tst_set("CA", col_ca, val_ca, 1);
	tst_set("CA", col_tca, val_tca, 1);
	tst_set("Root", col_root, val_root, 1);
	tst_set("Root", col_troot, val_troot, 1);

	g_main_context_invoke(NULL, trigger_update_info, NULL);

	return verify_result;
}

#define worst(res, new) (res == EID_VWR_RES_FAILED ? res : new)

static void* check_certs_thread(void* splat G_GNUC_UNUSED) {
	static pthread_once_t once = PTHREAD_ONCE_INIT;
	enum eid_vwr_result res = EID_VWR_RES_UNKNOWN;

	pthread_once(&once, create_proxy_factory);
	if(!pf) {
		uilog(EID_VWR_LOG_ERROR, "Certificate validation: Could not look up proxy");
		return NULL;
	}

	if(iters[CA] == NULL) {
		uilog(EID_VWR_LOG_NORMAL, "Certificate validation failed: no CA certificate found");
		return NULL;
	}

	if(iters[Root] == NULL) {
		uilog(EID_VWR_LOG_ERROR, "Certificate validation failed: no Root certificate found");
		return NULL;
	}

	res = check_cert("CA");
	if(iters[Signature] != NULL) {
		res = worst(res, check_cert("Signature"));
	}
	if(iters[Authentication] != NULL) {
		res = worst(res, check_cert("Authentication"));
	}
	if(iters[CERT_RN_FILE] != NULL) {
		res = worst(res, check_cert("CERT_RN_FILE"));
	}
	if(res == EID_VWR_RES_FAILED) {
		uilog(EID_VWR_LOG_ERROR, _("One or more certificates of the certificates on this card were found to be invalid or revoked. For more information, please see the log tab"));
	}
	return NULL;
}

void validate_all(gpointer event_source G_GNUC_UNUSED, gpointer user_data G_GNUC_UNUSED) {
	pthread_t thread;
	pthread_create(&thread, NULL, check_certs_thread, NULL);
}

/* newbindata() handler function for when we receive a certificate */
void add_certificate(char* label, void* data, int len) {
	X509 *cert = NULL;
	BIO *bio = BIO_new(BIO_s_mem());
	char *buf;
	size_t size;
	gint cols=9;
	gint *columns;
	gint cur=0;
	GValue *vals;
	GByteArray* ba;

	ensure_cert();

	columns = calloc(sizeof(gint),cols);
	vals = calloc(sizeof(GValue),cols);

	/* d2i_X509 moves its input pointer, so make sure we have a copy before
	 * we call that function */
	columns[cur] = CERT_COL_DATA;
	g_value_init(&(vals[cur]), G_TYPE_BYTE_ARRAY);
	ba = g_byte_array_sized_new(len);
	g_byte_array_append(ba, data, len);
	g_value_take_boxed(&(vals[cur++]), ba);

	if(d2i_X509(&cert, (const unsigned char**)&data, len) == NULL) {
		g_warning("Could not parse %s certificate", label);
		free(columns);
		free(vals);
		return;
	}

	/* Now fill all the other columns with the parsed-out data from the
	 * certificate */
	columns[cur] = CERT_COL_LABEL;
	g_value_init(&(vals[cur]), G_TYPE_STRING);
	char* str = eid_vwr_describe_cert(label, cert);
	g_value_take_string(&(vals[cur++]), g_strdup(str));
	free(str);

	columns[cur] = CERT_COL_IMAGE;
	g_value_init(&(vals[cur]), GDK_TYPE_PIXBUF);
	g_value_set_instance(&(vals[cur++]), unchecked_certificate);

	ASN1_TIME_print(bio, X509_get_notBefore(cert));
	buf = malloc((size = BIO_ctrl_pending(bio)) + 1);
	BIO_read(bio, buf, (int)size);
	buf[size] = '\0';
	columns[cur] = CERT_COL_VALIDFROM;
	g_value_init(&(vals[cur]), G_TYPE_STRING);
	g_value_set_string(&(vals[cur++]), buf);
	
	ASN1_TIME_print(bio, X509_get_notAfter(cert));
	buf = malloc((size = BIO_ctrl_pending(bio)) + 1);
	BIO_read(bio, buf, (int)size);
	buf[size] = '\0';
	columns[cur] = CERT_COL_VALIDTO;
	g_value_init(&(vals[cur]), G_TYPE_STRING);
	g_value_set_string(&(vals[cur++]), buf);

	columns[cur] = CERT_COL_DESC;
	g_value_init(&(vals[cur]), G_TYPE_STRING);
	g_value_take_string(&(vals[cur++]), eid_vwr_detail_cert(label, cert));

	columns[cur] = CERT_COL_USE;
	g_value_init(&(vals[cur]), G_TYPE_STRING);
	g_value_take_string(&(vals[cur++]), eid_vwr_get_use_flags(label, cert));

	columns[cur] = CERT_COL_VALIDFROM_PAST;
	g_value_init(&(vals[cur]), G_TYPE_BOOLEAN);
	g_value_set_boolean(&(vals[cur++]), (X509_cmp_current_time(X509_get_notBefore(cert)) < 0) ? FALSE : TRUE);

	columns[cur] = CERT_COL_VALIDTO_FUTURE;
	g_value_init(&(vals[cur]), G_TYPE_BOOLEAN);
	g_value_set_boolean(&(vals[cur++]), (X509_cmp_current_time(X509_get_notAfter(cert)) > 0) ? FALSE : TRUE);

	BIO_free(bio);

	tst_set(label, columns, vals, cols);
}

/* Return the tree model for the certificates treeview */
GtkTreeModel* certificates_get_model() {
	ensure_cert();
	return GTK_TREE_MODEL(certificates);
}

/* Helper function for clear_certdata() */
gboolean real_clear(gpointer data G_GNUC_UNUSED) {
	int i;
	gtk_tree_store_clear(certificates);
	for(i=0; i<CERTS_COUNT; i++) {
		iters[i]=NULL;
	}
	return FALSE;
}

/* Initialize the data needed for the certificate */
void certs_init() {
	GSettings* sets = get_prefs();
	g_settings_bind(sets, "validate",
			G_OBJECT(gtk_builder_get_object(builder, "validate_always")),
			"active", 0);
	G_GNUC_BEGIN_IGNORE_DEPRECATIONS
	unchecked_certificate = gdk_pixbuf_from_pixdata(&certificate_large, FALSE, NULL);
	bad_certificate = gdk_pixbuf_from_pixdata(&certificate_bad, FALSE, NULL);
	warn_certificate = gdk_pixbuf_from_pixdata(&certificate_warn, FALSE, NULL);
	good_certificate = gdk_pixbuf_from_pixdata(&certificate_checked, FALSE, NULL);
	G_GNUC_END_IGNORE_DEPRECATIONS
}

/* Clear the certificates treeview */
void clear_certdata() {
	g_main_context_invoke(NULL, real_clear, NULL);
}

/* Export a certificate to a file. Called by a menu option in the right-mouse
 * popup menu on the treeview */
void certexport(GtkMenuItem* item G_GNUC_UNUSED, gpointer userdata) {
	GtkWindow* win = GTK_WINDOW(gtk_builder_get_object(builder, "mainwin"));
	GtkWidget* dialog = gtk_file_chooser_dialog_new(
			_("Save eID file"), win, GTK_FILE_CHOOSER_ACTION_SAVE,
		       	_("Cancel"), GTK_RESPONSE_CANCEL,
		       	_("Save"), GTK_RESPONSE_ACCEPT,
			NULL);
	gchar* filename_sugg;
	gchar* desc;
	gint res;
	GtkFileFilter* filter;
	GtkTreeSelection* sel = gtk_tree_view_get_selection(
			GTK_TREE_VIEW(gtk_builder_get_object(builder, "tv_cert")));
	GtkTreeIter iter;
	GtkTreeModel* model;
	int s, d, len;

	/* Build a set of file filters for our "Save..." dialog */
	filter = gtk_file_filter_new();
	gtk_file_filter_add_pattern(filter, "*.pem");
	gtk_file_filter_set_name(filter, _("PEM files"));
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

	filter = gtk_file_filter_new();
	gtk_file_filter_add_pattern(filter, "*.pem");
	gtk_file_filter_add_pattern(filter, "*.der");
	gtk_file_filter_set_name(filter, _("PEM and DER files"));
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

	filter = gtk_file_filter_new();
	gtk_file_filter_add_pattern(filter, "*.der");
	gtk_file_filter_set_name(filter, _("DER files"));
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

	/* Generate a suggested filename based on the common name of the
	 * selected certificate */
	gtk_tree_selection_get_selected(sel, &model, &iter);
	gtk_tree_model_get(model, &iter, CERT_COL_LABEL, &desc, -1);
	if(strcmp((char*)userdata, "DER") == 0) {
		filename_sugg = g_strdup_printf("%s.der", desc);
	} else if(strcmp((char*)userdata, "chain") == 0) {
		filename_sugg = g_strdup_printf("%s_chain.pem", desc);
	} else {
		filename_sugg = g_strdup_printf("%s.pem", desc);
	}
	len = strlen(filename_sugg);
	for(s=0,d=0;s<len;s++) {
		switch(filename_sugg[s]) {
			case '(':
			case ')':
				break;
			case ' ':
				filename_sugg[d] = '_';
				d++;
				break;
			default:
				filename_sugg[d] = tolower(filename_sugg[s]);
				d++;
		}
	}
	filename_sugg[d]='\0';

	gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), filename_sugg);

	/* Show the dialog, and if the user accepts the selection, save to the given filename */
	res = gtk_dialog_run(GTK_DIALOG(dialog));
	if(res == GTK_RESPONSE_ACCEPT) {
		gchar* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		int fd;
		GByteArray* arr;

		gtk_tree_model_get(model, &iter, CERT_COL_DATA, &arr, -1);
		fd = open(filename, O_WRONLY | O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
		if(fd < 0) {
			uilog(EID_VWR_LOG_ERROR, _("Could not open file: %s"), strerror(errno));
			return;
		}
		eid_vwr_dumpcert(fd, arr->data, arr->len, strcmp((char*)userdata, "DER") ? DUMP_PEM : DUMP_DER);
		if(!strcmp((char*)userdata, "chain")) {
			GtkTreeIter child = iter;
			while(gtk_tree_model_iter_parent(model, &iter, &child)) {
				gtk_tree_model_get(model, &iter, CERT_COL_DATA, &arr, -1);
				eid_vwr_dumpcert(fd, arr->data, arr->len, DUMP_PEM);
				child = iter;
			}
		}
		close(fd);
	}
	g_free(filename_sugg);
	gtk_widget_destroy(dialog);
}

/* Show details on a given certificate. */
void certdetail(GtkMenuItem* item G_GNUC_UNUSED, gpointer userdata G_GNUC_UNUSED) {
	GtkWindow* win = GTK_WINDOW(gtk_builder_get_object(builder, "mainwin"));
	GtkTreeSelection *sel = gtk_tree_view_get_selection(
			GTK_TREE_VIEW(gtk_builder_get_object(builder, "tv_cert")));
	GtkTreeIter iter;
	GtkTreeModel *model;
	char* text;
	GByteArray *arr;

	gtk_tree_selection_get_selected(sel, &model, &iter);
	gtk_tree_model_get(model, &iter, CERT_COL_DATA, &arr, -1);

	text = eid_vwr_x509_get_details(arr->data, arr->len);
	GtkWidget* dialog = gtk_message_dialog_new(win, GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", text);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	free(text);
}
