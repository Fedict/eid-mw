/*
 * Copyright (c) 2010,2011,2012,2015 Apple Inc. All rights reserved.
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
#include <corecrypto/ccn.h>

//#include <assert.h>

/* Counts the number of bits which are zero before the first one bit
   from least to most significant bit. */
size_t ccn_trailing_zeros(cc_size count, const cc_unit *s)
{
    cc_size i;
    /* Skip leading zero units (which represent the least significant bits). */
    for (i = 0; i < count && s[i] == 0; ++i);
    /* typecast: will always fit in an int */
    unsigned int zeros = (unsigned int)(i * CCN_UNIT_BITS);
    if (i < count) {
        cc_unit v = s[i];
        static const unsigned char nibble2zeros[16] = {
            4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
        };
        cc_unit nibble;
        do {
            nibble  = v & 15;
            zeros  += nibble2zeros[nibble];
            v >>= 4;
        } while (nibble == 0);
    }
    //assert(zeros == ccn_cnt_trailzeros(count, s));
    return zeros;
}

/* TODO: Eliminate sn and define rn as the smaller of the two sizes (this is unlike most other functions work), or possible the larger of the two for consistency, plus this allows using r as a scratch. */
/* Binary form of Greatest Common Divisor. r becomes the gcd of s and t. */
void ccn_gcdn(cc_size rn, cc_unit *r, cc_size sn, const cc_unit *s, cc_size tn, const cc_unit *t)
{
    cc_size un = ccn_n(sn, s);
    cc_size vn = ccn_n(tn, t);
    assert(rn >= un && rn >= vn);
    if (un == 0) {
        ccn_setn(rn, r, vn, t);
        return;
    }
    if (vn == 0) {
        ccn_setn(rn, r, un, s);
        return;
    }

    size_t k = ccn_trailing_zeros(un, s);
    size_t l = ccn_trailing_zeros(vn, t);
    /* Extra set of pointers so CC_SWAP can be used on them. */
    cc_unit ud[rn], vd[rn];
    cc_unit *u = ud, *v = vd;

    ccn_shift_right_multi(un, u, s, k);
    ccn_zero(rn - un, u + un);
    //un -= k >> CCN_LOG2_BITS_PER_UNIT;
    un = ccn_n(un, u);

    ccn_shift_right_multi(vn, v, t, l);
    ccn_zero(rn - vn, v + vn);
    //vn -= l >> CCN_LOG2_BITS_PER_UNIT;
    vn = ccn_n(vn, v);

    if (k > l)
        k = l;
    int m;
    while ((m = ccn_cmpn(un, u, vn, v))) {
        /* Make sure v is the largest */
        if (m > 0) {
            /* swap u and v to make sure v is >= u */
            CC_SWAP(u, v);
            CC_SWAP(un, vn);
        }
        ccn_sub(vn, v, v, u);
        vn = ccn_n(vn, v);

        l = ccn_trailing_zeros(vn, v);
        if (l) {
            ccn_shift_right_multi(vn, v, v, l);
            //vn -= l >> CCN_LOG2_BITS_PER_UNIT;
            vn = ccn_n(vn, v);
        }
    }

    ccn_shift_left_multi(rn, r, v, k);
}

/* Binary form of Greatest Common Divisor. r becomes the gcd of s and t. */
void ccn_gcd(cc_size n, cc_unit *r, const cc_unit *s, const cc_unit *t)
{
    ccn_gcdn(n, r, n, s, n, t);
}
