/*
 * Copyright (c) 2010,2011,2012,2014,2015 Apple Inc. All rights reserved.
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


#include <corecrypto/ccn.h>
#include <corecrypto/ccn_priv.h>

// Number of cc_unit under which regular schoolbook multiplication is applied.
#define CCN_MUL_KARATSUBA_THRESHOLD 10

#if CCN_MUL_KARATSUBA
// Karatsuba.
// Recursive but the recursion depth is low < log2(n)
// Use the workspace for memory. Confined with 4*n.
void ccn_mul_ws(cc_size n, cc_unit *r, const cc_unit *s, const cc_unit *t, cc_ws_t ws)
{
    if (n<CCN_MUL_KARATSUBA_THRESHOLD) {
        ccn_mul(n, r, s, t);
    }
    else {
        cc_assert(r != s);
        cc_assert(r != t);
        cc_size m1=n/2;
        cc_size m0=n-m1; // m0>=m1 guaranteed. b=2^(m0*CCN_UNIT_BITS)
        cc_unit *tmp;
        // Karatsuba multiplication
        // s = s1.b + s0
        // t = t1.b + t0
        // s1,t1 of size m1=n/2 in size
        // s0,t0 of size m0=n-n/2 cc_units. m >=n/2

        tmp=ws->start;
        ws->start+=2*n;
        cc_assert(ws->start<=ws->end); // Check that provided workspace is sufficient;
        cc_assert(3*m0<=2*n);   // 2*n > 3*m
        cc_unit c0, c1;
        cc_unit *Rc0[2] __attribute__((aligned(16)))={&tmp[m0],&tmp[0]};
        cc_unit *Rc1[2] __attribute__((aligned(16)))={&tmp[2*m0],&tmp[m0]};

        if (m0==m1) { // Size of the operands, not data dependent
            // |s1 - s0| in tmp[0]
            c0=ccn_sub(m0, &tmp[0],&s[0],&s[m0]); // s0 - s1
            ccn_sub(m0, Rc0[c0],&s[m0],&s[0]); // if s0<s1 => (s1-s0)

            // |t1 - t0| in tmp[m0]
            c1=ccn_sub(m0, &tmp[m0],&t[0],&t[m0]); // t0 - t1
            ccn_sub(m0, Rc1[c1], &t[m0],&t[0]); // if t0<t1 => (t1-t0)
            ccn_zero(2*n-3*m0,&r[3*m0]); //clear upper part of r
        }
        else
        {
            // |s1 - s0| in tmp[0]
            ccn_setn(m0,&r[0],m1,&s[m0]); // s1
            c0=ccn_sub(m0, &tmp[0],&s[0],&r[0]); // s0 - s1
            ccn_sub(m0, Rc0[c0],&r[0],&s[0]); // if s0<s1 => (s1-s0)

            // |t1 - t0| in tmp[m0]
            ccn_setn(2*n,&r[0],m1,&t[m0]); // t1, clear upper part of r
            c1=ccn_sub(m0, &tmp[m0],&t[0],&r[0]); // t0 - t1
            ccn_sub(m0, Rc1[c1], &r[0],&t[0]); // if t0<t1 => (t1-t0)
        }
        c1=c0^c1;

        // r=b*(|s1 - s0| * |t1 - t0|)
        ccn_zero(m0,r);
        ccn_mul_ws(m0,&r[m0],&tmp[0],&tmp[m0],ws);

        // r=(-1)^(c0+c1) * |s1 - s0| * |t1 - t0|
        Rc1[0]=&r[m0];  // {&r[m0],&tmp[m0]}
        ccn_zero(2*n-m0,&tmp[m0]);
        ccn_sub(2*n-m0,&r[m0],Rc1[c1^1],Rc1[c1]);

        // x1.y1
        ccn_mul_ws(m1,&tmp[0],&s[m0],&t[m0],ws);
        ccn_add(2*n-m0,&r[m0],&r[m0],&tmp[0]);        // r += b s1.t1
        ccn_add(2*m1,&r[2*m0],&r[2*m0],&tmp[0]);      // r += b^2 s1.t1

        // x0.y0
        ccn_mul_ws(m0, &tmp[0],&s[0],&t[0],ws);
        ccn_add(2*n,&r[0],&r[0],&tmp[0]);           // r += s0.t0
        ccn_add(2*n-m0,&r[m0],&r[m0],&tmp[0]);      // r += b * s0.t0

        // Release workingspace.
        ws->start=tmp;
    }
#if 0
    {
        // Debug
        cc_unit r_expected[2*n];
        ccn_mul(n, r_expected, s, t);
        if (ccn_cmp(2*n,r,r_expected)) {
            ccn_lprint(n,"Input s: ", s);
            ccn_lprint(n,"Input t: ", t);
            ccn_lprint(2*n,"Expected: ", r_expected);
            ccn_lprint(2*n,"Computed: ", r);
            cc_unit delta[2*n];
            ccn_sub(2*n,delta,r_expected,r);
            ccn_lprint(2*n,"Delta:    ", delta);
        }
        cc_assert(ccn_cmp(2*n,r,r_expected)==0);
    }
#endif // 0 debug
}

#else // Schoolbook
/* Multiplication using a workspace.
 Workspace needs must be declared in ccn_priv.h 
CCN_MUL_WS_WORKSPACE_N */
void ccn_mul_ws(cc_size n, cc_unit *r, const cc_unit *s, const cc_unit *t, cc_ws_t ws)
{
    (void) ws;
    assert(ws->start<=ws->end); // Check that provided workspace is sufficient;
    ccn_mul(n, r, s, t);
}
#endif

#if !CCN_MUL_ASM

#if CCN_MUL1_ASM && CCN_ADDMUL1_ASM

/* Constant time. NOTE: Seems like r and s may overlap, but r and t may not.
   Also if n is 0 this still writes one word to r. */
void ccn_mul(cc_size n, cc_unit *r, const cc_unit *s, const cc_unit *t)
{
    const cc_size sn = n;
    cc_size tn = n;
    assert(r != s);
    assert(r != t);

    r[sn] = ccn_mul1 (sn, r, s, t[0]);
    while (tn > 1)
    {
        r += 1, t += 1, tn -= 1;
        r[sn] = ccn_addmul1 (sn, r, s, t[0]);
    }
}

#else /* !(CCN_MUL1_ASM && CCN_ADDMUL1_ASM) */

/* Do r = s * t, r is 2 * count cc_units in size, s and t are count * cc_units in size. */
void ccn_mul(cc_size count, cc_unit *r, const cc_unit *s, const cc_unit *t)
{
    assert(r != s);
    assert(r != t);
    ccn_zero(count * 2, r);

#if !CCN_MUL1_UINT128_SUPPORT_FOR_64BIT_ARCH && (CCN_UNIT_SIZE == 8)
    typedef uint32_t cc_mulw;
    typedef uint64_t cc_muld;
#define r ((cc_mulw *)r)
#define s ((const cc_mulw *)s)
#define t ((const cc_mulw *)t)
#define CCMULW_BITS  (32)
#define CCMULW_MASK ((cc_mulw)~0)
    count *= CCN_UNIT_SIZE / sizeof(cc_mulw);
#else
    typedef cc_unit cc_mulw;
    typedef cc_dunit cc_muld;
#define CCMULW_BITS  CCN_UNIT_BITS
#define CCMULW_MASK CCN_UNIT_MASK
#endif

    cc_muld prod1, prod2, carry1 = 0, carry2 = 0;
    const cc_mulw *aptr, *bptr = t;
    cc_mulw *destptr, mult1, mult2;
    cc_size ix;
	for (ix = 0; ix < count - 1; ix += 2) {
		mult1 = *(bptr++);
		mult2 = *(bptr++);

		cc_mulw prevmul = 0;
		carry1 = 0;
		carry2 = 0;
		aptr = s;
		destptr = &r[ix];
		cc_muld prevDigit = *destptr;

		for (cc_size j = 0; j < count; ++j) {
			cc_mulw curmul = *aptr++;
			prevDigit += carry1 + carry2;

			prod1 = (cc_muld)curmul * mult1;
			prod2 = (cc_muld)prevmul * mult2;

			carry1 = prod1 >> CCMULW_BITS;
			carry2 = prod2 >> CCMULW_BITS;

			prod1 &= CCMULW_MASK;
			prod2 &= CCMULW_MASK;

			cc_muld prodsum = prod1 + prod2 + prevDigit;
			carry1 += prodsum >> CCMULW_BITS;
			prevDigit = *(destptr+1);
			*(destptr++) = (cc_mulw)prodsum;
			prevmul = curmul;
		}

		prod1 = prevDigit + carry1;
		prod1 += (cc_muld)prevmul * mult2;
		prod1 += carry2;
		carry1 = prod1 >> CCMULW_BITS;
		*(destptr++) = (cc_mulw)prod1;
		*destptr = (cc_mulw)carry1;
	}

    if (ix < count) {
        mult1 = *bptr;
        carry1 = 0;
        aptr = s;
        destptr = &r[ix];
        for (cc_size j = 0; j < count; ++j) {
            //prod = *(aptr++) * mult + *destptr + carry;
            prod1 = (cc_muld)(*aptr++);
            prod1 *= mult1;
            prod1 += *destptr;
            prod1 += carry1;
            *(destptr++) = (cc_mulw)prod1;
            carry1 = prod1 >> CCMULW_BITS;
        }
        *destptr = (cc_mulw)carry1;
    }
}

#endif /* !(CCN_MUL1_ASM && CCN_ADDMUL1_ASM) */

#endif /* !CCN_MUL_ASM */
