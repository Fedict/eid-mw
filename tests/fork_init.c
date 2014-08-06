#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/wait.h>

#include "testlib.h"

/*
 * Check behaviour of C_Initialize across a fork() call (PKCS#11 §6.6.1)
 */

int main(void) {
	CK_RV rv;
	pid_t child;
	int status;

	rv = C_Initialize(NULL_PTR);
	check_rv;
	if((child = fork()) == 0) {
		CK_INFO info;

		rv = C_Initialize(NULL_PTR);
		verbose_assert(rv == CKR_CRYPTOKI_ALREADY_INITIALIZED || rv == CKR_OK);
		rv = C_GetInfo(&info);
		check_rv;
	} else {
		if(child == -1) {
			printf("fork failed, skipping test\n");
			return TEST_SKIP;
		}
		wait(&status);
		if(!WIFEXITED(status)) {
			return TEST_FAIL;
		} else {
			return WEXITSTATUS(status);
		}
	}
	rv = C_Finalize(NULL_PTR);
	check_rv;

	return TEST_OK;
}
