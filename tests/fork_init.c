#ifdef WIN32
#include <win32.h>
#else
#include <unix.h>
#endif
#include <pkcs11.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

#include "testlib.h"

/*
 * Check behaviour of C_Initialize across a fork() call (PKCS#11 §6.6.1)
 */

TEST_FUNC(fork_init) {
	pid_t child;
	int status;

	check_rv(C_Initialize(NULL_PTR));
	if((child = fork()) == 0) {
		CK_INFO info;
		ckrv_mod m[] = { CKR_CRYPTOKI_ALREADY_INITIALIZED, TEST_RV_OK };

		check_rv_long(C_Initialize(NULL_PTR), m);
		check_rv(C_GetInfo(&info));
	} else {
		if(child == -1) {
			printf("fork failed, skipping test\n");
			return TEST_RV_SKIP;
		}
		wait(&status);
		if(!WIFEXITED(status)) {
			return TEST_RV_FAIL;
		} else {
			return WEXITSTATUS(status);
		}
	}
	check_rv(C_Finalize(NULL_PTR));

	return TEST_RV_OK;
}
