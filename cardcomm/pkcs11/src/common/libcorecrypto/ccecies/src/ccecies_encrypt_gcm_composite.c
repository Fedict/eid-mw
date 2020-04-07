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


#include <corecrypto/ccecies.h>
#include <corecrypto/ccecies_priv.h>
#include <corecrypto/ccansikdf.h>
#include <corecrypto/ccec_priv.h>
#include <corecrypto/ccmode.h>
#include <corecrypto/cc_debug.h>
#include <corecrypto/cc_macros.h>

#define CC_DEBUG_ECIES (CORECRYPTO_DEBUG && 0)

static const uint8_t ecies_iv_data[ECIES_CIPHERIV_SIZE]={0};

static int ccecies_export(const int fullkey, const uint32_t options, void *out, ccec_full_ctx_t key)
{
    int status  = 0;

    if (ECIES_EXPORT_PUB_STANDARD == (options & ECIES_EXPORT_PUB_STANDARD))
    {
        ccec_x963_export(fullkey, out, key);
    }else if (ECIES_EXPORT_PUB_COMPACT == (options & ECIES_EXPORT_PUB_COMPACT)){
        ccec_compact_export(fullkey, out, key);
    }
    else{
        status=-2;
    }

    return status;
}

int
ccecies_encrypt_gcm_composite(ccec_pub_ctx_t public_key,
                    const ccecies_gcm_t ecies,
                    uint8_t *exported_public_key, /* output - length from ccecies_pub_key_size */
                    uint8_t *ciphertext,          /* output - length same as plaintext_len */
                    uint8_t *mac_tag,             /* output - length ecies->mac_length */
                    size_t plaintext_len,   const uint8_t *plaintext,
                    size_t sharedinfo1_byte_len, const void *sharedinfo_1,
                    size_t sharedinfo2_byte_len, const void *sharedinfo_2
)
{
    int status=-1;

    // Contexts:
    ccec_full_ctx_decl_cp(ccec_ctx_cp(public_key), ephemeral_key);
    size_t   skey_size = ccec_cp_prime_size(ccec_ctx_cp(public_key));
    uint8_t  skey[skey_size];
    const struct ccmode_gcm *gcm_encrypt=ecies->gcm;
    ccgcm_ctx_decl(gcm_encrypt->size,gcm_ctx);
    size_t exported_public_key_size;

    // 1) Generate ephemeral EC key pair
    cc_assert(ecies->rng!=NULL);
    cc_require(ccecdh_generate_key(ccec_ctx_cp(public_key), ecies->rng, ephemeral_key)==0,errOut);

#if CC_DEBUG_ECIES
    ccec_print_full_key("Ephemeral key",ephemeral_key);
#endif

    // 2) ECDH with input public key
    cc_require(ccecdh_compute_shared_secret(ephemeral_key, public_key, &skey_size, skey,ecies->rng)==0,errOut);

#if CC_DEBUG_ECIES
    cc_print("Shared secret key",skey_size,skey);
#endif

    // 3) Export ephemeral public key
    cc_require( ccecies_export(0, ecies->options, exported_public_key, ephemeral_key)==0, errOut);

    // 4) Derive Enc / Mac key
    // Hash(skey|00000001|sharedinfo_1)
    cc_assert(ecies->key_length<=skey_size);
    exported_public_key_size=ccecies_pub_key_size(ephemeral_key,ecies);
    if (ECIES_EPH_PUBKEY_IN_SHAREDINFO1 == (ecies->options & ECIES_EPH_PUBKEY_IN_SHAREDINFO1))
    {   // use ephemeral public key as shared info 1
        cc_require(ccansikdf_x963(ecies->di,
                                  skey_size,skey,
                                  exported_public_key_size,exported_public_key,
                                  ecies->key_length,skey)==0,errOut);
    }
    else
    {
        cc_require(ccansikdf_x963(ecies->di,
                                  skey_size,skey,
                                  sharedinfo1_byte_len,sharedinfo_1,
                                  ecies->key_length,skey)==0,errOut);
    }

#if CC_DEBUG_ECIES
    cc_print("Cipher key",ecies->key_length,skey);
#endif

    // 5) Encrypt
    ccgcm_init(gcm_encrypt, gcm_ctx,ecies->key_length,skey);
    ccgcm_set_iv(gcm_encrypt,gcm_ctx,sizeof(ecies_iv_data),ecies_iv_data);
    if ((sharedinfo_2!=NULL) && (sharedinfo2_byte_len>0)) {
        ccgcm_gmac(gcm_encrypt,gcm_ctx,sharedinfo2_byte_len,sharedinfo_2);
    }
    else
    {
        ccgcm_gmac(gcm_encrypt,gcm_ctx,0,NULL);
    }
    ccgcm_update(gcm_encrypt,gcm_ctx,
                 plaintext_len,plaintext,
                 ciphertext);

#if CC_DEBUG_ECIES
    cc_print("Encrypted message",plaintext_len,ciphertext);
#endif

    // 6) Mac (with SharedInfo 2)
    // sec1, p51: recommended: SharedInfo2 ended in a counter giving its length.
    ccgcm_finalize(gcm_encrypt,gcm_ctx,ecies->mac_length,mac_tag);
#if CC_DEBUG_ECIES
    cc_print("Mac Tag",ecies->mac_length,mac_tag);
#endif

    // Success
    status=0;

errOut:
    // Clear key material info
    ccgcm_ctx_clear(gcm_encrypt->size,gcm_ctx);
    cc_clear(sizeof(skey),skey);
    ccec_full_ctx_clear_cp(ccec_ctx_cp(public_key), ephemeral_key);
    return status;
}

