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


#import "ccn_unit.h"
#import "ECExportImport.h"
#import <corecrypto/ccder.h>
#import <corecrypto/ccrng.h>
#import <corecrypto/ccec.h>

static ccoid_t ccoid_secp192r1 = CC_EC_OID_SECP192R1;
static ccoid_t ccoid_secp256r1 = CC_EC_OID_SECP256R1;
static ccoid_t ccoid_secp521r1 = CC_EC_OID_SECP521R1;

unsigned char key_bin[] = {
    0x30, 0x77, 0x02, 0x01, 0x01, 0x04, 0x20, 0x79, 0xf4, 0x38, 0x5c, 0x35,
    0xe1, 0x97, 0xbf, 0xc7, 0x39, 0xc1, 0x2e, 0x40, 0x52, 0x9f, 0xd1, 0xf0,
    0x13, 0xa6, 0x94, 0xc5, 0xdc, 0x3b, 0x14, 0x5b, 0x08, 0x11, 0x28, 0xc5,
    0xb6, 0xc4, 0xd7, 0xa0, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d,
    0x03, 0x01, 0x07, 0xa1, 0x44, 0x03, 0x42, 0x00, 0x04, 0x96, 0xc8, 0x55,
    0x12, 0xa3, 0x7b, 0xa2, 0x30, 0xe0, 0x3f, 0x8a, 0x99, 0x60, 0xfa, 0x53,
    0xab, 0x6a, 0xb6, 0x9d, 0x1d, 0xa9, 0xcb, 0xbb, 0xa9, 0x4a, 0x4b, 0x5b,
    0xf0, 0x57, 0xee, 0x06, 0xf4, 0x79, 0x94, 0xbc, 0xad, 0x5c, 0x82, 0xc0,
    0x14, 0x08, 0x64, 0x86, 0x9b, 0x13, 0xfa, 0x94, 0x97, 0xc4, 0x24, 0xa0,
    0x6c, 0xdc, 0x8d, 0x56, 0x4b, 0x44, 0x0c, 0xc7, 0xb7, 0xdf, 0x1d, 0x39,
    0x16
};
unsigned int key_bin_len = 121;


unsigned char key_bin_priv_only[] = {
    0x30, 0x25, 0x02, 0x01, 0x01, 0x04, 0x20, 0x79, 0xf4, 0x38, 0x5c, 0x35,
    0xe1, 0x97, 0xbf, 0xc7, 0x39, 0xc1, 0x2e, 0x40, 0x52, 0x9f, 0xd1, 0xf0,
    0x13, 0xa6, 0x94, 0xc5, 0xdc, 0x3b, 0x14, 0x5b, 0x08, 0x11, 0x28, 0xc5,
    0xb6, 0xc4, 0xd7
};
unsigned int key_bin_priv_only_len = 39;


unsigned char key_bin_oid_only[] = {
    0x30, 0x31, 0x02, 0x01, 0x01, 0x04, 0x20, 0x79, 0xf4, 0x38, 0x5c, 0x35,
    0xe1, 0x97, 0xbf, 0xc7, 0x39, 0xc1, 0x2e, 0x40, 0x52, 0x9f, 0xd1, 0xf0,
    0x13, 0xa6, 0x94, 0xc5, 0xdc, 0x3b, 0x14, 0x5b, 0x08, 0x11, 0x28, 0xc5,
    0xb6, 0xc4, 0xd7, 0xa0, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d,
    0x03, 0x01, 0x07
};
unsigned int key_bin_oid_only_len = 51;


static ccec_const_cp_t ccec_cp_for_oid(ccoid_t oid)
{
    if (ccoid_equal(oid, ccoid_secp192r1)) {
        return ccec_cp_192();
    } else if (ccoid_equal(oid, ccoid_secp256r1)) {
        return ccec_cp_256();
    } else if (ccoid_equal(oid, ccoid_secp521r1)) {
        return ccec_cp_521();
    }
    return (ccec_const_cp_t){NULL};
}

@implementation ECExportImport

- (void)signVerify:(ccec_full_ctx_t) full
         verifyKey:(ccec_pub_ctx_t) public
              name:(NSString *)name
           withRng:(struct ccrng_state *) rng
{
    size_t siglen = ccec_sign_max_size(ccec_ctx_cp(full));
    uint8_t sig[siglen];
    uint8_t digest[24] = "012345678912345678901234";

    XCTAssertTrue(siglen > sizeof(digest), @"siglen large enough");

    siglen = sizeof(sig);
    XCTAssertEqual(ccec_sign(full, sizeof(digest), digest,
                             &siglen, sig, rng), 0, @"ccec_sign failed");
    bool ok;
    XCTAssertEqual(ccec_verify(public, sizeof(digest), digest, siglen, sig, &ok),
                   0, @"ccec_verify failed");
    XCTAssertTrue(ok, @"ecdsa_verify %@ ok", name);

}

- (void) doExportImportDER:(ccec_const_cp_t) cp
                   withOID:(ccoid_t)oid
                      name:(NSString *)name
{
    XCTestRNG(rng,"");

    ccec_full_ctx_decl_cp(cp, full);
    ccec_full_ctx_decl_cp(cp, full2);

    ccec_ctx_init(cp, full);
    ccec_generate_key(cp, rng, full);

    [self signVerify:full verifyKey:full name:name withRng:rng];

    size_t size;
    NSString *nametest;

    /*
     * no oid, with public
     */

    nametest  = [NSString stringWithFormat:@"no oid, public: %@", name];

    size = ccec_der_export_priv_size(full, (ccoid_t){ NULL }, 1);
    uint8_t public[size], public2[size];
    XCTAssertTrue(ccec_der_export_priv(full, (ccoid_t){ NULL }, 1, size, public) == 0, @"ccec_export_priv(NULL, 1)");
    ccec_ctx_init(cp, full2);
    XCTAssertTrue(ccec_der_import_priv(cp, size, public, full2) == 0, @"ccec_der_import_priv");
    XCTAssertTrue(ccec_der_export_priv(full2, (ccoid_t){ NULL }, 1, size, public2) == 0, @"ccec_export_priv(NULL, 1) 2");

    XCTAssertTrue(memcmp(public, public2, size) == 0, @"key same");

    [self signVerify:full2 verifyKey:full2 name:nametest withRng:rng];
    [self signVerify:full verifyKey:full2 name:nametest withRng:rng];
    [self signVerify:full2 verifyKey:full name:nametest withRng:rng];

    /*
     * no oid, no public
     */

    nametest = [NSString stringWithFormat:@"no oid, no public: %@", name];

    size = ccec_der_export_priv_size(full, (ccoid_t){ NULL }, 0);
    uint8_t nopublic[size];
    XCTAssertTrue(ccec_der_export_priv(full, (ccoid_t){ NULL }, 0, size, nopublic) == 0, @"ccec_export_priv(NULL, 0)");
    ccec_ctx_init(cp, full2);
    XCTAssertTrue(ccec_der_import_priv(cp, size, nopublic, full2) == 0, @"ccec_der_import_priv");

    [self signVerify:full verifyKey:full2 name:nametest withRng:rng];
    [self signVerify:full2 verifyKey:full name:nametest withRng:rng];

    /*
     * oid, no public
     */

    nametest = [NSString stringWithFormat:@"id, no public: %@", name];

    size = ccec_der_export_priv_size(full, oid, 0);
    uint8_t nopublicoid[size];
    XCTAssertTrue(ccec_der_export_priv(full, oid, 0, size, nopublicoid) == 0, @"ccec_export_priv(oid, 0)");
    ccec_ctx_init(cp, full2);
    XCTAssertTrue(ccec_der_import_priv(cp, size, nopublicoid, full2) == 0, @"ccec_der_import_priv");

    [self signVerify:full verifyKey:full2 name:nametest withRng:rng];
    [self signVerify:full2 verifyKey:full name:nametest withRng:rng];

    /*
     *
     */
    XCTestRNG_Done(rng);
}

- (void) testExportImportDERCP192 {
    [self doExportImportDER:ccec_cp_192() withOID:ccoid_secp192r1 name:@"secp192r1"];
}

- (void) testExportImportDERCP256 {
    [self doExportImportDER:ccec_cp_256() withOID:ccoid_secp256r1 name:@"secp256r1"];
}

- (void) testImportDER {

    ccec_full_ctx_decl(ccn_sizeof(521), full);

    ccoid_t oid;
    size_t n;
    ccec_const_cp_t cp;

    XCTAssertTrue(0 == ccec_der_import_priv_keytype(key_bin_len, key_bin, &oid, &n), @"ccec_der_import_priv_keytype() == 0");

    cp = ccec_cp_for_oid(oid);
    XCTAssertTrue((cp.zp), @"bad oid");

    ccec_ctx_init(cp, full);

    XCTAssertTrue(0 == ccec_der_import_priv(cp, key_bin_len, key_bin, full), @"ccec_der_import_priv() == 0");

    size_t size = ccec_der_export_priv_size(full, oid, 1);
    XCTAssertTrue(size == key_bin_len, @"key same size");

    uint8_t exp[size];
    XCTAssertTrue(0 == ccec_der_export_priv(full, oid, 1, size, exp), @"ccec_export_priv(oid, 1)");

    XCTAssertTrue(memcmp(exp, key_bin, size) == 0, @"key same");

    // validate roundtrip using key
}

- (void) testImportDEROIDOnly {

    ccec_full_ctx_decl(ccn_sizeof(521), full);

    ccoid_t oid;
    size_t n;
    ccec_const_cp_t cp;

    XCTAssertTrue(0 == ccec_der_import_priv_keytype(key_bin_oid_only_len, key_bin_oid_only, &oid, &n), @"ccec_der_import_priv_keytype() == 0");

    cp = ccec_cp_for_oid(oid);
    XCTAssertTrue((cp.zp), @"bad oid");

    ccec_ctx_init(cp, full);

    XCTAssertTrue(0 == ccec_der_import_priv(cp, key_bin_oid_only_len, key_bin_oid_only, full), @"ccec_der_import_priv() == 0");

    size_t size = ccec_der_export_priv_size(full, oid, 0);
    XCTAssertTrue(size == key_bin_oid_only_len, @"key same size");

    uint8_t exp[size];
    XCTAssertTrue(0 == ccec_der_export_priv(full, oid, 0, size, exp), @"ccec_export_priv(oid, 0)");

    XCTAssertTrue(memcmp(exp, key_bin_oid_only, size) == 0, @"key same");

    // validate roundtrip using key
}

- (void) testImportDERPrivOnly {

    ccec_full_ctx_decl(ccn_sizeof(521), full);

    ccoid_t oid;
    size_t n;
    ccec_const_cp_t cp;

    XCTAssertTrue(0 == ccec_der_import_priv_keytype(key_bin_priv_only_len, key_bin_priv_only, &oid, &n), @"ccec_der_import_priv_keytype() == 0");

    XCTAssertFalse((oid.oid), @"no oid");

    cp = ccec_curve_for_length_lookup(n * 8 /* bytes -> bits */,
        ccec_cp_192(), ccec_cp_224(), ccec_cp_256(), ccec_cp_384(), ccec_cp_521(), NULL);
    /* for lack of cp_equal, pointer match first element */
    XCTAssertTrue((cp.zp == ccec_cp_256().zp), @"bad oid");

    ccec_ctx_init(cp, full);

    XCTAssertTrue(0 == ccec_der_import_priv(cp, key_bin_priv_only_len, key_bin_priv_only, full), @"ccec_der_import_priv() == 0");

    size_t size = ccec_der_export_priv_size(full, oid, 0);
    XCTAssertTrue(size == key_bin_priv_only_len, @"key same size");

    uint8_t exp[size];
    XCTAssertTrue(0 == ccec_der_export_priv(full, oid, 0, size, exp), @"ccec_export_priv(NULL, 0)");

    XCTAssertTrue(memcmp(exp, key_bin_priv_only, size) == 0, @"key same");


    // validate roundtrip using key
}


@end
