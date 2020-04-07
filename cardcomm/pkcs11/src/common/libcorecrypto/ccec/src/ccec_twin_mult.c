/*
 * Copyright (c) 2010,2011,2014,2015 Apple Inc. All rights reserved.
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

/* Define to 1 to save some space (~115 bytes on i386 and ~100 bytes on arm)
   in ccec_twin_mult(), the new code is also slightly faster.  The only real
   reason the old code is kept around is as documention.  */
#define USE_BITMASK  1

/* Define to 1 to use a table for f_2_2_11 instead of a function.  Saves
   16 bytes on arm (saves 48 bytes code, then adds 32 bytes const data for
   the table). */
#define USE_F_TABLE    1

#if USE_F_TABLE
static const uint8_t f_2_2_11_t[32] = {
    12, 12, 12, 12,                  // [0, 4>
    14, 14, 14, 14, 14, 14, 14, 14,  // [4, 12>
    12, 12,                          // [12, 14>
    10, 10, 10, 10,                  // [14, 18>
    9, 9, 9, 9,                      // [18, 22>
    11, 11,                          // [22, 24>
    12, 12, 12, 12, 12, 12, 12, 12   // [24, 32>
};
#define f_2_2_11(T) ((__typeof__ (T))f_2_2_11_t[(T)])
#else
/* Routine 2.2.11 F (t): an auxilliary function for ccec_twin_mult */
static cc_unit f_2_2_11(cc_unit t) {
    if (18 <= t && t < 22) {
        return 9;
    } else if (14 <= t && t < 18) {
        return 10;
    } else if (22 <= t && t < 24) {
        return 11;
    } else if (4 <= t && t < 12) {
        return 14;
    } else {
        return 12;
    }
}
#endif

static void twin_mult_normalize(ccec_const_cp_t cp, ccec_projective_point_t r, ccec_const_projective_point_t s, const cc_unit *e, const cc_unit *b, const cc_unit *cd) {
    cc_unit lambda[ccec_cp_n(cp)], t[ccec_cp_n(cp)];
    cczp_mul(cp.zp, t, b, cd);                                  // bcd = b * cd
    cczp_mul(cp.zp, lambda, e, t);                              // lambda = a^-1 = (abcd)^-1 * bcd
    cczp_sqr(cp.zp, t, lambda);                                 // t = lambda^2
    cczp_mul(cp.zp, ccec_point_x(r, cp), t, ccec_const_point_x(s, cp));   // rx = t * sx
    cczp_mul(cp.zp, t, t, lambda);                              // t = lambda^3
    cczp_mul(cp.zp, ccec_point_y(r, cp), t, ccec_const_point_y(s, cp));   // ry = t * sy
    // Don't touch z here since it's still used by our caller.
}

// s and t must be different
void ccec_twin_mult(ccec_const_cp_t cp, ccec_projective_point_t r, const cc_unit *d0, ccec_const_projective_point_t s, const cc_unit *d1, ccec_const_projective_point_t t) {
    ccec_point_decl_cp(cp, spt);
    ccec_point_decl_cp(cp, smt);
    ccec_full_add(cp, spt, s, t);
    ccec_full_sub(cp, smt, s, t);

    /* Normalize s, t, spt and smt (have them all in affine format) */
    ccec_point_decl_cp(cp, ns);
    ccec_point_decl_cp(cp, nt);
    cc_unit st[ccec_cp_n(cp)], sptsmt[ccec_cp_n(cp)], stsptsmt_1[ccec_cp_n(cp)];
    cczp_mul(cp.zp, st, ccec_const_point_z(s, cp), ccec_const_point_z(t, cp));
    cczp_mul(cp.zp, sptsmt, ccec_const_point_z(spt, cp), ccec_const_point_z(smt, cp));
    cczp_mul(cp.zp, stsptsmt_1, st, sptsmt);
    cczp_mod_inv(cp.zp, stsptsmt_1, stsptsmt_1); // Inverse: (z(s)*z(t)*z(spt)*z(smt))^-1 mod p

    if (CCEC_ZP_IS_MONTGOMERY(cp)) {
        // In Montgomery, we just computed  (R*z(s)*z(t)*z(spt)*z(smt))^-1 mod p
        // We need to have R*(z(s)*z(t)*z(spt)*z(smt))^-1 mod p
        cczp_convert_to_montgomery(cp.zp, stsptsmt_1, stsptsmt_1); // (z(s)*z(t)*z(spt)*z(smt))^-1 mod p
        cczp_convert_to_montgomery(cp.zp, stsptsmt_1, stsptsmt_1); // R*(z(s)*z(t)*z(spt)*z(smt))^-1 mod p
    }
    twin_mult_normalize(cp, ns,  s,   stsptsmt_1, ccec_const_point_z(t, cp), sptsmt);
    twin_mult_normalize(cp, nt,  t,   stsptsmt_1, ccec_const_point_z(s, cp), sptsmt);
    twin_mult_normalize(cp, spt, spt, stsptsmt_1, st,   ccec_const_point_z(smt, cp));
    twin_mult_normalize(cp, smt, smt, stsptsmt_1, st,   ccec_const_point_z(spt, cp));
    
    if (CCEC_ZP_IS_MONTGOMERY(cp)) {
        ccn_seti(ccec_cp_n(cp),stsptsmt_1,1);
        cczp_convert_to_montgomery(cp.zp, ccec_point_z(ns, cp), stsptsmt_1); // 1 in montgomery form
        ccn_set(ccec_cp_n(cp), ccec_point_z(nt, cp),  ccec_point_z(ns, cp));
        ccn_set(ccec_cp_n(cp), ccec_point_z(spt, cp), ccec_point_z(ns, cp));
        ccn_set(ccec_cp_n(cp), ccec_point_z(smt, cp), ccec_point_z(ns, cp));
    }
    else {
        ccn_seti(ccec_cp_n(cp), ccec_point_z(ns, cp), 1);
        ccn_seti(ccec_cp_n(cp), ccec_point_z(nt, cp), 1);
        ccn_seti(ccec_cp_n(cp), ccec_point_z(spt, cp), 1);
        ccn_seti(ccec_cp_n(cp), ccec_point_z(smt, cp), 1);
    }

    const cc_unit *e[2] = { d0, d1 };
    unsigned long m0 = ccn_bitlen(ccec_cp_n(cp), d0);
    unsigned long m1 = ccn_bitlen(ccec_cp_n(cp), d1);
    unsigned long m = m0 > m1 ? m0 : m1;

#if USE_BITMASK
    cc_unit c[2];
    for (unsigned long i = 0; i < 2 ; ++i) {
        c[i] = ((ccn_bit(e[i], m - 1) << 3) +
                (ccn_bit(e[i], m - 2) << 2) +
                (ccn_bit(e[i], m - 3) << 1) +
                (ccn_bit(e[i], m - 4) << 0));
    }
#else
    bool c[2][6];
    for (unsigned long i = 0; i < 2 ; ++i) {
        c[i][0] = 0;
        c[i][1] = 0;
        c[i][2] = ccn_bit(e[i], m - 1);
        c[i][3] = ccn_bit(e[i], m - 2);
        c[i][4] = ccn_bit(e[i], m - 3);
        c[i][5] = ccn_bit(e[i], m - 4);
    }
#endif

    ccn_seti(ccec_cp_n(cp), ccec_point_x(r, cp), 1);
    ccn_seti(ccec_cp_n(cp), ccec_point_y(r, cp), 1);
    ccn_seti(ccec_cp_n(cp), ccec_point_z(r, cp), 0);

    for (unsigned long k = m + 1; k--;) {
        cc_unit h[2];
        for (unsigned long i = 0; i < 2 ; ++i) {
#if USE_BITMASK
            h[i] = c[i] & 0x1f;
            if (c[i] & 0x20) {
                h[i] = 31 - h[i];
            }
#else
            h[i] = 16 * c[i][1] + 8 * c[i][2] + 4 * c[i][3] + 2 * c[i][4] + c[i][5];
            if (c[i][0] == 1) {
                h[i] = 31 - h[i];
            }
#endif
        }
        int u[2];
        for (unsigned long i = 0; i < 2 ; ++i) {
#if USE_BITMASK
            cc_unit cmask = (k >= 5) ? ccn_bit(e[i], k - 5) : 0;
            if (h[i] < f_2_2_11(h[1-i])) {
                u[i] = 0;
            } else {
                cmask += 0x20;
                //u[i] = 1 - ((c[i] & 0x20) >> 4); /* Branchless */
                u[i] = c[i] & 0x20 ? -1 : 1;
            }
            c[i] = (c[i] << 1) ^ cmask;
#else
            if (h[i] < f_2_2_11(h[1-i])) {
                u[i] = 0;
                c[i][0] = c[i][1];
            } else if (c[i][0]) {
                u[i] = -1;
                c[i][0] = 1 ^ c[i][1];
            } else {
                u[i] = 1;
                c[i][0] = 1 ^ c[i][1];
            }

            c[i][1] = c[i][2];
            c[i][2] = c[i][3];
            c[i][3] = c[i][4];
            c[i][4] = c[i][5];
            c[i][5] = (k >= 5) ? ccn_bit(e[i], k - 5) : 0;
#endif
        }

        ccec_double(cp, r, r);

        if (u[0] == -1 && u[1] == -1) ccec_full_sub_normalized(cp, r, r, spt);
        if (u[0] == -1 && u[1] ==  0) ccec_full_sub_normalized(cp, r, r, ns);
        if (u[0] == -1 && u[1] ==  1) ccec_full_sub_normalized(cp, r, r, smt);
        if (u[0] ==  0 && u[1] == -1) ccec_full_sub_normalized(cp, r, r, nt);
        if (u[0] ==  0 && u[1] ==  1) ccec_full_add_normalized(cp, r, r, nt);
        if (u[0] ==  1 && u[1] == -1) ccec_full_add_normalized(cp, r, r, smt);
        if (u[0] ==  1 && u[1] ==  0) ccec_full_add_normalized(cp, r, r, ns);
        if (u[0] ==  1 && u[1] ==  1) ccec_full_add_normalized(cp, r, r, spt);
    }
    cc_assert(ccec_is_point_projective(cp, r));
}
