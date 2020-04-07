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

/******************************************************************************
 *  Component Test Interface
 *****************************************************************************/

CC_INLINE bool
ccsrp_component_equal(char *label, ccsrp_ctx_t srp, cc_unit *a, cc_unit *b) {
    bool retval = ccn_cmp(ccsrp_ctx_n(srp), a, b) == 0;
    if(!retval) {
        size_t bytes_n = ccsrp_ctx_sizeof_n(srp);
        cc_printf("ccsrp_test_calculations: mismatch for %s:\n", label);
        cc_printf("\t");
        dump_hex((void *) a, bytes_n);
        cc_printf("\t");
        dump_hex((void *) b, bytes_n);
        cc_printf("\n");
    }
    return retval;
}

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
                        )
{
    ccsrp_ctx_decl(di, gp, srp_c);
    ccsrp_ctx_decl(di, gp, srp_s);
    ccsrp_ctx_init(srp_c, di, gp);
    ccsrp_ctx_init(srp_s, di, gp);
    cc_size n = ccsrp_ctx_n(srp_c);
    cc_unit input_k[n], generated_k[n]; // x
    cc_unit input_x[n], generated_x[n]; // x
    cc_unit input_v[n]; // x
    cc_unit input_A[n]; // x
    cc_unit input_B[n]; // x
    cc_unit input_u[n], generated_u[n]; // x
    cc_unit input_S[n], generated_server_S[n], generated_client_S[n];
    
    ccsrp_import_ccn_with_len(srp_c, input_k, k_len, k);
    ccsrp_import_ccn_with_len(srp_c, input_x, x_len, x);
    ccsrp_import_ccn_with_len(srp_c, input_v, v_len, v);
    ccsrp_import_ccn_with_len(srp_c, ccsrp_ctx_private(srp_c), a_len, a);
    ccsrp_import_ccn_with_len(srp_c, ccsrp_ctx_private(srp_s), b_len, b);
    ccsrp_import_ccn_with_len(srp_c, input_A, A_len, A);
    ccsrp_import_ccn_with_len(srp_c, input_B, B_len, B);
    ccsrp_import_ccn_with_len(srp_c, input_u, u_len, u);
    ccsrp_import_ccn_with_len(srp_c, input_S, S_len, S);
    
    int retval = 0;
    
    // This requires x to be generated the same as the spec
    ccsrp_generate_x(srp_c, generated_x, username, salt_len, salt, password_len, password);
    if(!ccsrp_component_equal("x", srp_c, input_x, generated_x)) retval = -1;
    else cc_printf("x is correct\n");
    
    // These need to work and are ready to try out.
    ccsrp_generate_k(srp_c, generated_k);
    if(!ccsrp_component_equal("k", srp_c, input_k, generated_k)) retval = -1;
    else cc_printf("k is correct\n");
    ccsrp_generate_client_pubkey(srp_c);
    if(!ccsrp_component_equal("A", srp_c, input_A, ccsrp_ctx_public(srp_c))) retval = -1;
    else cc_printf("A is correct\n");

    // since x might be whacked, we'll use the input x
    ccsrp_generate_v(srp_c, input_x);
    if(!ccsrp_component_equal("v", srp_c, input_v, ccsrp_ctx_v(srp_c))) retval = -1;
    else cc_printf("v is correct\n");

    // since v might be whacked, we'll use the input v
    ccsrp_import_ccn_with_len(srp_s, ccsrp_ctx_v(srp_s), v_len, v);
    ccsrp_generate_server_pubkey(srp_s, input_k);
    if(!ccsrp_component_equal("B", srp_s, input_B, ccsrp_ctx_public(srp_s))) retval = -1;
    else cc_printf("B is correct\n");

    // 
    ccsrp_generate_u(srp_s, generated_u, input_A, input_B);
    if(!ccsrp_component_equal("u", srp_s, input_u, generated_u)) retval = -1;
    else cc_printf("u is correct\n");
    
    ccsrp_generate_server_S(srp_s, generated_server_S, input_u, input_A);
    if(!ccsrp_component_equal("ServerS", srp_s, input_S, generated_server_S)) retval = -1;
    else cc_printf("ServerS is correct\n");
    
    ccsrp_generate_client_S(srp_c, generated_client_S, input_k, input_x, input_u, input_B);
    if(!ccsrp_component_equal("ClientS", srp_c, input_S, generated_client_S)) retval = -1;
    else cc_printf("ClientS is correct\n");
    return retval;
}
