/*
 * Copyright (c) 2015 Apple Inc. All rights reserved.
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


#include <corecrypto/ccdrbg.h>
#include <corecrypto/cc_debug.h>
#include <corecrypto/ccaes.h>
#include <corecrypto/ccsha2.h>
#include <corecrypto/ccdrbg.h>
#include "testByteBuffer.h"
#include <stdlib.h>
#include <limits.h>
#include "drbg_limits_test.h"
#include "testmore.h"
#include <corecrypto/cc_macros.h>


//very boring piece of code
typedef void (*ccdrbg_factory_t)(struct ccdrbg_info *,  void *);
#define ok2(cond, s) (ok((cond), (s))?0:-1)
static char *s32 = "01234567890123456789012345678901";
static const int WHATEVER=5;

static int drbg_init_limits_test( ccdrbg_factory_t ccdrbg_factory, void *custom)
{
    struct ccdrbg_info drbg;
    int rc, rc2;
    struct ccdrbg_state *state;
    char *entropy=" ";
    char *nonce=" ";
    char *ps=" ";
    
    //too much entropy
    ccdrbg_factory(&drbg, custom);
    char b[drbg.size];     state = (struct ccdrbg_state *) b;
    rc = drbg.init(&drbg, state, CCDRBG_MAX_ENTROPY_SIZE+1, entropy, WHATEVER, nonce, WHATEVER, ps);
    rc2 = ok2(rc!=0, "drbg init: max entropy length test failed");
    
    //too little entropy
    ccdrbg_factory(&drbg, custom);
    rc = drbg.init(&drbg, state, 0, entropy, WHATEVER, nonce, WHATEVER, ps);
    rc2|=ok2(rc!=0, "drbg init: min entropy length test failed");

    //no test for nonce as it ias not checked.
    
    //too much PS
    ccdrbg_factory(&drbg, custom);
    rc = drbg.init(&drbg, state, 32, s32, WHATEVER, nonce, CCDRBG_MAX_PSINPUT_SIZE+1, ps);
    rc2|=ok2(rc!=0, "drbg init: max personalization string length test failed");

    //NULL PS
    ccdrbg_factory(&drbg, custom);
    rc = drbg.init(&drbg, state, 32, s32, WHATEVER, s32, WHATEVER, NULL);
    rc2|=ok2(rc==0, "drbg init: NULL personalization string  test failed");

    return rc2;
}

static int drbg_reseed_limits_test( ccdrbg_factory_t ccdrbg_factory, void *custom)
{
    struct ccdrbg_info drbg;
    int rc, rc2;
    struct ccdrbg_state *state;

    //init
    ccdrbg_factory(&drbg, custom);
    char b[drbg.size];     state = (struct ccdrbg_state *) b;
    rc = drbg.init(&drbg, state, 32, s32, WHATEVER, s32, WHATEVER, s32);
    rc2 = ok2(rc==0, "drbg init failed");
    
    //too much entropy
    rc = drbg.reseed(state, CCDRBG_MAX_ENTROPY_SIZE+1, s32, WHATEVER, s32);
    rc2 |= ok2(rc!=0, "drbg reseed: max entropy length test failed");
    
    //too little entropy
    rc = drbg.reseed(state, 2, s32, WHATEVER, s32);
    rc2|=ok2(rc!=0, "drbg reseed: min entropy length test failed");
    
    
    //too much additional input
    rc = drbg.reseed(state, 32, s32, CCDRBG_MAX_ADDITIONALINPUT_SIZE+1, s32);
    rc2|=ok2(rc!=0, "drbg reseed: max personalization string length test failed");
    
    //reseed with NULL additional input
    rc = drbg.reseed(state, 32, s32, WHATEVER, NULL);
    rc2|=ok2(rc==0, "drbg reseed: NULL personalization string  test failed");
    
    return rc2;
}

static int drbg_generate_limits_test( ccdrbg_factory_t ccdrbg_factory, void *custom)
{
    struct ccdrbg_info drbg;
    int rc, rc2;
    struct ccdrbg_state *state;
    char out[1024];
    
    ccdrbg_factory(&drbg, custom);
    char b[drbg.size];     state = (struct ccdrbg_state *) b;
    rc = drbg.init(&drbg, state, 32, s32, WHATEVER, s32, WHATEVER, s32);
    rc2 = ok2(rc==0, "drbg init failed");
   
    rc = drbg.generate(state, 0, out, WHATEVER, s32);
    rc2|=ok2(rc!=0, "drbg generate: minimum input len failed");

    rc = drbg.generate(state, CCDRBG_MAX_REQUEST_SIZE+1, out, WHATEVER, s32);
    rc2|=ok2(rc!=0, "drbg generate: maximum input len failed");

  
    rc = drbg.generate(state, 16, out, CCDRBG_MAX_PSINPUT_SIZE+1, s32);
    rc2|=ok2(rc!=0, "drbg generate: max personalization string length test failed");

    return rc2;
}

static int largest(int a, int b, int c)
{
   if( a>b && a>c )
       return a;
    if(b>c)
        return b;
    else
        return c;
}

static int drbg_stress_test( ccdrbg_factory_t ccdrbg_factory, void *custom)
{

    struct ccdrbg_info drbg;
    int rc=-1;
    struct ccdrbg_state *state;
    char *buf, *out;
    out=buf=NULL;
    
    ccdrbg_factory(&drbg, custom);
    char b[drbg.size]; state = (struct ccdrbg_state *) b;
    
    uint32_t n = largest(CCDRBG_MAX_ENTROPY_SIZE, CCDRBG_MAX_ADDITIONALINPUT_SIZE, CCDRBG_MAX_PSINPUT_SIZE);
    cc_assert(n <((uint32_t)1<<30));
    buf = malloc(n); cc_require(buf!=NULL, end);
    out = malloc(CCDRBG_MAX_REQUEST_SIZE); cc_require(out!=NULL, end);
    
    rc = drbg.init(&drbg, state, CCDRBG_MAX_ENTROPY_SIZE-1, buf, CCDRBG_MAX_ENTROPY_SIZE-1, buf, CCDRBG_MAX_PSINPUT_SIZE-1, buf);
    rc |= drbg.generate(state, CCDRBG_MAX_REQUEST_SIZE-1, out, CCDRBG_MAX_ADDITIONALINPUT_SIZE-1, buf);
    rc |= drbg.reseed  (state, CCDRBG_MAX_ENTROPY_SIZE-1, buf, CCDRBG_MAX_ADDITIONALINPUT_SIZE-1, buf);
    rc |= drbg.generate(state, CCDRBG_MAX_REQUEST_SIZE-1, out, CCDRBG_MAX_ADDITIONALINPUT_SIZE-1, buf);
    
end:
    free(buf);
    free(out);
    
    return rc;
}

static int drbg_all_limits_test( ccdrbg_factory_t ccdrbg_factory, void *custom)
{
    int rc;
    
    rc =  drbg_init_limits_test(ccdrbg_factory, custom);
    rc |= drbg_generate_limits_test(ccdrbg_factory, custom);
    rc |= drbg_reseed_limits_test(ccdrbg_factory, custom);
    ok(rc==0, "drbg limit test failed");
    
    rc = drbg_stress_test(ccdrbg_factory, custom);
    ok(rc==0, "drbg stress tst failed");
    
    return rc;
}

int drbg_limits_test(void)
{
    int rc;
    struct ccdrbg_nistctr_custom ctr_custom = {
        .ecb = ccaes_ecb_encrypt_mode(),
        .keylen = 16,
        .strictFIPS = 1,
        .use_df = 1,
    };
    
    struct ccdrbg_nisthmac_custom mac_custom = {
        .di = ccsha256_di(),
        .strictFIPS = 1,
    };

#if CORECRYPTO_DEBUG
    diag("Negative test - ccassert expected");
#endif
    // generates 3 error messages"
    rc  = drbg_all_limits_test((ccdrbg_factory_t)ccdrbg_factory_nistctr,  &ctr_custom);
    rc |= drbg_all_limits_test((ccdrbg_factory_t)ccdrbg_factory_nisthmac, &mac_custom);

#if CORECRYPTO_DEBUG
    diag("End of negative tests");
#endif

    return rc;
}




