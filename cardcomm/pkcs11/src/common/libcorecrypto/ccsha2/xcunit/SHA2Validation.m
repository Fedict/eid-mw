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


#import "SHA2Validation.h"

#include <corecrypto/cc_config.h>
#include <corecrypto/ccsha2.h>

static const struct ccdigest_vector sha224_vectors[] = {
	{
		0,
		"",
		"\xd1\x4a\x02\x8c\x2a\x3a\x2b\xc9\x47\x61\x02\xbb\x28\x82\x34\xc4\x15\xa2\xb0\x1f\x82\x8e\xa6\x2a\xc5\xb3\xe4\x2f"
	},
	{
		43,
		"The quick brown fox jumps over the lazy dog",
		"\x73\x0e\x10\x9b\xd7\xa8\xa3\x2b\x1c\xb9\xd9\xa0\x9a\xa2\x32\x5d\x24\x30\x58\x7d\xdb\xc0\xc3\x8b\xad\x91\x15\x25"
	},
	{
		175,
		"The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog",
		"\x9d\x62\x08\x74\x45\xb5\xb6\xe5\xf0\x60\x41\x39\x1a\xd1\xf3\x50\x0e\xa7\x6a\x7f\xfc\x04\x9f\x5f\xdf\xcb\x71\x98"
	},
	{
		351,
		"The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog",
		"\x5c\x33\xbf\x7d\xd7\x81\x08\xad\x8a\x12\x58\x88\xed\x06\xe5\x59\x25\xde\x76\x89\x00\xa1\x9d\x8d\x97\x2d\xd7\xe4"
	},
	{
		111,
		"123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901",
		"\x3a\x5c\x7c\x67\xec\x91\xae\xfa\xb3\x52\x92\xed\x63\xa9\x82\x16\x2c\x43\x67\x2d\xac\xa9\x85\x6e\xf3\x44\x3f\x01"
	},
#include "SHA224ShortMsg.inc"
#include "SHA224LongMsg.inc"
};
#define N_VECTORS_224 (sizeof(sha224_vectors)/sizeof(sha224_vectors[0]))

static const struct ccdigest_vector sha256_vectors[] = {
	{
		0,
		"",
		"\xe3\xb0\xc4\x42\x98\xfc\x1c\x14\x9a\xfb\xf4\xc8\x99\x6f\xb9\x24\x27\xae\x41\xe4\x64\x9b\x93\x4c\xa4\x95\x99\x1b\x78\x52\xb8\x55"
	},
	{
		43,
		"The quick brown fox jumps over the lazy dog",
		"\xd7\xa8\xfb\xb3\x07\xd7\x80\x94\x69\xca\x9a\xbc\xb0\x08\x2e\x4f\x8d\x56\x51\xe4\x6d\x3c\xdb\x76\x2d\x02\xd0\xbf\x37\xc9\xe5\x92"
	},
	{
		175,
		"The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog",
		"\x84\x80\xb7\x26\x39\x94\x62\xc5\x7e\x13\xda\xdb\x2f\x30\x79\x3e\xb2\xdf\xb8\x60\x00\xb3\x20\xd2\xa0\x2e\x45\x4c\xcd\xf5\x6f\x71"
	},
	{
		351,
		"The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog",
		"\x8c\xbd\xf6\xde\x5a\x8a\x93\xb3\x59\xae\x34\x60\x32\x5e\xc4\xe9\x11\x10\x89\x52\xf7\x22\xb2\xa3\xd3\x04\x9e\xbe\x4c\xa3\xfe\x19"
	},
	{
		111,
		"123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901",
		"\xc3\x7d\x69\x1a\x0a\x8a\x6a\x2a\x0c\x96\x17\x82\x8b\x4a\x7a\x86\xb0\x5b\x9f\x8c\xf4\x46\x13\xbb\xaf\x57\x3a\x6a\xb0\x0d\x68\x25"
	},
#include "SHA256ShortMsg.inc"
#include "SHA256LongMsg.inc"
};
#define N_VECTORS_256 (sizeof(sha256_vectors)/sizeof(sha256_vectors[0]))

static const struct ccdigest_vector sha384_vectors[] = {
	{
		0,
		"",
		"\x38\xb0\x60\xa7\x51\xac\x96\x38\x4c\xd9\x32\x7e\xb1\xb1\xe3\x6a\x21\xfd\xb7\x11\x14\xbe\x07\x43\x4c\x0c\xc7\xbf\x63\xf6\xe1\xda\x27\x4e\xde\xbf\xe7\x6f\x65\xfb\xd5\x1a\xd2\xf1\x48\x98\xb9\x5b"
	},
	{
		43,
		"The quick brown fox jumps over the lazy dog",
		"\xca\x73\x7f\x10\x14\xa4\x8f\x4c\x0b\x6d\xd4\x3c\xb1\x77\xb0\xaf\xd9\xe5\x16\x93\x67\x54\x4c\x49\x40\x11\xe3\x31\x7d\xbf\x9a\x50\x9c\xb1\xe5\xdc\x1e\x85\xa9\x41\xbb\xee\x3d\x7f\x2a\xfb\xc9\xb1"
	},
	{
		175,
		"The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog",
		"\x9e\x0a\x65\xe5\x55\xba\xbf\x29\x0e\xc5\x11\x58\x6c\x2b\x6c\xfc\xd3\x01\xeb\x4a\x4a\x28\xb7\xc8\xc2\x58\xc7\x77\x09\x92\x9c\xf1\x05\xbc\x14\xaa\x61\x4b\xe2\xf6\x76\x8d\xb3\x2c\x01\x3d\x7b\xab"
	},
	{
		351,
		"The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog",
		"\x75\xd0\x4a\x0d\x1a\x2e\xa1\xb4\x2f\xda\x4c\x32\x79\xa3\x7d\xae\x6f\x6c\xd7\x9d\x69\x9d\xf4\x16\x78\xc6\xea\xfc\x1d\x14\x1b\x0b\x99\x38\xd9\x0f\x0d\x92\x08\xbf\x87\x85\xcc\x11\x96\x3d\xe6\x2a"
	},
	{
		111,
		"123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901",
		"\x15\x6b\x52\x80\x27\xcb\x6d\xcc\x4a\xf1\x16\xcd\x69\xfb\x4b\xeb\x3e\xfb\x9c\x7e\xbc\xc5\x11\x47\xde\xbf\x16\x55\x0b\x75\x2c\x89\xd8\x42\xd6\xec\x9a\xb0\xa1\xb4\x0b\xb6\x9c\x95\xc5\x27\x4a\xb4"
	},
#include "SHA384ShortMsg.inc"
#include "SHA384LongMsg.inc"
};
#define N_VECTORS_384 (sizeof(sha384_vectors)/sizeof(sha384_vectors[0]))

static const struct ccdigest_vector sha512_vectors[] = {
	{
		0,
		"",
		"\xcf\x83\xe1\x35\x7e\xef\xb8\xbd\xf1\x54\x28\x50\xd6\x6d\x80\x07\xd6\x20\xe4\x05\x0b\x57\x15\xdc\x83\xf4\xa9\x21\xd3\x6c\xe9\xce\x47\xd0\xd1\x3c\x5d\x85\xf2\xb0\xff\x83\x18\xd2\x87\x7e\xec\x2f\x63\xb9\x31\xbd\x47\x41\x7a\x81\xa5\x38\x32\x7a\xf9\x27\xda\x3e"
	},
	{
		43,
		"The quick brown fox jumps over the lazy dog",
		"\x07\xe5\x47\xd9\x58\x6f\x6a\x73\xf7\x3f\xba\xc0\x43\x5e\xd7\x69\x51\x21\x8f\xb7\xd0\xc8\xd7\x88\xa3\x09\xd7\x85\x43\x6b\xbb\x64\x2e\x93\xa2\x52\xa9\x54\xf2\x39\x12\x54\x7d\x1e\x8a\x3b\x5e\xd6\xe1\xbf\xd7\x09\x78\x21\x23\x3f\xa0\x53\x8f\x3d\xb8\x54\xfe\xe6"
	},
	{
		175,
		"The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog",
		"\x7d\x95\x15\xbd\x20\x98\xde\x60\xc0\x02\xcb\x47\xa5\x53\x24\x2e\x7a\x90\xd4\x4a\x85\x84\x74\xd7\xdf\x6d\x99\xcb\xac\x5a\x07\xd1\xf5\xcc\x8d\x63\x21\xe9\x4e\x68\x9c\xff\x71\xa8\xf2\x18\x62\x08\xe6\xe2\xe2\xf1\x07\x6c\x08\x63\x5d\x2a\x45\x95\x6f\x3a\xde\xf6"
	},
	{
		351,
		"The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog",
		"\xab\x96\x5f\xed\xdd\x9b\x8e\x6d\xfe\xe5\x0d\x6c\x09\x71\x39\x0c\x84\x23\xab\x55\xae\xfd\xd4\x28\x1e\x2e\x67\x26\x17\xeb\xcb\x5d\x84\x42\xd2\xb8\x85\xa3\x98\xdb\xed\xc4\x41\x6d\xf2\x3e\x3c\xd1\x33\x6a\xf8\x7e\x86\xbc\x6b\xfd\xb7\x26\x27\xc6\xd3\x3d\x65\xc5"
	},
	{
		111,
		"123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901",
		"\x7a\xba\x44\x11\x84\x6c\x61\xb0\x8b\x0f\x22\x82\xa8\xa4\x60\x02\x32\xac\xe4\xdd\x96\x59\x3c\x75\x5b\xa9\xc9\xa4\xe7\xb7\x80\xb8\xbd\xc4\x37\xb5\xc5\x55\x74\xb3\xe8\x40\x9c\x7b\x51\x10\x32\xf9\x8e\xf1\x20\xe2\x54\x67\x67\x8f\x04\x58\x64\x35\x78\xeb\x60\xff"
	},
#include "SHA512ShortMsg.inc"
#include "SHA512LongMsg.inc"
};
#define N_VECTORS_512 (sizeof(sha512_vectors)/sizeof(sha512_vectors[0]))


@implementation SHA2Validation

struct ccdigest_impl {
    const struct ccdigest_info *di;
    NSString *name;
};

static const struct ccdigest_impl di_224[] = {
    {&ccsha224_ltc_di, @"ltc" },
#if !defined(__NO_ASM__) && CCSHA2_VNG_INTEL
    {&ccsha224_vng_intel_SupplementalSSE3_di, @"intel"}, // Assumes SupplementalSSE3
#endif
#if !defined(__NO_ASM__) && CCSHA2_VNG_ARMV7NEON
    {&ccsha224_vng_armv7neon_di, @"armv7 neon"},
#endif
};
#define N_IMPL_224 (sizeof(di_224)/sizeof(di_224[0]))


static const struct ccdigest_impl di_256[] = {
    {&ccsha256_ltc_di, @"ltc" },
#if !defined(__NO_ASM__) && CCSHA2_VNG_INTEL
    {&ccsha256_vng_intel_SupplementalSSE3_di, @"intel"}, // Assumes SupplementalSSE3
#endif
#if !defined(__NO_ASM__) && CCSHA2_VNG_ARMV7NEON
    {&ccsha256_vng_armv7neon_di, @"armv7 neon"},
#endif
};
#define N_IMPL_256 (sizeof(di_256)/sizeof(di_256[0]))

static const struct ccdigest_impl di_384[] = {
    {&ccsha384_ltc_di, @"ltc" },
};
#define N_IMPL_384 (sizeof(di_384)/sizeof(di_384[0]))

static const struct ccdigest_impl di_512[] = {
    {&ccsha512_ltc_di, @"ltc" },
};
#define N_IMPL_512 (sizeof(di_512)/sizeof(di_512[0]))

- (void) oneShotCommon: (int)n_impl : (const struct ccdigest_impl *)di : (int)n_vectors : (const struct ccdigest_vector *) vectors
{
    for(int j=0; j<n_impl; j++) {
        for(int i=0; i<n_vectors; i++) {
            XCTAssertEqual(0, ccdigest_test_vector(di[j].di, &vectors[i]),@"Vector %d (%@)", i, di[j].name);
        }
    }
}

- (void) chunkCommon: (int)n_impl : (const struct ccdigest_impl *)di : (int)n_vectors : (const struct ccdigest_vector*) vectors : (unsigned long) chunk
{
    for(int j=0; j<n_impl; j++) {
        for(int i=0; i<n_vectors; i++) {
            XCTAssertEqual(0, ccdigest_test_chunk_vector(di[j].di, &vectors[i], chunk),@"Vector %d (%@)", i, di[j].name);
        }
    }
}

- (void) testOneShotSHA224 {
    [self oneShotCommon:N_IMPL_224:di_224:N_VECTORS_224:sha224_vectors];
}

- (void) testChunksSHA224 {
    [self chunkCommon:N_IMPL_224: di_224: N_VECTORS_224: sha224_vectors: 1];
}

- (void) testOneShotSHA256 {
    [self oneShotCommon:N_IMPL_256: di_256: N_VECTORS_256: sha256_vectors];
}

- (void) testChunksSHA256 {
    [self chunkCommon:N_IMPL_256: di_256: N_VECTORS_256: sha256_vectors: 1];
}

- (void) testOneShotSHA384 {
    [self oneShotCommon:N_IMPL_384: di_384: N_VECTORS_384: sha384_vectors];
}

- (void) testChunksSHA384 {
    [self chunkCommon:N_IMPL_384: di_384: N_VECTORS_384: sha384_vectors: 1];
}

- (void) testOneShotSHA512 {
    [self oneShotCommon:N_IMPL_512: di_512: N_VECTORS_512: sha512_vectors];
}

- (void) testChunksSHA512 {
    [self chunkCommon:N_IMPL_512: di_512: N_VECTORS_512: sha512_vectors: 1];
}

@end
