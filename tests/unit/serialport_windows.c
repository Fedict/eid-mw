#include <win32.h>
#include <pkcs11.h>

#include "testlib.h"

CK_BBOOL robot_cmd(char cmd, CK_BBOOL check_result) {
	return;
}

CK_BBOOL reader_cmd(char cmd, CK_BBOOL check_result) {
	return;
}

CK_BBOOL open_robot(char *envvar) {
	return CK_FALSE;
}

CK_BBOOL open_reader_robot(char *envvar) {
	return CK_FALSE;
}