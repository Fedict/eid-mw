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


#import "AESValidation.h"
#import <corecrypto/ccaes.h>
#import <corecrypto/ccmode_factory.h>
#import <corecrypto/ccmode_test.h>
#import <corecrypto/cc_runtime_config.h>

#if CCAES_MUX
#include <corecrypto/ccsha1.h> /* Used by testAES_CBC_MUX */
#endif

#import "cc_unit.h"

/*
 * AES (Advanced Encryption Standard - FIPS 197) ecb mode test vectors.
 */

static const struct ccmode_ecb_vector aes_ecb_vectors[] = {

    {
        16,
        "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f",
        1,
        "\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff",
        "\x69\xc4\xe0\xd8\x6a\x7b\x04\x30\xd8\xcd\xb7\x80\x70\xb4\xc5\x5a"
    },
    {
        24,
        "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17",
        1,
        "\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff",
        "\xdd\xa9\x7c\xa4\x86\x4c\xdf\xe0\x6e\xaf\x70\xa0\xec\x0d\x71\x91"
    },
    {
        32,
        "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f",
        1,
        "\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff",
        "\x8e\xa2\xb7\xca\x51\x67\x45\xbf\xea\xfc\x49\x90\x4b\x49\x60\x89"
    },

#include "ECBGFSbox128.inc"
#include "ECBGFSbox192.inc"
#include "ECBGFSbox256.inc"
#include "ECBKeySbox128.inc"
#include "ECBKeySbox192.inc"
#include "ECBKeySbox256.inc"
#include "ECBVarKey128.inc"
#include "ECBVarKey192.inc"
#include "ECBVarKey256.inc"
#include "ECBVarTxt128.inc"
#include "ECBVarTxt192.inc"
#include "ECBVarTxt256.inc"
};


/*
 * AES (Advanced Encryption Standard - FIPS 197) cbc mode test vectors.
 */

static const struct ccmode_cbc_vector aes_cbc_vectors[] = {
    {
        16,
        "\xf0\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        1,
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        "\x97\x00\x14\xd6\x34\xe2\xb7\x65\x07\x77\xe8\xe8\x4d\x03\xcc\xd8"
    },
    {
        16,
        "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f",
        NULL, /* This is identical to all zeroes IV */
        1,
        "\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff",
        "\x69\xc4\xe0\xd8\x6a\x7b\x04\x30\xd8\xcd\xb7\x80\x70\xb4\xc5\x5a"
    },
    {
        16,
        "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f",
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        1,
        "\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff",
        "\x69\xc4\xe0\xd8\x6a\x7b\x04\x30\xd8\xcd\xb7\x80\x70\xb4\xc5\x5a"
    },
    {
        16,
        "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f",
        "\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff",
        1,
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        "\x69\xc4\xe0\xd8\x6a\x7b\x04\x30\xd8\xcd\xb7\x80\x70\xb4\xc5\x5a"
    },
    {
        16,
        "\xc2\x86\x69\x6d\x88\x7c\x9a\xa0\x61\x1b\xbb\x3e\x20\x25\xa4\x5a",
        "\x56\x2e\x17\x99\x6d\x09\x3d\x28\xdd\xb3\xba\x69\x5a\x2e\x6f\x58",
        2,
        "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f",
        "\xd2\x96\xcd\x94\xc2\xcc\xcf\x8a\x3a\x86\x30\x28\xb5\xe1\xdc\x0a\x75\x86\x60\x2d\x25\x3c\xff\xf9\x1b\x82\x66\xbe\xa6\xd6\x1a\xb1"
    },
    {
        16,
        "\x56\xe4\x7a\x38\xc5\x59\x89\x74\xbc\x46\x90\x3d\xba\x29\x03\x49",
        "\x8c\xe8\x2e\xef\xbe\xa0\xda\x3c\x44\x69\x9e\xd7\xdb\x51\xb7\xd9",
        4,
        "\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf",
        "\xc3\x0e\x32\xff\xed\xc0\x77\x4e\x6a\xff\x6a\xf0\x86\x9f\x71\xaa\x0f\x3a\xf0\x7a\x9a\x31\xa9\xc6\x84\xdb\x20\x7e\xb0\xef\x8e\x4e\x35\x90\x7a\xa6\x32\xc3\xff\xdf\x86\x8b\xb7\xb2\x9d\x3d\x46\xad\x83\xce\x9f\x9a\x10\x2e\xe9\x9d\x49\xa5\x3e\x87\xf4\xc3\xda\x55"
    },

#include "CBCGFSbox128.inc"
#include "CBCGFSbox192.inc"
#include "CBCGFSbox256.inc"
#include "CBCKeySbox128.inc"
#include "CBCKeySbox192.inc"
#include "CBCKeySbox256.inc"
#include "CBCVarKey128.inc"
#include "CBCVarKey192.inc"
#include "CBCVarKey256.inc"
#include "CBCVarTxt128.inc"
#include "CBCVarTxt192.inc"
#include "CBCVarTxt256.inc"

};

/*
 * AES (Advanced Encryption Standard - FIPS 197) cbc mode test vectors.
 */

static const struct ccmode_xts_vector aes_xts_vectors[] = {
    {
        16,
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        32,
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        "\x91\x7c\xf6\x9e\xbd\x68\xb2\xec\x9b\x9f\xe9\xa3\xea\xdd\xa6\x92\xcd\x43\xd2\xf5\x95\x98\xed\x85\x8c\x02\xc2\x65\x2f\xbf\x92\x2e"
    },
    {
        16,
        "\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11",
        "\x22\x22\x22\x22\x22\x22\x22\x22\x22\x22\x22\x22\x22\x22\x22\x22",
        "\x33\x33\x33\x33\x33\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        32,
        "\x44\x44\x44\x44\x44\x44\x44\x44\x44\x44\x44\x44\x44\x44\x44\x44\x44\x44\x44\x44\x44\x44\x44\x44\x44\x44\x44\x44\x44\x44\x44\x44",
        "\xc4\x54\x18\x5e\x6a\x16\x93\x6e\x39\x33\x40\x38\xac\xef\x83\x8b\xfb\x18\x6f\xff\x74\x80\xad\xc4\x28\x93\x82\xec\xd6\xd3\x94\xf0"
    },
    {
        16,
        "\x46\xe6\xed\x9e\xf4\x2d\xcd\xb3\xc8\x93\x09\x3c\x28\xe1\xfc\x0f",
        "\x91\xf5\xca\xa3\xb6\xe0\xbc\x5a\x14\xe7\x83\x21\x5c\x1d\x5b\x61",
        "\x72\xf3\xb0\x54\xcb\xdc\x2f\x9e\x3c\x5b\xc5\x51\xd4\x4d\xdb\xa0",
        16,
        "\xe3\x77\x8d\x68\xe7\x30\xef\x94\x5b\x4a\xe3\xbc\x5b\x93\x6b\xdd",
        "\x97\x40\x9f\x1f\x71\xae\x45\x21\xcb\x49\xa3\x29\x73\xde\x4d\x05"
    },
    /* XTS-AES-128 for non multiple of 16 bytes of data. */
    {
        16,
        "\xff\xfe\xfd\xfc\xfb\xfa\xf9\xf8\xf7\xf6\xf5\xf4\xf3\xf2\xf1\xf0",
        "\xbf\xbe\xbd\xbc\xbb\xba\xb9\xb8\xb7\xb6\xb5\xb4\xb3\xb2\xb1\xb0",
        "\x9a\x78\x56\x34\x12\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        17,
        "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10",
        "\x6c\x16\x25\xdb\x46\x71\x52\x2d\x3d\x75\x99\x60\x1d\xe7\xca\x09\xed"
    },
    {
        16,
        "\xff\xfe\xfd\xfc\xfb\xfa\xf9\xf8\xf7\xf6\xf5\xf4\xf3\xf2\xf1\xf0",
        "\xbf\xbe\xbd\xbc\xbb\xba\xb9\xb8\xb7\xb6\xb5\xb4\xb3\xb2\xb1\xb0",
        "\x9a\x78\x56\x34\x12\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        18,
        "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11",
        "\xd0\x69\x44\x4b\x7a\x7e\x0c\xab\x09\xe2\x44\x47\xd2\x4d\xeb\x1f\xed\xbf",
    },
    {
        16,
        "\xff\xfe\xfd\xfc\xfb\xfa\xf9\xf8\xf7\xf6\xf5\xf4\xf3\xf2\xf1\xf0",
        "\xbf\xbe\xbd\xbc\xbb\xba\xb9\xb8\xb7\xb6\xb5\xb4\xb3\xb2\xb1\xb0",
        "\x9a\x78\x56\x34\x12\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        19,
        "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12",
        "\xe5\xdf\x13\x51\xc0\x54\x4b\xa1\x35\x0b\x33\x63\xcd\x8e\xf4\xbe\xed\xbf\x9d",
    },
    {
        16,
        "\xff\xfe\xfd\xfc\xfb\xfa\xf9\xf8\xf7\xf6\xf5\xf4\xf3\xf2\xf1\xf0",
        "\xbf\xbe\xbd\xbc\xbb\xba\xb9\xb8\xb7\xb6\xb5\xb4\xb3\xb2\xb1\xb0",
        "\x9a\x78\x56\x34\x12\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        20,
        "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13",
        "\x9d\x84\xc8\x13\xf7\x19\xaa\x2c\x7b\xe3\xf6\x61\x71\xc7\xc5\xc2\xed\xbf\x9d\xac",
    },
    /* XTS-AES-128 for 128 bytes of data. */
    {
        16,
        "\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef",
        "\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf",
        "\x21\x43\x65\x87\xa9\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        128,
        "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b\x7c\x7d\x7e\x7f\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b\x7c\x7d\x7e\x7f\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff",
        "\x38\xb4\x58\x12\xef\x43\xa0\x5b\xd9\x57\xe5\x45\x90\x7e\x22\x3b\x95\x4a\xb4\xaa\xf0\x88\x30\x3a\xd9\x10\xea\xdf\x14\xb4\x2b\xe6\x8b\x24\x61\x14\x9d\x8c\x8b\xa8\x5f\x99\x2b\xe9\x70\xbc\x62\x1f\x1b\x06\x57\x3f\x63\xe8\x67\xbf\x58\x75\xac\xaf\xa0\x4e\x42\xcc\xbd\x7b\xd3\xc2\xa0\xfb\x1f\xff\x79\x1e\xc5\xec\x36\xc6\x6a\xe4\xac\x1e\x80\x6d\x81\xfb\xf7\x09\xdb\xe2\x9e\x47\x1f\xad\x38\x54\x9c\x8e\x66\xf5\x34\x5d\x7c\x1e\xb9\x4f\x40\x5d\x1e\xc7\x85\xcc\x6f\x6a\x68\xf6\x25\x4d\xd8\x33\x9f\x9d\x84\x05\x7e\x01\xa1\x77\x41\x99\x04\x82\x99\x95\x16\xb5\x61\x1a\x38\xf4\x1b\xb6\x47\x8e\x6f\x17\x3f\x32\x08\x05\xdd\x71\xb1\x93\x2f\xc3\x33\xcb\x9e\xe3\x99\x36\xbe\xea\x9a\xd9\x6f\xa1\x0f\xb4\x11\x2b\x90\x17\x34\xdd\xad\x40\xbc\x18\x78\x99\x5f\x8e\x11\xae\xe7\xd1\x41\xa2\xf5\xd4\x8b\x7a\x4e\x1e\x7f\x0b\x2c\x04\x83\x0e\x69\xa4\xfd\x13\x78\x41\x1c\x2f\x28\x7e\xdf\x48\xc6\xc4\xe5\xc2\x47\xa1\x96\x80\xf7\xfe\x41\xce\xfb\xd4\x9b\x58\x21\x06\xe3\x61\x6c\xbb\xe4\xdf\xb2\x34\x4b\x2a\xe9\x51\x93\x91\xf3\xe0\xfb\x49\x22\x25\x4b\x1d\x6d\x2d\x19\xc6\xd4\xd5\x37\xb3\xa2\x6f\x3b\xcc\x51\x58\x8b\x32\xf3\xec\xa0\x82\x9b\x6a\x5a\xc7\x25\x78\xfb\x81\x4f\xb4\x3c\xf8\x0d\x64\xa2\x33\xe3\xf9\x97\xa3\xf0\x26\x83\x34\x2f\x2b\x33\xd2\x5b\x49\x25\x36\xb9\x3b\xec\xb2\xf5\xe1\xa8\xb8\x2f\x5b\x88\x33\x42\x72\x9e\x8a\xe0\x9d\x16\x93\x88\x41\xa2\x1a\x97\xfb\x54\x3e\xea\x3b\xbf\xf5\x9f\x13\xc1\xa1\x84\x49\xe3\x98\x70\x1c\x1a\xd5\x16\x48\x34\x6c\xbc\x04\xc2\x7b\xb2\xda\x3b\x93\xa1\x37\x2c\xca\xe5\x48\xfb\x53\xbe\xe4\x76\xf9\xe9\xc9\x17\x73\xb1\xbb\x19\x82\x83\x94\xd5\x5d\x3e\x1a\x20\xed\x69\x11\x3a\x86\x0b\x68\x29\xff\xa8\x47\x22\x46\x04\x43\x50\x70\x22\x1b\x25\x7e\x8d\xff\x78\x36\x15\xd2\xca\xe4\x80\x3a\x93\xaa\x43\x34\xab\x48\x2a\x0a\xfa\xc9\xc0\xae\xda\x70\xb4\x5a\x48\x1d\xf5\xde\xc5\xdf\x8c\xc0\xf4\x23\xc7\x7a\x5f\xd4\x6c\xd3\x12\x02\x1d\x4b\x43\x88\x62\x41\x9a\x79\x1b\xe0\x3b\xb4\xd9\x7c\x0e\x59\x57\x85\x42\x53\x1b\xa4\x66\xa8\x3b\xaf\x92\xce\xfc\x15\x1b\x5c\xc1\x61\x1a\x16\x78\x93\x81\x9b\x63\xfb\x8a\x6b\x18\xe8\x6d\xe6\x02\x90\xfa\x72\xb7\x97\xb0\xce\x59\xf3"
    },

#include "XTSGenAES128.inc"
#include "XTSGenAES256.inc"

};

static const struct ccmode_ofb_vector aes_ofb_vectors[] = {
#include "OFBGFSbox128.inc"
#include "OFBGFSbox192.inc"
#include "OFBGFSbox256.inc"
#include "OFBKeySbox128.inc"
#include "OFBKeySbox192.inc"
#include "OFBKeySbox256.inc"
#include "OFBVarKey128.inc"
#include "OFBVarKey192.inc"
#include "OFBVarKey256.inc"
#include "OFBVarTxt128.inc"
#include "OFBVarTxt192.inc"
#include "OFBVarTxt256.inc"
};

static const struct ccmode_cfb_vector aes_cfb_vectors[] = {
#include "CFB128GFSbox128.inc"
#include "CFB128GFSbox192.inc"
#include "CFB128GFSbox256.inc"
#include "CFB128KeySbox128.inc"
#include "CFB128KeySbox192.inc"
#include "CFB128KeySbox256.inc"
#include "CFB128VarKey128.inc"
#include "CFB128VarKey192.inc"
#include "CFB128VarKey256.inc"
#include "CFB128VarTxt128.inc"
#include "CFB128VarTxt192.inc"
#include "CFB128VarTxt256.inc"
};

static const struct ccmode_cfb8_vector aes_cfb8_vectors[] = {
#include "CFB8GFSbox128.inc"
#include "CFB8GFSbox192.inc"
#include "CFB8GFSbox256.inc"
#include "CFB8KeySbox128.inc"
#include "CFB8KeySbox192.inc"
#include "CFB8KeySbox256.inc"
#include "CFB8VarKey128.inc"
#include "CFB8VarKey192.inc"
#include "CFB8VarKey256.inc"
#include "CFB8VarTxt128.inc"
#include "CFB8VarTxt192.inc"
#include "CFB8VarTxt256.inc"
};


static const struct ccmode_ctr_vector aes_ctr_vectors[] = {
    /* Vectors from RFC 3686 */
    {    /* Test Vector #1: Encrypting 16 octets using AES-CTR with 128-bit key */
        16,
        "\xAE\x68\x52\xF8\x12\x10\x67\xCC\x4B\xF7\xA5\x76\x55\x77\xF3\x9E",
        "\x00\x00\x00\x30\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01",
        16,
        "Single block msg",
        "\xE4\x09\x5D\x4F\xB7\xA7\xB3\x79\x2D\x61\x75\xA3\x26\x13\x11\xB8",
    },
    {   /*     Test Vector #2: Encrypting 32 octets using AES-CTR with 128-bit key */
        16,
        "\x7E\x24\x06\x78\x17\xFA\xE0\xD7\x43\xD6\xCE\x1F\x32\x53\x91\x63",
        "\x00\x6C\xB6\xDB\xC0\x54\x3B\x59\xDA\x48\xD9\x0B\x00\x00\x00\x01",
        32,
        "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F",
        "\x51\x04\xA1\x06\x16\x8A\x72\xD9\x79\x0D\x41\xEE\x8E\xDA\xD3\x88\xEB\x2E\x1E\xFC\x46\xDA\x57\xC8\xFC\xE6\x30\xDF\x91\x41\xBE\x28",
    },
    {   /*     Test Vector #3: Encrypting 36 octets using AES-CTR with 128-bit key */
        16,
        "\x76\x91\xBE\x03\x5E\x50\x20\xA8\xAC\x6E\x61\x85\x29\xF9\xA0\xDC",
        "\x00\xE0\x01\x7B\x27\x77\x7F\x3F\x4A\x17\x86\xF0\x00\x00\x00\x01",
        36,
        "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F\x20\x21\x22\x23",
        "\xC1\xCF\x48\xA8\x9F\x2F\xFD\xD9\xCF\x46\x52\xE9\xEF\xDB\x72\xD7\x45\x40\xA4\x2B\xDE\x6D\x78\x36\xD5\x9A\x5C\xEA\xAE\xF3\x10\x53\x25\xB2\x07\x2F",
    },
    {   /*     Test Vector #4: Encrypting 16 octets using AES-CTR with 192-bit key */
        24,
        "\x16\xAF\x5B\x14\x5F\xC9\xF5\x79\xC1\x75\xF9\x3E\x3B\xFB\x0E\xED\x86\x3D\x06\xCC\xFD\xB7\x85\x15",
        "\x00\x00\x00\x48\x36\x73\x3C\x14\x7D\x6D\x93\xCB\x00\x00\x00\x01",
        16,
        "Single block msg",
        "\x4B\x55\x38\x4F\xE2\x59\xC9\xC8\x4E\x79\x35\xA0\x03\xCB\xE9\x28",
    },
    {  /*     Test Vector #5: Encrypting 32 octets using AES-CTR with 192-bit key */
        24,
        "\x7C\x5C\xB2\x40\x1B\x3D\xC3\x3C\x19\xE7\x34\x08\x19\xE0\xF6\x9C\x67\x8C\x3D\xB8\xE6\xF6\xA9\x1A",
        "\x00\x96\xB0\x3B\x02\x0C\x6E\xAD\xC2\xCB\x50\x0D\x00\x00\x00\x01",
        32,
        "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F",
        "\x45\x32\x43\xFC\x60\x9B\x23\x32\x7E\xDF\xAA\xFA\x71\x31\xCD\x9F\x84\x90\x70\x1C\x5A\xD4\xA7\x9C\xFC\x1F\xE0\xFF\x42\xF4\xFB\x00",
    },
    {   /*     Test Vector #6: Encrypting 36 octets using AES-CTR with 192-bit key */
        24,
        "\x02\xBF\x39\x1E\xE8\xEC\xB1\x59\xB9\x59\x61\x7B\x09\x65\x27\x9B\xF5\x9B\x60\xA7\x86\xD3\xE0\xFE",
        "\x00\x07\xBD\xFD\x5C\xBD\x60\x27\x8D\xCC\x09\x12\x00\x00\x00\x01",
        36,
        "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F\x20\x21\x22\x23",
        "\x96\x89\x3F\xC5\x5E\x5C\x72\x2F\x54\x0B\x7D\xD1\xDD\xF7\xE7\x58\xD2\x88\xBC\x95\xC6\x91\x65\x88\x45\x36\xC8\x11\x66\x2F\x21\x88\xAB\xEE\x09\x35",
    },
    {   /*     Test Vector #7: Encrypting 16 octets using AES-CTR with 256-bit key */
        32,
        "\x77\x6B\xEF\xF2\x85\x1D\xB0\x6F\x4C\x8A\x05\x42\xC8\x69\x6F\x6C\x6A\x81\xAF\x1E\xEC\x96\xB4\xD3\x7F\xC1\xD6\x89\xE6\xC1\xC1\x04",
        "\x00\x00\x00\x60\xDB\x56\x72\xC9\x7A\xA8\xF0\xB2\x00\x00\x00\x01",
        16,
        "Single block msg",
        "\x14\x5A\xD0\x1D\xBF\x82\x4E\xC7\x56\x08\x63\xDC\x71\xE3\xE0\xC0",
    },
    {   /*     Test Vector #8: Encrypting 32 octets using AES-CTR with 256-bit key */
        32,
        "\xF6\xD6\x6D\x6B\xD5\x2D\x59\xBB\x07\x96\x36\x58\x79\xEF\xF8\x86\xC6\x6D\xD5\x1A\x5B\x6A\x99\x74\x4B\x50\x59\x0C\x87\xA2\x38\x84",
        "\x00\xFA\xAC\x24\xC1\x58\x5E\xF1\x5A\x43\xD8\x75\x00\x00\x00\x01",
        32,
        "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F",
        "\xF0\x5E\x23\x1B\x38\x94\x61\x2C\x49\xEE\x00\x0B\x80\x4E\xB2\xA9\xB8\x30\x6B\x50\x8F\x83\x9D\x6A\x55\x30\x83\x1D\x93\x44\xAF\x1C",
    },
    {   /*     Test Vector #9: Encrypting 36 octets using AES-CTR with 256-bit key */
        32,
        "\xFF\x7A\x61\x7C\xE6\x91\x48\xE4\xF1\x72\x6E\x2F\x43\x58\x1D\xE2\xAA\x62\xD9\xF8\x05\x53\x2E\xDF\xF1\xEE\xD6\x87\xFB\x54\x15\x3D",
        "\x00\x1C\xC5\xB7\x51\xA5\x1D\x70\xA1\xC1\x11\x48\x00\x00\x00\x01",
        36,
        "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F\x20\x21\x22\x23",
        "\xEB\x6C\x52\x82\x1D\x0B\xBB\xF7\xCE\x75\x94\x46\x2A\xCA\x4F\xAA\xB4\x07\xDF\x86\x65\x69\xFD\x07\xF4\x8C\xC0\xB5\x83\xD6\x07\x1F\x1E\xC0\xE6\xB8",
    },
};

static const struct ccmode_gcm_vector aes_gcm_vectors[] = {
#include "gcmEncryptExtIV128.inc"
#include "gcmEncryptExtIV192.inc"
#include "gcmEncryptExtIV256.inc"
};

static const struct ccmode_ccm_vector aes_ccm_vectors[] = {
#include "ccmVADT128rsp.inc"
#include "ccmVADT192rsp.inc"
#include "ccmVADT256rsp.inc"
};


@implementation AESValidation

- (void) cbc: (const NSString *)name encrypt: (const struct ccmode_cbc *) enc
     decrypt: (const struct ccmode_cbc *)dec
{
    unsigned int numVectors = (unsigned int)(sizeof(aes_cbc_vectors)/sizeof(aes_cbc_vectors[0]));
    for(unsigned int i=0; i<numVectors; i++)
    {
        const struct ccmode_cbc_vector *v=&aes_cbc_vectors[i];

        XCTAssertEqual(0, ccmode_cbc_test_one_vector(enc, v, 0), @"Encrypt Vector %d (%@)", i, name);
        XCTAssertEqual(0, ccmode_cbc_test_one_vector(dec, v, 1), @"Decrypt Vector %d (%@)", i, name);

        XCTAssertEqual(0, ccmode_cbc_test_one_vector_chained(enc, v, 0), @"Encrypt Chained Vector %d (%@)", i, name);
        XCTAssertEqual(0, ccmode_cbc_test_one_vector_chained(dec, v, 1), @"Decrypt Chained Vector %d (%@)", i, name);

        /* Self test with 2 blocks */
        XCTAssertEqual(0, ccmode_cbc_test_key_self(enc, dec, 2, v->keylen, v->key, 1000), @"Self Test Key %d (%@)", i, name);

        /* Chaining test with 2 blocks */
        XCTAssertEqual(0, ccmode_cbc_test_chaining_self(enc, dec, 2, v->keylen, v->key, 1000), @"Chaining Test Key %d (%@)", i, name);
    }
}

- (void) testAES_CBC
{
    [self cbc: @"default" encrypt: ccaes_cbc_encrypt_mode()
      decrypt: ccaes_cbc_decrypt_mode()];
}

- (void) testAES_CBC_Factory
{
    const struct ccmode_cbc factory_enc = CCMODE_FACTORY_CBC_ENCRYPT(ccaes_ecb_encrypt_mode());
    const struct ccmode_cbc factory_dec = CCMODE_FACTORY_CBC_DECRYPT(ccaes_ecb_decrypt_mode());
    [self cbc: @"factory" encrypt: &factory_enc decrypt: &factory_dec];
}

- (void) testAES_CBC_Gladman
{
    [self cbc: @"gladman" encrypt: &ccaes_gladman_cbc_encrypt_mode
      decrypt: &ccaes_gladman_cbc_decrypt_mode];
}

#if !defined(__NO_ASM__) && CCAES_ARM
- (void) testAES_CBC_ARM
{
    [self cbc: @"arm asm" encrypt: &ccaes_arm_cbc_encrypt_mode
      decrypt: &ccaes_arm_cbc_decrypt_mode];
}

#if CCAES_MUX
- (void) testAES_CBC_ARM_MUX
{
    [self cbc: @"ios mux" encrypt: ccaes_ios_mux_cbc_encrypt_mode()
      decrypt: ccaes_ios_mux_cbc_decrypt_mode()];
}

- (void) testAES_CBC_MUX
{
    size_t i;
    static const size_t testmax = 1024*1024*8; // 8 meg
    
    uint8_t rawkey[16];
    uint8_t iv[16];

    const struct ccmode_cbc stock_enc = CCMODE_FACTORY_CBC_ENCRYPT(ccaes_ecb_encrypt_mode());
    const struct ccmode_cbc *mux_enc = ccaes_ios_mux_cbc_encrypt_mode();
    const struct ccmode_cbc *mux_dec = ccaes_ios_mux_cbc_decrypt_mode();
    const struct ccdigest_info *sha1_di = ccsha1_di();

    uint8_t *plaintext = malloc(testmax);
    uint8_t *ciphertext = malloc(testmax);
    uint8_t *decryptedtext = malloc(testmax);
    uint8_t stock_hash[sha1_di->output_size];
    uint8_t mux_hash[sha1_di->output_size];

    size_t testsize = sizeof(plaintext);
    for(i=0; i<testsize; i++) plaintext[i] = i%256;
    for(i=0; i<16; i++) { rawkey[i] = i; iv[i] = 0; }

    while(testsize/16) {
        size_t blocksize = testsize / 16;
        cccbc_one_shot(&stock_enc, 16, rawkey, iv, blocksize, plaintext, ciphertext);
        ccdigest(sha1_di, testsize, ciphertext, stock_hash);
        cccbc_one_shot(mux_enc, 16, rawkey, iv, blocksize, plaintext, ciphertext);
        ccdigest(sha1_di, testsize, ciphertext, mux_hash);
        XCAssertMemEquals(sha1_di->output_size, stock_hash, mux_hash, @"Encrypted bytes are equal for size(%lu)", i);
        ccdigest(sha1_di, testsize, plaintext, stock_hash);
        cccbc_one_shot(mux_dec, 16, rawkey, iv, blocksize, ciphertext, decryptedtext);
        ccdigest(sha1_di, testsize, decryptedtext, mux_hash);
        XCAssertMemEquals(sha1_di->output_size, stock_hash, mux_hash, @"Decrypted bytes are equal for size(%lu)", i);
        testsize >>= 2;
    }
}
#endif /* CCAES_MUX */

#endif /* CCAES_ARM */

#if CCAES_INTEL
- (void) testAES_CBC_Intel_Opt_ASM
{
    [self cbc: @"intel opt asm" encrypt: &ccaes_intel_cbc_encrypt_opt_mode
      decrypt: &ccaes_intel_cbc_decrypt_opt_mode];
}

- (void) testAES_CBC_Intel_AES_NI
{
    if (CC_HAS_AESNI())
	{
        [self cbc: @"intel AESNI" encrypt: &ccaes_intel_cbc_encrypt_aesni_mode
          decrypt: &ccaes_intel_cbc_decrypt_aesni_mode];
    }
}
#endif /* CCAES_INTEL */


- (void) ofb: (const NSString *)name crypt: (const struct ccmode_ofb*) enc
{
    unsigned int numVectors = (unsigned int)(sizeof(aes_ofb_vectors)/sizeof(aes_ofb_vectors[0]));
    for(unsigned int i=0; i<numVectors; i++)
    {
        const struct ccmode_ofb_vector *v=&aes_ofb_vectors[i];

        XCTAssertEqual(0, ccmode_ofb_test_one_vector(enc, v, 0), @"Encrypt Vector %d (%@)", i, name);
        XCTAssertEqual(0, ccmode_ofb_test_one_vector(enc, v, 1), @"Decrypt Vector %d (%@)", i, name);

        XCTAssertEqual(0, ccmode_ofb_test_one_vector_chained(enc, v, 0), @"Encrypt Chained Vector %d (%@)", i, name);
        XCTAssertEqual(0, ccmode_ofb_test_one_vector_chained(enc, v, 1), @"Decrypt Chained Vector %d (%@)", i, name);
    }
}

- (void) testAES_OFB
{
    [self ofb: @"default" crypt: ccaes_ofb_crypt_mode()];
}

- (void) testAES_OFB_Factory
{
    const struct ccmode_ofb factory_enc = CCMODE_FACTORY_OFB_CRYPT(ccaes_ecb_encrypt_mode());
    [self ofb: @"factory" crypt: &factory_enc];
}

- (void) cfb: (const NSString *)name encrypt: (const struct ccmode_cfb*) enc
     decrypt: (const struct ccmode_cfb *)dec
{
    unsigned int numVectors = (unsigned int)(sizeof(aes_cfb_vectors)/sizeof(aes_cfb_vectors[0]));
    for(unsigned int i=0; i<numVectors; i++)
    {
        const struct ccmode_cfb_vector *v=&aes_cfb_vectors[i];

        XCTAssertEqual(0, ccmode_cfb_test_one_vector(enc, v, 0), @"Encrypt Vector %d (%@)", i, name);
        XCTAssertEqual(0, ccmode_cfb_test_one_vector(dec, v, 1), @"Decrypt Vector %d (%@)", i, name);

        XCTAssertEqual(0, ccmode_cfb_test_one_vector_chained(enc, v, 0), @"Encrypt Chained Vector %d (%@)", i, name);
        XCTAssertEqual(0, ccmode_cfb_test_one_vector_chained(dec, v, 1), @"Decrypt Chained Vector %d (%@)", i, name);
    }
}

- (void) testAES_CFB
{
    [self cfb: @"default" encrypt: ccaes_cfb_encrypt_mode() decrypt: ccaes_cfb_decrypt_mode()];
}

- (void) testAES_CFB_Factory
{
    const struct ccmode_cfb factory_enc = CCMODE_FACTORY_CFB_ENCRYPT(ccaes_ecb_encrypt_mode());
    const struct ccmode_cfb factory_dec = CCMODE_FACTORY_CFB_DECRYPT(ccaes_ecb_encrypt_mode());
    [self cfb: @"factory" encrypt: &factory_enc decrypt: &factory_dec];
}


- (void) cfb8: (const NSString *)name encrypt: (const struct ccmode_cfb8*) enc
     decrypt: (const struct ccmode_cfb8 *)dec
{
    unsigned int numVectors = (unsigned int)(sizeof(aes_cfb8_vectors)/sizeof(aes_cfb8_vectors[0]));
    for(unsigned int i=0; i<numVectors; i++)
    {
        const struct ccmode_cfb8_vector *v=&aes_cfb8_vectors[i];

        XCTAssertEqual(0, ccmode_cfb8_test_one_vector(enc, v, 0), @"Encrypt Vector %d (%@)", i, name);
        XCTAssertEqual(0, ccmode_cfb8_test_one_vector(dec, v, 1), @"Decrypt Vector %d (%@)", i, name);

        XCTAssertEqual(0, ccmode_cfb8_test_one_vector_chained(enc, v, 0), @"Encrypt Chained Vector %d (%@)", i, name);
        XCTAssertEqual(0, ccmode_cfb8_test_one_vector_chained(dec, v, 1), @"Decrypt Chained Vector %d (%@)", i, name);
    }
}

- (void) testAES_CFB8
{
    [self cfb8: @"default" encrypt: ccaes_cfb8_encrypt_mode() decrypt: ccaes_cfb8_decrypt_mode()];
}

- (void) testAES_CFB8_Factory
{
    const struct ccmode_cfb8 factory_enc = CCMODE_FACTORY_CFB8_ENCRYPT(ccaes_ecb_encrypt_mode());
    const struct ccmode_cfb8 factory_dec = CCMODE_FACTORY_CFB8_DECRYPT(ccaes_ecb_encrypt_mode());
    [self cfb8: @"factory" encrypt: &factory_enc decrypt: &factory_dec];
}


- (void) ctr: (const NSString *)name crypt: (const struct ccmode_ctr*) enc
{
    unsigned int numVectors = (unsigned int)(sizeof(aes_ctr_vectors)/sizeof(aes_ctr_vectors[0]));
    for(unsigned int i=0; i<numVectors; i++)
    {
        const struct ccmode_ctr_vector *v=&aes_ctr_vectors[i];

        XCTAssertEqual(0, ccmode_ctr_test_one_vector(enc, v, 0), @"Encrypt Vector %d (%@)", i, name);
        XCTAssertEqual(0, ccmode_ctr_test_one_vector(enc, v, 1), @"Decrypt Vector %d (%@)", i, name);

        XCTAssertEqual(0, ccmode_ctr_test_one_vector_chained(enc, v, 0), @"Encrypt Chained Vector %d (%@)", i, name);
        XCTAssertEqual(0, ccmode_ctr_test_one_vector_chained(enc, v, 1), @"Decrypt Chained Vector %d (%@)", i, name);
    }
}

- (void) testAES_CTR
{
    [self ctr: @"default" crypt: ccaes_ctr_crypt_mode()];
}

- (void) testAES_CTR_Factory
{
    const struct ccmode_ctr factory_enc = CCMODE_FACTORY_CTR_CRYPT(ccaes_ecb_encrypt_mode());
    [self ctr: @"factory" crypt: &factory_enc];
}


- (void) gcm: (const NSString *)name encrypt: (const struct ccmode_gcm*) enc
     decrypt: (const struct ccmode_gcm *)dec
{
    unsigned int numVectors = (unsigned int)(sizeof(aes_gcm_vectors)/sizeof(aes_gcm_vectors[0]));
    for(unsigned int i=0; i<numVectors; i++)
    {
        const struct ccmode_gcm_vector *v=&aes_gcm_vectors[i];

        XCTAssertEqual(0, ccmode_gcm_test_one_vector(enc, v, 0), @"Encrypt Vector %d (%@)", i, name);
        XCTAssertEqual(0, ccmode_gcm_test_one_vector(dec, v, 1), @"Decrypt Vector %d (%@)", i, name);

        XCTAssertEqual(0, ccmode_gcm_test_one_vector_chained(enc, v, 0), @"Encrypt Chained Vector %d (%@)", i, name);
        XCTAssertEqual(0, ccmode_gcm_test_one_vector_chained(dec, v, 1), @"Decrypt Chained Vector %d (%@)", i, name);
    }
}

- (void) testAES_GCM
{
    [self gcm: @"default" encrypt: ccaes_gcm_encrypt_mode() decrypt: ccaes_gcm_decrypt_mode()];
}

- (void) testAES_GCM_Factory
{
    const struct ccmode_gcm factory_enc = CCMODE_FACTORY_GCM_ENCRYPT(ccaes_ecb_encrypt_mode());
    const struct ccmode_gcm factory_dec = CCMODE_FACTORY_GCM_DECRYPT(ccaes_ecb_encrypt_mode());
    [self gcm: @"factory" encrypt: &factory_enc decrypt: &factory_dec];
}


- (void) ecb: (const NSString *)name encrypt: (const struct ccmode_ecb *) enc
     decrypt: (const struct ccmode_ecb *)dec
{
    unsigned int numVectors = (unsigned int)(sizeof(aes_ecb_vectors)/sizeof(aes_ecb_vectors[0]));
    for(unsigned int i=0; i<numVectors; i++)
    {
        const struct ccmode_ecb_vector *v=&aes_ecb_vectors[i];

        XCTAssertEqual(0, ccmode_ecb_test_one_vector(enc, v, 0), @"Encrypt Vector %d (%@)", i, name);
        XCTAssertEqual(0, ccmode_ecb_test_one_vector(dec, v, 1), @"Decrypt Vector %d (%@)", i, name);

        /* Self test with 2 blocks */
        XCTAssertEqual(0, ccmode_ecb_test_key_self(enc, dec, 2, v->keylen, v->key, 1000), @"Self Test Key %d (%@)", i, name);
        /* Self test with 3 blocks */
        XCTAssertEqual(0, ccmode_ecb_test_key_self(enc, dec, 3, v->keylen, v->key, 100), @"Self Test Key %d (%@)", i, name);
        /* Self test with 4 blocks */
        XCTAssertEqual(0, ccmode_ecb_test_key_self(enc, dec, 4, v->keylen, v->key, 100), @"Self Test Key %d (%@)", i, name);
        /* Self test with 10 blocks */
        XCTAssertEqual(0, ccmode_ecb_test_key_self(enc, dec, 10, v->keylen, v->key, 100), @"Self Test Key %d (%@)", i, name);
    }
}

- (void) testAES_ECB
{
    [self ecb: @"default" encrypt: ccaes_ecb_encrypt_mode()
      decrypt: ccaes_ecb_decrypt_mode()];
}

- (void) testAES_ECB_LTC
{
    [self ecb: @"ltc" encrypt: &ccaes_ltc_ecb_encrypt_mode
      decrypt: &ccaes_ltc_ecb_decrypt_mode];
}

#if !defined(__NO_ASM__) && CCAES_ARM
- (void) testAES_ECB_ARM
{
    [self ecb: @"arm asm" encrypt: &ccaes_arm_ecb_encrypt_mode
      decrypt: &ccaes_arm_ecb_decrypt_mode];
}
#endif /* CCAES_ARM */

#if CCAES_INTEL
- (void) testAES_ECB_INTEL_Opt
{
    [self ecb: @"intel opt" encrypt: &ccaes_intel_ecb_encrypt_opt_mode
      decrypt: &ccaes_intel_ecb_decrypt_opt_mode];
}

- (void) testAES_ECB_INTEL_NI
{
    if (CC_HAS_AESNI())
	{
        [self ecb: @"intel AESNI" encrypt: &ccaes_intel_ecb_encrypt_aesni_mode
          decrypt: &ccaes_intel_ecb_decrypt_aesni_mode];
    }
}
#endif /* CCAES_INTEL */


- (void) xts: (const NSString *)name encrypt: (const struct ccmode_xts*) enc
     decrypt: (const struct ccmode_xts *)dec
{
    unsigned int numVectors = (unsigned int)(sizeof(aes_xts_vectors) / sizeof(aes_xts_vectors[0]));
    for (unsigned int i = 0; i < numVectors; ++i)
    {
        const struct ccmode_xts_vector *v = &aes_xts_vectors[i];
        uint8_t temp[v->nbytes];
        
        ccmode_xts_test_one_vector(enc, v, temp, 0);
        XCAssertMemEquals(v->nbytes, temp, v->ct, @"Encrypt Vector %d (%@)", i, name);
        ccmode_xts_test_one_vector(dec, v, temp, 1);
        XCAssertMemEquals(v->nbytes, temp, v->pt, @"Decrypt Vector %d (%@)", i, name);
        
        ccmode_xts_test_one_vector_chained(enc, v, temp, 0);
        XCAssertMemEquals(v->nbytes, temp, v->ct, @"Encrypt Chained Vector %d (%@)", i, name);
        ccmode_xts_test_one_vector_chained(dec, v, temp, 1);
        XCAssertMemEquals(v->nbytes, temp, v->pt, @"Decrypt Chained Vector %d (%@)", i, name);
        
#if 0
        /* Self test with 2 blocks */
        XCAssertEquals(0, ccmode_xts_test_key_self(enc, dec, 2, v->keylen, v->key, 1000), @"Self Test Key %d (%@)", i, name);

        /* Chaining test with 2 blocks */
        XCAssertEquals(0, ccmode_xts_test_chaining_self(enc, dec, 2, v->keylen, v->key, 1000), @"Chaining Test Key %d (%@)", i, name);
#endif
    }
}

- (void) testAES_XTS
{
    [self xts: @"default" encrypt: ccaes_xts_encrypt_mode()
      decrypt: ccaes_xts_decrypt_mode()];
}

- (void) testAES_XTS_Factory
{
    const struct ccmode_xts factory_enc = CCMODE_FACTORY_XTS_ENCRYPT(ccaes_ecb_encrypt_mode(), ccaes_ecb_encrypt_mode());
    const struct ccmode_xts factory_dec = CCMODE_FACTORY_XTS_DECRYPT(ccaes_ecb_decrypt_mode(), ccaes_ecb_encrypt_mode());
    
    [self xts: @"factory" encrypt: &factory_enc decrypt: &factory_dec];
}

#if !defined(__NO_ASM__) && CCAES_ARM
- (void) testAES_XTS_ARM
{
    [self xts: @"arm" encrypt: &ccaes_arm_xts_encrypt_mode
      decrypt: &ccaes_arm_xts_decrypt_mode];
}
#endif

#if CCAES_INTEL
- (void) testAES_XTS_INTEL_Opt
{
    [self xts: @"intel opt" encrypt: &ccaes_intel_xts_encrypt_opt_mode
      decrypt: &ccaes_intel_xts_decrypt_opt_mode];
}

- (void) testAES_XTS_INTEL_NI
{
    if (CC_HAS_AESNI())
	{
        [self xts: @"intel AESNI" encrypt: &ccaes_intel_xts_encrypt_aesni_mode
          decrypt: &ccaes_intel_xts_decrypt_aesni_mode];
    }
}
#endif /* CCAES_INTEL */


- (void) testAES_CCM
{
    [self ccm: @"default" encrypt: ccaes_ccm_encrypt_mode()
      decrypt: ccaes_ccm_decrypt_mode()];
}

- (void) testAES_CCM_Factory
{
    const struct ccmode_ccm generic_enc = CCMODE_FACTORY_CCM_ENCRYPT(ccaes_ecb_encrypt_mode());
    const struct ccmode_ccm generic_dec = CCMODE_FACTORY_CCM_DECRYPT(ccaes_ecb_encrypt_mode());

    [self ccm: @"factory" encrypt: &generic_enc
      decrypt: &generic_dec];
}

- (void) ccm: (const NSString *)name encrypt: (const struct ccmode_ccm*) enc
     decrypt: (const struct ccmode_ccm *)dec
{
    unsigned int i;

    unsigned int numVectors = (unsigned int)(sizeof(aes_ccm_vectors)/sizeof(aes_ccm_vectors[0]));
    for(i=0; i<numVectors; i++)
    {
        const struct ccmode_ccm_vector *v=&aes_ccm_vectors[i];

        XCTAssertEqual(0, ccmode_ccm_test_one_vector(enc, v, 0, 0), @"Encrypt Vector %d (%@)", i, name);
        XCTAssertEqual(0, ccmode_ccm_test_one_vector(dec, v, 1, 0), @"Decrypt Vector %d (%@)", i, name);

        XCTAssertEqual(0, ccmode_ccm_test_one_vector(enc, v, 0, 1), @"Encrypt Chained Vector %d (%@)", i, name);
        XCTAssertEqual(0, ccmode_ccm_test_one_vector(dec, v, 1, 1), @"Decrypt Chained Vector %d (%@)", i, name);
    }
}


@end
