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

#define USE_SUPER_COOL_NEW_CCOID_T
#include "testmore.h"
#include "testbyteBuffer.h"
#include "testccnBuffer.h"

static int verbose = 0;

#if (CCPBKDF2TEST == 0)
entryPoint(ccpbkdf2test,"ccpbkdf2 test")
#else
#include <corecrypto/ccasn1.h>
#include <corecrypto/ccdigest.h>
#include <corecrypto/ccpbkdf2.h>
#import <corecrypto/ccmd2.h>
#import <corecrypto/ccmd4.h>
#import <corecrypto/ccmd5.h>
#import <corecrypto/ccsha1.h>
#import <corecrypto/ccsha2.h>
#import <corecrypto/ccripemd.h>

#if !defined(__NO_ASM__) && CCSHA1_VNG_INTEL
#if defined(__x86_64__)
static const int kTestTestCount = 192;
#else
static const int kTestTestCount = 160;
#endif
#elif !defined(__NO_ASM__) && CCSHA1_VNG_ARMV7NEON
#if defined(__arm64__)
static const int kTestTestCount = 168;
#else
static const int kTestTestCount = 140;
#endif
#else
static const int kTestTestCount = 120;
#endif

/* Currently, ccpbkdf2 and friends won't work when length == 0 and the
 * data pointer is NULL.
 */

#define password1     "password"
#define saltstr128    "000102030405060708090a0b0c0d0e0f"


#define pbkdf2_DATA_POINTER_NULL_TOLERANT 0

typedef struct test_vector_t {
    char *password;
    char *saltStr;
    unsigned long iterations;
    size_t result_len;
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

static int test_answer(const struct ccdigest_info *di, test_vector *vector, size_t answer_len, void*answer) {
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
    byteBuffer answer_bb = bytesToBytes(answer, answer_len);
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

static int test_oneshot(const struct ccdigest_info *di, test_vector *vector) {
    uint8_t answer[vector->result_len];
    byteBuffer salt = hexStringToBytes(vector->saltStr);
    ccpbkdf2_hmac(di, strlen(vector->password), vector->password, salt->len, salt->bytes, vector->iterations, vector->result_len, answer);
    ok(test_answer(di, vector, vector->result_len, answer), "check answer");
    free(salt);
    return 1;
}

static int test_size(const struct ccdigest_info *di) {
    size_t size_chk = 40;
    uint8_t output_buf[size_chk];
    static char *password = "test";
    static const uint8_t salt[16] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    };
    
    if(sizeof(size_t) > 4) { // only valid for 64 bit.
        size_t hlen = di->output_size;
        size_t too_big = hlen * ((size_t) UINT32_MAX + 1);
        ok(ccpbkdf2_hmac(di, strlen(password), password, sizeof(salt), salt, 10, too_big, output_buf) != 0, "Fails with size too big");
        ok(ccpbkdf2_hmac(di, strlen(password), password, sizeof(salt), salt, 10, size_chk, output_buf) == 0, "Passes");
    }
    return 1;
    
}

static int test_pbkdf2(const struct ccdigest_info *di) {
    static test_vector vector[] = {
        { password1, saltstr128, 100, 16,
            "4fb479b7843efec99b2a9d137682bba2", // MD2
            "a941b5a1246eb37e7d8bbfb803257d29", // MD4
            "c21f6f8b192757dc79bfd67378255152", // MD5
            "f61e774036c4007d4402fe9912e29a85", // SHA1
            "78e396298731187a7e9a355694296b1f", // SHA224
            "854a1e9b6834413dc51b4316dbe405e1", // SHA256
            "813022dc740ec35fb4c4eaf67cd42539", // SHA384
            "9842d62607fa2b5d5165d6526f74e119", // SHA512
            "5dd09887fd05b8140c51883f6bed9949", // RMD128
            "b68e9c0c55f521c3f2fcc84320a089de", // RMD160
            "1cdb7e76936a6c2f528a38521dab24a8", // RMD256
            "b68e9c0c55f521c3f2fcc84320a089de", // RMD320
        },
        { password1, saltstr128, 1000, 24,
            "ec806929819bb71b46b6552ac71d0e141af5360bfca0c03c", // MD2
            "b8d26c6338b43b11fe2c8c7a45d31b015f72ae7cfde36778", // MD4
            "cb2c261847e9e1c3141478bd084565da00024366ec9d167e", // MD5
            "0309e2fe4e0bdfe7d0fe4828d41c234416e2d9bfb61cdd8f", // SHA1
            "7d8603adef1af3704db8c7d2c471661ca73ac07c9044a5dc", // SHA224
            "25eb86acc76e43018f18b9a8f90c2fed462d1c799e83d48a", // SHA256
            "82d915ec6e30a50a987fe17cc6d260194c33fec4f2f14196", // SHA384
            "c74e4080d0fbb41fee5868c0ff60fd75acae2638215987e5", // SHA512
            "db8cc4a86241e5f9f4f2be8a020d2db8a6b065dc1604c92f", // RMD128
            "e4d14f220779d824d281b50a5c688a4071219411ce4ece1c", // RMD160
            "118c9330daafb701cd37c3f8e0f5fc1a1846b5e72153a65f", // RMD256
            "e4d14f220779d824d281b50a5c688a4071219411ce4ece1c", // RMD320
        },
        { password1, saltstr128, 10000, 8,
            "3e6698827388fc04", // MD2
            "a7e2590655919ffc", // MD4
            "d352d8ec8e276adc", // MD5
            "8e3e2f73c3eb6390", // SHA1
            "a7150cd1d2a90e2d", // SHA224
            "eb6c81535592203c", // SHA256
            "24f67028f09c4d89", // SHA384
            "5e5984ca905a5524", // SHA512
            "468f482fbf85e70b", // RMD128
            "b35814741b948ecb", // RMD160
            "9d544ae850aea450", // RMD256
            "b35814741b948ecb", // RMD320
        },

    };
    int vector_size = sizeof (vector) / sizeof (test_vector);
    if(verbose) diag("pbkdf2 LT Test\n");

    for(int i=0; i<vector_size; i++) {
        ok(test_oneshot(di, &vector[i]), "test one-shot with data less than blocksize");
    }
    test_size(di);
    return 1;
}

int ccpbkdf2test(TM_UNUSED int argc, TM_UNUSED char *const *argv)
{
	plan_tests(kTestTestCount);


    if(verbose) diag("Starting pbkdf2 tests\n");
    ok(test_pbkdf2(&ccmd2_ltc_di), "ccmd2_di");
    ok(test_pbkdf2(&ccmd4_ltc_di), "ccmd4_ltc_di");
    ok(test_pbkdf2(&ccmd5_ltc_di), "ccmd5_ltc_di");
    ok(test_pbkdf2(&ccsha1_ltc_di), "ccsha1_ltc_di");
    ok(test_pbkdf2(&ccsha1_eay_di), "ccsha1_eay_di");
    ok(test_pbkdf2(&ccsha224_ltc_di), "ccsha224_ltc_di");
    ok(test_pbkdf2(&ccsha256_ltc_di), "ccsha256_ltc_di");
    ok(test_pbkdf2(&ccsha384_ltc_di), "ccsha384_ltc_di");
    ok(test_pbkdf2(&ccsha512_ltc_di), "ccsha512_ltc_di");
    ok(test_pbkdf2(&ccrmd128_ltc_di), "ccrmd128_ltc_di");
    ok(test_pbkdf2(&ccrmd160_ltc_di), "ccrmd160_ltc_di");
    ok(test_pbkdf2(&ccrmd256_ltc_di), "ccrmd256_ltc_di");
    // ok(test_pbkdf2(&ccrmd320_ltc_di), "ccrmd320_ltc_di");

#if !defined(__NO_ASM__) && CCSHA1_VNG_INTEL
    ok(test_pbkdf2(&ccsha1_vng_intel_SupplementalSSE3_di), "ccsha1_vng_intel_SupplementalSSE3_di");
    ok(test_pbkdf2(&ccsha1_vng_intel_NOSupplementalSSE3_di), "ccsha1_vng_intel_NOSupplementalSSE3_di");
    ok(test_pbkdf2(&ccsha256_vng_intel_SupplementalSSE3_di), "ccsha256_vng_intel_SupplementalSSE3_di");
    ok(test_pbkdf2(&ccsha256_vng_intel_NOSupplementalSSE3_di), "ccsha256_vng_intel_NOSupplementalSSE3_di");
#endif
#if !defined(__NO_ASM__) &&  CCSHA1_VNG_ARMV7NEON
    ok(test_pbkdf2(&ccsha1_vng_armv7neon_di), "ccsha1_vng_armv7neon_di");
    ok(test_pbkdf2(&ccsha256_vng_armv7neon_di), "ccsha256_vng_armv7neon_di");
#endif
    return 0;
}
#endif

