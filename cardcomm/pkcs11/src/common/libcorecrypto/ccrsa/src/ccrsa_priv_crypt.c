/*
 * Copyright (c) 2011,2012,2014,2015 Apple Inc. All rights reserved.
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


#include <corecrypto/ccrsa_priv.h>

/*
 * power/mod using Chinese Remainder Theorem.
 *
 * result becomes base^d (mod m).
 *
 * ...but we don't know or need d or m. What we have are
 *
 * p, q such that m = p*q
 * reciprocals of p, q
 * dp = d mod (p-1)
 * dq = d mod (q-1)
 * qinv = q^(-1) mod p
 *
 * Plus one scratch giant, initialized by caller with the capacity at
 * least as big as p*q.
 *
 * The base and result arguments can be identical, i.e., an in/out
 * parameter.
 */
static void cczp_crt_power(cc_size n,             /* Size of x and r in units (keysize). */
                           cc_unit *r,            /* OUTPUT */
                           const cc_unit *x,      /* base (plaintext/ciphertext) */
                           cczp_const_t zp,       /* zp * zq = public modulus */
                           const cc_unit *dp,     /* d mod (p-1)   cczp_n(zp) sized */
                           const cc_unit *qinv,   /* q^(-1) mod p  cczp_n(zp) sized */
                           cczp_const_t zq,       /* zp * zq = public modulus */
                           const cc_unit *dq) {   /* d mod (q-1)   cczp_n(zq) sized */
    cc_unit sp[cczp_n(zp)+1];
    cc_unit sq[cczp_n(zq)];
    cc_unit tmp[2 * cczp_n(zp)];

    cc_assert(cczp_n(zp) >= cczp_n(zq));
    ccn_zero((2 * cczp_n(zp)) - n, tmp + n);     // padd x to 2 * cczp_n(zp)
    ccn_set(n, tmp, x);

    // Sp
    cczp_modn(zp, sp, n, tmp);
    cczp_power(zp, sp, sp, dp);                 // sp = (x ^ dp) mod p

    // Sq
    cczp_modn(zq, sq, n, tmp);
    cczp_power(zq, sq, sq, dq);                 // sq = (x ^ dq) mod q

    // Garner recombination (requires p>=q)
    sp[cczp_n(zp)]=ccn_add(cczp_n(zp), sp, sp, cczp_prime(zp));  // sp + p
    ccn_setn(cczp_n(zp)+1,tmp,cczp_n(zq),sq);       // handle cczp_n(zq) < cczp_n(zp)
    ccn_sub(cczp_n(zp)+1, tmp, sp, tmp);            // r = (sp + p) - sq
    cczp_modn(zp,r,cczp_n(zp)+1,tmp);               // r mod p
    cczp_mul(zp, r, r, qinv);                       // r = (r * qinv) mod p
    ccn_setn(cczp_n(zp), sp, cczp_n(zq), cczp_prime(zq));  // sp = q
    cc_assert(ccn_cmp(cczp_n(zp),cczp_prime(zp),sp)>=0); // Assert (p>=q)
    ccn_mul(cczp_n(zp), tmp, r, sp);                       // tmp = r * q
    cc_unit carry = ccn_add(cczp_n(zq), r, tmp, sq);       // r = tmp + sq
    ccn_add1(n - cczp_n(zq), r + cczp_n(zq), tmp + cczp_n(zq), carry);

    // Clear working buffers
	ccn_clear(cczp_n(zp), sp);
	ccn_clear(cczp_n(zq), sq);
	ccn_clear(2 * cczp_n(zp), tmp);
}

int ccrsa_priv_crypt(ccrsa_priv_ctx_t privk, cc_unit *out, const cc_unit *in) {
    cc_size n = ccn_nof(
                        ccn_bitlen(cczp_n(ccrsa_ctx_private_zp(privk)), cczp_prime(ccrsa_ctx_private_zp(privk)))
                        + ccn_bitlen(cczp_n(ccrsa_ctx_private_zq(privk)), cczp_prime(ccrsa_ctx_private_zq(privk))));

    // Reject dp=1 or dq=1 as a valid key because e=1 is not acceptable.
    // by definition dp*e=1 mod (p-1) and dq*e=1 mod (p-1)
    if ((ccn_bitlen(cczp_n(ccrsa_ctx_private_zp(privk)), ccrsa_ctx_private_dp(privk))<=1)
        || (ccn_bitlen(cczp_n(ccrsa_ctx_private_zq(privk)), ccrsa_ctx_private_dq(privk))<=1)) {
        return CCRSA_KEY_ERROR;
    }

    // Proceed
    cczp_crt_power(n, out, in, ccrsa_ctx_private_zp(privk), ccrsa_ctx_private_dp(privk),
                   ccrsa_ctx_private_qinv(privk), ccrsa_ctx_private_zq(privk),
                   ccrsa_ctx_private_dq(privk));
    return 0;
}
