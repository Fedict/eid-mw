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
#include <corecrypto/ccn.h>
#include <corecrypto/ccn_priv.h>
#include <corecrypto/cc_debug.h>

#define CC_DEBUG_MAKERECIP (CORECRYPTO_DEBUG && 0)

//------------------------------------------------------------------------------
// Compile time logic to choose algorithm to use:
//
// CC_MAKE_RECIP_MEMORY_SMALL can be set in cc_config.h for minimal memory usage
//   if small is not select then it will run faster.
//
// Algorithm also can be forced to one or the other by setting either
//      CC_MAKE_RECIP_NEWTONRAPHSON_OPTION
//      CC_MAKE_RECIP_SHIFT_SUB_OPTION
// but it is recommended to let this file make the choice.
//------------------------------------------------------------------------------
// Debug mode, enable both algorithms
#if CC_DEBUG_MAKERECIP
#define CC_MAKE_RECIP_SHIFT_SUB_OPTION     1
#define CC_MAKE_RECIP_NEWTONRAPHSON_OPTION 1
#endif

#if defined(CC_MAKE_RECIP_MEMORY_SMALL) && CC_MAKE_RECIP_MEMORY_SMALL
#define CC_MAKE_RECIP_SHIFT_SUB_OPTION     1
#define CC_MAKE_RECIP_NEWTONRAPHSON_OPTION 0
#endif


// By default, vote for the fastest algorithm
#ifndef CC_MAKE_RECIP_SHIFT_SUB_OPTION
// If assembly for substraction is not available, shift-sub is slower
#define CC_MAKE_RECIP_SHIFT_SUB_OPTION      CCN_SUB_ASM
#endif

#ifndef CC_MAKE_RECIP_NEWTONRAPHSON_OPTION
#define CC_MAKE_RECIP_NEWTONRAPHSON_OPTION  1
#endif

#if (!CC_MAKE_RECIP_SHIFT_SUB_OPTION && !CC_MAKE_RECIP_NEWTONRAPHSON_OPTION)
#error No algorithm define for reciprocal computation
#endif
//------------------------------------------------------------------------------
// Algorithm prototypes
//------------------------------------------------------------------------------
#if CC_MAKE_RECIP_NEWTONRAPHSON_OPTION
static void ccn_make_recip_newtonraphson(cc_size count, cc_unit *recip, const cc_unit *d);
#endif

#if CC_MAKE_RECIP_SHIFT_SUB_OPTION
static void ccn_make_recip_shift_sub(cc_size count, cc_unit *recip, const cc_unit *d);
#endif
/* Calculate the reciprocal r of a demonimator d.
 r becomes the steady-state reciprocal
 2^(2b)/d, where b = bit-length of d-1.
 Asymptotically better than Newton Raphson
    => better above 1280 on 64bit architectures
 Always better for 32bit architectures
*/
#if CC_MAKE_RECIP_SHIFT_SUB_OPTION
static void ccn_make_recip_shift_sub(cc_size count, cc_unit *recip, const cc_unit *d)
{
    size_t b = ccn_bitlen(count, d);
    cc_size n = (b+1+(CCN_UNIT_BITS-1)) / CCN_UNIT_BITS;
    
    /* Use the following property
    2^b     = d.Qi + r
    2^(b+1) = d.2.Qi + 2r  // if 2r > d Qi+1=2.Qi + 1 otherwise Qi+1=2.Qi
    */
    CC_DECL_WORKSPACE(ws,3*n);
    cc_unit *tmp_d=ws->start;        // n units
    cc_unit *work=ws->start+n;      // 2*n units
    ws->start+=3*n;
    cc_unit *R[2] __attribute__((aligned(16)))={&work[0],&work[n]};
    unsigned int R_index=0;
    cc_unit R_sign=0;
    long int i;
    if (count>n)
    {
        ccn_set(n,tmp_d,d);
    }
    else
    {
        ccn_setn(n,tmp_d,count,d);
    }
    ccn_zero(n,R[R_index]);
    ccn_set_bit(R[R_index], b, 1);
    ccn_sub(n, R[R_index], R[R_index], tmp_d); // r0 = 2^b - d
    
    ccn_zero(count+1,recip);
    ccn_set_bit(recip, b, 1); // 2^b/d = 1 by definition of b
    for (i=(b-1);i>=0;i-=1) {
        ccn_add(n, R[R_index], R[R_index], R[R_index]);   // 2*R, use add since faster than shift 1bit
        R_sign=1^ccn_sub(n,R[R_index^1],R[R_index],tmp_d);// 2*R - d
        R_index^=(R_sign);                                // We keep the positive value
        ccn_set_bit(recip, i, R_sign);      // Set the bit to one if we had to substract
    }
    ws->start-=3*n;
    CC_CLEAR_AND_FREE_WORKSPACE(ws);
}
#endif // CC_MAKE_RECIP_SHIFT_SUB_OPTION

/* Calculate the reciprocal r of a demonimator d.
 r becomes the steady-state reciprocal
 2^(2b)/d, where b = bit-length of d-1. 
 Better on 64bit architecture, especially below 1280bit */
#if CC_MAKE_RECIP_NEWTONRAPHSON_OPTION
static void ccn_make_recip_newtonraphson(cc_size count, cc_unit *recip, const cc_unit *d)
{
    cc_size b;
    if (ccn_is_zero(count, d)) {
		return;
	}
    
    b = ccn_bitlen(count, d);
    cc_size shift_units = b / CCN_UNIT_BITS;
    cc_size shift_bits = b & (CCN_UNIT_BITS - 1);
    cc_size n = (b+1+CCN_UNIT_BITS) / CCN_UNIT_BITS;

    // Working buffers
    CC_DECL_WORKSPACE(ws,7*n-shift_units+(CC_MAX(CCN_MUL_WS_WORKSPACE_N(n),CCN_SQR_WS_WORKSPACE_N(n))));
    cc_unit *tmp2=ws->start;                // 2n units - shift_units
    cc_unit *tmp1=tmp2 + 2*n - shift_units; // 2n units
    cc_unit *tmpd=tmp1 + 2*n; // 2n units
    cc_unit *tmpr=tmpd + 2*n; // n units
    ws->start=tmpr+n;
    
    // Locals
    if (count<n)
    {
        ccn_zero(2*n-count, tmpd + count);
        ccn_set(count, tmpd, d);
    }
    else
    {
        ccn_zero(n, tmpd + n);
        ccn_set(n, tmpd, d);
    }
    ccn_zero(n, tmpr);
    ccn_set_bit(tmpr, b+1, 1);
    ccn_sub(n, tmpr, tmpr, tmpd); // Set r as 2^(b+1)-d to skip first iteration
    
    // Working additional pointer to save on shift operations
    cc_unit *tmp1_shifted,*tmp2_shifted;
    tmp1_shifted=tmp1+shift_units; // Since tmp1 is at n+(n-shift_units), tmp2 has 2*n units available to tmp1_shifted
    tmp2_shifted=tmp2+shift_units;
    
#if CC_DEBUG_MAKERECIP
    cc_printf("cc_d[%lu] = ", b);
    ccn_print(count, d);
    cc_printf("\n");
#endif
    // First loop: quadratic convergence toward the quotient
    // Newton–Raphson division
    for (;;) {
#if CC_DEBUG_MAKERECIP
        cc_printf("cc_r1 = ");
        ccn_print(n, tmpr);
        cc_printf("\n");
#endif
        ccn_sqr_ws(n, tmp1, tmpr, ws);                     // t1 = r^2
        if (shift_bits!=0) {                        // t1 = t1/2^b = r^2/2^b
            ccn_shift_right(2*n-shift_units, tmp1_shifted, tmp1_shifted, shift_bits);
        }
        ccn_mul_ws(n, tmp2, tmpd, tmp1_shifted,ws);       // t2 = t1 * d = r^2/2^b * d
        if (shift_bits!=0) {                        // t2 = t2/2^b = r^2/2^b * d/2^b
            ccn_shift_right(2*n-shift_units, tmp2_shifted, tmp2_shifted, shift_bits);
        }
        cc_unit carry=ccn_sub(n, tmp2_shifted, tmp2_shifted,tmpr);       // t2 - r = r^2/2^b * d/2^b - r
        ccn_sub(n, tmpr, tmpr, tmp2_shifted);        // r' = 2r
        if (!carry) { //  check r' <= r equivalent to r^2/2^b * d/2^b - t > 0
            break;   // if r' <= r => done
        }
	}
    
    // Second loop, find the exact quotient
    ccn_mul_ws(n, tmp2, tmpr, tmpd, ws);
    ccn_seti(n, tmp1_shifted, 1);
	while (ccn_bitlen(2 * n, tmp2) > 2 * b) {
#if CC_DEBUG_MAKERECIP
        cc_printf("cc_r2 = ");
        ccn_print(n, tmpr);
        cc_printf("\n");
#endif
        ccn_sub(n, tmpr, tmpr, tmp1_shifted);
        ccn_sub(2 * n, tmp2, tmp2, tmpd);
	}
    ccn_setn(1 + count, recip, n, tmpr);
#if CC_DEBUG_MAKERECIP
    cc_printf("cc_r3 = ");
    ccn_print(1 + count, recip);
    cc_printf("\n");
#endif
    ws->start=tmp2;
    // Prime may be secret, clear intermediaries
    CC_CLEAR_AND_FREE_WORKSPACE(ws);
}
#endif // CC_MAKE_RECIP_NEWTONRAPHSON_OPTION


void cczp_init(cczp_t zp) {
    zp.zp->mod_prime = cczp_mod;
    zp.zp->options = 0;
#if CC_DEBUG_MAKERECIP
    ccn_make_recip_newtonraphson(cczp_n(zp), CCZP_RECIP(zp), cczp_prime(zp));
    cc_printf("Newton-Raphson = ");
    ccn_print(1 + cczp_n(zp), CCZP_RECIP(zp));
    cc_printf("\n");
    cc_unit recip[1 + cczp_n(zp)];
    ccn_make_recip_shift_sub(cczp_n(zp), recip, cczp_prime(zp));
    cc_printf("Shift sub = ");
    ccn_print(1 + cczp_n(zp), recip);
    cc_printf("\n");
    assert(ccn_cmp(cczp_n(zp)+1,CCZP_RECIP(zp),recip)==0);
#else

//------------------------------------------------------------------------------
// Runtime logic to choose algorithm to use
//------------------------------------------------------------------------------
#if (CC_MAKE_RECIP_SHIFT_SUB_OPTION && CC_MAKE_RECIP_NEWTONRAPHSON_OPTION)
#if defined(__i386__) || defined (__x86_64__)
    // On Intel, faster of 64bit after a certain size
    // Always faster for 32bit
    if ((CCN_UNIT_SIZE<=4) || (cczp_n(zp)>30))
#elif defined(__arm__) || defined(__arm64__)
    // On ARM, faster of 64bit after a certain size
    // slower on 32bit
    if ((CCN_UNIT_SIZE>4) && (cczp_n(zp)>20))
#endif
#endif // (CC_MAKE_RECIP_SHIFT_SUB_OPTION && CC_MAKE_RECIP_NEWTONRAPHSON_OPTION)
#if CC_MAKE_RECIP_SHIFT_SUB_OPTION
    {   // Asymptotically better for 64bit architecture
        ccn_make_recip_shift_sub(cczp_n(zp), CCZP_RECIP(zp), cczp_prime(zp));
    }
#endif // CC_MAKE_RECIP_SHIFT_SUB_OPTION
#if (CC_MAKE_RECIP_SHIFT_SUB_OPTION && CC_MAKE_RECIP_NEWTONRAPHSON_OPTION)
    else
#endif // CC_MAKE_RECIP_SHIFT_SUB_OPTION && CC_MAKE_RECIP_NEWTONRAPHSON_OPTION
#if CC_MAKE_RECIP_NEWTONRAPHSON_OPTION
    {
        ccn_make_recip_newtonraphson(cczp_n(zp), CCZP_RECIP(zp), cczp_prime(zp));
    }
#endif // CC_MAKE_RECIP_NEWTONRAPHSON_OPTION

#endif // CC_DEBUG_MAKERECIP
}
