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


#include "ccsrp_priv.h"
#include "ccdh_priv.h"

/******************************************************************************
 *  Client Side Routines
 *****************************************************************************/


int
ccsrp_client_start_authentication(ccsrp_ctx_t srp, struct ccrng_state *rng, void *A_bytes) {
    int status=CCSRP_ERROR_DEFAULT;
    cc_require((status=ccdh_generate_private_key(ccsrp_ctx_gp(srp),ccsrp_ctx_private(srp),rng)) == 0,errOut);
    ccsrp_generate_client_pubkey(srp);
    ccsrp_export_ccn(srp, ccsrp_ctx_public(srp), A_bytes);
    status=0;
errOut:
    return status;
}


int
ccsrp_client_process_challenge(ccsrp_ctx_t srp,
                               const void *username,
                               size_t password_len, const void *password,
                               size_t salt_len, const void *salt,
                               const void *B_bytes, void *M_bytes) {
    cc_size n = ccsrp_ctx_n(srp);
    cc_unit B[n];
    cc_unit u[n], x[n], k[n], v[n], S[n];
    cc_unit tmp1[n], tmp2[n], tmp3[n];
    ccn_zero_multi(n, B, u, x, k, v, tmp1, tmp2, tmp3, NULL);

    if (8*ccsrp_ctx_di(srp)->output_size > ccn_bitlen(ccsrp_ctx_n(srp),ccsrp_ctx_prime(srp))) {
        // u.x is of size hash output length * 2
        // this implementation requires sizeof(u)=sizeof(x)=hash_size <= sizeof(prime)
        return CCSRP_NOT_SUPPORTED_CONFIGURATION;
    }

    ccsrp_import_ccn(srp, B, B_bytes);
    cczp_modn(ccsrp_ctx_zp(srp), tmp1, n, B);
	if (ccn_is_zero(n, tmp1)) return CCSRP_SAFETY_CHECK; //SRP-6a safety check
    
    ccsrp_generate_u(srp, u, ccsrp_ctx_public(srp), B);

    cczp_modn(ccsrp_ctx_zp(srp), tmp1, n, u);
	if (ccn_is_zero(n, tmp1)) return CCSRP_SAFETY_CHECK; //SRP-6a safety check
    
    ccsrp_generate_x(srp, x, username, salt_len, salt, password_len, password);
    ccsrp_generate_k(srp, k);
    cczp_power(ccsrp_ctx_zp(srp), v, ccsrp_ctx_gp_g(srp), x);

    /* Client Side S = (B - k*(g^x)) ^ (a + ux) */
    ccsrp_generate_client_S(srp, S, k, x, u, B);

    // K = H(S)
    ccsrp_digest_ccn(srp, S, ccsrp_ctx_K(srp));
    
    ccsrp_generate_M(srp, username, salt_len, salt, ccsrp_ctx_public(srp), B);
    ccsrp_generate_H_AMK(srp, ccsrp_ctx_public(srp));
    CC_MEMCPY(M_bytes, ccsrp_ctx_M(srp), ccsrp_ctx_keysize(srp));
    ccn_zero_multi(n, B, u, x, k, v, tmp1, tmp2, tmp3, NULL);
    return 0;
}


bool
ccsrp_client_verify_session(ccsrp_ctx_t srp, const uint8_t *HAMK_bytes) {
    return srp.hdr->flags.authenticated = (cc_cmp_safe(ccsrp_ctx_keysize(srp), ccsrp_ctx_HAMK(srp), HAMK_bytes) == 0);
}

