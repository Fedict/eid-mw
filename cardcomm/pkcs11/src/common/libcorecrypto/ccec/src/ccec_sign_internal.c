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

int ccec_sign_internal(ccec_full_ctx_t key, size_t digest_len, const uint8_t *digest,
              cc_unit *r, cc_unit *s, struct ccrng_state *rng) {
    ccec_const_cp_t cp = ccec_ctx_cp(key) ;
    cczp_const_t zq = ccec_cp_zq(cp);
    int result;
    cc_size e_n=ccn_nof_size(digest_len);
    cc_unit e[cczp_n(zq)];
    size_t qbitlen=ccec_cp_order_bitlen(cp);
    ccec_full_ctx_decl_cp(cp, tmpkey);

    // Process input hash to represent it has an number
    if (e_n>cczp_n(zq)) {
        /* Case where the hash size is bigger than the curve size
         eg. SHA384 with P-256 */
        cc_unit e_big[e_n];
        if ((result = ccn_read_uint(e_n, e_big, digest_len, digest)) < 0) goto errOut;
        /* Keep the leftmost bits of the hash */
        ccn_shift_right_multi(e_n,e_big,e_big,(digest_len*8-qbitlen));
        ccn_set(cczp_n(zq),e,e_big);
    }
    else if ((result = ccn_read_uint(cczp_n(zq), e, digest_len, digest)) < 0) {
        goto errOut;
    }

    // ECDSA signing core
    for (;;) {
        if ((result = ccec_generate_key_internal_fips(cp, rng, tmpkey)) < 0) {
            goto errOut;
        }
        //ccec_print_full_key("ECDSA_k",tmpkey);
        /* Compute r = pubx mod q */
        if (ccn_cmp(ccec_cp_n(cp), ccec_ctx_x(tmpkey), cczp_prime(zq)) >= 0) {
            ccn_sub(ccec_cp_n(cp), r, ccec_ctx_x(tmpkey), cczp_prime(zq));
        } else {
            ccn_set(ccec_cp_n(cp), r, ccec_ctx_x(tmpkey));
        }

        /* Compute the rest of the signature */
        if (!ccn_is_zero(ccec_cp_n(cp), r)) {
#if CCEC_MASKING
            cc_unit mask[cczp_n(zq)];
            if ((result = ccn_random_bits(qbitlen-1, mask, rng) < 0)) {
                goto errOut;
            }
            ccn_set_bit(mask, qbitlen-2, 1);
            // Mask independently each intermediary variable
            cczp_mul(zq,ccec_ctx_k(tmpkey),ccec_ctx_k(tmpkey),mask); // (k*m)
            cczp_mul(zq,e,e,mask);                                   // (e*m)
            cczp_mul(zq,mask,ccec_ctx_k(key),mask);       // (x*m)

            // instead of computing (e + xr) / k mod q
            // the masked variant computes ((e.m) + (x.m).r) / (k.m).

            /* find s = (e + xr) / k mod q */
            cczp_mul(zq, s, mask, r);                                    // s = xr mod q
#else
            cczp_mul(zq, s, ccec_ctx_k(key), r);                         // s = xr mod q
#endif
            cczp_add(zq, s, e, s);                                       // s = e + xr mod q
            cczp_mod_inv_field(zq, e, ccec_ctx_k(tmpkey));    // k = k^-1 mod q
            cczp_mul(zq, s, e, s);                      // s = (e + xr)k^-1 mod q
            if (!ccn_is_zero(ccec_cp_n(cp), s)) {
                break;
            }
        }
    }

errOut:
    ccn_clear(ccec_cp_n(cp),e);
    ccec_full_ctx_clear_cp(cp, tmpkey);
    return result;
}

