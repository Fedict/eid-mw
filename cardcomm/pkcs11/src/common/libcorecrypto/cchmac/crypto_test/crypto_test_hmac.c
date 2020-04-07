/*
 * Copyright (c) 2012,2014,2015 Apple Inc. All rights reserved.
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

#define USE_SUPER_COOL_NEW_CCOID_T
#include "testmore.h"
#include "testbyteBuffer.h"
#include "testccnBuffer.h"

static int verbose = 0;

#if (CCHMACTEST == 0)
entryPoint(cchmactest,"cchmac test")
#else
#include <corecrypto/ccasn1.h>
#include <corecrypto/ccdigest.h>
#include <corecrypto/cchmac.h>
#import <corecrypto/ccmd2.h>
#import <corecrypto/ccmd4.h>
#import <corecrypto/ccmd5.h>
#import <corecrypto/ccsha1.h>
#import <corecrypto/ccsha2.h>
#import <corecrypto/ccripemd.h>

/* Currently, cchmac and friends won't work when length == 0 and the
 * data pointer is NULL.
 */

#define keystr128    "000102030405060708090a0b0c0d0e0f"


#define HMAC_DATA_POINTER_NULL_TOLERANT 0

typedef struct test_vector_t {
    char *keyStr;
    uint8_t *data;
    size_t data_len;
    char *md2_answer;
    char *md4_answer;
    char *md5_answer;
    char *sha1_answer;
    char *sha224_answer;
    char *sha256_answer;
    char *sha384_answer;
    char *sha512_answer;
    char *rmd128_answer;
    char *rmd160_answer;
    char *rmd256_answer;
    char *rmd320_answer;
} test_vector;

static char *
digest_name(const struct ccdigest_info *di) {
    if(ccdigest_oid_equal(di, (ccoid_t) CC_DIGEST_OID_MD2)) return "MD2";
    if(ccdigest_oid_equal(di, (ccoid_t) CC_DIGEST_OID_MD4)) return "MD4";
    if(ccdigest_oid_equal(di, (ccoid_t) CC_DIGEST_OID_MD5)) return "MD5";
    if(ccdigest_oid_equal(di, (ccoid_t) CC_DIGEST_OID_SHA1)) return "SHA1";
    if(ccdigest_oid_equal(di, (ccoid_t) CC_DIGEST_OID_SHA224)) return "SHA224";
    if(ccdigest_oid_equal(di, (ccoid_t) CC_DIGEST_OID_SHA256)) return "SHA256";
    if(ccdigest_oid_equal(di, (ccoid_t) CC_DIGEST_OID_SHA384)) return "SHA384";
    if(ccdigest_oid_equal(di, (ccoid_t) CC_DIGEST_OID_SHA512)) return "SHA512";
    if(ccdigest_oid_equal(di, (ccoid_t) CC_DIGEST_OID_RMD128)) return "RMD128";
    if(ccdigest_oid_equal(di, (ccoid_t) CC_DIGEST_OID_RMD160)) return "RMD160";
    if(ccdigest_oid_equal(di, (ccoid_t) CC_DIGEST_OID_RMD256)) return "RMD256";
    return "rmd320"; // hack
}

static int test_answer(const struct ccdigest_info *di, test_vector *vector, void*answer) {
    char *correct_answer = NULL;
    if(ccdigest_oid_equal(di, (ccoid_t) CC_DIGEST_OID_MD2)) correct_answer = vector->md2_answer;
    else if(ccdigest_oid_equal(di, (ccoid_t) CC_DIGEST_OID_MD4)) correct_answer = vector->md4_answer;
    else if(ccdigest_oid_equal(di, (ccoid_t) CC_DIGEST_OID_MD5)) correct_answer = vector->md5_answer;
    else if(ccdigest_oid_equal(di, (ccoid_t) CC_DIGEST_OID_SHA1)) correct_answer = vector->sha1_answer;
    else if(ccdigest_oid_equal(di, (ccoid_t) CC_DIGEST_OID_SHA224)) correct_answer = vector->sha224_answer;
    else if(ccdigest_oid_equal(di, (ccoid_t) CC_DIGEST_OID_SHA256)) correct_answer = vector->sha256_answer;
    else if(ccdigest_oid_equal(di, (ccoid_t) CC_DIGEST_OID_SHA384)) correct_answer = vector->sha384_answer;
    else if(ccdigest_oid_equal(di, (ccoid_t) CC_DIGEST_OID_SHA512)) correct_answer = vector->sha512_answer;
    else if(ccdigest_oid_equal(di, (ccoid_t) CC_DIGEST_OID_RMD128)) correct_answer = vector->rmd128_answer;
    else if(ccdigest_oid_equal(di, (ccoid_t) CC_DIGEST_OID_RMD160)) correct_answer = vector->rmd160_answer;
    else if(ccdigest_oid_equal(di, (ccoid_t) CC_DIGEST_OID_RMD256)) correct_answer = vector->rmd256_answer;
    else correct_answer = vector->rmd320_answer; // hack
    byteBuffer answer_bb = bytesToBytes(answer, di->output_size);
    if(correct_answer == NULL) {
        diag("\t\t\"%s\", // %s\n", bytesToHexString(answer_bb), digest_name(di));
        return 1;
    }
    byteBuffer correct_answer_bb = hexStringToBytes((char *) correct_answer);
    ok(bytesAreEqual(correct_answer_bb, answer_bb), "compare memory of answer");
    if(bytesAreEqual(correct_answer_bb, answer_bb) == 0) {
        printByteBuffer(correct_answer_bb, "Correct Answer");
        printByteBuffer(answer_bb, "Provided Answer");
    }
    free(correct_answer_bb);
    free(answer_bb);
    return 1;
}

static int guard_ok(uint8_t *p, int chr, size_t len) {
    for(size_t i=0; i<len; i++) if(p[i] != chr) return 0;
    return 1;
}
/*
 void cchmac_init(const struct ccdigest_info *di, cchmac_ctx_t ctx, unsigned long key_len, const void *key);
 void cchmac_update(const struct ccdigest_info *di, cchmac_ctx_t ctx, unsigned long data_len, const void *data);
 void cchmac_final(const struct ccdigest_info *di, cchmac_ctx_t ctx, unsigned char *mac);
 
 void cchmac(const struct ccdigest_info *di, unsigned long key_len, const void *key, unsigned long data_len, const void *data, unsigned char *mac);
 
 */

static int test_discreet(const struct ccdigest_info *di, test_vector *vector) {
    uint8_t answer[128];
    size_t total = vector->data_len;
    size_t chunk = vector->data_len/2;
    uint8_t *p = vector->data;
    uint8_t ctxfrontguard[4096];
    cchmac_di_decl(di, ctx);
    uint8_t ctxrearguard[4096];
    memset(ctxfrontguard, 0xee, 4096);
    memset(ctxrearguard, 0xee, 4096);
    // break it up into pieces.
    byteBuffer key = hexStringToBytes(vector->keyStr);
    cchmac_init(di, ctx, key->len, key->bytes);
    ok(guard_ok(ctxfrontguard, 0xee, 4096), "context is safe");
    ok(guard_ok(ctxrearguard, 0xee, 4096), "context is safe");
    do {
        cchmac_update(di, ctx, chunk, p);
        total -= chunk;
        p += chunk;
        chunk /= 2;
        if(chunk == 0) chunk = total;
    } while(total);
    ok(guard_ok(ctxfrontguard, 0xee, 4096), "context is safe");
    ok(guard_ok(ctxrearguard, 0xee, 4096), "context is safe");
    
    cchmac_final(di, ctx, answer);
    
    ok(guard_ok(ctxfrontguard, 0xee, 4096), "context is safe");
    ok(guard_ok(ctxrearguard, 0xee, 4096), "context is safe");
    ok(test_answer(di, vector, answer), "check answer");
    return 1;
}

static int test_oneshot(const struct ccdigest_info *di, test_vector *vector) {
    uint8_t answer[128];
    byteBuffer key = hexStringToBytes(vector->keyStr);
    cchmac(di, key->len, key->bytes, vector->data_len, vector->data, answer);
    ok(test_answer(di, vector, answer), "check answer");
    return 1;
}

static int test_hmac_many_blocks(const struct ccdigest_info *di) {
    static const size_t buffer_size = 16383;
    static uint8_t buffer[buffer_size];
    static test_vector vector[] = {
        { keystr128, buffer, buffer_size,
            "7182daf0dc1007cbac6a0e719098f170", // MD2
            "877a48e58590d460090c7777f4a840c6", // MD4
            "7f883f92f8ef750adfa292125b9fbc55", // MD5
            "4ce3f19af4a6ef30a24a178407e9fdac3b28f99a", // SHA1
            "210db31bed8c58f6f79dcbce97a7cea7e2ae4a1ff5f9488b0bc45c45", // SHA224
            "4185e361836f030b1e72b3f799923405741f4c9676151f496a4a0c55d3682665", // SHA256
            "ae1de46bbe8045f03fa5c54a8033c9fa823bd1f244155fa479eb05f771c1f110a21d31488b40f54f710d30b2eeb23f20", // SHA384
            "58b28d36a039a8267d0ad5bcef1c062caf6682699cd1ec2fa201f6d042b85946a32daee9abbeabb6ec2f29b6cadd29ce333aaa146ca1cba55302f236e03ce97d", // SHA512
            "4b4b23484f0f7cb819661f1fcc7fe7b2", // RMD128
            "8031f9ec1222c149ca224bd90ba9eadecba75f0f", // RMD160
            "8bf971d3821dc96d4764e184f9b7db68cc573b6ec5acc237c92270c5c26040a6", // RMD256
            "8031f9ec1222c149ca224bd90ba9eadecba75f0f", // RMD320
        },
    };
    int vector_size = sizeof (vector) / sizeof (test_vector);
    for(size_t n=0; n<buffer_size; n++) buffer[n] = n&0xff;
    if(verbose) diag("hmac Large Test\n");
    for(int i=0; i<vector_size; i++) {
        ok(test_oneshot(di, &vector[i]), "test one-shot with data less than blocksize");
        ok(test_discreet(di, &vector[i]), "test discreet with data less than blocksize");
    }
    return 1;
}

static int test_hmac_eq_blocksize(const struct ccdigest_info *di) {
    static test_vector vector[] = {
        { keystr128, (uint8_t *) "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF", 128,
            "71aa63826caf519e696c04ce8a7766c7", // MD2
            "ca5312e0065d940d76fb5142c74c8402", // MD4
            "e48be2f550c05454fef7c84dd60d4ff4", // MD5
            "2597cdf90d6807154e157362510ea592f8468f03", // SHA1
            "c6e8ebf4630eaa709c0a2bb4475263491537228b6b7bb891a5bb8103", // SHA224
            "89e5037a3d76ac6f6ca17cee03049c9c96d4f4757405a6b16be29c3e5c118ca1", // SHA256
            "b8b562827c8d48fb408a31b1eb418694c588962a0f247fd44dc71306c6eaa32cf1f4d795546a41494313f78f3e19e8e3", // SHA384
            "520bb5e76f51229c70751a8ed4a8a31c6b5f0d6836ff5a5389da6c1eccc40ff1193541fbb3141b8c2344a1356f492ad0513abf1d7d3ad79908a97c38c65bf95a", // SHA512
            "8cdfa517a4cd32e3b381c42273397fb4", // RMD128
            "bafe182a4fc5545054c99e55ce94b947387e0a8a", // RMD160
            "0476090bc9975a7203161ce97ace33ce653bb8826b2a1316e86a287fdd84b7d3", // RMD256
            "bafe182a4fc5545054c99e55ce94b947387e0a8a", // RMD320
        },
    };
    int vector_size = sizeof (vector) / sizeof (test_vector);
    if(verbose) diag("hmac EQ Test\n");
    for(int i=0; i<vector_size; i++) {
        ok(test_oneshot(di, &vector[i]), "test one-shot with data less than blocksize");
        ok(test_discreet(di, &vector[i]), "test discreet with data less than blocksize");
    }
    return 1;
}

static int test_hmac_lt_blocksize(const struct ccdigest_info *di) {
    static test_vector vector[] = {
        { keystr128, (uint8_t *) "Test vector from febooti.com", 28,
            "2d42a2ed33bd579625cbeffcf734ea65", // MD2
            "b024b400fd0bad206d1102344dc36b24", // MD4
            "33c778da7bbc0011c0933c806f59c85a", // MD5
            "7d5a46909cd7e0c51c2a2a32bf356f32277948e3", // SHA1
            "4d656dcbd37a5911314e6ddccfdd1f7756e9449ed44615bbe006249f", // SHA224
            "d9d9e526ea6cc3a93797e23e34138322721cd7454103124073263834a00393c3", // SHA256
            "cf40060ad6784b2a85ad9a15611d1f408cb91e68591db71953f77681cba706cadab6897c509f1731cd9eaae6e9a97392", // SHA384
            "5c4ea434cc9e3e2e971d1c4f0e202b17354aa810ebb2699ff752f301981950cdc53fe6a1215afbc7e5306e6da21fd7b58f4906727c856931b0798cd977b43614", // SHA512
            "0a465ca8b562e03d093872fd6ca23a6f", // RMD128
            "fde553210d3591ca69404e7ef1c9d401a260688d", // RMD160
            "5dcfbc84f95afba937329b663bbc801e6c065c2122b9fb4a7bebc30db753757f", // RMD256
            "fde553210d3591ca69404e7ef1c9d401a260688d", // RMD320
        },
    };
    int vector_size = sizeof (vector) / sizeof (test_vector);
    if(verbose) diag("hmac LT Test\n");

    for(int i=0; i<vector_size; i++) {
        ok(test_oneshot(di, &vector[i]), "test one-shot with data less than blocksize");
        ok(test_discreet(di, &vector[i]), "test discreet with data less than blocksize");
    }
    return 1;
}

static int test_hmac_of_zero(const struct ccdigest_info *di) {
#if hmac_DATA_POINTER_NULL_TOLERANT
    static test_vector vectorNULL = { keystr128, NULL, 0,
 		"a4171a022881b477ebb77da9eccbdaf6", // MD2
 		"752e874f35085e497d5032112cc65131", // MD4
  		"c91e40247251f39bdfe6a7b72a5857f9", // MD5
 		"5433122f77bcf8a4d9b874b4149823ef5b7c207e", // SHA1
 		"4e496054842798a861acb67a9fe85fb7ec7f4d91719f077ea3bd0f72", // SHA224
 		"07eff8b326b7798c9ccfcbdbe579489ac785a7995a04618b1a2813c26744777d", // SHA256
 		"6a0fdc1c54c664ad91c7c157d2670c5d44e4d44ebad2359a0206974c7088b1a867f76971e6c240c33b33a66ba295bb56", // SHA384
 		"2fec800ca276c44985a35aec92067e5e53a1bb80a6fdab1d9c97d54068118f30ad4c33717466d372ea00bbf126e5b79c6f7143dd36c31f72028330e92ae3a359", // SHA512
 		"e9bf401eb338ae9ece9f2de9cc104a5c", // RMD128
 		"8e4b77206892943105b5ec25c0d7132c6f541895", // RMD160
 		"b1641d0919f02d6391379474ab3f8ea05d1ad8bc7c0afc0770f8cd2b9af7552f", // RMD256
 		"8e4b77206892943105b5ec25c0d7132c6f541895", // RMD320
    };
#endif /* hmac_DATA_POINTER_NULL_TOLERANT */
    static test_vector vectorPOINTER = { keystr128, (uint8_t *) "XXXX", 0,
 		"a4171a022881b477ebb77da9eccbdaf6", // MD2
 		"752e874f35085e497d5032112cc65131", // MD4
  		"c91e40247251f39bdfe6a7b72a5857f9", // MD5
 		"5433122f77bcf8a4d9b874b4149823ef5b7c207e", // SHA1
 		"4e496054842798a861acb67a9fe85fb7ec7f4d91719f077ea3bd0f72", // SHA224
 		"07eff8b326b7798c9ccfcbdbe579489ac785a7995a04618b1a2813c26744777d", // SHA256
 		"6a0fdc1c54c664ad91c7c157d2670c5d44e4d44ebad2359a0206974c7088b1a867f76971e6c240c33b33a66ba295bb56", // SHA384
 		"2fec800ca276c44985a35aec92067e5e53a1bb80a6fdab1d9c97d54068118f30ad4c33717466d372ea00bbf126e5b79c6f7143dd36c31f72028330e92ae3a359", // SHA512
 		"e9bf401eb338ae9ece9f2de9cc104a5c", // RMD128
 		"8e4b77206892943105b5ec25c0d7132c6f541895", // RMD160
 		"b1641d0919f02d6391379474ab3f8ea05d1ad8bc7c0afc0770f8cd2b9af7552f", // RMD256
 		"8e4b77206892943105b5ec25c0d7132c6f541895", // RMD320
   };
#if hmac_DATA_POINTER_NULL_TOLERANT
    if(verbose) diag("NULL-Oneshot\n");
    ok_or_fail(test_oneshot(di, &vectorNULL), "test one-shot with NULL pointer");
    ok_or_fail(test_discreet(di, &vectorNULL), "test discreet with NULL pointer");
#endif /* hmac_DATA_POINTER_NULL_TOLERANT */
    if(verbose) diag("Pointer-Oneshot\n");
    ok(test_oneshot(di, &vectorPOINTER), "test one-shot with live pointer");
    ok(test_discreet(di, &vectorPOINTER), "test discreet with live pointer");
    return 1;
}

static int test_hmac(const struct ccdigest_info *di) {
    ok(test_hmac_of_zero(di), "test_hmac_of_zero");
    ok(test_hmac_lt_blocksize(di), "test_hmac_lt_blocksize");
    ok(test_hmac_eq_blocksize(di), "test_hmac_eq_blocksize");
    ok(test_hmac_many_blocks(di), "test_hmac_many_blocks");
    return 1;
}

int cchmactest(TM_UNUSED int argc, TM_UNUSED char *const *argv)
{
#if !defined(__NO_ASM__) && CCSHA1_VNG_INTEL
    plan_tests(848);
#elif !defined(__NO_ASM__) && CCSHA1_VNG_ARMV7NEON
    plan_tests(742);
#else
    plan_tests(636);
#endif
    if(verbose) diag("Starting hmac tests\n");
    ok(test_hmac(&ccmd2_ltc_di), "ccmd2_di");
    ok(test_hmac(&ccmd4_ltc_di), "ccmd4_ltc_di");
    ok(test_hmac(&ccmd5_ltc_di), "ccmd5_ltc_di");
    ok(test_hmac(&ccsha1_ltc_di), "ccsha1_ltc_di");
    ok(test_hmac(&ccsha1_eay_di), "ccsha1_eay_di");
    ok(test_hmac(&ccsha224_ltc_di), "ccsha224_ltc_di");
    ok(test_hmac(&ccsha256_ltc_di), "ccsha256_ltc_di");
    ok(test_hmac(&ccsha384_ltc_di), "ccsha384_ltc_di");
    ok(test_hmac(&ccsha512_ltc_di), "ccsha512_ltc_di");
    ok(test_hmac(&ccrmd128_ltc_di), "ccrmd128_ltc_di");
    ok(test_hmac(&ccrmd160_ltc_di), "ccrmd160_ltc_di");
    ok(test_hmac(&ccrmd256_ltc_di), "ccrmd256_ltc_di");
    // ok(test_hmac(&ccrmd320_ltc_di), "ccrmd320_ltc_di");

#if !defined(__NO_ASM__) && CCSHA1_VNG_INTEL
    ok(test_hmac(&ccsha1_vng_intel_SupplementalSSE3_di), "ccsha1_vng_intel_SupplementalSSE3_di");
    ok(test_hmac(&ccsha1_vng_intel_NOSupplementalSSE3_di), "ccsha1_vng_intel_NOSupplementalSSE3_di");
    ok(test_hmac(&ccsha256_vng_intel_SupplementalSSE3_di), "ccsha256_vng_intel_SupplementalSSE3_di");
    ok(test_hmac(&ccsha256_vng_intel_NOSupplementalSSE3_di), "ccsha256_vng_intel_NOSupplementalSSE3_di");
#endif
#if !defined(__NO_ASM__) && CCSHA1_VNG_ARMV7NEON
    ok(test_hmac(&ccsha1_vng_armv7neon_di), "ccsha1_vng_armv7neon_di");
    ok(test_hmac(&ccsha256_vng_armv7neon_di), "ccsha256_vng_armv7neon_di");
#endif
    return 0;
}
#endif

