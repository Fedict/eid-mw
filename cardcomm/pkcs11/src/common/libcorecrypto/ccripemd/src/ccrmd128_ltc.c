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


/*
 * Parts of this code adapted from LibTomCrypt
 *
 * LibTomCrypt, modular cryptographic library -- Tom St Denis
 *
 * LibTomCrypt is a library that provides various cryptographic
 * algorithms in a highly modular and flexible manner.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 *
 * Tom St Denis, tomstdenis@gmail.com, http://libtom.org
 */


#define USE_SUPER_COOL_NEW_CCOID_T
#include <corecrypto/ccripemd.h>
#include <corecrypto/ccmd4.h>
#include <corecrypto/cc_priv.h>
#include <corecrypto/ccdigest_priv.h>


/* the four basic functions F(), G() and H() */
#define F(x, y, z)        ((x) ^ (y) ^ (z))
#define G(x, y, z)        (((x) & (y)) | (~(x) & (z)))
#define H(x, y, z)        (((x) | ~(y)) ^ (z))
#define I(x, y, z)        (((x) & (z)) | ((y) & ~(z)))

/* the eight basic operations FF() through III() */
#define FF(a, b, c, d, x, s)        \
(a) += F((b), (c), (d)) + (x);\
(a) = CC_ROLc((a), (s));

#define GG(a, b, c, d, x, s)        \
(a) += G((b), (c), (d)) + (x) + 0x5a827999;\
(a) = CC_ROLc((a), (s));

#define HH(a, b, c, d, x, s)        \
(a) += H((b), (c), (d)) + (x) + 0x6ed9eba1;\
(a) = CC_ROLc((a), (s));

#define II(a, b, c, d, x, s)        \
(a) += I((b), (c), (d)) + (x) + 0x8f1bbcdc;\
(a) = CC_ROLc((a), (s));

#define FFF(a, b, c, d, x, s)        \
(a) += F((b), (c), (d)) + (x);\
(a) = CC_ROLc((a), (s));

#define GGG(a, b, c, d, x, s)        \
(a) += G((b), (c), (d)) + (x) + 0x6d703ef3;\
(a) = CC_ROLc((a), (s));

#define HHH(a, b, c, d, x, s)        \
(a) += H((b), (c), (d)) + (x) + 0x5c4dd124;\
(a) = CC_ROLc((a), (s));

#define III(a, b, c, d, x, s)        \
(a) += I((b), (c), (d)) + (x) + 0x50a28be6;\
(a) = CC_ROLc((a), (s));


static void rmd128_compress(ccdigest_state_t state, unsigned long nblocks, const void *in)
{
    uint32_t aa,bb,cc,dd,aaa,bbb,ccc,ddd,X[16];
    int i;
    uint32_t *s = ccdigest_u32(state);
    const unsigned char *buf = in;

    while(nblocks--) {

        /* load words X */
        for (i = 0; i < 16; i++){
            CC_LOAD32_LE(X[i], buf + (4 * i));
        }

        /* load state */
        aa = aaa = s[0];
        bb = bbb = s[1];
        cc = ccc = s[2];
        dd = ddd = s[3];

        /* round 1 */
        FF(aa, bb, cc, dd, X[ 0], 11);
        FF(dd, aa, bb, cc, X[ 1], 14);
        FF(cc, dd, aa, bb, X[ 2], 15);
        FF(bb, cc, dd, aa, X[ 3], 12);
        FF(aa, bb, cc, dd, X[ 4],  5);
        FF(dd, aa, bb, cc, X[ 5],  8);
        FF(cc, dd, aa, bb, X[ 6],  7);
        FF(bb, cc, dd, aa, X[ 7],  9);
        FF(aa, bb, cc, dd, X[ 8], 11);
        FF(dd, aa, bb, cc, X[ 9], 13);
        FF(cc, dd, aa, bb, X[10], 14);
        FF(bb, cc, dd, aa, X[11], 15);
        FF(aa, bb, cc, dd, X[12],  6);
        FF(dd, aa, bb, cc, X[13],  7);
        FF(cc, dd, aa, bb, X[14],  9);
        FF(bb, cc, dd, aa, X[15],  8);

        /* round 2 */
        GG(aa, bb, cc, dd, X[ 7],  7);
        GG(dd, aa, bb, cc, X[ 4],  6);
        GG(cc, dd, aa, bb, X[13],  8);
        GG(bb, cc, dd, aa, X[ 1], 13);
        GG(aa, bb, cc, dd, X[10], 11);
        GG(dd, aa, bb, cc, X[ 6],  9);
        GG(cc, dd, aa, bb, X[15],  7);
        GG(bb, cc, dd, aa, X[ 3], 15);
        GG(aa, bb, cc, dd, X[12],  7);
        GG(dd, aa, bb, cc, X[ 0], 12);
        GG(cc, dd, aa, bb, X[ 9], 15);
        GG(bb, cc, dd, aa, X[ 5],  9);
        GG(aa, bb, cc, dd, X[ 2], 11);
        GG(dd, aa, bb, cc, X[14],  7);
        GG(cc, dd, aa, bb, X[11], 13);
        GG(bb, cc, dd, aa, X[ 8], 12);

        /* round 3 */
        HH(aa, bb, cc, dd, X[ 3], 11);
        HH(dd, aa, bb, cc, X[10], 13);
        HH(cc, dd, aa, bb, X[14],  6);
        HH(bb, cc, dd, aa, X[ 4],  7);
        HH(aa, bb, cc, dd, X[ 9], 14);
        HH(dd, aa, bb, cc, X[15],  9);
        HH(cc, dd, aa, bb, X[ 8], 13);
        HH(bb, cc, dd, aa, X[ 1], 15);
        HH(aa, bb, cc, dd, X[ 2], 14);
        HH(dd, aa, bb, cc, X[ 7],  8);
        HH(cc, dd, aa, bb, X[ 0], 13);
        HH(bb, cc, dd, aa, X[ 6],  6);
        HH(aa, bb, cc, dd, X[13],  5);
        HH(dd, aa, bb, cc, X[11], 12);
        HH(cc, dd, aa, bb, X[ 5],  7);
        HH(bb, cc, dd, aa, X[12],  5);

        /* round 4 */
        II(aa, bb, cc, dd, X[ 1], 11);
        II(dd, aa, bb, cc, X[ 9], 12);
        II(cc, dd, aa, bb, X[11], 14);
        II(bb, cc, dd, aa, X[10], 15);
        II(aa, bb, cc, dd, X[ 0], 14);
        II(dd, aa, bb, cc, X[ 8], 15);
        II(cc, dd, aa, bb, X[12],  9);
        II(bb, cc, dd, aa, X[ 4],  8);
        II(aa, bb, cc, dd, X[13],  9);
        II(dd, aa, bb, cc, X[ 3], 14);
        II(cc, dd, aa, bb, X[ 7],  5);
        II(bb, cc, dd, aa, X[15],  6);
        II(aa, bb, cc, dd, X[14],  8);
        II(dd, aa, bb, cc, X[ 5],  6);
        II(cc, dd, aa, bb, X[ 6],  5);
        II(bb, cc, dd, aa, X[ 2], 12);

        /* parallel round 1 */
        III(aaa, bbb, ccc, ddd, X[ 5],  8);
        III(ddd, aaa, bbb, ccc, X[14],  9);
        III(ccc, ddd, aaa, bbb, X[ 7],  9);
        III(bbb, ccc, ddd, aaa, X[ 0], 11);
        III(aaa, bbb, ccc, ddd, X[ 9], 13);
        III(ddd, aaa, bbb, ccc, X[ 2], 15);
        III(ccc, ddd, aaa, bbb, X[11], 15);
        III(bbb, ccc, ddd, aaa, X[ 4],  5);
        III(aaa, bbb, ccc, ddd, X[13],  7);
        III(ddd, aaa, bbb, ccc, X[ 6],  7);
        III(ccc, ddd, aaa, bbb, X[15],  8);
        III(bbb, ccc, ddd, aaa, X[ 8], 11);
        III(aaa, bbb, ccc, ddd, X[ 1], 14);
        III(ddd, aaa, bbb, ccc, X[10], 14);
        III(ccc, ddd, aaa, bbb, X[ 3], 12);
        III(bbb, ccc, ddd, aaa, X[12],  6);

        /* parallel round 2 */
        HHH(aaa, bbb, ccc, ddd, X[ 6],  9);
        HHH(ddd, aaa, bbb, ccc, X[11], 13);
        HHH(ccc, ddd, aaa, bbb, X[ 3], 15);
        HHH(bbb, ccc, ddd, aaa, X[ 7],  7);
        HHH(aaa, bbb, ccc, ddd, X[ 0], 12);
        HHH(ddd, aaa, bbb, ccc, X[13],  8);
        HHH(ccc, ddd, aaa, bbb, X[ 5],  9);
        HHH(bbb, ccc, ddd, aaa, X[10], 11);
        HHH(aaa, bbb, ccc, ddd, X[14],  7);
        HHH(ddd, aaa, bbb, ccc, X[15],  7);
        HHH(ccc, ddd, aaa, bbb, X[ 8], 12);
        HHH(bbb, ccc, ddd, aaa, X[12],  7);
        HHH(aaa, bbb, ccc, ddd, X[ 4],  6);
        HHH(ddd, aaa, bbb, ccc, X[ 9], 15);
        HHH(ccc, ddd, aaa, bbb, X[ 1], 13);
        HHH(bbb, ccc, ddd, aaa, X[ 2], 11);

        /* parallel round 3 */
        GGG(aaa, bbb, ccc, ddd, X[15],  9);
        GGG(ddd, aaa, bbb, ccc, X[ 5],  7);
        GGG(ccc, ddd, aaa, bbb, X[ 1], 15);
        GGG(bbb, ccc, ddd, aaa, X[ 3], 11);
        GGG(aaa, bbb, ccc, ddd, X[ 7],  8);
        GGG(ddd, aaa, bbb, ccc, X[14],  6);
        GGG(ccc, ddd, aaa, bbb, X[ 6],  6);
        GGG(bbb, ccc, ddd, aaa, X[ 9], 14);
        GGG(aaa, bbb, ccc, ddd, X[11], 12);
        GGG(ddd, aaa, bbb, ccc, X[ 8], 13);
        GGG(ccc, ddd, aaa, bbb, X[12],  5);
        GGG(bbb, ccc, ddd, aaa, X[ 2], 14);
        GGG(aaa, bbb, ccc, ddd, X[10], 13);
        GGG(ddd, aaa, bbb, ccc, X[ 0], 13);
        GGG(ccc, ddd, aaa, bbb, X[ 4],  7);
        GGG(bbb, ccc, ddd, aaa, X[13],  5);

        /* parallel round 4 */
        FFF(aaa, bbb, ccc, ddd, X[ 8], 15);
        FFF(ddd, aaa, bbb, ccc, X[ 6],  5);
        FFF(ccc, ddd, aaa, bbb, X[ 4],  8);
        FFF(bbb, ccc, ddd, aaa, X[ 1], 11);
        FFF(aaa, bbb, ccc, ddd, X[ 3], 14);
        FFF(ddd, aaa, bbb, ccc, X[11], 14);
        FFF(ccc, ddd, aaa, bbb, X[15],  6);
        FFF(bbb, ccc, ddd, aaa, X[ 0], 14);
        FFF(aaa, bbb, ccc, ddd, X[ 5],  6);
        FFF(ddd, aaa, bbb, ccc, X[12],  9);
        FFF(ccc, ddd, aaa, bbb, X[ 2], 12);
        FFF(bbb, ccc, ddd, aaa, X[13],  9);
        FFF(aaa, bbb, ccc, ddd, X[ 9], 12);
        FFF(ddd, aaa, bbb, ccc, X[ 7],  5);
        FFF(ccc, ddd, aaa, bbb, X[10], 15);
        FFF(bbb, ccc, ddd, aaa, X[14],  8);

        /* combine results */
        ddd += cc + s[1];               /* final result for MDbuf[0] */
        s[1] = s[2] + dd + aaa;
        s[2] = s[3] + aa + bbb;
        s[3] = s[0] + bb + ccc;
        s[0] = ddd;

        buf+=CCRMD_BLOCK_SIZE;
    }
}

const struct ccdigest_info ccrmd128_ltc_di = {
    .output_size = CCRMD128_OUTPUT_SIZE,
    .state_size = CCRMD128_STATE_SIZE,
    .block_size = CCRMD_BLOCK_SIZE,
    .oid_size = 8,
    .oid = CC_DIGEST_OID_RMD128,
    .initial_state = ccmd4_initial_state,
    .compress = rmd128_compress,
    .final = ccdigest_final_64le,
};
