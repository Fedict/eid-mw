/*
 * Copyright (c) 2012,2014,2015 Apple Inc. All rights reserved.
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


#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

#include "testmore.h"
#include "testenv.h"

#include <corecrypto/cc_debug.h>
#include <corecrypto/cc_macros.h>
#include <corecrypto/ccrng_test.h>
#include <corecrypto/ccrng_system.h>
#include "testByteBuffer.h"

static int
tests_summary(int verbose) {
    int failed_tests = 0;
    int todo_tests = 0;
    int actual_tests = 0;
    int planned_tests = 0;
    int warning_tests = 0;
    int duration_tests = 0;

    // First compute the totals to help decide if we need to print headers or not.
    for (int i = 0; testlist[i].name; ++i) {
        if (testlist[i].executed) {
            failed_tests += testlist[i].failed_tests;
            todo_tests += testlist[i].todo_tests;
            actual_tests += testlist[i].actual_tests;
            planned_tests += testlist[i].planned_tests;
            warning_tests += testlist[i].warning_tests;
            duration_tests += testlist[i].duration;
        }
    }

    fprintf(stdout, "\n[SUMMARY]\n");

    // -v makes the summary verbose as well.
    if (verbose || failed_tests || actual_tests != planned_tests || todo_tests || warning_tests) {
        fprintf(stdout, "Test name                                                failed  warning  todo  ran  planned\n");
        fprintf(stdout, "============================================================================================\n");
    }
    for (int i = 0; testlist[i].name; ++i) {
        if (testlist[i].executed) {
            const char *token = NULL;
            if (testlist[i].failed_tests) {
                token = "FAIL";
            } else if (testlist[i].actual_tests != testlist[i].planned_tests
                       || (testlist[i].todo_tests)
                       || (testlist[i].warning_tests)) {
                token = "WARN";
            } else if (verbose) {
                token = "PASS";
            }
            if (token) {
                fprintf(stdout, "[%s] %-49s %6d  %6d %6d %6d %6d\n", token,
                        testlist[i].name,
                        testlist[i].failed_tests, testlist[i].warning_tests,
                        testlist[i].todo_tests,
                        testlist[i].actual_tests, testlist[i].planned_tests);
            }
        }
    }
    if (verbose || failed_tests || warning_tests || todo_tests || actual_tests != planned_tests) {
        fprintf(stdout, "============================================================================================\n");
    }
    else {
        fprintf(stdout, "Test name                                                failed  warning  todo  ran  planned\n");
    }
    fprintf(stdout, "Totals (%6ds)                                         %6d  %6d %6d %6d %6d\n", duration_tests/1000, failed_tests, warning_tests, todo_tests, actual_tests, planned_tests);
    return failed_tests;
}

static void usage(const char *progname)
{
    fprintf(stderr, "usage: %s [-v][-s seed][-w][testname [testargs] ...]\n", progname);
    exit(1);
}

static int tests_run_index(int i, int argc, char * const *argv)
{
    int verbose = 0;
    int ch;

    while ((ch = getopt(argc, argv, "v")) != -1)
    {
        switch  (ch)
        {
            case 'v':
                verbose++;
                break;
            default:
                usage(argv[0]);
        }
    }

    fprintf(stderr, "\n[BEGIN] %s\n", testlist[i].name);
    
    run_one_test(&testlist[i], argc, argv);
    if(testlist[i].failed_tests) {
        fprintf(stderr, "[FAIL] %s\n", testlist[i].name);
    } else {
        fprintf(stderr, "duration: %llu ms\n", testlist[i].duration);
        fprintf(stderr, "[PASS] %s\n", testlist[i].name);
    }
    
    return 0;
}

static int tests_named_index(const char *testcase)
{
    int i;

    for (i = 0; testlist[i].name; ++i) {
        if (strcmp(testlist[i].name, testcase) == 0) {
            return i;
        }
    }

    return -1;
}

static int tests_run_all(int argc, char * const *argv)
{
    int curroptind = optind;
    int i;

    for (i = 0; testlist[i].name; ++i) {
        tests_run_index(i, argc, argv);
        optind = curroptind;
    }
    
    return 0;
}

static struct ccrng_test_state test_rng;
struct ccrng_state *global_test_rng=(struct ccrng_state *)&test_rng;


static int tests_rng(const char *seed) {
    byteBuffer entropyBuffer;
    int status=-1; // Allocation error;

    if (seed==NULL || strlen(seed)<=0) {
        // If the seed is not in the argument, we generate one
        struct ccrng_system_state system_rng;
        size_t entropy_size=16; // Default size of the seed
        cc_require((entropyBuffer=mallocByteBuffer(entropy_size))!=NULL,errOut);
        cc_require((status=ccrng_system_init(&system_rng))==0, errOut);
        cc_require((status=ccrng_generate((struct ccrng_state *)&system_rng, entropyBuffer->len, entropyBuffer->bytes))==0, errOut);
        ccrng_system_done(&system_rng);
        cc_print("random seed value:",entropyBuffer->len,entropyBuffer->bytes);
    } else {
        // Otherwise, take the value from the arguments
        entropyBuffer = hexStringToBytes(seed);
        cc_print("input seed value:",entropyBuffer->len,entropyBuffer->bytes);
    }
    cc_require((status=ccrng_test_init(&test_rng, entropyBuffer->len,entropyBuffer->bytes))==0, errOut);
    return status;
errOut:
    printf("Error initializing test rng: %d\n",status);
    return -1;
}

static int tests_init(char* seed) {
    printf("[TEST] === corecrypto ===\n");
    return tests_rng(seed);
}

static int tests_end(void) {
    ccrng_test_done(&test_rng);
    return 0;
}

int
tests_begin(int argc, char * const *argv)
{
    const char *testcase = NULL;
    bool initialized = false;
    int verbose=0;
    int testix = -1;
	int ch;
    int retval;
    char *seed=NULL;
    for (;;) {
        while (!testcase && (ch = getopt(argc, argv, "vws:")) != -1)
        {
            switch  (ch)
            {
            case 's':
                seed = optarg;
                break;
            case 'w':
                sleep(100);
                break;
            case 'v':
                verbose=1;
                break;
            case '?':
            default:
                printf("invalid option %c\n",ch); 
                usage(argv[0]);
            }
        }

        if (optind < argc) {
            testix = tests_named_index(argv[optind]);
            if(testix<0) {
                printf("invalid test %s\n",argv[optind]); 
                usage(argv[0]);
            }
            argc -= optind;
            argv += optind;
        }

        if (testix < 0) {
            if (!initialized) {
                //initialized = true;
                if (tests_init(seed)!=0) return -1;
                tests_run_all(argc, argv);
            }
            break;
        } else {
            if (!initialized) {
                if (tests_init(seed)!=0) return -1;
                initialized = true;
            }
            tests_run_index(testix, argc, argv);
            testix = -1;
        }
    }

    retval=tests_summary(verbose);

    /* Cleanups */
    tests_end();
    
    return retval;
}

