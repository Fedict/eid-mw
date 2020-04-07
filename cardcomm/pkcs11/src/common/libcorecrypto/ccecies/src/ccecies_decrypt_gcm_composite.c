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

static int ccecies_import_pub(uint32_t options, ccec_full_ctx_t full_key, size_t *in_len, const uint8_t *in, ccec_pub_ctx_t key)
{
    int status=0;

    if (ECIES_EXPORT_PUB_STANDARD == (options & ECIES_EXPORT_PUB_STANDARD))
    {
        *in_len=ccec_x963_export_size(0, full_key);
        status=ccec_x963_import_pub(ccec_ctx_cp(full_key), *in_len,in, key);
    } else if (ECIES_EXPORT_PUB_COMPACT == (options & ECIES_EXPORT_PUB_COMPACT)){
        *in_len=ccec_compact_export_size(0, full_key);
        status=ccec_compact_import_pub(ccec_ctx_cp(full_key), *in_len,in, key);
    }
    else
    {
        status=-2;
    }
    return  status;
}


int
ccecies_decrypt_gcm_composite(ccec_full_ctx_t full_key,
                    const ccecies_gcm_t ecies,
                    uint8_t *plaintext,  /* output - expect length ccecies_decrypt_gcm_plaintext_size */
                    size_t sharedinfo1_byte_len, const void *sharedinfo_1,
                    size_t sharedinfo2_byte_len, const void *sharedinfo_2,
                    size_t ciphertext_len, const uint8_t *ciphertext,
                    const uint8_t *imported_public_key, /* expect length from ccecies_pub_key_size */
                    const uint8_t *mac_tag              /* expect length ecies->mac_length */

)
{
    int status=-1;
    size_t imported_pubkey_size=0;

    // Contexts
    ccec_pub_ctx_decl_cp(ccec_ctx_cp(full_key), public_key);
    size_t   skey_size = ccec_cp_prime_size(ccec_ctx_cp(full_key));
    uint8_t  skey[skey_size];
    const struct ccmode_gcm *gcm_decrypt=ecies->gcm;
    ccgcm_ctx_decl(gcm_decrypt->size,gcm_ctx);
    uint8_t computed_tag[ecies->mac_length];


    // 1) Import public key from ciphertext
    cc_require(ccecies_import_pub(ecies->options, full_key, &imported_pubkey_size, imported_public_key, public_key)==0, errOut);

#if CC_DEBUG_ECIES
    ccec_print_public_key("Ephemeral public key",public_key);
#endif

    // 2) ECDH with input private key and imported public key
    cc_require(ccecdh_compute_shared_secret(full_key, public_key, &skey_size, skey,ecies->rng)==0,errOut);

#if CC_DEBUG_ECIES
    cc_print("Shared secret key",skey_size,skey);
#endif

    // 3) Derive Enc / Mac key
    // Hash(skey|00000001|sharedinfo_1)
    cc_assert(ecies->key_length<=skey_size);
    if (ECIES_EPH_PUBKEY_IN_SHAREDINFO1 == (ecies->options & ECIES_EPH_PUBKEY_IN_SHAREDINFO1))
    {   // use ephemeral public key as shared info 1
        cc_require(ccansikdf_x963(ecies->di,
                                  skey_size,skey,
                                  imported_pubkey_size,imported_public_key,
                                  ecies->key_length,skey)==0,errOut);
    }
    else
    {   // use provided string
        cc_require(ccansikdf_x963(ecies->di,
                                  skey_size,skey,
                                  sharedinfo1_byte_len,sharedinfo_1,
                                  ecies->key_length,skey)==0,errOut);
    }

#if CC_DEBUG_ECIES
    cc_print("Cipher key",ecies->key_length,skey);
#endif

    // 3) Verify MAC on ciphertext (SharedInfo 2)
    ccgcm_init(gcm_decrypt, gcm_ctx,ecies->key_length,skey);
    ccgcm_set_iv(gcm_decrypt,gcm_ctx,sizeof(ecies_iv_data),ecies_iv_data);
    if ((sharedinfo_2!=NULL) && (sharedinfo2_byte_len>0)) {
        ccgcm_gmac(gcm_decrypt,gcm_ctx,sharedinfo2_byte_len,sharedinfo_2);
    }
    else
    {
        ccgcm_gmac(gcm_decrypt,gcm_ctx,0,NULL);
    }
    ccgcm_update(gcm_decrypt,gcm_ctx,
                 ciphertext_len,ciphertext,plaintext);

#if CC_DEBUG_ECIES
    cc_print("Decrypted message",ciphertext_len,plaintext);
#endif

    // 6) Mac (with SharedInfo 2)
    // sec1, p51: recommended: SharedInfo2 ended in a counter giving its length.
    ccgcm_finalize(gcm_decrypt,gcm_ctx,sizeof(computed_tag),computed_tag);

#if CC_DEBUG_ECIES
    cc_print("Computed Mac Tag",sizeof(computed_tag),computed_tag);
#endif

    // Compare received MAC with the computed one
    if (0==cc_cmp_safe(sizeof(computed_tag),computed_tag,mac_tag))
    {
        status=0;
    }

errOut:
    // Clear key material info
    ccgcm_ctx_clear(gcm_decrypt->size,gcm_ctx);
    cc_clear(sizeof(skey),skey);
    cc_clear(sizeof(computed_tag),computed_tag);
    ccec_pub_ctx_clear_cp(ccec_ctx_cp(public_key), public_key);
    return status;
}

