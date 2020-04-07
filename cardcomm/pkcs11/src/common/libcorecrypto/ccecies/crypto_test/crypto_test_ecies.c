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


#include "testmore.h"
#include "testbyteBuffer.h"
#include "testccnBuffer.h"
#include <corecrypto/cc_macros.h>

#if (CCECIES == 0)
entryPoint(ccies,"ccies")
#else
#define USE_SUPER_COOL_NEW_CCOID_T
#include "crypto_test_ecies.h"
#include <corecrypto/ccec.h>
#include <corecrypto/ccec_priv.h>
#include <corecrypto/ccecies.h>
#include <corecrypto/ccaes.h>
#include <corecrypto/ccrng_test.h>
#include <corecrypto/ccrng_ecfips_test.h>
#include <corecrypto/ccsha1.h>
#include <corecrypto/ccsha2.h>

static const int verbose=1;

static const uint8_t shared_info1[]="test1";
static const uint8_t shared_info2[]="test2";
static const uint8_t shared_info3[]="test3";

#define di_SHA1   &ccsha1_eay_di
#define di_SHA224 &ccsha224_ltc_di
#define di_SHA256 &ccsha256_ltc_di
#define di_SHA384 &ccsha384_ltc_di
#define di_SHA512 &ccsha512_ltc_di


const struct ccecies_vector ccecies_aes_gcm_vectors[]=
{
#include "../test_vectors/ecies_aes_gcm.inc"
};


// Negative testing based on KATs
static int ccecies_gcm_kat_negative_test(const struct ccecies_vector *test)
{
    int status = 0; // fail
    byteBuffer plaintext = hexStringToBytes(test->message);
    byteBuffer expectedCiphertext = hexStringToBytes(((test->options&ECIES_EXPORT_PUB_STANDARD)==
                                    ECIES_EXPORT_PUB_STANDARD)?test->cipher:test->compact_cipher);
    size_t sharedInfo1_size = strlen(test->sharedInfo1);
    size_t sharedInfo2_size = strlen(test->sharedInfo2);
    byteBuffer eph_priv_key = hexStringToBytes(test->eph_priv_key);
    byteBuffer dec_priv_key = hexStringToBytes(test->dec_priv_key);
    struct ccrng_ecfips_test_state rng;
    struct ccecies_gcm ecies_enc;
    struct ccecies_gcm ecies_dec;
    ccec_const_cp_t cp=test->curve();
    size_t fake_size;
    ccec_full_ctx_decl_cp(cp, remote_key);

    // Generate "remote" public key from private key
    ok_or_fail(ccec_recover_full_key(cp,dec_priv_key->len, dec_priv_key->bytes, remote_key) == 0, "Generated Key");

    // Set RNG to control ephemeral key
    ccrng_ecfips_test_init(&rng, eph_priv_key->len, eph_priv_key->bytes);
    ccecies_encrypt_gcm_setup(&ecies_enc,test->di,(struct ccrng_state *)&rng,ccaes_gcm_encrypt_mode(),
                              test->key_length,test->mac_length,test->options);
    ccecies_decrypt_gcm_setup(&ecies_dec,test->di,ccaes_gcm_decrypt_mode(),
                              test->key_length,test->mac_length,test->options);

    // Buffer for outputs
    byteBuffer ciphertext = mallocByteBuffer(ccecies_encrypt_gcm_ciphertext_size(remote_key,&ecies_enc,plaintext->len));
    byteBuffer plaintext_bis = mallocByteBuffer(ccecies_decrypt_gcm_plaintext_size(remote_key,&ecies_dec,ciphertext->len));

    // Valid encrypted value
    ok_or_goto(ccecies_encrypt_gcm(remote_key,&ecies_enc,
                                   plaintext->len,plaintext->bytes,
                                   sharedInfo1_size,test->sharedInfo1,
                                   sharedInfo2_size,test->sharedInfo2,
                                   &ciphertext->len,ciphertext->bytes)==0, "Encrypt", errout);

    // ------------------------------
    // Negative testing of decrypt
    // ------------------------------

    // Corrupted public key
    ciphertext->bytes[2]^=1;
    ok_or_goto(ccecies_decrypt_gcm(remote_key,&ecies_dec,
                                   ciphertext->len,ciphertext->bytes,
                                   sharedInfo1_size,test->sharedInfo1,
                                   sharedInfo2_size,test->sharedInfo2,
                                   &plaintext_bis->len,plaintext_bis->bytes)!=0, "Corrupted public key", errout);
    ciphertext->bytes[2]^=1;

    // Corrupted encrypted data (first byte)
    size_t b = ((test->options&ECIES_EXPORT_PUB_STANDARD)==ECIES_EXPORT_PUB_STANDARD)?
    ccec_x963_export_size(0,remote_key):ccec_compact_export_size(0,remote_key);
    ciphertext->bytes[b]^=1;
    ok_or_goto(ccecies_decrypt_gcm(remote_key,&ecies_dec,
                                    ciphertext->len,ciphertext->bytes,
                                    sharedInfo1_size,test->sharedInfo1,
                                    sharedInfo2_size,test->sharedInfo2,
                                    &plaintext_bis->len,plaintext_bis->bytes)!=0, "Corrupted encrypted data, first byte", errout);
    ciphertext->bytes[b]^=1;

    // Corrupted encrypted data (last byte)
    ciphertext->bytes[ciphertext->len-test->mac_length-1]^=1;
    ok_or_goto(ccecies_decrypt_gcm(remote_key,&ecies_dec,
                                   ciphertext->len,ciphertext->bytes,
                                   sharedInfo1_size,test->sharedInfo1,
                                   sharedInfo2_size,test->sharedInfo2,
                                   &plaintext_bis->len,plaintext_bis->bytes)!=0, "Corrupted encrypted data, last byte", errout);
    ciphertext->bytes[ciphertext->len-test->mac_length-1]^=1;

    // Corrupted mac
    ciphertext->bytes[ciphertext->len-test->mac_length]^=1;
    ok_or_goto(ccecies_decrypt_gcm(remote_key,&ecies_dec,
                                   ciphertext->len,ciphertext->bytes,
                                   sharedInfo1_size,test->sharedInfo1,
                                   sharedInfo2_size,test->sharedInfo2,
                                   &plaintext_bis->len,plaintext_bis->bytes)!=0, "Corrupted mac", errout);
    ciphertext->bytes[ciphertext->len-test->mac_length]^=1;

    // Output buffer too small
    fake_size=plaintext_bis->len-1;
    ok_or_goto(ccecies_decrypt_gcm(remote_key,&ecies_dec,
                                   ciphertext->len,ciphertext->bytes,
                                   sharedInfo1_size,test->sharedInfo1,
                                   sharedInfo2_size,test->sharedInfo2,
                                   &fake_size,plaintext_bis->bytes)!=0, "Decrypt: output too small", errout);

    // SharedInfo1 mismatch
    if (ECIES_EPH_PUBKEY_IN_SHAREDINFO1==(test->options & ECIES_EPH_PUBKEY_IN_SHAREDINFO1))
    {   // Pass shared info is ignored
        ok_or_goto(ccecies_decrypt_gcm(remote_key,&ecies_dec,
                                       ciphertext->len,ciphertext->bytes,
                                       sizeof(shared_info3),shared_info3,
                                       sharedInfo2_size,test->sharedInfo2,
                                       &plaintext_bis->len,plaintext_bis->bytes)==0, "SharedInfo1 mismatch", errout);
    }
    else
    {
        ok_or_goto(ccecies_decrypt_gcm(remote_key,&ecies_dec,
                                       ciphertext->len,ciphertext->bytes,
                                       sizeof(shared_info3),shared_info3,
                                       sharedInfo2_size,test->sharedInfo2,
                                       &plaintext_bis->len,plaintext_bis->bytes)!=0, "SharedInfo1 mismatch", errout);
    }

    // SharedInfo2 mismatch
    ok_or_goto(ccecies_decrypt_gcm(remote_key,&ecies_dec,
                                   ciphertext->len,ciphertext->bytes,
                                   sharedInfo1_size,test->sharedInfo1,
                                   sizeof(shared_info3),shared_info3,
                                   &plaintext_bis->len,plaintext_bis->bytes)!=0, "SharedInfo2 mismatch", errout);

    // ------------------------------
    // Negative testing of encrypt
    // ------------------------------
    // Bad random
    ccrng_ecfips_test_init(&rng, 0, NULL);
    ok_or_goto(ccecies_encrypt_gcm(remote_key,&ecies_enc,
                                   plaintext->len,plaintext->bytes,
                                   sharedInfo1_size,test->sharedInfo1,
                                   sharedInfo2_size,test->sharedInfo2,
                                   &ciphertext->len,ciphertext->bytes)!=0, "Bad random", errout);

    // Output size too small
    fake_size=test->mac_length+ccec_x963_export_size(0,remote_key)+plaintext->len-1;
    ok_or_goto(ccecies_encrypt_gcm(remote_key,&ecies_enc,
                                   plaintext->len,plaintext->bytes,
                                   sharedInfo1_size,test->sharedInfo1,
                                   sharedInfo2_size,test->sharedInfo2,
                                   &fake_size,ciphertext->bytes)!=0, "Encrypt: output too small", errout);

    status = 1;
errout:
    free(expectedCiphertext);
    free(eph_priv_key);
    free(dec_priv_key);
    free(plaintext);
    free(plaintext_bis);
    free(ciphertext);
    return status;
}

// Process one vector
static int ccecies_gcm_kat_test(const struct ccecies_vector *test)
{
    int status = 0; // fail
    byteBuffer plaintext = hexStringToBytes(test->message);
    byteBuffer expectedCiphertext = hexStringToBytes(((test->options&ECIES_EXPORT_PUB_STANDARD)==
                                    ECIES_EXPORT_PUB_STANDARD)?test->cipher:test->compact_cipher);
    size_t sharedInfo1_size = strlen(test->sharedInfo1);
    size_t sharedInfo2_size = strlen(test->sharedInfo2);
    byteBuffer eph_priv_key = hexStringToBytes(test->eph_priv_key);
    byteBuffer dec_priv_key = hexStringToBytes(test->dec_priv_key);
    struct ccrng_ecfips_test_state rng;
    struct ccecies_gcm ecies_enc;
    struct ccecies_gcm ecies_dec;
    ccec_const_cp_t cp=test->curve();

    ccec_full_ctx_decl_cp(cp, remote_key);

    // Generate "remote" public key from private key
    ok_or_fail(ccec_recover_full_key(cp,dec_priv_key->len, dec_priv_key->bytes, remote_key) == 0, "Generated Key");

    // Set RNG to control ephemeral key
    ccrng_ecfips_test_init(&rng, eph_priv_key->len, eph_priv_key->bytes);
    ccecies_encrypt_gcm_setup(&ecies_enc,test->di,(struct ccrng_state *)&rng,ccaes_gcm_encrypt_mode(),
                              test->key_length,test->mac_length,test->options);
    ccecies_decrypt_gcm_setup(&ecies_dec,test->di,ccaes_gcm_decrypt_mode(),
                              test->key_length,test->mac_length,test->options);

    // Buffer for outputs
    byteBuffer ciphertext = mallocByteBuffer(ccecies_encrypt_gcm_ciphertext_size(remote_key,&ecies_enc,plaintext->len));
    byteBuffer plaintext_bis = mallocByteBuffer(ccecies_decrypt_gcm_plaintext_size(remote_key,&ecies_dec,ciphertext->len));

    ok_or_goto(ccecies_encrypt_gcm(remote_key,&ecies_enc,
                                   plaintext->len,plaintext->bytes,
                                   sharedInfo1_size,test->sharedInfo1,
                                   sharedInfo2_size,test->sharedInfo2,
                                   &ciphertext->len,ciphertext->bytes)==0, "Encrypt", errout);

    ok_or_goto(ccecies_decrypt_gcm(remote_key,&ecies_dec,
                                   ciphertext->len,ciphertext->bytes,
                                   sharedInfo1_size,test->sharedInfo1,
                                   sharedInfo2_size,test->sharedInfo2,
                                   &plaintext_bis->len,plaintext_bis->bytes)==0, "Decrypt", errout);

    // Checks
    ok_or_goto(ciphertext->len == expectedCiphertext->len, "Ciphertext size correct", errout);
    ok_or_goto(memcmp(ciphertext->bytes,expectedCiphertext->bytes,expectedCiphertext->len)==0, "Ciphertext as expected", errout);
    ok_or_goto(plaintext->len == plaintext_bis->len, "Decrypted plaintext size correct", errout);
    ok_or_goto(memcmp(plaintext->bytes,plaintext_bis->bytes,plaintext->len)==0, "Shared secrets match", errout);

    status = 1;
errout:
    free(expectedCiphertext);
    free(eph_priv_key);
    free(dec_priv_key);
    free(plaintext);
    free(plaintext_bis);
    free(ciphertext);
    return status;
}


static int
ecies_aes_gcm_crypt_decrypt(ccec_full_ctx_t key,
                         size_t msg_size,
                         const struct ccdigest_info *di,
                         struct ccrng_state *rng,
                         uint32_t options)
{

    int status = 0; // fail
    byteBuffer plaintext = mallocByteBuffer(msg_size);


    struct ccecies_gcm ecies_enc;
    struct ccecies_gcm ecies_dec;

    ccrng_generate(rng,msg_size,plaintext->bytes);

    ccecies_encrypt_gcm_setup(&ecies_enc,di,rng,ccaes_gcm_encrypt_mode(),
                              16,16,options);
    ccecies_decrypt_gcm_setup(&ecies_dec,di,ccaes_gcm_decrypt_mode(),
                                  16,16,options);

    byteBuffer ciphertext = mallocByteBuffer(ccecies_encrypt_gcm_ciphertext_size(key,&ecies_enc,msg_size));

    byteBuffer plaintext_bis = mallocByteBuffer(ccecies_decrypt_gcm_plaintext_size(key,&ecies_dec,ciphertext->len));

    // With shared info
    ok_or_goto(ccecies_encrypt_gcm(key,&ecies_enc,
                                   plaintext->len,plaintext->bytes,
                                   sizeof(shared_info1), shared_info1,
                                   sizeof(shared_info2), shared_info2,
                                   &ciphertext->len,ciphertext->bytes)==0, "Encrypt", errout);

    ok_or_goto(ccecies_decrypt_gcm(key,&ecies_dec,
                                   ciphertext->len,ciphertext->bytes,
                                   sizeof(shared_info1), shared_info1,
                                   sizeof(shared_info2), shared_info2,
                                   &plaintext_bis->len,plaintext_bis->bytes)==0, "Decrypt", errout);


    ok_or_goto(plaintext->len == plaintext_bis->len, "Encrypt/Decrypt correct keysize", errout);
    ok_or_goto(memcmp(plaintext->bytes,plaintext_bis->bytes,plaintext->len)==0, "Shared secrets match", errout);

    // Without shared info
    ok_or_goto(ccecies_encrypt_gcm(key,&ecies_enc,
                                   plaintext->len,plaintext->bytes,
                                   sizeof(shared_info1), NULL,
                                   sizeof(shared_info2), NULL,
                                   &ciphertext->len,ciphertext->bytes)==0, "Encrypt", errout);

    ok_or_goto(ccecies_decrypt_gcm(key,&ecies_dec,
                                   ciphertext->len,ciphertext->bytes,
                                   sizeof(shared_info1), NULL,
                                   sizeof(shared_info2), NULL,
                                   &plaintext_bis->len,plaintext_bis->bytes)==0, "Decrypt", errout);


    ok_or_goto(plaintext->len == plaintext_bis->len, "Encrypt/Decrypt correct keysize", errout);
    ok_or_goto(memcmp(plaintext->bytes,plaintext_bis->bytes,plaintext->len)==0, "Shared secrets match", errout);

    status = 1; // Success

errout:
    free(plaintext);
    free(plaintext_bis);
    free(ciphertext);
    return status;
}


static int
ecies_test(struct ccrng_state *rng,
           size_t expected_keysize,
           ccec_const_cp_t cp,
           uint32_t options)
{
    int status = 0;
    ccec_full_ctx_decl_cp(cp, full_key);

    ok_or_fail(ccec_generate_key_fips(cp, rng, full_key) == 0, "Generated Key");

    if(verbose) diag("Test with keysize %u", expected_keysize);
    ok_or_goto(ccec_ctx_bitlen(full_key) == expected_keysize, "Generated correct keysize", errout);

    // SHA-1
    ok_or_goto(ecies_aes_gcm_crypt_decrypt(full_key,16,ccsha1_di(),rng,options), "ECIES AES GCM SHA1, Msg length 16", errout);
    ok_or_goto(ecies_aes_gcm_crypt_decrypt(full_key,1,ccsha1_di(),rng,options), "ECIES AES GCM SHA1, Msg length 1", errout);
    ok_or_goto(ecies_aes_gcm_crypt_decrypt(full_key,4096,ccsha1_di(),rng,options), "ECIES AES GCM SHA1, Msg length 4096", errout);
    ok_or_goto(ecies_aes_gcm_crypt_decrypt(full_key,4097,ccsha1_di(),rng,options), "ECIES AES GCM SHA1, Msg length 4097", errout);

    // SHA-256
    ok_or_goto(ecies_aes_gcm_crypt_decrypt(full_key,16,ccsha256_di(),rng,options), "ECIES AES GCM SHA256, Msg length 16", errout);
    ok_or_goto(ecies_aes_gcm_crypt_decrypt(full_key,1,ccsha256_di(),rng,options), "ECIES AES GCM SHA256, Msg length 1", errout);
    ok_or_goto(ecies_aes_gcm_crypt_decrypt(full_key,4096,ccsha256_di(),rng,options), "ECIES AES GCM SHA256, Msg length 4096", errout);
    ok_or_goto(ecies_aes_gcm_crypt_decrypt(full_key,4097,ccsha256_di(),rng,options), "ECIES AES GCM SHA256, Msg length 4097", errout);
    status = 1;
errout:
    return status;
}

typedef int (*test_func_t)(const struct ccecies_vector *test);
static int
ecies_gcm_vector_tests(test_func_t func, const struct ccecies_vector *test)
{
    size_t test_counter=0;
    int test_status=1;
    const struct ccecies_vector * current_test=&test[test_counter++];
    while (current_test->di!=NULL && test_status)
    {
        struct ccecies_vector ct = *current_test;
        test_status=func(&ct);
        //224 doesn't support compact option
        ccec_const_cp_t cp=ccec_cp_224();
        if (ccn_cmp( ccec_cp_n(cp), ccec_cp_p(cp), ccec_cp_p(ct.curve()) )){
            ct.options &= ~ECIES_EXPORT_PUB_STANDARD; //kill this option
            ct.options |= ECIES_EXPORT_PUB_COMPACT; //add this option instead
            test_status&=func(&ct); //and redo the test
        }
        current_test=&test[test_counter++];
    }
    return test_status;
}


static char* option_name(uint32_t option)
{

    if (ECIES_EXPORT_PUB_STANDARD == (option & ECIES_EXPORT_PUB_STANDARD)){
        return "ECIES_EXPORT_PUB_STANDARD";
    }
    else if (ECIES_EXPORT_PUB_COMPACT == (option & ECIES_EXPORT_PUB_COMPACT)){
        return "ECIES_EXPORT_PUB_COMPACT";
    } else
        return NULL;
}


static int options_test(struct ccrng_state *rng, uint32_t options)
{

    if(verbose) diag(option_name(options));

    ok(ecies_test(rng, 192, ccec_cp_192(),options),
       "ECIES with 192 bit EC Key");

    if( (options&ECIES_EXPORT_PUB_COMPACT)!= ECIES_EXPORT_PUB_COMPACT)
        ok(ecies_test(rng, 224, ccec_cp_224(),options),
       "ECIES with 224 bit EC Key");

    ok(ecies_test(rng, 256, ccec_cp_256(),options),
       "ECIES with 256 bit EC Key");
    ok(ecies_test(rng, 384, ccec_cp_384(),options),
       "ECIES with 384 bit EC Key");
    ok(ecies_test(rng, 521, ccec_cp_521(),options),
       "ECIES with 521 bit EC Key");
    if(verbose) diag_linereturn();

    char buf[133];
    strlcpy(buf, option_name(options), 133);
    strlcat(buf, "+ECIES_EPH_PUBKEY_IN_SHAREDINFO1", 133);
    if(verbose) diag(buf);
    ok(ecies_test(rng, 192, ccec_cp_192(),options+ECIES_EPH_PUBKEY_IN_SHAREDINFO1),
       "ECIES with 192 bit EC Key, public key in sharedInfo1");

    if( (options&ECIES_EXPORT_PUB_COMPACT)!= ECIES_EXPORT_PUB_COMPACT)
        ok(ecies_test(rng, 224, ccec_cp_224(),options+ECIES_EPH_PUBKEY_IN_SHAREDINFO1),
       "ECIES with 224 bit EC Key, public key in sharedInfo1");

    ok(ecies_test(rng, 256, ccec_cp_256(),options+ECIES_EPH_PUBKEY_IN_SHAREDINFO1),
       "ECIES with 256 bit EC Key, public key in sharedInfo1");
    ok(ecies_test(rng, 384, ccec_cp_384(),options+ECIES_EPH_PUBKEY_IN_SHAREDINFO1),
       "ECIES with 384 bit EC Key, public key in sharedInfo1");
    ok(ecies_test(rng, 521, ccec_cp_521(),options+ECIES_EPH_PUBKEY_IN_SHAREDINFO1),
       "ECIES with 521 bit EC Key, public key in sharedInfo1");
    if(verbose) diag_linereturn();

    return 0;
    
}

int ccecies(TM_UNUSED int argc, TM_UNUSED char *const *argv)
{
    struct ccrng_state *rng = global_test_rng;
    
    plan_tests(1838);

    if(verbose) diag("Negative tests");
    ok(ecies_gcm_vector_tests(ccecies_gcm_kat_negative_test, ccecies_aes_gcm_vectors),
       "AES GCM Negative tests");

    if(verbose) diag("KATs");
    ok(ecies_gcm_vector_tests(ccecies_gcm_kat_test, ccecies_aes_gcm_vectors),
       "AES GCM KAT");

    options_test(rng, ECIES_EXPORT_PUB_STANDARD);
    options_test(rng, ECIES_EXPORT_PUB_COMPACT);

    return 0;
}

#endif

