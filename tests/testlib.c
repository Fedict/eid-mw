#include <unix.h>
#include <pkcs11.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

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

CK_BBOOL have_pin() {
	char* envvar;
	if(have_robot() && is_manual_robot()) {
		return CK_TRUE;
	}
	envvar = getenv("EID_PIN_CODE");
	if(envvar != NULL) {
		return CK_TRUE;
	}
	return CK_FALSE;
}

CK_BBOOL is_manual_robot() {
	return robot_type == ROBOT_MECHANICAL_TURK;
}

typedef struct {
	const char* rvname;
	int result;
} ckrvdecode;

#define ADD_CKRV(ckrv, defaultrv) decodes[ckrv].rvname = #ckrv; decodes[ckrv].result = defaultrv

int ckrv_decode(CK_RV rv, int count, ...) {
	va_list ap;
	ckrvdecode *decodes = calloc(CKR_FUNCTION_REJECTED + 1, sizeof(ckrvdecode));
	int i;
	int retval;

	printf("function %d: ", fc_counter++);

	ADD_CKRV(CKR_ARGUMENTS_BAD, TEST_RV_FAIL);
	ADD_CKRV(CKR_BUFFER_TOO_SMALL, TEST_RV_FAIL);
	ADD_CKRV(CKR_CANT_LOCK, TEST_RV_FAIL);
	ADD_CKRV(CKR_CRYPTOKI_ALREADY_INITIALIZED, TEST_RV_FAIL);
	ADD_CKRV(CKR_CRYPTOKI_NOT_INITIALIZED, TEST_RV_FAIL);
	ADD_CKRV(CKR_DEVICE_ERROR, TEST_RV_FAIL);
	ADD_CKRV(CKR_DEVICE_MEMORY, TEST_RV_FAIL);
	ADD_CKRV(CKR_DEVICE_REMOVED, TEST_RV_FAIL);
	ADD_CKRV(CKR_FUNCTION_CANCELED, TEST_RV_FAIL);
	ADD_CKRV(CKR_FUNCTION_FAILED, TEST_RV_FAIL);
	ADD_CKRV(CKR_FUNCTION_NOT_SUPPORTED, TEST_RV_SKIP);
	ADD_CKRV(CKR_GENERAL_ERROR, TEST_RV_FAIL);
	ADD_CKRV(CKR_HOST_MEMORY, TEST_RV_FAIL);
	ADD_CKRV(CKR_MECHANISM_INVALID, TEST_RV_FAIL);
	ADD_CKRV(CKR_NEED_TO_CREATE_THREADS, TEST_RV_FAIL);
	ADD_CKRV(CKR_NO_EVENT, TEST_RV_FAIL);
	ADD_CKRV(CKR_OK, TEST_RV_OK);
	ADD_CKRV(CKR_OPERATION_NOT_INITIALIZED, TEST_RV_FAIL);
	ADD_CKRV(CKR_PIN_INCORRECT, TEST_RV_FAIL);
	ADD_CKRV(CKR_PIN_LOCKED, TEST_RV_FAIL);
	ADD_CKRV(CKR_SESSION_CLOSED, TEST_RV_FAIL);
	ADD_CKRV(CKR_SESSION_COUNT, TEST_RV_FAIL);
	ADD_CKRV(CKR_SESSION_HANDLE_INVALID, TEST_RV_FAIL);
	ADD_CKRV(CKR_SESSION_PARALLEL_NOT_SUPPORTED, TEST_RV_FAIL);
	ADD_CKRV(CKR_SESSION_READ_WRITE_SO_EXISTS, TEST_RV_FAIL);
	ADD_CKRV(CKR_SESSION_READ_ONLY_EXISTS, TEST_RV_FAIL);
	ADD_CKRV(CKR_SLOT_ID_INVALID, TEST_RV_FAIL);
	ADD_CKRV(CKR_TOKEN_NOT_PRESENT, TEST_RV_FAIL);
	ADD_CKRV(CKR_TOKEN_NOT_RECOGNIZED, TEST_RV_FAIL);
	ADD_CKRV(CKR_TOKEN_WRITE_PROTECTED, TEST_RV_FAIL);
	ADD_CKRV(CKR_USER_ALREADY_LOGGED_IN, TEST_RV_FAIL);
	ADD_CKRV(CKR_USER_ANOTHER_ALREADY_LOGGED_IN, TEST_RV_FAIL);
	ADD_CKRV(CKR_USER_PIN_NOT_INITIALIZED, TEST_RV_FAIL);
	ADD_CKRV(CKR_USER_TOO_MANY_TYPES, TEST_RV_FAIL);
	ADD_CKRV(CKR_USER_TYPE_INVALID, TEST_RV_FAIL);

	va_start(ap, count);
	for(i=0; i<count; i++) {
		CK_RV modrv = va_arg(ap, CK_RV);
		int toreturn = va_arg(ap, int);
		assert(modrv <= CKR_FUNCTION_REJECTED);
		assert(decodes[modrv].rvname != NULL);
		decodes[modrv].result = toreturn;
	}
	va_end(ap);

	if(decodes[rv].rvname != NULL) {
		printf("%s\n", decodes[rv].rvname);
		retval = decodes[rv].result;
	} else {
		printf("unknown CK_RV 0x%08x\n", rv);
		retval = TEST_RV_FAIL;
	}
	free(decodes);
	return retval;
}

void robot_insert_card() {
	char c;
	switch(robot_type) {
		case ROBOT_NONE:
			fprintf(stderr, "E: robot needed, no robot configured\n");
			exit(EXIT_FAILURE);
		case ROBOT_AUTO:
			fprintf(stderr, "Auto robot not yet implemented\n");
			exit(TEST_RV_SKIP);
		case ROBOT_MECHANICAL_TURK:
			printf("Please insert a card and press <enter>\n");
			scanf("%c", &c);
	}
}

void robot_remove_card() {
	char c;
	switch(robot_type) {
		case ROBOT_NONE:
			fprintf(stderr, "E: robot needed, no robot configured\n");
			exit(EXIT_FAILURE);
		case ROBOT_AUTO:
			fprintf(stderr, "Auto robot not yet implemented\n");
			exit(TEST_RV_SKIP);
		case ROBOT_MECHANICAL_TURK:
			printf("Please remove the card from the slot and press <enter>\n");
			scanf("%c", &c);
	}
}

void robot_insert_reader() {
}

void robot_remove_reader() {
}

int init_tests() {
	int count = 0;
#if (TESTS_TO_RUN & TEST_INIT_FINALIZE)
	test_ptrs[0] = init_finalize;
	count++;
#endif
#if (TESTS_TO_RUN & TEST_GETINFO)
	test_ptrs[1] = getinfo;
	count++;
#endif
#if (TESTS_TO_RUN & TEST_FUNCLIST)
	test_ptrs[2] = funclist;
	count++;
#endif
#if (TESTS_TO_RUN & TEST_SLOTLIST)
	test_ptrs[3] = slotlist;
	count++;
#endif
#if (TESTS_TO_RUN & TEST_SLOTINFO)
	test_ptrs[4] = slotinfo;
	count++;
#endif
#if (TESTS_TO_RUN & TEST_TKINFO)
	test_ptrs[5] = tkinfo;
	count++;
#endif
#if (TESTS_TO_RUN & TEST_DOUBLE_INIT)
	test_ptrs[6] = double_init;
	count++;
#endif
#if (TESTS_TO_RUN & TEST_FORK_INIT)
	test_ptrs[7] = fork_init;
	count++;
#endif
#if (TESTS_TO_RUN & TEST_SLOTEVENT)
	test_ptrs[8] = slotevent;
	count++;
#endif
#if (TESTS_TO_RUN & TEST_MECHLIST)
	test_ptrs[9] = mechlist;
	count++;
#endif
#if (TESTS_TO_RUN & TEST_MECHINFO)
	test_ptrs[10] = mechinfo;
	count++;
#endif
#if (TESTS_TO_RUN & TEST_SESSIONS)
	test_ptrs[11] = sessions;
	count++;
#endif
#if (TESTS_TO_RUN & TEST_SESSIONS_NOCARD)
	test_ptrs[12] = sessions_nocard;
	count++;
#endif
#if (TESTS_TO_RUN & TEST_SESSIONINFO)
	test_ptrs[13] = sessioninfo;
	count++;
#endif
#if (TESTS_TO_RUN & TEST_LOGIN)
	test_ptrs[14] = login;
	count++;
#endif
	verbose_assert(count > 0);
	if(count==1) {
		return 1;
	} else {
		return 0;
	}
}

int find_slot(CK_BBOOL with_token, CK_SLOT_ID_PTR slot) {
	CK_RV rv;
	CK_ULONG count = 0;
	CK_SLOT_ID_PTR list;

	rv = C_GetSlotList(with_token, NULL_PTR, &count);
	assert(ckrv_decode(rv, 1, (CK_RV)CKR_BUFFER_TOO_SMALL, (int)TEST_RV_OK) == TEST_RV_OK);
	printf("slots %sfound: %lu\n", with_token ? "with token " : "", count);
	if(count == 0 && with_token) {
		printf("Need at least one token to run this test\n");
		return TEST_RV_SKIP;
	}

	do {
		list = malloc(sizeof(CK_SLOT_ID) * count);
	} while((rv = C_GetSlotList(with_token, list, &count) == CKR_BUFFER_TOO_SMALL));
	check_rv;

	if(count > 1) {
		printf("INFO: multiple slots found, using slot %lu\n", list[0]);
	}

	*slot = list[0];
	free(list);

	return TEST_RV_OK;
}
