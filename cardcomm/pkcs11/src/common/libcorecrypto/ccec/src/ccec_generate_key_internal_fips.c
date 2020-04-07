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


#include <corecrypto/ccec_priv.h>
#include <corecrypto/ccrng.h>
#include <corecrypto/cczp.h>
#include <corecrypto/cc_macros.h>
#include <corecrypto/cc_debug.h>

#define MAX_RETRY 100

/* Implementation per FIPS186-4 - "TestingCandidates" */

int
ccec_generate_key_internal_fips(ccec_const_cp_t cp,  struct ccrng_state *rng, ccec_full_ctx_t key)
{
    int result=CCEC_GENERATE_KEY_DEFAULT_ERR;

    /* Get base point G in projected form. */
    ccec_point_decl_cp(cp, base);
    cczp_const_t zq = ccec_cp_zq(cp);
    cc_require((result=ccec_projectify(cp, base, ccec_cp_g(cp),rng))==0,errOut);

    /* Generate a random private key k. */
    ccec_ctx_init(cp, key);
    cc_unit *k = ccec_ctx_k(key);
    cc_unit *q_minus_2 = ccec_ctx_x(key); // used as temp buffer
    int cmp_result=1;

    /* Need to test candidate against q-2 */
    ccn_sub1(ccec_cp_n(cp), q_minus_2, cczp_prime(zq), 2);
    size_t i;

    /* Generate adequate random for private key */
    for (i = 0; i < MAX_RETRY && cmp_result>0; i++)
    {
        /* Random bits */
        cc_require(((result = ccn_random_bits(ccec_cp_order_bitlen(cp), k, rng)) == 0),errOut);

        /* If k <= q-2, the number is valid */
        cmp_result=ccn_cmp(ccec_cp_n(cp), k, q_minus_2);
    }
    if (i >= MAX_RETRY)
    {
        result=CCEC_GENERATE_KEY_TOO_MANY_TRIES;
    }
    else
    {
        cc_assert(cmp_result<=0);
        /* k is now in range [ 0, q-2 ] ==> +1 for range [ 1, q-1 ] */
        ccn_add1(ccec_cp_n(cp), k, k, 1);

        /* Calculate the public key for k. */
        cc_require_action(ccec_mult(cp, ccec_ctx_point(key), k, base,rng) == 0  ,errOut,
                            result=CCEC_GENERATE_KEY_MULT_FAIL);
        cc_require_action(ccec_is_point_projective(cp, ccec_ctx_point(key)),errOut,
                            result=CCEC_GENERATE_NOT_ON_CURVE);
        cc_require_action(ccec_affinify(cp, ccec_ctx_point(key), ccec_ctx_point(key)) == 0,errOut,
                            result=CCEC_GENERATE_KEY_AFF_FAIL);
        ccn_seti(ccec_cp_n(cp), ccec_ctx_z(key), 1);
        result=0;
    }
errOut:
    return result;
}
