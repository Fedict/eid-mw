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


#import "CCZKATValidation.h"
#import "ccz_unit.h"
#import "ccn_unit.h"
#import "string.h"

#import <corecrypto/ccz_priv.h>

struct test_alloc_context {
    unsigned test_allocs, test_frees, test_reallocs;
} gtac;

static void *
test_ccz_alloc(void *ctx, size_t size) {
    struct test_alloc_context *tac = ctx;
    tac->test_allocs++;
    return malloc(size);
}

static void
test_ccz_free(void *ctx, size_t oldsize, void *p) {
    struct test_alloc_context *tac = ctx;
    tac->test_frees++;
    cc_zero(oldsize, p);
    free(p);
}

static void *
test_ccz_realloc(void *ctx, size_t oldsize,
                 void *p, size_t newsize) {
    struct test_alloc_context *tac = ctx;
    tac->test_reallocs++;
    void *r = malloc(newsize);
    memcpy(r, p, oldsize);
    cc_zero(oldsize, p);
    free(p);
    return r;
}

struct ccz_class test_ccz_isa = {
	.ctx = &gtac,
	.ccz_alloc = test_ccz_alloc,
	.ccz_realloc = test_ccz_realloc,
	.ccz_free = test_ccz_free
};

static const uint8_t abytes[192/8] = { 0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0x21,0x22,0x23,0x24 };
static cc_unit a192[ccn_nof(192)] = {
    CCN192_C(01,02,03,04,05,06,07,08,09,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24)};
static cc_unit b192[ccn_nof(192)] = {
    CCN192_C(24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,09,08,07,06,05,04,03,02,01)};
static cc_unit c192[ccn_nof(192)] = {
    CCN192_C(00,00,00,00,00,00,17,16,15,14,13,12,11,10,09,00,00,00,00,00,00,00,00,00)};

static cc_unit lsr192a[ccn_nof(192)] = {
    CCN192_C(00,81,01,82,02,83,03,84,04,88,08,89,09,8A,0A,8B,0B,8C,0C,90,10,91,11,92)};
static cc_unit lsr192a8[ccn_nof(192)] = {
    CCN192_C(00,01,02,03,04,05,06,07,08,09,10,11,12,13,14,15,16,17,18,19,20,21,22,23)};
static cc_unit lsr192a64[ccn_nof(192)] = {
    CCN192_C(00,00,00,00,00,00,00,00,01,02,03,04,05,06,07,08,09,10,11,12,13,14,15,16)};
static cc_unit lsr192a65[ccn_nof(192)] = {
    CCN192_C(00,00,00,00,00,00,00,00,00,81,01,82,02,83,03,84,04,88,08,89,09,8A,0A,8B)};
static cc_unit lsl192a[ccn_nof(192)] = {
    CCN192_C(02,04,06,08,0A,0C,0E,10,12,20,22,24,26,28,2A,2C,2E,30,32,40,42,44,46,48)};
static cc_unit lsl256a64[ccn_nof(256)] = {
    CCN256_C(01,02,03,04,05,06,07,08,09,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,00,00,00,00,00,00,00,00)};
static cc_unit lsl256a65[ccn_nof(256)] = {
    CCN256_C(02,04,06,08,0A,0C,0E,10,12,20,22,24,26,28,2A,2C,2E,30,32,40,42,44,46,48,00,00,00,00,00,00,00,00)};
static cc_unit sum192ab[ccn_nof(192)] = {
    CCN192_C(25,25,25,25,25,1f,1f,1f,1f,25,25,25,25,25,25,1f,1f,1f,1f,25,25,25,25,25)};
static cc_unit diff192ba[ccn_nof(192)] = {
    CCN192_C(23,21,1f,1d,1b,13,11,0f,0d,05,03,00,fe,fc,fa,f2,f0,ee,ec,e4,e2,e0,de,dd)};
static cc_unit prod384ab[ccn_nof(384)] = {
    CCN384_C(00,24,6b,d5,60,0a,ce,aa,9d,a7,9e,a3,b5,d3,fd,0c,1e,32,47,5d,4a,2f,0a,dc,9d,d7,05,29,44,58,43,2e,1a,07,f8,cf,b1,9f,9a,a4,9b,a8,cd,09,5e,d4,6b,24)};
static cc_unit square384a[ccn_nof(384)] = {
    CCN384_C(00,01,04,0a,14,23,38,54,78,a5,e9,43,b6,41,e7,a8,85,7f,97,f3,7b,30,13,25,67,90,9f,93,6b,6e,53,18,be,40,e5,5a,9f,b3,95,68,06,6f,a2,9c,a2,62,dd,10)};

@implementation CCZKATValidation

- (void) setUp {
    a = malloc(ccz_size(&test_ccz_isa));
    b = malloc(ccz_size(&test_ccz_isa));
    c = malloc(ccz_size(&test_ccz_isa));
    d = malloc(ccz_size(&test_ccz_isa));
    r = malloc(ccz_size(&test_ccz_isa));

    ccz_init(&test_ccz_isa, a);
    ccz_init(&test_ccz_isa, b);
    ccz_init(&test_ccz_isa, c);
    ccz_init(&test_ccz_isa, d);
    ccz_init(&test_ccz_isa, r);

    a_c.n = ccn_n(ccn_nof(192), a192);
    a_c.u = a192;
    b_c.n = ccn_n(ccn_nof(192), b192);
    b_c.u = b192;
    c_c.n = ccn_n(ccn_nof(192), c192);
    c_c.u = c192;
    lsra_c.n = ccn_n(ccn_nof(192), lsr192a);
    lsra_c.u  = lsr192a;
    lsra8_c.n = ccn_n(ccn_nof(192), lsr192a8);
    lsra8_c.u  = lsr192a8;
    lsra64_c.n = ccn_n(ccn_nof(192), lsr192a64);
    lsra64_c.u  = lsr192a64;
    lsra65_c.n = ccn_n(ccn_nof(192), lsr192a65);
    lsra65_c.u  = lsr192a65;
    lsla_c.n = ccn_n(ccn_nof(192), lsl192a);
    lsla_c.u  = lsl192a;
    lsla64_c.n = ccn_n(ccn_nof(256), lsl256a64);
    lsla64_c.u  = lsl256a64;
    lsla65_c.n = ccn_n(ccn_nof(256), lsl256a65);
    lsla65_c.u  = lsl256a65;
    sumab_c.n = ccn_n(ccn_nof(192), sum192ab);
    sumab_c.u  = sum192ab;
    diffba_c.n = ccn_n(ccn_nof(192), diff192ba);
    diffba_c.u  = diff192ba;
    prodab_c.n = ccn_n(ccn_nof(384), prod384ab);
    prodab_c.u  = prod384ab;
    squarea_c.n = ccn_n(ccn_nof(384), square384a);
    squarea_c.u  = square384a;
    cc_zero(sizeof(gtac), &gtac);
    expected_reallocs = 0;
}

- (void) tearDown {
    ccz_free(r);
    ccz_free(d);
    ccz_free(c);
    ccz_free(b);
    ccz_free(a);

    free(r);
    free(d);
    free(c);
    free(b);
    free(a);

    XCTAssertEqual(gtac.test_allocs, gtac.test_frees, @"allocs == frees");
    XCTAssertEqual(gtac.test_reallocs, expected_reallocs, @"reallocs == 0");
}

- (void)test_bit {
    XCTAssertEqual(ccz_bit(&a_c, 0), (bool)0, @"bit 0 of a192 is 0.");
    XCTAssertEqual(ccz_bit(&a_c, 2), (bool)1, @"bit 2 of a192 is 1.");
    XCTAssertEqual(ccz_bit(&b_c, 0), (bool)1, @"bit 0 of b192 is 1.");
    XCTAssertEqual(ccz_bit(&b_c, 191), (bool)0, @"bit 191 of b192 is 0.");
    XCTAssertEqual(ccz_bit(&b_c, 189), (bool)1, @"bit 189 of b192 is 1.");
}

- (void)test_set_bit {
    ccz_zero(a);
    for (size_t bit = 0; bit < 128; ++bit) {
        XCTAssertEqual(ccz_bit(a, bit), (bool)0, @"bit %zu of a is 0.", bit);
        ccz_set_bit(a, bit, 1);
        XCTAssertEqual(ccz_bit(a, bit), (bool)1, @"bit %zu of a is now 1.", bit);
        ccz_set_bit(a, bit, 0);
        XCTAssertEqual(ccz_bit(a, bit), (bool)0, @"bit %zu of a is now 0 again.", bit);
    }
}

- (void)test_lsr {
    ccz_lsr(r, &a_c, 1);
    XCAssertCCZEquals(r, &lsra_c, @"ccz_lsr(r, a_c, 1) works");
    ccz_set(r, &a_c);
    ccz_lsr(r, r, 1);
    XCAssertCCZEquals(r, &lsra_c, @"ccz_lsr(r, r, 1) works");

    ccz_lsr(r, &a_c, 8);
    XCAssertCCZEquals(r, &lsra8_c, @"ccz_lsr(r, a_c, 8) works");
    ccz_set(r, &a_c);
    ccz_lsr(r, r, 8);
    XCAssertCCZEquals(r, &lsra8_c, @"ccz_lsr(r, r, 8) works");

    ccz_lsr(r, &a_c, 64);
    XCAssertCCZEquals(r, &lsra64_c, @"ccz_lsr(r, a_c, 64) works");
    ccz_set(r, &a_c);
    ccz_lsr(r, r, 64);
    XCAssertCCZEquals(r, &lsra64_c, @"ccz_lsr(r, r, 64) works");

    ccz_lsr(r, &a_c, 65);
    XCAssertCCZEquals(r, &lsra65_c, @"ccz_lsr(r, a_c, 65) works");
    ccz_set(r, &a_c);
    ccz_lsr(r, r, 65);
    XCAssertCCZEquals(r, &lsra65_c, @"ccz_lsr(r, r, 65) works");
}

- (void)test_lsl {
    ccz_lsl(r, &a_c, 1);
    XCAssertCCZEquals(r, &lsla_c, @"ccz_lsl(r, a_c, 1) works");
    ccz_set(r, &a_c);
    ccz_lsl(r, r, 1);
    XCAssertCCZEquals(r, &lsla_c, @"ccz_lsl(r, r, 1) works");

    ccz_lsl(r, &a_c, 64);
    XCAssertCCZEquals(r, &lsla64_c, @"ccz_lsl(r, a_c, 64) works");
    ccz_set(r, &a_c);
    ccz_lsl(r, r, 64);
    XCAssertCCZEquals(r, &lsla64_c, @"ccz_lsl(r, r, 64) works");

    ccz_lsl(r, &a_c, 65);
    XCAssertCCZEquals(r, &lsla65_c, @"ccz_lsl(r, a_c, 65) works");
    ccz_set(r, &a_c);
    ccz_lsl(r, r, 65);
    XCAssertCCZEquals(r, &lsla65_c, @"ccz_lsl(r, r, 65) works");
}


- (void)test_bitlen {
    XCTAssertEqual(ccz_bitlen(&a_c), (size_t)185, @"ccz_bitlen(r, a_c) works");
    XCTAssertEqual(ccz_bitlen(&b_c), (size_t)190, @"ccz_bitlen(r, b_c) works");
    XCTAssertEqual(ccz_bitlen(&c_c), (size_t)141, @"ccz_bitlen(r, c_c) works");
}

- (void)test_trailing_zeros {
    XCTAssertEqual(ccz_trailing_zeros(&a_c), (size_t)2, @"ccz_trailing_zeros(r, a_c) works");
    XCTAssertEqual(ccz_trailing_zeros(&b_c), (size_t)0, @"ccz_trailing_zeros(r, b_c) works");
    XCTAssertEqual(ccz_trailing_zeros(&c_c), (size_t)72, @"ccz_trailing_zeros(r, c_c) works");
}

- (void)test_is_zero {
    ccz_zero(r);
    XCTAssertEqual(ccz_is_zero(&a_c), (bool)false, @"ccz_is_zero(r, a_c) works");
    XCTAssertEqual(ccz_is_zero(r), (bool)true, @"ccz_is_zero(r, 0) works");
}

- (void)test_is_one {
    ccz_zero(r);
    XCTAssertEqual(ccz_is_one(&a_c), (bool)false, @"ccz_is_one(r, a_c) works");
    XCTAssertEqual(ccz_is_one(r), (bool)false, @"ccz_is_one(r, 0) works");
    ccz_seti(r, 1);
    XCTAssertEqual(ccz_is_one(r), (bool)true, @"ccz_is_one(r, 1) works");
}

- (void)test_basic {
    ccz_zero(a);
    XCTAssertEqual(ccz_is_zero(a), (bool)true, @"freshly initialized ccz is_zero");
    ccz_seti(b, 42);
    XCTAssertEqual(ccz_is_zero(b), (bool)false, @"42 not is_zero");

    XCTAssertEqual(ccz_is_negative(a), (bool)false, @"freshly initialized ccz is not negative");
    ccz_seti(c, 5047);
    ccz_neg(c);
    XCTAssertEqual(ccz_is_negative(c), (bool)true, @"-5047 is negative");

    XCTAssertEqual(ccz_is_one(a), (bool)false, @"freshly initialized ccz not is_one");
    XCTAssertEqual(ccz_is_one(b), (bool)false, @"42 not is_one");
    XCTAssertEqual(ccz_is_one(c), (bool)false, @"-5047 not is_one");
    ccz_seti(d, 1);
    XCTAssertEqual(ccz_is_one(d), (bool)true, @"1 is_one");
}

- (void)test_cmp {
    XCTAssertEqual(ccz_cmp(&a_c, &a_c), 0, @"ccz_cmp(r,a_c,a_c)");
    XCTAssertEqual(ccz_cmp(&a_c, &b_c), -1, @"ccz_cmp(r,a_c,b_c)");
    XCTAssertEqual(ccz_cmp(&b_c, &a_c), 1, @"ccz_cmp(r,b_c,a_c)");
}

// All code under test must be linked into the Unit Test bundle
- (void)test_sub {
    ccz_sub(r, &b_c, &a_c);
    XCAssertCCZEquals(r, &diffba_c, @"ccz_sub(r,b_c,a_c) works");

    ccz_sub(r, &a_c, &b_c);
    XCTAssertEqual(ccz_is_negative(r), (bool)true, @"a - b is negative");

    ccz_add(r, r, &b_c);
    XCAssertCCZEquals(r, &a_c, @"a_c - b_c + b_c = a_c");

    ccz_set(r, &b_c);
    ccz_sub(r, r, &a_c);
    XCAssertCCZEquals(r, &diffba_c, @"ccz_sub(r,r,a_c) works");

    ccz_set(r, &a_c);
    ccz_sub(r, &b_c, r);
    XCAssertCCZEquals(r, &diffba_c, @"ccz_sub(r,b_c,r) works");

    ccz_set(r, &a_c);
    ccz_sub(r, r, r);
    XCTAssertTrue(ccz_is_zero(r), @"after ccz_sub(r,r,r) r = 0");
}

#if 0
- (void)test_sub1 {
    cc_unit r[ccz_nof(_c)] = {},
            s[ccz_nof(_c)] = {},
            t[ccz_nof(_c)] = {};
    s[0] = a_c[0];
    XCAssertEquals(ccz_sub(ccz_nof(_c), t, r, s), (cc_unit)1,
                   @"r = 0, ccz_sub(r,r,a_c[0]) borrow");
    XCAssertEquals(ccz_sub1(r, r, &a_c[0]), (cc_unit)1,
                   @"r = 0, ccz_sub1(r,r,a_c[0]) borrow");
    XCAssertCCZEquals(r, t, @"ccz_sub(r,b_c,r) works");
    XCAssertEquals(ccz_sub1(r, &b_c, &a_c[0]), (cc_unit)0,
                   @"ccz_sub1(r,b_c,a_c[0]) no borrow");
}
#endif

// All code under test must be linked into the Unit Test bundle
- (void)test_add {
    ccz_add(r, &a_c, &b_c);
    XCAssertCCZEquals(r, &sumab_c, @"ccz_add(r,a,b) works");

    ccz_set(r, &a_c);
    ccz_add(r, r, &b_c);
    XCAssertCCZEquals(r, &sumab_c, @"ccz_add(r,r,b) works");

    ccz_set(r, &b_c);
    ccz_add(r, &a_c, r);
    XCAssertCCZEquals(r, &sumab_c, @"ccz_add(r,a,r) works");

    ccz_set(r, &b_c);
    ccz_add(r, &a_c, r);
    XCAssertCCZEquals(r, &sumab_c, @"ccz_add(r,a,r) works");

    ccz_set(r, &a_c);
    ccz_add(r, r, r);
    ccz_lsl(a, &a_c, 1);
    //XCAssertCCZEquals(a, r, @"ccz_add(r,r,r) yields same result as ccz_lsl(r, r, 1)");
}

#if 0
- (void)test_add1 {
    cc_unit r[ccz_nof(_c)] = {},
    s[ccz_nof(_c)] = {},
    t[ccz_nof(_c)] = {};
    s[0] = a_c[0];
    XCAssertEquals(ccz_add(ccz_nof(_c), t, &b_c, s), (cc_unit)0,
                   @"r = 0, ccz_add(r,r,a_c[0]) no carry");
    XCAssertEquals(ccz_add1(r, &b_c, &a_c[0]), (cc_unit)0,
                   @"r = 0, ccz_add1(r,r,a_c[0]) no carry");
    XCAssertCCZEquals(r, t, @"ccz_add(r,b_c,r) works");
    cc_unit max[1];
    max[0] = ~CC_UNIT_C(0);
    XCAssertEquals(ccz_add1(1, r, max, CC_UNIT_C(1)), (cc_unit)1,
                   @"r = 0, ccz_add1(r,~0,1) carry");
}
#endif

// All code under test must be linked into the Unit Test bundle
- (void)test_mul {
    ccz_mul(r, &a_c, &b_c);
    XCAssertCCZEquals(r, &prodab_c, @"ccz_mul(r,a_c,b_c) works");

#if 0
    ccz_div(r, r, &b_c);
    XCAssertCCZEquals(r, &a_c, @"a_c * b_c / b_c = a_c");
#endif

    ccz_set(r, &a_c);
    ccz_mul(r, r, &b_c);
    XCAssertCCZEquals(r, &prodab_c, @"r=a_c, ccz_mul(r,r,b_c) works");

    ccz_set(r, &b_c);
    ccz_mul(r, &a_c, r);
    XCAssertCCZEquals(r, &prodab_c, @"r=b_c, ccz_mul(r,a_c,r) works");

    ccz_mul(r, &a_c, &a_c);
    XCAssertCCZEquals(r, &squarea_c, @"ccz_mul(r,a_c,a_c) r = a_c^2");

    ccz_set(r, &a_c);
    ccz_mul(r, r, r);
    XCAssertCCZEquals(r, &squarea_c, @"r=a_c, ccz_mul(r,r,r) r = a_c^2");

    //making sure the swap in ccz_mul() works, when size of a and b are not equal
    ccz_readstr(a, "c968e40c5304364b057425920b18cc358f254ddb0f42f84850d6deec46006b4a692e52b7c3bddead45f77f2c1be1"
                   "c606521d8a24260429f362d65b57873dbf270e97e210b872e45e97cb4cd87977ad20491e53c48cf0e88da9a61312"
                   "675a2527c86ac537740c5e4206972f09c0f91fa1c9f14a2cf1be07e82a3b6fd58dc12c3a", 16);

    ccz_readstr(b,"d94a0fede757d782b54ddcf6fe8d714870b78b0e67a9754cb03a5cf63bbda1c71791902ea4527fb0cd76437391e542"
                  "2c704ffb6d6018261171d8cee98adcf0243f1fd520fb3761afe94a2f4d99f94", 16);
    ccz_readstr(r,"aaf42e28af692b7defeb473a1ef2d52ce90c3fa7f19e10212b22710ba3256fea16221e5a0e553c54ba5f03bff97220"
                  "c9b74904e52ec1731cec00cb569e86816e2c415828bd745ebff3674ea63b7a2207b96745689ba8ebf7cbd67a39fec5"
                  "a44c613e4ac197550b9cfeb3fcb97b274b6fbf04972627434b1ed50ec4c630d39359675a63921f9fd1fdfa6db57bd4"
                  "b13304ac0ac84d64c262d5802a1363ee1d519f88b8ca0997a77f7ece081042d88814da526c44f1323c7ac5b7eeedcc"
                  "da0e28bc65bc415bba767d34f161ab34f9788", 16);

    ccz_mul(d, a,b);
    XCAssertCCZEquals(d, r,@"ccz_mul(r,a,b) works");

    ccz_mul(d, b,a);
    XCAssertCCZEquals(d, r,@"ccz_mul(r,b,a) works");
}

- (void)test_gcd {
    ccz_set(a, &a_c);
    ccz_mul(b, &a_c, &b_c);
    ccz_gcd(c, b, a);
    XCAssertCCZEquals(c, a, @"ccz_gcd(a*b, a) == b");
}

- (void)test_read_uint {
    ccz_read_uint(r, 192 / 8, abytes);
    XCAssertCCZEquals(r, &a_c, @"ccz_read_uint(a)");
}

- (void)test_write_uint_size {
    XCTAssertEqual(ccz_write_uint_size(&a_c), (size_t)(192 / 8),
                   @"ccz_write_uint_size(a_c) == 192/8");
}

- (void)test_write_uint {
    size_t i_size = ccz_write_uint_size(&a_c);
    uint8_t bytes[i_size];
    ccz_write_uint(&a_c, i_size, bytes);
    XCAssertMemEquals(i_size, bytes, abytes, @"ccz_write_uint(a_c)");
}

- (void)test_write_uint_padded {
    static const uint8_t zeros[43] = {};
    size_t i_size = ccz_write_uint_size(&a_c);
    uint8_t bytes[i_size + 43];
    ccz_write_uint(&a_c, i_size + 43, bytes);
    XCAssertMemEquals(i_size, bytes + 43, abytes, @"ccz_write_uint_padded(a_c)");
    XCAssertMemEquals(43, bytes, zeros, @"ccz_write_uint_padded(a_c)");
}

- (void)test_write_int_size {
    XCTAssertEqual(ccz_write_int_size(&a_c), (size_t)(192 / 8), @"ccz_write_int_size(a_c) == 192/8");
    ccz_set(r, &a_c);
    ccz_set_bit(r, 191, 1);
    XCTAssertEqual(ccz_write_int_size(r), (size_t)(192 / 8) + 1, @"ccz_write_int_size(0x80 a_c) == 192/8 + 1");
}

- (void)test_write_int {
    size_t i_size = ccz_write_int_size(&a_c);
    uint8_t bytes[i_size];
    ccz_write_int(&a_c, i_size, bytes);
    XCAssertMemEquals(i_size, bytes, abytes, @"ccz_write_int(a_c)");

    ccz_set(r, &a_c);
    ccz_set_bit(r, 191, 1);
    size_t j_size = ccz_write_int_size(r);
    uint8_t jbytes[j_size];
    ccz_write_int(r, j_size, jbytes);
    XCAssertMemEquals(j_size - 1 - CCN_UNIT_SIZE, jbytes + 1 + CCN_UNIT_SIZE, abytes + CCN_UNIT_SIZE, @"ccz_write_int(0x80 a_c)");
    XCTAssertEqual(jbytes[0], (uint8_t)0, @"first byte is zero(a_c)");
}

- (void)test_read_radix2 {
    ccz_seti(c, 5047);

    char input_radix_sign[] = "+1001110110111";
    const char *input_radix_nosign=&input_radix_sign[1];

    // Positive
    XCTAssertEqual(ccz_read_radix(r, strlen(input_radix_nosign), input_radix_nosign, 2),(int)0, @"ccz_read_radix return value");
    XCTAssertEqual(ccz_cmp(c,r),(int)0, @"ccz_read_radix 1001110110111 base 2 is 5047");

    XCTAssertEqual(ccz_read_radix(r, strlen(input_radix_sign), input_radix_sign, 2),(int)0, @"ccz_read_radix return value");
    XCTAssertEqual(ccz_cmp(c,r),(int)0, @"ccz_read_radix +1001110110111 base 2 is 5047");

    // Negative
    input_radix_sign[0]='-';
    ccz_neg(c);
    XCTAssertEqual(ccz_read_radix(r, strlen(input_radix_sign), input_radix_sign, 2),(int)0, @"ccz_read_radix return value");
    XCTAssertEqual(ccz_cmp(c,r),(int)0, @"ccz_read_radix -1001110110111 base 2 is -5047");
}

- (void)test_read_radix16 {
    ccz_seti(c, 5047);

    char input_radix_sign[] = "+13B7";
    const char *input_radix_nosign=&input_radix_sign[1];

    // Positive
    XCTAssertEqual(ccz_read_radix(r, strlen(input_radix_nosign), input_radix_nosign, 16),(int)0, @"ccz_read_radix return value");
    XCTAssertEqual(ccz_cmp(c,r),(int)0, @"ccz_read_radix 13B7 base 16 is 5047");
    XCTAssertEqual(ccz_read_radix(r, strlen(input_radix_sign), input_radix_sign, 16),(int)0, @"ccz_read_radix return value");
    XCTAssertEqual(ccz_cmp(c,r),(int)0, @"ccz_read_radix +13B7 base 16 is 5047");

    // Negative
    input_radix_sign[0]='-';
    ccz_neg(c);
    XCTAssertEqual(ccz_read_radix(r, strlen(input_radix_sign), input_radix_sign, 16),(int)0, @"ccz_read_radix return value");
    XCTAssertEqual(ccz_cmp(c,r),(int)0, @"ccz_read_radix -13B7 base 16 is -5047");
}

- (void)test_read_radix10 {
    ccz_seti(c, 5047);

    char input_radix_sign[] = "+5047";
    const char *input_radix_nosign=&input_radix_sign[1];

    // Positive
    XCTAssertEqual(ccz_read_radix(r, strlen(input_radix_nosign), input_radix_nosign, 10),(int)0, @"ccz_read_radix return value");
    XCTAssertEqual(ccz_cmp(c,r),(int)0, @"ccz_read_radix 5047 base 10 is 5047");
    XCTAssertEqual(ccz_read_radix(r, strlen(input_radix_sign), input_radix_sign, 10),(int)0, @"ccz_read_radix return value");
    XCTAssertEqual(ccz_cmp(c,r),(int)0, @"ccz_read_radix +5047 base 10 is 5047");

    // Negative
    ccz_neg(c);
    input_radix_sign[0]='-';
    XCTAssertEqual(ccz_read_radix(r, strlen(input_radix_sign), input_radix_sign, 10),(int)0, @"ccz_read_radix return value");
    XCTAssertEqual(ccz_cmp(c,r),(int)0, @"ccz_read_radix -5047 base 10 is -5047");
}

- (void)test_read_radix_error {

    // Incorrect input
    const char *input_radix_minus = "-5047A";
    const char *input_radix_plus=&input_radix_minus[1];

    XCTAssertEqual(ccz_read_radix(r, strlen(input_radix_plus), input_radix_plus, 10),(int)CCZ_INVALID_INPUT_ERROR, @"Detect invalid input - Positive");
    ccz_neg(c);
    XCTAssertEqual(ccz_read_radix(r, strlen(input_radix_minus), input_radix_minus, 10),(int)CCZ_INVALID_INPUT_ERROR, @"Detect invalid input - Negative");

    // Incorrect radix
    XCTAssertEqual(ccz_read_radix(r, strlen(input_radix_plus)-1, input_radix_plus, 65),(int)CCZ_INVALID_RADIX_ERROR, @"Detect invalid radix - Too large");
    XCTAssertEqual(ccz_read_radix(r, strlen(input_radix_minus)-1, input_radix_minus, 0),(int)CCZ_INVALID_RADIX_ERROR, @"Detect invalid radix - Too zero");
}


- (void)test_write_radix16 {
    ccz_seti(c, 5047);
    ccz_neg(c);
    char buf[1024];

    size_t clen_b16 = ccz_write_radix_size(c, 16);
    XCTAssertEqual(clen_b16, (size_t)5, @"ccz_write_radix_size -5047 base 16 is 5");
    buf[0] = (char)0xde;
    buf[1 + clen_b16] = (char)0xed;
    ccz_write_radix(c, clen_b16, &buf[1], 16);
    XCTAssertEqual(buf[0], (char)0xde, @"begin marker ok");
    XCTAssertEqual(buf[1 + clen_b16], (char)0xed, @"end marker ok");
    XCAssertCharsEquals(clen_b16, &buf[1], "-13B7", @"ccz_write_radix -5047 base 16 is -13B7");
}

- (void)test_write_radix10 {
    ccz_seti(c, 5047);
    ccz_neg(c);
    char buf[1024];

    size_t clen_b10 = ccz_write_radix_size(c, 10);
    XCTAssertEqual(clen_b10, (size_t)5, @"ccz_write_radix_size -5047 base 10 is 5");
    buf[0] = (char)0xde;
    buf[1 + clen_b10] = (char)0xed;
    ccz_write_radix(c, clen_b10, &buf[1], 10);
    XCTAssertEqual(buf[0], (char)0xde, @"begin marker ok");
    XCTAssertEqual(buf[1 + clen_b10], (char)0xed, @"end marker ok");
    XCAssertCharsEquals(5, &buf[1], "-5047", @"ccz_write_radix -5047 base 10 is -5047");
}

- (void)test_write_radix2 {
    ccz_seti(c, 5047);
    ccz_neg(c);
    char buf[1024];

    size_t clen_b2 = ccz_write_radix_size(c, 2);
    XCTAssertEqual(clen_b2, (size_t)14, @"ccz_write_radix_size -5047 base 2 is 14");
    buf[0] = (char)0xde;
    buf[1 + clen_b2] = (char)0xed;
    ccz_write_radix(c, clen_b2, &buf[1], 2);
    XCTAssertEqual(buf[0], (char)0xde, @"begin marker ok");
    XCTAssertEqual(buf[1 + clen_b2], (char)0xed, @"end marker ok");
    XCAssertCharsEquals(clen_b2, &buf[1], "-1001110110111", @"ccz_write_radix -5047 base 2 is -1001110110111");
}

static inline int ccz_readstr(ccz *r, const char *str, unsigned radix) {
    return ccz_read_radix(r, strlen(str), str, radix);
}

- (void)test_mod {
    XCTAssertEqual(ccz_readstr(a, "c968e40c5304364b057425920b18cc358f254ddb0f42f84850d6deec46006b4a692e52b7c3bddead45f77f2c1be1c606521d8a24260429f362d65b57873dbf270e97e210b872e45e97cb4cd87977ad20491e53c48cf0e88da9a61312675a2527c86ac537740c5e4206972f09c0f91fa1c9f14a2cf1be07e82a3b6fd58dc12c3a", 16),(int)0, @"ccz_readstr: ccz_read_radix unexpected failure");
    XCTAssertEqual(ccz_readstr(c, "354c912b09ee7abff5b3d94ed52a9e8dcae582e094daa375c495f970710af73efcc4f9776010511f654c7408a6d5d351ab1d94a0fede757d782b54ddcf6fe8d714870b78b0e67a9754cb03a5cf63bbda1c71791902ea4527fb0cd76437391e5422c704ffb6d6018261171d8cee98adcf0243f1fd520fb3761afe94a2f4d99f94", 16),(int)0, @"ccz_readstr: ccz_read_radix unexpected failure");
    XCTAssertEqual(ccz_readstr(r, "2983308b3538c60b245899a58b98f08c2e74c53950b30de70314f29af2df858d72df6651a38ceb4f1612231227604c1150c4cc412968c97afa545cbe18ee04a1d102bfa6a5bf7498996a41e70b4c7991f3c9e87984321915b87f8ce5c1aeca2b6015b6384f8a59bae351d662f52f1634c3257434fb8eed85d93fb1ecaf344d7e", 16),(int)0, @"ccz_readstr: ccz_read_radix unexpected failure");

    ccz_mod(a, a, c);
    
    
    XCAssertCCZEquals(a, r, @"ccz_mod a = a % c");
    
    // pathological test case from failing SRP
    ccz_readstr(a, "12573135c66ab09ff6f5dfb777d18756", 16);
    ccz_readstr(b, "0000000000000010", 16);
    ccz_readstr(r, "0000000000000006", 16);
    ccz_mod(c, a, b);
    
    XCAssertCCZEquals(c, r, @"ccz_mod a = a % c");

}

- (void)test_modmul {

    ccz_readstr(a, "2983308b3538c60b245899a58b98f08c2e74c53950b30de70314f29af2df858d72df66"
                "51a38ceb4f1612231227604c1150c4cc412968c97afa545cbe18ee04a1d102bfa6a5bf"
                "7498996a41e70b4c7991f3c9e87984321915b87f8ce5c1aeca2b6015b6384f8a59bae3"
                "51d662f52f1634c3257434fb8eed85d93fb1ecaf344d7e", 16);

    ccz_readstr(b, "fb978a4f4fccdb14c7268918b784c4f6d5281c0d6ff43e60e88e97f97f2617608de248"
                "8c84eb99a3f467013c860536ec74f4968abeccbc1b026ee5873e40bdd292f8f7416a93"
                "df619288b49ba21d3e09aa796cb35a340b1abfda4e3b6cd92df2de64967e6a59f78758"
                "6929c4d2920da20caeb384594d7f2b7e999dab0d6a1ac", 16);

    ccz_readstr(c, "354c912b09ee7abff5b3d94ed52a9e8dcae582e094daa375c495f970710af73efcc4f9"
                "776010511f654c7408a6d5d351ab1d94a0fede757d782b54ddcf6fe8d714870b78b0e6"
                "7a9754cb03a5cf63bbda1c71791902ea4527fb0cd76437391e5422c704ffb6d6018261"
                "171d8cee98adcf0243f1fd520fb3761afe94a2f4d99f94", 16);

    ccz_readstr(r, "332f5fc965b485b8c960afbc26e6e278fc134163931602ae89c61246f3131cc62e711e"
                "637189a3a184eaf28d0ef89572360472f1d0ee625722606d36b031e608704b40840f17"
                "7ffc6cc392bfe628d51bd8cb5b1a6558ae5c1378f656877490d57d126629318400b866"
                "756827a525df23f931720250cddab02be65e14b624bfc4", 16);


    ccz_mulmod(d, a, b, c);

    XCAssertCCZEquals(d, r, @"ccz_mulmod r = a*b mod c");
    

    ccz_set(d,a);
    ccz_mulmod(d, d, b, c);
    XCAssertCCZEquals(d, r, @"ccz_mulmod r = a*b mod c - result in place of a");

    ccz_set(d,b);
    ccz_mulmod(d, a, d, c);
    XCAssertCCZEquals(d, r, @"ccz_mulmod r = a*b mod c - result in place of b");

}

- (void)test_expmod {
    ccz_seti(a, 2);
    ccz_seti(b, 64);
    ccz_seti(c, 0xffffffff);
    ccz_expmod(d, a, b, c);
    ccz_seti(a, 1);
    XCAssertCCZEquals(d, a, @"2**64 mod 0xffffffff = 1?");

    ccz_readstr(a, "c968e40c5304364b057425920b18cc358f254ddb0f42f84850d6deec46006b4a692e52b7c3bddead45f77f2c1be1c606521d8a24260429f362d65b57873dbf270e97e210b872e45e97cb4cd87977ad20491e53c48cf0e88da9a61312675a2527c86ac537740c5e4206972f09c0f91fa1c9f14a2cf1be07e82a3b6fd58dc12c3a", 16);
    ccz_seti(b, 65537);
    ccz_readstr(c, "354c912b09ee7abff5b3d94ed52a9e8dcae582e094daa375c495f970710af73efcc4f9776010511f654c7408a6d5d351ab1d94a0fede757d782b54ddcf6fe8d714870b78b0e67a9754cb03a5cf63bbda1c71791902ea4527fb0cd76437391e5422c704ffb6d6018261171d8cee98adcf0243f1fd520fb3761afe94a2f4d99f94", 16);
    ccz_readstr(r, "0fb978a4f4fccdb14c7268918b784c4f6d5281c0d6ff43e60e88e97f97f2617608de2488c84eb99a3f467013c860536ec74f4968abeccbc1b026ee5873e40bdd292f8f7416a93df619288b49ba21d3e09aa796cb35a340b1abfda4e3b6cd92df2de64967e6a59f787586929c4d2920da20caeb384594d7f2b7e999dab0d6a1ac", 16);
    ccz_expmod(d, a, b, c);
    XCAssertCCZEquals(d, r, @"ccz_expmod r = (a^b) mod c Test a > c");

    ccz_readstr(a, "2983308b3538c60b245899a58b98f08c2e74c53950b30de70314f29af2df858d72df6651a38ceb4f1612231227604c1150c4cc412968c97afa545cbe18ee04a1d102bfa6a5bf7498996a41e70b4c7991f3c9e87984321915b87f8ce5c1aeca2b6015b6384f8a59bae351d662f52f1634c3257434fb8eed85d93fb1ecaf344d7e", 16);
    ccz_expmod(d, a, b, c);
    XCAssertCCZEquals(d, r, @"ccz_expmod r = (a^b) mod c Test a normalized < c");
    
// #if 0
    ccz_readstr(r, "8f2fcb71936d463a70f02ef788ef27b363b3bfceb5f0692e8d3bddf74ac35c670f243d05097f99a0e8db9b29b22c6c99cfb8386d92c80c231a756bca77730baa", 16);
    ccz_readstr(a, "0000000000000002", 16);
    ccz_readstr(b, "d7ebf1988ad4ce0661ad4e8f52a7d9370fa92de13dac98ea8ccfe929bd4398ff", 16);
    ccz_readstr(c, "d4c7f8a2b32c11b8fba9581ec4ba4f1b04215642ef7355e37c0fc0443ef756ea2c6b8eeb755a1c723027663caa265ef785b8ff6a9b35227a52d86633dbdfca43", 16);
    ccz_expmod(d, a, b, c);
    XCAssertCCZEquals(d, r, @"ccz_expmod r = (a^b) mod c Test small a normalized < c");
// #endif

}

#if 0
// This works, but currently takes a long time.
-(void) test_write_radix10_10kbits {
    ccz_seti(a, 1);

    ccz_lsl(b, a, 10000);
    expected_reallocs += ccz_capacity(b) < ccn_nof(10000);

    size_t rs = ccz_write_radix_size(b, 10);
    char bbuf[rs + 1];
    bbuf[rs] = 0;
    ccz_write_radix(b, rs, bbuf, 10);
    fprintf(stderr, "%s\n", bbuf);
}
#endif

-(void) test_write_radix10_1kbits {
    ccz_seti(a, 1);
    ccz_lsl(b, a, 1000);
    const char expected_result[]="1071508607186267320948425049060001810561404811705533607443750388370351" \
    "0511249361224931983788156958581275946729175531468251871452856923140435" \
    "9845775746985748039345677748242309854210746050623711418779541821530464" \
    "7498358194126739876755916554394607706291457119647768654216766042983165" \
    "2624386837205668069376";
    expected_reallocs += ccz_capacity(b) < ccn_nof(1000);

    size_t rs = ccz_write_radix_size(b, 10);
    char bbuf[rs + 1];
    bbuf[rs] = 0;
    ccz_write_radix(b, rs, bbuf, 10);
    XCAssertCharsEquals(sizeof(expected_result), bbuf, &expected_result[0], @"ccz_write_radix 2^1000 base 10");
}

-(void) test_divmod {
    ccz_seti(a, 1);

    ccz_lsl(b, a, 10000);
    expected_reallocs += (ccz_capacity(a) < ccn_nof(10000));

    ccz_seti(c, 64);
    ccz_mul(d, b, c);
    ccz_divmod(a, b, d, c);
    XCTAssertEqual(ccz_is_zero(b), (bool)true, @"remainder is zero");
    ccz_lsr(b, a, 10000);
    ccz_seti(a, 1);
    XCAssertCCZEquals(b, a, @"((((1 << 10000) * 64) / 64) >> 10000) == 1?");
}

- (void)test_sqr {
    ccz_sqr(r, &a_c);
    XCAssertCCZEquals(r, &squarea_c, @"ccz_sqr(r,a_c) r = a_c^2");
}

- (void)test_set {
    ccz_set(r, &a_c);
    XCAssertCCZEquals(r, &a_c, @"ccz_set(r,a_c) r = a_c");
}

#if 0
- (void)test_seti {
    cc_unit r[ccz_nof(_c)];
    ccz_seti(r, &a_c[0]);
    cc_unit s[ccz_nof(_c)] = {};
    s[0] = a_c[0];
    XCAssertCCZEquals(r, s, @"ccz_seti(r,a_c) r = a_c");
}
#endif

@end
