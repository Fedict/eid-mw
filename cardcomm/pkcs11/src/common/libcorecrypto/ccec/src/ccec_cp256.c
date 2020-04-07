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


#include <corecrypto/ccec_priv.h>

#if defined(DEBUG) && 0
#define CCEC_P256_DEBUG 1
#else
#define CCEC_P256_DEBUG 0
#endif

/* Computes v*p, fillup CCN256_N+1 words */
CC_UNUSED static void ccn_addmul1_p256_generic(cc_size n, cc_unit *r) {
    cc_assert(n>=CCN256_N+1);
    cc_unit t[n];
    cc_unit v=r[0];
    t[CCN256_N]=ccn_mul1(CCN256_N,t,ccec_cp_p(ccec_cp_256()),v);
    ccn_zero(n-CCN256_N-1,&t[CCN256_N+1]);
    ccn_add(n,r,r,t);
}


#if (CCN_UNIT_SIZE==8)
static void ccn_addmul1_p256(cc_size n, cc_unit *r) {
    cc_assert(n>=CCN256_N+1);
    cc_unit v=r[0];
    cc_dunit prod;
    cc_unit  carry;

    // First word is omitted since this function is followed by shift;
    prod=(((cc_dunit)v)<<32) + r[1]; // v * (2^32 - 1) + r[1]
    r[1]=(cc_unit)prod;
    carry = prod >> CCN_UNIT_BITS;

    prod=(cc_dunit)r[2] + carry;     // v * 0          + r[2] + carry
    r[2]=(cc_unit)prod;
    carry = prod >> CCN_UNIT_BITS;

    prod=((((cc_dunit)v)*0xffffffff00000001) + r[3]) + carry; // v * (2^64 - 2^32) + 1 + r[3] + carry
    r[3]=(cc_unit)prod;
    carry = prod >> CCN_UNIT_BITS;

    for (cc_size j = 4; j < n-1; ++j) {
        prod=((cc_dunit)r[j])+carry;
        r[j]=(cc_unit)prod;
        carry = prod >> CCN_UNIT_BITS;
    }
    r[n-1]=r[n-1]+carry; // No carry here.
}

#elif (CCN_UNIT_SIZE==4)

static void ccn_addmul1_p256(cc_size n, cc_unit *r) {
    cc_assert(n>=CCN256_N+1);
    cc_dunit prod;
    cc_unit  carry;
    cc_unit v=r[0];

    // First word is omitted since this function is followed by shift;
    // Because the first word is omitted the first three iteration simplifies.
    // We can start directly at the word index 3.
    carry = v;
    for (cc_size j = 3; j < 6; j++) {
        prod=(cc_dunit)r[j] + carry;           // v * 0          + r[j] + carry
        r[j]=(cc_unit)prod;
        carry = prod >> CCN_UNIT_BITS;
    }

    prod=((cc_dunit)v) + r[6] + carry;           // v * 1          + r[6] + carry
    r[6]=(cc_unit)prod;
    carry = prod >> CCN_UNIT_BITS;

    prod=(((cc_dunit)v)<<32)-v + r[7] + carry; // v * (2^32 - 1) + r[7] + carry
    r[7]=(cc_unit)prod;
    carry = prod >> CCN_UNIT_BITS;

    for (cc_size j = 8; j < n-1; ++j) {
        prod=((cc_dunit)r[j])+carry;
        r[j]=(cc_unit)prod;
        carry = prod >> CCN_UNIT_BITS;
    }
    r[n-1]=r[n-1]+carry; // No carry here.
}

#else
static void ccn_addmul1_p256(cc_size n, cc_unit *r) {
    return ccn_addmul1_p256_generic(n, r);
}
#endif


static void ccn_mod_montgomery_256(cczp_const_t zp, cc_unit *r, const cc_unit *a, CC_UNUSED cc_ws_t ws) {
    /* TODO: use workspace */
    cc_assert(cczp_n(zp) == CCN256_N);
    cc_unit Tbuf[2*CCN256_N+1];
    cc_unit *T=Tbuf;
    cc_unit *select[2] __attribute__((aligned(16))) ={&Tbuf[0],&Tbuf[CCN256_N]};
    cc_size n=2*CCN256_N;
    cc_unit carry;
    /* Reduction assumes a Montgomery friendly modulus
         -1/p mod 2^s == 1
     */
#if CCEC_P256_DEBUG
    ccn_lprint(n,"Start:",a);
    ccn_lprint(1,"loop T1: ",a);
#endif
    ccn_set(n,T,a);
    T[n]=0;
    /* No carry possible on T for the first round if
     T < (p-1)(p-1) && T2 <= (2^s-1)*p
     => T+T2 < (p-1)(p-1)+(2^s-1)*p
     => T+T2 <  p^2 - 3p + 1 + 2^s.p */
    ccn_addmul1_p256(n,T++);
    for (size_t i=1;i<CCN256_N;i++)
    {
        ccn_addmul1_p256(n--,T++);
    }

    carry = T[CCN256_N];
    carry ^= ccn_sub(CCN256_N,Tbuf,T,cczp_prime(zp));
    ccn_set(CCN256_N,r,select[carry]);
#if CCEC_P256_DEBUG
    ccn_lprint(n,"Sub T:  ",T);
    ccn_lprint(n,"Sub T2: ",Tbuf2);
    ccn_lprint(CCN256_N,"End:",r);
#endif

    /* Sanity for debug */
    cc_assert(ccn_cmp(CCN256_N, r, cczp_prime(zp)) < 0);

}

static const ccec_cp_decl(256) ccec_cp256 =
{
    .zp = {
        .n = CCN256_N,
        .options = CCZP_MONTGOMERY,
        .mod_prime = ccn_mod_montgomery_256
    },
    .p = {
        CCN256_C(ff,ff,ff,ff,00,00,00,01,00,00,00,00,00,00,00,00,00,00,00,00,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff)
    },
    .pr = {
        CCN256_C(00,00,00,04,ff,ff,ff,fd,ff,ff,ff,ff,ff,ff,ff,fe,ff,ff,ff,fb,ff,ff,ff,ff,00,00,00,00,00,00,00,03),0
    },
#ifdef LEGACY
    .b = {
        CCN256_C(5a,c6,35,d8,aa,3a,93,e7,b3,eb,bd,55,76,98,86,bc,65,1d,06,b0,cc,53,b0,f6,3b,ce,3c,3e,27,d2,60,4b)
    },
#else
    .b = {
        CCN256_C(dc,30,06,1d,04,87,48,34,e5,a2,20,ab,f7,21,2e,d6,ac,f0,05,cd,78,84,30,90,d8,9c,df,62,29,c4,bd,df)
    },
#endif
    .gx = {
        CCN256_C(6b,17,d1,f2,e1,2c,42,47,f8,bc,e6,e5,63,a4,40,f2,77,03,7d,81,2d,eb,33,a0,f4,a1,39,45,d8,98,c2,96)
    },
    .gy = {
        CCN256_C(4f,e3,42,e2,fe,1a,7f,9b,8e,e7,eb,4a,7c,0f,9e,16,2b,ce,33,57,6b,31,5e,ce,cb,b6,40,68,37,bf,51,f5)
    },
    .zq = {
        .n = CCN256_N,
        .options = 0,
        .mod_prime = cczp_mod
    },
    .q = {
        CCN256_C(ff,ff,ff,ff,00,00,00,00,ff,ff,ff,ff,ff,ff,ff,ff,bc,e6,fa,ad,a7,17,9e,84,f3,b9,ca,c2,fc,63,25,51)
    },
    .qr = {
        CCN264_C(01,00,00,00,00,ff,ff,ff,ff,ff,ff,ff,fe,ff,ff,ff,ff,43,19,05,52,df,1a,6c,21,01,2f,fd,85,ee,df,9b,fe)
    }
};

ccec_const_cp_t ccec_cp_256(void)
{
        return (ccec_const_cp_t)(const struct cczp *)&ccec_cp256;
}


