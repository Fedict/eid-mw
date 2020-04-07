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


#include <corecrypto/ccrsa.h>
#include <corecrypto/ccder_rsa.h>
const uint8_t *ccder_decode_rsa_priv(const ccrsa_full_ctx_t key, const uint8_t *der, const uint8_t *der_end) {
    cc_size n = ccrsa_ctx_n(key);
    cc_size pqn = n/2+1;
    cc_unit tmpP[pqn], tmpQ[pqn];
    ccrsa_priv_ctx_t privk = ccrsa_ctx_private(key);
	cc_unit version_0[1] = {0x00};
    
    der = ccder_decode_constructed_tl(CCDER_CONSTRUCTED_SEQUENCE, &der_end, der, der_end);
    der = ccder_decode_uint(1, version_0, der, der_end);
    der = ccder_decode_uint(n, ccrsa_ctx_m(key), der, der_end);
    der = ccder_decode_uint(n, ccrsa_ctx_e(key), der, der_end);
    der = ccder_decode_uint(n, ccrsa_ctx_d(key), der, der_end);
    
    // need to determine exactly how many units are needed for P&Q before stuffing them into the key.
    if((der = ccder_decode_uint(pqn, tmpP, der, der_end)) == NULL) return NULL;
    CCZP_N(ccrsa_ctx_private_zp(privk)) = ccn_nof(ccn_bitlen(pqn, tmpP));
    ccn_set(cczp_n(ccrsa_ctx_private_zp(privk)), CCZP_PRIME(ccrsa_ctx_private_zp(privk)), tmpP);
    cczp_init(ccrsa_ctx_private_zp(privk));
    
    if((der = ccder_decode_uint(pqn, tmpQ, der, der_end)) == NULL) return NULL;
    CCZP_N(ccrsa_ctx_private_zq(privk)) = ccn_nof(ccn_bitlen(pqn, tmpQ));
    ccn_set(cczp_n(ccrsa_ctx_private_zq(privk)), CCZP_PRIME(ccrsa_ctx_private_zq(privk)), tmpQ);
    cczp_init(ccrsa_ctx_private_zq(privk));
    
    der = ccder_decode_uint(cczp_n(ccrsa_ctx_private_zp(privk)), ccrsa_ctx_private_dp(privk), der, der_end);
    der = ccder_decode_uint(cczp_n(ccrsa_ctx_private_zq(privk)), ccrsa_ctx_private_dq(privk), der, der_end);
    der = ccder_decode_uint(cczp_n(ccrsa_ctx_private_zp(privk)), ccrsa_ctx_private_qinv(privk), der, der_end);
    if(der) cczp_init(ccrsa_ctx_zm(key));
    return der;
}
