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


#import "RSASignVerify.h"
#import "ccn_unit.h"
#import <corecrypto/ccrsa.h>
#import <corecrypto/ccrsa_priv.h>
#import <corecrypto/ccrng_test.h>
#import <corecrypto/ccsha1.h>
#import <corecrypto/ccsha2.h>
#import <corecrypto/ccrsa_test.h>
#include <stdlib.h>

/* Nist CAVP vectors specifies the hash as strings - those are matching hashes implementations */
/* We picked the implementations that are on all platform, it does not matter since we are not testing the hash here */
#define di_SHA1 &ccsha1_eay_di
#define di_SHA224 &ccsha224_ltc_di
#define di_SHA256 &ccsha256_ltc_di
#define di_SHA384 &ccsha384_ltc_di
#define di_SHA512 &ccsha512_ltc_di

/* Nist CAVP vectors for verify specify the result as F (failed) or P (passed)
  those translate as true or false */

#define P true
#define F false

const struct ccrsa_verify_vector verify_vectors_pkcs1v15[] = {
#include "../test_vectors/SigVer15.inc"
};


const struct ccrsa_verify_vector verify_vectors_ansix931[] = {
#include "../test_vectors/SigVerRSA.inc"
};

const struct ccrsa_verify_vector verify_vectors_pkcs1PSS[] = {
#include "../test_vectors/SigVerPSS.inc"
};

#undef P
#undef F

@implementation RSASignVerify

- (void) evaluateRawEncDec:(unsigned long)nbits withKeyType: (int) keytype {

    struct ccrng_test_state test_rng;
    ccrng_test_init(&test_rng, 16, "0000000000000000");

    struct ccrng_state *rng = (struct ccrng_state *)&test_rng;

    /* Exponent 3. */
    ccrsa_full_ctx_decl(ccn_sizeof(nbits), full_key);
    uint8_t e3[] = { 0x03 };
    
    if(keytype == 1) {
        XCTAssertEqual(ccrsa_generate_key(nbits, full_key, sizeof(e3), e3, rng), 0,
                   @"ccrsa_generate_key failed");
    } else {
        int retval = ccrsa_generate_931_key(nbits, full_key, sizeof(e3), e3, rng, rng);
        if(retval == CCRSA_INVALID_CONFIG) return; // invalid keysize
        XCTAssertEqual(retval, 0,
                   @"ccrsa_generate_key failed");
        if(retval) return;
    }

    ccrsa_full_ctx *fk = full_key;
    ccrsa_pub_ctx_t pub_key = ccrsa_ctx_public(fk);
    ccrsa_priv_ctx_t priv_key = ccrsa_ctx_private(fk);

#if 0
    /* Exponent 65537. */
    uint8_t e65537[] = { 0x01, 0x00, 0x01 };
    XCAssertEquals(ccrsa_generate_key(nbits, fk, sizeof(e65537), e65537,
                                      rng), 0, @"ccrsa_generate_key failed");
#endif

    cc_unit r[ccn_nof(nbits)], s[ccn_nof(nbits)], t[ccn_nof(nbits)];
    ccn_seti(ccn_nof(nbits), s, 42);

    ccn_set_bit(s, nbits-9, 1);
    XCTAssertEqual(ccrsa_priv_crypt(priv_key, r, s),0,@"ccrsa_priv_crypt failed");
    XCTAssertEqual(ccrsa_pub_crypt(pub_key, t, r),0,@"ccrsa_pub_crypt failed");
    XCAssertCCNEquals(ccn_nof(nbits), t, s, @"raw prv encrypt / pub decrypt");

    XCTAssertEqual(ccrsa_pub_crypt(pub_key, r, s),0,@"ccrsa_priv_crypt failed");
    XCTAssertEqual(ccrsa_priv_crypt(priv_key, t, r),0,@"ccrsa_pub_crypt failed");
    XCAssertCCNEquals(ccn_nof(nbits), t, s, @"raw pub encrypt / prv decrypt");

    ccrng_test_done(&test_rng);
}

- (void) evaluateSign:(unsigned long)nbits withKeyType: (int) keytype
{

    struct ccrng_test_state test_rng;
    ccrng_test_init(&test_rng, 16, "0000000000000000");
    
    struct ccrng_state *rng = (struct ccrng_state *)&test_rng;

    ccrsa_full_ctx_decl(ccn_sizeof(nbits), full_key);
    uint8_t digest[24] = "012345678912345678901234";
    uint8_t sig[(nbits+7)/8];
    size_t siglen=sizeof(sig);

    /* Exponent 3. */
    uint8_t e3[] = { 0x03 };
    if(keytype == 1) {
        XCTAssertEqual(ccrsa_generate_key(nbits, full_key, sizeof(e3), e3, rng), 0,
                       @"ccrsa_generate_key failed");
    } else {
        XCTAssertEqual(ccrsa_generate_931_key(nbits, full_key, sizeof(e3), e3, rng, rng), 0,
                       @"ccrsa_generate_key failed");
    }

    ccrsa_full_ctx *fk = full_key;
    ccrsa_pub_ctx_t pub_key = ccrsa_ctx_public(fk);
//unused:    ccrsa_priv_ctx_t priv_key = ccrsa_ctx_private(fk);

#if 0
    /* Exponent 65537. */
    uint8_t e65537[] = { 0x01, 0x00, 0x01 };
    XCAssertEquals(ccrsa_generate_key(nbits, full_key, sizeof(e65537), e65537,
                                      rng), 0, @"ccrsa_generate_key failed");
#endif

    XCTAssertEqual(ccrsa_sign_pkcs1v15(full_key, ccoid_sha1, CCSHA1_OUTPUT_SIZE, digest,  &siglen, sig),
                   0, @"ccrsa_sign failed");
    bool ok;
    XCTAssertEqual(ccrsa_verify_pkcs1v15(pub_key, ccoid_sha1, CCSHA1_OUTPUT_SIZE, digest, siglen, sig, &ok),
                   0, @"ccrsa_verify failed");
    XCTAssertTrue(ok, @"ccrsa_verify %lu bits failed (siglen=%lu)", nbits, siglen);

    ccrng_test_done(&test_rng);
}



- (void) testRawEncDec1024 {
    [self evaluateRawEncDec: 1024 withKeyType: 2];
    [self evaluateRawEncDec: 1024 withKeyType: 1];
}

#if 0

static int niblet_value(char c)
{
    if(isdigit(c)) return c - '0';
    return (c-'a') + 10;
}

static void str2ccn(cc_size n, cc_unit *r, char *s)
{
    size_t len = strlen(s);
    uint8_t bin[len/2];
    for(size_t i=0; i<len; i+=2) {
        bin[i/2] = niblet_value(s[i])*16 + niblet_value(s[i+1]);
    }
    (void) ccn_read_uint(n, r, len/2, bin);
}

// This currently fails to compile with an "out of registers" message.

- (void) testMake931Key {
    unsigned long alpha = 101;
    unsigned long nbits = 1024;
    int ret;
    cc_unit e65537[1] = { 65537 };
    cc_unit p1[ccn_nof(alpha)], p2[ccn_nof(alpha)], xp[ccn_nof(nbits/2)];
    cc_unit q1[ccn_nof(alpha)], q2[ccn_nof(alpha)], xq[ccn_nof(nbits/2)];
    cc_unit  p[ccn_nof(nbits/2)],  q[ccn_nof(nbits/2)],  m[ccn_nof(nbits)],  d[ccn_nof(nbits)];
    cc_unit rp[ccn_nof(nbits/2)], rq[ccn_nof(nbits/2)], rm[ccn_nof(nbits)], rd[ccn_nof(nbits)];
    
    str2ccn(ccn_nof(alpha),   p1, "155e67ddb99eefb13e4b77a7f0");
    str2ccn(ccn_nof(alpha),   p2, "17044df236c14e8ec333e92506");
    str2ccn(ccn_nof(nbits/2), xp, "d4f2b30f4f062ad2d05fc742e91bc20ca3ee8a2d126aff592c7de19edb3b884550ddd6f99b0a6b2b785617b46c0995bc112176dbae9a5b7f0bec678e84d6f44c");
    
    str2ccn(ccn_nof(alpha),   q1, "1e2923b103c935e3788ebd10e4");
    str2ccn(ccn_nof(alpha),   q2, "11a2ccec655a8b362b5ec5fcc4");
    str2ccn(ccn_nof(nbits/2), xq, "f7c6a68cff2467f300b82591e5123b1d1256546d999a37f4b18fe4896464df6987e7cc80efee3ce4e2f5c7a3cc085bbe33e4d375ed59cbc591f2b3302bd823bc");
    
    str2ccn(ccn_nof(nbits/2),  p, "d4f2b30f4f062ad2d05fc742e91bc20ca3ee8a2d126aff592c7de19edb3b884550ddd6f99b13e5dd56ffb2ac1867030f385597e712f65ac8dd1de502857c1a41");
    str2ccn(ccn_nof(nbits/2),  q, "f7c6a68cff2467f300b82591e5123b1d1256546d999a37f4b18fe4896464df6987e7cc80efeeb4c59165f7d1aec9be2b34889dbe221147e7ceefb5c9bd5cb945");
    str2ccn(ccn_nof(nbits),    m, "ce1b6904ec27f4a8f420414860704f4797a202ed16a9a35f63a16511a31675ccb046b02b192ef121b328385922f5faa032113332d42f84c70d4323133e216b0f339ebaf672f6214d0d7c13bea301174485ec44f44fae0e8a7f8d3c81ced5df77723331816158c3added7dc55f1436a7e5f14730be22cf3bebab1b62915c80c85");
    str2ccn(ccn_nof(nbits),    d, "18d16522721b5793169e61ae08eacd291641ac6f8718933313c8a5e66b487393dbb00f5b89334556e4ff5555aa678b2fca07972e2a2db4a3d15d81b639f7852ffe71657918d0280ff1be2f8f5d90b3e68195ab35e5069a3053540958bc6d58489fecf8baab0981f4af7b4db43550bcf01114e5ecdcb18f228db1c617b5d09781");

    cc_size np, nq, nd, nm;
    
    np = nq = ccn_nof(nbits/2);
    nd = nm = ccn_nof(nbits);
    ccrsa_full_ctx_decl(ccn_sizeof(nbits), full_key);
    
    ret = ccrsa_make_931_key(1024, 1, e65537,
                       ccn_nof(alpha), p1, ccn_nof(alpha), p2, ccn_nof(nbits/2), xp,
                       ccn_nof(alpha), q1, ccn_nof(alpha), q2, ccn_nof(nbits/2), xq,
                       full_key,
                       &np, rp, &nq, rq, &nm, rm, &nd, rd);
    XCAssertEquals(ret, 0, @"ccrsa_generate_key failed");

    XCAssertEquals(ccn_cmp(ccn_nof(nbits/2), rp, p), 0, @"ccrsa_make_931_key made incorrect P");
    XCAssertEquals(ccn_cmp(ccn_nof(nbits/2), rq, q), 0, @"ccrsa_make_931_key made incorrect Q");
    XCAssertEquals(ccn_cmp(ccn_nof(nbits), rm, m), 0, @"ccrsa_make_931_key made incorrect M");
    XCAssertEquals(ccn_cmp(ccn_nof(nbits), rd, d), 0, @"ccrsa_make_931_key made incorrect D");

}
#endif

struct SizeRanges {
    unsigned long first;
    unsigned long last;
};

const struct SizeRanges sizesToTest[] = 
{
    { .first = 1024, .last = 1088 },
    { .first = 1200, .last = 1200 },
    { .first = 1280, .last = 1280 },
    { .first = 2048, .last = 2048 },
    { .first = 2056, .last = 2056 }
};

- (void) runSignVerifyFrom: (size_t) smallest to: (size_t) largest {
    for (size_t size = smallest; size <= largest; size++) {
        printf("Testing %lu bits.\n", size);
        [self evaluateSign: size withKeyType: 1];
        [self evaluateSign: size withKeyType: 2];
    }
}

- (void) testSignVerifyList {
    const struct SizeRanges *pastLast = sizesToTest + (sizeof(sizesToTest)/sizeof(*sizesToTest));
    for(const struct SizeRanges *current = sizesToTest;
        current < pastLast;
        ++current) {
        [self runSignVerifyFrom: current->first to: current->last];
    }
}

- (void) testRSAVerifyPKCS1v15 {
    for (size_t i=0; i < sizeof(verify_vectors_pkcs1v15)/sizeof(verify_vectors_pkcs1v15[0]); i++) {
        const struct ccrsa_verify_vector *v=&verify_vectors_pkcs1v15[i];
        XCTAssertEqual(0, ccrsa_test_verify_pkcs1v15_vector(v), @"Vectors %u", (unsigned int)i);
    }
}

@end
