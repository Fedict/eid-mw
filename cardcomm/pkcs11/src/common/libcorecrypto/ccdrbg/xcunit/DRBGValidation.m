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


#import "DRBGValidation.h"

#import <corecrypto/ccdrbg.h>
#import <corecrypto/ccdrbg_test.h>
#import <corecrypto/ccaes.h>
#import <corecrypto/ccsha1.h>
#import <corecrypto/ccsha2.h>
#import "cc_unit.h"

#include <stdlib.h>

/* TODO: Refactor */
static uint8_t hex_nibble(uint8_t hex) {
    if ('0' <= hex && hex <= '9')
        return hex - '0';
    else if ('a' <= hex && hex <= 'f')
        return hex + 10 - 'a';
    else if ('A' <= hex && hex <= 'F')
        return hex + 10 - 'A';
    else {
        NSLog(@"invalid hex digit: %d[%c]\n", hex, hex);
        return 0;
    }
}

static NSData *hex2binary(NSString *hex) {
    NSData *hex_d = [hex dataUsingEncoding: NSASCIIStringEncoding];
    NSUInteger hex_l = [hex_d length];
    NSMutableData *binary = [NSMutableData dataWithLength: (hex_l + 1) / 2];

    const uint8_t *s = (const uint8_t *)[hex_d bytes];
    uint8_t *d = (uint8_t *)[binary mutableBytes];
    const uint8_t *p = s + hex_l;
    d += (hex_l + 1) / 2;
    while (p > s + 1) {
        *--d = hex_nibble(*--p);
        *d |= (uint8_t)(hex_nibble(*--p) << 4);
    }
    if (p > s)
        *--d = hex_nibble(*--p);

    return binary;
}


static NSString *byte_string(size_t len, const unsigned char *s) {
    NSMutableString *r = [[NSMutableString alloc] initWithCapacity: len * 2];
    for (size_t ix = 0; ix<len; ix++) {
        [r appendFormat: @"%02x", s[ix]];
    }
    [r autorelease];
    return r;
}


@implementation DRBGValidation

- (void) commonTest: (struct ccdrbg_info *)info {
    unsigned char bytes[16];
    unsigned char entropy[16];
    unsigned long len=16;

    uint8_t state[info->size];
    struct ccdrbg_state *rng=(struct ccdrbg_state *)state;

    while(len--) {
        entropy[len]=(unsigned char)arc4random();
    }

    XCTAssertEqual(CCDRBG_STATUS_OK,info->init(info, rng, 16, entropy, 0, NULL, 0, NULL), @"Init");


    XCTAssertEqual(CCDRBG_STATUS_OK,ccdrbg_generate(info, rng, 4, bytes, 0, NULL), @"Generate 1");
    XCTAssertEqual(CCDRBG_STATUS_OK,ccdrbg_generate(info, rng, 4, bytes+4, 0, NULL), @"Generate 2");

    len=16;
    while(len--) {
        entropy[len]=(unsigned char)arc4random();
    }

    XCTAssertEqual(0,ccdrbg_reseed(info, rng, 16, entropy, 0, NULL), @"Reseed ");
    XCTAssertEqual(0,ccdrbg_generate(info, rng, 4, bytes+8, 0, NULL), @"Generate 3 ");
    XCTAssertEqual(0,ccdrbg_generate(info, rng, 4, bytes+12, 0, NULL), @"Generate 4 ");
}

- (void) testDummy {
    struct ccdrbg_info *info=&ccdrbg_dummy_info;

    [self commonTest:info ];
}


/*
#include "CTR_DRBG-AES-128-df-PR.inc"
#include "CTR_DRBG-AES-128-nodf-PR.inc"
#include "CTR_DRBG-AES-192-df-PR.inc"
#include "CTR_DRBG-AES-192-nodf-PR.inc"
#include "CTR_DRBG-AES-256-df-PR.inc"
#include "CTR_DRBG-AES-256-nodf-PR.inc"
*/

static struct ccdrbg_vector nistctr_aes128_df_vectors[] = {
#include "CTR_DRBG-AES-128-df.inc"
};

static struct ccdrbg_vector nistctr_aes128_nodf_vectors[] = {
#include "CTR_DRBG-AES-128-nodf.inc"
};

static struct ccdrbg_vector nistctr_aes192_df_vectors[] = {
#include "CTR_DRBG-AES-192-df.inc"
};

static struct ccdrbg_vector nistctr_aes192_nodf_vectors[] = {
#include "CTR_DRBG-AES-192-nodf.inc"
};

static struct ccdrbg_vector nistctr_aes256_df_vectors[] = {
#include "CTR_DRBG-AES-256-df.inc"
};

static struct ccdrbg_vector nistctr_aes256_nodf_vectors[] = {
#include "CTR_DRBG-AES-256-nodf.inc"
};

static struct ccdrbg_PR_vector nistctr_aes128_df_PR_vectors[] = {
#include "CTR_DRBG-AES-128-df-PR.inc"
};

static struct ccdrbg_PR_vector nistctr_aes128_nodf_PR_vectors[] = {
#include "CTR_DRBG-AES-128-nodf-PR.inc"
};

static struct ccdrbg_PR_vector nistctr_aes192_df_PR_vectors[] = {
#include "CTR_DRBG-AES-192-df-PR.inc"
};

static struct ccdrbg_PR_vector nistctr_aes192_nodf_PR_vectors[] = {
#include "CTR_DRBG-AES-192-nodf-PR.inc"
};

static struct ccdrbg_PR_vector nistctr_aes256_df_PR_vectors[] = {
#include "CTR_DRBG-AES-256-df-PR.inc"
};

static struct ccdrbg_PR_vector nistctr_aes256_nodf_PR_vectors[] = {
#include "CTR_DRBG-AES-256-nodf-PR.inc"
};

static struct ccdrbg_vector nisthmac_sha1_vectors[] = {
#include "HMAC_DRBG-SHA-1.inc"
};

static struct ccdrbg_vector nisthmac_sha224_vectors[] = {
#include "HMAC_DRBG-SHA-224.inc"
};

static struct ccdrbg_vector nisthmac_sha256_vectors[] = {
#include "HMAC_DRBG-SHA-256.inc"
};

#if 0
static struct ccdrbg_vector nisthmac_sha512_224_vectors[] = {
#include "HMAC_DRBG-SHA-512-224.inc"
};

static struct ccdrbg_vector nisthmac_sha512_256_vectors[] = {
#include "HMAC_DRBG-SHA-512-256.inc"
};
#endif

static struct ccdrbg_vector nisthmac_sha384_vectors[] = {
#include "HMAC_DRBG-SHA-384.inc"
};

static struct ccdrbg_vector nisthmac_sha512_vectors[] = {
#include "HMAC_DRBG-SHA-512.inc"
};

- (void) commonTestNistCtr:(const struct ccmode_ecb *)ecb :(unsigned long) keylen :(int)df :(struct ccdrbg_vector *)v :(unsigned long)n {
    struct ccdrbg_info info;
    struct ccdrbg_nistctr_custom custom = {
        .ecb = ecb,
        .keylen = keylen,
        .strictFIPS = 0,
        .use_df = df,
    };

    ccdrbg_factory_nistctr(&info, &custom);

    for(unsigned long i=0; i<n; i++)
    {
        unsigned char temp[v[i].randomLen];
        ccdrbg_nist_test_vector(&info, &v[i], temp);
        XCAssertMemEquals(v[i].randomLen, temp, v[i].random, @"vector %lu", i);
    }
}

- (void) commonTestNistHMAC:(const struct ccdigest_info *)di :(struct ccdrbg_vector *)v :(unsigned long)n {
    struct ccdrbg_info info;
    struct ccdrbg_nisthmac_custom custom = {
        .di = di,
        .strictFIPS = 0,
    };

    ccdrbg_factory_nisthmac(&info, &custom);

    for(unsigned long i=0; i<n; i++)
    {
        unsigned char temp[v[i].randomLen];
        ccdrbg_nist_14_3_test_vector(&info, &v[i], temp);
        XCAssertMemEquals(v[i].randomLen, temp, v[i].random, @"vector %lu", i);
    }
}

- (void) commonTestNistCtrPR:(const struct ccmode_ecb *)ecb :(unsigned long)keylen :(int)df :(struct ccdrbg_PR_vector *)v :(unsigned long)n {
    struct ccdrbg_info info;
    struct ccdrbg_nistctr_custom custom;
    unsigned long i;

    custom.ecb=ecb;
    custom.keylen=keylen;
    custom.strictFIPS=0;
    custom.use_df=df;

    ccdrbg_factory_nistctr(&info, &custom);

    for(i=0; i<n; i++)
    {
        unsigned char temp[v[i].randomLen];
        ccdrbg_nist_PR_test_vector(&info, &v[i], temp);
        XCAssertMemEquals(v[i].randomLen, temp, v[i].random, @"vector %lu", i);
    }
}



#define NOF(x) (x):(sizeof(x)/sizeof((x)[0]))

- (void) testNistCtrAES128_df {
    const struct ccmode_ecb *ecb=ccaes_ecb_encrypt_mode();
    [self commonTestNistCtr:ecb:16:1:NOF(nistctr_aes128_df_vectors)];
}

- (void) testNistCtrAES192_df {
    const struct ccmode_ecb *ecb=ccaes_ecb_encrypt_mode();
    [self commonTestNistCtr:ecb:24:1:NOF(nistctr_aes192_df_vectors)];
}

- (void) testNistCtrAES256_df {
    const struct ccmode_ecb *ecb=ccaes_ecb_encrypt_mode();
    [self commonTestNistCtr:ecb:32:1:NOF(nistctr_aes256_df_vectors)];
}

- (void) testNistCtrAES128_nodf {
    const struct ccmode_ecb *ecb=ccaes_ecb_encrypt_mode();
    [self commonTestNistCtr:ecb:16:0:NOF(nistctr_aes128_nodf_vectors)];
}

- (void) testNistCtrAES192_nodf {
    const struct ccmode_ecb *ecb=ccaes_ecb_encrypt_mode();

    [self commonTestNistCtr:ecb:24:0:NOF(nistctr_aes192_nodf_vectors)];
}

- (void) testNistCtrAES256_nodf {
    const struct ccmode_ecb *ecb=ccaes_ecb_encrypt_mode();
    [self commonTestNistCtr:ecb:32:0:NOF(nistctr_aes256_nodf_vectors)];
}


- (void) testNistCtrAES128_df_PR {
    const struct ccmode_ecb *ecb=ccaes_ecb_encrypt_mode();
    [self commonTestNistCtrPR:ecb:16:1:NOF(nistctr_aes128_df_PR_vectors)];
}

- (void) testNistCtrAES192_df_PR {
    const struct ccmode_ecb *ecb=ccaes_ecb_encrypt_mode();
    [self commonTestNistCtrPR:ecb:24:1:NOF(nistctr_aes192_df_PR_vectors)];
}

- (void) testNistCtrAES256_df_PR {
    const struct ccmode_ecb *ecb=ccaes_ecb_encrypt_mode();
    [self commonTestNistCtrPR:ecb:32:1:NOF(nistctr_aes256_df_PR_vectors)];
}

- (void) testNistCtrAES128_nodf_PR {
    const struct ccmode_ecb *ecb=ccaes_ecb_encrypt_mode();
    [self commonTestNistCtrPR:ecb:16:0:NOF(nistctr_aes128_nodf_PR_vectors)];
}

- (void) testNistCtrAES192_nodf_PR {
    const struct ccmode_ecb *ecb=ccaes_ecb_encrypt_mode();

    [self commonTestNistCtrPR:ecb:24:0:NOF(nistctr_aes192_nodf_PR_vectors)];
}

- (void) testNistCtrAES256_nodf_PR {
    const struct ccmode_ecb *ecb=ccaes_ecb_encrypt_mode();
    [self commonTestNistCtrPR:ecb:32:0:NOF(nistctr_aes256_nodf_PR_vectors)];
}

- (void) testNistHMACSHA1 {
    const struct ccdigest_info *di = ccsha1_di();
    [self commonTestNistHMAC:di:NOF(nisthmac_sha1_vectors)];
}

- (void) testNistHMACSHA224 {
    const struct ccdigest_info *di = ccsha224_di();
    [self commonTestNistHMAC:di:NOF(nisthmac_sha224_vectors)];
}

- (void) testNistHMACSHA256 {
    const struct ccdigest_info *di = ccsha256_di();
    [self commonTestNistHMAC:di:NOF(nisthmac_sha256_vectors)];
}

#if 0   // TODO: need real SHA512/224
- (void) testNistHMACSHA512_224 {
    const struct ccdigest_info *di = ccsha512_di();
    struct ccdigest_info sha512_224di = *di;
    sha512_224di.output_size = CCSHA224_OUTPUT_SIZE;
    for (int i = 0 ; i < 512 / 64; i++) {
        ((uint64_t *)sha512_224di.initial_state)[i] ^= 0xa5a5a5a5a5a5a5a5;
    }
    [self commonTestNistHMAC:&sha512_224di:NOF(nisthmac_sha512_224_vectors)];
}

- (void) testNistHMACSHA512_256 {
    const struct ccdigest_info *di = ccsha512_di();
    struct ccdigest_info sha512_256di = *di;
    sha512_256di.output_size = CCSHA256_OUTPUT_SIZE;
    for (int i = 0 ; i < 512 / 64; i++) {
        ((uint64_t *)sha512_256di.initial_state)[i] ^= 0xa5a5a5a5a5a5a5a5;
    }
    [self commonTestNistHMAC:&sha512_256di:NOF(nisthmac_sha512_256_vectors)];
}
#endif

- (void) testNistHMACSHA384 {
    const struct ccdigest_info *di = ccsha384_di();
    [self commonTestNistHMAC:di:NOF(nisthmac_sha384_vectors)];
}

- (void) testNistHMACSHA512 {
    const struct ccdigest_info *di = ccsha512_di();
    [self commonTestNistHMAC:di:NOF(nisthmac_sha512_vectors)];
}


/*
 AES (encrypt)

 COUNT = 0
 EntropyInput = b9ad873294a58a0d6c2e9d072f8a270b
 Nonce = 0d5849ccaa7b8a95
 PersonalizationString =
 AdditionalInput =
 EntropyInputReseed = e47485dda9d246a07c0c39f0cf8cb76b
 AdditionalInputReseed =
 AdditionalInput =
 ReturnedBits = 24bd4f7cc6eb71987ab7b06bd066cc07
*/

- (void) testNistCtrAES128 {
    unsigned char bytes[16];
    struct ccdrbg_info info;
    struct ccdrbg_nistctr_custom custom;

    custom.ecb=ccaes_ecb_encrypt_mode();
    custom.keylen=16;
    custom.strictFIPS=0;
    custom.use_df=1;

    ccdrbg_factory_nistctr(&info, &custom);

    uint8_t state[info.size];
    struct ccdrbg_state *rng=(struct ccdrbg_state *)state;

    NSData *entropy=hex2binary(@"b9ad873294a58a0d6c2e9d072f8a270b");
    NSData *nonce=hex2binary(@"0d5849ccaa7b8a95");
    NSData *reseed=hex2binary(@"e47485dda9d246a07c0c39f0cf8cb76b");
    NSString *result=@"24bd4f7cc6eb71987ab7b06bd066cc07";

    /* typecast: size of entropy and nonce must be less than 4GB, and fit in uint32_t */
    XCTAssertEqual(CCDRBG_STATUS_OK,info.init(&info, rng, (uint32_t)[entropy length], [entropy bytes],
                                              (uint32_t)[nonce length], [nonce bytes], 0, NULL), @"init");

    XCTAssertEqual(CCDRBG_STATUS_OK,ccdrbg_generate(&info, rng, 16, bytes, 0, NULL), @"Generate 1");
    XCTAssertEqual(CCDRBG_STATUS_OK,ccdrbg_reseed(&info, rng, [reseed length], [reseed bytes], 0, NULL), @"Reseed");
    XCTAssertEqual(CCDRBG_STATUS_OK,ccdrbg_generate(&info, rng, 16, bytes, 0, NULL), @"Generate 2");

    XCTAssertEqualObjects(byte_string(16, bytes), result, @"returned bytes");
}

/* AES-128 no df
COUNT = 0
EntropyInput = 420edbaff787fdbd729e12c2f3cfc0ec6704de59bf28ed438bf0d86ddde7ebcc
Nonce = be293b972894533b
PersonalizationString =
AdditionalInput =
EntropyInputReseed = a821c34b7505291f80341e37f930451659091550bef04cb68a01b1be394b1037
AdditionalInputReseed =
AdditionalInput =
ReturnedBits = 263c1cf3fd8c0bcb1ed754ce10cfc2fc
*/

- (void) testNistCtrAES128nodf {
    unsigned char bytes[16];
    struct ccdrbg_info info;
    struct ccdrbg_nistctr_custom custom;

    custom.ecb=ccaes_ecb_encrypt_mode();
    custom.keylen=16;
    custom.strictFIPS=0;
    custom.use_df=0;

    ccdrbg_factory_nistctr(&info, &custom);

    uint8_t state[info.size];
    struct ccdrbg_state *rng=(struct ccdrbg_state *)state;

    NSData *entropy=hex2binary(@"420edbaff787fdbd729e12c2f3cfc0ec6704de59bf28ed438bf0d86ddde7ebcc");
    NSData *nonce=hex2binary(@"be293b972894533b");
    NSData *reseed=hex2binary(@"a821c34b7505291f80341e37f930451659091550bef04cb68a01b1be394b1037");
    NSString *result=@"263c1cf3fd8c0bcb1ed754ce10cfc2fc";

    /* typecast: size of entropy and nonce must be less than 4GB, and fit in uint32_t */
    XCTAssertEqual(CCDRBG_STATUS_OK,info.init(&info, rng, (uint32_t)[entropy length], [entropy bytes],
                                              (uint32_t)[nonce length], [nonce bytes], 0, NULL), @"init");

    XCTAssertEqual(CCDRBG_STATUS_OK,ccdrbg_generate(&info, rng, 16, bytes, 0, NULL), @"Generate 1");
    XCTAssertEqual(CCDRBG_STATUS_OK,ccdrbg_reseed(&info, rng, [reseed length], [reseed bytes], 0, NULL), @"Reseed");
    XCTAssertEqual(CCDRBG_STATUS_OK,ccdrbg_generate(&info, rng, 16, bytes, 0, NULL), @"Generate 2");

    XCTAssertEqualObjects(byte_string(16, bytes), result, @"returned bytes");
}

/* AES-256

 COUNT = 0
 EntropyInput = ec0197a55b0c9962d549b161e96e732a0ee3e177004fe95f5d6120bf82e2c0ea
 Nonce = 9b131c601efd6a7cc2a21cd0534de8d8
 PersonalizationString =
 AdditionalInput =
 EntropyInputReseed = 61810b74d2ed76365ae70ee6772bba4938ee38d819ec1a741fb3ff4c352f140c
 AdditionalInputReseed =
 AdditionalInput =
 ReturnedBits = 7ea89ce613e11b5de7f979e14eb0da4d

 */

- (void) testNistCtrAES256 {
    unsigned char bytes[16];
    struct ccdrbg_info info;
    struct ccdrbg_nistctr_custom custom;

    custom.ecb=ccaes_ecb_encrypt_mode();
    custom.keylen=32;
    custom.strictFIPS=0;
    custom.use_df=1;

    ccdrbg_factory_nistctr(&info, &custom);

    uint8_t state[info.size];
    struct ccdrbg_state *rng=(struct ccdrbg_state *)state;

    NSData *entropy=hex2binary(@"ec0197a55b0c9962d549b161e96e732a0ee3e177004fe95f5d6120bf82e2c0ea");
    NSData *nonce=hex2binary(@"9b131c601efd6a7cc2a21cd0534de8d8");
    NSData *reseed=hex2binary(@"61810b74d2ed76365ae70ee6772bba4938ee38d819ec1a741fb3ff4c352f140c");
    NSString *result=@"7ea89ce613e11b5de7f979e14eb0da4d";

    /* typecast: size of entropy and nonce must be less than 4GB, and fit in uint32_t */
    XCTAssertEqual(CCDRBG_STATUS_OK,info.init(&info, rng, (uint32_t)[entropy length], [entropy bytes],
                                              (uint32_t)[nonce length], [nonce bytes], 0, NULL), @"init");

    XCTAssertEqual(CCDRBG_STATUS_OK,ccdrbg_generate(&info, rng, 16, bytes, 0, NULL), @"Generate 1");
    XCTAssertEqual(CCDRBG_STATUS_OK,ccdrbg_reseed(&info, rng, [reseed length], [reseed bytes], 0, NULL), @"Reseed");
    XCTAssertEqual(CCDRBG_STATUS_OK,ccdrbg_generate(&info, rng, 16, bytes, 0, NULL), @"Generate 2");

    XCTAssertEqualObjects(byte_string(16, bytes), result, @"returned bytes");
}

/* AES-192

 COUNT = 0
 EntropyInput = 1e259e4e7f5b4c5b5b4d5119f2cde4853dc1dd131172f394
 Nonce = 40347af9fb51845a5d3712a2169065cb
 PersonalizationString =
 AdditionalInput =
 EntropyInputReseed = 82bd0a6027531a768163ff636d88a8e7513018117627da6d
 AdditionalInputReseed =
 AdditionalInput =
 ReturnedBits = 0b4de73186bde75f0d4d551ba55af931

*/

 - (void) testNistCtrAES192 {
    unsigned char bytes[16];
    struct ccdrbg_info info;
    struct ccdrbg_nistctr_custom custom;

    custom.ecb=ccaes_ecb_encrypt_mode();
    custom.keylen=24;
    custom.strictFIPS=0;
    custom.use_df=1;

    ccdrbg_factory_nistctr(&info, &custom);

    uint8_t state[info.size];
    struct ccdrbg_state *rng=(struct ccdrbg_state *)state;

    NSData *entropy=hex2binary(@"1e259e4e7f5b4c5b5b4d5119f2cde4853dc1dd131172f394");
    NSData *nonce=hex2binary(@"40347af9fb51845a5d3712a2169065cb");
    NSData *reseed=hex2binary(@"82bd0a6027531a768163ff636d88a8e7513018117627da6d");
    NSString *result=@"0b4de73186bde75f0d4d551ba55af931";

    /* typecast: size of entropy and nonce must be less than 4GB, and fit in uint32_t */
    XCTAssertEqual(CCDRBG_STATUS_OK,info.init(&info, rng, (uint32_t)[entropy length], [entropy bytes],
                                               (uint32_t)[nonce length], [nonce bytes], 0, NULL), @"init");

    XCTAssertEqual(CCDRBG_STATUS_OK,ccdrbg_generate(&info, rng, 16, bytes, 0, NULL), @"Generate 1");
    XCTAssertEqual(CCDRBG_STATUS_OK,ccdrbg_reseed(&info, rng, [reseed length], [reseed bytes], 0, NULL), @"Reseed");
    XCTAssertEqual(CCDRBG_STATUS_OK,ccdrbg_generate(&info, rng, 16, bytes, 0, NULL), @"Generate 2");

    XCTAssertEqualObjects(byte_string(16, bytes), result, @"returned bytes");
}

#ifdef SOMEONE_FIXED_DRBG_TO_SUPPORT_KEYLEN_NOT_MULTIPLE_OF_FOUR

/* The drbg assume that the keylen is a multiple of 4 bytes - not the case for tdes */

/*
 3KeyTDEA (encrypt)

 COUNT = 0
 EntropyInput = 994c6b36fbd570abdff0925149ad
 Nonce = 1af5034e727780
 PersonalizationString =
 AdditionalInput =
 EntropyInputReseed = 6f29a7962aa01f31cb56aa6492c4
 AdditionalInputReseed =
 AdditionalInput =
 ReturnedBits = 523deda06869cad8
 */


- (void) testNistCtrTDES168 {
    struct ccdrbg_nistctr_state rng;
    unsigned char bytes[8];
    const struct ccmode_ecb *ecb=&ccdes168_ltc_ecb_encrypt_mode;
    uint32_t keys[CCDRBG_NISTCTR_SIZE(ecb, 168/8)/sizeof(uint32_t)];

    NSData *entropy=hex2binary(@"994c6b36fbd570abdff0925149ad");
    NSData *nonce=hex2binary(@"1af5034e727780");
    NSData *reseed=hex2binary(@"6f29a7962aa01f31cb56aa6492c4");
    NSString *result=@"523deda06869cad8";

    /* typecast: size of entropy and nonce must be less than 4GB, and fit in uint32_t */
    XCAssertEquals(CCDRBG_STATUS_OK,ccdrbg_nistctr_init(&rng, ecb, 168/8, keys, [entropy bytes], (uint32_t)[entropy length],
                                                        [nonce bytes], (uint32_t)[nonce length], NULL, 0, 1, 1), @"init");

    XCAssertEquals(CCDRBG_STATUS_OK,ccdrbg_generate(&rng, 8, bytes), @"Generate 1");
    XCAssertEquals(CCDRBG_STATUS_OK,ccdrbg_reseed(&rng, [reseed length], [reseed bytes]), @"Reseed");
    XCAssertEquals(CCDRBG_STATUS_OK,ccdrbg_generate(&rng, 8, bytes), @"Generate 2");

    XCAssertEqualObjects(byte_string(8, bytes), result, @"returned bytes");
}


/* TDEA no df

COUNT = 0
EntropyInput = 8d047024cc4371a3e291508d0aabdeec26bf71f20aaece5a097f57fcbf
Nonce = 10ef051fffb725
PersonalizationString =
AdditionalInput =
EntropyInputReseed = 13fce32a710a1e3341b0e6941cb789ad47572f981f18e51fc7e7ebfc0f
AdditionalInputReseed =
AdditionalInput =
ReturnedBits = e652a6d23313ef59

*/

- (void) testNistCtrTDES168nodf {
    struct ccdrbg_nistctr_state rng;
    unsigned char bytes[8];
    const struct ccmode_ecb *ecb=&ccdes168_ltc_ecb_encrypt_mode;
    uint32_t keys[CCDRBG_NISTCTR_SIZE(ecb, 168/8)/sizeof(uint32_t)];

    NSData *entropy=hex2binary(@"8d047024cc4371a3e291508d0aabdeec26bf71f20aaece5a097f57fcbf");
    NSData *nonce=hex2binary(@"10ef051fffb725");
    NSData *reseed=hex2binary(@"13fce32a710a1e3341b0e6941cb789ad47572f981f18e51fc7e7ebfc0f");
    NSString *result=@"e652a6d23313ef59";

    /* typecast: size of entropy and nonce must be less than 4GB, and fit in uint32_t */
    XCAssertEquals(CCDRBG_STATUS_OK,ccdrbg_nistctr_init(&rng, ecb, 168/8, keys, [entropy bytes], (uint32_t)[entropy length],
                                                        [nonce bytes], (uint32_t)[nonce length], NULL, 0, 1, 0), @"init");

    XCAssertEquals(CCDRBG_STATUS_OK,ccdrbg_generate(&rng, 8, bytes), @"Generate 1");
    XCAssertEquals(CCDRBG_STATUS_OK,ccdrbg_reseed(&rng, [reseed length], [reseed bytes]), @"Reseed");
    XCAssertEquals(CCDRBG_STATUS_OK,ccdrbg_generate(&rng, 8, bytes), @"Generate 2");

    XCAssertEqualObjects(byte_string(8, bytes), result, @"returned bytes");
}

#endif


@end
