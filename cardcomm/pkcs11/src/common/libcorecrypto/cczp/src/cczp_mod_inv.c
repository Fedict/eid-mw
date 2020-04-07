/*
 * Copyright (c) 2010,2011,2012,2013,2014,2015 Apple Inc. All rights reserved.
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


#include <corecrypto/cc.h>
#include <corecrypto/cczp.h>
#include <corecrypto/cc_debug.h>

#define CCZP_MOD_INV_SELFTEST (CORECRYPTO_DEBUG && 0)

int cczp_mod_inv(cczp_const_short_t zp, cc_unit *r, const cc_unit *a) {
    /* NIST ec modular inverse routine. */
    cc_unit u[cczp_n(zp)], v[cczp_n(zp)], x1[cczp_n(zp)], x2[cczp_n(zp)];
    ccn_set(cczp_n(zp), u, a);
    ccn_set(cczp_n(zp), v, cczp_prime(zp));
    ccn_seti(cczp_n(zp), x1, 1);
    ccn_zero(cczp_n(zp), x2);

    cc_assert(((cczp_prime(zp))[0] & 1)==1); // This implementation supports odd modulus only
    cc_assert(ccn_cmp(cczp_n(zp), cczp_prime(zp), a) > 0);
    if (ccn_is_zero(cczp_n(zp),a)) {
        return -2; // Zero has no inverse
    }
    while (!ccn_is_one(cczp_n(zp), u) && !ccn_is_one(cczp_n(zp), v)) {
        while ((u[0] & 1) == 0) {
            ccn_shift_right(cczp_n(zp), u, u, 1);
            cczp_div2(zp, x1, x1);
        }
        while ((v[0] & 1) == 0) {
            ccn_shift_right(cczp_n(zp), v, v, 1);
            cczp_div2(zp, x2, x2);
        }
        int m = ccn_cmp(cczp_n(zp), u, v);
        if (m > 0) {
            ccn_sub(cczp_n(zp), u, u, v);  /* u >= v so don't need cczp */
            //cczp_sub(zp, u, u, v);
            cczp_sub(zp, x1, x1, x2);
        } else if (m < 0) {
            ccn_sub(cczp_n(zp), v, v, u);  /* v > u so don't need cczp */
            //cczp_sub(zp, v, v, u);
            cczp_sub(zp, x2, x2, x1);
        } else {
            return -1; //ccn_lprint(cczp_n(zp), "u == v = ", u);
        }
    }
#ifdef CCZP_MOD_INV_SELFTEST
    cc_unit a_tmp[cczp_n(zp)];
    ccn_set(cczp_n(zp),a_tmp, a);
#endif
    ccn_set(cczp_n(zp), r, ccn_is_one(cczp_n(zp), u) ? x1 : x2);

    cc_assert(ccn_cmp(cczp_n(zp), r, cczp_prime(zp)) < 0);
#ifdef CCZP_MOD_INV_SELFTEST
    /* r = a^(-1) mod m => r * a mod m = 1. */
    cc_unit self_test[cczp_n(zp)];
    cc_unit testzp[cczp_nof_n(cczp_n(zp))];
    CCZP_N(testzp) = cczp_n(zp);
    ccn_set(cczp_n(zp), CCZP_PRIME(testzp), cczp_prime(zp));
    cczp_init(testzp);
    cczp_mul(testzp, self_test, r, a_tmp);
    if (!ccn_is_one(cczp_n(zp), self_test)) {
        ccn_lprint(cczp_n(zp), "r     =0x", r);
        ccn_lprint(cczp_n(zp), "a     =0x", a);
        ccn_lprint(cczp_n(zp), "m     =0x", cczp_prime(zp));
        ccn_lprint(cczp_n(zp), "r*a mod m=0x", self_test);
        ccn_lprint(cczp_n(zp), "u     =0x", u);
        ccn_lprint(cczp_n(zp), "v     =0x", v);
        return -1;
        //assert(ccn_is_one(cczp_n(zp), self_test));
    }
#endif
    return 0;
}
