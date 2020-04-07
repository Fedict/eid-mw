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

#define A(i) ccn64_64_parse(a,i)
#define Anil ccn64_64_null

static void ccn_mod_192(cczp_const_t zp, cc_unit *r, const cc_unit *a, CC_UNUSED cc_ws_t ws) {
    cc_assert(cczp_n(zp) == CCN192_N);
    cc_unit s1[CCN192_N] = { ccn192_64(Anil,  A(3),  A(3)) };
    cc_unit s2[CCN192_N] = { ccn192_64( A(4),  A(4),  Anil) };
    cc_unit s3[CCN192_N] = { ccn192_64( A(5),  A(5),  A(5)) };
    cc_unit *select[2] __attribute__((aligned(16))) ={s1,s2};
    
    cc_unit carry,carry_mask;
    carry =  ccn_add(CCN192_N, r, a, s1);
    carry += ccn_add(CCN192_N, r, r, s2);
    carry += ccn_add(CCN192_N, s2, r, s3);

    /* Reduce r mod p192 by subtraction of up to three multiples of p192. */
    carry_mask=CC_CARRY_2BITS(carry);
    carry -= (carry_mask & ccn_sub(CCN192_N,select[carry_mask],s2,cczp_prime(zp)));
    carry_mask=CC_CARRY_2BITS(carry);
    carry -= (carry_mask & ccn_sub(CCN192_N,select[carry_mask],s2,cczp_prime(zp)));
    carry ^= ccn_sub(CCN192_N,s1,s2,cczp_prime(zp));
    ccn_set(CCN192_N,r,select[carry]);

    /* Sanity for debug */
    cc_assert(ccn_cmp(CCN192_N, r, cczp_prime(zp)) < 0);
}

static const ccec_cp_decl(192) ccec_cp192 =
{
    .zp = {
        .n = CCN192_N,
        .options = 0,
        .mod_prime = ccn_mod_192
    },
    .p = {
        CCN192_C(ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,fe,ff,ff,ff,ff,ff,ff,ff,ff)
    },
    .b = {
        CCN192_C(64,21,05,19,e5,9c,80,e7,0f,a7,e9,ab,72,24,30,49,fe,b8,de,ec,c1,46,b9,b1)
    },
    .gx = {
        CCN192_C(18,8d,a8,0e,b0,30,90,f6,7c,bf,20,eb,43,a1,88,00,f4,ff,0a,fd,82,ff,10,12)
    },
    .gy = {
        CCN192_C(07,19,2b,95,ff,c8,da,78,63,10,11,ed,6b,24,cd,d5,73,f9,77,a1,1e,79,48,11)
    },
    .zq = {
        .n = CCN192_N,
        .options = 0,
        .mod_prime = cczp_mod
    },
    .q = {
        CCN192_C(ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,99,de,f8,36,14,6b,c9,b1,b4,d2,28,31)
    },
    .qr = {
        CCN200_C(01,00,00,00,00,00,00,00,00,00,00,00,00,66,21,07,c9,eb,94,36,4e,4b,2d,d7,cf)
    }
};

ccec_const_cp_t ccec_cp_192(void)
{
    return (ccec_const_cp_t)(const struct cczp *)&ccec_cp192;
}
