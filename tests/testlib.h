#ifndef EIDMW_TESTLIB_H
#define EIDMW_TESTLIB_H

#include <assert.h>

#define TEST_SKIP 77;
#define TEST_FAIL 1;
#define TEST_OK 0;

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

#endif
