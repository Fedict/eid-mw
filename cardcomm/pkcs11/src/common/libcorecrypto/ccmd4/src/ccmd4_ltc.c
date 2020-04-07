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
#include <corecrypto/ccmd4.h>
#include <corecrypto/cc_priv.h>
#include <corecrypto/ccdigest_priv.h>

#define S11 3
#define S12 7
#define S13 11
#define S14 19
#define S21 3
#define S22 5
#define S23 9
#define S24 13
#define S31 3
#define S32 9
#define S33 11
#define S34 15

/* F, G and H are basic LTC_MD4 functions. */
#define F(x, y, z) (z ^ (x & (y ^ z)))
#define G(x, y, z) ((x & y) | (z & (x | y)))
#define H(x, y, z) ((x) ^ (y) ^ (z))

/* ROTATE_LEFT rotates x left n bits. */
#define ROTATE_LEFT(x, n) CC_ROLc(x, n)

/* FF, GG and HH are transformations for rounds 1, 2 and 3 */
/* Rotation is separate from addition to prevent recomputation */

#define FF(a, b, c, d, x, s) { \
    (a) += F ((b), (c), (d)) + (x); \
    (a) = ROTATE_LEFT ((a), (s)); \
  }
#define GG(a, b, c, d, x, s) { \
    (a) += G ((b), (c), (d)) + (x) + 0x5a827999; \
    (a) = ROTATE_LEFT ((a), (s)); \
  }
#define HH(a, b, c, d, x, s) { \
    (a) += H ((b), (c), (d)) + (x) + 0x6ed9eba1; \
    (a) = ROTATE_LEFT ((a), (s)); \
  }

static void md4_compress(ccdigest_state_t state, unsigned long nblocks, const void *in)
{
    uint32_t x[16], a, b, c, d;
    int i;
    uint32_t *s = ccdigest_u32(state);
    const unsigned char *buf = in;

    while(nblocks--) {
        /* copy state */
        a = s[0];
        b = s[1];
        c = s[2];
        d = s[3];

        /* copy the state into 512-bits into W[0..15] */
        for (i = 0; i < 16; i++) {
            CC_LOAD32_LE(x[i], buf + (4*i));
        }

        /* Round 1 */
        FF (a, b, c, d, x[ 0], S11); /* 1 */
        FF (d, a, b, c, x[ 1], S12); /* 2 */
        FF (c, d, a, b, x[ 2], S13); /* 3 */
        FF (b, c, d, a, x[ 3], S14); /* 4 */
        FF (a, b, c, d, x[ 4], S11); /* 5 */
        FF (d, a, b, c, x[ 5], S12); /* 6 */
        FF (c, d, a, b, x[ 6], S13); /* 7 */
        FF (b, c, d, a, x[ 7], S14); /* 8 */
        FF (a, b, c, d, x[ 8], S11); /* 9 */
        FF (d, a, b, c, x[ 9], S12); /* 10 */
        FF (c, d, a, b, x[10], S13); /* 11 */
        FF (b, c, d, a, x[11], S14); /* 12 */
        FF (a, b, c, d, x[12], S11); /* 13 */
        FF (d, a, b, c, x[13], S12); /* 14 */
        FF (c, d, a, b, x[14], S13); /* 15 */
        FF (b, c, d, a, x[15], S14); /* 16 */

        /* Round 2 */
        GG (a, b, c, d, x[ 0], S21); /* 17 */
        GG (d, a, b, c, x[ 4], S22); /* 18 */
        GG (c, d, a, b, x[ 8], S23); /* 19 */
        GG (b, c, d, a, x[12], S24); /* 20 */
        GG (a, b, c, d, x[ 1], S21); /* 21 */
        GG (d, a, b, c, x[ 5], S22); /* 22 */
        GG (c, d, a, b, x[ 9], S23); /* 23 */
        GG (b, c, d, a, x[13], S24); /* 24 */
        GG (a, b, c, d, x[ 2], S21); /* 25 */
        GG (d, a, b, c, x[ 6], S22); /* 26 */
        GG (c, d, a, b, x[10], S23); /* 27 */
        GG (b, c, d, a, x[14], S24); /* 28 */
        GG (a, b, c, d, x[ 3], S21); /* 29 */
        GG (d, a, b, c, x[ 7], S22); /* 30 */
        GG (c, d, a, b, x[11], S23); /* 31 */
        GG (b, c, d, a, x[15], S24); /* 32 */

        /* Round 3 */
        HH (a, b, c, d, x[ 0], S31); /* 33 */
        HH (d, a, b, c, x[ 8], S32); /* 34 */
        HH (c, d, a, b, x[ 4], S33); /* 35 */
        HH (b, c, d, a, x[12], S34); /* 36 */
        HH (a, b, c, d, x[ 2], S31); /* 37 */
        HH (d, a, b, c, x[10], S32); /* 38 */
        HH (c, d, a, b, x[ 6], S33); /* 39 */
        HH (b, c, d, a, x[14], S34); /* 40 */
        HH (a, b, c, d, x[ 1], S31); /* 41 */
        HH (d, a, b, c, x[ 9], S32); /* 42 */
        HH (c, d, a, b, x[ 5], S33); /* 43 */
        HH (b, c, d, a, x[13], S34); /* 44 */
        HH (a, b, c, d, x[ 3], S31); /* 45 */
        HH (d, a, b, c, x[11], S32); /* 46 */
        HH (c, d, a, b, x[ 7], S33); /* 47 */
        HH (b, c, d, a, x[15], S34); /* 48 */


        /* Update our state */
        s[0] = s[0] + a;
        s[1] = s[1] + b;
        s[2] = s[2] + c;
        s[3] = s[3] + d;

        buf+=CCMD4_BLOCK_SIZE;
    }
}

const struct ccdigest_info ccmd4_ltc_di = {
    .output_size = CCMD4_OUTPUT_SIZE,
    .state_size = CCMD4_STATE_SIZE,
    .block_size = CCMD4_BLOCK_SIZE,
    .oid_size = 10,
    .oid = CC_DIGEST_OID_MD4,
    .initial_state = ccmd4_initial_state,
    .compress = md4_compress,
    .final = ccdigest_final_64le,
};
