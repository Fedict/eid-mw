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


#import "CCNKATValidation.h"
#import "ccn_unit.h"
#include <corecrypto/cc_priv.h>
#include <corecrypto/cc_debug.h>

@implementation CCNKATValidation

static const uint8_t abytes[192/8] = { 0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0x21,0x22,0x23,0x24 };
static cc_unit a192[ccn_nof(192)] = {
    CCN192_C(01,02,03,04,05,06,07,08,09,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24)};
static cc_unit b192[ccn_nof(192)] = {
    CCN192_C(24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,09,08,07,06,05,04,03,02,01)};
static cc_unit c192[ccn_nof(192)] = {
    CCN192_C(00,00,00,00,00,00,17,16,15,14,13,12,11,10,09,00,00,00,00,00,00,00,00,00)};

static cc_unit lsr192a[ccn_nof(192)] = {
    CCN192_C(00,81,01,82,02,83,03,84,04,88,08,89,09,8A,0A,8B,0B,8C,0C,90,10,91,11,92)};
static cc_unit lsr192a64[ccn_nof(192)] = {
    CCN192_C(00,00,00,00,00,00,00,00,01,02,03,04,05,06,07,08,09,10,11,12,13,14,15,16)};
static cc_unit lsr192a65[ccn_nof(192)] = {
    CCN192_C(00,00,00,00,00,00,00,00,00,81,01,82,02,83,03,84,04,88,08,89,09,8A,0A,8B)};
static cc_unit lsr8a192[ccn_nof(192)] = {
    CCN192_C(00,01,02,03,04,05,06,07,08,09,10,11,12,13,14,15,16,17,18,19,20,21,22,23)};
static cc_unit lsl192a[ccn_nof(192)] = {
    CCN192_C(02,04,06,08,0A,0C,0E,10,12,20,22,24,26,28,2A,2C,2E,30,32,40,42,44,46,48)};
static cc_unit lsl192a64[ccn_nof(192)] = {
    CCN192_C(09,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,00,00,00,00,00,00,00,00)};
static cc_unit lsl192a65[ccn_nof(192)] = {
    CCN192_C(12,20,22,24,26,28,2A,2C,2E,30,32,40,42,44,46,48,00,00,00,00,00,00,00,00)};
static cc_unit sum192ab[ccn_nof(192)] = {
    CCN192_C(25,25,25,25,25,1f,1f,1f,1f,25,25,25,25,25,25,1f,1f,1f,1f,25,25,25,25,25)};
static cc_unit diff192ba[ccn_nof(192)] = {
    CCN192_C(23,21,1f,1d,1b,13,11,0f,0d,05,03,00,fe,fc,fa,f2,f0,ee,ec,e4,e2,e0,de,dd)};
static cc_unit prod384ab[ccn_nof(384)] = {
    CCN384_C(00,24,6b,d5,60,0a,ce,aa,9d,a7,9e,a3,b5,d3,fd,0c,1e,32,47,5d,4a,2f,0a,dc,9d,d7,05,29,44,58,43,2e,1a,07,f8,cf,b1,9f,9a,a4,9b,a8,cd,09,5e,d4,6b,24)};
static cc_unit square384a[ccn_nof(384)] = {
    CCN384_C(00,01,04,0a,14,23,38,54,78,a5,e9,43,b6,41,e7,a8,85,7f,97,f3,7b,30,13,25,67,90,9f,93,6b,6e,53,18,be,40,e5,5a,9f,b3,95,68,06,6f,a2,9c,a2,62,dd,10)};

- (void)test_sizeof_n
{
    XCTAssertEqual(ccn_sizeof_n(10), sizeof(cc_unit) * 10,
                   @"10 units size in bytes ok");
}

- (void)test_nof
{
    XCTAssertEqual(ccn_nof(521),
                   (521 + 8 * sizeof(cc_unit) - 1) / (8 * sizeof(cc_unit)),
                   @"521 bit ccn size in units is ok");
    XCTAssertEqual(ccn_nof(513),
                   (513 + 8 * sizeof(cc_unit) - 1) / (8 * sizeof(cc_unit)),
                   @"513 bit ccn size in units is ok");

}

- (void)test_sizeof
{
    XCTAssertEqual(ccn_sizeof(0), (size_t)0,
                   @"0 bit ccn size is 0 bytes");

    XCTAssertEqual(ccn_sizeof(1), sizeof(cc_unit),
                   @"1 bit ccn size in bytes 1 unit");

    XCTAssertEqual(ccn_sizeof(63),
                   sizeof(cc_unit) * ((63 + 8 * sizeof(cc_unit) - 1) / (8 * sizeof(cc_unit))),
                   @"63 bit ccn size in bytes is ok");

    XCTAssertEqual(ccn_sizeof(64),
                   sizeof(cc_unit) * ((64 + 8 * sizeof(cc_unit) - 1) / (8 * sizeof(cc_unit))),
                   @"64 bit ccn size in bytes is ok");

    XCTAssertEqual(ccn_sizeof(521),
                   sizeof(cc_unit) * ((521 + 8 * sizeof(cc_unit) - 1) / (8 * sizeof(cc_unit))),
                   @"521 bit ccn size in bytes is ok");
}

- (void)test_nof_size
{
    XCTAssertEqual(ccn_nof_size(17),
                   (17 + CCN_UNIT_SIZE - 1) / CCN_UNIT_SIZE,
                   @"17 bytes aligned up to unit size.");
}

- (void)test_bitsof_n
{
    XCTAssertEqual(ccn_bitsof_n(17),
                   17 * 8 * sizeof(cc_unit),
                   @"bit count of 17 unit array.");
}

- (void)test_bitsof_size
{
    XCTAssertEqual(ccn_bitsof_size(17),
                   17 * 8,
                   @"bit count of 17 byte array.");
}

- (void)test_sizeof_size
{
    XCTAssertEqual(ccn_sizeof_size(17),
                   sizeof(cc_unit) * ((17 + sizeof(cc_unit) - 1) / sizeof(cc_unit)),
                   @"byte count of ccn capable of holding 17 bytes.");
}

- (void)test_bit
{
    XCTAssertEqual(ccn_bit(a192, 0U), (cc_unit)0, @"bit 0 of a192 is 0.");
    XCTAssertEqual(ccn_bit(a192, 2U), (cc_unit)1, @"bit 2 of a192 is 1.");
    XCTAssertEqual(ccn_bit(b192, 0U), (cc_unit)1, @"bit 0 of b192 is 1.");
    XCTAssertEqual(ccn_bit(b192, 191U), (cc_unit)0, @"bit 191 of b192 is 0.");
    XCTAssertEqual(ccn_bit(b192, 189U), (cc_unit)1, @"bit 189 of b192 is 1.");
}

- (void)test_set_bit
{
    cc_unit a[ccn_nof(128)] = {};
    for (size_t bit = 0; bit < 128; ++bit) {
        XCTAssertEqual(ccn_bit(a, bit), (cc_unit)0, @"bit %zu of a is 0.", bit);
        ccn_set_bit(a, bit, 1);
        XCTAssertEqual(ccn_bit(a, bit), (cc_unit)1, @"bit %zu of a is now 1.", bit);
        ccn_set_bit(a, bit, 0);
        XCTAssertEqual(ccn_bit(a, bit), (cc_unit)0, @"bit %zu of a is now 0 again.", bit);
    }
}

- (void)test_n
{
    cc_unit a[ccn_nof(384)] = {};
    XCTAssertEqual(ccn_n(ccn_nof(384), a), (cc_size)0, @"ccn_n of zero is 0");
    ccn_set(ccn_nof(192), a, a192);
    XCTAssertEqual(ccn_n(ccn_nof(384), a), ccn_nof(192),
                   @"ccn_n 192 bit number in 384 bit ccn is ccn_nof(192)");
}

- (void)test_shift_right
{
    cc_unit r[ccn_nof(192)] = {};
    XCTAssertEqual(ccn_shift_right(ccn_nof(192), r, a192, 1), (cc_unit)0, @"ccn_shift_right(r,a192,1) returns the bit shifted out");
    XCAssertCCNEquals(ccn_nof(192), lsr192a, r, @"ccn_shift_right(r, a192) works");
    XCTAssertEqual(ccn_shift_right(ccn_nof(192), r, b192, 1), ((cc_unit)1) << (CCN_UNIT_BITS - 1), @"ccn_shift_right(r,b192,1) returns 1");
    XCTAssertEqual(ccn_shift_right(ccn_nof(192), r, a192, 8), ((cc_unit)0x24) << (CCN_UNIT_BITS - 8), @"ccn_shift_right(r,a192,8) returns byte shifted out");
    XCAssertCCNEquals(ccn_nof(192), lsr8a192, r, @"ccn_shift_right(r,a192,8) works");
    ccn_set(ccn_nof(192), r, a192);
    XCTAssertEqual(ccn_shift_right(ccn_nof(192), r, r, 1), (cc_unit)0, @"ccn_shift_right(r,r,1) returns 0");
    XCAssertCCNEquals(ccn_nof(192), lsr192a, r, @"ccn_shift_right(r, r) works");
}

- (void)test_shift_right_multi
{
    cc_unit r[ccn_nof(192)] = {};
    ccn_shift_right_multi(ccn_nof(192), r, a192, 1);
    XCAssertCCNEquals(ccn_nof(192), lsr192a, r, @"ccn_shift_right_multi(r, a192) works");
    ccn_shift_right_multi(ccn_nof(192), r, a192, 8);
    XCAssertCCNEquals(ccn_nof(192), lsr8a192, r, @"ccn_shift_right(r,a192,8) works");
    ccn_set(ccn_nof(192), r, a192);
    ccn_shift_right_multi(ccn_nof(192), r, r, 1);
    XCAssertCCNEquals(ccn_nof(192), lsr192a, r, @"ccn_shift_right_multi(r, r) works");

    ccn_shift_right_multi(ccn_nof(192), r, a192, 64);
    XCAssertCCNEquals(ccn_nof(192), lsr192a64, r, @"ccn_shift_right_multi(r, r) works");
    ccn_shift_right_multi(ccn_nof(192), r, a192, 65);
    XCAssertCCNEquals(ccn_nof(192), lsr192a65, r, @"ccn_shift_right_multi(r, r) works");
}

- (void)test_shift_left
{
    cc_unit r[ccn_nof(192)] = {};
    XCTAssertEqual(ccn_shift_left(ccn_nof(192), r, a192, 1), (cc_unit)0, @"ccn_shift_left(r, a192, 1) returns 0");
    XCAssertCCNEquals(ccn_nof(192), r, lsl192a, @"ccn_shift_left(r, a192) works");
    XCTAssertEqual(ccn_shift_left(ccn_nof(192), r, b192, 3), (cc_unit)1, @"ccn_shift_left(r, b192, 3) returns 1");
    XCTAssertEqual(ccn_shift_left(ccn_nof(192), r, a192, 7), (cc_unit)0, @"ccn_shift_left(r, a192, 7) returns 0");
    XCTAssertEqual(ccn_shift_left(ccn_nof(192), r, b192, 8), (cc_unit)0x24, @"ccn_shift_left(r, a192, 8) returns byte shifted out");
    ccn_set(ccn_nof(192), r, a192);
    XCTAssertEqual(ccn_shift_left(ccn_nof(192), r, r, 1), (cc_unit)0, @"ccn_shift_left(r,r,1) returns the byte shifted out");
    XCAssertCCNEquals(ccn_nof(192), r, lsl192a, @"ccn_shift_left(r, r) works");
}

- (void)test_shift_left_multi
{
    cc_unit r[ccn_nof(192)] = {};
    ccn_shift_left_multi(ccn_nof(192), r, a192, 1);
    XCAssertCCNEquals(ccn_nof(192), lsl192a, r, @"ccn_shift_left_multi(r, a192) works");
    ccn_set(ccn_nof(192), r, a192);
    ccn_shift_left_multi(ccn_nof(192), r, r, 1);
    XCAssertCCNEquals(ccn_nof(192), lsl192a, r, @"ccn_shift_left_multi(r, r) works");

    ccn_shift_left_multi(ccn_nof(192), r, a192, 64);
    XCAssertCCNEquals(ccn_nof(192), lsl192a64, r, @"ccn_shift_left_multi(r, r) works");
    ccn_shift_left_multi(ccn_nof(192), r, a192, 65);
    XCAssertCCNEquals(ccn_nof(192), lsl192a65, r, @"ccn_shift_left_multi(r, r) works");
}

- (void)test_bitlen
{
    XCTAssertEqual(ccn_bitlen(ccn_nof(192), a192), (size_t)185, @"ccn_bitlen(r, a192) works");
    XCTAssertEqual(ccn_bitlen(ccn_nof(192), b192), (size_t)190, @"ccn_bitlen(r, b192) works");
    XCTAssertEqual(ccn_bitlen(ccn_nof(192), c192), (size_t)141, @"ccn_bitlen(r, c192) works");
    cc_unit plus1[ccn_nof(192)+1];
    ccn_set(ccn_nof(192), plus1, a192);
    plus1[ccn_nof(192)] = 1;
    XCTAssertEqual(ccn_bitlen(ccn_nof(192)+1, plus1), (size_t)193, @"ccn_bitlen(r, plus1) works");
}

- (void)test_trailing_zeros
{
    XCTAssertEqual(ccn_trailing_zeros(ccn_nof(192), a192), (size_t)2, @"ccn_trailing_zeros(r, a192) works");
    XCTAssertEqual(ccn_trailing_zeros(ccn_nof(192), b192), (size_t)0, @"ccn_trailing_zeros(r, b192) works");
    XCTAssertEqual(ccn_trailing_zeros(ccn_nof(192), c192), (size_t)72, @"ccn_trailing_zeros(r, c192) works");
}

- (void)test_is_zero
{
    cc_unit r[ccn_nof(192)] = {};
    XCTAssertEqual(ccn_is_zero(ccn_nof(192), a192), false, @"ccn_is_zero(r, a192) works");
    XCTAssertEqual(ccn_is_zero(ccn_nof(192), r), true, @"ccn_is_zero(r, 0) works");
}

- (void)test_is_one
{
    cc_unit r[ccn_nof(192)] = {};
    XCTAssertEqual(ccn_is_one(ccn_nof(192), a192), false, @"ccn_is_one(r, a192) works");
    XCTAssertEqual(ccn_is_one(ccn_nof(192), r), false, @"ccn_is_one(r, 0) works");
    r[0]=CC_UNIT_C(1);
    XCTAssertEqual(ccn_is_one(ccn_nof(192), r), true, @"ccn_is_one(r, 1) works");
}

- (void)test_cmp
{
    XCTAssertEqual(ccn_cmp(ccn_nof(192), a192, a192), 0, @"ccn_cmp(r,a192,a192)");
    XCTAssertEqual(ccn_cmp(ccn_nof(192), a192, b192), -1, @"ccn_cmp(r,a192,b192)");
    XCTAssertEqual(ccn_cmp(ccn_nof(192), b192, a192), 1, @"ccn_cmp(r,b192,a192)");
}

- (void)test_cmpn
{
    // TODO: Write this
}

// All code under test must be linked into the Unit Test bundle
- (void)test_sub
{
    cc_unit r[ccn_nof(192)];
    XCTAssertEqual(ccn_sub(ccn_nof(192), r, b192, a192), (cc_unit)0,
                   @"ccn_sub(r,b192,a192) no borrow");
    XCAssertCCNEquals(ccn_nof(192), r, diff192ba, @"ccn_sub(r,b192,a192) works");

    XCTAssertEqual(ccn_sub(ccn_nof(192), r, a192, b192), (cc_unit)1,
                   @"ccn_sub(r,a192,b192) borrows 1");
    XCTAssertEqual(ccn_add(ccn_nof(192), r, r, b192), (cc_unit)1,
                   @"ccn_add(r,r,b192) return carry 1");
    XCAssertCCNEquals(ccn_nof(192), r, a192, @"a192 - b192 + b192 = a192");

    ccn_set(ccn_nof(192), r, b192);
    XCTAssertEqual(ccn_sub(ccn_nof(192), r, r, a192), (cc_unit)0,
                   @"ccn_sub(r,r,a192) no borrow");
    XCAssertCCNEquals(ccn_nof(192), r, diff192ba, @"ccn_sub(r,r,a192) works");

    ccn_set(ccn_nof(192), r, a192);
    XCTAssertEqual(ccn_sub(ccn_nof(192), r, b192, r), (cc_unit)0,
                   @"ccn_sub(r,b192,r) no borrow");
    XCAssertCCNEquals(ccn_nof(192), r, diff192ba, @"ccn_sub(r,b192,r) works");

    ccn_set(ccn_nof(192), r, a192);
    XCTAssertEqual(ccn_sub(ccn_nof(192), r, r, r), (cc_unit)0,
                   @"ccn_sub(r,r,r) no borrow");
    XCTAssertTrue(ccn_is_zero(ccn_nof(192), r), @"after ccn_sub(r,r,r) r = 0");
}

- (void)test_sub1
{
    cc_unit r[ccn_nof(192)] = {},
            s[ccn_nof(192)] = {},
            t[ccn_nof(192)] = {};
    s[0] = a192[0];
    XCTAssertEqual(ccn_sub(ccn_nof(192), t, r, s), (cc_unit)1,
                   @"r = 0, ccn_sub(r,r,a192[0]) borrow");
    XCTAssertEqual(ccn_sub1(ccn_nof(192), r, r, a192[0]), (cc_unit)1,
                   @"r = 0, ccn_sub1(r,r,a192[0]) borrow");
    XCAssertCCNEquals(ccn_nof(192), r, t, @"ccn_sub(r,b192,r) works");
    XCTAssertEqual(ccn_sub1(ccn_nof(192), r, b192, a192[0]), (cc_unit)0,
                   @"ccn_sub1(r,b192,a192[0]) no borrow");
}

- (void)test_subn
{
    // TODO: Write this
}

// All code under test must be linked into the Unit Test bundle
- (void)test_add
{
    cc_unit r[ccn_nof(192)], s[ccn_nof(192)];
    XCTAssertEqual(ccn_add(ccn_nof(192), r, a192, b192), (cc_unit)0,
                   @"no carry adding a and b");
    XCAssertCCNEquals(ccn_nof(192), r, sum192ab, @"ccn_add(r,a,b) works");
    
    ccn_set(ccn_nof(192), r, a192);
    XCTAssertEqual(ccn_add(ccn_nof(192), r, r, b192), (cc_unit)0,
                   @"no carry adding a and b with &r == &a");
    XCAssertCCNEquals(ccn_nof(192), r, sum192ab, @"ccn_add(r,r,b) works");
    
    ccn_set(ccn_nof(192), r, b192);
    XCTAssertEqual(ccn_add(ccn_nof(192), r, a192, r), (cc_unit)0,
                   @"no carry adding a and b with &r == &a");
    XCAssertCCNEquals(ccn_nof(192), r, sum192ab, @"ccn_add(r,a,r) works");
    
    ccn_set(ccn_nof(192), r, b192);
    XCTAssertEqual(ccn_add(ccn_nof(192), r, a192, r), (cc_unit)0,
                   @"no carry adding a and b with &r == &a");
    XCAssertCCNEquals(ccn_nof(192), r, sum192ab, @"ccn_add(r,a,r) works");
    
    ccn_set(ccn_nof(192), r, a192);
    XCTAssertEqual(ccn_add(ccn_nof(192), r, r, r), (cc_unit)0,
                   @"no carry adding a and b with &r == &a");
    ccn_shift_left(ccn_nof(192), s, a192, 1);
    XCAssertCCNEquals(ccn_nof(192), s, r, @"ccn_add(r,r,r) yields same result as ccn_shift_left(r, r, 1)");
}

- (void)test_add1
{
    cc_unit r[ccn_nof(192)] = {},
    s[ccn_nof(192)] = {},
    t[ccn_nof(192)] = {};
    s[0] = a192[0];
    XCTAssertEqual(ccn_add(ccn_nof(192), t, b192, s), (cc_unit)0,
                   @"r = 0, ccn_add(r,r,a192[0]) no carry");
    XCTAssertEqual(ccn_add1(ccn_nof(192), r, b192, a192[0]), (cc_unit)0,
                   @"r = 0, ccn_add1(r,r,a192[0]) no carry");
    XCAssertCCNEquals(ccn_nof(192), r, t, @"ccn_add(r,b192,r) works");
    cc_unit max[1];
    max[0] = ~CC_UNIT_C(0);
    XCTAssertEqual(ccn_add1(1, r, max, CC_UNIT_C(1)), (cc_unit)1,
                   @"r = 0, ccn_add1(r,~0,1) carry");
}

- (void) test_mul1
{

    cc_unit r[ccn_nof(192)];
    cc_unit expected_r[ccn_nof(192)+1];

    // a * 2^(CCN_UNIT_SIZE-1) =?  a << (CCN_UNIT_SIZE-1)
    ccn_setn(ccn_nof(192)+1,expected_r,ccn_nof(192),a192);
    ccn_shift_left_multi(ccn_nof(192)+1,expected_r,expected_r,CCN_UNIT_BITS-1);
    XCTAssertEqual(ccn_mul1(ccn_nof(192),r,a192,(cc_unit)MAX_POWER_OF_2(r[0])), expected_r[ccn_nof(192)], @"ccn_mul1(r,a192,2) retval works");
    XCAssertCCNEquals(ccn_nof(192), r, expected_r, @"ccn_mul1(r,a192,2^(CCN_UNIT_SIZE-1)) works");

    // a * 1 =? a
    XCTAssertEqual(ccn_mul1(ccn_nof(192),r,a192,(cc_unit)1), (cc_unit)0, @"ccn_mul1(r,a192,1) retval works");
    XCAssertCCNEquals(ccn_nof(192), r, a192, @"ccn_mul1(r,a192,1) works");

    // (2*a) =? 2 * a
    ccn_zero(ccn_nof(192)+1,expected_r);
    ccn_shift_left(ccn_nof(192),expected_r,a192,2);
    XCTAssertEqual(ccn_mul1(ccn_nof(192),r,a192,(cc_unit)4), expected_r[ccn_nof(192)], @"ccn_mul1(r,a192,4) retval works");
    XCAssertCCNEquals(ccn_nof(192), r, expected_r, @"ccn_mul1(r,a192,4) works");

}


- (void) test_muladd1
{
    // b + a * 1 =? a + b
    cc_unit r[ccn_nof(192)];
    cc_unit expected_r[ccn_nof(192)+1];
    ccn_set(ccn_nof(192),r,b192);
    XCTAssertEqual(ccn_addmul1(ccn_nof(192),r,a192,(cc_unit)1), (cc_unit)0, @"ccn_addmul1(r,a192,1) retval works");
    XCAssertCCNEquals(ccn_nof(192), r, sum192ab, @"ccn_addmul1(r,a192,1) works");

    // (2*a)+ a * 2 =? 4 * a
    ccn_set(ccn_nof(192),r,lsl192a);
    ccn_zero(ccn_nof(192)+1,expected_r);
    ccn_shift_left(ccn_nof(192),expected_r,lsl192a,1);
    XCTAssertEqual(ccn_addmul1(ccn_nof(192),r,a192,(cc_unit)2), expected_r[ccn_nof(192)], @"ccn_addmul1(r,a192,2) retval works");
    XCAssertCCNEquals(ccn_nof(192), r, expected_r, @"ccn_addmul1(r,a192,2) works");

    // (a)+ a * (2^CCN_UNIT_SIZE-1) =? 2^CCN_UNIT_SIZE * a
    ccn_set(ccn_nof(192),r,a192);
    ccn_zero(ccn_nof(192)+1,expected_r);
    ccn_shift_left_multi(ccn_nof(192)+1,expected_r,a192,CCN_UNIT_BITS);
    XCTAssertEqual(ccn_addmul1(ccn_nof(192),r,a192,(cc_unit)CCN_UNIT_MASK), expected_r[ccn_nof(192)], @"ccn_addmul1(r,a192,2) retval works");
    XCAssertCCNEquals(ccn_nof(192), r, expected_r, @"ccn_addmul1(r,a192,2^CCN_UNIT_SIZE-1) works");
}

// All code under test must be linked into the Unit Test bundle
- (void)test_mul
{
    cc_unit r[ccn_nof(384)];
    ccn_mul(ccn_nof(192), r, a192, b192);
    XCAssertCCNEquals(ccn_nof(384), r, prod384ab, @"ccn_mul(r,a192,b192) works");

#if 0
    ccn_div(ccn_nof(192), r, r, b192);
    XCAssertCCNEquals(ccn_nof(192), r, a192, @"a192 * b192 / b192 = a192");
#endif

    //ccn_set(ccn_nof(192), r, a192);
    //ccn_mul(ccn_nof(192), r, r, b192);
    //XCAssertCCNEquals(ccn_nof(192), r, prod384ab, @"r=a192, ccn_mul(r,r,b192) works");

    //ccn_set(ccn_nof(192), r, b192);
    //ccn_mul(ccn_nof(192), r, a192, r);
    //XCAssertCCNEquals(ccn_nof(192), r, prod384ab, @"r=b192, ccn_mul(r,a192,r) works");

    ccn_mul(ccn_nof(192), r, a192, a192);
    XCAssertCCNEquals(ccn_nof(192), r, square384a, @"ccn_mul(r,a192,a192) r = a192^2");

    //ccn_set(ccn_nof(192), r, a192);
    //ccn_mul(ccn_nof(192), r, r, r);
    //XCAssertCCNEquals(ccn_nof(192), r, square384a, @"r=a192, ccn_mul(r,r,r) r = a192^2");
}

- (void)test_gcd
{
    cc_unit q[ccn_nof(384)],
            r[ccn_nof(384)],
            s[ccn_nof(384)];
    ccn_zero(ccn_nof(384), s);
    ccn_set(ccn_nof(192), s, a192);
    ccn_mul(ccn_nof(192), r, a192, b192);
    ccn_gcd(ccn_nof(384), q, r, s);
    XCAssertCCNEquals(ccn_nof(384), q, s, @"ccn_gcd(a*b, a) == b");
}

- (void)test_read_uint
{
    cc_unit r[ccn_nof(192)];
    XCTAssertEqual(ccn_read_uint(ccn_nof(192), r, 192 / 8, abytes), 0, @"ccn_read_uint");
    XCAssertCCNEquals(ccn_nof(192), r, a192, @"ccn_read_uint(a)");
}

- (void)test_write_uint_size
{
    XCTAssertEqual(ccn_write_uint_size(ccn_nof(192), a192), (size_t)(192 / 8), @"ccn_write_uint_size(a192) == 192/8");
}

- (void)test_write_uint
{
    size_t i_size = ccn_write_uint_size(ccn_nof(192), a192);
    uint8_t r[i_size];
    ccn_write_uint(ccn_nof(192), a192, i_size, r);
    XCAssertMemEquals(i_size, r, abytes, @"ccn_write_uint(a192)");
}

- (void)test_write_uint_padded
{
    static const uint8_t zeros[43] = {};
    size_t i_size = ccn_write_uint_size(ccn_nof(192), a192);
    uint8_t r[i_size + 43];
    ccn_write_uint_padded(ccn_nof(192), a192, i_size + 43, r);
    XCAssertMemEquals(i_size, r + 43, abytes, @"ccn_write_uint_padded(a192)");
    XCAssertMemEquals(43, r, zeros, @"ccn_write_uint_padded(a192)");
}


- (void)test_write_int_size
{
    XCTAssertEqual(ccn_write_int_size(ccn_nof(192), a192), (size_t)(192 / 8), @"ccn_write_int_size(a192) == 192/8");
    cc_unit r[ccn_nof(192)];
    ccn_set(ccn_nof(192), r, a192);
    r[ccn_nof(192) - 1] = ~CC_UNIT_C(0);
    XCTAssertEqual(ccn_write_int_size(ccn_nof(192), r), (size_t)(192 / 8) + 1, @"ccn_write_int_size(0xff a192) == 192/8 + 1");
}

- (void)test_write_int
{
    size_t i_size = ccn_write_int_size(ccn_nof(192), a192);
    uint8_t bytes[i_size];
    ccn_write_int(ccn_nof(192), a192, i_size, bytes);
    XCAssertMemEquals(i_size, bytes, abytes, @"ccn_write_int(a192)");

    cc_unit r[ccn_nof(192)];
    ccn_set(ccn_nof(192), r, a192);
    r[ccn_nof(192) - 1] = ~CC_UNIT_C(0);
    size_t j_size = ccn_write_int_size(ccn_nof(192), r);
    uint8_t jbytes[j_size];
    ccn_write_int(ccn_nof(192), r, j_size, jbytes);
    XCAssertMemEquals(j_size - 1 - CCN_UNIT_SIZE, jbytes + 1 + CCN_UNIT_SIZE, abytes + CCN_UNIT_SIZE, @"ccn_write_int(0xff a192)");
    XCTAssertEqual(jbytes[0], (uint8_t)0, @"first byte is zero(a192)");
}

- (void)test_sqr
{
    cc_unit r[ccn_nof(384)];
    ccn_sqr(ccn_nof(192), r, a192);
    XCAssertCCNEquals(ccn_nof(192), r, square384a, @"ccn_sqr(r,a192) r = a192^2");
}

- (void)test_set
{
    cc_unit r[ccn_nof(192)];
    ccn_set(ccn_nof(192), r, a192);
    XCAssertCCNEquals(ccn_nof(192), r, a192, @"ccn_set(r,a192) r = a192");
}


- (void)test_seti
{
    cc_unit r[ccn_nof(192)];
    ccn_seti(ccn_nof(192), r, a192[0]);
    cc_unit s[ccn_nof(192)] = {};
    s[0] = a192[0];
    XCAssertCCNEquals(ccn_nof(192), r, s, @"ccn_seti(r,a192) r = a192");
}

- (void)test_swap
{
    cc_unit r[ccn_nof(192)];
    size_t i_size = ccn_write_uint_size(ccn_nof(192), a192);
    ccn_write_uint(ccn_nof(192), a192, i_size, (uint8_t*)r);
    ccn_swap(ccn_nof(192), r);
    XCAssertMemEquals(i_size, r, a192, @"ccn_swap(a192)");
}

- (void)test_xor
{
    cc_unit r[ccn_nof(192)];
    ccn_xor(ccn_nof(192), r, a192, b192);
    ccn_xor(ccn_nof(192), r, r, b192);
    XCAssertCCNEquals(ccn_nof(192), r, a192, @"ccn_xor(a192)");
}

@end
