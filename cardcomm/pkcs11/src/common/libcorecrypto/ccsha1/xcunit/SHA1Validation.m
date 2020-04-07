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


#import "SHA1Validation.h"

#include <corecrypto/ccsha1.h>
#include <corecrypto/cc_config.h>
#include "cc_unit.h"

@implementation SHA1Validation

static const struct ccdigest_info *di[]={
    &ccsha1_eay_di,
    &ccsha1_ltc_di,
#if !defined(__NO_ASM__) && CCSHA1_VNG_INTEL
    &ccsha1_vng_intel_SupplementalSSE3_di, // Assumes SupplementalSSE3
#endif
#if !defined(__NO_ASM__) && CCSHA1_VNG_ARMV7NEON
    &ccsha1_vng_armv7neon_di,
#endif
};

#define N_IMPL (sizeof(di)/sizeof(di[0]))

static NSString *impl[N_IMPL]={
    @"eay (openssl)",
    @"ltc (libtomcrypt)",
#if !defined(__NO_ASM__) && CCSHA1_VNG_INTEL
    @"vng_intel (vector numerics group)",
#endif
#if !defined(__NO_ASM__) && CCSHA1_VNG_ARMV7NEON
    @"vng_armv7neon (vector numerics group)",
#endif
};

static const struct ccdigest_vector sha1_vectors[]=
{
    {
        0,
        "",
        "\xda\x39\xa3\xee\x5e\x6b\x4b\x0d\x32\x55\xbf\xef\x95\x60\x18\x90\xaf\xd8\x07\x09"
    },
    {
        43,
        "The quick brown fox jumps over the lazy dog",
        "\x2f\xd4\xe1\xc6\x7a\x2d\x28\xfc\xed\x84\x9e\xe1\xbb\x76\xe7\x39\x1b\x93\xeb\x12"
    },
    {
        175,
        "The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog",
        "\xbe\xfa\xa0\x1d\x4d\x6d\x1e\x09\xbc\x96\x6e\x81\x0d\xb6\xf7\xc5\x67\x23\xf8\x2a"
    },
    {
        351,
        "The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog",
        "\xbe\x29\x0f\xc5\xf6\x4c\xec\x43\x55\xf5\x57\x9f\x6a\x7b\xb1\x97\x48\x4f\x76\x77"
    },
    {
        111,
        "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901",
        "\xf5\xd1\xcd\xda\xd8\x54\x63\x60\x81\x76\x53\xc2\xf5\xee\x58\xde\xb9\x21\x79\xcb"
    },
#include "SHA1ShortMsg.inc"
#include "SHA1LongMsg.inc"
};

#define N_VECTORS (sizeof(sha1_vectors)/sizeof(sha1_vectors[0]))

/* Test vectors for the compress function only */

#if !defined(__NO_ASM__) && CCSHA1_VNG_ARMV7NEON
- (void) testCompressArmv7 {
    const struct ccdigest_info *ref = &ccsha1_eay_di;
    const struct ccdigest_info *vng = &ccsha1_vng_armv7neon_di;

    unsigned char vector[CCSHA1_BLOCK_SIZE]; 
    
    for(unsigned int i=0; i<CCSHA1_BLOCK_SIZE; i++) {
        cc_ctx_decl(struct ccdigest_state, CCSHA1_STATE_SIZE, state_ref);
        cc_ctx_decl(struct ccdigest_state, CCSHA1_STATE_SIZE, state_vng);
        cc_zero(sizeof(state_ref),state_ref);
        cc_zero(sizeof(state_vng),state_vng);
        cc_zero(sizeof(vector),vector);

        vector[i]=0x80;

        ref->compress(state_ref, 1, vector);
        vng->compress(state_vng, 1, vector);

        XCAssertMemEquals(20, state_ref, state_vng, @"%d\n", i);
        cc_zero(CCSHA1_STATE_SIZE, state_ref);
        cc_zero(CCSHA1_STATE_SIZE, state_vng);
    }
}
#endif

- (void) testOneShot {
    for(unsigned int j=0; j<N_IMPL; j++) {
        for(unsigned int i=0; i<N_VECTORS; i++) {
            XCTAssertEqual(0, ccdigest_test_vector(di[j], &sha1_vectors[i]),@"Vector %d (%@)", i, impl[j]);
        }
    }
}

- (void) testChunks {
    for(unsigned int j=0; j<N_IMPL; j++) {
        for(unsigned int i=0; i<N_VECTORS; i++) {
            XCTAssertEqual(0, ccdigest_test_chunk_vector(di[j], &sha1_vectors[i], 1),@"Vector %d (%@)", i, impl[j]);
        }
    }
}

@end
