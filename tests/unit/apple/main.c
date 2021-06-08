/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2014 FedICT.
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
#include <stdio.h>
#include <unix.h>
#include <pkcs11.h>
#include <string.h>

#include "testlib.h"

#define run_test(test) if(test_target == NULL || !strcmp(test_target, #test)) \
	{ \
		printf("Running %s...\n", #test); \
		switch(test) { \
		case TEST_RV_OK: \
			printf("--> Test completed OK\n"); \
			ok_count++; \
			break; \
		case TEST_RV_SKIP: \
			printf("--> Test skipped\n"); \
			skipped_count++; \
			break; \
		case TEST_RV_FAIL: \
			printf("--> Test failed\n"); \
			failed_count++; \
			break; \
		} \
	}

int main(int argc, char** argv) {
	int skipped_count=0, failed_count=0, ok_count=0;
	char* test_target = NULL;
	int i=1;

	do {
		if(argc > 1) {
			test_target = argv[i];
		}
		run_test(eject());
		run_test(init_finalize());
		run_test(fork_init());
		run_test(double_init());
		run_test(wrong_init());
		run_test(getinfo());
		run_test(funclist());
		run_test(slotlist());
		run_test(slotinfo());
		run_test(tkinfo());
		run_test(slotevent());
		run_test(mechlist());
		run_test(mechinfo());
		run_test(sessions());
		run_test(sessions_nocard());
		run_test(sessioninfo());
		run_test(slogin());
		run_test(nonsensible());
		run_test(objects());
		run_test(readdata());
		run_test(readdata_sequence());
		run_test(readdata_limited());
		run_test(digest());
		run_test(threads());
		run_test(sign());
		run_test(sign_state());
		run_test(decode_photo());
		run_test(ordering());
	} while(++i < argc);

	printf("Test suite finished.\nOK: %d\nFailed: %d\nSkipped: %d\n", ok_count, failed_count, skipped_count);

	return failed_count;
}
