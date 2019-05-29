#include <unix.h>
#include <pkcs11.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifndef HAVE_TERMIOS_H
#error need termios.h!
#endif

#include "testlib.h"

#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#ifndef O_EXLOCK
// needed on macOS, but only used in a bitmask, so...
#define O_EXLOCK 0
#endif

int robot_dev = 0;
int robot_unit = 0;
int reader_dev = 0;

bool robot_has_data(int fd, int delay_secs) {
	struct timeval tv;

	fd_set rb;
	FD_ZERO(&rb);
	FD_SET(fd, &rb);
	tv.tv_sec = delay_secs;
	tv.tv_usec = 0;
	select(fd+1, &rb, NULL, NULL, &tv);
	return FD_ISSET(fd, &rb) ? true : false;
}

void robot_cmd_l(int dev, char cmd, CK_BBOOL check_result, char *which) {
	struct expect {
		char command;
		char* result;
		bool wait;
	} expected[] = {
		{ 'i', "inserted", true },
		{ 'e', "ejected", false },
		{ 'p', "parked", false },
	};
	int len = 0;
	char line[80];
	unsigned int i;

	while(robot_has_data(dev, 0)) {
		read(dev, line, sizeof line);
	}
	printf("sending command %c to %s robot...\n", cmd, which);
	write(dev, &cmd, 1);
	if(!check_result) {
		printf("\tdone, not waiting\n");
		return;
	}
	do {
		if(!robot_has_data(dev, 5)) {
			fprintf(stderr, "No reply from robot after 5 seconds\n");
			exit(EXIT_FAILURE);
		}
		len += read(dev, line+len, 79);
		line[len]='\0';
	} while(line[len-1] != '\n');
	for(i=0; i<sizeof(expected) / sizeof(struct expect); i++) {
		if(expected[i].command == cmd) {
			if(strncmp(expected[i].result, line, strlen(expected[i].result))) {
				fprintf(stderr, "Robot handling failed: expected %s, received %s\n", expected[i].result, line);
				exit(TEST_RV_SKIP);
			}
			if(expected[i].wait) {
				sleep(2);
			} else {
				usleep(200);
			}
			printf("\tok\n");
			return;
		}
	}
}

void robot_cmd(char cmd, CK_BBOOL check_result) {
	return robot_cmd_l(robot_dev, cmd, check_result, "card");
}

void reader_cmd(char cmd, CK_BBOOL check_result) {
	return robot_cmd_l(reader_dev, cmd, check_result, "reader");
}

CK_BBOOL init_robot(int fd, char type_char) {
	struct termios ios;
	char line[80];
	int len = 0;
	struct rpos rp;

	if(fd < 0) {
		perror("open card robot");
		return CK_FALSE;
	}
	if(ioctl(fd, TIOCEXCL) == -1) {
		perror("ioctl TIOCEXCL");
		return CK_FALSE;
	}
	if(fcntl(fd, F_SETFL, 0) == -1) {
		perror("clearing O_NONBLOCK");
		return CK_FALSE;
	}
	tcgetattr(fd, &ios);
	cfsetispeed(&ios, B9600);
	cfsetospeed(&ios, B9600);
	ios.c_cflag &= ~PARENB & ~CSTOPB & ~CSIZE;
	ios.c_cflag |= CLOCAL | CREAD | CS8 | CRTSCTS;
	ios.c_lflag |= ICANON;
	tcsetattr(fd, TCSANOW, &ios);
	tcflow(fd, TCOON);

	bool written = false;
	while(!written) {
		if(write(fd, "R", 1) == -1) {
			if(errno != EAGAIN) {
				perror("write");
				return CK_FALSE;
			}
		} else {
			written = true;
		}
	}

	len = 0;
	do {
		ssize_t read_len = read(fd, line+len, sizeof(line) - len);
		if(read_len < 0) {
			if(errno == EAGAIN) {
				usleep(20);
				continue;
			} else {
				perror("Could not read robot");
				return CK_FALSE;
			}
		}
		len += read_len;
		line[len]=0;
		if(robot_type == ROBOT_AUTO) {
			if(strncmp(line, "READY.", len < 6 ? len : 6)) {
				fprintf(stderr, "Robot not found: received %s from serial line, expecting \"READY.\\n\"\n", line);
				return CK_FALSE;
			}
		} else {
			rp = skip_uninteresting(line);
			if(rp.pos < SYSTEM_FOUND || !rp.is_complete) {
				continue;
			}
			if(strlen(line) == rp.offset) {
				written = false;
				while(!written) {
					if(write(fd, "t", 1) == -1) {
						if(errno != EAGAIN) {
							perror("write");
							return CK_FALSE;
						} else {
							usleep(20);
						}
					} else {
						written = true;
					}
				}
				continue;
			}
			if(line[rp.offset] != 'T') {
				fprintf(stderr, "Robot not found: received %s from serial line, expecting \"T\"\n", line);
				return CK_FALSE;
			}
			if((len - rp.offset) > 4 && line[rp.offset + 1] == 'B') {
				if(robot_unit != 0) {
					if(line[rp.offset + 2] - 0x30 != robot_unit) {
						fprintf(stderr, "Robot does not match: card and USB devices not the same\n");
						return CK_FALSE;
					}
				}
				robot_unit = line[rp.offset + 2] - 0x30;
				if(line[rp.offset + 4] != type_char) {
					fprintf(stderr, "Robot does not match: wrong robot type found\n");
					return CK_FALSE;
				}
			}
		}
	} while(line[len-1] != '\n' || rp.pos < SYSTEM_FOUND || !rp.is_complete || rp.offset == strlen(line));

	return CK_TRUE;
}

CK_BBOOL open_robot(char* envvar) {
	char* dev;

	switch(robot_type) {
	case ROBOT_AUTO:
		if(strlen(envvar) == strlen("fedict")) {
			dev = strdup("/dev/ttyACM0");
		} else {
			dev = strdup(envvar + strlen("fedict") + 1);
		}
		break;
	case ROBOT_AUTO_2:
		if(strlen(envvar) == strlen("zetes")) {
			dev = strdup("/dev/ttyACM0");
		} else {
			char *p;
			dev = strdup(strchr(envvar, ':') + 1);
			if((p = strchr(dev, ':')) != NULL) {
				*p = '\0';
			}
		}
		break;
	default:
		fprintf(stderr, "E: can't open the robot when it's not there!\n");
		return CK_FALSE;
	}
	printf("opening card robot at %s\n", dev);
	robot_dev = open(dev, O_RDWR | O_NOCTTY | O_EXLOCK | O_NONBLOCK);
	free(dev);
	return init_robot(robot_dev, 'C');
}

CK_BBOOL open_reader_robot(char* envvar) {
	char* dev;

	if(robot_type != ROBOT_AUTO_2) {
		fprintf(stderr, "E: no reader robot connected!\n");
		return CK_FALSE;
	}
	if(strlen(envvar) == strlen("zetes")) {
		dev = "/dev/ttyACM1";
	} else {
		dev = strrchr(envvar, ':') + 1;
	}
	printf("opening reader robot at %s\n", dev);
	reader_dev = open(dev, O_RDWR | O_NOCTTY | O_NONBLOCK);
	return init_robot(reader_dev, 'U');
}
