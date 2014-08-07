#ifndef EIDMW_TESTLIB_H
#define EIDMW_TESTLIB_H

#include <assert.h>

#define TEST_RV_SKIP 77 // defined by automake
#define TEST_RV_FAIL 1
#define TEST_RV_OK 0

extern int va_counter;
extern int fc_counter;

#define verbose_assert(a) { printf("assertion %d: ", va_counter++); assert((a)); printf("ok\n"); }

#ifdef __GNUC__
#define EIDT_LIKELY(expr) __builtin_expect((expr), 1)
#define EIDT_UNLIKELY(expr) __builtin_expect((expr), 0)
#else
#define EIDT_LIKELY(expr) (expr)
#define EIDT_UNLIKELY(expr) (expr)
#endif

#define check_rv { printf("function call %d: ", fc_counter++); if(EIDT_LIKELY(rv == CKR_OK)) printf("ok\n"); else return ckrv_decode(rv); }

int ckrv_decode(CK_RV rv);

/* Verifies that a string does not contain a NULL character */
void verify_null(CK_UTF8CHAR* string, size_t length, int nulls_expected, char* msg);

/* Functions to work with card moving robots */
CK_BBOOL have_robot();
void robot_remove_card();
void robot_insert_card();
void robot_remove_reader();
void robot_insert_reader();

typedef unsigned int testmask;

/* masks for tests that exist */
#define TEST_INIT_FINALIZE	1 << 0
#define TEST_GETINFO 		1 << 1
#define TEST_FUNCLIST		1 << 2
#define TEST_SLOTLIST		1 << 3
#define TEST_SLOTINFO		1 << 4
#define TEST_TKINFO		1 << 5
#define TEST_DOUBLE_INIT	1 << 6
#define TEST_FORK_INIT		1 << 7
#define TEST_SLOTEVENT		1 << 8
#define TEST_MECHLIST		1 << 9
#define TEST_MECHINFO		1 << 10

#define TESTS_COUNT		11

#define TESTS_ALL		((testmask)1 << TEST_COUNT) - 1
#define FIRST_TEST		TEST_INIT_FINALIZE
/* function definitions for tests that exist */
int init_finalize();
int getinfo();
int funclist();
int slotlist();
int slotinfo();
int tkinfo();
int double_init();
int fork_init();
int slotevent();
int mechlist();
int mechinfo();

/* Define the tests to run for architectures where tests aren't run in individual programs */
#ifndef TESTS_TO_RUN
#define TESTS_TO_RUN TESTS_ALL
#endif

/* Array of function pointers to the above */
typedef int(*testptr_t)();
testptr_t test_ptrs[TESTS_COUNT];

/* initialize the above array */
int init_tests();

/* Check whether a test should be run */
#define should_run_test(mask, which) (mask & (1 << which))

#endif
