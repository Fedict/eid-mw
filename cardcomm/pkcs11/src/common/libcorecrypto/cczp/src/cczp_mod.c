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


#include <corecrypto/cczp.h>
#include <corecrypto/cczp_priv.h>

#define CCN_MOD_R_CTR_LIMIT  3

/* Do r = a % d, where recip is the precalculated steady-state reciprocal of d
 r is count cc_units in size, a is 2 * count units, d is count units
 in size and recip is count + 1 units in size. 
 IMPORTANT: Use only if s2n < 2^2s   (see the math section below)
 */
void cczp_mod(cczp_const_t zp, cc_unit *r, const cc_unit *s2n, cc_ws_t ws) {
    cc_size n=cczp_n(zp);
    size_t s = ccn_bitlen(1 + n, cczp_recip(zp));
    cc_assert(s>3); // Sanity check on reciprocal
    s--;

    cc_unit *tmp1,*tmp2;
    cc_unit *tmpd;
    cc_size unitShift_s_minus_1=(s-1) / CCN_UNIT_BITS;
    cc_size unitShift_s_plus_1=(s+1) / CCN_UNIT_BITS;
    cc_assert(ws!=NULL);
    tmp1=ws->start;       // tmp1 is 2*n
    tmp2=&tmp1[2*n+2-unitShift_s_plus_1];    // tmp2 is 2+2*n
    tmpd=&tmp2[2*n+2];    // tmpd is n+1
    cc_assert(tmpd+n+1<=ws->end); // Check that provided workspace is sufficient
    ws->start+=5*(n+1);
    cc_unit *R[2] __attribute__((aligned(16)))={tmp1,tmp2};
    cc_size R_index=1;

    ccn_setn(1 + n, tmpd, n, cczp_prime(zp));
    ccn_shift_right(2*n-unitShift_s_minus_1, tmp1, &s2n[unitShift_s_minus_1], (s - 1) & (CCN_UNIT_BITS-1));
    ccn_mul_ws(1 + n, tmp2, cczp_recip(zp), tmp1, ws);
    ccn_shift_right(2*n+2-unitShift_s_plus_1, tmp1, &tmp2[unitShift_s_plus_1], (s + 1) & (CCN_UNIT_BITS-1));
    ccn_mul_ws(n, tmp2, tmpd, tmp1, ws);
    ccn_sub(2 * n, R[R_index], s2n, tmp2);
    for (unsigned int ct=0; ct < CCN_MOD_R_CTR_LIMIT; ++ct) {
        // R_index moves only if result is positive
        R_index^= 1 ^ ccn_sub(1 + n, R[R_index^1], R[R_index], tmpd);
    }
    cc_assert(ccn_cmp(1 + n, R[R_index],tmpd)<0);
    ccn_set(n, r, R[R_index]);
    ws->start=tmp1;
}
/*
 A little math for CCN_MOD_R_CTR_LIMIT:

 The loop computes:
 q = ((s2n/2^(s+1))*R)/2^(s-1)
 where R = 2^(2s)/d is the reciprocal.

 Then it computes r = (s2n - q*d) which is an approximation of the remainder.

 Because all the divisions are integer division, the remainder needs to be adjusted to provide the exact result:
 It is possible to compute an upper bound of the adjustment by using the “ceil” of each division.
 For simplification, adding 1 to each of the intermediary quotients:

 q’ the exact quotient

 q’< ((s2n/2^(s+1) + 1)*(R+1))/2^(s-1) + 1
 <=> q’  < ( (s2n/2^(s+1)*(R+1) + (R+1) )/2^(s-1) + 1
 <=> q’  < ( (s2n/2^(s+1)*R+s2n/2^(s+1)+(R+1)  )/2^(s-1) + 1
 <=> q’  < ( (s2n/2^(s+1)*R/2^(s-1))+(s2n/2^(s+1)+(R+1))/2^(s-1)  + 1
 <=> q’  < ( q + (s2n/2^(2s) + (R+1)/2^(s-1) + 1)

 By definition of R, R=2^(2s)/d where 2^(s-1) < d < 2^s
 therefore R < 2^(s+1)
 as a consequence  (R+1)/2^(s-1) <= 2
 => q’  < ( q + (s2n/2^(2s) + 3)
 => q’ - q <  (s2n/2^(2s) + 3)

 As long as the input s2n is < 2^2s, at most 3 substractions are needed.

 */



/* Do r = sn % d, where recip is the precalculated steady-state reciprocal of d
 r is count cc_units in size, sn of size up to ns units, d is n units
 in size and recip is n + 1 units in size.
*/

void cczp_modn(cczp_const_t zp, cc_unit *r, cc_size ns, const cc_unit *sn) {
    size_t s = ccn_bitlen(1 + cczp_n(zp), cczp_recip(zp));
    cc_assert(s>3); // Sanity check on reciprocal
    s--;
    cc_size ntmp = (2* cczp_n(zp)) > ns ? (2* cczp_n(zp)) : ns;

    // each loop iteration reduces the number by 2^(s-2)
    //   because the substraction on the loop does tmpn-d*q where q <= 3+tmpn/d
    //   therefore we deal with up to s-2 bits at each iteration (see the math below)
    const size_t loop_iterations=((ccn_bitsof_n(ntmp)-1)/(s-2));


    cc_unit tmp1[2*(ntmp-ccn_nof(s)+1)], tmp2[1+CC_MAX(2+2*cczp_n(zp),ntmp)];
    cc_unit tmpd[(ntmp-ccn_nof(s)+1)],tmprecip[(ntmp-ccn_nof(s)+1)];
    cc_unit tmpn[ntmp];

    if (ntmp>2+2*cczp_n(zp)) {
        ccn_zero(ntmp-(2+2*cczp_n(zp)),&tmp2[2+2*cczp_n(zp)]);
    }
    ccn_setn(ntmp, tmprecip, cczp_n(zp)+1, cczp_recip(zp));
    ccn_setn(ntmp, tmpd, cczp_n(zp), cczp_prime(zp));
    ccn_setn(ntmp, tmpn, ns, sn);
    for (size_t k=0; k<loop_iterations; k++) {
        /*
         q = (sn / 2^(s-1) * (2^(2s)/d)) / 2^(s+1) is an approximation of the quotient tmpn/d
         Error is tmpn/d - q <= 3. We adjust after the loop.
         */
        ccn_shift_right_multi(ntmp, tmp2, tmpn, s - 1); //  sn / 2^(s-1)
        ccn_mul(ntmp-ccn_nof(s)+1, tmp1, tmprecip, tmp2); //  sn / 2^(s-1) * (2^(2s)/d)
        ccn_shift_right_multi(ntmp+1, tmp2, tmp1, s + 1);  //  (sn / 2^(s-1) * (2^(2s)/d)) / 2^(s+1)
        ccn_mul(ntmp-ccn_nof(s)+1, tmp1, tmpd, tmp2); // (sn / 2^(s-1) * (2^(2s)/d)) / 2^(s+1) * d
        ccn_sub(ntmp, tmpn, tmpn, tmp1);
        ntmp = CC_MAX((2* cczp_n(zp)),ntmp-ccn_nof(s)+1); // Adjust ntmp for performance
    }

    // Adjust the result due to the quotient approximation of the last iteration
    cc_unit *R[2] __attribute__((aligned(16)))={tmp1,tmpn};
    cc_size R_index=1;
    for (unsigned int ct=0; ct < CCN_MOD_R_CTR_LIMIT; ++ct) {
        // R_index moves only if result is positive
        R_index^= 1 ^ ccn_sub(1 + cczp_n(zp), R[R_index^1], R[R_index], tmpd);
    }
    // Done
    cc_assert(ccn_cmp(1 + cczp_n(zp), R[R_index],tmpd)<0);
    ccn_set(cczp_n(zp), r, R[R_index]);
}

/*
 A little math for a good sleep:

 The loop computes:
 q = ((s2n/2^(s+1))*R)/2^(s-1)
 where R = 2^(2s)/d is the reciprocal.

 Then it computes r = (tmpn - q*d) which is an approximation of the remainder.

 Because all the divisions are integer division, the remainder needs to be adjusted to provide the exact result:
 It is possible to compute an upper bound of the adjustment by using the “ceil” of each division.
 For simplification, adding 1 to each of the intermediary quotients:

 q’_i the exact quotient of iteration i
 tmpn < 2^k where initially k=ccn_bitsof_n(ntmp)


 q’_1 < ((2^k/2^(s+1) + 1)*(R+1))/2^(s-1) + 1
 <=> q’  < ( (2^k/2^(s+1)*(R+1) + (R+1) )/2^(s-1) + 1
 <=> q’  < ( (2^k/2^(s+1)*R+2^k/2^(s+1)+(R+1)  )/2^(s-1) + 1
 <=> q’  < ( (2^k/2^(s+1)*R/2^(s-1))+(2^k/2^(s+1)+(R+1))/2^(s-1)  + 1
 <=> q’  < ( q + (2^k/2^(2s) + (R+1)/2^(s-1) + 1)

 By definition of R, R=2^(2s)/d where 2^(s-1) < d < 2^s
 therefore R < 2^(s+1)
 as a consequence  (R+1)/2^(s-1) <= 2
 => q’  < ( q + (2^k/2^(2s) + 3)
 => q’ - q < (2^k/2^(2s) + 3)

 therefore 
 (tmpn - q*p) < (tmpn - (q'-3-(2^k/2^(2s)))*p)
 (tmpn - q*p) < (tmpn - q'*p) + (3+2^k/2^2s)*p

 by definition of the division, (tmpn - q'*p) = r < p
  (tmpn - q*p) < r + (3+2^k/2^2s)*p

 with
    3+2^k/2^2s < 2^(k-2s+2)    (because 2^x+3 < 4*2^x <=> 3<2^x(4-1) <=> 1<2^x)

 it becomes
 (tmpn - q*p) < r + (2^(k-2s+2))*p  and using p<2^s

 (tmpn - q*p) < r + (2^(k-(s+2))

 therefore the algorithm needs at most ceiling(k/(s+2)) - 1 iteration to converge
 the math is slightly different for the last iteration since k<2s.
 For this case, please refer to the math of the function cczp_mod.
 */




