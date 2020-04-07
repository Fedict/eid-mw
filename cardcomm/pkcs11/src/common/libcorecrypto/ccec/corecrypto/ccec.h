/*
 * Copyright (c) 2010,2011,2012,2014,2015 Apple Inc. All rights reserved.
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


#ifndef _CORECRYPTO_CCEC_H_
#define _CORECRYPTO_CCEC_H_
// #define USE_SUPER_COOL_NEW_CCOID_T
#include <corecrypto/ccasn1.h>
#include <corecrypto/ccdigest.h>
#include <corecrypto/ccrng.h>
#include <corecrypto/cczp.h>
#include <stdbool.h>
#include <stdarg.h>


/* An ec_point. A ccec_projective_point_t is a point with x,y and z.
   A ccec_affine_point_t only has x and y. */
cc_aligned_struct(8) ccec_projective_point;

struct ccec_point_hdr {
    cc_unit x;
};

typedef union {
    struct ccec_point_hdr *hdr;
    ccec_projective_point *_p;
} __attribute__((transparent_union)) ccec_projective_point_t;

typedef union {
    const struct ccec_point_hdr *hdr;
    ccec_projective_point_t point;
    ccec_projective_point *_p;
    const ccec_projective_point *_cp;
} __attribute__((transparent_union)) ccec_const_projective_point_t;

cc_aligned_struct(8) ccec_affine_point;

typedef union {
    struct ccec_point_hdr *hdr;
    ccec_projective_point_t point;
    ccec_affine_point *_a;
    ccec_projective_point *_p;
} __attribute__((transparent_union)) ccec_affine_point_t;

typedef union {
    const struct ccec_point_hdr *hdr;
    const cc_unit *x;
    ccec_affine_point_t ap;
    ccec_projective_point_t p;
    ccec_const_projective_point_t cp;
    ccec_affine_point *_a;
    const ccec_affine_point *_ca;
    ccec_projective_point *_p;
    const ccec_projective_point *_cp;
} __attribute__((transparent_union)) ccec_const_affine_point_t;


/* A ccec_const_cp_t is a const pointer to a set of curve parameters.
   The first entry is a const (short) prime field. */
typedef union {
    const struct cczp   *zp;         // Accessor/constructor
    const struct cczp_prime *prime;  // Accessor to prime->ccn for the p in zp
} __attribute__((transparent_union)) ccec_const_cp_t;


/* Every ec context (a scheduled public or private key) starts with this. */
struct ccec_ctx_header {
    ccec_const_cp_t      cp;
    uint8_t              pad[16 - sizeof(ccec_const_cp_t *)];
} __attribute__((aligned(16)));

struct ccec_ctx_body {
    struct ccec_ctx_header  hdr;
    cc_unit              ccn[];
} __attribute__((aligned(16)));

struct ccec_ctx_public {
    struct ccec_ctx_header  hdr;
    ccec_projective_point point[];
} __attribute__((aligned(16)));

/* The ccec_full_ctx_decl macro allocates an array of ccec_full_ctx */
typedef struct ccec_full_ctx {
    struct ccec_ctx_header  hdr;
} __attribute__((aligned(16))) ccec_full_ctx;

/* The ccec_pub_ctx_decl macro allocates an array of ccec_pub_ctx */
typedef struct ccec_pub_ctx {
    struct ccec_ctx_header  hdr;
} __attribute__((aligned(16))) ccec_pub_ctx;

/* A ccec_full_ctx_t is a pointer to an ec key pair.  It should be
   allocated to be sizeof(ccec_full_ctx_decl()) bytes. Each of the
   ccns within an ec key is always ccec_ctx_n() cc_units long. */
typedef union {
    ccec_full_ctx *_full;              // Constructor
    struct ccec_ctx_header *hdr;
    struct ccec_ctx_body *body;
    struct ccec_ctx_public *pub;
} __attribute__((transparent_union)) ccec_full_ctx_t;

typedef union {
    ccec_pub_ctx *_pub;              // Constructor
    ccec_full_ctx *_full;            // Constructor
    struct ccec_ctx_header *hdr;
    struct ccec_ctx_body *body;
    struct ccec_ctx_public *pub;
    ccec_full_ctx_t fullt;           // Conversion constructor fk->pk
} __attribute__((transparent_union)) ccec_pub_ctx_t;

typedef union {
    const ccec_full_ctx *_cfull;     // Constructor
    ccec_full_ctx *_full;            // Constructor
    const struct ccec_ctx_header *hdr;
    const struct ccec_ctx_body *body;
    const struct ccec_ctx_public *pub;
    ccec_full_ctx_t _fullt;          // Conversion constructor fk->pk
} __attribute__((transparent_union)) ccec_full_ctx_in_t;

typedef union {
    const ccec_pub_ctx *_cpub;       // Constructor
    const ccec_full_ctx *_cfull;     // Constructor
    ccec_pub_ctx *_pub;              // Constructor
    ccec_full_ctx *_full;            // Constructor
    const struct ccec_ctx_header *hdr;
    const struct ccec_ctx_body *body;
    const struct ccec_ctx_pub *pub;
    ccec_full_ctx_t _fullt;          // Conversion constructor fk->pk
    ccec_full_ctx_in_t _cfullt;      // Conversion
} __attribute__((transparent_union)) ccec_pub_ctx_in_t;

/* Return the size of an ccec_full_ctx where each ccn is _size_ bytes. */
#define ccec_full_ctx_size(_size_)  (sizeof(struct ccec_ctx_header) + 4 * (_size_))
#define ccec_pub_ctx_size(_size_)   (sizeof(struct ccec_ctx_header) + 3 * (_size_))

/* Declare a fully scheduled ec key.  Size is the size in bytes each ccn in
 the key.  For example to declare (on the stack or in a struct) a 256 bit
 ec public key named foo use ccec_pub_ctx_decl(ccn_sizeof(256), foo). */
#define ccec_full_ctx_decl(_size_, _name_)  cc_ctx_decl(ccec_full_ctx, ccec_full_ctx_size(_size_), _name_)
#define ccec_full_ctx_clear(_size_, _name_) cc_clear(ccec_full_ctx_size(_size_), _name_)
#define ccec_pub_ctx_decl(_size_, _name_)   cc_ctx_decl(ccec_pub_ctx, ccec_pub_ctx_size(_size_), _name_)
#define ccec_pub_ctx_clear(_size_, _name_)  cc_clear(ccec_pub_ctx_size(_size_), _name_)
/* Declare storage for a fully scheduled ec key for a given set of curve
   parameters. */
#define ccec_full_ctx_decl_cp(_cp_, _name_) ccec_full_ctx_decl(ccec_ccn_size(_cp_), _name_)
#define ccec_full_ctx_clear_cp(_cp_, _name_) ccec_full_ctx_clear(ccec_ccn_size(_cp_), _name_)
#define ccec_pub_ctx_decl_cp(_cp_, _name_)  ccec_pub_ctx_decl(ccec_ccn_size(_cp_), _name_)
#define ccec_pub_ctx_clear_cp(_cp_, _name_) ccec_pub_ctx_clear(ccec_ccn_size(_cp_), _name_)

/* Declare storage for a projected or affine point respectively. */
#define ccec_point_decl_cp(_cp_, _name_) cc_ctx_decl(ccec_projective_point, 3 * ccec_ccn_size(_cp_), _name_)
#define ccec_point_clear_cp(_cp_, _name_) cc_clear(3 * ccec_ccn_size(_cp_), _name_)
#define ccec_affine_decl_cp(_cp_, _name_)  cc_ctx_decl(ccec_affine_point, 2 * ccec_ccn_size(_cp_), _name_)
#define ccec_affine_clear_cp(_cp_, _name_) cc_clear(2 * ccec_ccn_size(_cp_), _name_)

/* lvalue accessors to ccec_ctx fields. (only a ccec_full_ctx_t has K). */
//#define ccec_cp_n(CP)        ((CP).zp->n)
#define ccec_ctx_cp(KEY)     (((ccec_pub_ctx_t)(KEY)).hdr->cp)
#define ccec_ctx_n(KEY)      (((ccec_pub_ctx_t)(KEY)).hdr->cp.zp->n)
#define ccec_ctx_prime(KEY)  (((ccec_pub_ctx_t)(KEY)).hdr->cp.prime->ccn)

/* The public key as a projected point on the curve. */
#define ccec_ctx_point(KEY)    ((ccec_projective_point_t)(((ccec_pub_ctx_t)(KEY)).pub->point))
/* The ccn x, y and z of the public key as a projected point on the curve. */
#define ccec_ctx_x(KEY)  (((ccec_pub_ctx_t)(KEY)).body->ccn)
#define ccec_ctx_y(KEY)  (((ccec_pub_ctx_t)(KEY)).body->ccn + 1 * ccec_ctx_n(KEY))
#define ccec_ctx_z(KEY)  (((ccec_pub_ctx_t)(KEY)).body->ccn + 2 * ccec_ctx_n(KEY))
/* The ccn k of a full key which makes up the private key. */
#define ccec_ctx_k(KEY)      (((ccec_full_ctx_t)(KEY)).body->ccn + 3 * ccec_ctx_n(KEY))

/***************************************************************************/
/* EC Sizes                                                                */
/***************************************************************************/

/* Return the length of the prime for cp in bits. */
#define ccec_cp_prime_bitlen(CP) (ccn_bitlen((CP).zp->n, (CP).prime->ccn))
/* Return the length of the order for cp in bits. */
#define ccec_cp_order_bitlen(CP) (ccn_bitlen(cczp_n(ccec_cp_zq(cp)), cczp_prime(ccec_cp_zq(cp))))
/* Return the sizeof the prime for cp. */
#define ccec_cp_prime_size(CP) ((ccec_cp_prime_bitlen(CP)+7)/8)
/* Return the ec keysize in bits. */
#define ccec_ctx_bitlen(KEY) (ccec_cp_prime_bitlen(ccec_ctx_cp(KEY)))
/* Return the ec keysize in bytes. */
#define ccec_ctx_size(KEY) (ccec_cp_prime_size(ccec_ctx_cp(KEY)))


/* Callers must call this function to initialze a ccec_full_ctx or
   ccec_pub_ctx before using most of the macros in this file. */ 
CC_INLINE
void ccec_ctx_init(ccec_const_cp_t cp, ccec_pub_ctx_t key) {
    key.hdr->cp = cp;
}

/* rvalue accessors to ccec_ctx fields. */

/* Return count (n) of a ccn for cp. */
CC_CONST CC_INLINE
cc_size ccec_cp_n(ccec_const_cp_t cp) {
    return cp.zp->n;
}

/* Return sizeof a ccn for cp. */
CC_CONST CC_INLINE
size_t ccec_ccn_size(ccec_const_cp_t cp) {
    return ccn_sizeof_n(ccec_cp_n(cp));
}

/***************************************************************************/
/* EC Curve Parameters                                                     */
/***************************************************************************/

CC_CONST ccec_const_cp_t ccec_cp_192(void);
CC_CONST ccec_const_cp_t ccec_cp_224(void);
CC_CONST ccec_const_cp_t ccec_cp_256(void);
CC_CONST ccec_const_cp_t ccec_cp_384(void);
CC_CONST ccec_const_cp_t ccec_cp_521(void);

/***************************************************************************/
/* EC Wrap Params                                                          */
/***************************************************************************/

struct ccec_rfc6637_curve;
struct ccec_rfc6637_wrap;
struct ccec_rfc6637_unwrap;

extern struct ccec_rfc6637_wrap ccec_rfc6637_wrap_sha256_kek_aes128;
extern struct ccec_rfc6637_wrap ccec_rfc6637_wrap_sha512_kek_aes256;
extern struct ccec_rfc6637_unwrap ccec_rfc6637_unwrap_sha256_kek_aes128;
extern struct ccec_rfc6637_unwrap ccec_rfc6637_unwrap_sha512_kek_aes256;
extern struct ccec_rfc6637_curve ccec_rfc6637_dh_curve_p256;
extern struct ccec_rfc6637_curve ccec_rfc6637_dh_curve_p521;

/***************************************************************************/
/* EC Key Generation                                                       */
/***************************************************************************/

/* Default - Currently invoke the legacy function 20976009 */
CC_NONNULL_TU((1,3)) CC_NONNULL2
int ccec_generate_key(ccec_const_cp_t cp, struct ccrng_state *rng,
                      ccec_full_ctx_t key);

/* Not recommended: For legacy purposes - deterministic keys */
/*    2 * ccn_sizeof(ccec_cp_order_bitlen(cp)) of random bytes needed
 */
CC_NONNULL_TU((1,3)) CC_NONNULL2
int ccec_generate_key_legacy(ccec_const_cp_t cp,  struct ccrng_state *rng,
                             ccec_full_ctx_t key);

/* FIPS compliant and more secure */
/* Use a non deterministic amount of random bytes */
CC_NONNULL_TU((1,3)) CC_NONNULL2
int ccec_generate_key_fips(ccec_const_cp_t cp,  struct ccrng_state *rng,
                           ccec_full_ctx_t key);

/* Based on FIPS compliant version. Output a compact key */
/* Use a non deterministic amount of random bytes */
CC_NONNULL_TU((1,3)) CC_NONNULL2
int ccec_compact_generate_key(ccec_const_cp_t cp,  struct ccrng_state *rng,
                              ccec_full_ctx_t key);


/* Implementation per FIPS186-4 - "TestingCandidates" */
#define CCEC_GENERATE_KEY_DEFAULT_ERR     -1
#define CCEC_GENERATE_KEY_TOO_MANY_TRIES  -10
#define CCEC_GENERATE_KEY_MULT_FAIL       -11
#define CCEC_GENERATE_KEY_AFF_FAIL        -12
#define CCEC_GENERATE_KEY_CONSISTENCY     -13
#define CCEC_GENERATE_NOT_ON_CURVE        -14

/***************************************************************************/
/* EC SIGN/VERIFY  (ECDSA)                                                 */
/***************************************************************************/

/* Return the maximum buffer size needed to hold a signature for key. */
CC_INLINE CC_PURE CC_NONNULL_TU((1))
size_t ccec_sign_max_size(ccec_const_cp_t cp) {
    /* tag + 2 byte len + 2 * (tag + 1 byte len + optional leading zero + ccec_cp_prime_size) */
    return 3 + 2 * (3 + ccec_cp_prime_size(cp));
}

/*
    Signature in DER format 
*/
CC_NONNULL_TU((1)) CC_NONNULL((3, 4, 5, 6))
int ccec_sign(ccec_full_ctx_t key, size_t digest_len, const uint8_t *digest,
              size_t *sig_len, uint8_t *sig, struct ccrng_state *rng);
CC_NONNULL_TU((1)) CC_NONNULL((3, 5, 6))
int ccec_verify(ccec_pub_ctx_t key, size_t digest_len, const uint8_t *digest,
                size_t sig_len, const uint8_t *sig,  bool *valid);


/*
  Raw signature, big endian, padded to the key size.
 */
CC_NONNULL_TU((1))
size_t
ccec_signature_r_s_size(ccec_full_ctx_t key);

CC_NONNULL_TU((1)) CC_NONNULL((3, 4, 5, 6))
int ccec_sign_composite(ccec_full_ctx_t key, size_t digest_len, const uint8_t *digest,
                        uint8_t *sig_r, uint8_t *sig_s, struct ccrng_state *rng);

CC_NONNULL_TU((1)) CC_NONNULL((3, 4, 5, 6))
int ccec_verify_composite(ccec_pub_ctx_t key, size_t digest_len, const uint8_t *digest,
                          uint8_t *sig_r, uint8_t *sig_s, bool *valid);

/***************************************************************************/
/* EC Diffie-Hellman                                                       */
/***************************************************************************/

/*
   Deprecated. Do not use.
   Migrate existing calls to ccecdh_compute_shared_secret
 */

/*!
 @function   ccec_compute_key
 @abstract   DEPRECATED. Use ccecdh_compute_shared_secret.
 */

CC_NONNULL_TU((1,2)) CC_NONNULL((3, 4))
int ccec_compute_key(ccec_full_ctx_t private_key, ccec_pub_ctx_t public_key,
                     size_t *computed_key_len, uint8_t *computed_key);

/*!
 @function   ccecdh_compute_shared_secret
 @abstract   Elliptic Curve Diffie-Hellman
            from ANSI X9.63 and NIST SP800-56A, section 5.7.1.2

 @param  private_key                Input: EC private key
 @param  public_key                 Input: EC private key
 @param  computed_shared_secret_len Input: Size of allocate for computed_shared_secret.
                                    Output: Effective size of data in computed_shared_secret
 @param  computed_shared_secret     Output: DH shared secret

 @result 0 iff successful
 
 @discussion The shared secret MUST be transformed with a KDF function or at
             least Hash (SHA-256 or above) before being used.
             It shall not be used directly as a key.
 */

CC_NONNULL_TU((1,2)) CC_NONNULL((3, 4))
int ccecdh_compute_shared_secret(ccec_full_ctx_t private_key,
                                 ccec_pub_ctx_t public_key,
                                 size_t *computed_shared_secret_len, uint8_t *computed_shared_secret,
                                 struct ccrng_state *masking_rng);

/***************************************************************************/
/* EC WRAP/UNWRAP                                                          */
/***************************************************************************/
/*
 * Use rfc6637 style PGP wrapping for using EC keys
 *
 * Wrapped keys are always length hidden, so key part of output size fixed at 48 bytes.
 * This is hard coded and thus means we can only wrap keys up to 38 bytes.
 */

CC_NONNULL_TU((1))
size_t ccec_rfc6637_wrap_key_size(ccec_pub_ctx_t public_key,
                                  unsigned long flags,
                                  size_t key_len);

/*
 * When CCEC_RFC6637_COMPACT_KEYS flag is used, the wrapping is NOT
 * compatible with RFC6637 so make sure the peer supports this mode
 * before using it.  It currently saves half of the public key size
 * which for P256 is 32 bytes which end up being about 1/4 of the
 * wrapping size.
 */
#define CCEC_RFC6637_COMPACT_KEYS   1
#define CCEC_RFC6637_DEBUG_KEYS     2

CC_NONNULL_TU((1)) CC_NONNULL((2, 6, 7, 8, 9, 10))
int ccec_rfc6637_wrap_key(ccec_pub_ctx_t public_key,
                          void  *wrapped_key,
                          unsigned long flags,
                          uint8_t algid,
                          size_t key_len,
                          const void *key,
                          const struct ccec_rfc6637_curve *curve,
                          const struct ccec_rfc6637_wrap *wrap,
                          const uint8_t *fingerprint,
                          struct ccrng_state *rng);

CC_NONNULL_TU((1)) CC_NONNULL((2, 3, 5, 6, 7, 8, 10))
int ccec_rfc6637_unwrap_key(ccec_full_ctx_t private_key,
                            size_t *key_len,
                            void *key,
                            unsigned long flags,
                            uint8_t *symm_key_alg,
                            const struct ccec_rfc6637_curve *curve,
                            const struct ccec_rfc6637_unwrap *wrap,
                            const uint8_t *fingerprint,
                            size_t wrapped_key_len,
                            const void  *wrapped_key);

/***************************************************************************/
/* EC Import/Export                                                        */
/***************************************************************************/

CC_NONNULL_TU((1,4)) CC_NONNULL((3))
int ccec_import_pub(ccec_const_cp_t cp, size_t in_len, const uint8_t *in,
                    ccec_pub_ctx_t key);

/* Return the sizeof a buffer needed to exported public key to. */
CC_INLINE CC_CONST CC_NONNULL_TU((1))
size_t ccec_export_pub_size(ccec_pub_ctx_t key) {
    return 1 + 2 * ccec_cp_prime_size(ccec_ctx_cp(key));
}

/* Export key to out. Out must be ccec_export_pub_size(key) bytes long. */
CC_NONNULL_TU((1)) CC_NONNULL2
void ccec_export_pub(ccec_pub_ctx_t key, void *out);

/* ---------------------------------*/
/* DER (RFC 5480)                   */
/* ---------------------------------*/

/* Export EC priv to DER (RFC 5480) */
CC_NONNULL_TU((1,2))
size_t
ccec_der_export_priv_size(ccec_full_ctx_t key, ccoid_t key_oid, int includePublic);

CC_NONNULL_TU((1,2)) CC_NONNULL5
int
ccec_der_export_priv(ccec_full_ctx_t key, ccoid_t key_oid, int includePublic, size_t out_len, void *out);

/* import EC priv from DER (RFC 5480) */

CC_NONNULL_TU((3)) CC_NONNULL((2,4))
int ccec_der_import_priv_keytype(size_t len, const uint8_t * data, ccoid_t *oid, size_t *n);

CC_NONNULL_TU((1,4)) CC_NONNULL((3))
int ccec_der_import_priv(ccec_const_cp_t cp, size_t length, const uint8_t *data, ccec_full_ctx_t full_key);

/* ---------------------------------*/
/* x963							    */
/* ---------------------------------*/

/* Export 9.63 */
CC_INLINE CC_CONST CC_NONNULL_TU((2))
size_t ccec_x963_export_size(const int fullkey, ccec_full_ctx_t key){
    return (((ccec_ctx_bitlen(key)+7)/8) * ((fullkey == 1) + 2)) + 1;
}

CC_NONNULL_TU((3)) CC_NONNULL2
void ccec_x963_export(const int fullkey, void *out, ccec_full_ctx_t key);

/* Import 9.63 */
CC_NONNULL_TU((4)) CC_NONNULL3
int ccec_x963_import(const int fullkey, size_t in_len, const uint8_t *in, ccec_full_ctx_t key);

size_t ccec_x963_import_pub_size(size_t in_len);

CC_NONNULL_TU((1,4)) CC_NONNULL3
int ccec_x963_import_pub(ccec_const_cp_t cp, size_t in_len, const uint8_t *in, ccec_pub_ctx_t key);

size_t ccec_x963_import_priv_size(size_t in_len);

CC_NONNULL_TU((1,4)) CC_NONNULL3
int ccec_x963_import_priv(ccec_const_cp_t cp, size_t in_len, const uint8_t *in, ccec_full_ctx_t key);

/* ---------------------------------*/
/* Compact						    */
/* ---------------------------------*/

/* Export Compact 
 Output as the same bitlen than p */
CC_NONNULL_TU((3)) CC_NONNULL2
void ccec_compact_export(const int fullkey, void *out, ccec_full_ctx_t key);

CC_INLINE CC_CONST CC_NONNULL_TU((2))
size_t ccec_compact_export_size(const int fullkey, ccec_full_ctx_t key){
    return (((ccec_ctx_bitlen(key)+7)/8) * ((fullkey == 1) + 1));
}

/* Import Compact
 The public key is the x coordinate, in big endian, of length the byte length of p
 No preambule byte */
 
size_t ccec_compact_import_pub_size(size_t in_len);

CC_NONNULL_TU((1,4)) CC_NONNULL3
int ccec_compact_import_pub(ccec_const_cp_t cp, size_t in_len, const uint8_t *in, ccec_pub_ctx_t key);

size_t ccec_compact_import_priv_size(size_t in_len);

CC_NONNULL_TU((1,4)) CC_NONNULL3
int ccec_compact_import_priv(ccec_const_cp_t cp, size_t in_len, const uint8_t *in, ccec_full_ctx_t key);

/***************************************************************************/
/* EC Construction and Validation                                          */
/***************************************************************************/

CC_NONNULL_TU((1))
int ccec_get_pubkey_components(ccec_pub_ctx_t key, size_t *nbits,
                           uint8_t *x, size_t *xsize,
                           uint8_t *y, size_t *ysize);

CC_NONNULL_TU((1))
int ccec_get_fullkey_components(ccec_full_ctx_t key, size_t *nbits,
                            uint8_t *x, size_t *xsize,
                            uint8_t *y, size_t *ysize,
                            uint8_t *d, size_t *dsize);

CC_NONNULL_TU((6))
int ccec_make_pub(size_t nbits,
                  size_t xlength, uint8_t *x,
                  size_t ylength, uint8_t *y,
                  ccec_pub_ctx_t key);

CC_NONNULL_TU((8))
int ccec_make_priv(size_t nbits,
                   size_t xlength, uint8_t *x,
                   size_t ylength, uint8_t *y,
                   size_t klength, uint8_t *k,
                   ccec_full_ctx_t key);

int ccec_keysize_is_supported(size_t keysize);

ccec_const_cp_t ccec_get_cp(size_t keysize);

CC_NONNULL_TU((1)) CC_NONNULL2
bool ccec_pairwise_consistency_check(const ccec_full_ctx_t full_key, struct ccrng_state *rng);

const ccec_const_cp_t ccec_curve_for_length_lookup(size_t keylen, ...);

#endif /* _CORECRYPTO_CCEC_H_ */
