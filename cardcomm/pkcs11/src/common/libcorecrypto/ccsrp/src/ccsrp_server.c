/*
 * Copyright (c) 2012,2014,2015 Apple Inc. All rights reserved.
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


#include "ccsrp_priv.h"
#include "ccdh_priv.h"

/******************************************************************************
 *  Server Side Routines
 *****************************************************************************/

int
ccsrp_server_generate_public_key(ccsrp_ctx_t srp, struct ccrng_state *rng,
                                  const void *verifier, void *B_bytes)
{
    int status=CCSRP_ERROR_DEFAULT;
    cc_size n = ccsrp_ctx_n(srp);
    cc_unit k[n];

    ccn_zero_multi(n, ccsrp_ctx_v(srp), k, NULL);
    ccsrp_import_ccn(srp, ccsrp_ctx_v(srp), verifier);

    srp.hdr->flags.authenticated = false;

    // Create b (ccsrp_ctx_private)
    cc_require((status=ccdh_generate_private_key(ccsrp_ctx_gp(srp),ccsrp_ctx_private(srp),rng)) == 0,errOut);

    // Generate parameter k
    ccsrp_generate_k(srp, k);

    /* B = kv + g^b */
    ccsrp_generate_server_pubkey(srp, k);
    ccsrp_export_ccn(srp, ccsrp_ctx_public(srp), B_bytes);
errOut:
    ccn_zero_multi(n, k, NULL);
    return status;
}

int
ccsrp_server_compute_session(ccsrp_ctx_t srp,
                             const void *username,
                             size_t salt_len, const void *salt,
                             const void *A_bytes)
{
    cc_size n = ccsrp_ctx_n(srp);
    cc_unit A[n],u[n], S[n];

    if (ccn_is_zero(n, ccsrp_ctx_public(srp))) return CCSRP_PUBLIC_KEY_MISSING;

    // Import A and sanity check on it
    ccsrp_import_ccn(srp, A, A_bytes);
    cczp_modn(ccsrp_ctx_zp(srp), u, n, A);
    if (ccn_is_zero(n, u)) return CCSRP_SAFETY_CHECK;

    /* u = H(A,B) */
    ccn_zero(n,u);
    ccsrp_generate_u(srp, u, A, ccsrp_ctx_public(srp));

    /* S = (A *(v^u)) ^ b */
    ccsrp_generate_server_S(srp, S, u, A);

    /* K = H(S) */
    ccsrp_digest_ccn(srp, S, ccsrp_ctx_K(srp));

    ccsrp_generate_M(srp, username, salt_len, salt, A, ccsrp_ctx_public(srp));
    ccsrp_generate_H_AMK(srp, A);
    
    ccn_zero_multi(n, A, u, S, NULL);
    return 0;
}



int
ccsrp_server_start_authentication(ccsrp_ctx_t srp, struct ccrng_state *rng,
        const void *username,
        size_t salt_len, const void *salt,
        const void *verifier, const void *A_bytes,  void *B_bytes)
{
    int status=CCSRP_ERROR_DEFAULT;
    
    // Generate server public key B
    cc_require((status = ccsrp_server_generate_public_key(srp, rng,
                                                          verifier, B_bytes))==0,errOut);
    /* We're done with that part of the handshake the rest now computes the remaining
     * handshake values K, M, and HAMK
     */

    // Generate session key material
    cc_require((status = ccsrp_server_compute_session(srp,
                                                  username,
                                                  salt_len, salt,
                                                  A_bytes))==0,errOut);

errOut:
    return status;
}


bool
ccsrp_server_verify_session(ccsrp_ctx_t srp, const void *user_M, void *HAMK_bytes) {
    srp.hdr->flags.authenticated = (cc_cmp_safe(ccsrp_ctx_keysize(srp), ccsrp_ctx_M(srp), user_M) == 0 );
    if(srp.hdr->flags.authenticated) CC_MEMCPY(HAMK_bytes, ccsrp_ctx_HAMK(srp), ccsrp_ctx_keysize(srp));
    return srp.hdr->flags.authenticated;
}

