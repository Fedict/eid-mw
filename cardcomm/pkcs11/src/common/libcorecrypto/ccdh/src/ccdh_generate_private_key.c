/*
 * Copyright (c) 2015 Apple Inc. All rights reserved.
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


#include <corecrypto/ccdh_priv.h>
#include <corecrypto/cc_debug.h>
#include <corecrypto/cc_macros.h>

#define MAX_RETRY 100

int ccdh_generate_private_key(ccdh_const_gp_t gp, cc_unit *x, struct ccrng_state *rng)
{
    int result=CCDH_ERROR_DEFAULT;
    int cmp_result=1;
    size_t i=0;
    cc_size l;
    cc_size rand_bitlen;
    cc_unit upper_bound[ccdh_gp_n(gp)];
    ccn_zero(ccdh_gp_n(gp), x);
    ccn_zero(ccdh_gp_n(gp), upper_bound);

    l=ccdh_gp_l(gp);

    // Pre-requisite, per PKCS #3 (section 6)
    cc_require_action((l<=ccn_bitlen(ccdh_gp_n(gp),cczp_prime(gp.zp))),
                      errOut,result = CCDH_INVALID_DOMAIN_PARAMETER);

    // Generate the random private key x
    // (following pkcs#3 section 7.1 when order is not present)
    // Three cases
    // a) order q is available
    //    0 < x < q-1
    // b) "l" is set, 2^(l-1) <= x < 2^l
    //      upper bound is implicitely met
    //      lower bound is met by setting MS bit
    // c) "l"==0, 0 < x < p-1

    // "l" <= bitlengh(order)+64 is a security risk due to the biais it causes
    // Using the order to generate the key is more secure and efficient
    // and therefore takes precedence.

    if (ccdh_gp_order_bitlen(gp)>0)
    {
        // Upper bound: 0 < x <=q-2
        ccn_sub1(ccdh_gp_n(gp), upper_bound, ccdh_gp_order(gp), 2);
        rand_bitlen=ccdh_gp_order_bitlen(gp);
    }
    else if (l>=1) {
        // Bounds are implicitely met
        cc_require(((result = ccn_random_bits(l, x, rng)) == 0),errOut);
        ccn_set_bit(x, l-1, 1); // 2^(l-1)
        cmp_result=0; // Not entering the loop below
    }
    else {
        // Upper bound: 0 < x <=p-2
        ccn_sub1(ccdh_gp_n(gp), upper_bound, cczp_prime(gp.zp), 2);
        rand_bitlen=ccn_bitlen(ccdh_gp_n(gp),cczp_prime(gp.zp));
    }

    // Try until finding an integer in the correct range
    // This avoids biais in key generation that occurs when using mod.
    for (i = 0; i < MAX_RETRY && cmp_result>0; i++)
    {
        /* Random bits */
        cc_require(((result = ccn_random_bits(rand_bitlen, x, rng)) == 0),errOut);

        /* Check bound */
        cmp_result = ccn_cmp(ccdh_gp_n(gp), x, upper_bound);  // -1, 0  ok
        cmp_result += 2*ccn_is_zero(ccdh_gp_n(gp),x);   // 0 ok
    }

    // Check that an integer has been found.
    if (i >= MAX_RETRY)
    {
        result = CCDH_GENERATE_KEY_TOO_MANY_TRIES;
    }
    else
    {
        result = 0;
    }
errOut:
    return result;
}

