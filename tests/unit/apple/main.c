#include <stdio.h>
#include <unix.h>
#include <pkcs11.h>
#include "testlib.h"

#define run_test(test) { printf("Running %s...\n", #test); switch(test) { \
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

int main(void) {
	int skipped_count=0, failed_count=0, ok_count=0;

	run_test(init_finalize());
    run_test(fork_init());
    run_test(double_init());
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
    run_test(login());
    run_test(nonsensible());
    run_test(objects());
    run_test(readdata());
    run_test(digest());
    run_test(threads());
    run_test(sign());
    run_test(decode_photo());
    
    printf("Test suite finished.\nOK: %d\nFailed: %d\nSkipped: %d\n", ok_count, failed_count, skipped_count);
}
