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

#include <corecrypto/ccblowfish.h>
#include <corecrypto/cc_priv.h>
#include "ltc_blowfish.h"

/*!
 Decrypts a block of text with Blowfish
 @param ct The input ciphertext (8 bytes)
 @param pt The output plaintext (8 bytes)
 @param skey The key as scheduled 
 */
#ifdef LTC_CLEAN_STACK
static void _ltc_blowfish_ecb_decrypt(const ccecb_ctx *skey, unsigned long nblocks, const void *in, void *out)
#else
static void ccblowfish_ltc_ecb_decrypt(const ccecb_ctx *skey, unsigned long nblocks, const void *in, void *out)
#endif
{
    uint32_t L, R;
    int r;
#ifndef __GNUC__
    uint32_t *S1, *S2, *S3, *S4;
#endif
    const unsigned char *ct = in;
    unsigned char *pt = out;
    const ltc_blowfish_keysched *xkey = (const ltc_blowfish_keysched *)skey;
    
#ifndef __GNUC__
    S1 = xkey->S[0];
    S2 = xkey->S[1];
    S3 = xkey->S[2];
    S4 = xkey->S[3];
#endif
    
    while(nblocks--) {
        /* load it */
        CC_LOAD32_BE(R, &ct[0]);
        CC_LOAD32_BE(L, &ct[4]);
        
        /* undo last keying */
        R ^= xkey->K[17];
        L ^= xkey->K[16];
        
        /* do 16 rounds */
        for (r = 15; r > 0; ) {
            L ^= LTC_F(R); R ^= xkey->K[r--];
            R ^= LTC_F(L); L ^= xkey->K[r--];
            L ^= LTC_F(R); R ^= xkey->K[r--];
            R ^= LTC_F(L); L ^= xkey->K[r--];
        }
        
        /* store */
        CC_STORE32_BE(L, &pt[0]);
        CC_STORE32_BE(R, &pt[4]);
        
        pt += CCBLOWFISH_BLOCK_SIZE;
        ct += CCBLOWFISH_BLOCK_SIZE;
    }
}

#ifdef LTC_CLEAN_STACK
static void ccblowfish_ltc_ecb_decrypt(const ccecb_ctx *skey, unsigned long nblocks, const void *in, void *out)
{
    int err = _ltc_blowfish_ecb_decrypt(skey, nblocks, in, out);
    
    ltc_burn_stack(sizeof(uint32_t) * 2 + sizeof(int));
    return err;
}
#endif


const struct ccmode_ecb ccblowfish_ltc_ecb_decrypt_mode = {
    .size = sizeof(ltc_blowfish_keysched),
    .block_size = CCBLOWFISH_BLOCK_SIZE,
    .init = ccblowfish_ltc_setup,
    .ecb = ccblowfish_ltc_ecb_decrypt
};
