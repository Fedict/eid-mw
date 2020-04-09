/*
 * Copyright (c) 2011,2012,2013,2014,2015 Apple Inc. All rights reserved.
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


#ifndef _CORECRYPTO_CCRSA_PRIV_H_
#define _CORECRYPTO_CCRSA_PRIV_H_

#include <corecrypto/ccrsa.h>
#include <corecrypto/cc_priv.h>

// For reference, subject to change for unification accross corecrypto.

// Program error: buffer too small or encrypted message is too small
#define CCRSA_INVALID_INPUT                         -1
// Invalid crypto configuration: Hash length versus RSA key size
#define CCRSA_INVALID_CONFIG                        -2
// The data is invalid (we won't say more for security
#define CCRSA_DECRYPTION_ERROR                      -3

#define CCRSA_ENCODING_ERROR                        -4
#define CCRSA_DECODING_ERROR                        -5
#define CCRSA_SIGNATURE_GEN_ERROR                   -6
#define CCRSA_KEY_ERROR                             -7

// Key generation specific
#define CCRSA_KEYGEN_PRIME_NOT_FOUND                 -9
#define CCRSA_KEYGEN_PRIME_NEED_NEW_SEED             -10
#define CCRSA_KEYGEN_PRIME_TOO_MANY_ITERATIONS       -11
#define CCRSA_KEYGEN_PRIME_SEED_GENERATION_ERROR     -12
#define CCRSA_KEYGEN_MODULUS_CRT_INV_ERROR           -13
#define CCRSA_KEYGEN_NEXT_PRIME_ERROR                -14
#define CCRSA_KEYGEN_SEED_X_ERROR                    -15
#define CCRSA_KEYGEN_SEED_r_ERROR                    -16
#define CCRSA_KEYGEN_KEYGEN_CONSISTENCY_FAIL         -17
#define CCRSA_KEYGEN_R1R2_SIZE_ERROR                 -18
#define CCRSA_KEYGEN_PQ_DELTA_ERROR                  -19

/*!
 @brief emsa_pss_encode () encodes message M acording to EMSA-PSS in PKCS 1 V2


 @return 	    0:ok non-zero:error



 @param	di	    hash function (hLen denotes the length in octets of the hash function output)
 @param	sLen	intended length in octets of the salt
 @param   salt    salt for encoding
 @param   hLen    length of hash function. must be equal to di->output_size
 @param	mHash   the input that needs to be encoded. This is the hash of message M with length of hLen
 @param   emBits  maximal bit length of the integer OS2IP (EM) (see Section 4.2), at least 8hLen + 8sLen + 9. It is one bit smalller than modulus.
 @param   EM      encoded message output, an octet string of length emLen = ⎡emBits/8⎤

 <pre>
 @textblock
                                        +-------------+
                                        |  M[2^61-1]  |
                                        +-------------+
                                               |
                                               V
                                            Hash[hLen]
                                               |
                                               V
                               +--------+-------------+------------+
             M'[8+hLen+sLen] = |  0[8]  | mHash[hLen] | salt[sLen] |
                               +--------+-------------+------------+
                  t=emLen-sLen-hLen-2          |
                  +------+---+------------+    V
 DB[emLen-hLen-1]=| 0[t] | 1 | salt[sLen] |  Hash[hLen]
                  +------+---+------------+    |
                             |                 |
                             V dbMask[]        |
          [emLen- hLen-1]  xor <------- MGF <--|
                             |                 |
                             |                 |
                 bit 0       V                 V
                 +------------------------+----------+--+
      EM[emLen]= | maskedDB[emLen-hLen-1] | H[hLen]  |bc|
                 +------------------------+----------+--+
 @/textblock
 </pre>
 */

CC_NONNULL((1,2,4,6,8))
int ccrsa_emsa_pss_encode(const struct ccdigest_info* di, const struct ccdigest_info* MgfDi,
                          size_t sSize, const uint8_t *salt,
                          size_t hSize, const uint8_t *mHash,
                          size_t emBits, uint8_t *EM);
CC_NONNULL((1,2,5,7))
int ccrsa_emsa_pss_decode(const struct ccdigest_info* di, const struct ccdigest_info* MgfDi,
                    size_t sSize,
                    size_t mSize,  const uint8_t *mHash,
                    size_t emBits, const uint8_t *EM);

// EMSA
CC_NONNULL2 CC_NONNULL4 CC_NONNULL5
int ccrsa_emsa_pkcs1v15_encode(size_t emlen, uint8_t *em,
                               size_t dgstlen, const uint8_t *dgst,
                               const uint8_t *oid);

CC_NONNULL2 CC_NONNULL4 CC_NONNULL5
int ccrsa_emsa_pkcs1v15_verify(size_t emlen, uint8_t *em,
                               size_t dgstlen, const uint8_t *dgst,
                               const uint8_t *oid);


// MGF for OAEP

// Seed is di->output_size bytes, r_size must be >= di>output_size
CC_NONNULL1 CC_NONNULL3 CC_NONNULL5
int ccmgf(const struct ccdigest_info* di,
           size_t r_size, void* r,
           size_t seed_size, const void* seed);


// OAEP

/*
 r_size is the blocksize of the key for which the encoding is being done.
 */
CC_NONNULL((1, 2, 4, 6))
int ccrsa_oaep_encode_parameter(const struct ccdigest_info* di,
                                struct ccrng_state *rng,
                                size_t r_size, cc_unit *r,
                                size_t message_len, const uint8_t *message,
                                size_t parameter_data_len, const uint8_t *parameter_data);

/*
 r_size is the blocksize of the key for which the encoding is being done.
 */

CC_INLINE CC_NONNULL((1, 2, 4, 6))
int ccrsa_oaep_encode(const struct ccdigest_info* di,
                      struct ccrng_state *rng,
                      size_t r_size, cc_unit *r,
                      size_t message_len, const uint8_t *message)
{
    return ccrsa_oaep_encode_parameter(di, rng, r_size, r, message_len, message, 0, NULL);
}

/*
 r_len is the blocksize of the key for which the decoding is being done.
 */
CC_NONNULL((1, 2, 3, 5))
int ccrsa_oaep_decode_parameter(const struct ccdigest_info* di,
                                size_t *r_len, uint8_t *r,
                                size_t s_size, cc_unit *s,
                                size_t parameter_data_len, const uint8_t *parameter_data);


CC_INLINE CC_NONNULL((1, 2, 3, 5))
int ccrsa_oaep_decode(const struct ccdigest_info* di,
                      size_t *r_len, uint8_t *r,
                      size_t s_size, cc_unit* s)
{
    return ccrsa_oaep_decode_parameter(di, r_len, r, s_size, s, 0, NULL);
}

CC_NONNULL_TU((1,4,7)) CC_NONNULL((2, 3, 5, 6, 8))
int ccrsa_crt_makekey(cczp_t zm, const cc_unit *e, cc_unit *d, cczp_t zp, cc_unit *dp, cc_unit *qinv, cczp_t zq, cc_unit *dq);


/*!
 @function   ccrsa_eme_pkcs1v15_encode
 @abstract   Encode a key in PKCS1 V1.5 EME format prior to encrypting.

 @param      rng        A handle to an initialized rng state structure.
 @param      r_size     (In/Out) Result buffer size.
 @param      r          Result cc_unit buffer.
 @param      s_size     Source (payload) length.
 @param      s          Source buffer to be encoded.

 @result     Returns 0 on success, -1 otherwise.
 */

CC_NONNULL((1, 3, 5))
int ccrsa_eme_pkcs1v15_encode(struct ccrng_state *rng,
                              size_t r_size, cc_unit *r,
                              size_t s_size, const uint8_t *s);

/*!
 @function   ccrsa_eme_pkcs1v15_decode
 @abstract   Decode a payload in PKCS1 V1.5 EME format to a key after decrypting.

 @param      r_size     (In/Out) Result buffer size.
 @param      r          Result buffer.
 @param      s_size     Source (PKCS1 EME Payload) length.
 @param      s          Source cc_unit buffer to be decoded.

 @result     Returns 0 on success, -1 otherwise.
 */

CC_NONNULL((1, 2, 4))
int ccrsa_eme_pkcs1v15_decode(size_t *r_size, uint8_t *r,
                              size_t s_size, cc_unit *s);

/*!
 @function   ccrsa_encrypt_eme_pkcs1v15
 @abstract   Encode a key in PKCS1 V1.5 EME format and encrypt.

 @param      key        A public key to use to encrypt the package.
 @param      rng        A handle to an initialized rng state structure.
 @param      r_size     (In/Out) Result buffer size.
 @param      r          Result cc_unit buffer.
 @param      s_size     Source (payload) length.
 @param      s          Source buffer to be encoded.

 @result     Returns 0 on success, -1 otherwise.
 */

CC_NONNULL_TU((1)) CC_NONNULL((2, 3, 4, 6))
int ccrsa_encrypt_eme_pkcs1v15(ccrsa_pub_ctx_t key,
                           struct ccrng_state *rng,
                           size_t *r_size, uint8_t *r,
                           size_t s_size, const uint8_t *s);

/*!
 @function   ccrsa_decrypt_eme_pkcs1v15
 @abstract   Decrypt and decode a payload in PKCS1 V1.5 EME format to a key.

 @param      key        A private key to use to decrypt the package.
 @param      r_size     (In/Out) Result buffer size.
 @param      r          Result buffer.
 @param      s_size     Source (PKCS1 EME Payload) length.
 @param      s          Source buffer to be decoded.

 @result     Returns 0 on success, -1 otherwise.
 */


CC_NONNULL_TU((1)) CC_NONNULL((2, 3, 5))
int ccrsa_decrypt_eme_pkcs1v15(ccrsa_full_ctx_t key,
                           size_t *r_size, uint8_t *r,
                           size_t s_size, uint8_t *s);

/*!
 @function   ccrsa_encrypt_eme_pkcs1v15
 @abstract   Encode a key in PKCS1 V2.1 OAEP format and encrypt.

 @param      key        A public key to use to encrypt the package.
 @param      di         A descriptor for the digest used to encode the package.
 @param      rng        A handle to an initialized rng state structure.
 @param      r_size     (In/Out) Result buffer size.
 @param      r          Result cc_unit buffer.
 @param      s_size     Source (payload) length.
 @param      s          Source buffer to be encoded.
 @param      parameter_data_len Length of tag data (optional)
 @param      parameter_data Pointer to tag data (optional)

 @result     Returns 0 on success, -1 otherwise.
 */
CC_NONNULL_TU((1)) CC_NONNULL((2, 3, 4, 5, 7))
int ccrsa_encrypt_oaep(ccrsa_pub_ctx_t key,
                   const struct ccdigest_info* di,
                   struct ccrng_state *rng,
                   size_t *r_size, uint8_t *r,
                   size_t s_size, uint8_t *s,
                   size_t parameter_data_len,
                   const uint8_t *parameter_data);


/*!
 @function   ccrsa_decrypt_oaep
 @abstract   Decrypt and decode a payload in PKCS1 V2.1 OAEP format to a key.

 @param      key        A private key to use to decrypt the package.
 @param      di         A descriptor for the digest used to encode the package.
 @param      r_size     (In/Out) Result buffer size.
 @param      r          Result buffer.
 @param      c_size     Source (PKCS1 EME Payload) length.
 @param      c          Source buffer to be decoded.
 @param      parameter_data_len Length of tag data (optional)
 @param      parameter_data Pointer to tag data (optional)

 @result     Returns 0 on success, -1 otherwise.
 */

CC_NONNULL_TU((1)) CC_NONNULL((2, 3, 4, 6))
int ccrsa_decrypt_oaep(ccrsa_full_ctx_t key,
                   const struct ccdigest_info* di,
                   size_t *r_size, uint8_t *r,
                       size_t c_size, uint8_t *c,
                       size_t parameter_data_len,
                       const uint8_t *parameter_data);


CC_INLINE cc_size ccrsa_n_from_size(size_t size) {
    return ccn_nof_size(size);
}

CC_INLINE size_t ccrsa_sizeof_n_from_size(size_t size) {
    return ccn_sizeof_n(ccn_nof_size(size));
}

CC_INLINE uint8_t *ccrsa_block_start(size_t size, cc_unit *p, int clear_to_start) {
    size_t fullsize = ccrsa_sizeof_n_from_size(size);
    size_t offset = fullsize - size;
    if(clear_to_start) cc_zero(offset,p);
    return ((uint8_t *) p) + offset;
}

CC_INLINE size_t ccrsa_block_size(ccrsa_pub_ctx_t key) {
    return ccn_write_uint_size(ccrsa_ctx_n(key), ccrsa_ctx_m(key));
}

CC_NONNULL_TU((1))
bool ccrsa_pairwise_consistency_check(const ccrsa_full_ctx_t full_key);

/*
 * Display key in console for debug
 */
void ccrsa_dump_key(ccrsa_full_ctx_t key);


#endif
