#include <unix.h>
#include <pkcs11.h>
#include <stdio.h>
#include "testlib.h"

/*
 * Main function
 */
int main(int argc, char** argv) {
	int failed=0;
	int skipped=0;
	int ran=0;
	int i;
	int retval;

	int return_now = init_tests();

	for(i=0; i< TESTS_COUNT; i++) {
		if(!return_now) {
			printf("test %d: ", i);
		}
		if(should_run_test(TESTS_TO_RUN, i)) {
			retval = test_ptrs[i]();
			if(return_now) {
				return retval;
			} else {
				switch(retval) {
				case TEST_RV_SKIP:
					skipped++;
					printf("skipped\n");
					break;
				case TEST_RV_FAIL:
					ran++;
					failed++;
					printf("failed\n");
					break;
				case TEST_RV_OK:
					ran++;
					printf("ok\n");
					break;
				}
			}
		}
	}
	if(return_now) {
		printf("test not run!\n");
		return TEST_RV_FAIL;
	} else {
		printf("totals: ran %d tests, of which %d failed. %d were skipped.\n", ran, failed, skipped);
	}
	return 0;
}
