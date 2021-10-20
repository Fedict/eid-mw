#ifdef WIN32
#include <win32.h>
#else
#include <unix.h>
#endif
#include <pkcs11.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

#include "testlib.h"

int child_part();

/*
 * Check behaviour of C_Initialize across a fork() call (PKCS#11 §6.6.1)
 */
TEST_FUNC(fork_init) {
	pid_t child;
	int status;

	check_rv(C_Initialize(NULL_PTR));
	if((child = fork()) == 0) {
		if(child_part() == TEST_RV_OK) {
			exit(EXIT_SUCCESS);
		} else {
			exit(EXIT_FAILURE);
		}
	} else {
		if(child == -1) {
			printf("fork failed, skipping test\n");
			return TEST_RV_SKIP;
		}
        waitpid(child, &status, 0);
		if(!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
			check_rv(C_Finalize(NULL_PTR));
			return TEST_RV_FAIL;
		}
	}
	check_rv(C_Finalize(NULL_PTR));

	return TEST_RV_OK;
}

int child_part() {
	CK_INFO info;
	ckrv_mod m[] = { { CKR_CRYPTOKI_ALREADY_INITIALIZED, TEST_RV_OK } };

	check_rv_long(C_Initialize(NULL_PTR), m);
	check_rv(C_GetInfo(&info));

	return TEST_RV_OK;
}

