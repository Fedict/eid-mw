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


#import "RMDValidation.h"

#import <corecrypto/ccripemd.h>

static NSString *rmd_string(const unsigned char *s, size_t len) {
    NSMutableString *r = [[NSMutableString alloc] initWithCapacity: len * 2];
    for (size_t ix = 0; ix<len; ix++) {
        [r appendFormat: @"%02x", s[ix]];
    }
    [r autorelease];
    return r;
}

@implementation RMDValidation


#define N_VECTORS 7
static const char *text[N_VECTORS]={
    "",
    "a",
    "abc",
    "message digest",
    "abcdefghijklmnopqrstuvwxyz",
    "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
};

static NSString *rmd128_digest[N_VECTORS]={
    @"cdf26213a150dc3ecb610f18f6b38b46",
    @"86be7afa339d0fc7cfc785e72f578d33",
    @"c14a12199c66e4ba84636b0f69144c77",
    @"9e327b3d6e523062afc1132d7df9d1b8",
    @"fd2aa607f71dc8f510714922b371834e",
    @"a1aa0689d0fafa2ddc22e88b49133a06",
    @"d1e959eb179c911faea4624c60c5c702",

};

static NSString *rmd160_digest[N_VECTORS]={
    @"9c1185a5c5e9fc54612808977ee8f548b2258d31",
    @"0bdc9d2d256b3ee9daae347be6f4dc835a467ffe",
    @"8eb208f7e05d987a9b044a8e98c6b087f15a0bfc",
    @"5d0689ef49d2fae572b881b123a85ffa21595f36",
    @"f71c27109c692c1b56bbdceb5b9d2865b3708dbc",
    @"12a053384a9c0c88e405a06c27dcf49ada62eb2b",
    @"b0e20b6e3116640286ed3a87a5713079b21f5189",
};

static NSString *rmd256_digest[N_VECTORS]={
    @"02ba4c4e5f8ecd1877fc52d64d30e37a2d9774fb1e5d026380ae0168e3c5522d",
    @"f9333e45d857f5d90a91bab70a1eba0cfb1be4b0783c9acfcd883a9134692925",
    @"afbd6e228b9d8cbbcef5ca2d03e6dba10ac0bc7dcbe4680e1e42d2e975459b65",
    @"87e971759a1ce47a514d5c914c392c9018c7c46bc14465554afcdf54a5070c0e",
    @"649d3034751ea216776bf9a18acc81bc7896118a5197968782dd1fd97d8d5133",
    @"3843045583aac6c8c8d9128573e7a9809afb2a0f34ccc36ea9e72f16f6368e3f",
    @"5740a408ac16b720b84424ae931cbb1fe363d1d0bf4017f1a89f7ea6de77a0b8",
};

static NSString *rmd320_digest[N_VECTORS]={
    @"22d65d5661536cdc75c1fdf5c6de7b41b9f27325ebc61e8557177d705a0ec880151c3a32a00899b8",
    @"ce78850638f92658a5a585097579926dda667a5716562cfcf6fbe77f63542f99b04705d6970dff5d",
    @"de4c01b3054f8930a79d09ae738e92301e5a17085beffdc1b8d116713e74f82fa942d64cdbc4682d",
    @"3a8e28502ed45d422f68844f9dd316e7b98533fa3f2a91d29f84d425c88d6b4eff727df66a7c0197",
    @"cabdb1810b92470a2093aa6bce05952c28348cf43ff60841975166bb40ed234004b8824463e6b009",
    @"d034a7950cf722021ba4b84df769a5de2060e259df4c9bb4a4268c0e935bbc7470a969c9d072a1ac",
    @"ed544940c86d67f250d232c30b7b3e5770e0c60c8cb9a4cafe3b11388af9920e1b99230b843c86a4",
};

- (void) testRMD128 {
    unsigned char e[CCRMD128_OUTPUT_SIZE];
    for(int i=0; i<N_VECTORS; i++) {
        ccdigest(&ccrmd128_di, strlen(text[i]), (const unsigned char *)text[i], e);
        XCTAssertEqualObjects(rmd_string(e, CCRMD128_OUTPUT_SIZE), rmd128_digest[i], @"Failed RMD128 of \"%s\"", text[i]);
    }
}


- (void) testRMD160 {
    unsigned char e[CCRMD160_OUTPUT_SIZE];
    for(int i=0; i<N_VECTORS; i++) {
        ccdigest(&ccrmd160_di, strlen(text[i]), (const unsigned char *)text[i], e);
        XCTAssertEqualObjects(rmd_string(e, CCRMD160_OUTPUT_SIZE), rmd160_digest[i], @"Failed RMD160 of \"%s\"", text[i]);
    }
}

- (void) testRMD256 {
    unsigned char e[CCRMD256_OUTPUT_SIZE];
    for(int i=0; i<N_VECTORS; i++) {
        ccdigest(&ccrmd256_di, strlen(text[i]), (const unsigned char *)text[i], e);
        XCTAssertEqualObjects(rmd_string(e, CCRMD256_OUTPUT_SIZE), rmd256_digest[i], @"Failed RMD256 of \"%s\"", text[i]);
    }
}

- (void) testRMD320 {
    unsigned char e[CCRMD320_OUTPUT_SIZE];
    for(int i=0; i<N_VECTORS; i++) {
        ccdigest(&ccrmd320_di, strlen(text[i]), (const unsigned char *)text[i], e);
        XCTAssertEqualObjects(rmd_string(e, CCRMD320_OUTPUT_SIZE), rmd320_digest[i], @"Failed RMD320 of \"%s\"", text[i]);
    }
}


@end
