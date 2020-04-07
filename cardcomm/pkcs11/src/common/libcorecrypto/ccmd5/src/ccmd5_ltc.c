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
#include <corecrypto/ccmd5.h>
#include <corecrypto/cc_priv.h>
#include <corecrypto/ccdigest_priv.h>

#define F(x,y,z)  (z ^ (x & (y ^ z)))
#define G(x,y,z)  (y ^ (z & (y ^ x)))
#define H(x,y,z)  (x^y^z)
#define I(x,y,z)  (y^(x|(~z)))

#ifdef CC_SMALL_CODE

#define FF(a,b,c,d,M,s,t) \
a = (a + F(b,c,d) + M + t); a = CC_ROL(a, s) + b;

#define GG(a,b,c,d,M,s,t) \
a = (a + G(b,c,d) + M + t); a = CC_ROL(a, s) + b;

#define HH(a,b,c,d,M,s,t) \
a = (a + H(b,c,d) + M + t); a = CC_ROL(a, s) + b;

#define II(a,b,c,d,M,s,t) \
a = (a + I(b,c,d) + M + t); a = CC_ROL(a, s) + b;

static const unsigned char Worder[64] = {
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
    1,6,11,0,5,10,15,4,9,14,3,8,13,2,7,12,
    5,8,11,14,1,4,7,10,13,0,3,6,9,12,15,2,
    0,7,14,5,12,3,10,1,8,15,6,13,4,11,2,9
};

static const unsigned char Rorder[64] = {
    7,12,17,22,7,12,17,22,7,12,17,22,7,12,17,22,
    5,9,14,20,5,9,14,20,5,9,14,20,5,9,14,20,
    4,11,16,23,4,11,16,23,4,11,16,23,4,11,16,23,
    6,10,15,21,6,10,15,21,6,10,15,21,6,10,15,21
};

static const uint32_t Korder[64] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

#else

#define FF(a,b,c,d,M,s,t) \
a = (a + F(b,c,d) + M + t); a = CC_ROLc(a, s) + b;

#define GG(a,b,c,d,M,s,t) \
a = (a + G(b,c,d) + M + t); a = CC_ROLc(a, s) + b;

#define HH(a,b,c,d,M,s,t) \
a = (a + H(b,c,d) + M + t); a = CC_ROLc(a, s) + b;

#define II(a,b,c,d,M,s,t) \
a = (a + I(b,c,d) + M + t); a = CC_ROLc(a, s) + b;


#endif


static void md5_compress(ccdigest_state_t state, unsigned long nblocks, const void *in)
{
    uint32_t i, W[16], a, b, c, d;
#ifdef CC_SMALL_CODE
    uint32_t t;
#endif
    uint32_t *s = ccdigest_u32(state);
    const unsigned char *buf = in;

    while(nblocks--) {

        /* copy the state into 512-bits into W[0..15] */
        for (i = 0; i < 16; i++) {
            CC_LOAD32_LE(W[i], buf + (4*i));
        }

        /* copy state */
        a = s[0];
        b = s[1];
        c = s[2];
        d = s[3];

    #ifdef CC_SMALL_CODE
        for (i = 0; i < 16; ++i) {
            FF(a,b,c,d,W[Worder[i]],Rorder[i],Korder[i]);
            t = d; d = c; c = b; b = a; a = t;
        }

        for (; i < 32; ++i) {
            GG(a,b,c,d,W[Worder[i]],Rorder[i],Korder[i]);
            t = d; d = c; c = b; b = a; a = t;
        }

        for (; i < 48; ++i) {
            HH(a,b,c,d,W[Worder[i]],Rorder[i],Korder[i]);
            t = d; d = c; c = b; b = a; a = t;
        }

        for (; i < 64; ++i) {
            II(a,b,c,d,W[Worder[i]],Rorder[i],Korder[i]);
            t = d; d = c; c = b; b = a; a = t;
        }

    #else
        FF(a,b,c,d,W[0],7,0xd76aa478)
        FF(d,a,b,c,W[1],12,0xe8c7b756)
        FF(c,d,a,b,W[2],17,0x242070db)
        FF(b,c,d,a,W[3],22,0xc1bdceee)
        FF(a,b,c,d,W[4],7,0xf57c0faf)
        FF(d,a,b,c,W[5],12,0x4787c62a)
        FF(c,d,a,b,W[6],17,0xa8304613)
        FF(b,c,d,a,W[7],22,0xfd469501)
        FF(a,b,c,d,W[8],7,0x698098d8)
        FF(d,a,b,c,W[9],12,0x8b44f7af)
        FF(c,d,a,b,W[10],17,0xffff5bb1)
        FF(b,c,d,a,W[11],22,0x895cd7be)
        FF(a,b,c,d,W[12],7,0x6b901122)
        FF(d,a,b,c,W[13],12,0xfd987193)
        FF(c,d,a,b,W[14],17,0xa679438e)
        FF(b,c,d,a,W[15],22,0x49b40821)
        GG(a,b,c,d,W[1],5,0xf61e2562)
        GG(d,a,b,c,W[6],9,0xc040b340)
        GG(c,d,a,b,W[11],14,0x265e5a51)
        GG(b,c,d,a,W[0],20,0xe9b6c7aa)
        GG(a,b,c,d,W[5],5,0xd62f105d)
        GG(d,a,b,c,W[10],9,0x02441453)
        GG(c,d,a,b,W[15],14,0xd8a1e681)
        GG(b,c,d,a,W[4],20,0xe7d3fbc8)
        GG(a,b,c,d,W[9],5,0x21e1cde6)
        GG(d,a,b,c,W[14],9,0xc33707d6)
        GG(c,d,a,b,W[3],14,0xf4d50d87)
        GG(b,c,d,a,W[8],20,0x455a14ed)
        GG(a,b,c,d,W[13],5,0xa9e3e905)
        GG(d,a,b,c,W[2],9,0xfcefa3f8)
        GG(c,d,a,b,W[7],14,0x676f02d9)
        GG(b,c,d,a,W[12],20,0x8d2a4c8a)
        HH(a,b,c,d,W[5],4,0xfffa3942)
        HH(d,a,b,c,W[8],11,0x8771f681)
        HH(c,d,a,b,W[11],16,0x6d9d6122)
        HH(b,c,d,a,W[14],23,0xfde5380c)
        HH(a,b,c,d,W[1],4,0xa4beea44)
        HH(d,a,b,c,W[4],11,0x4bdecfa9)
        HH(c,d,a,b,W[7],16,0xf6bb4b60)
        HH(b,c,d,a,W[10],23,0xbebfbc70)
        HH(a,b,c,d,W[13],4,0x289b7ec6)
        HH(d,a,b,c,W[0],11,0xeaa127fa)
        HH(c,d,a,b,W[3],16,0xd4ef3085)
        HH(b,c,d,a,W[6],23,0x04881d05)
        HH(a,b,c,d,W[9],4,0xd9d4d039)
        HH(d,a,b,c,W[12],11,0xe6db99e5)
        HH(c,d,a,b,W[15],16,0x1fa27cf8)
        HH(b,c,d,a,W[2],23,0xc4ac5665)
        II(a,b,c,d,W[0],6,0xf4292244)
        II(d,a,b,c,W[7],10,0x432aff97)
        II(c,d,a,b,W[14],15,0xab9423a7)
        II(b,c,d,a,W[5],21,0xfc93a039)
        II(a,b,c,d,W[12],6,0x655b59c3)
        II(d,a,b,c,W[3],10,0x8f0ccc92)
        II(c,d,a,b,W[10],15,0xffeff47d)
        II(b,c,d,a,W[1],21,0x85845dd1)
        II(a,b,c,d,W[8],6,0x6fa87e4f)
        II(d,a,b,c,W[15],10,0xfe2ce6e0)
        II(c,d,a,b,W[6],15,0xa3014314)
        II(b,c,d,a,W[13],21,0x4e0811a1)
        II(a,b,c,d,W[4],6,0xf7537e82)
        II(d,a,b,c,W[11],10,0xbd3af235)
        II(c,d,a,b,W[2],15,0x2ad7d2bb)
        II(b,c,d,a,W[9],21,0xeb86d391)
    #endif

        /* store state */
        s[0] += a;
        s[1] += b;
        s[2] += c;
        s[3] += d;

        buf+=CCMD5_BLOCK_SIZE;
    }
}

const struct ccdigest_info ccmd5_ltc_di = {
    .output_size = CCMD5_OUTPUT_SIZE,
    .state_size = CCMD5_STATE_SIZE,
    .block_size = CCMD5_BLOCK_SIZE,
    .oid_size = 10,
    .oid = CC_DIGEST_OID_MD5,
    .initial_state = ccmd4_initial_state,
    .compress = md5_compress,
    .final = ccdigest_final_64le,
};
