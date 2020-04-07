/*
 * Copyright (c) 2014,2015 Apple Inc. All rights reserved.
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


#include "cc_unit.h"
#import "DHValidation.h"
#include <corecrypto/ccdh.h>
#import <corecrypto/ccrng_sequence.h>
#include <corecrypto/cc_config.h>


@implementation DHValidation

#define F false
#define P true

static const struct ccdh_compute_vector dh_compute_vectors[]=
{
#include "DH.inc"
};

#define N_COMPUTE_VECTORS (sizeof(dh_compute_vectors)/sizeof(dh_compute_vectors[0]))


- (void) testDHCompute {
    for(unsigned int i=0; i<N_COMPUTE_VECTORS; i++) {
        XCTAssertEqual(0, ccdh_test_compute_vector(&dh_compute_vectors[i]),@"Vector %d", i);
    }
}

#include <corecrypto/ccdh_gp.h>


/* 
  This test generate 2 random key pairs for a given group and do the key exchange both way,
  Test fail if the generated secrets do not match
*/

- (void) DHGenerate :(ccdh_const_gp_t) gp {

    struct ccrng_sequence_state seq_rng;
    struct ccrng_state *rng_dummy=(struct ccrng_state *)&seq_rng;

    XCTestRNG(rng,"");

    /* Key exchange with l */
    const cc_size n = ccdh_gp_n(gp);
    const size_t s = ccn_sizeof_n(n);
    uint8_t key_seed[s];
    ccdh_full_ctx_decl(s, a);
    ccdh_full_ctx_decl(s, b);
    cc_unit z1[n], z2[n];
    size_t private_key_length;

    XCTAssertEqual(ccdh_gp_prime_bitlen(gp),ccn_bitsof_n(n), @"Bitlength");

    XCTAssertTrue(!ccdh_generate_key(gp, rng, a), @"Computing first key");
    private_key_length=ccn_bitlen(n,ccdh_ctx_x(a));
    if (ccdh_gp_order_bitlen(gp)) {
        // Probabilistic test. Fails with prob < 2^-64
        XCTAssertTrue((private_key_length<=ccdh_gp_order_bitlen(gp))
                      && (private_key_length>ccdh_gp_order_bitlen(gp)-64),
                    @"Checking private key length is exactly l");
    }
    else if (ccdh_gp_l(gp)) {
        XCTAssertTrue(private_key_length==ccdh_gp_l(gp),
                      @"Checking private key length is exactly l");
    }

    XCTAssertTrue(!ccdh_generate_key(gp, rng, b), @"Computing second key");
    private_key_length=ccn_bitlen(n,ccdh_ctx_x(a));
    if (ccdh_gp_order_bitlen(gp)) {
        // Probabilistic test. Fails with prob < 2^-64
        XCTAssertTrue((private_key_length<=ccdh_gp_order_bitlen(gp))
                      && (private_key_length>ccdh_gp_order_bitlen(gp)-64),
                      @"Checking private key length is exactly l");
    }
    else if (ccdh_gp_l(gp)) {
        XCTAssertTrue(private_key_length==ccdh_gp_l(gp),
                      @"Checking private key length is exactly l");
    }
    
    XCTAssertTrue(!ccdh_compute_key(a, b, z1), @"Computing first secret");
    XCTAssertTrue(!ccdh_compute_key(b, a, z2), @"Computing second secret");

    XCAssertMemEquals(s, z1, z2, @"Computed secrets dont match");

    /* Key exchange without l, 4 steps. */
    ccdh_gp_decl(ccn_sizeof_n(n), gp2);
    ccdh_gp_t gp_local = (ccdh_gp_t)gp2;
    CCZP_N(gp_local.zp) = n;

    // a) encode / decode in gp_local
    size_t encSize = ccder_encode_dhparams_size(gp);
    uint8_t *encder = malloc(encSize);
    uint8_t *encder_end = encder + encSize;
    XCTAssertTrue(ccder_encode_dhparams(gp, encder, encder_end)==encder,@"Encode failed");
    XCTAssertTrue(ccder_decode_dhparams(gp_local, encder, encder_end)!=NULL,@"Decode failed");
    free(encder);

    // b) Force l to 0
    CCDH_GP_L(gp_local)=0;

    // c) re-generate the key a
    XCTAssertTrue(!ccdh_generate_key(gp_local, rng, a), @"Computing first key with l=0");
    XCTAssertTrue((ccn_bitlen(n,ccdh_ctx_x(a))<=ccn_bitlen(n,ccdh_ctx_prime(a)))
                  && (ccn_bitlen(n,ccdh_ctx_x(a))>=ccn_bitlen(n,ccdh_ctx_prime(a)))-64,
                  @"Checking private key length when l==0");


    // d) Key exchange
    XCTAssertTrue(!ccdh_compute_key(a, b, z1), @"Computing first secret");
    XCTAssertTrue(!ccdh_compute_key(b, a, z2), @"Computing second secret");
    XCAssertMemEquals(s, z1, z2, @"Computed secrets dont match");

    // e) re-generate the key a = p-2
    cc_unit p_minus_2[n];
    ccn_sub1(n,p_minus_2,ccdh_ctx_prime(a),2);
    memcpy(key_seed,p_minus_2,s);
    ccrng_sequence_init(&seq_rng,sizeof(key_seed),key_seed);

    XCTAssertTrue(!ccdh_generate_key(gp_local, rng_dummy, a), @"Private key with random = p-2");
    XCAssertMemEquals(s, ccdh_ctx_x(a), p_minus_2, @"Private key is p-2");

    // f) re-generate the key a = 1
    memset(key_seed,0x00,s);
    key_seed[0]=1;
    ccrng_sequence_init(&seq_rng,sizeof(key_seed),key_seed);
    XCTAssertTrue(!ccdh_generate_key(gp_local, rng_dummy, a), @"Private key with random = 1");
    XCAssertMemEquals(s, ccdh_ctx_x(a), key_seed, @"Private key is 1");

    /* Negative testing */

    // 1) Bad random
    ccrng_sequence_init(&seq_rng,0,NULL);
    XCTAssertEqual(ccdh_generate_key(gp, rng_dummy, a),
                   CC_ERR_CRYPTO_CONFIG,
                   @"Error random");

    // 2) Random too big
    uint8_t c=0xff;
    ccrng_sequence_init(&seq_rng,1,&c);
    XCTAssertEqual(ccdh_generate_key(gp_local, rng_dummy, a),
                   CCDH_GENERATE_KEY_TOO_MANY_TRIES,
                   @"Value consistently too big (all FF)");

    // 3) Random too big p-1
    memcpy(key_seed,ccdh_ctx_prime(a),s);
    key_seed[0]^=1;
    ccrng_sequence_init(&seq_rng,1,&c);
    XCTAssertEqual(ccdh_generate_key(gp_local, rng_dummy, a),
                   CCDH_GENERATE_KEY_TOO_MANY_TRIES,
                   @"Value consistently too big (p-1)");

    // 4) Random zero
    c=0;
    ccrng_sequence_init(&seq_rng,1,&c);
    XCTAssertEqual(ccdh_generate_key(gp_local, rng_dummy, a),
                   CCDH_GENERATE_KEY_TOO_MANY_TRIES,
                   @"Value consistently zero");

    /* Finished */
    XCTestRNG_Done(rng);
}

#define TEST_GP(_name_) - (void) test_##_name_ { [self DHGenerate:ccdh_gp_##_name_()]; }
#define TEST_GP_SRP(_name_) - (void) test_##_name_ { [self DHGenerate:ccsrp_gp_##_name_()]; }


TEST_GP(rfc5114_MODP_1024_160)
TEST_GP(rfc5114_MODP_2048_224)
TEST_GP(rfc5114_MODP_2048_256)
TEST_GP(rfc3526group05)
TEST_GP(rfc3526group14)
TEST_GP(rfc3526group15)
TEST_GP(rfc3526group16)
TEST_GP(rfc3526group17)
TEST_GP(rfc3526group18)
TEST_GP_SRP(rfc5054_1024)
TEST_GP_SRP(rfc5054_2048)
TEST_GP_SRP(rfc5054_3072)
TEST_GP_SRP(rfc5054_4096)
TEST_GP_SRP(rfc5054_8192)

@end
