/*
 * Copyright (c) 2010,2011,2015 Apple Inc. All rights reserved.
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

#include <corecrypto/ccsha2.h>
#include <corecrypto/cc_priv.h>
#include "ccsha2_internal.h"

/* the K array */
static const uint64_t K[80] = {
0x428a2f98d728ae22, 0x7137449123ef65cd,
0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc,
0x3956c25bf348b538, 0x59f111f1b605d019,
0x923f82a4af194f9b, 0xab1c5ed5da6d8118,
0xd807aa98a3030242, 0x12835b0145706fbe,
0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
0x72be5d74f27b896f, 0x80deb1fe3b1696b1,
0x9bdc06a725c71235, 0xc19bf174cf692694,
0xe49b69c19ef14ad2, 0xefbe4786384f25e3,
0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
0x2de92c6f592b0275, 0x4a7484aa6ea6e483,
0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
0x983e5152ee66dfab, 0xa831c66d2db43210,
0xb00327c898fb213f, 0xbf597fc7beef0ee4,
0xc6e00bf33da88fc2, 0xd5a79147930aa725,
0x06ca6351e003826f, 0x142929670a0e6e70,
0x27b70a8546d22ffc, 0x2e1b21385c26c926,
0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
0x650a73548baf63de, 0x766a0abb3c77b2a8,
0x81c2c92e47edaee6, 0x92722c851482353b,
0xa2bfe8a14cf10364, 0xa81a664bbc423001,
0xc24b8b70d0f89791, 0xc76c51a30654be30,
0xd192e819d6ef5218, 0xd69906245565a910,
0xf40e35855771202a, 0x106aa07032bbd1b8,
0x19a4c116b8d2d0c8, 0x1e376c085141ab53,
0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8,
0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb,
0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3,
0x748f82ee5defb2fc, 0x78a5636f43172f60,
0x84c87814a1f0ab72, 0x8cc702081a6439ec,
0x90befffa23631e28, 0xa4506cebde82bde9,
0xbef9a3f7b2c67915, 0xc67178f2e372532b,
0xca273eceea26619c, 0xd186b8c721c0c207,
0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178,
0x06f067aa72176fba, 0x0a637dc5a2c898a6,
0x113f9804bef90dae, 0x1b710b35131c471b,
0x28db77f523047d84, 0x32caab7b40c72493,
0x3c9ebe0a15c9bebc, 0x431d67c49c100d4c,
0x4cc5d4becb3e42b6, 0x597f299cfc657e2a,
0x5fcb6fab3ad6faec, 0x6c44198c4a475817
};

/* Various logical functions */
#define Ch(x,y,z)       (z ^ (x & (y ^ z)))
#define Maj(x,y,z)      (((x | y) & z) | (x & y))
#define S(x, n)         CC_ROR64c(x, n)
#define R(x, n)         (((x) & 0xFFFFFFFFFFFFFFFF )>>((uint64_t)n))
#define Sigma0(x)       (S(x, 28) ^ S(x, 34) ^ S(x, 39))
#define Sigma1(x)       (S(x, 14) ^ S(x, 18) ^ S(x, 41))
#define Gamma0(x)       (S(x, 1) ^ S(x, 8) ^ R(x, 7))
#define Gamma1(x)       (S(x, 19) ^ S(x, 61) ^ R(x, 6))

/* compress 1024-bits */
void ccsha512_ltc_compress(ccdigest_state_t state, unsigned long nblocks, const void *in)
{
    uint64_t S[8], W[80], t0, t1;
    int i;
    uint64_t *s = ccdigest_u64(state);
    const unsigned char *buf = in;

    while(nblocks--) {
        /* copy state into S */
        for (i = 0; i < 8; i++) {
            S[i] = s[i];
        }

        /* copy the state into 1024-bits into W[0..15] */
        for (i = 0; i < 16; i++) {
            CC_LOAD64_BE(W[i], buf + (8*i));
        }

        /* fill W[16..79] */
        for (i = 16; i < 80; i++) {
            W[i] = Gamma1(W[i - 2]) + W[i - 7] + Gamma0(W[i - 15]) + W[i - 16];
        }

        /* Compress */
    #ifdef CC_SMALL_CODE
        for (i = 0; i < 80; i++) {
            t0 = S[7] + Sigma1(S[4]) + Ch(S[4], S[5], S[6]) + K[i] + W[i];
            t1 = Sigma0(S[0]) + Maj(S[0], S[1], S[2]);
            S[7] = S[6];
            S[6] = S[5];
            S[5] = S[4];
            S[4] = S[3] + t0;
            S[3] = S[2];
            S[2] = S[1];
            S[1] = S[0];
            S[0] = t0 + t1;
        }
    #else
    #define RND(a,b,c,d,e,f,g,h,i)                    \
         t0 = h + Sigma1(e) + Ch(e, f, g) + K[i] + W[i];   \
         t1 = Sigma0(a) + Maj(a, b, c);                  \
         d += t0;                                        \
         h  = t0 + t1;

         for (i = 0; i < 80; i += 8) {
             RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],i+0);
             RND(S[7],S[0],S[1],S[2],S[3],S[4],S[5],S[6],i+1);
             RND(S[6],S[7],S[0],S[1],S[2],S[3],S[4],S[5],i+2);
             RND(S[5],S[6],S[7],S[0],S[1],S[2],S[3],S[4],i+3);
             RND(S[4],S[5],S[6],S[7],S[0],S[1],S[2],S[3],i+4);
             RND(S[3],S[4],S[5],S[6],S[7],S[0],S[1],S[2],i+5);
             RND(S[2],S[3],S[4],S[5],S[6],S[7],S[0],S[1],i+6);
             RND(S[1],S[2],S[3],S[4],S[5],S[6],S[7],S[0],i+7);
         }
    #endif


        /* feedback */
        for (i = 0; i < 8; i++) {
            s[i] = s[i] + S[i];
        }

        buf+=CCSHA512_BLOCK_SIZE;
    }
}

void ccsha512_final(const struct ccdigest_info *di, ccdigest_ctx_t ctx,
                    unsigned char *digest) {
    ccdigest_nbits(di, ctx) += ccdigest_num(di, ctx) << 3;
    ccdigest_data(di, ctx)[ccdigest_num(di, ctx)++] = 0x80;

    /* If we don't have at least 16 bytes (for the length) left we need to add
       a second block. */
    if (ccdigest_num(di, ctx) > di->block_size - 16) {
        while (ccdigest_num(di, ctx) < di->block_size) {
            ccdigest_data(di, ctx)[ccdigest_num(di, ctx)++] = 0;
        }
        di->compress((ccdigest_state_t)ccdigest_state(di, ctx), 1, ccdigest_data(di, ctx));
        ccdigest_num(di, ctx) = 0;
    }

    /* Pad up to block_size minus 8 with 0s */
    while (ccdigest_num(di, ctx) < di->block_size - 8) {
        ccdigest_data(di, ctx)[ccdigest_num(di, ctx)++] = 0;
    }

    CC_STORE64_BE(ccdigest_nbits(di, ctx), ccdigest_data(di, ctx) + di->block_size - 8);
    di->compress((ccdigest_state_t)ccdigest_state(di, ctx), 1, ccdigest_data(di, ctx));

    /* Copy output */
    for (unsigned int i = 0; i < di->output_size / 8; i++) {
        CC_STORE64_BE(ccdigest_state_u64(di, ctx)[i], digest+(8*i));
    }
}
