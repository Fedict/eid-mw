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


#include <corecrypto/ccec_priv.h>
#include <corecrypto/cc_macros.h>
#include <corecrypto/cc_debug.h>

/*
 Compute an ECC shared secret between private_key and public_key. Return
 the result in computed_shared_secret.
 It conforms to EC-DH from ANSI X9.63 and NIST SP800-56A, section 5.7.1.2)
 and the length of the result in bytes in computed_key_len. Return 0 iff
 successful.
 Note: The shared secret MUST be transformed with a KDF function or at
 least Hash (SHA-256) before being used.
 It shall not be used directly as a key.

 RNG may be used internally to randomize computation and prevent attacks such as timing and
 cache attacks */
int ccecdh_compute_shared_secret(ccec_full_ctx_t private_key,
                                 ccec_pub_ctx_t public_key,
                                 size_t *computed_shared_secret_len, uint8_t *computed_shared_secret,
                                 struct ccrng_state *masking_rng) {
    ccec_const_cp_t cp = private_key.hdr->cp;
    ccec_point_decl_cp(cp, r);
    ccec_point_decl_cp(cp, Q);
    int result = -1;

    /* Sanity check the prime */
    cc_require((private_key.hdr->cp.zp == public_key.hdr->cp.zp),errOut);

    /* Sanity check the input key */
    cc_require((ccec_validate_pub_and_projectify(Q,public_key,masking_rng)==0),errOut);

    /* Actual computation. Assume curve has cofactor = 1 */
    cc_require((ccec_mult(cp, r, ccec_ctx_k(private_key), Q,masking_rng) == 0),errOut);

    /* Check that result point is on the curve */
    cc_require(ccec_is_point_projective(cp,r),errOut);
    cc_require((ccec_affinify_x_only(cp, ccec_point_x(r, cp), r) == 0),errOut);

    /* Good so far: finalize output of result */
    size_t p_len = ccn_write_uint_size(ccec_cp_n(cp), ccec_cp_p(cp));
    if (*computed_shared_secret_len < p_len) {
        *computed_shared_secret_len = -1;
        goto errOut;
    }
    *computed_shared_secret_len = p_len;
    ccn_write_uint_padded(ccec_cp_n(cp), ccec_point_x(r, cp), p_len, computed_shared_secret);

    result = 0;

errOut:
    ccec_point_clear_cp(cp, r);
    ccec_point_clear_cp(cp, Q);
    return result;
}
