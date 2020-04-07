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


#ifndef _TESTMORE_H_
#define _TESTMORE_H_  1

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <corecrypto/cc_debug.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TM_UNUSED __attribute__((unused))

/* This is included here, because its already included by all the test case */
#include "testlist.h"

/* rng to use for testing */
extern struct ccrng_state *global_test_rng;

typedef int (*one_test_entry)(int argc, char *const *argv);
    
#define ONE_TEST_ENTRY(x) int x(int argc, char *const *argv)
    
struct one_test_s {
    char *name;            /* test name. */
    one_test_entry entry;  /* entry point. */
    int sub_tests;         /* number of subtests. */
    int failed_tests;      /* number of failed tests. */
    int warning_tests;     /* number of tests raised a warning. */
    int todo_tests;        /* number of todo tests */
    int actual_tests;      /* number of tests attempted. */
    int planned_tests;     /* number of planned tests. */
    const char *plan_file; /* full path to file that called plan_tests() */
    int plan_line;         /* line number in plan_file at which plan_tests was called. */
    unsigned long long duration; /* test duration in msecs. */
    int executed;           /* whether the test was run */
    /* add more later: timing, etc... */
};

extern struct one_test_s testlist[];
    
int run_one_test(struct one_test_s *test, int argc, char * const *argv);

/* this test harnes rely on shadowing for TODO, SKIP and SETUP blocks */
#pragma GCC diagnostic ignored "-Wshadow"

#define diag_linereturn() fputs("\n", stderr);

#define ok(THIS, TESTNAME) \
    test_ok(!!(THIS), TESTNAME, test_directive, test_reason, \
		__FILE__, __LINE__, NULL)
#define is(THIS, THAT, TESTNAME) \
({ \
    __typeof__(THIS) _this = (THIS); \
    __typeof__(THAT) _that = (THAT); \
    test_ok((_this == _that), TESTNAME, test_directive, test_reason, \
		__FILE__, __LINE__, \
		"#          got: '%d'\n" \
		"#     expected: '%d'\n", \
		_this, _that); \
})
#define isnt(THIS, THAT, TESTNAME) \
	cmp_ok((THIS), !=, (THAT), (TESTNAME))
#define diag(MSG, ARGS...) \
	test_diag(test_directive, test_reason, __FILE__, __LINE__, MSG, ## ARGS)
#define cmp_ok(THIS, OP, THAT, TESTNAME) \
({ \
	__typeof__(THIS) _this = (THIS); \
	__typeof__(THAT) _that = (THAT); \
	test_ok((_this OP _that), TESTNAME, test_directive, test_reason, \
		__FILE__, __LINE__, \
	   "#     '%d'\n" \
	   "#         " #OP "\n" \
	   "#     '%d'\n", \
	   _this, _that); \
})
#define eq_string(THIS, THAT, TESTNAME) \
({ \
	const char *_this = (THIS); \
	const char *_that = (THAT); \
	test_ok(!strcmp(_this, _that), TESTNAME, test_directive, test_reason, \
		__FILE__, __LINE__, \
	   "#     '%s'\n" \
	   "#         eq\n" \
	   "#     '%s'\n", \
	   _this, _that); \
})
#define eq_stringn(THIS, THISLEN, THAT, THATLEN, TESTNAME) \
({ \
	__typeof__(THISLEN) _thislen = (THISLEN); \
	__typeof__(THATLEN) _thatlen = (THATLEN); \
	const char *_this = (THIS); \
	const char *_that = (THAT); \
	test_ok(_thislen == _thatlen && !strncmp(_this, _that, _thislen), \
		TESTNAME, test_directive, test_reason, \
		__FILE__, __LINE__, \
	   "#     '%.*s'\n" \
	   "#         eq\n" \
	   "#     '%.*s'\n", \
	   (int)_thislen, _this, (int)_thatlen, _that); \
})
#define like(THIS, REGEXP, TESTNAME) like_not_yet_implemented()
#define unlike(THIS, REGEXP, TESTNAME) unlike_not_yet_implemented()
#define is_deeply(STRUCT1, STRUCT2, TESTNAME) is_deeply_not_yet_implemented()
#define TODO switch(0) default
#define SKIP switch(0) default
#define SETUP switch(0) default
#define todo(REASON) const char *test_directive __attribute__((unused)) = "TODO", \
	*test_reason __attribute__((unused)) = (REASON)
#define skip(WHY, HOW_MANY, UNLESS) if (!(UNLESS)) \
    { test_skip((WHY), (HOW_MANY), 0); break; }
#define setup(REASON) const char *test_directive = "SETUP", \
	*test_reason = (REASON)
#define pass(TESTNAME) ok(1, (TESTNAME))
#define fail(TESTNAME) ok(0, (TESTNAME))
#define BAIL_OUT(WHY) test_bail_out(WHY, __FILE__, __LINE__)
#define plan_skip_all(REASON) test_plan_skip_all(REASON)
#define plan_tests(COUNT) test_plan_tests(COUNT, __FILE__, __LINE__)
    
#define ok_status(THIS, TESTNAME) \
({ \
	int _this = (THIS); \
	test_ok(!_this, TESTNAME, test_directive, test_reason, \
		__FILE__, __LINE__, \
	   "#     status: %s(%ld)\n", \
	   sec_errstr(_this), _this); \
})
    
#define ok_or_fail(THIS, TESTNAME) \
({ \
int _this = (THIS); \
test_ok(_this, TESTNAME, test_directive, test_reason, \
__FILE__, __LINE__, \
"#     status: %s(%ld)\n", \
sec_errstr(_this), _this); \
if(_this == 0) return 0; \
})

#define ok_or_warning(THIS, TESTNAME) \
({ \
int _this = (THIS); \
test_ok(_this, TESTNAME, "WARNING", test_reason, \
__FILE__, __LINE__, \
"#     status: %s(%ld)\n", \
sec_errstr(_this), _this); \
})
    
#define ok_or_goto(THIS, TESTNAME, LABEL) \
({ \
int _this = (THIS); \
test_ok(_this, TESTNAME, test_directive, test_reason, \
__FILE__, __LINE__, \
"#     status: %s(%ld)\n", \
sec_errstr(_this), _this); \
if(_this == 0) goto LABEL; \
})

    
#define ok_memcmp_or_fail(_P1_, _P2_, _LEN_, TESTNAME) \
({ \
int _this = (memcmp((_P1_), (_P2_), _LEN_) == 0); \
if(_this==0) {cc_print("Compare: ",_LEN_,(const uint8_t*)(_P1_));cc_print("    and: ",_LEN_,(const uint8_t*)(_P2_));} \
test_ok(_this, TESTNAME, test_directive, test_reason, \
__FILE__, __LINE__, \
"#     status: %s(%ld)\n", \
sec_errstr(_this), _this); \
if(_this == 0) return 0; \
})


#define is_status(THIS, THAT, TESTNAME) \
({ \
    OSStatus _this = (THIS); \
    OSStatus _that = (THAT); \
    test_ok(_this == _that, TESTNAME, test_directive, test_reason, \
		__FILE__, __LINE__, \
	   "#          got: %s(%ld)\n" \
	   "#     expected: %s(%ld)\n", \
	   sec_errstr(_this), _this, sec_errstr(_that), _that); \
})
#define ok_unix(THIS, TESTNAME) \
({ \
    int _this = (THIS) < 0 ? errno : 0; \
    test_ok(!_this, TESTNAME, test_directive, test_reason, \
		__FILE__, __LINE__, \
	   "#          got: %s(%d)\n", \
	   strerror(_this), _this); \
})
#define is_unix(THIS, THAT, TESTNAME) \
({ \
    int _result = (THIS); \
    int _this = _result < 0 ? errno : 0; \
    int _that = (THAT); \
    _that && _result < 0 \
	? test_ok(_this == _that, TESTNAME, test_directive, test_reason, \
		__FILE__, __LINE__, \
		"#          got: %s(%d)\n" \
		"#     expected: %s(%d)\n", \
		strerror(_this), _this, strerror(_that), _that) \
	: test_ok(_this == _that, TESTNAME, test_directive, test_reason, \
		__FILE__, __LINE__, \
		"#            got: %d\n" \
		"# expected errno: %s(%d)\n", \
		_result, strerror(_that), _that); \
})

#define entryPoint(testname,supportname) \
    int testname(TM_UNUSED int argc, TM_UNUSED char *const *argv) { \
    char prString[80];\
    sprintf(prString, "No %s Support in this release\n", supportname);\
    plan_tests(1); \
    diag(prString); \
    ok(1, prString); \
    return 0; \
}


extern const char *test_directive;
extern const char *test_reason;

void test_bail_out(const char *reason, const char *file, unsigned line);
int test_diag(const char *directive, const char *reason,
	const char *file, unsigned line, const char *fmt, ...);
int test_ok(int passed, const char *description, const char *directive,
	const char *reason, const char *file, unsigned line, const char *fmt, ...);
void test_plan_skip_all(const char *reason);
void test_plan_tests(int count, const char *file, unsigned line);
void test_skip(const char *reason, int how_many, int unless);

const char *sec_errstr(int err);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_TESTMORE_H_ */
