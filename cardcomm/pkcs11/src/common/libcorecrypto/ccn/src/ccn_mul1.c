/*
 * Copyright (c) 2012,2013,2014,2015 Apple Inc. All rights reserved.
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


#if !(CCN_MUL1_ASM && CCN_ADDMUL1_ASM)

// No need to handle 64bit separately.
// uint128_t is defined for 64bit architectures
#if !CCN_MUL1_UINT128_SUPPORT_FOR_64BIT_ARCH && (CCN_UNIT_SIZE == 8)
typedef uint32_t cc_mulw;
typedef uint64_t cc_muld;
#define CCMULW_BITS  (32)
#define CCMULW_MASK ((cc_mulw)~0)
#else
typedef cc_unit cc_mulw;
typedef cc_dunit cc_muld;
#define CCMULW_BITS  CCN_UNIT_BITS
#define CCMULW_MASK CCN_UNIT_MASK
#endif


#if !CCN_MUL1_UINT128_SUPPORT_FOR_64BIT_ARCH && (CCN_UNIT_SIZE == 8)

/* Write s_count + 2 words to destptr. computes destptr += aptr * (mult1, mult2). */
static void ccn_mul2_(cc_size s_count, cc_mulw *destptr, const cc_mulw *aptr, cc_mulw mult1, cc_mulw mult2) {
    cc_mulw prevmul = 0;
    cc_muld prod1, prod2, carry1 = 0, carry2 = 0;
    
    for (cc_size j = 0; j < s_count; ++j) {
        cc_mulw curmul = *aptr++;
        
        cc_muld prodsum = carry1 + carry2;
        
        prod1 = (cc_muld)curmul * mult1;
        prod2 = (cc_muld)prevmul * mult2;
        
        carry1 = prod1 >> CCMULW_BITS;
        carry2 = prod2 >> CCMULW_BITS;
        
        prod1 &= CCMULW_MASK;
        prod2 &= CCMULW_MASK;
        
        prodsum += prod1 + prod2;
        carry1 += prodsum >> CCMULW_BITS;
        *(destptr++) = (cc_mulw)prodsum;
        prevmul = curmul;
    }
#error CoreCrypto assumes uint128_t is defined on 64-bit machines
    /* TODO: See <rdar://problem/20654881> */
    prod1 = carry1;
    prod1 += (cc_muld)prevmul * mult2;
    prod1 += carry2;
    carry1 = prod1 >> CCMULW_BITS;
    *(destptr++) = (cc_mulw)prod1;
    *destptr = (cc_mulw)carry1;
}

/* Write s_count + 2 words to destptr. computes destptr += aptr * (mult1, mult2). */
static void ccn_addmul2_(cc_size s_count, cc_mulw *destptr, const cc_mulw *aptr, cc_mulw mult1, cc_mulw mult2) {
    assert(destptr);
    assert(aptr);
    cc_mulw prevmul = 0;
    cc_muld prod1, prod2, carry1 = 0, carry2 = 0, prevDigit = *destptr;
    
    for (cc_size j = 0; j < s_count; ++j) {
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

static void ccn_addmul1_c(cc_size n, cc_unit *r, const cc_unit *s, const cc_unit v) {
    cc_mulw *destptr = (cc_mulw *)r;
    cc_mulw *aptr = (cc_mulw *)s;
    cc_mulw *u = (cc_mulw *) &v;
    ccn_addmul2_(2 * n, destptr, aptr, u[0], u[1]);
}
static void ccn_mul1_c(cc_size n, cc_unit *r, const cc_unit *s, const cc_unit v) {
    cc_mulw *destptr = (cc_mulw *)r;
    cc_mulw *aptr = (cc_mulw *)s;
    cc_mulw *u = (cc_mulw *) &v;
    ccn_mul2_(2 * n, destptr, aptr, u[0], u[1]);
}

#else // if uin128_t is supported or it is a 32 bit machine
/* Write s_count + 1 words to destptr. computes destptr += aptr * mult1. */
static void ccn_mul1_(cc_size s_count, cc_mulw *destptr, const cc_mulw *aptr, cc_mulw mult1) {
    cc_muld prod1, carry1 = 0;
    for (cc_size j = 0; j < s_count; ++j) {
        //prod = *(aptr++) * mult + carry;
        prod1 = (cc_muld)(*aptr++);
        prod1 *= mult1;
        prod1 += carry1;
        *(destptr++) = (cc_mulw)prod1;
        carry1 = prod1 >> CCMULW_BITS;
    }
    *destptr = (cc_mulw)carry1;
}

/* Write s_count + 1 words to destptr. computes destptr += aptr * mult1. */
static void ccn_addmul1_(cc_size s_count, cc_mulw *destptr, const cc_mulw *aptr, cc_mulw mult1) {
    cc_muld prod1, carry1 = 0;
    for (cc_size j = 0; j < s_count; ++j) {
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

static void ccn_addmul1_c(cc_size n, cc_unit *r, const cc_unit *s, const cc_unit v) {
    cc_mulw *destptr = (cc_mulw *)r;
    const cc_mulw *aptr = (const cc_mulw *)s;
    ccn_addmul1_(n, destptr, aptr, v);
}
static void ccn_mul1_c(cc_size n, cc_unit *r, const cc_unit *s, const cc_unit v) {
    cc_mulw *destptr = (cc_mulw *)r;
    const cc_mulw *aptr = (const cc_mulw *)s;
    ccn_mul1_(n, destptr, aptr, v);
}

#endif /* CCN_UNIT_SIZE == 8 */

cc_unit ccn_mul1(cc_size n, cc_unit *r, const cc_unit *s, const cc_unit v) {
    cc_unit tmp[n+1];
    
    ccn_mul1_c(n, tmp, s, v);
    ccn_set(n, r, tmp);
    return tmp[n];
}

cc_unit ccn_addmul1(cc_size n, cc_unit *r, const cc_unit *s, const cc_unit v) {
    cc_unit tmp[n+1];
    ccn_setn(n+1,tmp,n,r);
    ccn_addmul1_c(n, tmp, s, v);
    ccn_set(n, r, tmp);
    return tmp[n];
}


#endif /* !(CCN_MUL1_ASM && CCN_ADDMUL1_ASM) */
