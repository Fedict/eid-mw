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
#import "ECComputeKey.h"
#import <corecrypto/ccec.h>
#import <corecrypto/ccec_priv.h>
#include <stdlib.h>

@implementation ECComputeKey

- (void) evaluateComputeKey:(ccec_const_cp_t)cp : (const char *)cname
{
    ccec_full_ctx_decl_cp(cp, key1);
    ccec_full_ctx_decl_cp(cp, key2);
    ccec_pub_ctx_decl_cp(cp, keyp);

    uint8_t out1[ccec_ccn_size(cp)];
    uint8_t out2[ccec_ccn_size(cp)];
    size_t out1_len, out2_len;

    XCTestRNG(rng,"");

    XCTAssertEqual(ccec_generate_key(cp, rng, key1), 0,
                   @"ccec_generate_key %s failed", cname);
    XCTAssertEqual(ccec_generate_key(cp, rng, key2), 0,
                   @"ccec_generate_key %s failed", cname);
    out1_len = sizeof(out1);
    XCTAssertEqual(ccecdh_compute_shared_secret(key1, key2, &out1_len, out1, rng),
                   0, @"ccecdh_compute_shared_secret %s failed", cname);
    out2_len = sizeof(out2);
    XCTAssertEqual(ccecdh_compute_shared_secret(key2, key1, &out2_len, out2, rng),
                   0, @"ccecdh_compute_shared_secret %s failed", cname);
    XCTAssertEqual(out1_len, out2_len, @"compute_key %s length mismatch", cname);
    XCTAssertTrue(!memcmp(out1, out2, out1_len), @"compute_key %s mismatch", cname);

    /* Now test export/import. */
    uint8_t pub_exported[ccec_export_pub_size(key1)];
    ccec_export_pub(key1, pub_exported);
    XCTAssertEqual(ccec_import_pub(cp, sizeof(pub_exported), pub_exported, keyp),
                   0, @"ccec_import_pub %s", cname);
    out2_len = sizeof(out2);
    XCTAssertEqual(ccecdh_compute_shared_secret(key2, keyp, &out2_len, out2, rng),
                   0, @"ccecdh_compute_shared_secret %s failed", cname);
    XCTAssertEqual(out1_len, out2_len, @"compute_key %s length mismatch", cname);
    XCTAssertTrue(!memcmp(out1, out2, out1_len), @"compute_key %s mismatch", cname);
    
    XCTestRNG_Done(rng);
}

- (void) testP192 {
    [self evaluateComputeKey: ccec_cp_192() : "p192"];
}

- (void) testP224 {
    [self evaluateComputeKey: ccec_cp_224() : "p224"];
}

- (void) testP256 {
    [self evaluateComputeKey: ccec_cp_256() : "p256"];
}

- (void) testP384 {
    [self evaluateComputeKey: ccec_cp_384() : "p384"];
}

- (void) testP521 {
    [self evaluateComputeKey: ccec_cp_521() : "p521"];
}

@end
