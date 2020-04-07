/*
 * Copyright (c) 2013,2014,2015 Apple Inc. All rights reserved.
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
#include "drbg_limits_test.h"

#if (CCRNG == 0)
entryPoint(ccrng,"ccrng")
#else

#include <corecrypto/ccrng.h>
#include <corecrypto/ccrng_system.h>
#include <corecrypto/ccrng_drbg.h>
#include <corecrypto/ccrng_CommonCrypto.h>
#include <corecrypto/ccrng_sequence.h>
#include <corecrypto/ccrng_pbkdf2_prng.h>
#include <corecrypto/ccrng_dummy.h>
#include <corecrypto/ccrng_test.h>
#include <corecrypto/ccaes.h>
#include <corecrypto/ccsha2.h>

#include "ccdrbg_nistctr.h"

static int verbose = 0;

static int compare64_t(const void *va, const void *vb) {
    uint64_t a = *((const uint64_t *) va);
    uint64_t b = *((const uint64_t *) vb);
    if(a < b) return -1;
    if(a == b) return 0;
    return 1;
}

static int no_repeat(struct ccrng_state *rng) {
    uint64_t results[128];
    int status;
    
    for(int i = 0; i<128; i++) {
        if(verbose) diag("Creating %d\n", i);
        status = ccrng_generate(rng, 8, &results[i]);
        ok_or_fail(status == 0, "got random bytes");
    }
    
    qsort(results, 128, sizeof(uint64_t), compare64_t);
    
    for(int i = 0; i < 127; i++) {
        if(verbose) diag("Comparing %d\n", i);
        ok(results[i] != results[i+1], "All values are unique");
    }
    return 1;
}


static int must_repeat(struct ccrng_state *rng) {
    uint64_t results[2];
    int status;
    
    status = ccrng_generate(rng, 8, &results[0]);
    ok_or_fail(status == 0, "got bytes");
    status = ccrng_generate(rng, 8, &results[1]);
    ok_or_fail(status == 0, "got bytes");
    ok_or_fail(results[0] == results[1], "results match");
    return 1;
}

static int too_many(struct ccrng_state *rng, size_t limit, size_t split) {
    // Assume that there are "limit" bytes available, no less, no more.
    uint64_t results[limit+1];
    int status;
    size_t done=0;
    for (size_t i=0;i<split;i++) {
        status = ccrng_generate(rng, (size_t)limit/split, &results[0]);
        ok_or_fail(status == 0, "got bytes");
        done+=(size_t)limit/split;
    }
    status = ccrng_generate(rng, (limit-done)+1, &results[0]);
    ok_or_fail(status == CC_ERR_OUT_OF_ENTROPY, "report lack of bytes"); // Todo: Proper Error code
    return 1;
}

static int must_be(struct ccrng_state *rng, size_t len, char *pattern) {
    uint8_t results[len];
    int status;
    
    status = ccrng_generate(rng, len, results);
    ok_or_fail(status == 0, "got bytes");
    
    
    if(pattern) {
        struct byte_buf resultsByte={len,&results[0]};
        char * got_string = bytesToHexString(&resultsByte);
        eq_string(got_string, pattern, "Results Match");
        free(got_string);
    } else {
        byteBuffer got = bytesToBytes(results, len);
        diag("Results for must_be - %s", bytesToHexString(got));
        free(got);
    }
    return 1;
}


static const struct ccdrbg_nistctr_custom DRBGcustom = {
    .ecb = &ccaes_ltc_ecb_encrypt_mode,
    .keylen = 16,
    .strictFIPS = 1,
    .use_df = 1
};


static uint8_t salt[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
static size_t saltlen = sizeof(salt);

int ccrng(TM_UNUSED int argc, TM_UNUSED char *const *argv)
{
    int test_system_rng = 1;
    int test_nist_rng = 1;
    int test_pbkdf2_rng = 1;
    int test_drbg_rng = 1;
    int test_sequence = 1;
    // int test_dummy = 1;
    int status;

    plan_tests(2127);
    if(verbose) diag("Testing drbg limits\n");
    drbg_limits_test();

    
    if(test_drbg_rng) {
        if(verbose) diag("Testing drbg rng\n");
        struct ccrng_drbg_state drbg_ctx;
        struct ccdrbg_info info;
        char *expect_result_init;
        char *expect_result_reseed;
        for (size_t i=0;i<2;i++) {
            switch (i) {
                case 0:
                {
                    // DRBG - NIST CTR
                    struct ccdrbg_nistctr_custom custom = {
                        .ecb = ccaes_ecb_encrypt_mode(),
                        .keylen = 16,
                        .strictFIPS = 0,
                        .use_df = 1,
                    };
                    ccdrbg_factory_nistctr(&info, &custom);
                    expect_result_init="63ab25cafde05939a75a636287c2c2f1";
                    expect_result_reseed="939cfa09e5a9b8184c849d7668c074d9";
                    break;
                }
                case 1:
                {
                    // DRBG - NIST HMAC
                    struct ccdrbg_nisthmac_custom custom = {
                        .di = ccsha256_di(),
                        .strictFIPS = 0,
                    };
                    ccdrbg_factory_nisthmac(&info, &custom);
                    expect_result_init="d83762537ee61165041c5175ebad7a61";
                    expect_result_reseed="6fc130f1ae32efd9637d6c0bab738d2a";
                    break;
                }
                default:
                    assert(false); // should never get here
                    expect_result_init=NULL;
                    expect_result_reseed=NULL;
                    break;

            }
            // Actual test
            struct ccdrbg_state *state = malloc(info.size);
            const char drbg_init_salt[]="init 17byte seed";
            const char drbg_reseed_salt[]="reseed 19byte seed";
            status = ccrng_drbg_init(&drbg_ctx, &info, state, sizeof(drbg_init_salt), drbg_init_salt);
            ok(status == 0, "Made drbg context");
            if(status == 0) {
                ok(no_repeat((struct ccrng_state *)&drbg_ctx), "successful generation");
                ok(must_be((struct ccrng_state *)&drbg_ctx, 16, expect_result_init), "Got expected pattern init");
                status = ccrng_drbg_reseed(&drbg_ctx, sizeof(drbg_reseed_salt), drbg_reseed_salt, 0, NULL);
                ok(status == 0, "Reseed drbg context");
                ok(no_repeat((struct ccrng_state *)&drbg_ctx), "successful generation");
                ok(must_be((struct ccrng_state *)&drbg_ctx, 16, expect_result_reseed), "Got expected pattern reseed");
            }
            ccrng_drbg_done(&drbg_ctx);
            free(state);
        }

    }

    if(test_system_rng) {
        if(verbose) diag("Testing system rng\n");
        struct ccrng_system_state system_ctx;
        status = ccrng_system_init(&system_ctx);
        ok(status >= 0, "Made system_rng context");
        if(status >= 0) {
            ok(no_repeat((struct ccrng_state *)&system_ctx), "successful generation");
        }
        ccrng_system_done(&system_ctx);
    }

    if(test_nist_rng) {

        if(verbose) diag("Testing nist rng\n");
#if !CC_KERNEL
        __block struct ccrng_CommonCrypto_state nist_ctx;
#else
        struct ccrng_CommonCrypto_state nist_ctx;
#endif
        struct ccdrbg_info info;
        ccdrbg_factory_nistctr(&info, &DRBGcustom);
        struct ccdrbg_state *state = malloc(info.size);
        
        status = ccrng_CommonCrypto_init(&nist_ctx, &info, state, 0);
        ok(status == 0, "Made nist_rng context");

        // Test basic no repeatance
        if(status == 0) {
            ok(no_repeat((struct ccrng_state *)&nist_ctx), "successful generation");
        }

        // Test reseeding
        size_t tmp_size = CCDRBG_MAX_REQUEST_SIZE+8;
        uint8_t *tmp = malloc(tmp_size);
#if CORECRYPTO_DEBUG
        diag("Expect one ccassert here");
#endif
#if !CC_KERNEL
        dispatch_sync(nist_ctx.q, ^{
#endif
            struct ccdrbg_nistctr_state *drbg = (struct ccdrbg_nistctr_state *)nist_ctx.drbg_state;
            drbg->reseed_counter=CCDRBG_RESEED_INTERVAL+1;
#if !CC_KERNEL
        });
#endif
        status = ccrng_generate((struct ccrng_state *)&nist_ctx, tmp_size, tmp);
        ok_or_fail(status == 0, "got random bytes");

        // Test recovering from broken prediction
#if !CC_KERNEL
        dispatch_sync(nist_ctx.q, ^{
#endif
            nist_ctx.predictionBreak_status=CC_ERR_OUT_OF_ENTROPY;
#if !CC_KERNEL
        });
#endif
        status = ccrng_generate((struct ccrng_state *)&nist_ctx, tmp_size, tmp);
        ok_or_fail(status == 0, "recover from predictionBreak error");

        ccrng_CommonCrypto_done(&nist_ctx);

        // Test init after done
        status = ccrng_CommonCrypto_init(&nist_ctx, &info, state, 0);
        ok(status == 0, "Made nist_rng context again");

        // Test basic no repeatance
        if(status == 0) {
            ok(no_repeat((struct ccrng_state *)&nist_ctx), "successful generation again");
        }
        ccrng_CommonCrypto_done(&nist_ctx);

        free(state);
        free(tmp);
        
    }

    if(test_pbkdf2_rng) {
        if(verbose) diag("Testing pbkdf2 rng\n");
        struct ccrng_pbkdf2_prng_state pbkdf2rng_ctx;
        
        status = ccrng_pbkdf2_prng_init(&pbkdf2rng_ctx, 4096,
                                        14, "Test_test_test",
                                        saltlen, salt,
                                        1024);
        ok(status == 0, "Made pbkdf2_rng context");
        if(status == 0) {
            ok(must_be((struct ccrng_state *)&pbkdf2rng_ctx, 16, "7bad1f07c4ff2c9d287974f05cd5ea55"), "Got expected pattern");
            ok(no_repeat((struct ccrng_state *)&pbkdf2rng_ctx), "successful generation");
        }

        // Negative testing to limits, one shot
        ok(ccrng_pbkdf2_prng_init(&pbkdf2rng_ctx, 64,
                                        14, "Test_test_test",
                                        saltlen, salt,
                                  10)==0,"Reinit context");
        ok(too_many((struct ccrng_state *)&pbkdf2rng_ctx,64,0), "too many");

        // Negative testing to limits, 2 calls
        ok(ccrng_pbkdf2_prng_init(&pbkdf2rng_ctx, 64,
                                  14, "Test_test_test",
                                  saltlen, salt,
                                  10)==0,"Reinit context");
        ok(too_many((struct ccrng_state *)&pbkdf2rng_ctx,64,1), "too many");

        // Negative testing to limits, 4 calls
        ok(ccrng_pbkdf2_prng_init(&pbkdf2rng_ctx, 64,
                                  14, "Test_test_test",
                                  saltlen, salt,
                                  10)==0,"Reinit context");
        ok(too_many((struct ccrng_state *)&pbkdf2rng_ctx,64,3), "too many");

        // Negative testing, invalid initialization
        ok(ccrng_pbkdf2_prng_init(&pbkdf2rng_ctx, sizeof(pbkdf2rng_ctx.random_buffer)+1,
                                  14, "Test_test_test",
                                  saltlen, salt,
                                  10)==CC_ERR_PARAMETER,"Reinit context");
        ok(too_many((struct ccrng_state *)&pbkdf2rng_ctx,0,0), "any is too many when init fails");
    }

    if(test_sequence) {
        if(verbose) diag("Testing sequence rng\n");
        struct ccrng_sequence_state sequence_ctx;
        
        status = ccrng_sequence_init(&sequence_ctx, saltlen, salt);
 
        ok(status == 0, "Made sequence context");
        if(status == 0) {
            ok(must_repeat((struct ccrng_state *)&sequence_ctx), "successful generation");
            ok(must_be((struct ccrng_state *)&sequence_ctx, saltlen, "000102030405060708090a0b0c0d0e0f"), "Got expected pattern");
        }
    }

#if 0
    if(test_dummy) {
        if(verbose) diag("Testing dummy rng\n");
        struct ccrng_dummy_state dummy_ctx;
        
        status = ccrng_dummy_init(&dummy_ctx);
        
        ok(status == 0, "Made dummy context");
        if(status == 0) {
            ok(must_be((struct ccrng_state *)&dummy_ctx, 4, "00010203"), "Got expected pattern");
        }
    }
#endif

    return 1;
}
#endif


