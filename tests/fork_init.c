#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/wait.h>

#include "testlib.h"

/*
 * Check behaviour of C_Initialize across a fork() call (PKCS#11 §6.6.1)
 */

int fork_init(void) {
	CK_RV rv;
	pid_t child;
	int status;

	rv = C_Initialize(NULL_PTR);
	check_rv;
	if((child = fork()) == 0) {
		CK_INFO info;

		rv = C_Initialize(NULL_PTR);
		assert(ckrv_decode(rv, 1, (CK_RV)CKR_CRYPTOKI_ALREADY_INITIALIZED, (int)TEST_RV_OK) == TEST_RV_OK);
		rv = C_GetInfo(&info);
		check_rv;
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
	rv = C_Finalize(NULL_PTR);
	check_rv;

	return TEST_RV_OK;
}
