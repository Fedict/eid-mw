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

/* accept two distinct, non–infinite, projective points S, T and set R equal to the projective point S + T . Routine 2.2.7 performs no checks on its inputs.

   /                         / SyTz^3 - TySz^3 == 0 => ccec_double
  / SxTz^2 - TxSz^2 == 0 => {
 /                           \ SyTz^3 - TySz^3 != 0 => Point at Infinity
{
 \                           / (SyTz^3 - TySz^3)^2
  \ SxTz^2 - TxSz^2 != 0 => {  (SyTz^3 - TySz^3(3(SxTz^2 + TxSz^2)(SxTz^2 - TxSz^2)^2 - 2(SyTz^3 - TySz^3)^2) - (SyTz^3 + TySz^3)(SxTz^2 - TxSz^2)^3) / 2
   \                         \ SxSzTz^3 - TxTzSz^3
 */

#define CCEC_ADD_WORKSPACE_SIZE(n) (7*(n))
static void ccec_add(ccec_const_cp_t cp, ccec_projective_point_t r, ccec_const_projective_point_t s, ccec_const_projective_point_t t, bool is_t_Normalized) {
    cc_unit
        *t1=ccec_point_x(r, cp),
        *t2=ccec_point_y(r, cp),
        *t3=ccec_point_z(r, cp);

    cc_size n=ccec_cp_n(cp);
    CC_DECL_WORKSPACE(ws,CCEC_ADD_WORKSPACE_SIZE(n));
    cc_unit *t4=ws->start;
    cc_unit *t5=ws->start+n;
    cc_unit *t6=ws->start+2*n;
    ws->start+=3*n;
    cc_assert((ws->start)<=ws->end); // Check that provided workspace is sufficient;

    cc_assert(r.hdr!=t.hdr); // the points r and t must not overlap.

    // Cost:
    // Normalized:     3S +  8M + 10add/sub + 1div2
    // Not normalized: 4S + 12M + 10add/sub + 1div2

    if (!is_t_Normalized) {                                    // if Tz != 1
        cczp_sqr_ws(cp.zp, t6, ccec_const_point_z(t, cp), ws);                               // t6 = Tz^2
        cczp_mul_ws(cp.zp, t1, ccec_const_point_x(s, cp), t6, ws);                           // t1 = SxTz^2
        cczp_mul_ws(cp.zp, t6, ccec_const_point_z(t, cp), t6, ws);                           // t6 = Tz^3
        cczp_mul_ws(cp.zp, t2, ccec_const_point_y(s, cp), t6, ws);                           // t2 = SyTz^3
    } else {
        ccn_set(ccec_cp_n(cp), t1, ccec_const_point_x(s, cp));  // t1 = Sx
        ccn_set(ccec_cp_n(cp), t2, ccec_const_point_y(s, cp));  // t2 = Sy
    }

    cczp_sqr_ws(cp.zp, t6, ccec_const_point_z(s, cp), ws);                                   // t6 = Sz^2
    cczp_mul_ws(cp.zp, t4, ccec_const_point_x(t, cp), t6, ws);                               // t4 = TxSz^2
    cczp_mul_ws(cp.zp, t6, ccec_const_point_z(s, cp), t6, ws);                               // t6 = Sz^3
    cczp_mul_ws(cp.zp, t5, ccec_const_point_y(t, cp), t6, ws);                               // t5 = TySz^3
    cczp_sub_ws(cp.zp, t4, t1, t4, ws);                               // t4 = SxTz^2 - TxSz^2
    cczp_sub_ws(cp.zp, t5, t2, t5, ws);                               // t5 = SyTz^3 - TySz^3
    // If t4 ==0 => x_s == x_t, s = +/- t, not supported or result is point at infinite.
    // This will naturally propagate to Z, no need for early abort.

    cczp_add_ws(cp.zp, t1, t1, t1, ws);                               // Or cczp__shift_left(t1, t1, 1, cp)
    cczp_sub_ws(cp.zp, t1, t1, t4, ws);                               // t1 = SxTz^2 + TxSz^2

    cczp_add_ws(cp.zp, t2, t2, t2, ws); // Or cczp__shift_left(t2, t2, 1, cp)
    cczp_sub_ws(cp.zp, t2, t2, t5, ws);                               // t2 = SyTz^3 + TySz^3
    if (!is_t_Normalized) {                                    // if Tz != 1
        cczp_mul_ws(cp.zp, t3, ccec_const_point_z(s, cp), ccec_const_point_z(t, cp), ws);                           // t3 = SzTz
        cczp_mul_ws(cp.zp, t3, t3, t4, ws);                               // t3 = SxSzTz^3 - TxTzSz^3
    } else {
        cczp_mul_ws(cp.zp, t3, ccec_const_point_z(s, cp), t4, ws);        // t3 = SxSzTz^3 - TxTzSz^3
    }
    cczp_sqr_ws(cp.zp, t6, t4, ws);                                   // t6 = (SxTz^2 - TxSz^2)^2
    cczp_mul_ws(cp.zp, t4, t4, t6, ws);                               // t4 = (SxTz^2 - TxSz^2)^3
    cczp_mul_ws(cp.zp, t6, t1, t6, ws);                               // t6 = (SxTz^2 + TxSz^2)(SxTz^2 - TxSz^2)^2
    cczp_sqr_ws(cp.zp, t1, t5, ws);                                   // t1 = (SyTz^3 - TySz^3)^2
    cczp_sub_ws(cp.zp, t1, t1, t6, ws);                               // t1 = (SyTz^3 - TySz^3)^2 - (SxTz^2 + TxSz^2)(SxTz^2 - TxSz^2)^2
    cczp_sub_ws(cp.zp, t6, t6, t1, ws); // TODO Could optimize by calculating 2 t1 here
    cczp_sub_ws(cp.zp, t6, t6, t1, ws);                               // t6 = 3(SxTz^2 + TxSz^2)(SxTz^2 - TxSz^2)^2 - 2(SyTz^3 - TySz^3)^2
    cczp_mul_ws(cp.zp, t5, t5, t6, ws);                               // t5 = SyTz^3 - TySz^3(3(SxTz^2 + TxSz^2)(SxTz^2 - TxSz^2)^2 - 2(SyTz^3 - TySz^3)^2)
    cczp_mul_ws(cp.zp, t4, t2, t4, ws);                               // t4 = (SyTz^3 + TySz^3)(SxTz^2 - TxSz^2)^3
    cczp_sub_ws(cp.zp, t2, t5, t4, ws);                               // t2 = SyTz^3 - TySz^3(3(SxTz^2 + TxSz^2)(SxTz^2 - TxSz^2)^2 - 2(SyTz^3 - TySz^3)^2) - (SyTz^3 + TySz^3)(SxTz^2 - TxSz^2)^3
    // Rx = (SyTz^3 - TySz^3)^2
    cczp_div2(cp.zp, t2, t2);                   // Ry = (SyTz^3 - TySz^3(3(SxTz^2 + TxSz^2)(SxTz^2 - TxSz^2)^2 - 2(SyTz^3 - TySz^3)^2) - (SyTz^3 + TySz^3)(SxTz^2 - TxSz^2)^3) / 2
    // Rz = SxSzTz^3 - TxTzSz^3

    // Result point is {t1,t2,t3}
    ws->start-=3*n;
    CC_FREE_WORKSPACE(ws);
}

void ccec_full_add_normalized(ccec_const_cp_t cp, ccec_projective_point_t r, ccec_const_projective_point_t s, ccec_const_projective_point_t t) {
    // The point T is expected to have Z set to the neutral element
    // 1 or the montgomery constant R if using Montgomery form
    if (ccn_is_zero(ccec_cp_n(cp), ccec_const_point_z(s, cp))) {
        ccn_set(ccec_cp_n(cp), ccec_point_x(r, cp), ccec_const_point_x(t, cp));
        ccn_set(ccec_cp_n(cp), ccec_point_y(r, cp), ccec_const_point_y(t, cp));
        ccn_set(ccec_cp_n(cp), ccec_point_z(r, cp), ccec_const_point_z(t, cp));
        return;
    }
    ccec_add(cp, r, s, t,true);
}

void ccec_full_add(ccec_const_cp_t cp, ccec_projective_point_t r, ccec_const_projective_point_t s, ccec_const_projective_point_t t) {
    if (ccn_is_zero(ccec_cp_n(cp), ccec_const_point_z(s, cp))) {
        ccn_set(ccec_cp_n(cp), ccec_point_x(r, cp), ccec_const_point_x(t, cp));
        ccn_set(ccec_cp_n(cp), ccec_point_y(r, cp), ccec_const_point_y(t, cp));
        ccn_set(ccec_cp_n(cp), ccec_point_z(r, cp), ccec_const_point_z(t, cp));
        return;
    }
    if (ccn_is_zero(ccec_cp_n(cp), ccec_const_point_z(t, cp))) {
        ccn_set(ccec_cp_n(cp), ccec_point_x(r, cp), ccec_const_point_x(s, cp));
        ccn_set(ccec_cp_n(cp), ccec_point_y(r, cp), ccec_const_point_y(s, cp));
        ccn_set(ccec_cp_n(cp), ccec_point_z(r, cp), ccec_const_point_z(s, cp));
        return;
    }
    ccec_add(cp, r, s, t,false);
}


