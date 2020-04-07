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

#define A(i) ccn32_32_parse(a,i)
#define Anil ccn32_32_null

static void ccn_mod_384(cczp_const_t zp, cc_unit *r, const cc_unit *a, CC_UNUSED cc_ws_t ws) {
    cc_assert(cczp_n(zp) == CCN384_N);
    cc_unit s1[CCN384_N] = { ccn384_32(  Anil,  Anil,  Anil,  Anil,  Anil, A(23), A(22), A(21),  Anil,  Anil,  Anil,  Anil) };
    //cc_unit s2[CCN384_N] = { ccn384_32( A(23), A(22), A(21), A(20), A(19), A(18), A(17), A(16), A(15), A(14), A(13), A(12)) };
    cc_unit s3[CCN384_N] = { ccn384_32( A(20), A(19), A(18), A(17), A(16), A(15), A(14), A(13), A(12), A(23), A(22), A(21)) };
    cc_unit s4[CCN384_N] = { ccn384_32( A(19), A(18), A(17), A(16), A(15), A(14), A(13), A(12), A(20),  Anil, A(23),  Anil) };
    cc_unit s5[CCN384_N] = { ccn384_32(  Anil,  Anil,  Anil,  Anil, A(23), A(22), A(21), A(20),  Anil,  Anil,  Anil,  Anil) };
    cc_unit s6[CCN384_N] = { ccn384_32(  Anil,  Anil,  Anil,  Anil,  Anil,  Anil, A(23), A(22), A(21),  Anil,  Anil, A(20)) };
    cc_unit d1[CCN384_N] = { ccn384_32( A(22), A(21), A(20), A(19), A(18), A(17), A(16), A(15), A(14), A(13), A(12), A(23)) };
    cc_unit d2[CCN384_N] = { ccn384_32(  Anil,  Anil,  Anil,  Anil,  Anil,  Anil,  Anil, A(23), A(22), A(21), A(20),  Anil) };
    cc_unit d3[CCN384_N] = { ccn384_32(  Anil,  Anil,  Anil,  Anil,  Anil,  Anil,  Anil, A(23), A(23),  Anil,  Anil,  Anil) };
    cc_unit *select[2] __attribute__((aligned(16))) ={s1,s3};

    cc_unit carry,carry_mask;
    ccn_add(ccn_nof(160)+1, d2, d2, d3);  // smaller size and no carry possible
    ccn_add(ccn_nof(224)+1, s1, s1, s1);  // smaller size and no carry possible, alternatively cc_shiftl(s1, 1) but add is currently faster.
    ccn_add(ccn_nof(256)+1, s5, s5, s1);  // smaller size and no carry possible
    ccn_add(ccn_nof(256)+1, s5, s5, s6);  // smaller size and no carry possible

    carry = ccn_add(CCN384_N, r, a, &a[CCN384_N]);
    carry += ccn_add(CCN384_N, r, r, s3);
    carry += ccn_add(CCN384_N, r, r, s4);
    carry += ccn_add(CCN384_N, r, r, s5);
    carry -= ccn_sub(CCN384_N, d1, cczp_prime(zp), d1);
    carry += ccn_add(CCN384_N, r, r, d1);
    carry -= ccn_sub(CCN384_N, s3, r, d2);

    /* Reduce r mod p384 by subtraction of up to four multiples of p384. */
    carry_mask=CC_CARRY_3BITS(carry);
    carry -= (carry_mask & ccn_sub(CCN384_N,select[carry_mask],s3,cczp_prime(zp)));
    carry_mask=CC_CARRY_2BITS(carry);
    carry -= (carry_mask & ccn_sub(CCN384_N,select[carry_mask],s3,cczp_prime(zp)));
    carry_mask=CC_CARRY_2BITS(carry);
    carry -= (carry_mask & ccn_sub(CCN384_N,select[carry_mask],s3,cczp_prime(zp)));
    carry ^= ccn_sub(CCN384_N,s1,s3,cczp_prime(zp));

    ccn_set(CCN384_N,r,select[carry]);

    /* Sanity for debug */
    cc_assert(ccn_cmp(CCN384_N, r, cczp_prime(zp)) < 0);

}

static const ccec_cp_decl(384) ccec_cp384 =
{
    .zp = {
        .n = CCN384_N,
        .options = 0,
        .mod_prime = ccn_mod_384
    },
    .p = {
        CCN384_C(ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,fe,ff,ff,ff,ff,00,00,00,00,00,00,00,00,ff,ff,ff,ff)
    },
    .b = {
        CCN384_C(b3,31,2f,a7,e2,3e,e7,e4,98,8e,05,6b,e3,f8,2d,19,18,1d,9c,6e,fe,81,41,12,03,14,08,8f,50,13,87,5a,c6,56,39,8d,8a,2e,d1,9d,2a,85,c8,ed,d3,ec,2a,ef)
    },
    .gx = {
        CCN384_C(aa,87,ca,22,be,8b,05,37,8e,b1,c7,1e,f3,20,ad,74,6e,1d,3b,62,8b,a7,9b,98,59,f7,41,e0,82,54,2a,38,55,02,f2,5d,bf,55,29,6c,3a,54,5e,38,72,76,0a,b7)
    },
    .gy = {
        CCN384_C(36,17,de,4a,96,26,2c,6f,5d,9e,98,bf,92,92,dc,29,f8,f4,1d,bd,28,9a,14,7c,e9,da,31,13,b5,f0,b8,c0,0a,60,b1,ce,1d,7e,81,9d,7a,43,1d,7c,90,ea,0e,5f)
    },
    .zq = {
        .n = CCN384_N,
        .options = 0,
        .mod_prime = cczp_mod
    },
    .q = {
        CCN384_C(ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,c7,63,4d,81,f4,37,2d,df,58,1a,0d,b2,48,b0,a7,7a,ec,ec,19,6a,cc,c5,29,73)
    },
    .qr = {
        CCN392_C(01,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,38,9c,b2,7e,0b,c8,d2,20,a7,e5,f2,4d,b7,4f,58,85,13,13,e6,95,33,3a,d6,8d)
    }
};

ccec_const_cp_t ccec_cp_384(void)
{
    return (ccec_const_cp_t)(const struct cczp *)&ccec_cp384;
}
