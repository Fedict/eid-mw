/*
 * Copyright (c) 2010,2011,2012,2013,2014,2015 Apple Inc. All rights reserved.
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


#ifndef _CORECRYPTO_CCEC_PRIV_H_
#define _CORECRYPTO_CCEC_PRIV_H_

#include <corecrypto/ccec.h>
#include <corecrypto/cc_priv.h>
#include <corecrypto/cczp_priv.h>
#include <corecrypto/cczp.h>

/* Configuration */

#define CCEC_MASKING 1

/* Define this to 1 to use ccec_twin_mult for ccec_verify.  This gives you a
   big speedup for about 1500-2000 bytes of extra code. */
#define CCEC_USE_TWIN_MULT   1

/* Define the condition for Montgomery arithmetic support */
#define CCEC_ZP_IS_MONTGOMERY(cp)    (cczp_is_montgomery(ccec_cp_zp(cp)))

#define CCEC_DEBUG 0

/* Low level ec functions and types. */

/* Macros for accessing X and Y in an ccec_affine_point and X Y and Z in
   an ccec_projective_point. */
#define ccec_const_point_x(EP, _cp_)  (&((ccec_const_affine_point_t)(EP)).hdr->x)
#define ccec_const_point_y(EP, _cp_)  ((&((ccec_const_affine_point_t)(EP)).hdr->x) + ccec_cp_n(_cp_))
#define ccec_const_point_z(EP, _cp_)  ((&((ccec_const_projective_point_t)(EP)).hdr->x) + ccec_cp_n(_cp_) * 2)

#define ccec_point_x(EP, _cp_)  (&((ccec_affine_point_t)(EP)).hdr->x)
#define ccec_point_y(EP, _cp_)  ((&((ccec_affine_point_t)(EP)).hdr->x) + ccec_cp_n(_cp_))
#define ccec_point_z(EP, _cp_)  ((&((ccec_projective_point_t)(EP)).hdr->x) + ccec_cp_n(_cp_) * 2)

/* Macro to define a struct for a ccec_cp of _n_ units. This is
   only to be used for static initializers of curve parameters.
   Note that _n_ is evaluated multiple times. */
#define ccec_cp_decl_n(_n_)  struct { \
    struct cczp zp; \
    cc_unit p[(_n_)]; \
    cc_unit pr[(_n_) + 1]; \
    cc_unit b[(_n_)]; \
    cc_unit gx[(_n_)]; \
    cc_unit gy[(_n_)]; \
    struct cczp zq; \
    cc_unit q[(_n_)];\
    cc_unit qr[(_n_) + 1];\
}

/* Macro to define a struct for a ccec_cp of _bits_ bits. This is
   only to be used for static initializers of curve parameters. */
#define ccec_cp_decl(_bits_) ccec_cp_decl_n(ccn_nof(_bits_))

#define ccec_cp_p(_cp_)      ((_cp_).prime->ccn)
#define ccec_cp_zp(_cp_)     ((_cp_).zp)
#define ccec_cp_b(_cp_)      ((_cp_).prime->ccn + 1 + 2 * ccec_cp_n(_cp_))
#define ccec_cp_g(_cp_)      ((ccec_const_affine_point_t)(const ccec_affine_point *)((_cp_).prime->ccn + 1 + ccec_cp_n(_cp_) * 3))
#define ccec_cp_zq(_cp_)     ((cczp_const_t)((_cp_).prime->ccn + 1 + ccec_cp_n(_cp_) * 5))
#define ccec_cp_options(_cp_) (*(ccec_cp_zq(_cp_).prime->ccn + 1 + 2 * ccec_cp_n(_cp_)))

/* accept an affine point S and set R equal to its projective representation. */
int ccec_projectify(ccec_const_cp_t cp,
                     ccec_projective_point_t r,
                     ccec_const_affine_point_t s,
                     struct ccrng_state *masking_rng);

/* accept a projective point S and set R equal to its affine representation. */
int ccec_affinify(ccec_const_cp_t cp, ccec_affine_point_t r, ccec_const_projective_point_t s);

/* accept a projective point S and output the x coordinate only of its affine representation. */
int ccec_affinify_x_only(ccec_const_cp_t cp, cc_unit* sx, ccec_const_projective_point_t s);

/* Take a x coordinate a recompute the point. No particular convention for y */
int ccec_affine_point_from_x(ccec_const_cp_t cp, ccec_affine_point_t r, cc_unit *x, cc_unit *work2n);

/* Return true if the point is on the curve. Requires curve with a=-3 */
/* Z must be initialized. Set to 1 for points in affine representation */
bool ccec_is_point(ccec_const_cp_t cp, ccec_const_projective_point_t s);

/* accept an affine point S = (Sx,Sy) and return true if it is on the curve, (i.e., if SY2 = SX3 − 3SX.SZ^4 + bSZ^6 (mod p)), otherwise return false. */
bool ccec_is_point_projective(ccec_const_cp_t cp, ccec_const_projective_point_t s);

/* Validate the public key with respect to the curve information */
int ccec_validate_pub_and_projectify(ccec_projective_point_t r,
                                     const ccec_pub_ctx_t public_key,
                                     struct ccrng_state *masking_rng);

/* accept a projective point S and set R equal to the projective point 2S. Routine 2.2.6 performs no checks on its inputs. */
void ccec_double(ccec_const_cp_t cp, ccec_projective_point_t r, ccec_const_projective_point_t s);

/* accept two projective points S, T and set R equal to the projective point S + T . Routine 2.2.8 checks whether one of S or T is the point at infinity or whether S == T, and if so, takes the appropriate action. 
 Require r!=t. Ok with r==s */
void ccec_full_add(ccec_const_cp_t cp, ccec_projective_point_t r, ccec_const_projective_point_t s, ccec_const_projective_point_t t);

/* accept two projective points S, T and set R equal to the projective point S + T . Routine 2.2.8 checks whether one of S or T is the point at infinity or whether S == T, and if so, takes the appropriate action. 
    T is required to be the neutral element (1 or R if Montgomery) */
void ccec_full_add_normalized(ccec_const_cp_t cp, ccec_projective_point_t r, ccec_const_projective_point_t s, ccec_const_projective_point_t t);

/* accept two projective points S, T and set R equal to the projective point S − T . Routine 2.2.9 checks whether one of S or T is the point at infinity or whether S == T, and if so, takes the appropriate action. */
void ccec_full_sub(ccec_const_cp_t cp, ccec_projective_point_t r, ccec_const_projective_point_t s, ccec_const_projective_point_t t);

/* accept two projective points S, T and set R equal to the projective point S − T . Routine 2.2.9 checks whether one of S or T is the point at infinity or whether S == T, and if so, takes the appropriate action. 
    T is required to be the neutral element (1 or R if Montgomery)*/
void ccec_full_sub_normalized(ccec_const_cp_t cp, ccec_projective_point_t r, ccec_const_projective_point_t s, ccec_const_projective_point_t t);


/* accept a projective point S, an integer 1 ≤ d < q and 2 set R equal to the projective point dS. 
    Requires the point s to have been generated by "ccec_projectify" */
int ccec_mult(ccec_const_cp_t cp, ccec_projective_point_t r, const cc_unit *d,
                            ccec_const_projective_point_t s,
                            struct ccrng_state *masking_rng);

/* accept two projective points S, T , two integers 0 ≤ d0, d1 < p, and set R equal to the projective point d0S + d1T. */
void ccec_twin_mult(ccec_const_cp_t cp, ccec_projective_point_t r, const cc_unit *d0,
                    ccec_const_projective_point_t s,
                    const cc_unit *d1,
                    ccec_const_projective_point_t t);

/* Debugging */
void ccec_alprint(ccec_const_cp_t cp, const char *label, ccec_const_affine_point_t s);
void ccec_plprint(ccec_const_cp_t cp, const char *label, ccec_const_projective_point_t s);

void ccec_print_full_key(const char *label, ccec_full_ctx_t key);
void ccec_print_public_key(const char *label, ccec_pub_ctx_t key);
void ccec_print_sig(const char *label, unsigned long count, const uint8_t *s);

/*
 * EC key generation
 */

/* Not recommended: For legacy purposes or deterministic keys */
int
ccec_generate_key_internal_legacy(ccec_const_cp_t cp, struct ccrng_state *rng,
                           ccec_full_ctx_t key);

/* FIPS compliant and more secure */
int
ccec_generate_key_internal_fips(ccec_const_cp_t cp,  struct ccrng_state *rng,
                                ccec_full_ctx_t key);

int ccecdh_generate_key(ccec_const_cp_t cp,  struct ccrng_state *rng, ccec_full_ctx_t key);

/* 
 * EC Digital Signature - ECDSA
 */

int ccec_verify_internal(ccec_pub_ctx_t key, size_t digest_len, const uint8_t *digest,
                         cc_unit *r, cc_unit *s, bool *valid);

int ccec_sign_internal(ccec_full_ctx_t key, size_t digest_len, const uint8_t *digest,
                       cc_unit *r, cc_unit *s, struct ccrng_state *rng);


/*
 * RFC6637 wrap/unwrap
 */

#define ccec_rfc6637_ecdh_public_key_id    18
#define ccec_rfc6637_ecdsa_public_key_id   19

#define ccpgp_digest_sha256            8
#define ccpgp_digest_sha384            9
#define ccpgp_digest_sha512            10

#define ccpgp_cipher_aes128            7
#define ccpgp_cipher_aes192            8
#define ccpgp_cipher_aes256            9

struct ccec_rfc6637 {
    const char *name;
    const uint8_t kdfhash_id;
    const struct ccdigest_info * (*difun)(void);
    const uint8_t kek_id;
    const size_t keysize;
};

struct ccec_rfc6637_curve {
    const uint8_t *curve_oid;
    uint8_t public_key_alg;
};

extern struct ccec_rfc6637 ccec_rfc6637_sha256_kek_aes128;
extern struct ccec_rfc6637 ccec_rfc6637_sha512_kek_aes256;

void
ccec_rfc6637_kdf(const struct ccdigest_info *di,
                 const struct ccec_rfc6637_curve *curve,
                 const struct ccec_rfc6637 *wrap,
                 size_t epkey_size, const void *epkey,
                 size_t fingerprint_size, const void *fingerprint,
                 void *hash);

uint16_t
pgp_key_checksum(size_t key_len, const uint8_t *key);

#endif /* _CORECRYPTO_CCEC_PRIV_H_ */
