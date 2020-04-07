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


#import "CCZPKATValidation.h"

#import <corecrypto/cczp.h>
#import "ccn_unit.h"
#include <corecrypto/ccn_debug.h>

@implementation CCZPKATValidation

static const uint8_t abytes[192/8] = { 0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0x21,0x22,0x23,0x24 };
static cc_unit a192[ccn_nof(192)] = {
    CCN192_C(01,02,03,04,05,06,07,08,09,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24)};
static cc_unit a192_recip[1 + ccn_nof(192)] = {
    CCN200_C(00,03,f8,03,ff,ff,ff,ff,ff,ff,e8,47,b8,18,00,00,00,00,00,8d,ab,a5,78,a8,00)};

- (void)test_size {
    STAssertEquals(cczp_size(8), sizeof(struct cczp) + sizeof(cc_unit) + 2 * 8,
                   @"8 byte prime cczp size ok");
}

- (void)test_short_size {
    STAssertEquals(cczp_short_size(16), sizeof(struct cczp) + 16,
                   @"16 byte prime cczp_short size ok");
}

- (void)test_nof_n {
    STAssertEquals(cczp_nof_n(16), ccn_nof_size(sizeof(struct cczp)) + 1 + 2 * 16,
                   @"16 unit prime cczp n ok");
}

- (void)test_short_nof_n {
    STAssertEquals(cczp_short_nof_n(16), ccn_nof_size(sizeof(struct cczp)) + 16,
                   @"16 unit prime cczp_short n ok");
}

- (void)test_decl_n {
    struct zp42 {
        cczp_decl_n(42, myzp);
    };
    STAssertEquals(sizeof(struct zp42), ((cczp_size(ccn_sizeof_n(42)) + 15) & ~15),
                   @"cczp_decl_n(42) is the right size.");
}

- (void)test_short_decl_n {
    struct zps42 {
        cczp_short_decl_n(42, myzp);
    };
    STAssertEquals(sizeof(struct zps42), ((cczp_short_size(ccn_sizeof_n(42)) + 15) & ~15),
                   @"cczp_short_decl_n(42) is the right size.");
}

- (void)test_N {
    cczp_short_decl_n(1, zp);
    CCZP_N(zp) = 21;
    STAssertEquals(CCZP_N(zp), (size_t)21,
                   @"CCZP_N(zp) is 21.");
}

- (void)test_MOD_PRIME {
    cczp_short_decl_n(1, zp);
    CCZP_MOD_PRIME(zp) = cczp_mod;
    STAssertEquals(CCZP_MOD_PRIME(zp), &cczp_mod,
                   @"CCZP_MOD_PRIME(zp)");
}

- (void)test_PRIME {
    cczp_short_decl_n(4, zp);
    CCZP_N(zp) = 4;
    cc_unit *prime = CCZP_PRIME(zp);
    STAssertEquals((uint8_t *)prime, ((uint8_t *)zp) + sizeof(struct cczp),
                   @"CCZP_PRIME(zp)");
}

- (void)test_RECIP {
    cczp_decl_n(4, zp);
    CCZP_N(zp) = 4;
    cc_unit *recip = CCZP_RECIP(zp);
    STAssertEquals((uint8_t *)recip, ((uint8_t *)zp) + sizeof(struct cczp) + ccn_sizeof_n(4),
                   @"CCZP_RECIP(zp)");
}

- (void)test_n {
    cczp_short_decl_n(1, zp);
    CCZP_N(zp) = 25;
    STAssertEquals(cczp_n(zp), (size_t)25,
                   @"cczp_n(zp) is 25.");
}

- (void)test_mod_prime {
    cczp_short_decl_n(1, zp);
    CCZP_MOD_PRIME(zp) = cczp_mod;
    STAssertEquals(cczp_mod_prime(zp), &cczp_mod,
                   @"cczp_mod_prime(zp)");
}

- (void)test_prime {
    cczp_short_decl_n(4, zp);
    CCZP_N(zp) = 4;
    const cc_unit *prime = cczp_prime(zp);
    STAssertEquals((uint8_t *)prime, ((uint8_t *)zp) + sizeof(struct cczp),
                   @"cczp_mod_prime(zp)");
}

- (void)test_recip {
    cczp_decl_n(4, zp);
    CCZP_N(zp) = 4;
    const cc_unit *recip = cczp_recip(zp);
    STAssertEquals((uint8_t *)recip, ((uint8_t *)zp) + sizeof(struct cczp) + ccn_sizeof_n(4),
                   @"cczp_recip(zp)");
}

- (void)test_init {
    cczp_decl_n(ccn_nof(192), zp);
    CCZP_N(zp) = ccn_nof(192);
    ccn_set(ccn_nof(192), CCZP_PRIME(zp), a192);
    cczp_init(zp);

    STAssertEquals(cczp_mod_prime(zp), &cczp_mod,
                   @"cczp_init(zp) initialized mod_prime");
    STAssertCCNEquals(1 + ccn_nof(192), cczp_recip(zp), a192_recip,
                   @"cczp_init(zp)initialized recip");
}

- (void)test_mod {
    // TODO
}

- (void)test_modn {
    // TODO
}

- (void)test_mul {
    // TODO
}

- (void)test_sqr {
    // TODO
}

- (void)test_power {
    // TODO
}

- (void)test_powern {
    // TODO
}

- (void)test_add {
    // TODO
}

- (void)test_sub {
    // TODO
}

- (void)test_div2 {
    // TODO
}

- (void)test_div {
    // TODO
}

- (void)test_mod_inv {
    // TODO
}

- (void)test_mod_inv_slow {
    // TODO
}

- (void)test_mod_inv_slown {
    // TODO
}

static void cczp_set_prime(cczp_t zp, cc_size n, const cc_unit *prime) {
    CCZP_N(zp) = n;
    ccn_set(n, CCZP_PRIME(zp), prime);
    //cczp_init(zp);
}

static void cczp_set_prime1(cczp_t zp, uint64_t v) {
    const size_t n = ccn_nof_size(sizeof(v));
    cc_unit td[n] = { ccn64_v(v) };
    cczp_set_prime(zp, n, td);
}

- (void)test_rabin_miller {
    const cc_size n = ccn_nof(192);
    cczp_decl_n(n, zp);
    cczp_set_prime(zp, n, a192);
    STAssertFalse(cczp_rabin_miller(zp, 8), @"a192 is not prime");

    cczp_set_prime1(zp, 17);
    STAssertTrue(cczp_rabin_miller(zp, 8), @"17 is prime");
    cczp_set_prime1(zp, 0x665);
    STAssertTrue(cczp_rabin_miller(zp, 8), @"0x665 is prime");
    cczp_set_prime1(zp, 0x679);
    STAssertTrue(cczp_rabin_miller(zp, 8), @"0x679 is prime");
}

- (void)test_random_prime {
    // TODO
}

@end
