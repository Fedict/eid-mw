#include "verify.h"
#include "photo.h"

static GHashTable* hash = NULL;

static void init_table() {
	if(G_UNLIKELY(!hash)) {
		hash = g_hash_table_new(g_str_hash, g_str_equal);
	}
}

void add_verify_data(char* label, void* data, int len) {
	init_table();
	g_hash_table_insert(hash, label, data);
}

gboolean data_verifies() {
	int photovalid = photo_is_valid();
	if(photovalid < 0) {
		return FALSE;
	}
	if(!photovalid) {
		return FALSE;
	}

#define REQUIRE(s) if(!g_hash_table_contains(hash, s)) return FALSE
	REQUIRE("SIGN_DATA_FILE");
	REQUIRE("SIGN_ADDRESS_FILE");
	REQUIRE("ADDRESS_FILE");
	REQUIRE("DATA_FILE");
	REQUIRE("CERT_RN_FILE");
#undef REQUIRE
	// TODO: do actual verification
}
