#include "verify.h"
#include "photo.h"
#include "glib_util.h"
#include "p11.h"
#include "logging.h"

#include <string.h>

#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/bio.h>
#include <openssl/sha.h>

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
	int photovalid = photo_is_valid();
	BIO *bio;
	X509 *cert;
	EVP_PKEY* pubkey;
	void* address_data;
	unsigned char digest[SHA_DIGEST_LENGTH];
	struct storage_data *tmp, *datsign;

	if(photovalid <= 0) {
		return FALSE;
	}
	/* The photo_is_valid() function only verifies the photo's hash,
	 * it does not verify any signature. However, the photo hash is
	 * stored in the DATA_FILE (which is signed), so if that file's
	 * signature validates, then the photo is valid and signed, too.
	 */

	/* Make sure we have all the info we need. */
#define REQUIRE(s) if(!g_hash_table_contains(hash, s)) return FALSE
	REQUIRE("SIGN_DATA_FILE");
	REQUIRE("SIGN_ADDRESS_FILE");
	REQUIRE("ADDRESS_FILE");
	REQUIRE("DATA_FILE");
	REQUIRE("CERT_RN_FILE");
#undef REQUIRE

	/* Feed RRN certificate to OpenSSL */
	tmp = g_hash_table_lookup(hash, "CERT_RN_FILE");
	bio = BIO_new_mem_buf(tmp->data, tmp->len);
	cert = d2i_X509_bio(bio, NULL);
	pubkey = X509_get_pubkey(cert);
	if(EVP_PKEY_type(pubkey->type) != EVP_PKEY_RSA) {
		return FALSE;
	}

	tmp = g_hash_table_lookup(hash, "DATA_FILE");
	SHA1(tmp->data, tmp->len, digest);
	datsign = g_hash_table_lookup(hash, "SIGN_DATA_FILE");
	if(RSA_verify(NID_sha1, digest, sizeof(digest), datsign->data, datsign->len, EVP_PKEY_get1_RSA(pubkey)) != 1) {
		return FALSE;
	}

	tmp = g_hash_table_lookup(hash, "ADDRESS_FILE");
	address_data = g_malloc(tmp->len + datsign->len);
	memcpy(address_data, tmp->data, tmp->len);
	memcpy(address_data + tmp->len, datsign->data, datsign->len);
	if(memcmp(address_data, tmp->data, tmp->len)) {
		uilog(EID_VWR_LOG_COARSE, "The universe is weird!");
	}
	if(memcmp(address_data + tmp->len, datsign->data, datsign->len)) {
		uilog(EID_VWR_LOG_COARSE, "The universe is really weird!");
	}
	SHA1(address_data, tmp->len + datsign->len, digest);
	g_free(address_data);
	tmp = g_hash_table_lookup(hash, "SIGN_ADDRESS_FILE");
	if(RSA_verify(NID_sha1, digest, sizeof(digest), tmp->data, tmp->len, EVP_PKEY_get1_RSA(pubkey)) != 1) {
		return FALSE;
	}

	return TRUE;
}
