#include <unix.h>
#include <pkcs11.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include "testlib.h"

int va_counter;
int fc_counter;

enum {
	ROBOT_NONE,
	ROBOT_AUTO,
	ROBOT_MECHANICAL_TURK,
} robot_type;

void verify_null(CK_UTF8CHAR* string, size_t length, int expect, char* msg) {
	int nullCount = 0;
	char* buf = malloc(length + 1);
	int i;
	for(i=0; i<length; i++) {
		if(string[i] == '\0') {
			nullCount++;
		}
	}
	verbose_assert(nullCount == expect);

	strncpy(buf, string, length + 1);
	buf[length] = '\0';
	printf(msg, buf);
}

CK_BBOOL have_robot() {
	char* envvar = getenv("EID_ROBOT_STYLE");
	if(envvar == NULL) {
		robot_type = ROBOT_NONE;
		return CK_FALSE;
	}
	if(!strcmp(envvar, "auto")) {
		robot_type = ROBOT_AUTO;
		return CK_TRUE;
	}
	if(!strcmp(envvar, "manual")) {
		robot_type = ROBOT_MECHANICAL_TURK;
		return CK_TRUE;
	}
}

int ckrv_decode(CK_RV rv) {
	switch(rv) {
	case CKR_ARGUMENTS_BAD:
		printf("arguments bad\n");
		return TEST_FAIL;
	case CKR_BUFFER_TOO_SMALL:
		printf("buffer too small\n");
		return TEST_FAIL;
	case CKR_CANT_LOCK:
		printf("can't lock\n");
		return TEST_FAIL;
	case CKR_CRYPTOKI_ALREADY_INITIALIZED:
		printf("already initialized\n");
		return TEST_FAIL;
	case CKR_CRYPTOKI_NOT_INITIALIZED:
		printf("not initialized\n");
		return TEST_FAIL;
	case CKR_DEVICE_ERROR:
		printf("device error\n");
		return TEST_FAIL;
	case CKR_DEVICE_MEMORY:
		printf("device memory\n");
		return TEST_FAIL;
	case CKR_DEVICE_REMOVED:
		printf("device removed\n");
		return TEST_FAIL;
	case CKR_FUNCTION_FAILED:
		printf("function failed\n");
		return TEST_FAIL;
	case CKR_FUNCTION_NOT_SUPPORTED:
		printf("function not supported\n");
		return TEST_SKIP;
	case CKR_GENERAL_ERROR:
		printf("general error\n");
		return TEST_FAIL;
	case CKR_HOST_MEMORY:
		printf("host memory\n");
		return TEST_FAIL;
	case CKR_NEED_TO_CREATE_THREADS:
		printf("need to create tests\n");
		return TEST_FAIL;
	case CKR_NO_EVENT:
		printf("no event\n");
		return TEST_FAIL;
	case CKR_OK:
		printf("ok\n");
		return TEST_OK;
	case CKR_SLOT_ID_INVALID:
		printf("slot id invalid\n");
		return TEST_FAIL;
	case CKR_TOKEN_NOT_PRESENT:
		printf("token not present\n");
		return TEST_FAIL;
	case CKR_TOKEN_NOT_RECOGNIZED:
		printf("token not recognized\n");
		return TEST_FAIL;
	default:
		printf("unknown return code found: %d\n", rv);
		return TEST_FAIL;
	}
}

void robot_insert_card() {
}

void robot_remove_card() {
}

void robot_insert_reader() {
}

void robot_remove_reader() {
}
