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


#ifndef SRP_H
#define SRP_H

#include <corecrypto/ccn.h>
#include <corecrypto/cczp.h>
#include <corecrypto/ccdigest.h>
#include <corecrypto/ccdh.h>

/* Aliases for DH-style group params for SRP */
typedef ccdh_gp ccsrp_gp;
typedef ccdh_gp_t ccsrp_gp_t;
typedef ccdh_const_gp_t ccsrp_const_gp_t;

#define CCSRP_HDR_PAD 32
struct ccsrp_ctx_header {
    const struct ccdigest_info *di;
    ccsrp_const_gp_t gp;
    struct {
        unsigned int authenticated:1;
        unsigned int noUsernameInX:1;
    } flags;
    uint8_t           pad[CCSRP_HDR_PAD - (sizeof(struct ccdigest_info *)+
                        sizeof(ccsrp_const_gp_t)+sizeof(bool))];
    cc_unit              ccn[1];
} __attribute__((aligned(16)));

typedef struct ccsrp_ctx {
    struct ccsrp_ctx_header  hdr;
} __attribute__((aligned(16))) ccsrp_ctx;

struct ccsrp_ctx_body {
    struct ccsrp_ctx_header  hdr;
    cc_unit              ccn[];
} __attribute__((aligned(16)));

typedef union {
    ccsrp_ctx *_full;              // Constructor
    struct ccsrp_ctx_header *hdr;
    struct ccsrp_ctx_body *body;
} __attribute__((transparent_union)) ccsrp_ctx_t;

#define ccsrp_gpbuf_size(_gp_) (ccdh_ccn_size(_gp_)*3)
#define ccsrp_dibuf_size(_di_) ((_di_)->output_size*3)

/* Size of the context structure for the di and gp combo */
#define ccsrp_sizeof_srp(_di_,_gp_) sizeof(struct ccsrp_ctx_header)+\
    ccsrp_gpbuf_size(_gp_)+ccsrp_dibuf_size(_di_)

/* Use this to declare a context on the stack */
#define ccsrp_ctx_decl(_di_, _gp_, _name_) \
    cc_ctx_decl(ccsrp_ctx, ccsrp_sizeof_srp(_di_,_gp_), _name_)

/*
 Accessors to the context structure.
 */

#define ccsrp_ctx_gp(KEY)   (((ccsrp_ctx_t)(KEY)).hdr->gp)
#define ccsrp_ctx_di(KEY)   (((ccsrp_ctx_t)(KEY)).hdr->di)
#define ccsrp_ctx_zp(KEY)   (ccsrp_ctx_gp(KEY).zp)
#define ccsrp_ctx_gp_g(KEY)   (ccdh_gp_g(ccsrp_ctx_gp(KEY)))
#define ccsrp_ctx_gp_l(KEY)   (ccdh_gp_l(ccsrp_ctx_gp(KEY)))
#define ccsrp_ctx_n(KEY)      (ccdh_gp_n(ccsrp_ctx_gp(KEY)))
#define ccsrp_ctx_prime(KEY)  (ccdh_gp_prime(ccsrp_ctx_gp(KEY)))
#define ccsrp_ctx_ccn(KEY)  (((ccsrp_ctx_t)(KEY)).hdr->ccn)
#define ccsrp_ctx_pki_key(KEY,_N_) (ccsrp_ctx_ccn(KEY) + ccsrp_ctx_n(KEY) * _N_)
#define ccsrp_ctx_public(KEY)  (ccsrp_ctx_pki_key(KEY,0))
#define ccsrp_ctx_private(KEY)  (ccsrp_ctx_pki_key(KEY,1))
#define ccsrp_ctx_v(KEY)  (ccsrp_ctx_pki_key(KEY,2))
#define ccsrp_ctx_K(KEY) ((uint8_t *)(ccsrp_ctx_pki_key(KEY,3)))
#define ccsrp_ctx_M(KEY)    (uint8_t *)(ccsrp_ctx_K(KEY)+\
            ccsrp_ctx_di(KEY)->output_size)
#define ccsrp_ctx_HAMK(KEY) (uint8_t *)(ccsrp_ctx_K(KEY)+\
            2*ccsrp_ctx_di(KEY)->output_size)

/* Session Keys and M and HAMK are returned in this many bytes */
#define ccsrp_ctx_keysize(KEY)   (ccsrp_ctx_di(KEY)->output_size)

/* The public keys and the verifier are returned in this many bytes */
#define ccsrp_ctx_sizeof_n(KEY)   (ccn_sizeof_n(ccsrp_ctx_n(KEY)))

/* Init context structures with this function */

CC_INLINE void
ccsrp_ctx_init(ccsrp_ctx_t srp, const struct ccdigest_info *di, ccsrp_const_gp_t gp) {
    cc_zero(ccsrp_sizeof_srp(di, gp),srp.hdr);
    srp.hdr->di = di;
    srp.hdr->gp = gp;
    srp.hdr->flags.authenticated = false;
}

/******************************************************************************
 *  Error codes
 *****************************************************************************/

#define CCSRP_ERROR_DEFAULT                 CCDH_ERROR_DEFAULT
#define CCSRP_GENERATE_KEY_TOO_MANY_TRIES   CCDH_GENERATE_KEY_TOO_MANY_TRIES
#define CCSRP_NOT_SUPPORTED_CONFIGURATION   CCDH_NOT_SUPPORTED_CONFIGURATION
#define CCSRP_SAFETY_CHECK                  CCDH_SAFETY_CHECK
#define CCSRP_PUBLIC_KEY_MISSING            CCDH_PUBLIC_KEY_MISSING
#define CCSRP_INVALID_DOMAIN_PARAMETER      CCDH_INVALID_DOMAIN_PARAMETER

/******************************************************************************
 *  Salt and Verification Generation - used to setup an account.
 *****************************************************************************/

int
ccsrp_generate_salt_and_verification(ccsrp_ctx_t srp, struct ccrng_state *rng,
                                     const char *username,
                                     size_t password_len, const void *password,
                                     size_t salt_len, void *salt,
                                     void *verifier);

int
ccsrp_generate_verifier(ccsrp_ctx_t srp,
			const char *username,
			size_t password_len, const void *password, 
			size_t salt_len, const void *salt,
			void *verifier);

/******************************************************************************
 *  Server Side Routines
 *****************************************************************************/

/* Compute B, the server public key */
int
ccsrp_server_generate_public_key(ccsrp_ctx_t srp, struct ccrng_state *rng,
                                 const void *verifier, void *B_bytes);

/* Compute the session key material */
int
ccsrp_server_compute_session(ccsrp_ctx_t srp,
                              const void *username,
                              size_t salt_len, const void *salt,
                              const void *A_bytes);

/* Performs in one shot the server public key and the session key material */
int
ccsrp_server_start_authentication(ccsrp_ctx_t srp, struct ccrng_state *rng,
                                  const void *username,
                                  size_t salt_len, const void *salt,
                                  const void *verifier, const void *A_bytes,
                                  void *B_bytes);

bool
ccsrp_server_verify_session(ccsrp_ctx_t srp, const void *user_M,
                            void *HAMK_bytes);

/******************************************************************************
 *  Client Side Routines
 *****************************************************************************/

int
ccsrp_client_start_authentication(ccsrp_ctx_t srp, struct ccrng_state *rng,
                                  void *A_bytes);

int
ccsrp_client_process_challenge(ccsrp_ctx_t srp,
                               const void *username,
                               size_t password_len, const void *password,
                               size_t salt_len, const void *salt,
                               const void *B_bytes,
                               void *M_bytes);

bool
ccsrp_client_verify_session(ccsrp_ctx_t srp, const uint8_t *HAMK_bytes);

CC_INLINE bool
ccsrp_client_set_noUsernameInX(ccsrp_ctx_t srp, bool flag)
{
    return srp.hdr->flags.noUsernameInX = !!flag;
}


/******************************************************************************
 *  Functions for both sides
 *****************************************************************************/

CC_INLINE bool
ccsrp_is_authenticated(ccsrp_ctx_t srp) {
	return srp.hdr->flags.authenticated;
}


CC_INLINE size_t
ccsrp_exchange_size(ccsrp_ctx_t srp) {
    return ccsrp_ctx_sizeof_n(srp);
}

CC_INLINE size_t
ccsrp_session_size(ccsrp_ctx_t srp) {
    return ccsrp_ctx_keysize(srp);
}


CC_INLINE const void *
ccsrp_get_session_key(ccsrp_ctx_t srp, size_t *key_length) {
    *key_length = ccsrp_ctx_keysize(srp);
    return ccsrp_ctx_K(srp);
}

CC_INLINE size_t
ccsrp_get_session_key_length(ccsrp_ctx_t srp) {
    return ccsrp_ctx_keysize(srp);
}



/******************************************************************************
 *  Component Test Interface
 *****************************************************************************/

int
ccsrp_test_calculations(const struct ccdigest_info *di, ccsrp_const_gp_t gp,
                        const void *username,
                        size_t password_len, const void *password,
                        size_t salt_len, const void *salt,
                        size_t k_len, const void *k,
                        size_t x_len, const void *x,
                        size_t v_len, const void *v,
                        size_t a_len, const void *a,
                        size_t b_len, const void *b,
                        size_t A_len, const void *A,
                        size_t B_len, const void *B,
                        size_t u_len, const void *u,
                        size_t S_len, const void *S
                        );

#endif /* Include Guard */

