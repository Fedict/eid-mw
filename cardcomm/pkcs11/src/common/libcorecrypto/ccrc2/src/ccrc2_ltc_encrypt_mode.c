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


#include <corecrypto/ccrc2.h>
#include "ltc_rc2.h"

/**********************************************************************\
 * Encrypt an 8-byte block of plaintext using the given key.            *
 \**********************************************************************/
/*!
 Encrypts a block of text with LTC_RC2
 @param pt The input plaintext (8 bytes)
 @param ct The output ciphertext (8 bytes)
 @param skey The key as scheduled
 */

static void ltc_rc2_ecb_encrypt(const ccecb_ctx *skey, unsigned long nblocks, const void *in, void *out)
{
    const uint32_t *xkey;
    uint32_t x76, x54, x32, x10, i;
    const ltc_rc2_keysched *rc2;
    const unsigned char *pt=in;
    unsigned char *ct=out;

    rc2 = (const ltc_rc2_keysched *)skey;
    xkey = rc2->xkey;

    while(nblocks--) {

        x76 = ((uint32_t)pt[7] << 8) + (uint32_t)pt[6];
        x54 = ((uint32_t)pt[5] << 8) + (uint32_t)pt[4];
        x32 = ((uint32_t)pt[3] << 8) + (uint32_t)pt[2];
        x10 = ((uint32_t)pt[1] << 8) + (uint32_t)pt[0];

        for (i = 0; i < 16; i++) {
            x10 = (x10 + (x32 & ~x76) + (x54 & x76) + xkey[4*i+0]) & 0xFFFF;
            x10 = ((x10 << 1) | (x10 >> 15));

            x32 = (x32 + (x54 & ~x10) + (x76 & x10) + xkey[4*i+1]) & 0xFFFF;
            x32 = ((x32 << 2) | (x32 >> 14));

            x54 = (x54 + (x76 & ~x32) + (x10 & x32) + xkey[4*i+2]) & 0xFFFF;
            x54 = ((x54 << 3) | (x54 >> 13));

            x76 = (x76 + (x10 & ~x54) + (x32 & x54) + xkey[4*i+3]) & 0xFFFF;
            x76 = ((x76 << 5) | (x76 >> 11));

            if (i == 4 || i == 10) {
                x10 = (x10 + xkey[x76 & 63]) & 0xFFFF;
                x32 = (x32 + xkey[x10 & 63]) & 0xFFFF;
                x54 = (x54 + xkey[x32 & 63]) & 0xFFFF;
                x76 = (x76 + xkey[x54 & 63]) & 0xFFFF;
            }
        }

        ct[0] = (unsigned char)x10;
        ct[1] = (unsigned char)(x10 >> 8);
        ct[2] = (unsigned char)x32;
        ct[3] = (unsigned char)(x32 >> 8);
        ct[4] = (unsigned char)x54;
        ct[5] = (unsigned char)(x54 >> 8);
        ct[6] = (unsigned char)x76;
        ct[7] = (unsigned char)(x76 >> 8);

        pt+=CCRC2_BLOCK_SIZE;
        ct+=CCRC2_BLOCK_SIZE;
    }
}

#include <corecrypto/cc_priv.h>

const struct ccmode_ecb ccrc2_ltc_ecb_encrypt_mode = {
    .size = sizeof(ltc_rc2_keysched),
    .block_size = CCRC2_BLOCK_SIZE,
    .init = ccrc2_ltc_setup,
    .ecb = ltc_rc2_ecb_encrypt,
};
