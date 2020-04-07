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

struct ccec_rfc6637_wrap {
    struct ccec_rfc6637 *pgp;
    const struct ccmode_ecb * (*enc)(void);
};

struct ccec_rfc6637_wrap ccec_rfc6637_wrap_sha256_kek_aes128 = {
    .pgp = &ccec_rfc6637_sha256_kek_aes128,
    .enc = ccaes_ecb_encrypt_mode,
};

struct ccec_rfc6637_wrap ccec_rfc6637_wrap_sha512_kek_aes256 = {
    .pgp = &ccec_rfc6637_sha512_kek_aes256,
    .enc = ccaes_ecb_encrypt_mode,
};

size_t
ccec_rfc6637_wrap_key_size(ccec_pub_ctx_t public_key,
                       unsigned long flags,
                       size_t key_len)
{
    size_t len;

    if (flags & CCEC_RFC6637_COMPACT_KEYS)
        len = ccec_compact_export_size(0, (ccec_full_ctx *)public_key.pub);
    else
        len = ccec_export_pub_size(public_key);
    if (flags & CCEC_RFC6637_DEBUG_KEYS) {
        len += 2;
        len += key_len;
        len += ccec_cp_prime_size(ccec_ctx_cp(public_key));
    }
    return 2 + len + 1 + 48;
}

int
ccec_rfc6637_wrap_key(ccec_pub_ctx_t public_key,
                  void *wrapped_key,
                  unsigned long flags,
                  uint8_t symm_alg_id,
                  size_t key_len,
                  const void *key,
                  const struct ccec_rfc6637_curve *curve,
                  const struct ccec_rfc6637_wrap *wrap,
                  const uint8_t *fingerprint, /* 20 bytes */
                  struct ccrng_state *rng)
{
    const struct ccdigest_info *di = wrap->pgp->difun();
    unsigned long n;
    uint8_t m[40];
    int res;
    size_t ephemeral_key_size;

    if (key_len > sizeof(m) - 1 - 2 - 1) /* ALG-ID, CHECKSUM, pkcs5 padding */
        return -1;
    
    if (di->output_size < wrap->pgp->keysize)
        return -1;
    
    /*
     * Generate a empheral keypair and share keypublic key
     */
    
    ccec_full_ctx_decl_cp(ccec_ctx_cp(public_key), ephemeral_key);
    if (flags & CCEC_RFC6637_COMPACT_KEYS) {
        res = ccec_compact_generate_key(ccec_ctx_cp(public_key), rng, ephemeral_key);
        ephemeral_key_size = ccec_compact_export_size(0, ephemeral_key);
    } else {
        res = ccec_generate_key_fips(ccec_ctx_cp(public_key), rng, ephemeral_key);
        ephemeral_key_size = ccec_export_pub_size(ephemeral_key);
    }
    if (res)
        return res;
    size_t skey_size = ccec_cp_prime_size(ccec_ctx_cp(public_key));

    uint8_t skey[skey_size];
    
    res = ccecdh_compute_shared_secret(ephemeral_key, public_key, &skey_size, skey,rng);
    if (res)
        return res;
    
    /*
     * generate m(essage)
     */
    
    m[0] = symm_alg_id;
    CC_MEMCPY(&m[1], key, key_len);
    uint16_t cksum = pgp_key_checksum(key_len, key);
    m[1 + key_len + 0] = (cksum >> 8) & 0xff;
    m[1 + key_len + 1] = (cksum     ) & 0xff;
    uint8_t padbyte = sizeof(m) - 1 - key_len - 2;
    for (n = 1 + key_len + 2; n < sizeof(m); n++)
        m[n] = padbyte;
    
    /*
     * KDF
     */
    uint8_t hash[di->output_size];
    
    ccec_rfc6637_kdf(di, curve, wrap->pgp, skey_size, skey, 20, fingerprint, hash);
    cc_clear(skey_size, skey);

    /* MPI(public_key) | len(C) (byte) | C */
    uint8_t *output = wrapped_key;
    
    uint8_t epkey[ephemeral_key_size];
    if (flags & CCEC_RFC6637_COMPACT_KEYS)
        ccec_compact_export(0, epkey, ephemeral_key);
    else
        ccec_export_pub(ephemeral_key, epkey);

    size_t t = sizeof(epkey) * 8;
    output[0] = (t >> 8) & 0xff;
    output[1] = (t     ) & 0xff;
    CC_MEMCPY(&output[2], epkey, sizeof(epkey));
    output[2 + sizeof(epkey) + 0] = sizeof(m);
    CC_MEMCPY(&output[2 + sizeof(epkey) + 1], m, sizeof(m));
    
    /*
     * wrap
     */
    
    const struct ccmode_ecb *ecbmode = wrap->enc();
    
    ccecb_ctx_decl(ccecb_context_size(ecbmode), ecb);
    ccecb_init(ecbmode, ecb, wrap->pgp->keysize, hash);
    cc_clear(sizeof(hash),hash);
    
    size_t obytes;
    
    res = ccwrap_auth_encrypt(ecbmode, ecb, sizeof(m), m, &obytes, &output[2 + sizeof(epkey) + 1]);
    ccecb_ctx_clear(ccecb_context_size(ecbmode), ecb);
    assert (obytes == sizeof(m) + ecbmode->block_size / 2);
    
    output[2 + sizeof(epkey) + 0] = obytes;

    if (flags & CCEC_RFC6637_DEBUG_KEYS) {
        output[2 + sizeof(epkey) + 1 + obytes] = key_len;
        output[2 + sizeof(epkey) + 1 + obytes + 1] = skey_size;
        CC_MEMCPY(&output[2 + sizeof(epkey) + 1 + obytes + 2], key, key_len);
        CC_MEMCPY(&output[2 + sizeof(epkey) + 1 + obytes + 2 + key_len], skey, skey_size);
    }

    return res;
}
