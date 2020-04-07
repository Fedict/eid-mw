/*
 * Copyright (c) 2011,2014,2015 Apple Inc. All rights reserved.
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


#include <corecrypto/cc_runtime_config.h>
#include "ccmode_internal.h"

#if defined(CCMODE_GCM_TABLES) || defined(LRW_TABLES) || defined(CCMODE_GCM_FAST)

/* this is x*2^128 mod p(x) ... the results are 16 bytes each stored in a packed format.  Since only the
 * lower 16 bits are not zero'ed I removed the upper 14 bytes */
const unsigned char gcm_shift_table[256*2] = {
    0x00, 0x00, 0x01, 0xc2, 0x03, 0x84, 0x02, 0x46, 0x07, 0x08, 0x06, 0xca, 0x04, 0x8c, 0x05, 0x4e,
    0x0e, 0x10, 0x0f, 0xd2, 0x0d, 0x94, 0x0c, 0x56, 0x09, 0x18, 0x08, 0xda, 0x0a, 0x9c, 0x0b, 0x5e,
    0x1c, 0x20, 0x1d, 0xe2, 0x1f, 0xa4, 0x1e, 0x66, 0x1b, 0x28, 0x1a, 0xea, 0x18, 0xac, 0x19, 0x6e,
    0x12, 0x30, 0x13, 0xf2, 0x11, 0xb4, 0x10, 0x76, 0x15, 0x38, 0x14, 0xfa, 0x16, 0xbc, 0x17, 0x7e,
    0x38, 0x40, 0x39, 0x82, 0x3b, 0xc4, 0x3a, 0x06, 0x3f, 0x48, 0x3e, 0x8a, 0x3c, 0xcc, 0x3d, 0x0e,
    0x36, 0x50, 0x37, 0x92, 0x35, 0xd4, 0x34, 0x16, 0x31, 0x58, 0x30, 0x9a, 0x32, 0xdc, 0x33, 0x1e,
    0x24, 0x60, 0x25, 0xa2, 0x27, 0xe4, 0x26, 0x26, 0x23, 0x68, 0x22, 0xaa, 0x20, 0xec, 0x21, 0x2e,
    0x2a, 0x70, 0x2b, 0xb2, 0x29, 0xf4, 0x28, 0x36, 0x2d, 0x78, 0x2c, 0xba, 0x2e, 0xfc, 0x2f, 0x3e,
    0x70, 0x80, 0x71, 0x42, 0x73, 0x04, 0x72, 0xc6, 0x77, 0x88, 0x76, 0x4a, 0x74, 0x0c, 0x75, 0xce,
    0x7e, 0x90, 0x7f, 0x52, 0x7d, 0x14, 0x7c, 0xd6, 0x79, 0x98, 0x78, 0x5a, 0x7a, 0x1c, 0x7b, 0xde,
    0x6c, 0xa0, 0x6d, 0x62, 0x6f, 0x24, 0x6e, 0xe6, 0x6b, 0xa8, 0x6a, 0x6a, 0x68, 0x2c, 0x69, 0xee,
    0x62, 0xb0, 0x63, 0x72, 0x61, 0x34, 0x60, 0xf6, 0x65, 0xb8, 0x64, 0x7a, 0x66, 0x3c, 0x67, 0xfe,
    0x48, 0xc0, 0x49, 0x02, 0x4b, 0x44, 0x4a, 0x86, 0x4f, 0xc8, 0x4e, 0x0a, 0x4c, 0x4c, 0x4d, 0x8e,
    0x46, 0xd0, 0x47, 0x12, 0x45, 0x54, 0x44, 0x96, 0x41, 0xd8, 0x40, 0x1a, 0x42, 0x5c, 0x43, 0x9e,
    0x54, 0xe0, 0x55, 0x22, 0x57, 0x64, 0x56, 0xa6, 0x53, 0xe8, 0x52, 0x2a, 0x50, 0x6c, 0x51, 0xae,
    0x5a, 0xf0, 0x5b, 0x32, 0x59, 0x74, 0x58, 0xb6, 0x5d, 0xf8, 0x5c, 0x3a, 0x5e, 0x7c, 0x5f, 0xbe,
    0xe1, 0x00, 0xe0, 0xc2, 0xe2, 0x84, 0xe3, 0x46, 0xe6, 0x08, 0xe7, 0xca, 0xe5, 0x8c, 0xe4, 0x4e,
    0xef, 0x10, 0xee, 0xd2, 0xec, 0x94, 0xed, 0x56, 0xe8, 0x18, 0xe9, 0xda, 0xeb, 0x9c, 0xea, 0x5e,
    0xfd, 0x20, 0xfc, 0xe2, 0xfe, 0xa4, 0xff, 0x66, 0xfa, 0x28, 0xfb, 0xea, 0xf9, 0xac, 0xf8, 0x6e,
    0xf3, 0x30, 0xf2, 0xf2, 0xf0, 0xb4, 0xf1, 0x76, 0xf4, 0x38, 0xf5, 0xfa, 0xf7, 0xbc, 0xf6, 0x7e,
    0xd9, 0x40, 0xd8, 0x82, 0xda, 0xc4, 0xdb, 0x06, 0xde, 0x48, 0xdf, 0x8a, 0xdd, 0xcc, 0xdc, 0x0e,
    0xd7, 0x50, 0xd6, 0x92, 0xd4, 0xd4, 0xd5, 0x16, 0xd0, 0x58, 0xd1, 0x9a, 0xd3, 0xdc, 0xd2, 0x1e,
    0xc5, 0x60, 0xc4, 0xa2, 0xc6, 0xe4, 0xc7, 0x26, 0xc2, 0x68, 0xc3, 0xaa, 0xc1, 0xec, 0xc0, 0x2e,
    0xcb, 0x70, 0xca, 0xb2, 0xc8, 0xf4, 0xc9, 0x36, 0xcc, 0x78, 0xcd, 0xba, 0xcf, 0xfc, 0xce, 0x3e,
    0x91, 0x80, 0x90, 0x42, 0x92, 0x04, 0x93, 0xc6, 0x96, 0x88, 0x97, 0x4a, 0x95, 0x0c, 0x94, 0xce,
    0x9f, 0x90, 0x9e, 0x52, 0x9c, 0x14, 0x9d, 0xd6, 0x98, 0x98, 0x99, 0x5a, 0x9b, 0x1c, 0x9a, 0xde,
    0x8d, 0xa0, 0x8c, 0x62, 0x8e, 0x24, 0x8f, 0xe6, 0x8a, 0xa8, 0x8b, 0x6a, 0x89, 0x2c, 0x88, 0xee,
    0x83, 0xb0, 0x82, 0x72, 0x80, 0x34, 0x81, 0xf6, 0x84, 0xb8, 0x85, 0x7a, 0x87, 0x3c, 0x86, 0xfe,
    0xa9, 0xc0, 0xa8, 0x02, 0xaa, 0x44, 0xab, 0x86, 0xae, 0xc8, 0xaf, 0x0a, 0xad, 0x4c, 0xac, 0x8e,
    0xa7, 0xd0, 0xa6, 0x12, 0xa4, 0x54, 0xa5, 0x96, 0xa0, 0xd8, 0xa1, 0x1a, 0xa3, 0x5c, 0xa2, 0x9e,
    0xb5, 0xe0, 0xb4, 0x22, 0xb6, 0x64, 0xb7, 0xa6, 0xb2, 0xe8, 0xb3, 0x2a, 0xb1, 0x6c, 0xb0, 0xae,
    0xbb, 0xf0, 0xba, 0x32, 0xb8, 0x74, 0xb9, 0xb6, 0xbc, 0xf8, 0xbd, 0x3a, 0xbf, 0x7c, 0xbe, 0xbe };

#endif

#ifndef CCMODE_GCM_FAST
/* right shift */
/* TODO: Check if we don't already have this function in the xts code somewhere. */
static void gcm_rightshift(unsigned char *a)
{
    int x;
    for (x = 15; x > 0; x--) {
        a[x] = (a[x]>>1) | ((a[x-1]<<7)&0x80);
    }
    a[0] >>= 1;
}

/* c = b*a */
static const unsigned char mask[] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
static const unsigned char poly[] = { 0x00, 0xE1 };


/*!
 GCM GF multiplier (internal use only)  bitserial
 @param a   First value
 @param b   Second value
 @param c   Destination for a * b
 */
void ccmode_gcm_gf_mult(const unsigned char *a, const unsigned char *b, unsigned char *c)
{
    unsigned char Z[16], V[16];
    unsigned x, y, z;

    cc_zero(16, Z);
    CC_MEMCPY(V, a, 16);
    for (x = 0; x < 128; x++) {
        if (b[x>>3] & mask[x&7]) {
            for (y = 0; y < 16; y++) {
                Z[y] ^= V[y];
            }
        }
        z     = V[15] & 0x01;
        gcm_rightshift(V);
        V[0] ^= poly[z];
    }
    CC_MEMCPY(c, Z, 16);
}

#else

/* map normal numbers to "ieee" way ... e.g. bit reversed */
#define M(x) ( ((x&8)>>3) | ((x&4)>>1) | ((x&2)<<1) | ((x&1)<<3) )

#define BPD (sizeof(CCMODE_GCM_FAST_TYPE) * 8)
#define WPV (1 + (16 / sizeof(CCMODE_GCM_FAST_TYPE)))

/*!
 GCM GF multiplier (internal use only)  word oriented
 @param a   First value
 @param b   Second value
 @param c   Destination for a * b
 */
void ccmode_gcm_gf_mult(const unsigned char *a, const unsigned char *b, unsigned char *c)
{
    int i, j, k, u;
    CCMODE_GCM_FAST_TYPE B[16][WPV], tmp[32 / sizeof(CCMODE_GCM_FAST_TYPE)], pB[16 / sizeof(CCMODE_GCM_FAST_TYPE)], zz, z;
    unsigned char pTmp[32];

    /* create simple tables */
    cc_zero(sizeof(B[0]), B[0]);
    cc_zero(sizeof(B[M(1)]), B[M(1)]);

#if CCN_UNIT_SIZE == 4
    for (i = 0; i < 4; i++) {
        CC_LOAD32_BE(B[M(1)][i], a + (i<<2));
        CC_LOAD32_LE(pB[i],      b + (i<<2));
    }
#elif CCN_UNIT_SIZE == 8
    for (i = 0; i < 2; i++) {
        CC_LOAD64_BE(B[M(1)][i], a + (i<<3));
        CC_LOAD64_LE(pB[i],      b + (i<<3));
    }
#else
#error unsupported CCN_UNIT_SIZE
#endif

    /* now create 2, 4 and 8 */
    B[M(2)][0] = B[M(1)][0] >> 1;
    B[M(4)][0] = B[M(1)][0] >> 2;
    B[M(8)][0] = B[M(1)][0] >> 3;
    for (i = 1; i < (int)WPV; i++) {
        B[M(2)][i] = (B[M(1)][i-1] << (BPD-1)) | (B[M(1)][i] >> 1);
        B[M(4)][i] = (B[M(1)][i-1] << (BPD-2)) | (B[M(1)][i] >> 2);
        B[M(8)][i] = (B[M(1)][i-1] << (BPD-3)) | (B[M(1)][i] >> 3);
    }

    /*  now all values with two bits which are 3, 5, 6, 9, 10, 12 */
    for (i = 0; i < (int)WPV; i++) {
        B[M(3)][i]  = B[M(1)][i] ^ B[M(2)][i];
        B[M(5)][i]  = B[M(1)][i] ^ B[M(4)][i];
        B[M(6)][i]  = B[M(2)][i] ^ B[M(4)][i];
        B[M(9)][i]  = B[M(1)][i] ^ B[M(8)][i];
        B[M(10)][i] = B[M(2)][i] ^ B[M(8)][i];
        B[M(12)][i] = B[M(8)][i] ^ B[M(4)][i];

        /*  now all 3 bit values and the only 4 bit value: 7, 11, 13, 14, 15 */
        B[M(7)][i]  = B[M(3)][i] ^ B[M(4)][i];
        B[M(11)][i] = B[M(3)][i] ^ B[M(8)][i];
        B[M(13)][i] = B[M(1)][i] ^ B[M(12)][i];
        B[M(14)][i] = B[M(6)][i] ^ B[M(8)][i];
        B[M(15)][i] = B[M(7)][i] ^ B[M(8)][i];
    }

    cc_zero(sizeof(tmp), tmp);

    /* compute product four bits of each word at a time */
    /* for each nibble */
    for (i = (BPD/4)-1; i >= 0; i--) {
        /* for each word */
        for (j = 0; j < (int)(WPV-1); j++) {
            /* grab the 4 bits recall the nibbles are backwards so it's a shift by (i^1)*4 */
            u = (pB[j] >> ((i^1)<<2)) & 15;

            /* add offset by the word count the table looked up value to the result */
            for (k = 0; k < (int)WPV; k++) {
                tmp[k+j] ^= B[u][k];
            }
        }
        /* shift result up by 4 bits */
        if (i != 0) {
            for (z = j = 0; j < (int)(32 / sizeof(CCMODE_GCM_FAST_TYPE)); j++) {
                zz = tmp[j] << (BPD-4);
                tmp[j] = (tmp[j] >> 4) | z;
                z = zz;
            }
        }
    }

    /* store product */
#if CCN_UNIT_SIZE == 4
    for (i = 0; i < 8; i++) {
        CC_STORE32_BE(tmp[i], pTmp + (i<<2));
    }
#elif CCN_UNIT_SIZE == 8
    for (i = 0; i < 4; i++) {
        CC_STORE64_BE(tmp[i], pTmp + (i<<3));
    }
#else
#error unsupported CCN_UNIT_SIZE
#endif

    /* reduce by taking most significant byte and adding the appropriate two byte sequence 16 bytes down */
    for (i = 31; i >= 16; i--) {
        pTmp[i-16] ^= gcm_shift_table[((unsigned)pTmp[i]<<1)];
        pTmp[i-15] ^= gcm_shift_table[((unsigned)pTmp[i]<<1)+1];
    }

    for (i = 0; i < 16; i++) {
        c[i] = pTmp[i];
    }
}

#endif
