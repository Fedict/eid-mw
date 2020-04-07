/*
 * Copyright (c) 2010,2011,2015 Apple Inc. All rights reserved.
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

// Convert point from affine to jacobian projective coordinates
int ccec_projectify(ccec_const_cp_t cp, ccec_projective_point_t r, ccec_const_affine_point_t s,
                     struct ccrng_state *masking_rng) {
    int status;
    cc_assert(r.hdr!=s.hdr); // Points must differ
#if CCEC_DEBUG
    ccec_alprint(cp, "ccec_projectify input", s);
#endif

    // Initialize z
#if CCEC_MASKING
    // Randomized z coordinate
    if (masking_rng) {
        cc_size bitlen=ccec_cp_prime_bitlen(cp);
        status=ccn_random_bits(bitlen-1, ccec_point_z(r, cp), masking_rng);
        ccn_set_bit(ccec_point_z(r, cp), bitlen-2, 1);
        cczp_sqr(cp.zp, ccec_point_x(r, cp), ccec_point_z(r, cp));                       // Z^2 (mtgR^-1)
        cczp_mul(cp.zp, ccec_point_y(r, cp), ccec_point_x(r, cp), ccec_point_z(r, cp));  // Z^3 (mtgR^-2)

        // Set point coordinate from Z, Z^2, Z^3
        cczp_mul(cp.zp, ccec_point_x(r, cp), ccec_point_x(r, cp), ccec_const_point_x(s, cp)); // x.Z^2.mtgR (mtgR^-3)
        cczp_mul(cp.zp, ccec_point_y(r, cp), ccec_point_y(r, cp), ccec_const_point_y(s, cp)); // y.Z^3.mtgR (mtgR^-4)
                                                                                              // Z.mtgR     (mtgR^-1)
        if (CCEC_ZP_IS_MONTGOMERY(cp)) {
            cczp_convert_to_montgomery(cp.zp, ccec_point_x(r, cp), ccec_point_x(r, cp));      // x.Z^2.mtgR (mtgR^-2)
            cczp_convert_to_montgomery(cp.zp, ccec_point_y(r, cp), ccec_point_y(r, cp));      // y.Z^3.mtgR (mtgR^-3)
        }                                                                                     // Z.mtgR     (mtgR^-1)
    } else
#endif
    // Fixed z coordinate
    {
        ccn_seti(ccec_cp_n(cp), ccec_point_z(r, cp),1);
        (void) masking_rng;

        // Set point in the arithmetic representation
        if (CCEC_ZP_IS_MONTGOMERY(cp)) {
            cczp_convert_to_montgomery(cp.zp, ccec_point_x(r, cp), ccec_const_point_x(s, cp));
            cczp_convert_to_montgomery(cp.zp, ccec_point_y(r, cp), ccec_const_point_y(s, cp));
            cczp_convert_to_montgomery(cp.zp, ccec_point_z(r, cp), ccec_point_z(r, cp));
        }
        else {
            ccn_set(ccec_cp_n(cp), ccec_point_x(r, cp), ccec_const_point_x(s, cp));
            ccn_set(ccec_cp_n(cp), ccec_point_y(r, cp), ccec_const_point_y(s, cp));
        }
        status=0;
    }
#if CCEC_DEBUG
    ccec_plprint(cp, "ccec_projectify output", r);
#endif
    return status;
}

