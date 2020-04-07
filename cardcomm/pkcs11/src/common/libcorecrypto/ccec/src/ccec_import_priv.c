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


#include <corecrypto/ccder.h>
#include <corecrypto/ccec_priv.h>
#include <corecrypto/cc_macros.h>
#include <corecrypto/cc_debug.h>


int ccec_der_import_priv_keytype(size_t length, const uint8_t * data, ccoid_t *oid, size_t *n)
{
    uint64_t version;
    size_t priv_size = 0, pub_size = 0;
    const uint8_t *priv_key = NULL, *pub_key = NULL;
    ccoid_t key_oid;

    cc_require(ccder_decode_eckey(&version, &priv_size, &priv_key, &key_oid, &pub_size, &pub_key, data, data + length), out);

    /* oid is optional, may have to derive cp from private key length */
    *oid = key_oid;
    *n = priv_size;

    return 0;
out:
    return -1;
}


int ccec_der_import_priv(ccec_const_cp_t cp, size_t length, const uint8_t *data, ccec_full_ctx_t full_key)
{
    uint64_t version;
    size_t priv_size = 0, pub_size = 0;
    const uint8_t *priv_key = NULL, *pub_key = NULL;
    ccoid_t key_oid;

    cc_require(ccder_decode_eckey(&version, &priv_size, &priv_key, &key_oid, &pub_size, &pub_key, data, data + length), out);

    /* Load up private key */
    cc_require(priv_size == ccec_cp_prime_size(cp), out);
    cc_require(0 == ccn_read_uint(ccec_cp_n(cp), ccec_ctx_k(full_key), priv_size, priv_key), out);

    /* pub_size is partially checked by import pub: being odd */
    if (pub_key && (pub_size / 8 >= 2 * ccec_cp_prime_size(cp) + 1)) {
        cc_require(0 == ccec_import_pub(cp, pub_size / 8, pub_key, full_key.pub), out);
    } else {
        /* Calculate pub from priv if absent */
        ccec_point_decl_cp(cp, base);

        cc_require(0 == ccec_projectify(cp, base, ccec_cp_g(cp),NULL), out);
        if(ccec_mult(cp, ccec_ctx_point(full_key), ccec_ctx_k(full_key), base, NULL) != 0) goto out;
        if(ccec_affinify(cp, ccec_ctx_point(full_key), ccec_ctx_point(full_key)) != 0) goto out;
        ccn_seti(ccec_cp_n(cp), ccec_ctx_z(full_key), 1);
    }
    
    return 0;
out:
    return -1;
}

