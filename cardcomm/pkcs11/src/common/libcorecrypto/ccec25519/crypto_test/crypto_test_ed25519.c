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


#define USE_SUPER_COOL_NEW_CCOID_T
#include "testmore.h"
#include "testbyteBuffer.h"
#include "testccnBuffer.h"

#if (CCED25519TEST == 0)
entryPoint(cced25519_test,"cced25519 test")
#else
#include <corecrypto/ccec25519.h>
#include <corecrypto/ccsha2.h>

static int verbose = 0;

typedef struct {
	uint8_t			sk[32];
	uint8_t			pk[32];
	uint8_t			sig[64];
	size_t			len;
	const void *	msg;
	
}	ed25519_test_vector;

static const ed25519_test_vector	testVectors[] = {
	#include "crypto_test_ed25519.inc"
};

static int cced_round_trip(const struct ccdigest_info *di, char *extlabel,
    const ccec25519secretkey sk, const ccec25519pubkey pk,
    const void *katsig,
    size_t msglen, const void *msg) {
    
    uint8_t sig[64];
    char label[64];
    int err;
    
    cc_zero(sizeof(sig),sig);
    cced25519_sign(di, sig, msglen, msg, pk, sk);
    if(katsig) {
        snprintf(label, sizeof(label), "Sign %s", extlabel);
        ok(memcmp(sig, katsig, sizeof(sig)) == 0, label);
    }
    err = cced25519_verify(di, msglen, msg, sig, pk);
    snprintf(label, sizeof(label), "Verify %s", extlabel);
    ok(!err, label);
    return err == 0;
}

int cced25519test(TM_UNUSED int argc, TM_UNUSED char *const *argv) {
	size_t i, n;
    struct ccrng_state *rng = global_test_rng;
    const struct ccdigest_info *di = ccsha512_di();
    char label[64];
    
	plan_tests(2058);
	
	if(verbose) diag("Starting ed25519 tests\n");
	
	n = sizeof(testVectors) / sizeof(*testVectors);
	for(i = 0; i < n; ++i) {
		const ed25519_test_vector * const 	tv  = &testVectors[i];
		snprintf(label, sizeof(label), "test vector %zu", i + 1);
        cced_round_trip(di, label, tv->sk, tv->pk, tv->sig, tv->len, tv->msg);
	}
    
    byteBuffer msg = hexStringToBytes("1010101010101010101010101010");
	for(i = 0; i < 10; ++i) {
        ccec25519secretkey sk;
        ccec25519pubkey pk;
        cced25519_make_key_pair(di, rng, pk, sk);
		snprintf(label, sizeof(label), "Generated Pair Test %zu", i + 1);
        cced_round_trip(di, label, sk, pk, NULL, msg->len, msg->bytes);
    }
    free(msg);
	return 0;
}

#endif // CCED25519TEST
