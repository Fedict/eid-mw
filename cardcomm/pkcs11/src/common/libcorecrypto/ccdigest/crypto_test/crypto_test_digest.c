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

#if (CCDIGESTTEST == 0)
entryPoint(ccdigesttest,"ccdigest test")
#else
#include <corecrypto/ccasn1.h>
#include <corecrypto/ccdigest.h>
#import <corecrypto/ccmd2.h>
#import <corecrypto/ccmd4.h>
#import <corecrypto/ccmd5.h>
#import <corecrypto/ccsha1.h>
#import <corecrypto/ccsha2.h>
#import <corecrypto/ccripemd.h>

/* Currently, ccdigest and friends won't work when length == 0 and the
 * data pointer is NULL.
 */

#define DIGEST_DATA_POINTER_NULL_TOLERANT 0

typedef struct test_vector_t {
    uint8_t *input;
    size_t len;
    uint8_t *md2_answer;
    uint8_t *md4_answer;
    uint8_t *md5_answer;
    uint8_t *sha1_answer;
    uint8_t *sha224_answer;
    uint8_t *sha256_answer;
    uint8_t *sha384_answer;
    uint8_t *sha512_answer;
    uint8_t *rmd128_answer;
    uint8_t *rmd160_answer;
    uint8_t *rmd256_answer;
    uint8_t *rmd320_answer;
} test_vector;

static int test_answer(const struct ccdigest_info *di, test_vector *vector, void*answer) {
    uint8_t *correct_answer = NULL;
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
        printByteBuffer(answer_bb, "Answer Provided");
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

static int test_discreet(const struct ccdigest_info *di, test_vector *vector) {
    uint8_t answer[128];
    size_t total = vector->len;
    size_t chunk = vector->len/2;
    uint8_t *p = vector->input;
    uint8_t ctxfrontguard[4096];
    ccdigest_di_decl(di, ctx);
    uint8_t ctxrearguard[4096];
    memset(ctxfrontguard, 0xee, 4096);
    memset(ctxrearguard, 0xee, 4096);
    // break it up into pieces.
    ccdigest_init(di, ctx);
    ok(guard_ok(ctxfrontguard, 0xee, 4096), "context is safe");
    ok(guard_ok(ctxrearguard, 0xee, 4096), "context is safe");
    do {
        ccdigest_update(di, ctx, chunk, p);
        total -= chunk;
        p += chunk;
        chunk /= 2;
        if(chunk == 0) chunk = total;
    } while(total);
    ok(guard_ok(ctxfrontguard, 0xee, 4096), "context is safe");
    ok(guard_ok(ctxrearguard, 0xee, 4096), "context is safe");
    
    ccdigest_final(di, ctx, answer);
    
    ok(guard_ok(ctxfrontguard, 0xee, 4096), "context is safe");
    ok(guard_ok(ctxrearguard, 0xee, 4096), "context is safe");
    ok(test_answer(di, vector, answer), "check answer");
    return 1;
}

static int test_oneshot(const struct ccdigest_info *di, test_vector *vector) {
    uint8_t answer[128];
    ccdigest(di, vector->len, vector->input, answer);
    ok(test_answer(di, vector, answer), "check answer");
    return 1;
}

static int test_digest_many_blocks(const struct ccdigest_info *di) {
    static const size_t buffer_size = 16383;
    static uint8_t buffer[buffer_size];
    static test_vector vector[] = {
        { buffer, buffer_size,
            (uint8_t *) "460825c272c70b141a24364117e1242e", //MD2
            (uint8_t *) "45eb5470a0e700ef2f47e408652792dc", //MD4
            (uint8_t *) "ef17d771a405e1effd7fb6a1f9950018", //MD5
            (uint8_t *) "217edfa45f521c1232e4cc2cacac29bf8a9d1e66", //SHA1
            (uint8_t *) "bfcef78f215b9682767c4ba7404379ef87012c4b7346631ccb965c2d", //SHA224
            (uint8_t *) "fab82f1352405c22ca2953ff80a508e5567c51e1a9aeb57cf9a56447e40ba066", //SHA256
            (uint8_t *) "220dfd6babc12b08f6f456133d52aa2975dfb50689de810ab0fa8cd9a7650218dc6afaf24f77f6b969f9ea7141f9aeb7", //SHA384
            (uint8_t *) "a238834f3693080d7cce3c44c7600e8a09311ba8e6059002bc67d4158430148abd2d8255afdf3b2f944fa5e1025dd6c4646f5dd9f6858ee1222a67430a24d491", //SHA512
            (uint8_t *) "205f2c6a034b5ce4c845acc125cde8be", //RMD128
            (uint8_t *) "d6cca8771842686d759d5778702c24bcc4d355e0", //RMD160
            (uint8_t *) "f2226ceb81b7501a648c6d9f085c7735bbb5825bccf3d02682579f1c778518cd", //RMD256
            (uint8_t *) "205f2c6a034b5ce4c845acc125cde8be", //RMD320
        },
    };
    int vector_size = sizeof (vector) / sizeof (test_vector);
    for(size_t n=0; n<buffer_size; n++) buffer[n] = n&0xff;
    if(verbose) diag("Digest Large Test");
    for(int i=0; i<vector_size; i++) {
        ok(test_oneshot(di, &vector[i]), "test one-shot with data less than blocksize");
        ok(test_discreet(di, &vector[i]), "test discreet with data less than blocksize");
    }
    return 1;
}

static int test_digest_eq_blocksize(const struct ccdigest_info *di) {
    static test_vector vector[] = {
        { (uint8_t *) "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF", 128,
            (uint8_t *) "33a584d542532ca385fb9da278844fe1", //MD2
            (uint8_t *) "f7a45071e2b72f5ecbdec4e30342883a", //MD4
            (uint8_t *) "b63f67ab30bf8e5fcada5bd0ce610bf3", //MD5
            (uint8_t *) "82e0a30115ca91906859075062e59f25d3c42949", //SHA1
            (uint8_t *) "60ea0c511d336cbf383e67e139c5e4672e73536cc7cce168b90bbcf9", //SHA224
            (uint8_t *) "16f3e2071629d02b0ba9e4a43643f6976514ebd8b4b8f0f9ebf3bd7cde6463d8", //SHA256
            (uint8_t *) "7b61f365fe573d3691bef585f5c5862210223bfb08d9994762b6c54308a4f9934d8c35b3823255116c1e63f821bd794d", //SHA384
            (uint8_t *) "92fd0a1e6218274d4ab9824bf2be236ef8bdc5bd5fead472e04850f01aabcdfa8ecccc8d690fd86ae2295886ff26b4602e8f8651d12434a3cef0b4aff8ca13b4", //SHA512
            (uint8_t *) "3db5bd58aec5be6cab79bbf59ca352df", //RMD128
            (uint8_t *) "4c06c1234bcf7345d1fda40ae79618ad35eca158", //RMD160
            (uint8_t *) "a4601915d4bed490452f25e8e742488a663dcc2e566950a62ea9c019a50bb682", //RMD256
            (uint8_t *) "4c06c1234bcf7345d1fda40ae79618ad35eca158", //RMD320
        },
    };
    int vector_size = sizeof (vector) / sizeof (test_vector);
    if(verbose) diag("Digest EQ Test");
    for(int i=0; i<vector_size; i++) {
        ok(test_oneshot(di, &vector[i]), "test one-shot with data less than blocksize");
        ok(test_discreet(di, &vector[i]), "test discreet with data less than blocksize");
    }
    return 1;
}

static int test_digest_lt_blocksize(const struct ccdigest_info *di) {
    static test_vector vector[] = {
        { (uint8_t *) "Test vector from febooti.com", 28,
        (uint8_t *) "db128d6e0d20a1192a6bd1fade401150", //MD2
        (uint8_t *) "6578f2664bc56e0b5b3f85ed26ecc67b", //MD4
        (uint8_t *) "500ab6613c6db7fbd30c62f5ff573d0f", //MD5
        (uint8_t *) "a7631795f6d59cd6d14ebd0058a6394a4b93d868", //SHA1
        (uint8_t *) "3628b402254caa96827e3c79c0a559e4558da8ee2b65f1496578137d", //SHA224
        (uint8_t *) "077b18fe29036ada4890bdec192186e10678597a67880290521df70df4bac9ab", //SHA256
        (uint8_t *) "388bb2d487de48740f45fcb44152b0b665428c49def1aaf7c7f09a40c10aff1cd7c3fe3325193c4dd35d4eaa032f49b0", //SHA384
        (uint8_t *) "09fb898bc97319a243a63f6971747f8e102481fb8d5346c55cb44855adc2e0e98f304e552b0db1d4eeba8a5c8779f6a3010f0e1a2beb5b9547a13b6edca11e8a", //SHA512
        (uint8_t *) "ab076efaab01d30d16bb57f88d63c073", //RMD128
        (uint8_t *) "4e1ff644ca9f6e86167ccb30ff27e0d84ceb2a61", //RMD160
        (uint8_t *) "3bcbe8d6c9cf2cff39fb53e0dcef37f1554223da45d941d95836e1f5f84677eb", //RMD256
        (uint8_t *) "ab076efaab01d30d16bb57f88d63c073", //RMD320
        },
    };
    int vector_size = sizeof (vector) / sizeof (test_vector);
    if(verbose) diag("Digest LT Test");

    for(int i=0; i<vector_size; i++) {
        ok(test_oneshot(di, &vector[i]), "test one-shot with data less than blocksize");
        ok(test_discreet(di, &vector[i]), "test discreet with data less than blocksize");
    }
    return 1;
}

static int test_digest_of_zero(const struct ccdigest_info *di) {
#if DIGEST_DATA_POINTER_NULL_TOLERANT
    static test_vector vectorNULL = { NULL, 0,
        (uint8_t *) "8350e5a3e24c153df2275c9f80692773", //MD2
        (uint8_t *) "31d6cfe0d16ae931b73c59d7e0c089c0", //MD4
        (uint8_t *) "d41d8cd98f00b204e9800998ecf8427e", //MD5
        (uint8_t *) "da39a3ee5e6b4b0d3255bfef95601890afd80709", //SHA1
        (uint8_t *) "d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f", //SHA224
        (uint8_t *) "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855", //SHA256
        (uint8_t *) "38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b", //SHA384
        (uint8_t *) "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e", //SHA512
        (uint8_t *) "cdf26213a150dc3ecb610f18f6b38b46", //RMD128
        (uint8_t *) "9c1185a5c5e9fc54612808977ee8f548b2258d31", //RMD160
        (uint8_t *) "02ba4c4e5f8ecd1877fc52d64d30e37a2d9774fb1e5d026380ae0168e3c5522d", //RMD256
        (uint8_t *) "22d65d5661536cdc75c1fdf5c6de7b41b9f27325ebc61e8557177d705a0ec880151c3a32a00899b8", //RMD320
    };
#endif /* DIGEST_DATA_POINTER_NULL_TOLERANT */
    static test_vector vectorPOINTER = { (uint8_t *) "XXXX", 0,
        (uint8_t *) "8350e5a3e24c153df2275c9f80692773", //MD2
        (uint8_t *) "31d6cfe0d16ae931b73c59d7e0c089c0", //MD4
        (uint8_t *) "d41d8cd98f00b204e9800998ecf8427e", //MD5
        (uint8_t *) "da39a3ee5e6b4b0d3255bfef95601890afd80709", //SHA1
        (uint8_t *) "d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f", //SHA224
        (uint8_t *) "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855", //SHA256
        (uint8_t *) "38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b", //SHA384
        (uint8_t *) "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e", //SHA512
        (uint8_t *) "cdf26213a150dc3ecb610f18f6b38b46", //RMD128
        (uint8_t *) "9c1185a5c5e9fc54612808977ee8f548b2258d31", //RMD160
        (uint8_t *) "02ba4c4e5f8ecd1877fc52d64d30e37a2d9774fb1e5d026380ae0168e3c5522d", //RMD256
        (uint8_t *) "22d65d5661536cdc75c1fdf5c6de7b41b9f27325ebc61e8557177d705a0ec880151c3a32a00899b8", //RMD320
    };
#if DIGEST_DATA_POINTER_NULL_TOLERANT
    if(verbose) diag("NULL-Oneshot");
    ok_or_fail(test_oneshot(di, &vectorNULL), "test one-shot with NULL pointer");
    ok_or_fail(test_discreet(di, &vectorNULL), "test discreet with NULL pointer");
#endif /* DIGEST_DATA_POINTER_NULL_TOLERANT */
    if(verbose) diag("Pointer-Oneshot");
    ok(test_oneshot(di, &vectorPOINTER), "test one-shot with live pointer");
    ok(test_discreet(di, &vectorPOINTER), "test discreet with live pointer");
    return 1;
}

static int test_digest(const struct ccdigest_info *di) {
    ok(test_digest_of_zero(di), "test_digest_of_zero");
    ok(test_digest_lt_blocksize(di), "test_digest_lt_blocksize");
    ok(test_digest_eq_blocksize(di), "test_digest_eq_blocksize");
    ok(test_digest_many_blocks(di), "test_digest_many_blocks");
    return 1;
}

int ccdigesttest(TM_UNUSED int argc, TM_UNUSED char *const *argv)
{
#if !defined(__NO_ASM__) && CCSHA1_VNG_INTEL
	plan_tests(848);
#elif !defined(__NO_ASM__) && CCSHA1_VNG_ARMV7NEON
	plan_tests(742);
#else
        plan_tests(636);
#endif
    if(verbose) diag("Starting digest tests");
    ok(test_digest(&ccmd2_ltc_di), "ccmd2_di");
    ok(test_digest(&ccmd4_ltc_di), "ccmd4_ltc_di");
    ok(test_digest(&ccmd5_ltc_di), "ccmd5_ltc_di");
    ok(test_digest(&ccsha1_ltc_di), "ccsha1_ltc_di");
    ok(test_digest(&ccsha1_eay_di), "ccsha1_eay_di");
    ok(test_digest(&ccsha224_ltc_di), "ccsha224_ltc_di");
    ok(test_digest(&ccsha256_ltc_di), "ccsha256_ltc_di");
    ok(test_digest(&ccsha384_ltc_di), "ccsha384_ltc_di");
    ok(test_digest(&ccsha512_ltc_di), "ccsha512_ltc_di");
    ok(test_digest(&ccrmd128_ltc_di), "ccrmd128_ltc_di");
    ok(test_digest(&ccrmd160_ltc_di), "ccrmd160_ltc_di");
    ok(test_digest(&ccrmd256_ltc_di), "ccrmd256_ltc_di");
    // ok(test_digest(&ccrmd320_ltc_di), "ccrmd320_ltc_di");

#if !defined(__NO_ASM__) && CCSHA1_VNG_INTEL
    ok(test_digest(&ccsha1_vng_intel_SupplementalSSE3_di), "ccsha1_vng_intel_SupplementalSSE3_di");
    ok(test_digest(&ccsha1_vng_intel_NOSupplementalSSE3_di), "ccsha1_vng_intel_NOSupplementalSSE3_di");
    ok(test_digest(&ccsha256_vng_intel_SupplementalSSE3_di), "ccsha256_vng_intel_SupplementalSSE3_di");
    ok(test_digest(&ccsha256_vng_intel_NOSupplementalSSE3_di), "ccsha256_vng_intel_NOSupplementalSSE3_di");
#endif
#if !defined(__NO_ASM__) && CCSHA1_VNG_ARMV7NEON
    ok(test_digest(&ccsha1_vng_armv7neon_di), "ccsha1_vng_armv7neon_di");
    ok(test_digest(&ccsha256_vng_armv7neon_di), "ccsha256_vng_armv7neon_di");
#endif
    return 0;
}
#endif

