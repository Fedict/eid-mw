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


#import "HMACValidation.h"

#import <corecrypto/cchmac.h>
#import <corecrypto/ccmd5.h>
#import <corecrypto/ccsha1.h>
#import <corecrypto/ccsha2.h>

@implementation HMACValidation


/* Nist CAVP vectors specifies the hash as L=xx - those are the matching hashes implementations */
/* We use implementations that are available on all platforms, it does not matter which as we are not testing the hash implementation here */
#define hmac_di_20 &ccsha1_eay_di
#define hmac_di_28 &ccsha224_ltc_di
#define hmac_di_32 &ccsha256_ltc_di
#define hmac_di_48 &ccsha384_ltc_di
#define hmac_di_64 &ccsha512_ltc_di

const struct cchmac_test_input hmac_vectors[] ={
{
    &ccmd5_ltc_di,
    10, "\xfe\x34\x67\xf6\xbe\xdc\x12\x04\x6a\x5c",
    10, "\xfe\x34\x67\xf6\xbe\xdc\x12\x04\x6a\x5c",
    16, "\x9a\x26\xc2\x30\x1f\x0c\xb0\x9c\x5c\xea\x0d\xdb\x43\xf6\x50\x34"
},
{
    &ccsha1_ltc_di,
    10, "\xfe\x34\x67\xf6\xbe\xdc\x12\x04\x6a\x5c",
    10, "\xfe\x34\x67\xf6\xbe\xdc\x12\x04\x6a\x5c",
    20, "\x2f\x74\x92\xb9\x39\xb3\x97\x44\x39\xa9\xdc\x2e\xab\xcc\x69\x9c\xec\xc2\x3b\x02"
},
{
    &ccsha224_ltc_di,
    10, "\xfe\x34\x67\xf6\xbe\xdc\x12\x04\x6a\x5c",
    10, "\xfe\x34\x67\xf6\xbe\xdc\x12\x04\x6a\x5c",
    28, "\xef\xf1\xcd\xa4\x3e\xe4\x0d\x65\x1a\xfa\x9f\x41\x19\xc1\xde\x45\x4c\x49\xfd\xbc\x21\x4d\x6e\x9e\x29\xc4\x06\x89",
},
{
    &ccsha256_ltc_di,
    10, "\xfe\x34\x67\xf6\xbe\xdc\x12\x04\x6a\x5c",
    10, "\xfe\x34\x67\xf6\xbe\xdc\x12\x04\x6a\x5c",
    32, "\x80\x93\x01\xb1\x80\x58\x78\x2e\xc6\x61\x46\x3b\xdd\x77\x8e\x3c\xea\x82\x65\xff\x10\xac\xf7\x1d\x3f\x6d\x5b\x8f\xbf\xbd\xb5\x8a"
},
{
    &ccsha384_ltc_di,
    10, "\xfe\x34\x67\xf6\xbe\xdc\x12\x04\x6a\x5c",
    10, "\xfe\x34\x67\xf6\xbe\xdc\x12\x04\x6a\x5c",
    48, "\xbe\x3c\x3e\x8e\x91\xd4\x71\x5e\xb4\x8f\x3d\x10\x28\xcd\x0e\xb2\x8c\xa5\x17\x10\x67\xef\x68\xc0\x1f\x0e\x53\xc5\xbb\x77\xa8\xce\xd6\x51\xe0\xdf\x0f\x82\xbd\x53\xf6\x2e\x82\xea\x07\x23\x9e\x7b"
},
{
    &ccsha512_ltc_di,
    10, "\xfe\x34\x67\xf6\xbe\xdc\x12\x04\x6a\x5c",
    10, "\xfe\x34\x67\xf6\xbe\xdc\x12\x04\x6a\x5c",
    64, "\xb3\x7a\xaa\x28\xc4\xea\xd6\xa5\xa1\x1c\xc9\xb3\xbb\x47\x1d\x47\x1f\x0c\x43\xb6\x31\x2f\x76\x57\x07\xed\x67\xce\xfa\x81\x6b\xf6\xd7\xc6\xb7\xbc\x1f\x3e\x51\xfe\xd8\xe4\x86\x4b\x4e\xca\x3b\x59\x6b\xb7\xc3\x45\x74\x8a\x9d\x45\x49\x7e\xd6\x7d\x53\x8a\x22\x3f"
},
{
    &ccsha1_eay_di,
    80, "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef",
    6, "abcdef",
    20, "\x37\x5d\xee\xf8\x45\xd7\x46\x58\xb1\xdc\xd4\xd5\x77\x26\xba\x86\x78\xe8\x61\xdc",
},

#include "HMAC.inc"

};

- (void) testOneShot {
    for (size_t i=0; i < sizeof(hmac_vectors)/sizeof(hmac_vectors[0]); i++) {
        const struct cchmac_test_input *v=&hmac_vectors[i];
        XCTAssertEqual(0, cchmac_test(v), @"Vectors %u", (unsigned int)i);
    }
}

- (void) testChunks {
    for (size_t i=0; i < sizeof(hmac_vectors)/sizeof(hmac_vectors[0]); i++) {
        const struct cchmac_test_input *v=&hmac_vectors[i];
        XCTAssertEqual(0, cchmac_test_chunks(v, 1), @"Vectors %u", (unsigned int)i);
    }
}
@end
