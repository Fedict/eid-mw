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
#include <corecrypto/cc_runtime_config.h>

#if (CCANSIKDFTEST == 0)
entryPoint(ccansikdf,"ccansikdf")
#else
#include "crypto_test_ansikdf.h"
static const int kTestTestCount = 209;

#import <corecrypto/ccsha1.h>
#import <corecrypto/ccsha2.h>

#define di_SHA1   &ccsha1_eay_di
#define di_SHA224 &ccsha224_ltc_di
#define di_SHA256 &ccsha256_ltc_di
#define di_SHA384 &ccsha384_ltc_di
#define di_SHA512 &ccsha512_ltc_di

const struct ccansi_kdf_vector kdf_vectors_x963_sha1[]=
{
#include "../test_vectors/ansx963_cavs_sha1.inc"
};

const struct ccansi_kdf_vector kdf_vectors_x963_sha224[]=
{
#include "../test_vectors/ansx963_cavs_sha224.inc"
};

const struct ccansi_kdf_vector kdf_vectors_x963_sha256[]=
{
#include "../test_vectors/ansx963_cavs_sha256.inc"
};

const struct ccansi_kdf_vector kdf_vectors_x963_sha384[]=
{
#include "../test_vectors/ansx963_cavs_sha384.inc"
};

const struct ccansi_kdf_vector kdf_vectors_x963_sha512[]=
{
#include "../test_vectors/ansx963_cavs_sha512.inc"
};

const uint32_t magic=0xFACE;

// Process one vector
static int ccansikdf_x963_vector(const struct ccansi_kdf_vector *test)
{
    size_t output_len=(CC_BITLEN_TO_BYTELEN(test->key_data_length));
    uint8_t output[output_len+sizeof(magic)];
    cc_zero(sizeof(output),output);
    memcpy(&output[output_len],&magic,sizeof(magic));

    byteBuffer Z_data = hexStringToBytes(test->Z);
    byteBuffer SharedInfo_data = hexStringToBytes(test->SharedInfo);
    byteBuffer ExpectedKeyData_data = hexStringToBytes(test->key_data);

    assert(Z_data->len==CC_BITLEN_TO_BYTELEN(test->shared_secret_length));
    assert(SharedInfo_data->len==CC_BITLEN_TO_BYTELEN(test->SharedInfo_length));
    assert(ExpectedKeyData_data->len==output_len);

    ccansikdf_x963(test->di,
                   Z_data->len, Z_data->bytes,
                   SharedInfo_data->len,SharedInfo_data->bytes,
                   output_len,output);

    ok_memcmp_or_fail(output,ExpectedKeyData_data->bytes,ExpectedKeyData_data->len,
                      "Known answer test KDF x963");
    ok_memcmp_or_fail(&output[output_len],&magic,sizeof(magic),
                      "Output overflow KDF x963");
    free(Z_data);
    free(SharedInfo_data);
    free(ExpectedKeyData_data);

    return 1; // Pass
}

// Loop through all vectors
static int ccansikdf_x963_test(const struct ccansi_kdf_vector *test)
{
    size_t test_counter=0;
    int test_status=1;
    const struct ccansi_kdf_vector * current_test=&test[test_counter++];
    while (current_test->di!=NULL && test_status)
    {
        test_status=ccansikdf_x963_vector(current_test);
        current_test=&test[test_counter++];
    }
    return test_status;
}


int ccansikdf(TM_UNUSED int argc, TM_UNUSED char *const *argv)
{
    plan_tests(kTestTestCount);
    ok_status(ccansikdf_x963_test(kdf_vectors_x963_sha1)!=1,"x9.63 SHA1");
    ok_status(ccansikdf_x963_test(kdf_vectors_x963_sha224)!=1,"x9.63 SHA224");
    ok_status(ccansikdf_x963_test(kdf_vectors_x963_sha256)!=1,"x9.63 SHA256");
    ok_status(ccansikdf_x963_test(kdf_vectors_x963_sha384)!=1,"x9.63 SHA384");
    ok_status(ccansikdf_x963_test(kdf_vectors_x963_sha512)!=1,"x9.63 SHA512");
    return 0;
}

#endif
