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

struct ccec_rfc6637_unwrap {
    struct ccec_rfc6637 *pgp;
    const struct ccmode_ecb * (*dec)(void);
};

struct ccec_rfc6637_unwrap ccec_rfc6637_unwrap_sha256_kek_aes128 = {
    .pgp = &ccec_rfc6637_sha256_kek_aes128,
    .dec = ccaes_ecb_decrypt_mode,
};

struct ccec_rfc6637_unwrap ccec_rfc6637_unwrap_sha512_kek_aes256 = {
    .pgp = &ccec_rfc6637_sha512_kek_aes256,
    .dec = ccaes_ecb_decrypt_mode,
};

int
ccec_rfc6637_unwrap_key(ccec_full_ctx_t private_key,
                        size_t *key_len,
                        void *key,
                        unsigned long flags,
                        uint8_t *symm_key_alg,
                        const struct ccec_rfc6637_curve *curve,
                        const struct ccec_rfc6637_unwrap *wrap,
                        const uint8_t *fingerprint,
                        size_t wrapped_key_len,
                        const void  *wrapped_key)
{
    const struct ccdigest_info *di = wrap->pgp->difun();
    const uint8_t *wkey = wrapped_key;
    int res;

    if (di->output_size < wrap->pgp->keysize)
        return -1;
    
    if (wrapped_key_len < 5)
        return -1;
    
    size_t wkey_size = CC_BITLEN_TO_BYTELEN((wkey[0] << 8) | wkey[1]);
    if (wkey_size > wrapped_key_len - 2 - 1)
        return -1;
    
    size_t wrapped_size = wkey[2 + wkey_size];
    if ((flags & CCEC_RFC6637_DEBUG_KEYS)) {
        if (wrapped_key_len < 2 + wkey_size + 1 + wrapped_size)
            return -1;
    } else if (wrapped_key_len != 2 + wkey_size + 1 + wrapped_size) {
        return -1;
    }

    /*
     * Generate a empheral keypair and share keypublic key
     */
    
    ccec_pub_ctx_decl_cp(ccec_ctx_cp(private_key), ephemeral_key);
    ccec_ctx_init(ccec_ctx_cp(private_key), ephemeral_key);

    /*
     * There is no ccec_NNN_IMPORT_pub_size()
     */
    if (ccec_export_pub_size(ephemeral_key) == wkey_size) {
        res = ccec_import_pub(ccec_ctx_cp(private_key), wkey_size, &wkey[2], ephemeral_key);
    } else if ((flags & CCEC_RFC6637_COMPACT_KEYS) && ccec_compact_export_size(0, (ccec_full_ctx *)ephemeral_key) >= wkey_size) {
        res = ccec_compact_import_pub(ccec_ctx_cp(private_key), wkey_size, &wkey[2], ephemeral_key);
    } else {
        res = -1;
    }
    if (res)
        return res;

    size_t skey_size = ccec_cp_prime_size(ccec_ctx_cp(private_key));

    uint8_t skey[skey_size];
    res = ccecdh_compute_shared_secret(private_key, ephemeral_key, &skey_size, skey, NULL);
    if (res)
        return res;
    
    /*
     * KDF
     */
    uint8_t hash[di->output_size];
    
    ccec_rfc6637_kdf(di, curve, wrap->pgp, skey_size, skey, 20, fingerprint, hash);
    cc_clear(sizeof(skey), skey);

    /*
     * unwrap
     */
    
    const struct ccmode_ecb *ecbmode = wrap->dec();
    
    ccecb_ctx_decl(ccecb_context_size(ecbmode), ecb);
    ccecb_init(ecbmode, ecb, wrap->pgp->keysize, hash);
    cc_clear(sizeof(hash), hash);


    uint8_t m[wrapped_size];
    size_t m_size = wrapped_size;
    
    res = ccwrap_auth_decrypt(ecbmode, ecb, wrapped_size, &wkey[2 + wkey_size + 1], &m_size, m);
    ccecb_ctx_clear(ccecb_context_size(ecbmode), ecb);
    if (res)
        return res;

    /*
     * validate key
     */
    
    if (m_size < 1 || m_size > sizeof(m) - 1)
        return -1;
    
    *symm_key_alg = m[0];
    
    uint8_t padding = m[m_size - 1];
    
    /*
     * Don't need to make this constant time since ccwrap_auth_decrypt() have a checksum.
     */
    if (padding > m_size - 1 - 2)
        return -1;
    
    size_t n;
    for (n = 0; n < padding; n++)
        if (m[m_size - 1 - n] != padding)
            return -1;
    
    if (*key_len >= m_size - 1 - 2 - padding)
        *key_len = m_size - 1 - 2 - padding;
    else
        return -1;
    
    /*
     * validate key checksum
     */
    
    uint16_t cksum = pgp_key_checksum(*key_len, m + 1);
    if (((cksum >> 8) & 0xff) != m[1 + *key_len] || (cksum & 0xff) != m[1 + *key_len + 1])
        return -1;
    
    CC_MEMCPY(key, m + 1, *key_len);
    
    return res;
}
