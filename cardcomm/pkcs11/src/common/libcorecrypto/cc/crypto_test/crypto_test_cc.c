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


#include <corecrypto/cc_priv.h>
#include "testmore.h"
#include "testbyteBuffer.h"

#define CC_SECURITY_TEST

#if (CC == 0)
entryPoint(cc,"cc")
#else

#ifdef CC_SECURITY_TEST
#import <corecrypto/ccrng_test.h>
#include "cccycles.h"
#include "ccstats.h"
#include "ccconstanttime.h"
#endif


// Disable the static analyzer for the code below since we do voluntary access to
// uninitialized memory area in stack

#ifdef __clang_analyzer__
int stack_clear_test(size_t size);
#endif

#ifndef __clang_analyzer__

#if defined(__has_feature) && __has_feature(address_sanitizer)
#define CC_NO_SANITIZE __attribute__((no_sanitize_address))
#else
#define CC_NO_SANITIZE
#endif // __has_feature

#define STACK_MAGIC 0xC0DEBA5E

CC_NO_SANITIZE static void
stack_dirty(size_t size)
{
    volatile uint32_t array[size];
    for (size_t i=0;i<size;i++)
    {
        array[i]=STACK_MAGIC;
    }
}

CC_NO_SANITIZE static void
stack_clear(size_t size)
{
    uint32_t array[size];
    cc_clear(sizeof(array),array);
}

CC_NO_SANITIZE static int
stack_test(size_t size)
{
    volatile uint32_t array[size];
    for (size_t i=0;i<size;i++)
    {
        if (array[i]==STACK_MAGIC)
        {
            return 1; //error stack was not cleared.
        }
    }
    return 0;
}

CC_NO_SANITIZE static int
stack_clear_test(size_t size)
{
    stack_dirty(size);
    stack_clear(size);
    return stack_test(size);
}

#endif  /* __clang_analyzer__ */
// Static analyzer re-enabled.

static void
Rotate_Tests(void) {
    int c=1;
    uint32_t result32=0xaaaaaaaa;
    uint64_t result64=0xaaaaaaaaaaaaaaaa;

    /* The first argument is NOT a variable on purpose */
    is(result32, CC_ROL(0x55555555, c), "CC_ROL 1");

    is(result32, CC_ROLc(0x55555555, 1), "CC_ROLc 1");

    is(result64, CC_ROL64(0x5555555555555555, c), "CC_ROL64 1");

    is(result64, CC_ROL64c(0x5555555555555555, 1), "CC_ROL64c 1");

    is(result32, CC_ROR(0x55555555, c), "CC_ROR 1");

    is(result32, CC_RORc(0x55555555, 1), "CC_RORc 1");

    is(result64, CC_ROR64(0x5555555555555555, c), "CC_ROR64 1");

    is(result64, CC_ROR64c(0x5555555555555555, 1), "CC_ROR64c 1");
}

static void
HEAVISIDE_STEP_Tests(void)
{
    typedef uint8_t uint24_t[3];
    uint8_t i8;
    uint16_t i16;
    uint32_t i32;
    uint64_t i64;
    size_t i; // loop index
    uint8_t err=0,nb_test=0;

    // Sanity check on intended lengths
    ok(sizeof(uint8_t) == 1, "sizeof(uint8_t)!=1");
    ok(sizeof(uint16_t) == 2, "sizeof(uint16_t)!=2");
    ok(sizeof(uint32_t) == 4, "sizeof(uint32_t)!=4");
    ok(sizeof(uint64_t) == 8, "sizeof(uint64_t)!=1");

    for (i=0;i<8*sizeof(i8);i++)
    {
        nb_test++;
        i8=((uint8_t)1<<i);
        CC_HEAVISIDE_STEP(i8);
        if (i8!=1) err++;
    }
    ok(err==0,"CC_HEAVISIDE_STEP(i8)");

    for (i=0;i<8*sizeof(i16);i++)
    {
        nb_test++;
        i16=((uint16_t)1<<i);
        CC_HEAVISIDE_STEP(i16);
        if (i16!=1) err++;
    }
    ok(err==0,"CC_HEAVISIDE_STEP(i16)");

    for (i=0;i<8*sizeof(i32);i++)
    {
        nb_test++;
        i32=((uint32_t)1<<i);
        CC_HEAVISIDE_STEP(i32);
        if (i32!=1) err++;
    }
    ok(err==0,"CC_HEAVISIDE_STEP(i32)");

    for (i=0;i<8*sizeof(i64);i++)
    {
        nb_test++;
        i64=((uint64_t)1<<i);
        CC_HEAVISIDE_STEP(i64);
        if (i64!=1) err++;
    }
    ok(err==0,"CC_HEAVISIDE_STEP(i64)");

    ok(err + (64+32+16+8)-nb_test==0, "CC HEAVISIDE_STEP test failed");
}

static void
cmp_secure_functionalTests(void) {
#define ARRAY_SIZE 10

    // --- Bytes
    uint8_t array1[ARRAY_SIZE]={1,2,3,4,5,6,7,8,9,0};
    uint8_t array2[ARRAY_SIZE];

    CC_MEMCPY(array2,array1,sizeof(array1));
    // Equal
    ok(cc_cmp_safe(sizeof(array1), array1,array2)==0, "array1 to array2");
    ok(cc_cmp_safe(sizeof(array1), array2,array1)==0, "array2 to array1");

    // length is zero
    ok(cc_cmp_safe(0, array2,array1)!=0, "Array of size 0");

    // Equal but first byte
    array1[0]++;
    ok(cc_cmp_safe(sizeof(array1), array1,array2)!=0, "first byte");
    array1[0]--;

    // Equal but last byte
    array1[sizeof(array1)-1]++;
    ok(cc_cmp_safe(sizeof(array1), array1,array2)!=0, "last byte");
    array1[sizeof(array1)-1]--;

    // --- cc_units
    uint64_t u64_array1[ARRAY_SIZE]={};
    for (size_t i=0;i<ARRAY_SIZE;i++) u64_array1[i]=i;
    uint64_t u64_array2[ARRAY_SIZE];
    uint64_t tmp;

    CC_MEMCPY(u64_array2,u64_array1,sizeof(u64_array1));
    // Equal
    ok(cc_cmp_safe(sizeof(u64_array1), u64_array1,u64_array2)==0, "array1 to array2");
    ok(cc_cmp_safe(sizeof(u64_array1), u64_array2,u64_array1)==0, "array2 to array1");

    // length is zero
    ok(cc_cmp_safe(0, u64_array2,u64_array1)!=0, "Array of size 0");

    // Equal but first byte
    ((uint8_t *)u64_array1)[0]++;
    ok(cc_cmp_safe(sizeof(u64_array1),u64_array1,u64_array2)!=0, "first byte");
    ((uint8_t *)u64_array1)[0]--;

    // Equal but last byte
    CC_LOAD64_BE(tmp,&u64_array1[ARRAY_SIZE-1]);
    CC_STORE64_BE(tmp^0x80,&u64_array1[ARRAY_SIZE-1]);
    ok(cc_cmp_safe(sizeof(u64_array1), u64_array1,u64_array2)!=0, "last byte");
    CC_STORE64_BE(tmp,&u64_array1[ARRAY_SIZE-1]);
}

#ifdef CC_SECURITY_TEST

//======================================================================
// Constant time verification parameters
//======================================================================

// Number of iteration of test where timings are not taken into account.
// Made to reach a stable performance state
#define CC_WARMUP        20

// Each sample is the average time for many iteration with identical inputs
#define CC_TIMING_REPEAT  200

// Number of sample for the statistical analysis
// typically 100~1000 is a good range
#define CC_TIMING_SAMPLES 200

// In case of failure, try many times
// This is to reduce false positives due to noise/timing accuracy.
// If implementation is not constant time, the behavior will be consistent
// So that this does not reduce the detection power.
#define CC_TIMING_RETRIES 10

// Two statitical tools are available: T-test and Wilcoxon.
// T-test assumes that the distribution to be compared are normal
// Wilcoxon measure offset between distribution.
// Due to potential switches between performance state or occasional
// latencies, Wilcoxon is recommended.
// > Set to 1 to use T-test instead of Wilcoxon
#define T_TEST  1

// Number of iteration of the full test (to play with to evaluate chances of false positives)
#define CMP_SECURITY_TEST_ITERATION 1

// Quantile for the repeated timing. Empirical value.
#define CC_TIMING_PERCENTILE 9

//======================================================================

static const int verbose=1;

#define TEST_LAST_BYTE 1
#define TEST_FIRST_BYTE 2
#define TEST_RANDOM 3
#define TEST_EQUAL 4

static int
cmp_secure_timeconstantTests(size_t length, struct ccrng_state *rng, uint32_t test_id) {

    // Random for messages
    uint8_t array1[length];
    uint8_t array2[length];
    int failure_cnt=0;
    int early_abort=1;
    uint32_t j,sample_counter;
    bool retry=true;

    j=0;
    while(retry)
    {
        sample_counter=0; // Index of current sample
        measurement_t timing_sample[2*CC_TIMING_SAMPLES];

        for (size_t i=0;i<2*CC_TIMING_SAMPLES+(CC_WARMUP/CC_TIMING_REPEAT);i++)
        {
            ccrng_generate(rng,length,array1);
            volatile int cmp_result;
            if ((i&1) == 0)
            {
                // -------------------------
                //      Random
                // -------------------------
                switch(test_id) {
                    // All equal, except last byte
                    case TEST_LAST_BYTE:
                        memcpy(array2,array1,length);
                        array2[length-1]^=1;
                        break;
                    // All equal, except first byte
                    case TEST_FIRST_BYTE:
                        memcpy(array2,array1,length);
                        array2[0]^=1;
                        break;
                    // Random
                    case TEST_RANDOM:
                        ccrng_generate(rng,length,array2);
                        break;
                    // All equal
                    case TEST_EQUAL:
                        memcpy(array2,array1,length);
                        break;
                    default:
                        return 0; // failure
                }

            }
            else
            {
                // -------------------------
                //      Equal
                // -------------------------
                memcpy(array2,array1,length);
            }
#if 1
            // Actual function to test
            TIMING_WITH_QUANTILE(timing_sample[sample_counter].timing,
                                 CC_TIMING_REPEAT,
                                 CC_TIMING_PERCENTILE,
                                 cmp_result=cc_cmp_safe(length, array1, array2),errOut);
#else
            // Reference which can be expected to fail
            TIMING_WITH_QUANTILE(timing_sample[sample_counter].timing,
                                 CC_TIMING_REPEAT,
                                 CC_TIMING_PERCENTILE,
                                 cmp_result=memcmp(array1, array2,length),errOut);
#endif
            timing_sample[sample_counter].group=sample_counter&1;
#if CC_WARMUP
            if (i>=CC_WARMUP/CC_TIMING_REPEAT)
#endif
            {
                sample_counter++;
            }
        }
#if CCN_OSX
        if (verbose>1) {
            char file_name[64];
            snprintf(file_name,sizeof(file_name),"corecrypto_test_cc_cmp_timings_%.2zu.csv",length);
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
            if (j>=CC_TIMING_RETRIES)
            {
                diag("Constant timing FAILED for len %d after %d attempts",length,j);
                //ok_or_fail((status==0),"Decrypt+padding constant timing");
                failure_cnt++;
                break;
            }
        }
        else
        {
            if ((verbose>1) && (j>0)) diag("Constant timing ok for len %d after %d attempts (of %d)",length,j+1,CC_TIMING_RETRIES);
            break;
        }
    } // retry

    early_abort=0;
errOut:
    if (failure_cnt || early_abort)
    {
        return 0;
    }
    return 1;
}

#define CMP_SECURITY_TEST_MAX_LENGTH 2048
static void
memcmp_secure_securityTests(void) {

    // Random for messages
    struct ccrng_state *rng = global_test_rng;
    for (size_t i=0;i<CMP_SECURITY_TEST_ITERATION;i++)
    {
        size_t r;
        ccrng_generate(rng,sizeof(r),&r);
        r=r%CMP_SECURITY_TEST_MAX_LENGTH;
        ok(cmp_secure_timeconstantTests(r,rng,TEST_FIRST_BYTE), "Time constant check, first byte difference");
        ok(cmp_secure_timeconstantTests(r,rng,TEST_LAST_BYTE), "Time constant check, last byte difference");
        ok(cmp_secure_timeconstantTests(r,rng,TEST_RANDOM), "Time constant check, random");
        ok(cmp_secure_timeconstantTests(r,rng,TEST_EQUAL), "Time constant check of equal input - if it fails, it's a test issue");
    }
}
#endif // CC_SECURITY_TEST

int cc(TM_UNUSED int argc, TM_UNUSED char *const *argv)
{

#ifdef CC_SECURITY_TEST
    plan_tests(32);
#else
    plan_tests(27);
#endif

    if(verbose) diag("Stack cleanup");
    ok(stack_clear_test(100)==0, "Stack clearing");

    if(verbose) diag("HEAVISIDE_STEP test");
    HEAVISIDE_STEP_Tests();

    if(verbose) diag("Rotate test");
    Rotate_Tests();

    if(verbose) diag("Secure comparison test");
    cmp_secure_functionalTests();

#ifdef CC_SECURITY_TEST
    if(verbose) diag("Secure comparison security test");
    memcmp_secure_securityTests();
#endif // CC_SECURITY_TEST

    return 0;
}


#endif //CC
