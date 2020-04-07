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


/* LibTomCrypt, modular cryptographic library -- Tom St Denis
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
#include <corecrypto/ccmd2.h>
#include <corecrypto/cc_priv.h>


static const unsigned char PI_SUBST[256] = {
    41, 46, 67, 201, 162, 216, 124, 1, 61, 54, 84, 161, 236, 240, 6,
    19, 98, 167, 5, 243, 192, 199, 115, 140, 152, 147, 43, 217, 188,
    76, 130, 202, 30, 155, 87, 60, 253, 212, 224, 22, 103, 66, 111, 24,
    138, 23, 229, 18, 190, 78, 196, 214, 218, 158, 222, 73, 160, 251,
    245, 142, 187, 47, 238, 122, 169, 104, 121, 145, 21, 178, 7, 63,
    148, 194, 16, 137, 11, 34, 95, 33, 128, 127, 93, 154, 90, 144, 50,
    39, 53, 62, 204, 231, 191, 247, 151, 3, 255, 25, 48, 179, 72, 165,
    181, 209, 215, 94, 146, 42, 172, 86, 170, 198, 79, 184, 56, 210,
    150, 164, 125, 182, 118, 252, 107, 226, 156, 116, 4, 241, 69, 157,
    112, 89, 100, 113, 135, 32, 134, 91, 207, 101, 230, 45, 168, 2, 27,
    96, 37, 173, 174, 176, 185, 246, 28, 70, 97, 105, 52, 64, 126, 15,
    85, 71, 163, 35, 221, 81, 175, 58, 195, 92, 249, 206, 186, 197,
    234, 38, 44, 83, 13, 110, 133, 40, 132, 9, 211, 223, 205, 244, 65,
    129, 77, 82, 106, 220, 55, 200, 108, 193, 171, 250, 36, 225, 123,
    8, 12, 189, 177, 74, 120, 136, 149, 139, 227, 99, 232, 109, 233,
    203, 213, 254, 59, 0, 29, 57, 242, 239, 183, 14, 102, 88, 208, 228,
    166, 119, 114, 248, 235, 117, 75, 10, 49, 68, 80, 180, 143, 237,
    31, 26, 219, 153, 141, 51, 159, 17, 131, 20
};

/* adds 16 bytes to the checksum */
static void md2_update_chksum(unsigned char *chksum, const unsigned char *buf)
{
    int j;
    unsigned char L;

    L = chksum[15];
    for (j = 0; j < 16; j++) {
        /* caution, the RFC says its "C[j] = S[M[i*16+j] xor L]" but the
         * reference source code [and test vectors] say otherwise.
         */
        L = (chksum[j] ^= PI_SUBST[(buf[j] ^ L)]);
    }
}

static void md2_compress(unsigned char *X, const unsigned char *buf)
{
    int j, k;
    unsigned char t;

    /* copy block */
    for (j = 0; j < 16; j++) {
        X[16+j] = buf[j];
        X[32+j] = X[j] ^ X[16+j];
    }

    t = (unsigned char)0;

    /* do 18 rounds */
    for (j = 0; j < 18; j++) {
        for (k = 0; k < 48; k++) {
            t = (X[k] ^= PI_SUBST[t]);
        }
        t = (t + (unsigned char)j) & 255;
    }
}

static void md2_processblock(ccdigest_state_t state, unsigned long nblocks, const void *in)
{
    unsigned char *X=ccdigest_u8(state);
    unsigned char *chksum=X+48;
    const unsigned char *buf = in;

    while (nblocks--) {
        md2_compress(X, buf);
        md2_update_chksum(chksum, buf);
        buf+=CCMD2_BLOCK_SIZE;
    }
}


static void md2_final(const struct ccdigest_info *di, ccdigest_ctx_t ctx,
                      unsigned char *out)
{
    unsigned long i, k;

    unsigned char *X=ccdigest_state_u8(di, ctx);
    unsigned char *chksum=X+48;

    /* pad the message */
    k = 16 - ccdigest_num(di, ctx);
    for (i = ccdigest_num(di, ctx); i < 16; i++) {
        ccdigest_data(di, ctx)[i] = (unsigned char)k;
    }

    /* hash and update */
    md2_compress(X, ccdigest_data(di, ctx));
    md2_update_chksum(chksum, ccdigest_data(di, ctx));

    /* hash checksum */
    CC_MEMCPY(ccdigest_data(di, ctx), chksum, 16);
    md2_compress(X, ccdigest_data(di, ctx));

    /* output is lower 16 bytes of X */
    CC_MEMCPY(out, X, 16);
}

/* MD2 initial state is zero */
const uint32_t ccmd2_initial_state[] = {
    0, 0, 0 , 0,
    0, 0, 0 , 0,
    0, 0, 0 , 0,
    0, 0, 0 , 0,
};


const struct ccdigest_info ccmd2_ltc_di = {
    .output_size = CCMD2_OUTPUT_SIZE,
    .state_size = CCMD2_STATE_SIZE,
    .block_size = CCMD2_BLOCK_SIZE,
    .oid_size = 10,
    .oid = CC_DIGEST_OID_MD2,
    .initial_state = ccmd2_initial_state,
    .compress = md2_processblock,
    .final = md2_final
};
