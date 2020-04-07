/*
 * Copyright (c) 2011,2012,2015 Apple Inc. All rights reserved.
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
#include <corecrypto/ccn_priv.h>

#define CCN_MOD_R_CTR_LIMIT  3


/* Compute q = a_2n / cczp_prime(zd) (mod cczp_prime(zd)) . Will write cczp_n(zd)
 units to q and r. Will read 2 * cczp_n(zd) units units from a. If r and a
 are not identical they must not overlap. Before calling this function
 either cczp_init(zp) must have been called or both
 CCZP_MOD_PRIME((cc_unit *)zd) and CCZP_RECIP((cc_unit *)zd) must be
 initialized some other way. 
 Time dependency on the bitlength of the input only.
 */
void cczp_div(cczp_const_t zd, cc_unit *q, cc_unit *r, const cc_unit *a_2n) {
    cc_size n = cczp_n(zd);
    size_t s = ccn_bitlen(1 + n, cczp_recip(zd));
    cc_assert(s>2); // Sanity check on reciprocal.
    // s=0, reciprocal is missing
    // the reciprocal is one bit longer than the prime
    s--;

    // each loop iteration reduces the number by 2^(s-2)
    //   because the substraction on the loop does tmpn-d*q where q <= 3+tmpn/d
    //   therefore we deal with up to s-2 bits at each iteration (see math below)
    size_t loop_iterations;

    if (s>2) {
        loop_iterations=((ccn_bitsof_n(2*n)-1)/(s-2));
    } else { // case where s=2, at least one bit per iteration (very slow case)
        loop_iterations=(ccn_bitsof_n(2*n)-2);
    }

    cc_unit tmp1[2*(2*n-ccn_nof(s)+1)], tmp2[2*n+1];
    cc_unit tmpd[2*n-ccn_nof(s)+1],tmprecip[2*n];
    cc_unit tmpn[2*n];
    cc_unit carry;

    ccn_setn(2*n, tmprecip, n+1, cczp_recip(zd));
    ccn_setn(2*n, tmpd, n, cczp_prime(zd));
    ccn_set(2*n,  tmpn, a_2n);
    ccn_zero(n,q);
    for (size_t k=0; k<loop_iterations; k++) {
        /*
         q = (sn / 2^(s-1) * (2^(2s)/d)) / 2^(s+1) is an approximation of the quotient tmpn/d
         Error is tmpn/d - q <= 3. We adjust after the loop.
         */
        ccn_shift_right_multi(2*n, tmp2, tmpn, s - 1); //  sn / 2^(s-1)
        ccn_mul(2*n-ccn_nof(s)+1, tmp1, tmprecip, tmp2); //  sn / 2^(s-1) * (2^(2s)/d)
        ccn_shift_right_multi(2*n+1, tmp2, tmp1, s + 1);  //  (sn / 2^(s-1) * (2^(2s)/d)) / 2^(s+1)
        ccn_mul(2*n-ccn_nof(s)+1, tmp1, tmpd, tmp2); // (sn / 2^(s-1) * (2^(2s)/d)) / 2^(s+1) * d
        ccn_sub(2*n, tmpn, tmpn, tmp1);
        ccn_add(n,q,q,tmp2); // quotient
    }

    // Adjust the result due to the quotient approximation of the last iteration
    cc_unit *R[2] __attribute__((aligned(16)))={tmp1,tmpn};
    cc_size R_index=1;
    for (unsigned int ct=0; ct < CCN_MOD_R_CTR_LIMIT; ++ct) {
        // R_index moves only if result is positive
        carry=1 ^ ccn_sub(1 + n, R[R_index^1], R[R_index], tmpd);
        R_index^= 1 ^ ccn_sub(1 + n, R[R_index^1], R[R_index], tmpd);
        ccn_add1(n,q,q,carry);
    }
    ccn_set(cczp_n(zd), r, R[R_index]);

    // Done
    cc_assert(ccn_cmp(1 + n, R[R_index],tmpd)<0);
    ccn_set(cczp_n(zd), r, R[R_index]);
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

 q’ the exact quotient of iteration i
 tmpn < 2^k where initially k=ccn_bitsof_n(ntmp)


 q’ < ((2^k/2^(s+1) + 1)*(R+1))/2^(s-1) + 1
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

 therefore the algorithm needs at most ceiling(k/(s+2)) - 1 iterations to converge
 the math is slightly different for the last iteration since k<2s.
 For this case, please refer to the math of the function cczp_mod.
 */



