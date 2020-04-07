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

static void ccn_mod_521(cczp_const_t zp, cc_unit *r, const cc_unit *a, CC_UNUSED cc_ws_t ws) {
    cc_assert(cczp_n(zp) == CCN521_N);
    cc_unit t[CCN521_N];
    cc_unit t2[CCN521_N];
    cc_unit *select[2] __attribute__((aligned(16))) ={t,t2};
    cc_unit borrow;

#if CCN_UNIT_SIZE == 1
    ccn_shift_right(CCN521_N - 1, t2, &a[CCN521_N - 1], 1); // r = a521,...,a1041
    t2[CCN521_N - 1] += a[CCN521_N - 1] & CC_UNIT_C(1);
    t2[CCN521_N - 1] += ccn_add(CCN521_N - 1,t2,t2,a);
#else
    ccn_shift_right(CCN521_N, t2, &a[CCN512_N], 9);  // r = a521,...,a1041
    t2[CCN512_N] += a[CCN512_N] & CC_UNIT_C(0x1ff);  // r += (a512,...,a520)*2^512
    t2[CCN512_N] += ccn_add(CCN512_N,t2,t2,a);         // r += a0,...,a511
#endif
    borrow=ccn_sub(CCN521_N, t, t2, cczp_prime(zp));
    ccn_set(CCN521_N,r,select[borrow]);
}

static const ccec_cp_decl(521) ccec_cp521 =
{
    .zp = {
        .n = CCN521_N,
        .options = 0,
        .mod_prime = ccn_mod_521
    },
    .p = {
        CCN528_C(01,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff)
    },
    .b = {

        CCN528_C(00,51,95,3e,b9,61,8e,1c,9a,1f,92,9a,21,a0,b6,85,40,ee,a2,da,72,5b,99,b3,15,f3,b8,b4,89,91,8e,f1,09,e1,56,19,39,51,ec,7e,93,7b,16,52,c0,bd,3b,b1,bf,07,35,73,df,88,3d,2c,34,f1,ef,45,1f,d4,6b,50,3f,00)
    },
    .gx = {

        CCN528_C(00,c6,85,8e,06,b7,04,04,e9,cd,9e,3e,cb,66,23,95,b4,42,9c,64,81,39,05,3f,b5,21,f8,28,af,60,6b,4d,3d,ba,a1,4b,5e,77,ef,e7,59,28,fe,1d,c1,27,a2,ff,a8,de,33,48,b3,c1,85,6a,42,9b,f9,7e,7e,31,c2,e5,bd,66)
    },
    .gy = {
        CCN528_C(01,18,39,29,6a,78,9a,3b,c0,04,5c,8a,5f,b4,2c,7d,1b,d9,98,f5,44,49,57,9b,44,68,17,af,bd,17,27,3e,66,2c,97,ee,72,99,5e,f4,26,40,c5,50,b9,01,3f,ad,07,61,35,3c,70,86,a2,72,c2,40,88,be,94,76,9f,d1,66,50)
    },
    .zq = {
        .n = CCN521_N,
        .options = 0,
        .mod_prime = cczp_mod
    },
    .q = {
        CCN528_C(01,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,fa,51,86,87,83,bf,2f,96,6b,7f,cc,01,48,f7,09,a5,d0,3b,b5,c9,b8,89,9c,47,ae,bb,6f,b7,1e,91,38,64,09)
    },
    .qr = {
        CCN528_C(02,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,05,ae,79,78,7c,40,d0,69,94,80,33,fe,b7,08,f6,5a,2f,c4,4a,36,47,76,63,b8,51,44,90,48,e1,6e,c7,9b,f7)
    }
};

ccec_const_cp_t ccec_cp_521(void)
{
    return (ccec_const_cp_t)(const struct cczp *)&ccec_cp521;
}
