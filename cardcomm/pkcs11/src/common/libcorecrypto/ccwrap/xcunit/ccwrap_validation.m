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


#import "ccwrap_validation.h"

#include <corecrypto/ccwrap.h>
#include <corecrypto/ccaes.h>


@implementation ccwrap_validation

- (void)tryWithKek:(const char *)kek kek_len:(unsigned)kek_len
              key:(const char *)key key_len:(unsigned)key_len
           cipher:(const char *)cipher cipher_len:(unsigned)cipher_len
{
    uint8_t wrapped[cipher_len];
    uint8_t unwrapped[key_len];
    unsigned long osize;

    {
        // set up kek
        const struct ccmode_ecb *ecb_mode = ccaes_ecb_encrypt_mode();
        ccecb_ctx_decl(ecb_mode->size, enc_ctx);
        ecb_mode->init(ecb_mode, enc_ctx, kek_len, kek);

        // in != out
        ccwrap_auth_encrypt(ecb_mode, enc_ctx, key_len, key, &osize, wrapped);
        //unsigned i; for(i=0; i<cipher_len; i++) printf("%02X", wrapped[i]); printf("\n");
        XCTAssertTrue(cipher_len == osize, @"check output size");
        XCTAssertTrue(0 == memcmp(wrapped, cipher, cipher_len), @"in!=out wrap result");

        // in == out
        memcpy(wrapped, key, key_len);
        ccwrap_auth_encrypt(ecb_mode, enc_ctx, key_len, wrapped, &osize, wrapped);
        XCTAssertTrue(cipher_len == osize, @"check output size");
        XCTAssertTrue(0 == memcmp(wrapped, cipher, cipher_len), @"in==out wrap result");
    }

    {
        // set up kek
        const struct ccmode_ecb *ecb_mode = ccaes_ecb_decrypt_mode();
        ccecb_ctx_decl(ecb_mode->size, dec_ctx);
        ecb_mode->init(ecb_mode, dec_ctx, kek_len, kek);

        // in != out
        XCTAssertTrue(0 == ccwrap_auth_decrypt(ecb_mode, dec_ctx, cipher_len, wrapped, &osize, unwrapped), @"in!=out unwrap");
        XCTAssertTrue(key_len == osize, @"check output size");
        XCTAssertTrue(0 == memcmp(unwrapped, key, key_len), @"in!=out unwrap result");

        // in == out
        memcpy(wrapped, cipher, cipher_len);
        XCTAssertTrue(0 == ccwrap_auth_decrypt(ecb_mode, dec_ctx, cipher_len, wrapped, &osize, unwrapped), @"in==out unwrap");
        XCTAssertTrue(key_len == osize, @"check output size");
        XCTAssertTrue(0 == memcmp(unwrapped, key, key_len), @"in==out unwrap result");
    }
}


#pragma mark KW tests

- (void)testKW128_128 {


    // 128 bit key and kek
        const char *kek = "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F";
        const char *key = "\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xAA\xBB\xCC\xDD\xEE\xFF";
        const char *cipher = "\x1F\xA6\x8B\x0A\x81\x12\xB4\x47\xAE\xF3\x4B\xD8\xFB\x5A\x7B\x82\x9D\x3E\x86\x23\x71\xD2\xCF\xE5";
        [self tryWithKek:kek kek_len:16 key:key key_len:16 cipher:cipher cipher_len:24];
}

- (void)testKW128_192 {
    // 128 bit key with 192 bit kek (this test is pointless)
        const char *kek = "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17";
        const char *key = "\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xAA\xBB\xCC\xDD\xEE\xFF";
        const char *cipher = "\x96\x77\x8B\x25\xAE\x6C\xA4\x35\xF9\x2B\x5B\x97\xC0\x50\xAE\xD2\x46\x8A\xB8\xA1\x7A\xD8\x4E\x5D";
        [self tryWithKek:kek kek_len:24 key:key key_len:16 cipher:cipher cipher_len:24];
}

- (void)testKW192_192 {
    // 192 bit key and kek
        const char *kek = "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17";
        const char *key = "\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xAA\xBB\xCC\xDD\xEE\xFF\x00\x01\x02\x03\x04\x05\x06\x07";
        const char *cipher = "\x03\x1D\x33\x26\x4E\x15\xD3\x32\x68\xF2\x4E\xC2\x60\x74\x3E\xDC\xE1\xC6\xC7\xDD\xEE\x72\x5A\x93\x6B\xA8\x14\x91\x5C\x67\x62\xD2";
        [self tryWithKek:kek kek_len:24 key:key key_len:24 cipher:cipher cipher_len:32];
}

- (void)testKW256_256 {

        const char *kek = "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F";
        const char *key = "\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xAA\xBB\xCC\xDD\xEE\xFF\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F";
        const char *cipher = "\x28\xC9\xF4\x04\xC4\xB8\x10\xF4\xCB\xCC\xB3\x5C\xFB\x87\xF8\x26\x3F\x57\x86\xE2\xD8\x0E\xD3\x26\xCB\xC7\xF0\xE7\x1A\x99\xF4\x3B\xFB\x98\x8B\x9B\x7A\x02\xDD\x21";
        [self tryWithKek:kek kek_len:32 key:key key_len:32 cipher:cipher cipher_len:40];
}

#if 0
- (void)testKWP160_192 {
        // The first example wraps 20 octets of key data with a 192-bit KEK.
        const char *skek = "5840df6e29b02af1 ab493b705bf16ea1 ae8338f4dcc176a8";
        const char *skey = "c37b7e6492584340 bed1220780894115 5068f738";
        const char *scipher = "138bdeaa9b8fa7fc 61f97742e72248ee 5ae6ae5360d1ae6a 5f54f373fa543b6a";
        cc_require(0 == test_one_set(skek, skey, scipher), out);
}

- (void)testKWP56_192 {
        // The second example wraps 7 octets of key data with a 192-bit KEK.
        const char *skek = "5840df6e29b02af1 ab493b705bf16ea1 ae8338f4dcc176a8";
        const char *skey = "466f7250617369";
        const char *scipher = "afbeb0f07dfbf541 9200f2ccb50bb24f";
        cc_require(0 == test_one_set(skek, skey, scipher), out);
}
#endif
@end
