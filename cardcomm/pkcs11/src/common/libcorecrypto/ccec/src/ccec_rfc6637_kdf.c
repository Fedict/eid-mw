/*
 * Copyright (c) 2014,2015 Apple Inc. All rights reserved.
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


#include <corecrypto/ccec_priv.h>
#include <corecrypto/ccaes.h>
#include <corecrypto/ccdigest.h>
#include <corecrypto/ccsha2.h>
#include <corecrypto/ccwrap.h>


struct ccec_rfc6637 ccec_rfc6637_sha256_kek_aes128 = {
    .name = "wrap-sha256-kex-aes128wrap",
    .kdfhash_id = ccpgp_digest_sha256,
    .difun = ccsha256_di,
    .kek_id = ccpgp_cipher_aes128,
    .keysize = CCAES_KEY_SIZE_128,
};

struct ccec_rfc6637 ccec_rfc6637_sha512_kek_aes256 = {
    .name = "wrap-sha512-kek-aes128",
    .kdfhash_id = ccpgp_digest_sha512,
    .difun = ccsha512_di,
    .kek_id = ccpgp_cipher_aes256,
    .keysize = CCAES_KEY_SIZE_256,
};

struct ccec_rfc6637_curve ccec_rfc6637_dh_curve_p256 = {
    .curve_oid = (const uint8_t *)"\x08\x2A\x86\x48\xCE\x3D\x03\x01\x07",
    .public_key_alg = ccec_rfc6637_ecdh_public_key_id,
};

struct ccec_rfc6637_curve ccec_rfc6637_dh_curve_p521 = {
    .curve_oid = (const uint8_t *)"\x05\x2B\x81\x04\x00\x23",
    .public_key_alg = ccec_rfc6637_ecdh_public_key_id,
};

void
ccec_rfc6637_kdf(const struct ccdigest_info *di,
                 const struct ccec_rfc6637_curve *curve,
                 const struct ccec_rfc6637 *wrap,
                 size_t skey_size, const void *skey,
                 size_t fingerprint_size, const void *fingerprint,
                 void *hash)
{
    ccdigest_di_decl(di, dictx);

    ccdigest_init(di, (ccdigest_ctx_t)dictx);
    ccdigest_update(di, (ccdigest_ctx_t)dictx, 4, "\x00\x00\x00\x01");
    ccdigest_update(di, (ccdigest_ctx_t)dictx, skey_size, skey);

    /* params */
    ccdigest_update(di, (ccdigest_ctx_t)dictx, 1, &curve->curve_oid[0]);
    ccdigest_update(di, (ccdigest_ctx_t)dictx, curve->curve_oid[0], &curve->curve_oid[1]);
    ccdigest_update(di, (ccdigest_ctx_t)dictx, 1, &curve->public_key_alg);
    ccdigest_update(di, (ccdigest_ctx_t)dictx, 2, "\x03\x01");
    ccdigest_update(di, (ccdigest_ctx_t)dictx, 1, &wrap->kdfhash_id);
    ccdigest_update(di, (ccdigest_ctx_t)dictx, 1, &wrap->kek_id);
    ccdigest_update(di, (ccdigest_ctx_t)dictx, 20, "Anonymous Sender    ");
    ccdigest_update(di, (ccdigest_ctx_t)dictx, fingerprint_size, fingerprint);
    ccdigest_final(di, (ccdigest_ctx_t)dictx, hash);
    ccdigest_di_clear(di, dictx);
}
