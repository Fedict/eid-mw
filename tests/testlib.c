#include <unix.h>
#include <pkcs11.h>
#include <malloc.h>
#include <string.h>

#include "verbose_assert.h"
#include "testlib.h"

int va_counter;

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
