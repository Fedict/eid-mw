#ifndef EIDMW_TESTLIB_H
#define EIDMW_TESTLIB_H

#define TEST_SKIP 77;
#define TEST_FAIL 1;
#define TEST_OK 0;

#define check_ok verbose_assert(rv == CKR_OK)

void verify_null(CK_UTF8CHAR* string, size_t length, int nulls_expected, char* msg);

#endif
