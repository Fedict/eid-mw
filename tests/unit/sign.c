/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2014 FedICT.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */
#ifdef WIN32
#include <win32.h>
#pragma pack(push, cryptoki, 1)
#include "pkcs11.h"
#pragma pack(pop, cryptoki)
#else
#include <unix.h>
#include <pkcs11.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "testlib.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdbool.h>

#if HAVE_OPENSSL
#include <openssl/opensslv.h>
#include <openssl/evp.h>
#include <openssl/engine.h>

// These were copied from eid-test-ca:derencode.c
int verify_sig(const unsigned char *sig_in, CK_ULONG siglen, const unsigned char *certificate, size_t certlen, bool is_rsa) {
#if OPENSSL_VERSION_NUMBER > 0x10100000L
	X509 *cert = NULL;
	EVP_PKEY *pkey = NULL;
	EVP_MD_CTX *mdctx;
	EVP_PKEY_CTX *pctx;
	const EVP_MD *md = EVP_get_digestbyname("sha256");
	unsigned char *sig = (unsigned char*)sig_in;

	if(d2i_X509(&cert, &certificate, certlen) == NULL) {
		fprintf(stderr, "E: could not parse X509 certificate\n");
		return TEST_RV_FAIL;
	}
	pkey = X509_get0_pubkey(cert);
	if(pkey == NULL) {
		fprintf(stderr, "E: could not find public key in certificate\n");
		return TEST_RV_FAIL;
	}
	mdctx = EVP_MD_CTX_new();
	if(EVP_DigestVerifyInit(mdctx, &pctx, md, NULL, pkey) != 1) {
		fprintf(stderr, "E: initialization for signature validation failed!\n");
		return TEST_RV_FAIL;
	}
	if(EVP_DigestVerifyUpdate(mdctx, (const unsigned char*)"foo", 3) != 1) {
		fprintf(stderr, "E: hashing for signature failed!\n");
		return TEST_RV_FAIL;
	}

	ECDSA_SIG* ec_sig;
	if(!is_rsa) {
		BIGNUM *r;
		BIGNUM *s;
		ec_sig = ECDSA_SIG_new();
		if((r = BN_bin2bn(sig, siglen / 2, NULL)) == NULL) {
			fprintf(stderr, "E: could not convert R part of ECDSA signature!\n");
			return TEST_RV_FAIL;
		}
		if((s = BN_bin2bn(sig + (siglen / 2), siglen / 2, NULL)) == NULL) {
			fprintf(stderr, "E: could not convert S part of ECDSA signature!\n");
			return TEST_RV_FAIL;
		}
		if(ECDSA_SIG_set0(ec_sig, r, s) == 0) {
			fprintf(stderr, "E: could not set ECDSA_SIG structure!\n");
			return TEST_RV_FAIL;
		}
		siglen = i2d_ECDSA_SIG(ec_sig, NULL);
		unsigned char *dersig = sig = malloc(siglen);
		siglen = i2d_ECDSA_SIG(ec_sig, &dersig);
	}
	if(EVP_DigestVerifyFinal(mdctx, sig, siglen) != 1) {
		fprintf(stderr, "E: signature fails validation!\n");
		return TEST_RV_FAIL;
	}
	if(!is_rsa) {
		free(sig);
	}
	printf("signature verified\n");
	return TEST_RV_OK;
#else
	printf("OpenSSL too old for verification\n");
#endif
}
#endif

int test_key(char* label, CK_SESSION_HANDLE session, CK_SLOT_ID slot) {
	CK_ATTRIBUTE attr[2];
	CK_MECHANISM mech;
	CK_BYTE data[] = { 'f', 'o', 'o' };
	CK_BYTE_PTR sig, mod, exp;
	CK_ULONG sig_len, type, count;
	CK_OBJECT_HANDLE privatekey, publickey, certificate;
	CK_KEY_TYPE keytype;
	bool is_rsa = false;

	attr[0].type = CKA_CLASS;
	attr[0].pValue = &type;
	type = CKO_PRIVATE_KEY;
	attr[0].ulValueLen = sizeof(CK_ULONG);

	attr[1].type = CKA_LABEL;
	attr[1].pValue = label;
	attr[1].ulValueLen = strlen(label);

	check_rv(C_FindObjectsInit(session, attr, 2));
	check_rv(C_FindObjects(session, &privatekey, 1, &count));
	verbose_assert(count == 1 || count == 0);
	check_rv(C_FindObjectsFinal(session));

	if(count == 0) {
		fprintf(stderr, "Cannot test \"%s\" signature on a card without a \"%s\" key\n", label, label);
		return TEST_RV_SKIP;
	}
	
	attr[0].type = CKA_KEY_TYPE;
	attr[0].pValue = &keytype;
	attr[0].ulValueLen = sizeof(keytype);

	check_rv(C_GetAttributeValue(session, privatekey, attr, 1));

	verbose_assert(keytype == CKK_RSA || keytype == CKK_EC);
	if(keytype == CKK_RSA) {
		is_rsa = true;
		mech.mechanism = CKM_SHA256_RSA_PKCS;
	} else {
		is_rsa = false;
		mech.mechanism = CKM_ECDSA_SHA256;
	}
	check_rv(C_SignInit(session, &mech, privatekey));

	check_rv(C_Sign(session, data, sizeof(data), NULL, &sig_len));
	sig = malloc(sig_len);
	check_rv(C_Sign(session, data, sizeof(data), sig, &sig_len));

	printf("Received a signature with length %lu:\n", sig_len);

	hex_dump((char*)sig, sig_len);

	if(is_rsa) {
		type = CKO_PUBLIC_KEY;
		check_rv(C_FindObjectsInit(session, attr, 2));
		check_rv(C_FindObjects(session, &publickey, 1, &count));
		verbose_assert(count == 1);
		check_rv(C_FindObjectsFinal(session));

		attr[0].type = CKA_MODULUS;
		attr[0].pValue = NULL_PTR;
		attr[0].ulValueLen = 0;

		attr[1].type = CKA_PUBLIC_EXPONENT;
		attr[1].pValue = NULL_PTR;
		attr[1].ulValueLen = 0;

		check_rv(C_GetAttributeValue(session, publickey, attr, 2));

		verbose_assert(attr[0].ulValueLen == sig_len);

		mod = malloc(attr[0].ulValueLen);
		mod[0] = 0xde; mod[1] = 0xad; mod[2] = 0xbe; mod[3] = 0xef;
		exp = malloc(attr[1].ulValueLen);
		exp[0] = 0xde; exp[1] = 0xad; exp[2] = 0xbe; exp[3] = 0xef;

		attr[0].pValue = mod;
		attr[1].pValue = exp;

		check_rv(C_GetAttributeValue(session, publickey, attr, 2));

		printf("Received key modulus with length %lu:\n", attr[0].ulValueLen);
		hex_dump((char*)mod, attr[0].ulValueLen);

		printf("Received public exponent of key with length %lu:\n", attr[1].ulValueLen);
		hex_dump((char*)exp, attr[1].ulValueLen);
	}

#if HAVE_OPENSSL && OPENSSL_VERSION_NUMBER > 0x10100000L
	unsigned char cert[4096];
	attr[0].type = CKA_CLASS;
	attr[0].pValue = &type;
	type = CKO_CERTIFICATE;
	attr[0].ulValueLen = sizeof(CK_ULONG);

	attr[1].type = CKA_LABEL;
	attr[1].pValue = label;
	attr[1].ulValueLen = strlen(label);

	check_rv(C_FindObjectsInit(session, attr, 2));
	check_rv(C_FindObjects(session, &certificate, 1, &count));
	verbose_assert(count == 1);
	check_rv(C_FindObjectsFinal(session));

	attr[0].type = CKA_VALUE;
	attr[0].pValue = cert;
	attr[0].ulValueLen = sizeof(cert);

	check_rv(C_GetAttributeValue(session, certificate, attr, 1));

	printf("Received certificate with length %lu:\n", attr[0].ulValueLen);
	hex_dump((char*)cert, attr[0].ulValueLen);

	return verify_sig(sig, sig_len, cert, attr[0].ulValueLen, is_rsa);
#else
	return TEST_RV_OK;
#endif
}

TEST_FUNC(sign) {
	int ret;
	CK_SESSION_HANDLE session;
	CK_SLOT_ID slot;

	if(!have_pin()) {
		fprintf(stderr, "Cannot test signature without a pin code\n");
		return TEST_RV_SKIP;
	}

	check_rv(C_Initialize(NULL_PTR));

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		check_rv(C_Finalize(NULL_PTR));
		return ret;
	}

	check_rv(C_OpenSession(slot, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session));

	if(!can_enter_pin(slot)) {
		return TEST_RV_SKIP;
	}

	if((ret = test_key("Authentication", session, slot)) != TEST_RV_OK) {
		return ret;
	}
	if((ret = test_key("Signature", session, slot)) != TEST_RV_OK) {
		return ret;
	}

	check_rv(C_Finalize(NULL_PTR));

	return TEST_RV_OK;
}
