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


#include "testmore.h"
#include "testbyteBuffer.h"
#include "testccnBuffer.h"

#include "crypto_test_ec.h"
#include <corecrypto/ccsha1.h>
#include <corecrypto/ccsha2.h>
#include <corecrypto/ccrng_ecfips_test.h>

#include <corecrypto/ccec_priv.h>

const struct ccecdsa_vector ccecdsa_vectors[]=
{
#include "../test_vectors/ecdsa.inc"
};

// Process one vector
static int ccecdsa_kat_vector(const struct ccecdsa_vector *test)
{
    int status = 0; // fail
    byteBuffer expected_r = hexStringToBytes(test->r);
    byteBuffer expected_s = hexStringToBytes(test->s);
    byteBuffer priv_key = hexStringToBytes(test->priv_key);
    byteBuffer k = hexStringToBytes(test->k);
    struct ccrng_ecfips_test_state rng;
    ccec_const_cp_t cp=test->curve();
    const struct ccdigest_info *di=test->di;
    ccec_full_ctx_decl_cp(cp, key);
    bool valid;
    // Generate "remote" public key from private key
    ok_or_fail((ccec_recover_full_key(cp,priv_key->len, priv_key->bytes, key) == 0), "Generated Key");


    // Set RNG to control k
    ccrng_ecfips_test_init(&rng, k->len, k->bytes);

    // Buffer for outputs
    byteBuffer computed_r = mallocByteBuffer(ccec_signature_r_s_size(key));
    byteBuffer computed_s = mallocByteBuffer(ccec_signature_r_s_size(key));
    byteBuffer hash = mallocByteBuffer(di->output_size);

    ccdigest(di, strlen(test->msg), test->msg, hash->bytes);
    // ccec_print_full_key("Imported key", key);
    ok_or_goto((ccec_verify_composite(key, di->output_size, hash->bytes,
                                      expected_r->bytes, expected_s->bytes, &valid)==0), "Verify", errout);

    ok_or_goto(valid==true, "Stock signature verification", errout);

    ok_or_goto((ccec_sign_composite(key, di->output_size, hash->bytes,
                                   computed_r->bytes, computed_s->bytes, (struct ccrng_state *)&rng)==0), "Sign", errout);

    ok_or_goto((ccec_verify_composite(key, di->output_size, hash->bytes,
                                      computed_r->bytes, computed_s->bytes, &valid)==0), "Verify", errout);

    ok_or_goto(valid==true, "Generated signature verification", errout);

    // Checks
    //cc_print("r: ",expected_r->len, computed_r->bytes);
    //cc_print("s: ",expected_s->len, computed_s->bytes);
    ok_or_goto(memcmp(computed_r->bytes,expected_r->bytes,expected_r->len)==0, "signature r", errout);
    ok_or_goto(memcmp(computed_s->bytes,expected_s->bytes,expected_s->len)==0, "signature s", errout);

    status = 1; // success
errout:
    free(computed_r);
    free(computed_s);
    free(hash);
    free(expected_r);
    free(expected_s);
    free(priv_key);
    free(k);
    return status;
}

// Process one vector
static int ccecdsa_negative_vector(const struct ccecdsa_vector *test)
{
    int status = 0; // fail
    byteBuffer expected_r = hexStringToBytes(test->r);
    byteBuffer expected_s = hexStringToBytes(test->s);
    byteBuffer priv_key = hexStringToBytes(test->priv_key);
    byteBuffer k = hexStringToBytes(test->k);
    struct ccrng_ecfips_test_state rng;
    ccec_const_cp_t cp=test->curve();
    const struct ccdigest_info *di=test->di;
    ccec_full_ctx_decl_cp(cp, key);
    bool valid;

    // Generate "remote" public key from private key
    ok_or_fail(ccec_recover_full_key(cp,priv_key->len, priv_key->bytes, key) == 0, "Generated Key");

    // Set RNG to control k
    ccrng_ecfips_test_init(&rng, k->len, k->bytes);

    // Buffer for outputs
    byteBuffer computed_r = mallocByteBuffer(ccec_signature_r_s_size(key));
    byteBuffer computed_s = mallocByteBuffer(ccec_signature_r_s_size(key));
    byteBuffer hash = mallocByteBuffer(di->output_size);

    ccdigest(di, strlen(test->msg), test->msg, hash->bytes);

    //==============================================
    // Negative testing of verify
    //==============================================

    // Good verify
    ok_or_goto((ccec_verify_composite(key, di->output_size, hash->bytes,
                                      expected_r->bytes, expected_s->bytes, &valid)==0), "Verify", errout);
    ok_or_goto(valid==true, "Good signature", errout);

    // r is corrupted
    expected_r->bytes[0]^=1;
    ok_or_goto((ccec_verify_composite(key, di->output_size, hash->bytes,
                                      expected_r->bytes, expected_s->bytes, &valid)==0), "Verify: r corrupted", errout);
    ok_or_goto(valid==false, "r corrupted", errout);
    expected_r->bytes[0]^=1;

    // s is corrupted
    expected_s->bytes[0]^=1;
    ok_or_goto((ccec_verify_composite(key, di->output_size, hash->bytes,
                                      expected_r->bytes, expected_s->bytes, &valid)==0), "Verify: s corrupted", errout);
    ok_or_goto(valid==false, "s corrupted", errout);
    expected_s->bytes[0]^=1;

    //==============================================
    // Negative testing of signature
    //==============================================

    ok_or_goto((ccec_sign_composite(key, di->output_size, hash->bytes,
                                    computed_r->bytes, computed_s->bytes, (struct ccrng_state *)&rng)==0), "Sign", errout);

    // Checks
    ok_or_goto(memcmp(computed_r->bytes,expected_r->bytes,expected_r->len)==0, "signature r", errout);
    ok_or_goto(memcmp(computed_s->bytes,expected_s->bytes,expected_s->len)==0, "signature s", errout);

    // RNG error
    ccrng_ecfips_test_init(&rng, 0, k->bytes);
    ok_or_goto((ccec_sign_composite(key, di->output_size, hash->bytes,
                                    computed_r->bytes, computed_s->bytes, (struct ccrng_state *)&rng)!=0), "Sign: rng", errout);



    status = 1; // success
errout:
    free(computed_r);
    free(computed_s);
    free(hash);
    free(expected_r);
    free(expected_s);
    free(priv_key);
    free(k);
    return status;
}


int
ecdsa_known_answer_tests(void)
{
    size_t test_counter=0;
    int test_status=1;
    const struct ccecdsa_vector * current_test=&ccecdsa_vectors[test_counter++];
    while (current_test->di!=NULL)
    {
        test_status=ccecdsa_kat_vector(current_test);
        current_test=&ccecdsa_vectors[test_counter++];
    }
    return test_status;
}

int
ecdsa_negative_tests(void)
{
    size_t test_counter=0;
    int test_status=1;
    const struct ccecdsa_vector * current_test=&ccecdsa_vectors[test_counter++];
    while (current_test->di!=NULL)
    {
        test_status=ccecdsa_negative_vector(current_test);
        current_test=&ccecdsa_vectors[test_counter++];
    }
    return test_status;
}

