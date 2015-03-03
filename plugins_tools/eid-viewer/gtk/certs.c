#include "certs.h"

#include <gtk/gtk.h>

#include <openssl/x509.h>

enum columns {
	CERT_COL_LABEL,
	CERT_COL_IMAGE,
	CERT_COL_VALIDFROM,
	CERT_COL_VALIDTO,
	CERT_COL_USE,
	CERT_COL_VALIDITY,
	CERT_COL_DESC,
	CERT_COL_DATA,
	CERT_COL_NCOLS
};

enum certs {
	Root,
	CA,
	CERT_RN_FILE,
	Authentication,
	Signature,
	CERTS_COUNT,
};

static GtkTreeStore* certificates = NULL;
static GtkTreeIter* iters[CERTS_COUNT];
static GdkPixbuf* good_certificate;
static GdkPixbuf* bad_certificate;
static GdkPixbuf* unchecked_certificate;

static GtkTreeIter* get_iter_for(char* which) {
	GtkTreeIter* parent;
	enum certs w;

#define char_to_enum(c) if(!strcmp(#c, which)) { w = c; }
	char_to_enum(Root);
	char_to_enum(CA);
	char_to_enum(CERT_RN_FILE);
	char_to_enum(Authentication);
	char_to_enum(Signature);
#undef char_to_enum

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

const gchar* describe_cert(char* label, X509* cert) {
	return g_strdup_printf("TODO (%s)", label);
}

static void ensure_cert() {
	if(G_UNLIKELY(certificates == NULL)) {
		certificates = gtk_tree_store_new(CERT_COL_NCOLS,
				G_TYPE_STRING, // label (short description)
				GDK_TYPE_PIXBUF, // image
				G_TYPE_STRING, // valid from
				G_TYPE_STRING, // valid through 
				G_TYPE_STRING, // use
				G_TYPE_BOOLEAN, // validity
				G_TYPE_STRING, // description (multi-line field 
				G_TYPE_POINTER); // data (X509*)
	}
}

void add_certificate(char* label, void* data, int len) {
	X509 *cert = NULL;
	GtkTreeIter* iter;
	BIO *bio = BIO_new(BIO_s_mem());
	char *buf;
	size_t size;

	ensure_cert();
	if(!strcmp(label, "CERT_RN_FILE")) {
		add_verify_data(label, data, len);
	}
	if(d2i_X509(&cert, (const unsigned char**)&data, len) == NULL) {
		g_warning("Could not parse %s certificate", label);
		return;
	}
	iter = get_iter_for(label);
	gtk_tree_store_set(certificates, iter,
	       		CERT_COL_LABEL, describe_cert(label, cert),
			CERT_COL_IMAGE, unchecked_certificate,
			-1);

	ASN1_TIME_print(bio, X509_get_notBefore(cert));
	buf = malloc((size = BIO_ctrl_pending(bio)));
	BIO_read(bio, buf, (int)size);
	gtk_tree_store_set(certificates, iter,
			CERT_COL_VALIDFROM, buf,
			-1);
	
	ASN1_TIME_print(bio, X509_get_notAfter(cert));
	buf = malloc((size = BIO_ctrl_pending(bio)));
	BIO_read(bio, buf, (int)size);
	gtk_tree_store_set(certificates, iter,
			CERT_COL_VALIDTO, buf,
			-1);
}

GtkTreeModel* certificates_get_model() {
	ensure_cert();
	return GTK_TREE_MODEL(certificates);
}
