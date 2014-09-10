#ifndef EIDMW_TESTLIB_H
#define EIDMW_TESTLIB_H

#include <assert.h>

#define TEST_RV_SKIP 77 // defined by automake
#define TEST_RV_FAIL 1
#define TEST_RV_OK 0

extern int va_counter;
extern int fc_counter;

#define verbose_assert(a) { printf("assertion %d: \"%s\": ", va_counter++, #a); assert((a)); printf("ok\n"); }

#ifdef __GNUC__
#define EIDT_LIKELY(expr) __builtin_expect((expr), 1)
#define EIDT_UNLIKELY(expr) __builtin_expect((expr), 0)
#else
#define EIDT_LIKELY(expr) (expr)
#define EIDT_UNLIKELY(expr) (expr)
#endif

typedef struct {
	CK_RV rv;
	int retval;
} ckrv_mod;

#define check_rv_late(func) { int retval = ckrv_decode(rv, func, 0, NULL); if(EIDT_UNLIKELY(retval != TEST_RV_OK)) { printf("not ok, returning\n"); return retval; }}
#define check_rv(call) check_rv_action(call, 0, NULL)
#define check_rv_action(call, count, mods) { CK_RV rv = call; int retval = ckrv_decode(rv, #call, count, mods); if(EIDT_UNLIKELY(retval != TEST_RV_OK)) { printf("not ok, returning\n"); return retval; }}
#define check_rv_long(call, mods) { int c = sizeof(mods) / sizeof(ckrv_mod); check_rv_action(call, c, mods); }

int ckrv_decode(CK_RV rv, char* fc, int count, ckrv_mod*);

char* ckm_to_charp(CK_MECHANISM_TYPE);

#ifdef HAVE_CONFIG_H
#define TEST_FUNC(a) int main(void)
#else
#define TEST_FUNC(a) int a(void)
#endif

/* Verifies that a string does not contain a NULL character */
void verify_null(CK_UTF8CHAR* string, size_t length, int nulls_expected, char* msg);

/* Functions to work with card moving robots */
CK_BBOOL have_robot();
CK_BBOOL is_manual_robot();
CK_BBOOL can_confirm();
CK_BBOOL have_pin();
CK_BBOOL can_enter_pin();
void robot_remove_card();
void robot_insert_card();
void robot_remove_reader();
void robot_insert_reader();
void hex_dump(char* data, CK_ULONG length);

/* Helper functions to not have to repeat common operations all the time */
int find_slot(CK_BBOOL with_token, CK_SLOT_ID_PTR slot);

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
#define TEST_SESSIONS		1 << 11
#define TEST_SESSIONS_NOCARD	1 << 12
#define TEST_SESSIONINFO	1 << 13
#define TEST_LOGIN		1 << 14
#define TEST_NONSENSIBLE	1 << 15
#define TEST_OBJECTS		1 << 16
#define TEST_READDATA		1 << 17
#define TEST_DIGEST		1 << 18
#define TEST_DECODEPHOTO	1 << 19

#define TESTS_COUNT		20

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
int sessions();
int sessions_nocard();
int sessioninfo();
int login();
int nonsensible();
int objects();
int readdata();
int digest();
int decode_photo();

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
