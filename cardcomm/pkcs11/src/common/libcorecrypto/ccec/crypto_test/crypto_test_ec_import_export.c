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

#include <corecrypto/ccder.h>
#include <corecrypto/ccrng.h>
#include <corecrypto/ccrng_ecfips_test.h>
#include <corecrypto/ccec.h>
#include <corecrypto/ccec_priv.h>
#include <corecrypto/cc_macros.h>
#include "crypto_test_ec.h"
#include <stdlib.h>

// Get a full key from the raw scalar
int ccec_recover_full_key(ccec_const_cp_t cp,size_t length, uint8_t *data,ccec_full_ctx_t key)
{
    int result=-1;
    struct ccrng_ecfips_test_state rng;

    cc_require(ccrng_ecfips_test_init(&rng, length, data) == 0,errOut);
    cc_require(ccec_generate_key_internal_fips(cp, (struct ccrng_state *)&rng, key) == 0,errOut);

    // No problem
    result=0;
errOut:
    return result;
}


static ccoid_t ccoid_secp192r1 = CC_EC_OID_SECP192R1;
static ccoid_t ccoid_secp256r1 = CC_EC_OID_SECP256R1;



static void
signVerify(ccec_full_ctx_t full, ccec_pub_ctx_t public, const char * __unused name, struct ccrng_state *rng)
{
    size_t siglen = ccec_sign_max_size(ccec_ctx_cp(full));
    uint8_t sig[siglen];
    uint8_t digest[24] = "012345678912345678901234";

    ok(siglen > sizeof(digest), "siglen large enough");

    siglen = sizeof(sig);
    is(ccec_sign(full, sizeof(digest), digest,
                             &siglen, sig, rng), 0, "ccec_sign failed");
    bool valid;
    is(ccec_verify(public, sizeof(digest), digest, siglen, sig, &valid),
                   0, "ccec_verify failed");
    ok(valid, "ecdsa_verify ok");
}

static void doExportImportDER_oneKAT(ccec_const_cp_t cp,struct ccrng_state *rng, const char* der_key_string)
{
    ccec_full_ctx_decl(ccec_ccn_size(cp), full);
    ccec_full_ctx_decl(ccec_ccn_size(cp), full2);
    ccec_ctx_init(cp, full);
    ccec_ctx_init(cp, full2);

    byteBuffer fullkey_der = hexStringToBytes(der_key_string);

    ok(ccec_der_import_priv(cp, fullkey_der->len, fullkey_der->bytes, full) == 0, "ccec_der_import_priv");
    byteBuffer fullkey_der_bis = mallocByteBuffer(ccec_der_export_priv_size(full, (ccoid_t){ NULL }, 1));
    ok(ccec_der_export_priv(full, (ccoid_t){ NULL }, 1, fullkey_der_bis->len, fullkey_der_bis->bytes) == 0, "ccec_export_priv(NULL, 1) 2");
    ok(ccec_der_import_priv(cp, fullkey_der_bis->len, fullkey_der_bis->bytes, full2) == 0, "ccec_der_import_priv");

    ok(fullkey_der->len==fullkey_der_bis->len, "key size same");
    ok(memcmp(fullkey_der->bytes, fullkey_der_bis->bytes,fullkey_der->len) == 0, "der key same");

    signVerify(full, full, "Imported key working", rng);    // If this pass, import was working
    signVerify(full2, full2, "Re-imported key working", rng);

    free(fullkey_der);
    free(fullkey_der_bis);
}

static void doExportImportDER_KATs(struct ccrng_state *rng)
{
    // Random
    doExportImportDER_oneKAT(ccec_cp_192(),rng,"30530201010418670902dd80999724dcf751f2bc2c340fb6f377312be93736a1340332000403a731d8bd0c192c8b732e887b4d69c852f9a583a2bbc37a73d7f46ae34c30880d0253193bb43eb33d1265a7379daac5");
    // Leading 0 in k
    doExportImportDER_oneKAT(ccec_cp_192(),rng,"3053020101041800f9cab2200806e4b51b23fa85cf258c67ab5839015f9667a13403320004f360dac619f7a286a84a84b9b7b9c586fe466ec06a61a2d4f4818fefd9ea2f521cd38677013a26909942261eaf5a586b");
    // Leading 0 in x
    doExportImportDER_oneKAT(ccec_cp_192(),rng,"305302010104182c3dfc53cba9a38028e89ffc1c1a390cf3218e284c792326a1340332000400d62388a47dbd308fa7810f45897da581b9b4e9565708de216e54ae118363544fd55e4be8980e61f1206d4608c5e804");
    // Leading 0 in y
    doExportImportDER_oneKAT(ccec_cp_256(),rng,"306b020101042086877959d1c63c502430a4af891dd194235679469372313924e60196c8ebf388a144034200048cfad78af1b9add73a33b59aad520d14d66b355679d6742a377e2f33a6abee35007082899cfc97c4895c1650ad6055a670ee071bfee4f0a063c07324979204c7");
}

static void
doExportImportDER(ccec_const_cp_t cp, ccoid_t oid, const char *name, struct ccrng_state *rng)
{
    char *testname;
    size_t size;

    ccec_full_ctx_decl_cp(cp, full);
    ccec_full_ctx_decl_cp(cp, full2);

    ccec_ctx_init(cp, full);
    is(ccec_generate_key_fips(cp, rng, full),0,"Generate key");

    signVerify(full, full, name, rng);

    /*
     * no oid, with public
     */

    asprintf(&testname, "no oid, public: %s", name);

    size = ccec_der_export_priv_size(full, (ccoid_t){ NULL }, 1);
    uint8_t public[size], public2[size];
    ok(ccec_der_export_priv(full, (ccoid_t){ NULL }, 1, size, public) == 0, "ccec_export_priv(NULL, 1)");
    ccec_ctx_init(cp, full2);
    ok(ccec_der_import_priv(cp, size, public, full2) == 0, "ccec_der_import_priv");
    ok(ccec_der_export_priv(full2, (ccoid_t){ NULL }, 1, size, public2) == 0, "ccec_export_priv(NULL, 1) 2");

    ok(memcmp(public, public2, size) == 0, "key same");

    if (ccn_cmp(ccec_cp_n(cp),ccec_ctx_k(full),ccec_ctx_k(full2))
        || ccn_cmp(ccec_cp_n(cp),ccec_ctx_x(full),ccec_ctx_x(full2))
        || ccn_cmp(ccec_cp_n(cp),ccec_ctx_y(full),ccec_ctx_y(full2))
        || ccn_cmp(ccec_cp_n(cp),ccec_ctx_z(full),ccec_ctx_z(full2)))
    {
        ccec_print_full_key("Generated key",full);
        ccec_print_full_key("Reconstructed key",full2);
        cc_print("Exported public key", sizeof(public),public);
        ok(false, "key reconstruction mismatch");
    }

    signVerify(full, full2, testname, rng);
    signVerify(full2, full, testname, rng);
    signVerify(full2, full2, testname, rng);

    free(testname);

    /*
     * no oid, no public
     */

    asprintf(&testname, "no oid, no public: %s", name);

    size = ccec_der_export_priv_size(full, (ccoid_t){ NULL }, 0);
    uint8_t nopublic[size];
    ok(ccec_der_export_priv(full, (ccoid_t){ NULL }, 0, size, nopublic) == 0, "ccec_export_priv(NULL, 0)");
    ccec_ctx_init(cp, full2);
    ok(ccec_der_import_priv(cp, size, nopublic, full2) == 0, "ccec_der_import_priv");

    signVerify(full2, full, testname, rng);
    signVerify(full, full2, testname, rng);
    signVerify(full2, full2, testname, rng);

    free(testname);

    /*
     * oid, no public
     */

    asprintf(&testname, "oid, no public:: %s", name);

    size = ccec_der_export_priv_size(full, oid, 0);
    uint8_t nopublicoid[size];
    ok(ccec_der_export_priv(full, oid, 0, size, nopublicoid) == 0, "ccec_export_priv(oid, 0)");
    ccec_ctx_init(cp, full2);
    ok(ccec_der_import_priv(cp, size, nopublicoid, full2) == 0, "ccec_der_import_priv");

    signVerify(full2, full, testname, rng);
    signVerify(full, full2, testname, rng);
    signVerify(full2, full2, testname, rng);

    free(testname);

}


int ccec_import_export(TM_UNUSED int argc, TM_UNUSED char *const *argv)
{
    struct ccrng_state *rng = global_test_rng;

    plan_tests(150);
    doExportImportDER_KATs(rng);
    doExportImportDER(ccec_cp_192(), ccoid_secp192r1, "secp192r1", rng);
    doExportImportDER(ccec_cp_256(), ccoid_secp256r1, "secp256r1", rng);

    return 0;
}
