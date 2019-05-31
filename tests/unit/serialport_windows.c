#include <win32.h>
#include <pkcs11.h>

#include "testlib.h"

#include <Windows.h>

static HANDLE robot_port = INVALID_HANDLE_VALUE;
static HANDLE reader_port = INVALID_HANDLE_VALUE;
static int robot_unit = 0;

bool robot_has_data(HANDLE port, int delay_secs) {
	if (delay_secs) return true;
	return false;
}

void robot_cmd_l(char cmd, CK_BBOOL check_result, HANDLE port, char *which) {
	struct expect {
		char command;
		char *result;
		bool wait;
	} expected[] = {
		{ 'i', "inserted", true },
		{ 'e', "ejected", true },
		{ 'p', "parked", false },
	};
	int len = 0;
	DWORD performed;
	char line[80];
	unsigned int i;

	while (robot_has_data(port, 0)) {
		ReadFile(port, line, sizeof line, &performed, NULL);
	}
	printf("sending command %c to %s robot...\n", cmd, which);
	WriteFile(port, &cmd, 1, &performed, NULL);
	if (!check_result) {
		printf("\tdone, not waiting\n");
		return;
	}
	do {
		if (!robot_has_data(port, 5)) {
			fprintf(stderr, "No reply from robot after 5 seconds\n");
			exit(EXIT_FAILURE);
		}
		ReadFile(port, line + len, 79 - len, &performed, NULL);
		len += performed;
		line[len] = '\0';
	} while (line[len - 1] != '\n');
	for(i=0; i<sizeof(expected) / sizeof(struct expect); i++) {
		if (expected[i].command == cmd) {
			if (strncmp(expected[i].result, line, strlen(expected[i].result))) {
				fprintf(stderr, "Robot handling failed: expected %s, received %s\n", expected[i].result, line);
				exit(EXIT_FAILURE);
			}
			if (expected[i].wait) {
				Sleep(2000);
			} else {
				Sleep(200);
			}
			printf("\tok\n");
			return;
		}
	}
}

void robot_cmd(char cmd, CK_BBOOL check_result) {
	return robot_cmd_l(cmd, check_result, robot_port, "card");
}

void reader_cmd(char cmd, CK_BBOOL check_result) {
	robot_cmd_l(cmd, check_result, reader_port, "reader");
}

CK_BBOOL port_init(HANDLE port, char type_char) {
	DCB dcb;
	COMMTIMEOUTS timeouts;
	int len;
	struct rpos rp;

	if (!GetCommState(robot_port, &dcb)) {
		return CK_FALSE;
	}
	dcb.BaudRate = 9600;
	dcb.ByteSize = 8;
	dcb.fBinary = TRUE;
	dcb.fDtrControl = DTR_CONTROL_ENABLE;
	dcb.fDsrSensitivity = FALSE;
	dcb.fTXContinueOnXoff = FALSE;
	dcb.fOutX = FALSE;
	dcb.fInX = FALSE;
	dcb.fErrorChar = FALSE;
	dcb.fRtsControl = RTS_CONTROL_ENABLE;
	dcb.fAbortOnError = FALSE;
	dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = FALSE;
	dcb.StopBits = ONESTOPBIT;
	dcb.Parity = NOPARITY;
	if (!SetCommState(robot_port, &dcb)) {
		return CK_FALSE;
	}
	timeouts.ReadIntervalTimeout = 1;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;
	if (!SetCommTimeouts(robot_port, &timeouts)) {
		return CK_FALSE;
	}

	DWORD performed;
	WriteFile(port, "R", 1, &performed, NULL);
	if (performed != 1) {
		return CK_FALSE;
	}

	len = 0;
	char line[80];
	do {
		ReadFile(port, line + len, sizeof(line) - len, &performed, NULL);
		len += performed;
		line[len] = '\0';
		if (robot_type == ROBOT_AUTO) {
			if (strncmp(line, "READY.", len < 6 ? len : 6)) {
				fprintf(stderr, "Robot not found: received %s from serial line, expecting \"READY.\\n\"\n", line);
				return CK_FALSE;
			}
		} else {
			rp = skip_uninteresting(line);
			if (rp.pos < SYSTEM_FOUND || !rp.is_complete) {
				continue;
			}
		}
		if (strlen(line) == rp.offset) {
			WriteFile(port, "t", 1, &performed, NULL);
			if (performed != 1) {
				printf("could not write");
				return CK_FALSE;
			}
			continue;
		}
		if (line[rp.offset] != 'T') {
			fprintf(stderr, "Robot was not found: received %s from serial line, expecting \"T\"\n", line);
			return CK_FALSE;
		}
		if ((len - rp.offset) > 4 && line[rp.offset + 1] == 'B') {
			if (robot_unit != 0) {
				if (line[rp.offset + 2] - 0x30 != robot_unit) {
					fprintf(stderr, "Robot does not match: card and USB devices not the same\n");
					return CK_FALSE;
				}
			}
			robot_unit = line[rp.offset + 2] - 0x30;
			if (line[rp.offset + 4] != type_char) {
				fprintf(stderr, "Robot does not match: wrong robot type found\n");
				return CK_FALSE;
			}
		}
	} while (line[len - 1] != '\r' || rp.pos < SYSTEM_FOUND || !rp.is_complete || rp.offset == strlen(line));

	return CK_TRUE;
}

CK_BBOOL open_robot(char *envvar) {
	char *dev;
	if (robot_port != INVALID_HANDLE_VALUE) {
		return CK_TRUE;
	}
	switch (robot_type) {
	case ROBOT_AUTO:
		if (strlen(envvar) == strlen("fedict")) {
			dev = strdup("\\\\.\\COM1");
		} else {
			dev = strdup(envvar + strlen("fedict") + 1);
		}
		break;
	case ROBOT_AUTO_2:
		if (strlen(envvar) == strlen("zetes")) {
			dev = strdup("\\\\.\\COM1");
		} else {
			char *p;
			dev = strdup(strchr(envvar, ':') + 1);
			if ((p = strchr(dev, ':')) != NULL) {
				*p = '\0';
			}
		}
		break;
	default:
		fprintf(stderr, "E: can't open the robot when it's not there!\n");
		return CK_FALSE;
	}
	printf("opening card robot at %s\n", dev);
	robot_port = CreateFile(dev, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (robot_port == INVALID_HANDLE_VALUE) {
		return CK_FALSE;
	}
	free(dev);
	return port_init(robot_port, 'C');
}

CK_BBOOL open_reader_robot(char *envvar) {
	char *dev;
	if (reader_port != INVALID_HANDLE_VALUE) {
		return CK_TRUE;
	}
	switch (robot_type) {
	case ROBOT_AUTO_2:
		if (strlen(envvar) == strlen("zetes")) {
			dev = strdup("\\\\.\\COM2");
		}
		else {
			char *p;
			dev = strdup(strrchr(envvar, ':') + 1);
		}
		break;
	default:
		fprintf(stderr, "E: can't open the reader robot when it's not there!\n");
		return CK_FALSE;
	}

	printf("opening reader robot at %s\n", dev);
	reader_port = CreateFile(dev, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (reader_port == INVALID_HANDLE_VALUE) {
		return CK_FALSE;
	}
	free(dev);
	return port_init(reader_port, 'U');
}