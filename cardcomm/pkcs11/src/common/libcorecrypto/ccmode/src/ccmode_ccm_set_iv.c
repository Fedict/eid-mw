/*
 * Copyright (c) 2012,2013,2014,2015 Apple Inc. All rights reserved.
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


#include "ccmode_internal.h"

/* Implementation per SP800-38C standard */
void ccmode_ccm_set_iv(ccccm_ctx *key, ccccm_nonce *nonce_ctx, size_t n, const void *nonce, size_t t, size_t a, size_t p) {

    /*  n: size of the nonce
        t: size of the tag
        p: size of the plaintext
        q: size of the size of the plaintext
        a: size of the authenticated data
     */

    /* blocksize should not have been an unsigned long. */
    unsigned block_size = (unsigned)CCMODE_CCM_KEY_ECB(key)->block_size;
    cc_require(block_size==16, out);
    unsigned long q = block_size - 1 - n;   // q+n = 15

    /* reset so you can't start without initializing successfully */
    _CCMODE_CCM_NONCE(nonce_ctx)->mode = CCMODE_CCM_MODE_IV;
    cc_zero(16, CCMODE_CCM_KEY_PAD(nonce_ctx));
    CCMODE_CCM_KEY_PAD_LEN(nonce_ctx) = 0;

    /* Length requirements, SP800-38C, A.2.2 */
    cc_require((4 <= t) && (t <= block_size) && !(t & 1), out);
    cc_require((2 <= q) && (q <= 8), out);
    cc_require((7 <= n) && (n <= block_size-3), out);
    cc_require((q>=sizeof(p)) || p<((__typeof__(p))1<<(8*q)), out);

    /*  SP800-38C - A.2.2
        If 0 < a < 2^16-2^8, then a is encoded as [a]16, i.e., two octets.
        If 2^16-2^8 ≤ a < 2^32, then a is encoded as 0xff || 0xfe || [a]32, i.e., six octets.
        If 2^32 ≤ a < 2^64, then a is encoded as 0xff || 0xff || [a]64, i.e., ten octets. */

    /* encoding of lengths larger than 0xFF00 unimplemented (q<=2) */
    cc_require(a < ((1<<16)-(1<<8)), out);

    CCMODE_CCM_KEY_NONCE_LEN(nonce_ctx) = n;
    CCMODE_CCM_KEY_MAC_LEN(nonce_ctx) = t;

    /* set up B_0 parameters per SP800-38C, A.2.1 */
    CCMODE_CCM_KEY_B_I(nonce_ctx)[0] =
        ((a > 0) ? 1<<6 : 0) |
        (CCMODE_CCM_KEY_MAC_LEN(nonce_ctx) / 2 - 1) << 3 |
        (q - 1);

    /* B_0 nonce */
    CC_MEMCPY(&CCMODE_CCM_KEY_B_I(nonce_ctx)[1], nonce, n);

    /* B_0 length = data_len */
    uint64_t len = p;
    for (size_t l = 0; l < q; l++) {
        CCMODE_CCM_KEY_B_I(nonce_ctx)[block_size - 1 - l] = (len & 255);
        len >>= 8;
    }

    /* encrypt B_0 in place */
    CCMODE_CCM_KEY_ECB(key)->ecb(CCMODE_CCM_KEY_ECB_KEY(key), 1,
                                 CCMODE_CCM_KEY_B_I(nonce_ctx),
                                 CCMODE_CCM_KEY_B_I(nonce_ctx));

    /* set up A_0 */
    CCMODE_CCM_KEY_A_I(nonce_ctx)[0] = q - 1;
    CC_MEMCPY(&CCMODE_CCM_KEY_A_I(nonce_ctx)[1], nonce, n);
    cc_zero(q, &CCMODE_CCM_KEY_A_I(nonce_ctx)[1] + n);

    /* A_0 encrypts MAC */
    CCMODE_CCM_KEY_ECB(key)->ecb(CCMODE_CCM_KEY_ECB_KEY(key), 1,
                                 CCMODE_CCM_KEY_A_I(nonce_ctx),
                                 CCMODE_CCM_KEY_MAC(nonce_ctx));

    /* prepare for authenticated data */
    if (a) {
        CCMODE_CCM_KEY_B_I(nonce_ctx)[0] ^= a >> 8;
        CCMODE_CCM_KEY_B_I(nonce_ctx)[1] ^= a & 255;
        CCMODE_CCM_KEY_AUTH_LEN(nonce_ctx) = 2;
        _CCMODE_CCM_NONCE(nonce_ctx)->mode = CCMODE_CCM_MODE_AAD;
    } else {
        CCMODE_CCM_KEY_AUTH_LEN(nonce_ctx) = 0;
        _CCMODE_CCM_NONCE(nonce_ctx)->mode = CCMODE_CCM_MODE_TEXT;
    }

out:
    return;
}
