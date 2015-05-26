#include "verify.h"
#include "photo.h"
#include "glib_util.h"
#include "p11.h"
#include "logging.h"

#include <string.h>

static GHashTable* hash = NULL;

struct storage_data {
	void* data;
	int len;
};

static void delete_val(gpointer data) {
	struct storage_data *dat = data;
	g_free(dat->data);
	g_free(dat);
}

static void init_table() {
	if(G_UNLIKELY(!hash)) {
		hash = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, delete_val);
	}
}

void add_verify_data(char* label, void* data, int len) {
	struct storage_data *dat = g_malloc(sizeof(struct storage_data));

	init_table();

	dat->data = g_malloc(len);
	memcpy(dat->data, data, len);
	dat->len = len;

	g_hash_table_insert(hash, g_strdup(label), dat);
}

gboolean data_verifies() {
	struct storage_data *data, *address, *datsig, *addsig, *cert;

	/* Make sure we have all the info we need. */
#define REQUIRE(s) if(!g_hash_table_contains(hash, s)) return FALSE
	REQUIRE("SIGN_DATA_FILE");
	REQUIRE("SIGN_ADDRESS_FILE");
	REQUIRE("ADDRESS_FILE");
	REQUIRE("DATA_FILE");
	REQUIRE("CERT_RN_FILE");
#undef REQUIRE

	/* Feed RRN certificate to OpenSSL */
	cert = g_hash_table_lookup(hash, "CERT_RN_FILE");

	data = g_hash_table_lookup(hash, "DATA_FILE");
	address = g_hash_table_lookup(hash, "ADDRESS_FILE");
	datsig = g_hash_table_lookup(hash, "SIGN_DATA_FILE");
	addsig = g_hash_table_lookup(hash, "SIGN_ADDRESS_FILE");
	const struct photo_info* pi = photo_get_data();
	return (check_data_validity(pi->raw, pi->plen,
				pi->hash, pi->hlen,
				data->data, data->len,
				datsig->data, datsig->len,
				address->data, address->len,
				addsig->data, addsig->len,
				cert->data, cert->len) == 1) ? TRUE : FALSE;
}
