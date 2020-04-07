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


#import "PBKDF_HMACValidation.h"
#import <corecrypto/ccdigest.h>
#import <corecrypto/ccsha1.h>
#import <corecrypto/ccpbkdf2.h>

@implementation PBKDF_HMACValidation

#define kMaxExpectedSize 128

struct pbkdf2_hmac_test_info {
    const struct ccdigest_info*   di;
    const char*                   password;
    const char*                   salt;
    const unsigned long           iterations;
    const unsigned long           resultSize;
    const uint8_t                 expected[kMaxExpectedSize];
};

const struct pbkdf2_hmac_test_info unitTests[] = {
    {
        .di         = &ccsha1_eay_di,
        .password   = "password",
        .salt       = "salt",
        .iterations = 1,
        .resultSize = 20,
        .expected   = { 0x0c, 0x60, 0xc8, 0x0f,
                        0x96, 0x1f, 0x0e, 0x71,
                        0xf3, 0xa9, 0xb5, 0x24,
                        0xaf, 0x60, 0x12, 0x06, 
                        0x2f, 0xe0, 0x37, 0xa6 }
    },
    {
        .di         = &ccsha1_eay_di,
        .password   = "password",
        .salt       = "salt",
        .iterations = 2,
        .resultSize = 20,
        .expected   = { 0xea, 0x6c, 0x01, 0x4d,
                        0xc7, 0x2d, 0x6f, 0x8c,
                        0xcd, 0x1e, 0xd9, 0x2a,
                        0xce, 0x1d, 0x41, 0xf0,
                        0xd8, 0xde, 0x89, 0x57 }
    },
    {
        .di         = &ccsha1_eay_di,
        .password   = "password",
        .salt       = "salt",
        .iterations = 4096,
        .resultSize = 20,
        .expected   = { 0x4b, 0x00, 0x79, 0x01,
                        0xb7, 0x65, 0x48, 0x9a,
                        0xbe, 0xad, 0x49, 0xd9,
                        0x26, 0xf7, 0x21, 0xd0,
                        0x65, 0xa4, 0x29, 0xc1 }
    },
    {
        .di         = &ccsha1_eay_di,
        .password   = "ThisPasswordIsMoreThan100BytesLongThisPasswordIsMoreThan100BytesLongThisPasswordIsMoreThan100BytesLongThisPasswordIsMoreThan100BytesLongThisPasswordIsMoreThan100BytesLongThisPasswordIsMoreThan100BytesLongThisPasswordIsMoreThan100BytesLongThisPasswordIsMoreThan100BytesLong",
        .salt       = "salt",
        .iterations = 1000,
        .resultSize = 8,
        .expected   = { 0xd9, 0xef, 0xed, 0xda, 0x5a, 0xba, 0x3d, 0xb9 }
    },    
    /* test vectors with big salt and keylen > hash_size */
    /* generated from : http://anandam.name/pbkdf2/ */
    {
        .di         = &ccsha1_eay_di,
        .password   = "passwordPASSWORDpassword", /* (24 octets) */
        .salt       = "saltSALTsaltSALTsaltSALTsaltSALTsalt", /* (36 octets) */
        .iterations = 4096,
        .resultSize = 25,
        .expected   = { 0x3d, 0x2e, 0xec, 0x4f,
                        0xe4, 0x1c, 0x84, 0x9b,
                        0x80, 0xc8, 0xd8, 0x36,
                        0x62, 0xc0, 0xe4, 0x4a,
                        0x8b, 0x29, 0x1a, 0x96,
                        0x4c, 0xf2, 0xf0, 0x70,
                        0x38 }
    },
    {
        .di         = &ccsha1_eay_di,
        .password   = "password",
        .salt       = "saltSALTsaltSALTsaltSALTsaltSALTsaltSALTsaltSALTsaltSALTsaltSALTsaltSALTsaltSALTsaltSALTsaltSALTsaltSALTsaltSALTsaltSALTsaltSALT", /* (128octets) */
        .iterations = 1,
        .resultSize = 4,
        .expected   = { 0x34, 0x3b, 0x6c, 0x04 }
    },
#if USE_SLOW_TEST
    {
        .di         = &ccsha1_eay_di,
        .password   = "password",
        .salt       = "salt",
        .iterations = 16777216,
        .resultSize = 20,
        .expected   = { 0xee, 0xfe, 0x3d, 0x61,
                        0xcd, 0x4d, 0xa4, 0xe4,
                        0xe9, 0x94, 0x5b, 0x3d,
                        0x6b, 0xa2, 0x15, 0x8c,
                        0x26, 0x34, 0xe9, 0x84 }
    }
#endif
};
    
#define kTotalTests (sizeof(unitTests)/sizeof(unitTests[0]))


- (void) testPBKDF_HMAC {
    for (unsigned long testNumber = 0; testNumber < kTotalTests; ++testNumber) {
        const struct pbkdf2_hmac_test_info* currentTest = unitTests + testNumber;
        uint8_t result[currentTest->resultSize];

        ccpbkdf2_hmac(currentTest->di,
                      strlen(currentTest->password), currentTest->password,
                      strlen(currentTest->salt), currentTest->salt,
                      currentTest->iterations,
                      currentTest->resultSize, result);
        
        XCAssertMemEquals(currentTest->resultSize, (const unsigned char *)result,
                          (const unsigned char *)currentTest->expected, @"pbkdf failed");
    }
}

@end
