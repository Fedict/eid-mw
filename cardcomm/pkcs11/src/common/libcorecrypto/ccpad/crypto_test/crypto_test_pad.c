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
#include "cc_debug.h"
#include <corecrypto/ccaes.h>
#include <corecrypto/ccdes.h>
#include <corecrypto/ccmode.h>
#include <corecrypto/ccpad.h>
#include <corecrypto/cc_runtime_config.h>
#include <corecrypto/ccn.h>
#include <ccconstanttime.h>

#if (CCPAD == 0)
entryPoint(ccpad,"ccpad")
#else
#include "crypto_test_modes.h"
#include "crypto_test_pad.h"
#include "ccsymmetric_pad.h"

static const int kTestTestCount = 1174;

static const int verbose=0;

#import <corecrypto/ccrng_test.h>
#include "cccycles.h"
#include "ccstats.h"

//======================================================================
// Constant time verification parameters
//======================================================================

// Number of iteration of test where timings are not taken into account.
// Made to reach a stable performance state
#define CCPAD_WARMUP        0

// Each sample is the average time for many iteration with identical inputs
#define CCPAD_TIMING_REPEAT  200

// Number of sample for the statistical analysis
// typically 100~1000 is a good range
#define CCPAD_TIMING_SAMPLES 200

// In case of failure, try many times
// This is to reduce false positives due to noise/timing accuracy.
// If implementation is not constant time, the behavior will be consistent
// So that this does not reduce the detection power.
#define CCPAD_TIMING_RETRIES 10

// Two statitical tools are available: T-test and Wilcoxon.
// T-test assumes that the distribution to be compared are normal
// Wilcoxon measure offset between distribution.
// Due to potential switches between performance state or occasional
// latencies, Wilcoxon is recommended.
// > Set to 1 to use T-test instead of Wilcoxon
#define T_TEST  1

// Number of iteration of the full test (to play with to evaluate chances of false positives)
#define CMP_TIMING_TEST_ITERATION 1

// Quantile for the repeated timing. Empirical value.
#define CCPAD_PERCENTILE 9

//======================================================================

// Local types
typedef struct {
    char *keyStr;
    char *init_ivStr;
    char *ptStr;
    char *paddingStr;
} ccpad_test_vector_t;

typedef struct duplex_cryptor_t {
    ciphermode_t encrypt_ciphermode;
    ciphermode_t decrypt_ciphermode;
    cc_cipher_select cipher;
    cc_mode_select mode;
} duplex_cryptor_s, *duplex_cryptor;

// Prototypes
int ccpad(TM_UNUSED int argc, TM_UNUSED char *const *argv);

static int
run_symmetric_pad_vectors(duplex_cryptor cryptor,ccpad_select padding_mode);

static int
ccsymmetric_pad_tests(duplex_cryptor cryptor, ccpad_select padding_mode, const ccpad_test_vector_t *test);

// Test vectors

// These are stock keys/IVs/blocks to encode - don't change them - add if you

#define keystr64     "0001020304050607"
#define keystr128    "000102030405060708090a0b0c0d0e0f"
#define ivstr64      "0f0e0d0c0b0a0908"
#define ivstr128     "0f0e0d0c0b0a09080706050403020100"

#define MSG_01       "00"
#define MSG_02       "0000"
#define MSG_03       "000000"
#define MSG_04       "00000000"
#define MSG_05       "0000000000"
#define MSG_06       "000000000000"
#define MSG_07       "00000000000000"
#define MSG_08       "0000000000000000"
#define MSG_09       "000000000000000000"
#define MSG_10       "00000000000000000000"
#define MSG_11       "0000000000000000000000"
#define MSG_12       "000000000000000000000000"
#define MSG_13       "00000000000000000000000000"
#define MSG_14       "0000000000000000000000000000"
#define MSG_15       "000000000000000000000000000000"
#define MSG_16       "00000000000000000000000000000000"
#define MSG_17       "0000000000000000000000000000000000"
#define MSG_18       "000000000000000000000000000000000000"
#define MSG_19       "00000000000000000000000000000000000000"
#define MSG_20       "0000000000000000000000000000000000000000"
#define MSG_21       "000000000000000000000000000000000000000000"
#define MSG_22       "00000000000000000000000000000000000000000000"
#define MSG_23       "0000000000000000000000000000000000000000000000"
#define MSG_24       "000000000000000000000000000000000000000000000000"
#define MSG_25       "00000000000000000000000000000000000000000000000000"
#define MSG_26       "0000000000000000000000000000000000000000000000000000"
#define MSG_27       "000000000000000000000000000000000000000000000000000000"
#define MSG_28       "00000000000000000000000000000000000000000000000000000000"
#define MSG_29       "0000000000000000000000000000000000000000000000000000000000"
#define MSG_30       "000000000000000000000000000000000000000000000000000000000000"
#define MSG_31       "00000000000000000000000000000000000000000000000000000000000000"
#define MSG_32       "0000000000000000000000000000000000000000000000000000000000000000"


#define PAD_16       "10101010101010101010101010101010"
#define PAD_15       "0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f"
#define PAD_14       "0e0e0e0e0e0e0e0e0e0e0e0e0e0e"
#define PAD_13       "0d0d0d0d0d0d0d0d0d0d0d0d0d"
#define PAD_12       "0c0c0c0c0c0c0c0c0c0c0c0c"
#define PAD_11       "0b0b0b0b0b0b0b0b0b0b0b"
#define PAD_10       "0a0a0a0a0a0a0a0a0a0a"
#define PAD_09       "090909090909090909"
#define PAD_08       "0808080808080808"
#define PAD_07       "07070707070707"
#define PAD_06       "060606060606"
#define PAD_05       "0505050505"
#define PAD_04       "04040404"
#define PAD_03       "030303"
#define PAD_02       "0202"
#define PAD_01       "01"

#define END_VECTOR   { NULL, NULL, NULL, NULL}

static const ccpad_test_vector_t aes_vectors[] = {
    {keystr128,ivstr128, MSG_01,  PAD_15},
    {keystr128,ivstr128, MSG_02,  PAD_14},
    {keystr128,ivstr128, MSG_03,  PAD_13},
    {keystr128,ivstr128, MSG_04,  PAD_12},
    {keystr128,ivstr128, MSG_05,  PAD_11},
    {keystr128,ivstr128, MSG_06,  PAD_10},
    {keystr128,ivstr128, MSG_07,  PAD_09},
    {keystr128,ivstr128, MSG_08,  PAD_08},
    {keystr128,ivstr128, MSG_09,  PAD_07},
    {keystr128,ivstr128, MSG_10,  PAD_06},
    {keystr128,ivstr128, MSG_11,  PAD_05},
    {keystr128,ivstr128, MSG_12,  PAD_04},
    {keystr128,ivstr128, MSG_13,  PAD_03},
    {keystr128,ivstr128, MSG_14,  PAD_02},
    {keystr128,ivstr128, MSG_15,  PAD_01},
    {keystr128,ivstr128, MSG_16,  PAD_16},
    {keystr128,ivstr128, MSG_17,  PAD_15},
    {keystr128,ivstr128, MSG_18,  PAD_14},
    {keystr128,ivstr128, MSG_19,  PAD_13},
    {keystr128,ivstr128, MSG_20,  PAD_12},
    {keystr128,ivstr128, MSG_21,  PAD_11},
    {keystr128,ivstr128, MSG_22,  PAD_10},
    {keystr128,ivstr128, MSG_23,  PAD_09},
    {keystr128,ivstr128, MSG_24,  PAD_08},
    {keystr128,ivstr128, MSG_25,  PAD_07},
    {keystr128,ivstr128, MSG_26,  PAD_06},
    {keystr128,ivstr128, MSG_27,  PAD_05},
    {keystr128,ivstr128, MSG_28,  PAD_04},
    {keystr128,ivstr128, MSG_29,  PAD_03},
    {keystr128,ivstr128, MSG_30,  PAD_02},
    {keystr128,ivstr128, MSG_31,  PAD_01},
    {keystr128,ivstr128, MSG_32,  PAD_16},
    { NULL, NULL, NULL, NULL}
};

static const ccpad_test_vector_t des_vectors[] = {
    {keystr64, ivstr64, MSG_01,  PAD_07},
    {keystr64, ivstr64, MSG_02,  PAD_06},
    {keystr64, ivstr64, MSG_03,  PAD_05},
    {keystr64, ivstr64, MSG_04,  PAD_04},
    {keystr64, ivstr64, MSG_05,  PAD_03},
    {keystr64, ivstr64, MSG_06,  PAD_02},
    {keystr64, ivstr64, MSG_07,  PAD_01},
    {keystr64, ivstr64, MSG_08,  PAD_08},
    {keystr64, ivstr64, MSG_09,  PAD_07},
    {keystr64, ivstr64, MSG_10,  PAD_06},
    {keystr64, ivstr64, MSG_11,  PAD_05},
    {keystr64, ivstr64, MSG_12,  PAD_04},
    {keystr64, ivstr64, MSG_13,  PAD_03},
    {keystr64, ivstr64, MSG_14,  PAD_02},
    {keystr64, ivstr64, MSG_15,  PAD_01},
    {keystr64, ivstr64, MSG_16,  PAD_08},
    { NULL, NULL, NULL, NULL}
};


// Functions implementation
static int
ccsymmetric_pad_tests(duplex_cryptor cryptor, ccpad_select padding_mode, const ccpad_test_vector_t *test) {
    // Retrieve test case info
    byteBuffer key =        hexStringToBytes(test->keyStr);
    byteBuffer init_iv =    hexStringToBytes(test->init_ivStr);
    byteBuffer message =    hexStringToBytes(test->ptStr);
    byteBuffer expected_padding =    hexStringToBytes(test->paddingStr);
    size_t len = message->len;
    size_t result_len;
    size_t block_size=0;
    size_t padded_len=0;

    byteBuffer ciphertext,padded_message;

    // Set cipher
    cc_ciphermode_descriptor_s encrypt_desc;
    cc_ciphermode_descriptor_s decrypt_desc;

    encrypt_desc.cipher = cryptor->cipher;
    encrypt_desc.mode = cryptor->mode;
    encrypt_desc.direction = cc_Encrypt;
    encrypt_desc.ciphermode = cryptor->encrypt_ciphermode;

    decrypt_desc.cipher = cryptor->cipher;
    decrypt_desc.mode = cryptor->mode;
    decrypt_desc.direction = cc_Decrypt;
    decrypt_desc.ciphermode = cryptor->decrypt_ciphermode;

    block_size=cc_symmetric_bloc_size(&encrypt_desc);
    padded_len=(len+block_size) & (~(block_size-1)); // Assumes block size is a power of 2.

    MAKE_GENERIC_MODE_CONTEXT(encrypt_ctx, &encrypt_desc);
    MAKE_GENERIC_MODE_CONTEXT(decrypt_ctx, &decrypt_desc);
    ok_or_fail((cc_symmetric_setup(&encrypt_desc, key->bytes, key->len, init_iv->bytes, encrypt_ctx) == 0), "Encrypted cipher-mode is initted");
    ok_or_fail((cc_symmetric_setup(&decrypt_desc, key->bytes, key->len, init_iv->bytes, decrypt_ctx) == 0), "Decrypted cipher-mode is initted");

    // Temporary buffers
    ciphertext  = mallocByteBuffer(padded_len);
    padded_message = mallocByteBuffer(padded_len);

    //----------------------------------
    // 1. Test padding encryption
    //----------------------------------

    // a. Encrypt using padding function
    result_len=cc_symmetric_crypt_pad((cc_symmetric_context_p) encrypt_ctx,padding_mode, init_iv->bytes, message->bytes, ciphertext->bytes, len);
    ok_or_fail(result_len >= len, "Encryption with padding failed");

    // b. Decrypt raw data
    cc_symmetric_crypt((cc_symmetric_context_p) decrypt_ctx, init_iv->bytes, ciphertext->bytes, padded_message->bytes, result_len);

    // c. Compare last block with expected value
    ok_memcmp_or_fail(&padded_message->bytes[result_len-expected_padding->len], expected_padding->bytes, expected_padding->len, "Padding as expected");

    //----------------------------------
    // 2. Test padding decryption
    //----------------------------------

    // a. Construct the good padded zero message
    memcpy(padded_message->bytes,message->bytes, len);
    memcpy(&padded_message->bytes[len],expected_padding->bytes,expected_padding->len);

    // b. Encrypt with proper padding
    cc_symmetric_crypt((cc_symmetric_context_p) encrypt_ctx, init_iv->bytes, padded_message->bytes, ciphertext->bytes, len+expected_padding->len);
    memset(padded_message->bytes,0xff,padded_len); // Clean buffer to catch mismatches below

    // b. Decrypt with padding decrypt function
    result_len=cc_symmetric_crypt_pad((cc_symmetric_context_p) decrypt_ctx,padding_mode, init_iv->bytes, ciphertext->bytes, padded_message->bytes,len+expected_padding->len);
    ok_or_fail(result_len <= len+expected_padding->len, "Decryption with padding failed");

    // c. Compare that the message is all there.
    ok_memcmp_or_fail(padded_message->bytes,
                      message->bytes, len, "Decrypted handcrafted padding");

    ok_or_fail(len == result_len, "Decrypted length from handcrafted padding");

    //----------------------------------
    // 3. wrap/unwrap
    //----------------------------------

    // a. Encrypt with padding
    result_len=cc_symmetric_crypt_pad((cc_symmetric_context_p) encrypt_ctx,padding_mode, init_iv->bytes, message->bytes, ciphertext->bytes, len);
    ok_or_fail(result_len >= len, "Encryption with padding failed");

    // b. Decrypt with padding
    result_len=cc_symmetric_crypt_pad((cc_symmetric_context_p) decrypt_ctx,padding_mode, init_iv->bytes, ciphertext->bytes, padded_message->bytes, result_len);
    ok_or_fail(result_len <= len, "Decryption with padding failed");

    // c. Compare with original messsage
    ok_memcmp_or_fail(padded_message->bytes,
                      message->bytes, len, "Wrap/Unwrapped decrypted message");

    ok_or_fail(message->len == result_len, "Wrap/Unwrapped decrypted length");

    return 1;
}

__unused static int
ccsymmetric_crypt_pad_timing_tests(duplex_cryptor cryptor, ccpad_select padding_mode, const ccpad_test_vector_t *test) {
    // Retrieve test case info
    // Message is ignore, random messages are generated
    byteBuffer key =        hexStringToBytes(test->keyStr);
    byteBuffer init_iv =    hexStringToBytes(test->init_ivStr);
    byteBuffer plaintext,output,ciphertext_valid,ciphertext_random;
    size_t len = 0;
    size_t max_len = 0;
    size_t result_len = 0;
    size_t block_size=0;
    size_t padded_len=0;
    int failure_cnt=0;
    int early_abort=1;
    uint32_t j,sample_counter;
    bool retry=true;

    // Random for messages
    struct ccrng_state *rng = global_test_rng;

    // Set cipher
    cc_ciphermode_descriptor_s encrypt_desc;
    cc_ciphermode_descriptor_s decrypt_desc;

    encrypt_desc.cipher = cryptor->cipher;
    encrypt_desc.mode = cryptor->mode;
    encrypt_desc.direction = cc_Encrypt;
    encrypt_desc.ciphermode = cryptor->encrypt_ciphermode;

    decrypt_desc.cipher = cryptor->cipher;
    decrypt_desc.mode = cryptor->mode;
    decrypt_desc.direction = cc_Decrypt;
    decrypt_desc.ciphermode = cryptor->decrypt_ciphermode;

    MAKE_GENERIC_MODE_CONTEXT(encrypt_ctx, &encrypt_desc);
    MAKE_GENERIC_MODE_CONTEXT(decrypt_ctx, &decrypt_desc);
    ok_or_fail((cc_symmetric_setup(&encrypt_desc, key->bytes, key->len, init_iv->bytes, encrypt_ctx) == 0), "Encrypted cipher-mode is initted");
    ok_or_fail((cc_symmetric_setup(&decrypt_desc, key->bytes, key->len, init_iv->bytes, decrypt_ctx) == 0), "Decrypted cipher-mode is initted");

    // Work on messages of size 0 < len <= blocksize
    block_size=cc_symmetric_bloc_size(&encrypt_desc);
    len=block_size;
    max_len=(len+block_size) & (~(block_size-1)); // Assumes block size is a power of 2.

    // Temporary buffers
    plaintext  = mallocByteBuffer(max_len);
    ciphertext_valid  = mallocByteBuffer(max_len);
    ciphertext_random  = mallocByteBuffer(max_len);
    output  = mallocByteBuffer(max_len);

    for (len=0; len<=block_size;len++)
    {
        j=0;
        while(retry)
        {
            sample_counter=0; // Index of current sample
            measurement_t timing_sample[2*CCPAD_TIMING_SAMPLES];

            for (size_t i=0;i<2*CCPAD_TIMING_SAMPLES+(CCPAD_WARMUP/CCPAD_TIMING_REPEAT);i++)
            {
                padded_len=(len+block_size) & (~(block_size-1)); // Assumes block size is a power of 2.
                volatile size_t decode_result;
                if ((len==0) || ((i&1) == 0))
                {
                    // -------------------------
                    //      Random
                    // -------------------------
                    ccrng_generate(rng,padded_len,plaintext->bytes); // Full length
                    cc_symmetric_crypt((cc_symmetric_context_p) encrypt_ctx, init_iv->bytes, plaintext->bytes, ciphertext_valid->bytes, padded_len);
                    result_len=padded_len;
                }
                else
                {
                    // -------------------------
                    //      Correct padding
                    // -------------------------
                    // Create message with good padding
                    ccrng_generate(rng,len,plaintext->bytes);         // Actual length
                    result_len=cc_symmetric_crypt_pad((cc_symmetric_context_p) encrypt_ctx,padding_mode, init_iv->bytes, plaintext->bytes, ciphertext_valid->bytes, len);
                    assert(result_len == padded_len);
                }

                // Decrypt with padding decrypt function
                cc_symmetric_crypt((cc_symmetric_context_p) decrypt_ctx,init_iv->bytes, ciphertext_valid->bytes, output->bytes,result_len);

                TIMING_WITH_QUANTILE(timing_sample[sample_counter].timing,
                                     CCPAD_TIMING_REPEAT,
                                     CCPAD_PERCENTILE,
                                     decode_result=ccpad_pkcs7_decode(block_size,output->bytes+result_len-block_size),errOut);

                timing_sample[sample_counter].group=sample_counter&1;
#if CCPAD_WARMUP
                if (i>=(CCPAD_WARMUP/CCPAD_TIMING_REPEAT))
#endif
                {
                    sample_counter++;
                }
            }
#if CCN_OSX
            if (verbose>1) {
                char file_name[64];
                snprintf(file_name,sizeof(file_name),"corecrypto_test_timings_%.2zu.csv",len);
                export_measurement_to_file(file_name,timing_sample,sample_counter);
            }
#endif
            // Process results
#if T_TEST
            // T test
            int status=T_test_isRejected(timing_sample,sample_counter);
#else
            // Wilcoxon Rank-Sum Test
            int status=WilcoxonRankSumTest(timing_sample,sample_counter);
#endif
            if (status!=0)
            {
                j++; // retry counter
                if (j>=CCPAD_TIMING_RETRIES)
                {
                    // If it fails for len==0 it's a test issue since it is all random
                    if (len==0) {
                        diag("Constant timing FAILED for all random, this is a test issue",len,j);
                    }
                    else
                    {
                        diag("Constant timing FAILED for len %d after %d attempts",len,j);
                        //ok_or_fail((status==0),"Decrypt+padding constant timing");
                        failure_cnt++;
                    }
                    break;
                }
            }
            else
            {
                if ((verbose>1) && (j>0)) diag("Constant timing ok for len %d after %d attempts (of %d)",len,j+1,CCPAD_TIMING_RETRIES);
                break;
            }
        } // retry
    }
    early_abort=0;
errOut:
    free(plaintext);
    free(ciphertext_valid);
    free(ciphertext_random);
    free(output);
    free(key);
    free(init_iv);
    if (failure_cnt || early_abort)
    {
        return 0;
    }
    return 1;
}

static int
run_symmetric_pad_vectors(duplex_cryptor cryptor,ccpad_select padding_mode) {
    const ccpad_test_vector_t *run_vector=NULL;

    ok_or_fail(padding_mode==ccpad_pkcs7, "Padding mode supported");
    if (cc_cipherAES==cryptor->cipher)
    {
        run_vector=aes_vectors;
    }
    else if (cc_cipherDES==cryptor->cipher)
    {
        run_vector=des_vectors;
    }
    ok_or_fail(NULL!=run_vector, "Test not implemented");


    if (cc_ModeECB!=cryptor->mode)
    {   // Timing attack not relevant on ECB
        for (int i=0;i<CMP_TIMING_TEST_ITERATION;i++)
        ok_or_warning(ccsymmetric_crypt_pad_timing_tests(cryptor,padding_mode,&run_vector[0]), "Constant Time test");
    }
    for(int i=0; run_vector[i].keyStr != NULL; i++) {
        const ccpad_test_vector_t *test = &run_vector[i];
        ok_or_fail(ccsymmetric_pad_tests(cryptor,padding_mode,test), "Test Vector Passed");
    }

    return 1;
}

static int test_pad(ciphermode_t encrypt_ciphermode, ciphermode_t decrypt_ciphermode,cc_cipher_select cipher,cc_mode_select mode,                     ccpad_select padding_mode) {
    duplex_cryptor_s cryptor;
    cryptor.cipher = cipher;
    cryptor.mode = mode;
    cryptor.encrypt_ciphermode = encrypt_ciphermode;
    cryptor.decrypt_ciphermode = decrypt_ciphermode;
    ok_or_fail(run_symmetric_pad_vectors(&cryptor,padding_mode), "Symmetric padding test");
    return 1;
}

int ccpad(TM_UNUSED int argc, TM_UNUSED char *const *argv)
{
	plan_tests(kTestTestCount);
    if (verbose) diag("PKCS7 Padding - Default AES-ECB");
    ok(test_pad((ciphermode_t) ccaes_ecb_encrypt_mode(), (ciphermode_t) ccaes_ecb_decrypt_mode(),cc_cipherAES,cc_ModeECB,ccpad_pkcs7) == 1, "PKCS7 Padding - Default AES-ECB");
    if (verbose) diag("PKCS7 Padding - Default AES-CBC");
    ok(test_pad((ciphermode_t) ccaes_cbc_encrypt_mode(), (ciphermode_t) ccaes_cbc_decrypt_mode(),cc_cipherAES,cc_ModeCBC,ccpad_pkcs7) == 1, "PKCS7 Padding - Default AES-CBC");
    if (verbose) diag("PKCS7 Padding - Default DES-ECB");
    ok(test_pad((ciphermode_t) ccdes_ecb_encrypt_mode(), (ciphermode_t) ccdes_ecb_decrypt_mode(),cc_cipherDES,cc_ModeECB,ccpad_pkcs7) == 1, "PKCS7 Padding - Default DES-ECB");
    if (verbose) diag("PKCS7 Padding - Default DES-CBC");
    ok(test_pad((ciphermode_t) ccdes_cbc_encrypt_mode(), (ciphermode_t) ccdes_cbc_decrypt_mode(),cc_cipherDES,cc_ModeCBC,ccpad_pkcs7) == 1, "PKCS7 Padding - Default DES-CBC");
    return 0;
}
#endif

