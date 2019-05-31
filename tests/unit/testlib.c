/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2015 FedICT.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */
//#define _CRT_SECURE_NO_WARNINGS
#ifdef WIN32
#include <win32.h>
#include <stdio.h>
#include <stdint.h>
#include <tchar.h>
#else
#include <unix.h>
#include <unistd.h>
#endif
#include <pkcs11.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <errno.h>

#include "testlib.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef WIN32
char *strndup(const char *s, size_t n) {
	char *rv = calloc(n + 1, 1);
	strncpy(rv, s, n);
	return rv;
}
#endif

int va_counter;
int fc_counter;
#ifdef WIN32
extern _TCHAR* eid_robot_style;
extern _TCHAR* eid_dialogs_style;
extern _TCHAR* eid_builtin_reader;
#endif

struct banners {
	char* banner;
	reading_pos pos;
};

static struct banners banners[] = {
	{ "VELLEMAN_SHIELD_KIT\t800 STEPS\t60 RPM", VELLEMAN_FOUND },
	{ "system ready", SYSTEM_FOUND },
	{ "READY.", SYSTEM_FOUND },
};

struct rpos skip_uninteresting(char* line) {
	size_t len;
	int i;
	char *firstline = strdup(line);
	bool finished = false;
	struct rpos rv = {NOTHING_FOUND, 0, false};

	while(!finished) {
		if(rv.offset == strlen(line)) {
			finished = true;
			continue;
		}
		if((len = strcspn(line + rv.offset, "\r\n")) != strlen(line + rv.offset)) {
			if(len == 0) {
				rv.offset++;
				continue;
			} else {
				free(firstline);
				firstline = strndup(line + rv.offset, len);
			}
		}
		if(!finished) {
			finished = true;
			for(i=0; i<sizeof(banners) / sizeof(banners[0]); i++) {
				char *pos = strstr(banners[i].banner, firstline);
				if(pos != NULL) {
					rv.offset += len;
					rv.pos = banners[i].pos;
					finished = false;
					if(strlen(pos) == strlen(firstline)) {
						rv.is_complete = true;
					} else {
						rv.is_complete = false;
					}
				}
			}
		}
	}

	free(firstline);
	return rv;
}

enum robot_type robot_type;
enum dialogs_type dialogs_type;

bool have_builtin_reader = false;

int verify_null_func(CK_UTF8CHAR* string, size_t length, int expect, char* msg) {
	int nullCount = 0;
	char* buf = (char*)malloc(length + 1);
	unsigned int i;
	for(i=0; i<length; i++) {
		if(string[i] == '\0') {
			nullCount++;
		}
	}
	printf("nullCount: %d; expect: %d\n", nullCount, expect);
#ifdef WIN32
	strncpy_s(buf,  (size_t)(length + 1),(const char*)string, length);
#else
	strncpy(buf, (char*)string, length + 1);
#endif
	buf[length] = '\0';
	printf(msg, buf);
	free(buf);
	verbose_assert(nullCount == expect);
    
	return TEST_RV_OK;
}

CK_BBOOL have_robot() {
#ifdef WIN32
	char* envvar = eid_robot_style;
#else
	char* envvar = getenv("EID_ROBOT_STYLE");
#endif
	if(envvar == NULL) {
		robot_type = ROBOT_NONE;
		return CK_FALSE;
	}
	if(!strncmp(envvar, "fedict", strlen("fedict"))) {
		robot_type = ROBOT_AUTO;
		return open_robot(envvar);
	}
	if(!strncmp(envvar, "zetes", strlen("zetes"))) {
		robot_type = ROBOT_AUTO_2;
		return open_robot(envvar);
	}
	if(!strcmp(envvar, "manual")) {
		robot_type = ROBOT_MECHANICAL_TURK;
		return CK_TRUE;
	}

	return CK_FALSE;
}

CK_BBOOL have_reader_robot(void) {
#ifdef WIN32
	char* envvar_rbt = eid_robot_style;
	char* envvar_rdr = eid_builtin_reader;
#else
	char* envvar_rbt = getenv("EID_ROBOT_STYLE");
	char* envvar_rdr = getenv("EID_BUILTIN_READER");
#endif

	if(!have_robot()) {
		return CK_FALSE;
	}
	if(envvar_rdr == NULL || strlen(envvar_rdr) == 0) {
		have_builtin_reader = false;
		switch(robot_type) {
			case ROBOT_NONE:
			case ROBOT_AUTO:
				return CK_FALSE;
			case ROBOT_MECHANICAL_TURK:
				return CK_TRUE;
			case ROBOT_AUTO_2:
				return open_reader_robot(envvar_rbt);
		}
	}
	return CK_FALSE;
}

CK_BBOOL want_dialogs() {
#ifdef WIN32
	char* envvar = eid_dialogs_style;
#else
	char* envvar = getenv("EID_DIALOGS_STYLE");
#endif
#ifdef NO_DIALOGS
	dialogs_type = DIALOGS_NOPIN;
#else
	dialogs_type = DIALOGS_AVOID;
#endif

	if(envvar == NULL) {
		return CK_FALSE;
	}
	if(!strncmp(envvar, "nopin", strlen("nopin"))) {
		dialogs_type = DIALOGS_NOPIN;
	}
	if(!strncmp(envvar, "ok", strlen("ok"))) {
		dialogs_type = DIALOGS_OK;
	}
	if(dialogs_type == DIALOGS_AVOID) {
		return CK_FALSE;
	}

	return CK_TRUE;
}

CK_BBOOL have_pin() {
#ifdef WIN32
	return CK_FALSE;
#else
	if(want_dialogs() && dialogs_type > DIALOGS_NOPIN) {
		return CK_TRUE;
	}
	return CK_FALSE;
#endif
}

CK_BBOOL can_enter_pin(CK_SLOT_ID slot) {
	CK_SESSION_HANDLE session;
	CK_SESSION_INFO info;
	CK_BBOOL retval = CK_TRUE;

	if(!have_pin()) {
		return CK_FALSE;
	}
	if(have_robot() && !is_manual_robot()) {
		if(C_OpenSession(slot, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session) != CKR_OK) {
			printf("Could not open a session\n");
			return CK_FALSE;
		}
		if(C_GetSessionInfo(session, &info) != CKR_OK) {
			printf("Could not request session info\n");
			retval = CK_FALSE;
		}
		if(info.flags & CKF_PROTECTED_AUTHENTICATION_PATH) {
			fprintf(stderr, "E: robot cannot enter a pin code on a protected auth path SC reader\n");
			retval = CK_FALSE;
		}
		C_CloseSession(session);
	} else {
		retval = CK_TRUE;
	}
	return retval;
}

CK_BBOOL is_manual_robot() {
	return robot_type == ROBOT_MECHANICAL_TURK;
}

CK_BBOOL can_confirm() {
	if(want_dialogs()) {
		return CK_TRUE;
	}
	return CK_FALSE;
}

typedef struct {
	const char* rvname;
	int result;
} ckrvdecode;

#define ADD_CKRV(ckrv, defaultrv) decodes[ckrv].rvname = #ckrv; decodes[ckrv].result = defaultrv

int ckrv_decode(CK_RV rv, char* fc, int count, const ckrv_mod* mods) {
	static ckrvdecode *decodes;
	int i;
	int retval;

	printf("function %d: \"%s\": ", fc_counter++, fc);

	if(!decodes) {
		decodes = (ckrvdecode*)calloc(CKR_FUNCTION_REJECTED + 1, sizeof(ckrvdecode));
		ADD_CKRV(CKR_ARGUMENTS_BAD, TEST_RV_FAIL);
		ADD_CKRV(CKR_ATTRIBUTE_TYPE_INVALID, TEST_RV_FAIL);
		ADD_CKRV(CKR_ATTRIBUTE_VALUE_INVALID, TEST_RV_FAIL);
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
		ADD_CKRV(CKR_KEY_HANDLE_INVALID, TEST_RV_FAIL);
		ADD_CKRV(CKR_KEY_TYPE_INCONSISTENT, TEST_RV_FAIL);
		ADD_CKRV(CKR_MECHANISM_INVALID, TEST_RV_FAIL);
		ADD_CKRV(CKR_NEED_TO_CREATE_THREADS, TEST_RV_FAIL);
		ADD_CKRV(CKR_NO_EVENT, TEST_RV_FAIL);
		ADD_CKRV(CKR_OK, TEST_RV_OK);
		ADD_CKRV(CKR_OPERATION_NOT_INITIALIZED, TEST_RV_FAIL);
		ADD_CKRV(CKR_OPERATION_ACTIVE, TEST_RV_FAIL);
		ADD_CKRV(CKR_PIN_EXPIRED, TEST_RV_FAIL);
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
		ADD_CKRV(CKR_USER_NOT_LOGGED_IN, TEST_RV_FAIL);
		ADD_CKRV(CKR_KEY_FUNCTION_NOT_PERMITTED, TEST_RV_FAIL);
		ADD_CKRV(CKR_OBJECT_HANDLE_INVALID, TEST_RV_FAIL);
	}
	
	int toreturn = -1;

	for(i=0; i<count; i++) {
		CK_RV modrv = mods[i].rv;

		assert(modrv <= CKR_FUNCTION_REJECTED);
		assert(decodes[modrv].rvname != NULL);
		if(modrv == rv) {
			toreturn = mods[i].retval;
		}
	}

	if(decodes[rv].rvname != NULL) {
		printf("%s\n", decodes[rv].rvname);
		retval = toreturn > -1 ? toreturn : decodes[rv].result;
	} else {
		printf("unknown CK_RV %#08lx\n", rv);
		retval = TEST_RV_FAIL;
	}
	return retval;
}

#define CKM_PRINT(ckm) case ckm: return #ckm;
char* ckm_to_charp(CK_MECHANISM_TYPE mech) {
	switch(mech) {
		CKM_PRINT(CKM_RSA_PKCS);
		CKM_PRINT(CKM_RIPEMD160);
		CKM_PRINT(CKM_MD5);
		CKM_PRINT(CKM_SHA_1);
		CKM_PRINT(CKM_SHA256);
		CKM_PRINT(CKM_SHA384);
		CKM_PRINT(CKM_SHA512);
		CKM_PRINT(CKM_RIPEMD160_RSA_PKCS);
		CKM_PRINT(CKM_MD5_RSA_PKCS);
		CKM_PRINT(CKM_SHA1_RSA_PKCS);
		CKM_PRINT(CKM_SHA1_RSA_PKCS_PSS);
		CKM_PRINT(CKM_SHA256_RSA_PKCS);
		CKM_PRINT(CKM_SHA256_RSA_PKCS_PSS);
		CKM_PRINT(CKM_SHA384_RSA_PKCS);
		CKM_PRINT(CKM_SHA512_RSA_PKCS);
		CKM_PRINT(CKM_ECDSA_SHA256);
		CKM_PRINT(CKM_ECDSA_SHA384);
		CKM_PRINT(CKM_ECDSA_SHA512);
		CKM_PRINT(CKM_ECDSA);
		default:
			return "Unknown mechanism";
	}
}

void robot_insert_card() {
	char buf[80];
	switch(robot_type) {
		case ROBOT_NONE:
			fprintf(stderr, "E: robot needed, no robot configured\n");
			exit(EXIT_FAILURE);
		case ROBOT_AUTO:
		case ROBOT_AUTO_2:
			robot_cmd('i', CK_TRUE);
			break;
		case ROBOT_MECHANICAL_TURK:
			printf("Please insert a card and press <enter>\n");
			if(fgets(buf, 80, stdin) == NULL) {
				printf("something happened, skipping test\n");
				exit(TEST_RV_SKIP);
			}
	}
}

void robot_insert_card_delayed() {
	switch(robot_type) {
		case ROBOT_NONE:
			fprintf(stderr, "E: robot needed, no robot configured\n");
			exit(EXIT_FAILURE);
		case ROBOT_AUTO:
		case ROBOT_AUTO_2:
			robot_cmd('I', CK_FALSE);
			break;
		case ROBOT_MECHANICAL_TURK:
			printf("Please wait a moment and then insert the card\n");
			break;
	}
}

void robot_remove_card() {
	char buf[80];
	switch(robot_type) {
		case ROBOT_NONE:
			fprintf(stderr, "E: robot needed, no robot configured\n");
			exit(EXIT_FAILURE);
		case ROBOT_AUTO:
		case ROBOT_AUTO_2:
			robot_cmd('e', CK_TRUE);
			break;
		case ROBOT_MECHANICAL_TURK:
			printf("Please remove the card from the slot and press <enter>\n");
			if(fgets(buf, 80, stdin) == NULL) {
				printf("soemthing happened, skipping test\n");
				exit(TEST_RV_SKIP);
			}
	}
}

void robot_remove_card_delayed() {
	switch(robot_type) {
		case ROBOT_NONE:
			fprintf(stderr, "E: robot needed, no robot configured\n");
			exit(EXIT_FAILURE);
		case ROBOT_AUTO:
		case ROBOT_AUTO_2:
			robot_cmd('E', CK_FALSE);
			break;
		case ROBOT_MECHANICAL_TURK:
			printf("Please wait a moment and then remove the card\n");
			break;
	}
}

void robot_insert_reader() {
	assert(have_reader_robot() != CK_FALSE);

	char buf[80];
	switch(robot_type) {
		case ROBOT_NONE:
			fprintf(stderr, "E: robot needed, no robot configured\n");
			exit(EXIT_FAILURE);
		case ROBOT_AUTO:
			fprintf(stderr, "E: card robot needed, not supported by current robot\n");
			exit(EXIT_FAILURE);
		case ROBOT_AUTO_2:
			reader_cmd('i', CK_TRUE);
			break;
		case ROBOT_MECHANICAL_TURK:
			printf("Please insert a reader and press <enter>\n");
			if(fgets(buf, 80, stdin) == NULL) {
				printf("something happened, skipping test\n");
				exit(TEST_RV_SKIP);
			}
	}
}

void robot_remove_reader() {
	assert(have_reader_robot() != CK_FALSE);

	char buf[80];
	switch(robot_type) {
		case ROBOT_NONE:
			fprintf(stderr, "E: robot needed, no robot configured\n");
			exit(EXIT_FAILURE);
		case ROBOT_AUTO:
			fprintf(stderr, "E: card robot needed, not supported by current robot\n");
			exit(EXIT_FAILURE);
		case ROBOT_AUTO_2:
			reader_cmd('e', CK_TRUE);
			break;
		case ROBOT_MECHANICAL_TURK:
			printf("Please remove all readers and press <enter>\nIf you are not able to remove one or more readers from the computer, please set the EID_BUILTIN_READER environment variable to a non-empty value\n");
			if(fgets(buf, 80, stdin) == NULL) {
				printf("something happened, skipping test\n");
				exit(TEST_RV_SKIP);
			}
	}
}

int find_slot(CK_BBOOL with_token, CK_SLOT_ID_PTR slot) {
	CK_RV rv;
	CK_ULONG count = 0;
	CK_SLOT_ID_PTR list = NULL;
	unsigned int i;
	ckrv_mod m[] = { { CKR_BUFFER_TOO_SMALL, TEST_RV_OK } };
	CK_SESSION_HANDLE session;

	if(have_reader_robot()) {
		robot_insert_reader();
	}
	check_rv_long(C_GetSlotList(with_token, NULL_PTR, &count), m);
	printf("INFO: slots %sfound: %lu\n", with_token ? "with token " : "", count);
	if(count == 0 && with_token) {
		/* no slots with token found; try asking for a token */
		if(!have_robot()) {
			printf("Need at least one token to run this test\n");
			return TEST_RV_SKIP;
		}
		robot_insert_card();
		// assume we have one token, now
		count=1;
	}

	do {
		list = (CK_SLOT_ID_PTR)realloc(list, sizeof(CK_SLOT_ID) * count);
	} while((rv = C_GetSlotList(with_token, list, &count) == CKR_BUFFER_TOO_SMALL));
	check_rv_late("C_GetSlotList");

	i = 0;
	if(with_token) {
		do {
			CK_RV closerv;
			rv = C_OpenSession(list[i], CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session);
			if(rv != CKR_TOKEN_NOT_RECOGNIZED) {
				check_rv_late("C_OpenSession");
			} else {
				printf("INFO: skipping slot %lu, token not recognized\n", list[i]);
			}
			closerv = C_CloseSession(session);
			if(closerv != CKR_OK) {
				free(list);
				return ckrv_decode(rv, "C_CloseSession(session)", 0, NULL);
			}
		} while(rv != CKR_OK && (++i < count));
		if (i >= count ) {
			printf("Need at least one known token for this test\n");
			free(list);
			return TEST_RV_SKIP;
		}
	}
	printf("INFO: using slot %lu\n", list[i]);

	*slot = list[i];
	free(list);

	return TEST_RV_OK;
}

void hex_dump(char* data, CK_ULONG length) {
	CK_ULONG i;
	int j;

	for(i=0, j=0; i<length; i++) {
		int8_t d = (int8_t)(data[i]);
		printf("%02hhx ", d);
		j+=3;
		if(!((i + 1) % 5)) {
			printf(" ");
			j += 1;
		}
		if(j >= 80) {
			printf("\n");
			j = 0;
		}
	}
	if(j) {
		printf("\n");
	}
}
