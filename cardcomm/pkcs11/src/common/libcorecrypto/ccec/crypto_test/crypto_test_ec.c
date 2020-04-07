/*
 * Copyright (c) 2012,2013,2014,2015 Apple Inc. All rights reserved.
 * 
 * corecrypto Internal Use License Agreement
 * 
 * IMPORTANT:  This Apple corecrypto software is supplied to you by Apple Inc. ("Apple")
 * in consideration of your agreement to the following terms, and your download or use
 * of this Apple software constitutes acceptance of these terms.  If you do not agree
 * with these terms, please do not download or use this Apple software.
 * 
 * 1.	As used in this Agreement, the term "Apple Software" collectively means and
 * includes all of the Apple corecrypto materials provided by Apple here, including
 * but not limited to the Apple corecrypto software, frameworks, libraries, documentation
 * and other Apple-created materials. In consideration of your agreement to abide by the
 * following terms, conditioned upon your compliance with these terms and subject to
 * these terms, Apple grants you, for a period of ninety (90) days from the date you
 * download the Apple Software, a limited, non-exclusive, non-sublicensable license
 * under Apple’s copyrights in the Apple Software to make a reasonable number of copies
 * of, compile, and run the Apple Software internally within your organization only on
 * devices and computers you own or control, for the sole purpose of verifying the
 * security characteristics and correct functioning of the Apple Software; provided
 * that you must retain this notice and the following text and disclaimers in all
 * copies of the Apple Software that you make. You may not, directly or indirectly,
 * redistribute the Apple Software or any portions thereof. The Apple Software is only
 * licensed and intended for use as expressly stated above and may not be used for other
 * purposes or in other contexts without Apple's prior written permission.  Except as
 * expressly stated in this notice, no other rights or licenses, express or implied, are
 * granted by Apple herein.
 * 
 * 2.	The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED WARRANTIES
 * OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, REGARDING
 * THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS,
 * SYSTEMS, OR SERVICES. APPLE DOES NOT WARRANT THAT THE APPLE SOFTWARE WILL MEET YOUR
 * REQUIREMENTS, THAT THE OPERATION OF THE APPLE SOFTWARE WILL BE UNINTERRUPTED OR
 * ERROR-FREE, THAT DEFECTS IN THE APPLE SOFTWARE WILL BE CORRECTED, OR THAT THE APPLE
 * SOFTWARE WILL BE COMPATIBLE WITH FUTURE APPLE PRODUCTS, SOFTWARE OR SERVICES. NO ORAL
 * OR WRITTEN INFORMATION OR ADVICE GIVEN BY APPLE OR AN APPLE AUTHORIZED REPRESENTATIVE
 * WILL CREATE A WARRANTY. 
 * 
 * 3.	IN NO EVENT SHALL APPLE BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) ARISING
 * IN ANY WAY OUT OF THE USE, REPRODUCTION, COMPILATION OR OPERATION OF THE APPLE
 * SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING
 * NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 * 
 * 4.	This Agreement is effective until terminated. Your rights under this Agreement will
 * terminate automatically without notice from Apple if you fail to comply with any term(s)
 * of this Agreement.  Upon termination, you agree to cease all use of the Apple Software
 * and destroy all copies, full or partial, of the Apple Software. This Agreement will be
 * governed and construed in accordance with the laws of the State of California, without
 * regard to its choice of law rules.
 * 
 * You may report security issues about Apple products to product-security@apple.com,
 * as described here:  https://www.apple.com/support/security/.  Non-security bugs and
 * enhancement requests can be made via https://bugreport.apple.com as described
 * here: https://developer.apple.com/bug-reporting/
 *
 * EA1350 
 * 10/5/15
 */

#include "testmore.h"
#include "testbyteBuffer.h"
#include "testccnBuffer.h"

// static int verbose = 1;

#if (CCEC == 0)
entryPoint(ccec,"ccec")
#else
#define USE_SUPER_COOL_NEW_CCOID_T
#include <corecrypto/ccec.h>
#include <corecrypto/ccec_priv.h>
#import <corecrypto/ccrng_test.h>
#import <corecrypto/ccsha1.h>
#import <corecrypto/ccsha2.h>
#import <corecrypto/ccrng_pbkdf2_prng.h>
#import <corecrypto/ccrng_sequence.h>
#include "crypto_test_ec.h"

static int verbose = 1;

static const int kTestTestCount = 2632;

static int saneKeySize(ccec_full_ctx_t fk) {
    switch(ccec_ctx_bitlen(fk)) {
        case 192:
        case 224:
        case 256:
        case 384:
        case 521: return 1;
        default: return 0;
    }
}

#define MAXKEYSPACE 8192

static int key_exchange(ccec_full_ctx_t key1, ccec_full_ctx_t key2)
{
    int status = 0; // fail
    byteBuffer shared_secret1 = mallocByteBuffer(MAXKEYSPACE);
    size_t outlen1=shared_secret1->len;
    byteBuffer shared_secret2 = mallocByteBuffer(MAXKEYSPACE);
    size_t outlen2=shared_secret2->len;
    
    ok_or_goto(ccecdh_compute_shared_secret(key1,key2,&outlen1,shared_secret1->bytes,global_test_rng)==0, "Compute secret 1", errout);
    
    ok_or_goto(ccecdh_compute_shared_secret(key2,key1,&outlen2,shared_secret2->bytes,global_test_rng)==0, "Compute secret 2", errout);
    
    ok_or_goto(outlen1 == outlen2, "Sign/Verify correct keysize", errout);
    ok_or_goto(memcmp(shared_secret1->bytes,shared_secret2->bytes,outlen1)==0, "Shared secrets match", errout);
    
    status = 1; // Success
    
errout:
    free(shared_secret1);
    free(shared_secret2);
    return status;
}

static int key_exchange_compact(ccec_full_ctx_t key1, ccec_full_ctx_t key2)
{
    int status = 0; // fail
    byteBuffer shared_secret1 = mallocByteBuffer(MAXKEYSPACE);
    size_t outlen1=shared_secret1->len;
    size_t export_pubsize1 = ccec_compact_export_size(0, key1);
    size_t export_pubsize2 = ccec_compact_export_size(0, key2);
    uint8_t exported_pubkey1[export_pubsize1];
    uint8_t exported_pubkey2[export_pubsize2];
    byteBuffer shared_secret2 = mallocByteBuffer(MAXKEYSPACE);
    size_t outlen2=shared_secret2->len;
    ccec_pub_ctx_decl_cp(ccec_ctx_cp(key1), reconstituted_pub1);
    ccec_pub_ctx_decl_cp(ccec_ctx_cp(key2), reconstituted_pub2);
    
    /* Export keys */
    ccec_compact_export(0, exported_pubkey2, key2);
    ccec_compact_export(0, exported_pubkey1, key1);
    
    /* Party 1 */
    ok_or_goto(ccec_compact_import_pub(ccec_ctx_cp(key2), export_pubsize2, exported_pubkey2, reconstituted_pub2)==0,
               "Import compact key", errout);
    ok_or_goto(ccecdh_compute_shared_secret(key1,reconstituted_pub2,&outlen2,shared_secret2->bytes,global_test_rng)==0, "Compute secret 1", errout);
    
    /* Party 2 */
    ok_or_goto(ccec_compact_import_pub(ccec_ctx_cp(key1), export_pubsize1, exported_pubkey1, reconstituted_pub1)==0,
               "Import compact key", errout);
    ok_or_goto(ccecdh_compute_shared_secret(key2,reconstituted_pub1,&outlen1,shared_secret1->bytes,global_test_rng)==0, "Compute secret 2", errout);
    
    /* Check both parties have the same key */
    ok_or_goto(outlen1 == outlen2, "Sign/Verify correct keysize", errout);
    ok_or_goto(memcmp(shared_secret1->bytes,shared_secret2->bytes,outlen1)==0, "Shared secrets match", errout);
    
    /* Party 2 tries again without the export/import */
    ok_or_goto(ccecdh_compute_shared_secret(key2,key1,&outlen1,shared_secret1->bytes,global_test_rng)==0, "Compute secret 2", errout);
    
    /* Check both parties have the same key again */
    ok_or_goto(outlen1 == outlen2, "Sign/Verify correct keysize", errout);
    ok_or_goto(memcmp(shared_secret1->bytes,shared_secret2->bytes,outlen1)==0, "Shared secrets match", errout);
    
    status = 1; // Success
    
errout:
    free(shared_secret1);
    free(shared_secret2);
    return status;
}



static int sign_verify(ccec_full_ctx_t sign_key, ccec_full_ctx_t verify_key, struct ccrng_state *rng, const struct ccdigest_info *di)
{
    bool valid = true;
    byteBuffer signature = mallocByteBuffer(MAXKEYSPACE*2);
    byteBuffer hash = hexStringToBytes("000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e0f");
    
    if(hash->len < di->output_size) {
        diag("hash pattern is too small");
        fail("HASH PATTERN SIZE");
        return 0;
    }
    hash->len = di->output_size;
    
    int status = 0;

    // DER APIs
    ok_or_goto(ccec_ctx_bitlen(sign_key) == ccec_ctx_bitlen(sign_key), "Sign/Verify correct keysize", errout);
    ok_or_goto(ccec_sign(sign_key, hash->len, hash->bytes, &signature->len, signature->bytes, rng) == 0, "Signed Hash", errout);
    ok_or_goto(ccec_verify(verify_key, hash->len, hash->bytes, signature->len, signature->bytes, &valid) == 0, "Verified Signed Hash", errout);
    ok_or_goto(valid == true, "Signature verifies", errout);

    // Composite APIs
    ok_or_goto(ccec_sign_composite(sign_key, hash->len, hash->bytes, &signature->bytes[0], &signature->bytes[ccec_signature_r_s_size(sign_key)], rng) == 0, "Signed Hash", errout);
    ok_or_goto(ccec_verify_composite(verify_key, hash->len, hash->bytes, &signature->bytes[0], &signature->bytes[ccec_signature_r_s_size(sign_key)], &valid) == 0, "Verified Signed Hash", errout);
    ok_or_goto(valid == true, "Composite signature verifies", errout);

    status = 1;
    
errout:
    free(signature);
    free(hash);
    return status;
}

static int export_import(ccec_full_ctx_t fk, struct ccrng_state *rng)
{
    int status = 0;
    
    size_t keysize = ccec_ctx_bitlen(fk);
    
    size_t export_pubsize = ccec_x963_export_size(0, fk);
    size_t export_privsize = ccec_x963_export_size(1, fk);
    uint8_t exported_pubkey[export_pubsize];
    uint8_t exported_privkey[export_privsize];
    
    ccec_x963_export(0, exported_pubkey, fk);
    ccec_x963_export(1, exported_privkey, fk);
    
    size_t pub_keysize = ccec_x963_import_pub_size(export_pubsize);
    size_t priv_keysize = ccec_x963_import_priv_size(export_privsize);
    
    ok_or_goto(pub_keysize == keysize, "Package Keysize is the same as we started with", errout);
    ok_or_goto(priv_keysize == keysize, "Package Keysize is the same as we started with", errout);
    ok_or_goto(priv_keysize == pub_keysize, "Package Keysizes agree", errout);
    
    const ccec_const_cp_t cp = ccec_curve_for_length_lookup(keysize,
                ccec_cp_192(), ccec_cp_224(), ccec_cp_256(), ccec_cp_384(), ccec_cp_521());
    {
        ccec_full_ctx_decl_cp(cp, reconstituted_pub);
        ccec_full_ctx_decl_cp(cp, reconstituted_priv);
    
        ok_or_goto(ccec_x963_import_pub(cp, export_pubsize, exported_pubkey, reconstituted_pub) == 0,
               "Imported Public Key Successfully", errout);
        ok_or_goto(saneKeySize(reconstituted_pub), "Keysize is realistic", errout);

        ok_or_goto(ccec_x963_import_priv(cp, export_privsize, exported_privkey, reconstituted_priv) == 0,
               "Imported Private Key Successfully", errout);
        ok_or_goto(saneKeySize(reconstituted_priv), "Keysize is realistic", errout);

        ok_or_goto(sign_verify(reconstituted_priv, reconstituted_pub, rng, ccsha1_di()), "Can perform round-trip sign/verify", errout);    
    }
    status = 1;
errout:
    return status;
    
}

static int export_import_compact(ccec_full_ctx_t fk, struct ccrng_state *rng)
{
    int status = 0;
    
    size_t keysize = ccec_ctx_bitlen(fk);
    
    size_t export_pubsize = ccec_compact_export_size(0, fk);
    size_t export_privsize = ccec_compact_export_size(1, fk);
    uint8_t exported_pubkey[export_pubsize];
    uint8_t exported_privkey[export_privsize];
    cc_zero(sizeof(exported_pubkey),exported_pubkey);
    cc_zero(sizeof(exported_privkey),exported_privkey);
    
    ccec_compact_export(0, exported_pubkey, fk);
    ccec_compact_export(1, exported_privkey, fk);
    
    size_t pub_keysize = ccec_compact_import_pub_size(export_pubsize);
    size_t priv_keysize = ccec_compact_import_priv_size(export_privsize);
    
    ok_or_goto(pub_keysize == keysize, "Compact package Keysize is the same as we started with", errout);
    ok_or_goto(priv_keysize == keysize, "Compact peysize is the same as we started with", errout);
    ok_or_goto(priv_keysize == pub_keysize, "Compact peysizes agree", errout);
    
    const ccec_const_cp_t cp = ccec_curve_for_length_lookup(keysize,
                ccec_cp_192(), ccec_cp_224(), ccec_cp_256(), ccec_cp_384(), ccec_cp_521());
    {
        ccec_full_ctx_decl_cp(cp, reconstituted_pub);
        ccec_full_ctx_decl_cp(cp, reconstituted_priv);
        
        ok_or_goto(ccec_compact_import_pub(cp, export_pubsize, exported_pubkey, reconstituted_pub) == 0,
                   "Imported Compact Public Key Successfully", errout);
        ok_or_goto(saneKeySize(reconstituted_pub), "Compact Keysize is realistic", errout);
        
        ok_or_goto(ccec_compact_import_priv(cp, export_privsize, exported_privkey, reconstituted_priv) == 0,
                   "Imported Compact Private Key Successfully", errout);
        ok_or_goto(saneKeySize(reconstituted_priv), "Compact Keysize is realistic", errout);
        
        ok_or_goto(sign_verify(reconstituted_priv, reconstituted_pub, rng, ccsha1_di()), "Can perform round-trip sign/verify", errout);
    }
    status = 1;
errout:
    return status;
}



static int
round_trip_tests(ccec_full_ctx_t fk)
{
    struct ccrng_state *rng = global_test_rng;
    
    int status = 0;
    ok_or_goto(saneKeySize(fk), "Keysize is realistic", errout);
    // wrap-unwrap isn't working yet.
#ifdef PR_10568130
    ok_or_goto(wrapUnwrap(fk, ccsha1_di(), rng), "Can perform round-trip wrap/unwrap", errout);
#endif
    ok_or_goto(sign_verify(fk, fk, rng, ccsha1_di()), "Can perform round-trip SHA1 sign/verify", errout);
    ok_or_goto(sign_verify(fk, fk, rng, ccsha512_di()), "Can perform round-trip SHA512 sign/verify", errout);
    ok_or_goto(export_import(fk, rng), "Can perform round-trip import/export", errout);
    ok_or_goto(key_exchange(fk,fk), "Can perform key exchange", errout);
    status = 1;
errout:
    return status;    
}

static int construction(ccec_full_ctx_t fk)
{
    cc_size n = ccec_ctx_n(fk);
    size_t bufsiz = ccn_sizeof_n(n);
    uint8_t x[bufsiz], y[bufsiz], d[bufsiz];
    size_t nbits, xsize, ysize, dsize;
    xsize = ysize = dsize = bufsiz;
    ccec_const_cp_t cp = ccec_ctx_cp(fk);
    ccec_full_ctx_decl_cp(cp, newkey);
    
    ok_or_goto(ccec_get_fullkey_components(fk, &nbits, x, &xsize, y, &ysize, d, &dsize) == 0, "Get Key Components", errout);
    ok_or_goto(ccec_make_priv(nbits, xsize, x, ysize, y, dsize, d, newkey) == 0, "Reconstruct Key", errout);
    ok_or_goto(round_trip_tests(newkey), "EC Round-Trip Key Tests", errout);
    
    return 1;
errout:
    return 0;
}


static int ccec_keys_are_equal(ccec_full_ctx_t fk1, ccec_full_ctx_t fk2)
{
    size_t bufsiz1 = ccec_x963_export_size(1, fk1);
    size_t bufsiz2 = ccec_x963_export_size(1, fk2);
    
    ok_or_fail(bufsiz1 == bufsiz2, "EC Key Sizes match");
    uint8_t buf1[bufsiz1], buf2[bufsiz1];
    cc_zero(bufsiz1,buf1);
    cc_zero(bufsiz1,buf2);
    
    ccec_x963_export(1, buf1, fk1);
    ccec_x963_export(1, buf2, fk2);
    
    ok_or_fail(memcmp(buf1, buf2, bufsiz1) == 0, "exported keys match");
    return 1;
}

static int
ECZeroGenTest(size_t expected_keysize, ccec_const_cp_t cp)
{
    if(verbose) diag("Test with keysize %u", expected_keysize);
    ccec_full_ctx_decl_cp(cp, full_key1);
    struct ccrng_sequence_state sequence_prng;
    static uint8_t zerobuf[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
    ccrng_sequence_init(&sequence_prng, sizeof(zerobuf), zerobuf);
    struct ccrng_state *rng2 = (struct ccrng_state *)&sequence_prng;
    
    ok_or_fail(ccec_generate_key_legacy(cp, rng2, full_key1) != 0, "Don't create EC key with 0 for K");
    return 1;
}

static int
ECZeroGenFIPSTest(size_t expected_keysize, ccec_const_cp_t cp)
{
    if(verbose) diag("Test with keysize %u", expected_keysize);
    ccec_full_ctx_decl_cp(cp, full_key1);
    struct ccrng_sequence_state sequence_prng;
    struct ccrng_state *fake_rng = (struct ccrng_state *)&sequence_prng;
    uint8_t fake_rng_buf[MAXKEYSPACE];
    cc_unit order_minus_x[ccec_cp_n(cp)];

    // Rng always return 0
    memset(fake_rng_buf,0,sizeof(fake_rng_buf));
    ccrng_sequence_init(&sequence_prng, sizeof(fake_rng_buf), fake_rng_buf);
    ok_or_fail(ccec_generate_key_fips(cp, fake_rng, full_key1) == 0, "Ok to create key with 0");

    // Rng always returns ff: we will never get a scalar is in the appropriate range
    memset(fake_rng_buf,0xff,sizeof(fake_rng_buf));
    ccrng_sequence_init(&sequence_prng, sizeof(fake_rng_buf), fake_rng_buf);
    ok_or_fail(ccec_generate_key_fips(cp, fake_rng, full_key1) != 0, "Can't pickup scalar in range");

    // Rng always returns order, needs to fail.
    memcpy(fake_rng_buf,(const uint8_t *)cczp_prime(ccec_cp_zq(cp)),CC_BITLEN_TO_BYTELEN(ccec_cp_order_bitlen(cp)));
    memset(fake_rng_buf+CC_BITLEN_TO_BYTELEN(ccec_cp_order_bitlen(cp)),
           0,
           sizeof(fake_rng_buf)-CC_BITLEN_TO_BYTELEN(ccec_cp_order_bitlen(cp)));
    ccrng_sequence_init(&sequence_prng,sizeof(fake_rng_buf),fake_rng_buf);
    ok_or_fail(ccec_generate_key_fips(cp, fake_rng, full_key1) != 0, "Can't pickup scalar in range");

    // Rng always returns order-1, needs to fail.
    memset(fake_rng_buf,0xff,sizeof(fake_rng_buf));
    ccn_sub1(ccec_cp_n(cp),order_minus_x,cczp_prime(ccec_cp_zq(cp)),1);
    memcpy(fake_rng_buf,order_minus_x,ccn_sizeof_n(ccec_cp_n(cp)));
    ccrng_sequence_init(&sequence_prng,sizeof(fake_rng_buf),fake_rng_buf);
    ok_or_fail(ccec_generate_key_fips(cp, fake_rng, full_key1) != 0, "EC key gen fips with q-1");

    // Rng always returns order-2, needs to work.
    memset(fake_rng_buf,0xff,sizeof(fake_rng_buf));
    ccn_sub1(ccec_cp_n(cp),order_minus_x,cczp_prime(ccec_cp_zq(cp)),2);
    memcpy(fake_rng_buf,order_minus_x,ccn_sizeof_n(ccec_cp_n(cp)));
    ccrng_sequence_init(&sequence_prng,sizeof(fake_rng_buf),fake_rng_buf);
    ok_or_fail(ccec_generate_key_fips(cp, fake_rng, full_key1) == 0, "EC key gen fips with q-2");

    return 1;
}


static int
ECStaticGenTest(size_t expected_keysize, ccec_const_cp_t cp)
{
    ccec_full_ctx_decl_cp(cp, full_key1);
    ccec_full_ctx_decl_cp(cp, full_key2);
    struct ccrng_pbkdf2_prng_state pbkdf2_prng;
    const char *password = "foofoofoo";
    const char *salt = "AAAAAAAA";
    unsigned long saltlen = 8;
    unsigned long iterations = 1024;
    size_t random_length=2*ccn_sizeof(ccec_cp_order_bitlen(cp));
    struct ccrng_state *rng2 = (struct ccrng_state *)&pbkdf2_prng;

    ok_or_fail(ccrng_pbkdf2_prng_init(&pbkdf2_prng, random_length, strlen(password), password, saltlen, salt, iterations)==0,"pbkdf2 init");
    ok_or_fail(ccec_generate_key_legacy(cp, rng2, full_key1) == 0, "Generated Key");

    ok_or_fail(ccrng_pbkdf2_prng_init(&pbkdf2_prng, random_length, strlen(password), password, saltlen, salt, iterations)==0,"pbkdf2 init");
    ok_or_fail(ccec_generate_key_legacy(cp, rng2, full_key2) == 0, "Generated Key");
    ok(ccec_keys_are_equal(full_key1, full_key2), "Two Password Derived Keys are equal from the same password");
    
    int status = 0;
    if(verbose) diag("Test with keysize %u", expected_keysize);
    ok_or_goto(ccec_ctx_bitlen(full_key1) == expected_keysize, "Generated correct keysize", errout);
    ok_or_goto(round_trip_tests(full_key1), "EC Round-Trip Key Tests", errout);
    ok_or_goto(construction(full_key1), "EC Construction Tests", errout);
    status = 1;
errout:
    return status;
}

static void fill(int *guard) {
    guard[0] = -1;
    guard[1] = -1;
    guard[2] = -1;
    guard[3] = -1;
}

static int chkit(int *guard) {
    return guard[0] == -1 &&
    guard[1] == -1 &&
    guard[2] == -1 &&
    guard[3] == -1;
}

static int
ECDH_negativeTesting(ccec_const_cp_t cp)
{
    size_t n=ccec_cp_n(cp);
    ccec_full_ctx_decl_cp(cp, full_key);
    uint8_t out[ccec_ccn_size(cp)];
    size_t  out_len=sizeof(out);
    uint32_t status=0;
    uint32_t nb_test=0;
    int result=0;
    
    // Curve param
    ccec_ctx_init(cp, full_key);
    
    // Set a dummy private key
    ccn_seti(n, ccec_ctx_k(full_key), 2);

    /* 0) Sanity: valid arguments */
    ccn_set(n,ccec_ctx_x(full_key),ccec_const_point_x(ccec_cp_g(cp),cp));
    ccn_set(n,ccec_ctx_y(full_key),ccec_const_point_y(ccec_cp_g(cp),cp));
    ccn_seti(n, ccec_ctx_z(full_key), 1);
    if (ccecdh_compute_shared_secret(full_key, full_key,&out_len, out,global_test_rng)==0)
    {
        status|=1<<nb_test;
    }
    nb_test++;
    
    /* 1) Set x to p */
    ccn_set(n,ccec_ctx_x(full_key),ccec_ctx_prime(full_key));
    ccn_set(n,ccec_ctx_y(full_key),ccec_const_point_y(ccec_cp_g(cp),cp));
    ccn_seti(n, ccec_ctx_z(full_key), 1);
    if (ccecdh_compute_shared_secret(full_key, full_key,&out_len, out,global_test_rng)!=0)
    {
        status|=1<<nb_test;
    }
    nb_test++;
    
    /* 2) Set y to p */
    ccn_set(n,ccec_ctx_x(full_key),ccec_const_point_x(ccec_cp_g(cp),cp));
    ccn_set(n,ccec_ctx_y(full_key),ccec_ctx_prime(full_key));
    ccn_seti(n, ccec_ctx_z(full_key), 1);
    if (ccecdh_compute_shared_secret(full_key, full_key,&out_len, out,global_test_rng)!=0)
    {
        status|=1<<nb_test;
    }
    nb_test++;

    /* 3) Use a point which is not on the curve. */
    // The point (0,0) can't be on the curve with equation y^2=x^3-3x+b.
    if (ccn_is_zero(n,ccec_cp_b(cp)))
    {   // The point (1,1) can't be on the curve with equation y^2=x^3-3x+0.
        ccn_seti(n,ccec_ctx_x(full_key),1);
        ccn_seti(n,ccec_ctx_y(full_key),1);
    }
    else
    {   // The point (0,0) can't be on the curve with equation y^2=x^3-3x+b with b!=0
        ccn_zero(n,ccec_ctx_x(full_key));
        ccn_zero(n,ccec_ctx_y(full_key));
    }
    if (ccecdh_compute_shared_secret(full_key, full_key,&out_len, out,global_test_rng)!=0)
    {
        status|=1<<nb_test;
    }
    nb_test++;

    /* 4) Output is infinite point  */
    ccn_set(n,ccec_ctx_x(full_key),ccec_const_point_x(ccec_cp_g(cp),cp));
    ccn_set(n,ccec_ctx_y(full_key),ccec_const_point_y(ccec_cp_g(cp),cp));
    ccn_set(n, ccec_ctx_k(full_key), cczp_prime(ccec_cp_zq(cp)));
    if (ccecdh_compute_shared_secret(full_key, full_key,&out_len, out,global_test_rng)!=0)
    {
        status|=1<<nb_test;
    }
    nb_test++;
    
    /* Test aftermath */
    if ((nb_test==0) || (status!=((1<<nb_test)-1)))
    {
        result=0;
    }
    else
    {
        result=1; // Test is successful, Yeah!
    }

    return result;
}

static int
ECCompactGenTest(struct ccrng_state *rng, size_t expected_keysize, ccec_const_cp_t cp)
{
    int top[4];
    ccec_full_ctx_decl_cp(cp, full_key1);
    ccec_full_ctx_decl_cp(cp, full_key2);
    int bottom[4];
    
    fill(top); fill(bottom);
    ok_or_fail(ccec_compact_generate_key(cp, rng, full_key1) == 0, "Generated Key 1");
    ok_or_fail(ccec_compact_generate_key(cp, rng, full_key2) == 0, "Generated Key 2");
    
    if(!chkit(top) || !chkit(bottom)) diag("ALARM");
    
    int status = 0;
    if(verbose) diag("Compact Test with keysize %u", expected_keysize);
    ok_or_goto(ccec_ctx_bitlen(full_key1) == expected_keysize, "Generated correct keysize 1", errout);
    ok_or_goto(ccec_ctx_bitlen(full_key2) == expected_keysize, "Generated correct keysize 2", errout);
    ok_or_goto(export_import_compact(full_key1,rng), "Import Export compact format", errout);
    ok_or_goto(key_exchange_compact(full_key1,full_key2), "EC Construction Tests", errout);
    status = 1;
errout:
    return status;
}


static int
ECStdGenTest(struct ccrng_state *rng, size_t expected_keysize, ccec_const_cp_t cp, int fips)
{
    int top[4];
    ccec_full_ctx_decl_cp(cp, full_key);
    int bottom[4];
    
    fill(top); fill(bottom);
    if(fips) ok_or_fail(ccec_generate_key_legacy(cp, rng, full_key) == 0, "Generated Key");
    else ok_or_fail(ccecdh_generate_key(cp, rng, full_key) == 0, "Generated Key");

    if(!chkit(top) || !chkit(bottom)) diag("ALARM");
    
    int status = 0;
    if(verbose) diag("Test with keysize %u", expected_keysize);
    ok_or_goto(ccec_ctx_bitlen(full_key) == expected_keysize, "Generated correct keysize", errout);
    ok_or_goto(round_trip_tests(full_key), "EC Round-Trip Key Tests", errout);
    ok_or_goto(construction(full_key), "EC Construction Tests", errout);
    status = 1;
errout:
    return status;
}

static bool
rfc6637KAT_P256(void)
{
    ccec_const_cp_t cp = ccec_cp_256();
    ccec_full_ctx_decl_cp(cp, private_key);

    uint8_t private_key_bin[] = {
        0x04,
        0x7f, 0x70, 0xc0, 0xa8, 0x18, 0x4c, 0xdc, 0xae,
        0xa5, 0xdb, 0x20, 0xba, 0x8f, 0xed, 0x17, 0xe4,
        0x7b, 0xde, 0xfb, 0x74, 0x4d, 0x57, 0x5e, 0xc4,
        0x49, 0x13, 0x0a, 0xf3, 0x7e, 0xda, 0xde, 0x65,
        0x8a, 0xe7, 0xee, 0x35, 0xd2, 0x0e, 0x88, 0x97,
        0x91, 0x1c, 0x9f, 0x56, 0x4b, 0xe3, 0x3d, 0x9a,
        0x94, 0xbc, 0x1e, 0x5c, 0x92, 0x7b, 0x1a, 0xa0,
        0x7f, 0xf7, 0x50, 0xd2, 0xd1, 0x1c, 0x29, 0x71,
        0xa0, 0x5c, 0xd1, 0x47, 0x49, 0xbe, 0xa3, 0xf3,
        0xd1, 0x4c, 0x92, 0xdc, 0x43, 0x8e, 0x45, 0xe3,
        0x51, 0xef, 0xe8, 0x60, 0x36, 0x0c, 0x43, 0x17,
        0x05, 0xb7, 0xd4, 0x24, 0x10, 0x58, 0x18, 0x43
    };

    uint8_t wrapped_key[] = {
        /* length i bits MSB */
        0x02, 0x08,

        /* empherial dh key */
        0x04, 0x1d, 0xd2, 0x18, 0x65, 0x0c, 0x7e, 0xad,
        0x5d, 0x5c, 0x8f, 0x2e, 0xea, 0x8a, 0xd2, 0x0e,
        0x10, 0x80, 0x85, 0x17, 0x3a, 0x78, 0x72, 0x3b,
        0x11, 0x5f, 0x8a, 0xfe, 0xac, 0x37, 0x4c, 0x71,
        0x05, 0x2c, 0x87, 0xb5, 0x59, 0x99, 0xb3, 0xf5,
        0x73, 0xd0, 0xb4, 0xb3, 0xdb, 0x20, 0x08, 0xac,
        0x2e, 0x6d, 0x67, 0xe4, 0x71, 0x85, 0x25, 0xd9,
        0x93, 0xa3, 0xbb, 0x88, 0x32, 0xfa, 0xce, 0x0c,
        0x61,

        /* aes-wrap key size */
        0x30,

        /* aes warpped data */
        0x5b, 0xe3, 0xf8, 0xfc, 0x26, 0x39, 0xc9, 0xf4,
        0x1a, 0xa7, 0xe8, 0xb5, 0xae, 0xb5, 0x35, 0xc9,
        0xcf, 0x0b, 0x1d, 0x3f, 0x5d, 0xd5, 0x20, 0x92,
        0xe7, 0x86, 0x23, 0x6a, 0x21, 0x27, 0x51, 0xdf,
        0xfe, 0xb1, 0x59, 0x0d, 0x08, 0x09, 0x16, 0x33,
        0xc1, 0xbf, 0x79, 0x9d, 0x46, 0x75, 0x73, 0x50
    };
    uint8_t key[sizeof(wrapped_key)];
    size_t key_len = sizeof(key);
    uint8_t symalg;
    uint8_t fingerprint[20] = {
        0xC3, 0xDC, 0x05, 0x54, 0xFF, 0x5E, 0xEF, 0x32, 0x50, 0x81,
        0xD6, 0xE3, 0x77, 0xCE, 0xB7, 0xA3, 0x40, 0x89, 0xAB, 0x73
    };
    int res;

    uint8_t rkey[32] = {
        0x17, 0x5D, 0x17, 0x98, 0x75, 0x78, 0x0E, 0xE3,
        0x27, 0x0F, 0x6C, 0x0D, 0xCE, 0x3E, 0x11, 0x51,
        0xD4, 0xE2, 0x5C, 0xE5, 0x67, 0xFD, 0xC8, 0xAB,
        0x2C, 0x2D, 0xB2, 0x64, 0xAF, 0x8F, 0x91, 0xBC
    };

    ok_status(ccec_x963_import_priv(cp, sizeof(private_key_bin),  private_key_bin, private_key),
              "ccec_x963_import_priv");

    res = ccec_rfc6637_unwrap_key(private_key, &key_len, key, 0, &symalg,
                                  &ccec_rfc6637_dh_curve_p256,
                                  &ccec_rfc6637_unwrap_sha256_kek_aes128,
                                  fingerprint, sizeof(wrapped_key), wrapped_key);

    ok_or_goto(res == 0, "ccec_rfc6637_unwrap_key", fail);

    ok_or_goto(symalg == 9, "symalg", fail);
    ok_or_goto(key_len == 32, "keylen", fail);
    ok_memcmp_or_fail(key, rkey, 32, "key");

    return 1;
fail:
    return 0;
}

static bool
ECPGPWrapUnwrapTest(struct ccrng_state * rng, unsigned long flags, size_t keysize, ccec_const_cp_t cp, struct ccec_rfc6637_curve *curve, struct ccec_rfc6637_wrap *wrap, struct ccec_rfc6637_unwrap *unwrap)
{
    ccec_full_ctx_decl_cp(cp, peer);
    unsigned long i;

    ok(ccec_generate_key_legacy(cp, rng, peer) == 0, "Generated Key");
    
    uint8_t key[keysize];
    
    memset(key, 0x23, sizeof(key));
    
    size_t wrapped_size = ccec_rfc6637_wrap_key_size(peer, flags, keysize);
    
    uint8_t wrapped_key[wrapped_size];
    uint8_t fingerprint[20];
    
    ok(ccec_rfc6637_wrap_key(peer, wrapped_key, flags, 0, keysize, key, curve, wrap, fingerprint, rng) == 0, "pgp wrap");
    
    uint8_t alg;
    uint8_t unwrapped_key[100];
    size_t unwrapped_key_size = sizeof(unwrapped_key);
    
    ok(ccec_rfc6637_unwrap_key(peer, &unwrapped_key_size, unwrapped_key, flags, &alg, curve, unwrap, fingerprint, wrapped_size, wrapped_key) == 0, "pgp unwrap");
    
    ok(unwrapped_key_size == keysize, "check keysize");
    ok(memcmp(key, unwrapped_key, keysize) == 0, "check key");

    /* check that unwrap refused to support compact keys if flag isn't passed in */
    if (flags & CCEC_RFC6637_COMPACT_KEYS) {
        unsigned long nflags = flags & ~CCEC_RFC6637_COMPACT_KEYS;
        ok(ccec_rfc6637_unwrap_key(peer, &unwrapped_key_size, unwrapped_key, nflags, &alg, curve, unwrap, fingerprint, wrapped_size, wrapped_key) != 0, "pgp unwrap compact keys");
    }

    /* premutate each byte */
    for (i = 0; i < wrapped_size; i++) {
        wrapped_key[i] ^= arc4random_uniform(255) + 1;
        ok(ccec_rfc6637_unwrap_key(peer, &unwrapped_key_size, unwrapped_key, flags, &alg, curve, unwrap, fingerprint, wrapped_size, wrapped_key) != 0, "pgp unwrap");
    }
    
    return true;
}



#define FIPS 1
#define ECDH 0

int ccec(TM_UNUSED int argc, TM_UNUSED char *const *argv)
{
    struct ccrng_state *rng = global_test_rng;
    int stdgen = 1;
    int stddhgen = 1;
    int staticgen = 1;
    int pgpwrap = 1;
    int compact = 1;
    
	plan_tests(kTestTestCount);

    if(verbose) diag("ECDSA KATs");
    ok(ecdsa_known_answer_tests(), "ECDSA KATs");

    if(verbose) diag("ECDSA Negative tests");
    ok(ecdsa_negative_tests(), "ECDSA Negative tests");
    fputs("\n", stdout);

    if(verbose) diag("Zero Gen FIPS Tests");
        ok(ECZeroGenFIPSTest(224, ccec_cp_224()), "Generate 224 bit EC Key Pair");
        ok(ECZeroGenFIPSTest(256, ccec_cp_256()), "Generate 256 bit EC Key Pair");
        ok(ECZeroGenFIPSTest(521, ccec_cp_521()), "Generate 521 bit EC Key Pair");
        if(verbose) diag_linereturn();

    if(stdgen) {
        if(verbose) diag("Standard Gen Tests");
        ok(ECStdGenTest(rng, 192, ccec_cp_192(), FIPS), "Generate 192 bit EC(FIPS) Key Pair");
        ok(ECStdGenTest(rng, 224, ccec_cp_224(), FIPS), "Generate 224 bit EC(FIPS) Key Pair");
        ok(ECStdGenTest(rng, 256, ccec_cp_256(), FIPS), "Generate 256 bit EC(FIPS) Key Pair");
        ok(ECStdGenTest(rng, 384, ccec_cp_384(), FIPS), "Generate 384 bit EC(FIPS) Key Pair");
        ok(ECStdGenTest(rng, 521, ccec_cp_521(), FIPS), "Generate 521 bit EC(FIPS) Key Pair");
        if(verbose) diag_linereturn();
    } /* stdgen */
    
    if(stddhgen) {

        if(verbose) diag("Standard ECDH Tests");
        ok(ECStdGenTest(rng, 192, ccec_cp_192(), ECDH), "Generate 192 bit ECDH Key Pair");
        ok(ECStdGenTest(rng, 224, ccec_cp_224(), ECDH), "Generate 224 bit ECDH Key Pair");
        ok(ECStdGenTest(rng, 256, ccec_cp_256(), ECDH), "Generate 256 bit ECDH Key Pair");
        ok(ECStdGenTest(rng, 384, ccec_cp_384(), ECDH), "Generate 384 bit ECDH Key Pair");
        ok(ECStdGenTest(rng, 521, ccec_cp_521(), ECDH), "Generate 521 bit ECDH Key Pair");
        if(verbose) diag_linereturn();

        if(verbose) diag("ECDH Negative Tests");
        ok(ECDH_negativeTesting(ccec_cp_192()), "ECDH Negative testing on 192 bit curve");
        ok(ECDH_negativeTesting(ccec_cp_224()), "ECDH Negative testing on 224 bit curve");
        ok(ECDH_negativeTesting(ccec_cp_256()), "ECDH Negative testing on 256 bit curve");
        ok(ECDH_negativeTesting(ccec_cp_384()), "ECDH Negative testing on 384 bit curve");
        ok(ECDH_negativeTesting(ccec_cp_521()), "ECDH Negative testing on 521 bit curve");
        fputs("\n", stdout);
    } /* stddhgen */
    
    if(staticgen) {
        if(verbose) diag("Static Gen Tests");
        ok(ECStaticGenTest(192, ccec_cp_192()), "Generate 192 bit EC Key Pair");
        ok(ECStaticGenTest(224, ccec_cp_224()), "Generate 224 bit EC Key Pair");
        ok(ECStaticGenTest(256, ccec_cp_256()), "Generate 256 bit EC Key Pair");
        ok(ECStaticGenTest(384, ccec_cp_384()), "Generate 384 bit EC Key Pair");
        ok(ECStaticGenTest(521, ccec_cp_521()), "Generate 521 bit EC Key Pair");
        if(verbose) diag_linereturn();
    } /* stdgen */
    
    if(compact) {
        if(verbose) diag("Compact representation");
        ok(ECCompactGenTest(rng, 192, ccec_cp_192()), "Generate 192 bit EC Key Pair");
        //ok(ECCompactGenTest(rng, 224, ccec_cp_224()), "Generate 224 bit EC Key Pair"); Not supported yet because of sqrt
        ok(ECCompactGenTest(rng, 256, ccec_cp_256()), "Generate 256 bit EC Key Pair");
        ok(ECCompactGenTest(rng, 384, ccec_cp_384()), "Generate 384 bit EC Key Pair");
        ok(ECCompactGenTest(rng, 521, ccec_cp_521()), "Generate 521 bit EC Key Pair");
        if(verbose) diag_linereturn();
    } /* stdgen */
    
    if(verbose) diag("Zero Gen Tests");
    ok(ECZeroGenTest(256, ccec_cp_256()), "Generate 256 bit EC Key Pair");
    if(verbose) diag_linereturn();

    if (pgpwrap) {
        if(verbose) diag("Standard rfc6637 wrap");
        ok(ECPGPWrapUnwrapTest(rng, 0, 16, ccec_cp_256(), &ccec_rfc6637_dh_curve_p256, &ccec_rfc6637_wrap_sha256_kek_aes128, &ccec_rfc6637_unwrap_sha256_kek_aes128),  "EC PGP wrap 256 bit key 16");
        ok(ECPGPWrapUnwrapTest(rng, 0, 16, ccec_cp_521(), &ccec_rfc6637_dh_curve_p521, &ccec_rfc6637_wrap_sha256_kek_aes128, &ccec_rfc6637_unwrap_sha256_kek_aes128), "EC PGP wrap 521 bit key 16");
        ok(ECPGPWrapUnwrapTest(rng, 0, 32, ccec_cp_256(), &ccec_rfc6637_dh_curve_p256, &ccec_rfc6637_wrap_sha256_kek_aes128, &ccec_rfc6637_unwrap_sha256_kek_aes128),  "EC PGP wrap 256 bit key 32");
        ok(ECPGPWrapUnwrapTest(rng, 0, 32, ccec_cp_521(), &ccec_rfc6637_dh_curve_p521, &ccec_rfc6637_wrap_sha256_kek_aes128, &ccec_rfc6637_unwrap_sha256_kek_aes128), "EC PGP wrap 521 bit key 32");
        ok(ECPGPWrapUnwrapTest(rng, CCEC_RFC6637_COMPACT_KEYS, 16, ccec_cp_256(), &ccec_rfc6637_dh_curve_p256, &ccec_rfc6637_wrap_sha256_kek_aes128, &ccec_rfc6637_unwrap_sha256_kek_aes128),  "EC PGP wrap 256 bit key 16");
        ok(ECPGPWrapUnwrapTest(rng, CCEC_RFC6637_COMPACT_KEYS, 16, ccec_cp_521(), &ccec_rfc6637_dh_curve_p521, &ccec_rfc6637_wrap_sha256_kek_aes128, &ccec_rfc6637_unwrap_sha256_kek_aes128), "EC PGP wrap 521 bit key 16");
        ok(ECPGPWrapUnwrapTest(rng, CCEC_RFC6637_COMPACT_KEYS, 32, ccec_cp_256(), &ccec_rfc6637_dh_curve_p256, &ccec_rfc6637_wrap_sha256_kek_aes128, &ccec_rfc6637_unwrap_sha256_kek_aes128),  "EC PGP wrap 256 bit key 32");
        ok(ECPGPWrapUnwrapTest(rng, CCEC_RFC6637_COMPACT_KEYS, 32, ccec_cp_521(), &ccec_rfc6637_dh_curve_p521, &ccec_rfc6637_wrap_sha256_kek_aes128, &ccec_rfc6637_unwrap_sha256_kek_aes128), "EC PGP wrap 521 bit key 32");
        if(verbose) diag("rfc6637 KAT\n");
        ok(rfc6637KAT_P256(), "rfc6637 p256");

    }
    return 0;
}

#endif

