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


static void ccn_mod_224(cczp_const_t zp, cc_unit *r, const cc_unit *a, CC_UNUSED cc_ws_t ws) {
    cc_assert(cczp_n(zp) == CCN224_N);
    cc_unit s1[CCN224_N] = { ccn224_32(A(10),  A(9),  A(8),  A(7),  Anil,  Anil,  Anil) };
    cc_unit s2[CCN224_N] = { ccn224_32( Anil, A(13), A(12), A(11),  Anil,  Anil,  Anil) };
    cc_unit d2[CCN224_N] = { ccn224_32( Anil,  Anil,  Anil,  Anil, A(13), A(12), A(11)) };
    cc_unit carry;
    cc_unit *select[2] __attribute__((aligned(16))) ={s1,s2};

#if (CCN_UNIT_SIZE == 8)
    cc_unit d1[CCN224_N] = { ccn224_32(A(13), A(12), A(11), A(10),  A(9),  A(8),  A(7)) };
    cc_unit  t[CCN224_N] = { ccn224_32( A(6),  A(5),  A(4),  A(3),  A(2),  A(1),  A(0)) };

    /* No carry needed since ccn's are 32 bit bigger than needed and we only
       need 2 bits of carry. */
    ccn_add(CCN224_N, r, t, s1);
    ccn_add(CCN224_N, r, r, s2);
    ccn_add(CCN224_N, r, r, cczp_prime(zp));
    ccn_sub(CCN224_N, r, r, d1);
    ccn_sub(CCN224_N, s2, r, d2);
    
    /* Reduce r mod p224 by subtraction of up to three multiples of p224. */
    carry=ccn_sub(CCN224_N,s1,s2,cczp_prime(zp));
    carry^=ccn_sub(CCN224_N,select[carry^1],select[carry],cczp_prime(zp));
    carry^=ccn_sub(CCN224_N,select[carry],select[carry^1],cczp_prime(zp));
#else
    cc_unit carry_mask;
    carry = ccn_add(CCN224_N, r, a, s1);
    carry += ccn_add(CCN224_N, r, r, s2);
    carry -= ccn_sub(CCN224_N, s2, cczp_prime(zp), &a[CCN224_N]);
    carry += ccn_add(CCN224_N, r, r, s2);
    carry -= ccn_sub(CCN224_N, s2, r, d2);

    /* Reduce r mod p224 by subtraction of up to three multiples of p224. */
    carry_mask=CC_CARRY_2BITS(carry);
    carry -= (carry_mask & ccn_sub(CCN224_N,select[carry_mask],s2,cczp_prime(zp)));
    carry_mask=CC_CARRY_2BITS(carry);
    carry -= (carry_mask & ccn_sub(CCN224_N,select[carry_mask],s2,cczp_prime(zp)));
    carry ^= ccn_sub(CCN224_N,s1,s2,cczp_prime(zp));
#endif
    ccn_set(CCN224_N,r,select[carry]);

    /* Sanity for debug */
    cc_assert(ccn_cmp(CCN224_N, r, cczp_prime(zp)) < 0);
}

static const ccec_cp_decl(224) ccec_cp224 =
{
    .zp = {
        .n = CCN224_N,
        .options = 0,
        .mod_prime = ccn_mod_224
    },
    .p = {
        CCN224_C(ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,00,00,00,00,00,00,00,00,00,00,00,01)
    },
    .b = {
        CCN224_C(b4,05,0a,85,0c,04,b3,ab,f5,41,32,56,50,44,b0,b7,d7,bf,d8,ba,27,0b,39,43,23,55,ff,b4)
    },
    .gx = {
        CCN224_C(b7,0e,0c,bd,6b,b4,bf,7f,32,13,90,b9,4a,03,c1,d3,56,c2,11,22,34,32,80,d6,11,5c,1d,21)
    },
    .gy = {
        CCN224_C(bd,37,63,88,b5,f7,23,fb,4c,22,df,e6,cd,43,75,a0,5a,07,47,64,44,d5,81,99,85,00,7e,34)
    },
    .zq = {
        .n = CCN224_N,
        .options = 0,
        .mod_prime = cczp_mod
    },
    .q = {
        CCN224_C(ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,16,a2,e0,b8,f0,3e,13,dd,29,45,5c,5c,2a,3d)
    },
    .qr = {
        CCN232_C(01,00,00,00,00,00,00,00,00,00,00,00,00,00,00,e9,5d,1f,47,0f,c1,ec,22,d6,ba,a3,a3,d5,c3)
    }
};

ccec_const_cp_t ccec_cp_224(void)
{
    return (ccec_const_cp_t)(const struct cczp *)&ccec_cp224;
}
