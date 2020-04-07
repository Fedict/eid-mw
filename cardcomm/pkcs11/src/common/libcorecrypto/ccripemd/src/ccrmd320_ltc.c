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
#include <corecrypto/cc_priv.h>
#include <corecrypto/ccdigest_priv.h>

/* the five basic functions F(), G() and H() */
#define F(x, y, z)        ((x) ^ (y) ^ (z))
#define G(x, y, z)        (((x) & (y)) | (~(x) & (z)))
#define H(x, y, z)        (((x) | ~(y)) ^ (z))
#define I(x, y, z)        (((x) & (z)) | ((y) & ~(z)))
#define J(x, y, z)        ((x) ^ ((y) | ~(z)))

/* the ten basic operations FF() through III() */
#define FF(a, b, c, d, e, x, s)        \
(a) += F((b), (c), (d)) + (x);\
(a) = CC_ROLc((a), (s)) + (e);\
(c) = CC_ROLc((c), 10);

#define GG(a, b, c, d, e, x, s)        \
(a) += G((b), (c), (d)) + (x) + 0x5a827999;\
(a) = CC_ROLc((a), (s)) + (e);\
(c) = CC_ROLc((c), 10);

#define HH(a, b, c, d, e, x, s)        \
(a) += H((b), (c), (d)) + (x) + 0x6ed9eba1;\
(a) = CC_ROLc((a), (s)) + (e);\
(c) = CC_ROLc((c), 10);

#define II(a, b, c, d, e, x, s)        \
(a) += I((b), (c), (d)) + (x) + 0x8f1bbcdc;\
(a) = CC_ROLc((a), (s)) + (e);\
(c) = CC_ROLc((c), 10);

#define JJ(a, b, c, d, e, x, s)        \
(a) += J((b), (c), (d)) + (x) + 0xa953fd4e;\
(a) = CC_ROLc((a), (s)) + (e);\
(c) = CC_ROLc((c), 10);

#define FFF(a, b, c, d, e, x, s)        \
(a) += F((b), (c), (d)) + (x);\
(a) = CC_ROLc((a), (s)) + (e);\
(c) = CC_ROLc((c), 10);

#define GGG(a, b, c, d, e, x, s)        \
(a) += G((b), (c), (d)) + (x) + 0x7a6d76e9;\
(a) = CC_ROLc((a), (s)) + (e);\
(c) = CC_ROLc((c), 10);

#define HHH(a, b, c, d, e, x, s)        \
(a) += H((b), (c), (d)) + (x) + 0x6d703ef3;\
(a) = CC_ROLc((a), (s)) + (e);\
(c) = CC_ROLc((c), 10);

#define III(a, b, c, d, e, x, s)        \
(a) += I((b), (c), (d)) + (x) + 0x5c4dd124;\
(a) = CC_ROLc((a), (s)) + (e);\
(c) = CC_ROLc((c), 10);

#define JJJ(a, b, c, d, e, x, s)        \
(a) += J((b), (c), (d)) + (x) + 0x50a28be6;\
(a) = CC_ROLc((a), (s)) + (e);\
(c) = CC_ROLc((c), 10);

static void rmd320_compress(ccdigest_state_t state, unsigned long nblocks, const void *in)
{
    uint32_t aa,bb,cc,dd,ee,aaa,bbb,ccc,ddd,eee,tmp,X[16];
    int i;
    uint32_t *s = ccdigest_u32(state);
    const unsigned char *buf = in;

    while(nblocks--) {

        /* load words X */
        for (i = 0; i < 16; i++){
            CC_LOAD32_LE(X[i], buf + (4 * i));
        }

        /* load state */
        aa = s[0];
        bb = s[1];
        cc = s[2];
        dd = s[3];
        ee = s[4];
        aaa = s[5];
        bbb = s[6];
        ccc = s[7];
        ddd = s[8];
        eee = s[9];

        /* round 1 */
        FF(aa, bb, cc, dd, ee, X[ 0], 11);
        FF(ee, aa, bb, cc, dd, X[ 1], 14);
        FF(dd, ee, aa, bb, cc, X[ 2], 15);
        FF(cc, dd, ee, aa, bb, X[ 3], 12);
        FF(bb, cc, dd, ee, aa, X[ 4],  5);
        FF(aa, bb, cc, dd, ee, X[ 5],  8);
        FF(ee, aa, bb, cc, dd, X[ 6],  7);
        FF(dd, ee, aa, bb, cc, X[ 7],  9);
        FF(cc, dd, ee, aa, bb, X[ 8], 11);
        FF(bb, cc, dd, ee, aa, X[ 9], 13);
        FF(aa, bb, cc, dd, ee, X[10], 14);
        FF(ee, aa, bb, cc, dd, X[11], 15);
        FF(dd, ee, aa, bb, cc, X[12],  6);
        FF(cc, dd, ee, aa, bb, X[13],  7);
        FF(bb, cc, dd, ee, aa, X[14],  9);
        FF(aa, bb, cc, dd, ee, X[15],  8);

        /* parallel round 1 */
        JJJ(aaa, bbb, ccc, ddd, eee, X[ 5],  8);
        JJJ(eee, aaa, bbb, ccc, ddd, X[14],  9);
        JJJ(ddd, eee, aaa, bbb, ccc, X[ 7],  9);
        JJJ(ccc, ddd, eee, aaa, bbb, X[ 0], 11);
        JJJ(bbb, ccc, ddd, eee, aaa, X[ 9], 13);
        JJJ(aaa, bbb, ccc, ddd, eee, X[ 2], 15);
        JJJ(eee, aaa, bbb, ccc, ddd, X[11], 15);
        JJJ(ddd, eee, aaa, bbb, ccc, X[ 4],  5);
        JJJ(ccc, ddd, eee, aaa, bbb, X[13],  7);
        JJJ(bbb, ccc, ddd, eee, aaa, X[ 6],  7);
        JJJ(aaa, bbb, ccc, ddd, eee, X[15],  8);
        JJJ(eee, aaa, bbb, ccc, ddd, X[ 8], 11);
        JJJ(ddd, eee, aaa, bbb, ccc, X[ 1], 14);
        JJJ(ccc, ddd, eee, aaa, bbb, X[10], 14);
        JJJ(bbb, ccc, ddd, eee, aaa, X[ 3], 12);
        JJJ(aaa, bbb, ccc, ddd, eee, X[12],  6);

        tmp = aa; aa = aaa; aaa = tmp;

        /* round 2 */
        GG(ee, aa, bb, cc, dd, X[ 7],  7);
        GG(dd, ee, aa, bb, cc, X[ 4],  6);
        GG(cc, dd, ee, aa, bb, X[13],  8);
        GG(bb, cc, dd, ee, aa, X[ 1], 13);
        GG(aa, bb, cc, dd, ee, X[10], 11);
        GG(ee, aa, bb, cc, dd, X[ 6],  9);
        GG(dd, ee, aa, bb, cc, X[15],  7);
        GG(cc, dd, ee, aa, bb, X[ 3], 15);
        GG(bb, cc, dd, ee, aa, X[12],  7);
        GG(aa, bb, cc, dd, ee, X[ 0], 12);
        GG(ee, aa, bb, cc, dd, X[ 9], 15);
        GG(dd, ee, aa, bb, cc, X[ 5],  9);
        GG(cc, dd, ee, aa, bb, X[ 2], 11);
        GG(bb, cc, dd, ee, aa, X[14],  7);
        GG(aa, bb, cc, dd, ee, X[11], 13);
        GG(ee, aa, bb, cc, dd, X[ 8], 12);

        /* parallel round 2 */
        III(eee, aaa, bbb, ccc, ddd, X[ 6],  9);
        III(ddd, eee, aaa, bbb, ccc, X[11], 13);
        III(ccc, ddd, eee, aaa, bbb, X[ 3], 15);
        III(bbb, ccc, ddd, eee, aaa, X[ 7],  7);
        III(aaa, bbb, ccc, ddd, eee, X[ 0], 12);
        III(eee, aaa, bbb, ccc, ddd, X[13],  8);
        III(ddd, eee, aaa, bbb, ccc, X[ 5],  9);
        III(ccc, ddd, eee, aaa, bbb, X[10], 11);
        III(bbb, ccc, ddd, eee, aaa, X[14],  7);
        III(aaa, bbb, ccc, ddd, eee, X[15],  7);
        III(eee, aaa, bbb, ccc, ddd, X[ 8], 12);
        III(ddd, eee, aaa, bbb, ccc, X[12],  7);
        III(ccc, ddd, eee, aaa, bbb, X[ 4],  6);
        III(bbb, ccc, ddd, eee, aaa, X[ 9], 15);
        III(aaa, bbb, ccc, ddd, eee, X[ 1], 13);
        III(eee, aaa, bbb, ccc, ddd, X[ 2], 11);

        tmp = bb; bb = bbb; bbb = tmp;

        /* round 3 */
        HH(dd, ee, aa, bb, cc, X[ 3], 11);
        HH(cc, dd, ee, aa, bb, X[10], 13);
        HH(bb, cc, dd, ee, aa, X[14],  6);
        HH(aa, bb, cc, dd, ee, X[ 4],  7);
        HH(ee, aa, bb, cc, dd, X[ 9], 14);
        HH(dd, ee, aa, bb, cc, X[15],  9);
        HH(cc, dd, ee, aa, bb, X[ 8], 13);
        HH(bb, cc, dd, ee, aa, X[ 1], 15);
        HH(aa, bb, cc, dd, ee, X[ 2], 14);
        HH(ee, aa, bb, cc, dd, X[ 7],  8);
        HH(dd, ee, aa, bb, cc, X[ 0], 13);
        HH(cc, dd, ee, aa, bb, X[ 6],  6);
        HH(bb, cc, dd, ee, aa, X[13],  5);
        HH(aa, bb, cc, dd, ee, X[11], 12);
        HH(ee, aa, bb, cc, dd, X[ 5],  7);
        HH(dd, ee, aa, bb, cc, X[12],  5);

        /* parallel round 3 */
        HHH(ddd, eee, aaa, bbb, ccc, X[15],  9);
        HHH(ccc, ddd, eee, aaa, bbb, X[ 5],  7);
        HHH(bbb, ccc, ddd, eee, aaa, X[ 1], 15);
        HHH(aaa, bbb, ccc, ddd, eee, X[ 3], 11);
        HHH(eee, aaa, bbb, ccc, ddd, X[ 7],  8);
        HHH(ddd, eee, aaa, bbb, ccc, X[14],  6);
        HHH(ccc, ddd, eee, aaa, bbb, X[ 6],  6);
        HHH(bbb, ccc, ddd, eee, aaa, X[ 9], 14);
        HHH(aaa, bbb, ccc, ddd, eee, X[11], 12);
        HHH(eee, aaa, bbb, ccc, ddd, X[ 8], 13);
        HHH(ddd, eee, aaa, bbb, ccc, X[12],  5);
        HHH(ccc, ddd, eee, aaa, bbb, X[ 2], 14);
        HHH(bbb, ccc, ddd, eee, aaa, X[10], 13);
        HHH(aaa, bbb, ccc, ddd, eee, X[ 0], 13);
        HHH(eee, aaa, bbb, ccc, ddd, X[ 4],  7);
        HHH(ddd, eee, aaa, bbb, ccc, X[13],  5);

        tmp = cc; cc = ccc; ccc = tmp;

        /* round 4 */
        II(cc, dd, ee, aa, bb, X[ 1], 11);
        II(bb, cc, dd, ee, aa, X[ 9], 12);
        II(aa, bb, cc, dd, ee, X[11], 14);
        II(ee, aa, bb, cc, dd, X[10], 15);
        II(dd, ee, aa, bb, cc, X[ 0], 14);
        II(cc, dd, ee, aa, bb, X[ 8], 15);
        II(bb, cc, dd, ee, aa, X[12],  9);
        II(aa, bb, cc, dd, ee, X[ 4],  8);
        II(ee, aa, bb, cc, dd, X[13],  9);
        II(dd, ee, aa, bb, cc, X[ 3], 14);
        II(cc, dd, ee, aa, bb, X[ 7],  5);
        II(bb, cc, dd, ee, aa, X[15],  6);
        II(aa, bb, cc, dd, ee, X[14],  8);
        II(ee, aa, bb, cc, dd, X[ 5],  6);
        II(dd, ee, aa, bb, cc, X[ 6],  5);
        II(cc, dd, ee, aa, bb, X[ 2], 12);

        /* parallel round 4 */
        GGG(ccc, ddd, eee, aaa, bbb, X[ 8], 15);
        GGG(bbb, ccc, ddd, eee, aaa, X[ 6],  5);
        GGG(aaa, bbb, ccc, ddd, eee, X[ 4],  8);
        GGG(eee, aaa, bbb, ccc, ddd, X[ 1], 11);
        GGG(ddd, eee, aaa, bbb, ccc, X[ 3], 14);
        GGG(ccc, ddd, eee, aaa, bbb, X[11], 14);
        GGG(bbb, ccc, ddd, eee, aaa, X[15],  6);
        GGG(aaa, bbb, ccc, ddd, eee, X[ 0], 14);
        GGG(eee, aaa, bbb, ccc, ddd, X[ 5],  6);
        GGG(ddd, eee, aaa, bbb, ccc, X[12],  9);
        GGG(ccc, ddd, eee, aaa, bbb, X[ 2], 12);
        GGG(bbb, ccc, ddd, eee, aaa, X[13],  9);
        GGG(aaa, bbb, ccc, ddd, eee, X[ 9], 12);
        GGG(eee, aaa, bbb, ccc, ddd, X[ 7],  5);
        GGG(ddd, eee, aaa, bbb, ccc, X[10], 15);
        GGG(ccc, ddd, eee, aaa, bbb, X[14],  8);

        tmp = dd; dd = ddd; ddd = tmp;

        /* round 5 */
        JJ(bb, cc, dd, ee, aa, X[ 4],  9);
        JJ(aa, bb, cc, dd, ee, X[ 0], 15);
        JJ(ee, aa, bb, cc, dd, X[ 5],  5);
        JJ(dd, ee, aa, bb, cc, X[ 9], 11);
        JJ(cc, dd, ee, aa, bb, X[ 7],  6);
        JJ(bb, cc, dd, ee, aa, X[12],  8);
        JJ(aa, bb, cc, dd, ee, X[ 2], 13);
        JJ(ee, aa, bb, cc, dd, X[10], 12);
        JJ(dd, ee, aa, bb, cc, X[14],  5);
        JJ(cc, dd, ee, aa, bb, X[ 1], 12);
        JJ(bb, cc, dd, ee, aa, X[ 3], 13);
        JJ(aa, bb, cc, dd, ee, X[ 8], 14);
        JJ(ee, aa, bb, cc, dd, X[11], 11);
        JJ(dd, ee, aa, bb, cc, X[ 6],  8);
        JJ(cc, dd, ee, aa, bb, X[15],  5);
        JJ(bb, cc, dd, ee, aa, X[13],  6);

        /* parallel round 5 */
        FFF(bbb, ccc, ddd, eee, aaa, X[12] ,  8);
        FFF(aaa, bbb, ccc, ddd, eee, X[15] ,  5);
        FFF(eee, aaa, bbb, ccc, ddd, X[10] , 12);
        FFF(ddd, eee, aaa, bbb, ccc, X[ 4] ,  9);
        FFF(ccc, ddd, eee, aaa, bbb, X[ 1] , 12);
        FFF(bbb, ccc, ddd, eee, aaa, X[ 5] ,  5);
        FFF(aaa, bbb, ccc, ddd, eee, X[ 8] , 14);
        FFF(eee, aaa, bbb, ccc, ddd, X[ 7] ,  6);
        FFF(ddd, eee, aaa, bbb, ccc, X[ 6] ,  8);
        FFF(ccc, ddd, eee, aaa, bbb, X[ 2] , 13);
        FFF(bbb, ccc, ddd, eee, aaa, X[13] ,  6);
        FFF(aaa, bbb, ccc, ddd, eee, X[14] ,  5);
        FFF(eee, aaa, bbb, ccc, ddd, X[ 0] , 15);
        FFF(ddd, eee, aaa, bbb, ccc, X[ 3] , 13);
        FFF(ccc, ddd, eee, aaa, bbb, X[ 9] , 11);
        FFF(bbb, ccc, ddd, eee, aaa, X[11] , 11);

        tmp = ee; ee = eee; eee = tmp;

        /* combine results */
        s[0] += aa;
        s[1] += bb;
        s[2] += cc;
        s[3] += dd;
        s[4] += ee;
        s[5] += aaa;
        s[6] += bbb;
        s[7] += ccc;
        s[8] += ddd;
        s[9] += eee;

        buf+=CCRMD_BLOCK_SIZE;
    }
}

static const uint32_t ccrmd320_initial_state[10] = {
    0x67452301,
    0xefcdab89,
    0x98badcfe,
    0x10325476,
    0xc3d2e1f0,
    0x76543210,
    0xfedcba98,
    0x89abcdef,
    0x01234567,
    0x3c2d1e0f,
};

const struct ccdigest_info ccrmd320_ltc_di = {
    .output_size = CCRMD320_OUTPUT_SIZE,
    .state_size = CCRMD320_STATE_SIZE,
    .block_size = CCRMD_BLOCK_SIZE,
    .oid_size = 0,
    .oid = CC_DIGEST_OID_RMD320,
    .initial_state = ccrmd320_initial_state,
    .compress = rmd320_compress,
    .final = ccdigest_final_64le,
};
