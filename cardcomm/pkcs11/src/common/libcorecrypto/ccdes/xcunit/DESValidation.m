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


#import "DESValidation.h"
#import <corecrypto/ccmode.h>
#import <corecrypto/ccdes.h>
#import <corecrypto/ccmode_test.h>
#import <corecrypto/ccmode_factory.h>

/* Test vectors */
static const struct ccmode_ecb_vector des_ecb_vectors[] = {
    {
        8,
        "\x10\x31\x6E\x02\x8C\x8F\x3B\x4A",
        1,
        "\x00\x00\x00\x00\x00\x00\x00\x00",
        "\x82\xDC\xBA\xFB\xDE\xAB\x66\x02"
    },
    {
        8,
        "\x01\x01\x01\x01\x01\x01\x01\x01",
        1,
        "\x95\xF8\xA5\xE5\xDD\x31\xD9\x00",
        "\x80\x00\x00\x00\x00\x00\x00\x00"
    },
    {
        8,
        "\x01\x01\x01\x01\x01\x01\x01\x01",
        1,
        "\xDD\x7F\x12\x1C\xA5\x01\x56\x19",
        "\x40\x00\x00\x00\x00\x00\x00\x00"
    },
    {
        8,
        "\x01\x01\x01\x01\x01\x01\x01\x01",
        1,
        "\x2E\x86\x53\x10\x4F\x38\x34\xEA",
        "\x20\x00\x00\x00\x00\x00\x00\x00"
    },
    {
        8,
        "\x01\x01\x01\x01\x01\x01\x01\x01",
        1,
        "\x4B\xD3\x88\xFF\x6C\xD8\x1D\x4F",
        "\x10\x00\x00\x00\x00\x00\x00\x00"
    },
    {
        8,
        "\x01\x01\x01\x01\x01\x01\x01\x01",
        1,
        "\x20\xB9\xE7\x67\xB2\xFB\x14\x56",
        "\x08\x00\x00\x00\x00\x00\x00\x00"
    },
    {
        8,
        "\x01\x01\x01\x01\x01\x01\x01\x01",
        1,
        "\x55\x57\x93\x80\xD7\x71\x38\xEF",
        "\x04\x00\x00\x00\x00\x00\x00\x00"
    },
    {
        8,
        "\x01\x01\x01\x01\x01\x01\x01\x01",
        1,
        "\x6C\xC5\xDE\xFA\xAF\x04\x51\x2F",
        "\x02\x00\x00\x00\x00\x00\x00\x00"
    },
    {
        8,
        "\x01\x01\x01\x01\x01\x01\x01\x01",
        1,
        "\x0D\x9F\x27\x9B\xA5\xD8\x72\x60",
        "\x01\x00\x00\x00\x00\x00\x00\x00"
    },
    {
        8,
        "\x01\x01\x01\x01\x01\x01\x01\x01",
        1,
        "\xD9\x03\x1B\x02\x71\xBD\x5A\x0A",
        "\x00\x80\x00\x00\x00\x00\x00\x00"
    },
    {
        8,
        "\x00\x00\x00\x00\x00\x00\x00\x00",
        1,
        "\x00\x00\x00\x00\x00\x00\x00\x00",
        "\x8C\xA6\x4D\xE9\xC1\xB1\x23\xA7"
    },
    {
        8,
        "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
        1,
        "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
        "\x73\x59\xB2\x16\x3E\x4E\xDC\x58"
    },
    {
        8,
        "\x30\x00\x00\x00\x00\x00\x00\x00",
        1,
        "\x10\x00\x00\x00\x00\x00\x00\x01",
        "\x95\x8E\x6E\x62\x7A\x05\x55\x7B"
    },
    {
        8,
        "\x11\x11\x11\x11\x11\x11\x11\x11",
        1,
        "\x11\x11\x11\x11\x11\x11\x11\x11",
        "\xF4\x03\x79\xAB\x9E\x0E\xC5\x33"
    },
    {
        8,
        "\x01\x23\x45\x67\x89\xAB\xCD\xEF",
        1,
        "\x11\x11\x11\x11\x11\x11\x11\x11",
        "\x17\x66\x8D\xFC\x72\x92\x53\x2D"
    },
    {
        8,
        "\x11\x11\x11\x11\x11\x11\x11\x11",
        1,
        "\x01\x23\x45\x67\x89\xAB\xCD\xEF",
        "\x8A\x5A\xE1\xF8\x1A\xB8\xF2\xDD"
    },
    {
        8,
        "\x00\x00\x00\x00\x00\x00\x00\x00",
        1,
        "\x00\x00\x00\x00\x00\x00\x00\x00",
        "\x8C\xA6\x4D\xE9\xC1\xB1\x23\xA7"
    },
    {
        8,
        "\xFE\xDC\xBA\x98\x76\x54\x32\x10",
        1,
        "\x01\x23\x45\x67\x89\xAB\xCD\xEF",
        "\xED\x39\xD9\x50\xFA\x74\xBC\xC4"
    },
    {
        8,
        "\x7C\xA1\x10\x45\x4A\x1A\x6E\x57",
        1,
        "\x01\xA1\xD6\xD0\x39\x77\x67\x42",
        "\x69\x0F\x5B\x0D\x9A\x26\x93\x9B"
    },
    {
        8,
        "\x01\x31\xD9\x61\x9D\xC1\x37\x6E",
        1,
        "\x5C\xD5\x4C\xA8\x3D\xEF\x57\xDA",
        "\x7A\x38\x9D\x10\x35\x4B\xD2\x71"
    },
    {
        8,
        "\x07\xA1\x13\x3E\x4A\x0B\x26\x86",
        1,
        "\x02\x48\xD4\x38\x06\xF6\x71\x72",
        "\x86\x8E\xBB\x51\xCA\xB4\x59\x9A"
    },
    {
        8,
        "\x38\x49\x67\x4C\x26\x02\x31\x9E",
        1,
        "\x51\x45\x4B\x58\x2D\xDF\x44\x0A",
        "\x71\x78\x87\x6E\x01\xF1\x9B\x2A"
    },
    {
        8,
        "\x04\xB9\x15\xBA\x43\xFE\xB5\xB6",
        1,
        "\x42\xFD\x44\x30\x59\x57\x7F\xA2",
        "\xAF\x37\xFB\x42\x1F\x8C\x40\x95"
    },
    {
        8,
        "\x01\x13\xB9\x70\xFD\x34\xF2\xCE",
        1,
        "\x05\x9B\x5E\x08\x51\xCF\x14\x3A",
        "\x86\xA5\x60\xF1\x0E\xC6\xD8\x5B"
    },
    {
        8,
        "\x01\x70\xF1\x75\x46\x8F\xB5\xE6",
        1,
        "\x07\x56\xD8\xE0\x77\x47\x61\xD2",
        "\x0C\xD3\xDA\x02\x00\x21\xDC\x09"
    },
    {
        8,
        "\x43\x29\x7F\xAD\x38\xE3\x73\xFE",
        1,
        "\x76\x25\x14\xB8\x29\xBF\x48\x6A",
        "\xEA\x67\x6B\x2C\xB7\xDB\x2B\x7A"
    },
    {
        8,
        "\x07\xA7\x13\x70\x45\xDA\x2A\x16",
        1,
        "\x3B\xDD\x11\x90\x49\x37\x28\x02",
        "\xDF\xD6\x4A\x81\x5C\xAF\x1A\x0F"
    },
    {
        8,
        "\x04\x68\x91\x04\xC2\xFD\x3B\x2F",
        1,
        "\x26\x95\x5F\x68\x35\xAF\x60\x9A",
        "\x5C\x51\x3C\x9C\x48\x86\xC0\x88"
    },
    {
        8,
        "\x37\xD0\x6B\xB5\x16\xCB\x75\x46",
        1,
        "\x16\x4D\x5E\x40\x4F\x27\x52\x32",
        "\x0A\x2A\xEE\xAE\x3F\xF4\xAB\x77"
    },
    {
        8,
        "\x1F\x08\x26\x0D\x1A\xC2\x46\x5E",
        1,
        "\x6B\x05\x6E\x18\x75\x9F\x5C\xCA",
        "\xEF\x1B\xF0\x3E\x5D\xFA\x57\x5A"
    },
    {
        8,
        "\x58\x40\x23\x64\x1A\xBA\x61\x76",
        1,
        "\x00\x4B\xD6\xEF\x09\x17\x60\x62",
        "\x88\xBF\x0D\xB6\xD7\x0D\xEE\x56"
    },
    {
        8,
        "\x02\x58\x16\x16\x46\x29\xB0\x07",
        1,
        "\x48\x0D\x39\x00\x6E\xE7\x62\xF2",
        "\xA1\xF9\x91\x55\x41\x02\x0B\x56"
    },
    {
        8,
        "\x49\x79\x3E\xBC\x79\xB3\x25\x8F",
        1,
        "\x43\x75\x40\xC8\x69\x8F\x3C\xFA",
        "\x6F\xBF\x1C\xAF\xCF\xFD\x05\x56"
    },
    {
        8,
        "\x4F\xB0\x5E\x15\x15\xAB\x73\xA7",
        1,
        "\x07\x2D\x43\xA0\x77\x07\x52\x92",
        "\x2F\x22\xE4\x9B\xAB\x7C\xA1\xAC"
    },
    {
        8,
        "\x49\xE9\x5D\x6D\x4C\xA2\x29\xBF",
        1,
        "\x02\xFE\x55\x77\x81\x17\xF1\x2A",
        "\x5A\x6B\x61\x2C\xC2\x6C\xCE\x4A"
    },
    {
        8,
        "\x01\x83\x10\xDC\x40\x9B\x26\xD6",
        1,
        "\x1D\x9D\x5C\x50\x18\xF7\x28\xC2",
        "\x5F\x4C\x03\x8E\xD1\x2B\x2E\x41"
    },
    {
        8,
        "\x1C\x58\x7F\x1C\x13\x92\x4F\xEF",
        1,
        "\x30\x55\x32\x28\x6D\x6F\x29\x5A",
        "\x63\xFA\xC0\xD0\x34\xD9\xF7\x93"
    },
    {
        8,
        "\x01\x01\x01\x01\x01\x01\x01\x01",
        1,
        "\x01\x23\x45\x67\x89\xAB\xCD\xEF",
        "\x61\x7B\x3A\x0C\xE8\xF0\x71\x00"
    },
    {
        8,
        "\x1F\x1F\x1F\x1F\x0E\x0E\x0E\x0E",
        1,
        "\x01\x23\x45\x67\x89\xAB\xCD\xEF",
        "\xDB\x95\x86\x05\xF8\xC8\xC6\x06"
    },
    {
        8,
        "\xE0\xFE\xE0\xFE\xF1\xFE\xF1\xFE",
        1,
        "\x01\x23\x45\x67\x89\xAB\xCD\xEF",
        "\xED\xBF\xD1\xC6\x6C\x29\xCC\xC7"
    },
    {
        8,
        "\x00\x00\x00\x00\x00\x00\x00\x00",
        1,
        "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
        "\x35\x55\x50\xB2\x15\x0E\x24\x51"
    },
    {
        8,
        "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
        1,
        "\x00\x00\x00\x00\x00\x00\x00\x00",
        "\xCA\xAA\xAF\x4D\xEA\xF1\xDB\xAE"
    },
    {
        8,
        "\x01\x23\x45\x67\x89\xAB\xCD\xEF",
        1,
        "\x00\x00\x00\x00\x00\x00\x00\x00",
        "\xD5\xD4\x4F\xF7\x20\x68\x3D\x0D"
    },
    {
        8,
        "\xFE\xDC\xBA\x98\x76\x54\x32\x10",
        1,
        "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
        "\x2A\x2B\xB0\x08\xDF\x97\xC2\xF2"
    },
    {
        8,
        "\x01\x01\x01\x01\x01\x01\x01\x01",
        9,
        "\x95\xF8\xA5\xE5\xDD\x31\xD9\x00\xDD\x7F\x12\x1C\xA5\x01\x56\x19\x2E\x86\x53\x10\x4F\x38\x34\xEA\x4B\xD3\x88\xFF\x6C\xD8\x1D\x4F\x20\xB9\xE7\x67\xB2\xFB\x14\x56\x55\x57\x93\x80\xD7\x71\x38\xEF\x6C\xC5\xDE\xFA\xAF\x04\x51\x2F\x0D\x9F\x27\x9B\xA5\xD8\x72\x60\xD9\x03\x1B\x02\x71\xBD\x5A\x0A",
        "\x80\x00\x00\x00\x00\x00\x00\x00\x40\x00\x00\x00\x00\x00\x00\x00\x20\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00\x00\x00\x00\x00\x08\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x80\x00\x00\x00\x00\x00\x00"
    },
};

static const struct ccmode_cbc_vector tdes_cbc_vectors[] = {
#include "TCBCMMT1.inc"
#include "TCBCMMT2.inc"
#include "TCBCMMT3.inc"
//#include "TCBCMonte1.inc"
//#include "TCBCMonte2.inc"
//#include "TCBCMonte3.inc"
#include "TCBCinvperm.inc"
#include "TCBCpermop.inc"
#include "TCBCsubtab.inc"
#include "TCBCvarkey.inc"
#include "TCBCvartext.inc"
};

@implementation DESValidation

- (void) test3DES
{
    uint8_t keydata[CCDES_KEY_SIZE*3];
    uint8_t x;
    const struct ccmode_ecb *enc=&ccdes3_ltc_ecb_encrypt_mode;
    const struct ccmode_ecb *dec=&ccdes3_ltc_ecb_decrypt_mode;

    for(x=0;x<CCDES_KEY_SIZE*3;x++)
        keydata[x]=x;

    /* No vectors for 3DES, only self tests */
    XCTAssertEqual(0, ccmode_ecb_test_key_self(enc, dec, 2, CCDES_KEY_SIZE*3, keydata, 1000), @"Self Test Key");
}

- (void) test3DES_CBC
{
    unsigned int i,j;

    const struct ccmode_cbc *default_enc=ccdes3_cbc_encrypt_mode();
    const struct ccmode_cbc *default_dec=ccdes3_cbc_decrypt_mode();

    const struct ccmode_cbc factory_enc=CCMODE_FACTORY_CBC_ENCRYPT(&ccdes3_ltc_ecb_encrypt_mode);
    const struct ccmode_cbc factory_dec=CCMODE_FACTORY_CBC_DECRYPT(&ccdes3_ltc_ecb_decrypt_mode);

    struct {
        const NSString *name;
        const struct ccmode_cbc *enc;
        const struct ccmode_cbc *dec;
    } impl[] = {
        { @"default", default_enc, default_dec},
        { @"factory", &factory_enc, &factory_dec},
    };

    for(j=0; j<sizeof(impl)/sizeof(impl[0]); j++)
    {
        const struct ccmode_cbc *enc=impl[j].enc;
        const struct ccmode_cbc *dec=impl[j].dec;
        const NSString *name=impl[j].name;

        for(i=0; i<sizeof(tdes_cbc_vectors)/sizeof(tdes_cbc_vectors[0]); i++)
        {
            const struct ccmode_cbc_vector *v=&tdes_cbc_vectors[i];

            XCTAssertEqual(0, ccmode_cbc_test_one_vector(enc, v, 0), @"Encrypt Vector %d (%@)", i, name);
            XCTAssertEqual(0, ccmode_cbc_test_one_vector(dec, v, 1), @"Decrypt Vector %d (%@)", i, name);

//            XCAssertEquals(0, ccmode_cbc_test_one_vector_chained(enc, v, 0), @"Encrypt Chained Vector %d (%@)", i, name);
//            XCAssertEquals(0, ccmode_cbc_test_one_vector_chained(dec, v, 1), @"Decrypt Chained Vector %d (%@)", i, name);

            /* Self test with 2 blocks */
//            XCAssertEquals(0, ccmode_cbc_test_key_self(enc, dec, 2, v->keylen, v->key, 1000), @"Self Test Key %d (%@)", i, name);

            /* Chaining test with 2 blocks */
//            XCAssertEquals(0, ccmode_cbc_test_chaining_self(enc, dec, 2, v->keylen, v->key, 1000), @"Chaining Test Key %d (%@)", i, name);
        }
    }
}

- (void) testDES
{
    unsigned int i;
    const struct ccmode_ecb *enc=&ccdes_ltc_ecb_encrypt_mode;
    const struct ccmode_ecb *dec=&ccdes_ltc_ecb_decrypt_mode;

    for(i=0; i<sizeof(des_ecb_vectors)/sizeof(des_ecb_vectors[0]); i++)
    {
        const struct ccmode_ecb_vector *v=&des_ecb_vectors[i];

        XCTAssertEqual(0, ccmode_ecb_test_one_vector(enc, v, 0), @"Encrypt Vector %d", i);
        XCTAssertEqual(0, ccmode_ecb_test_one_vector(dec, v, 1), @"Decrypt Vector %d", i);

        /* Self test with 2 blocks */
        XCTAssertEqual(0, ccmode_ecb_test_key_self(enc, dec, 2, CCDES_KEY_SIZE, v->key, 1000), @"Self Test Key %d", i);
        v++;
    }
}


@end
