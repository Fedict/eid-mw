#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef HAVE_TERMIOS_H
#error Need termios.h! This cannot work.
#endif

#include "serial_io.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/select.h>

char *default_card_port = "/dev/ttyACM0";
char *default_usb_port = "/dev/ttyACM1";

struct tstserial {
	int fd;
	char buf[200];
	int data_start;
	int data_len;
};

Serial* serial_open(char *portname) {
	Serial* rv = calloc(sizeof(struct tstserial), 1);
	if(!rv) {
		perror("malloc");
		return NULL;
	}

	rv->fd = open(portname, O_RDWR | O_NOCTTY);

	struct termios ios;
	if(rv->fd < 0) {
		perror("open card robot");
		free(rv);
		return NULL;
	}
	if(ioctl(rv->fd, TIOCEXCL) == -1) {
		perror("ioctl TIOCEXCL");
		return NULL;
	}
	tcgetattr(rv->fd, &ios);
	cfsetispeed(&ios, B9600);
	cfsetospeed(&ios, B9600);
	ios.c_cflag &= ~PARENB & ~CSTOPB & ~CSIZE;
	ios.c_cflag |= CLOCAL | CREAD | CS8 | CRTSCTS;
	ios.c_lflag |= ICANON;
	tcsetattr(rv->fd, TCSANOW, &ios);
	tcflow(rv->fd, TCOON);

	return rv;
}

void serial_close(Serial *port) {
	close(port->fd);
	free(port);
}

bool serial_has_data(Serial *port) {
	struct timeval tv;

	fd_set rb;
	FD_ZERO(&rb);
	FD_SET(port->fd, &rb);
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	select(port->fd+1, &rb, NULL, NULL, &tv);
	return FD_ISSET(port->fd, &rb) ? true : false;
}

char *serial_read_line(Serial *port) {
	if(!port->data_len) {
		port->data_start = 0;
	} else {
		size_t len = strcspn(port->buf + port->data_start, "\r\n");
		if(len == 0) {
			port->data_len--;
			port->data_start++;
			return serial_read_line(port);
		}
		if(len < port->data_len) {
			char *ptr = port->buf + port->data_start;
			port->data_len -= len + 1;
			port->data_start += len + 1;
			return strndup(ptr, len);
		}
	}
	ssize_t len = read(port->fd, port->buf + port->data_start + port->data_len, sizeof(port->buf) - port->data_start - port->data_len - 1);
	if(len < 0) {
		perror("read");
		return NULL;
	}
	port->buf[port->data_start + port->data_len + len + 1] = '\0';
	port->data_len += len;
	return serial_read_line(port);
}

void serial_free_line(char *line) {
	free(line);
}

void serial_clear(Serial *port) {
	while(serial_has_data(port)) {
		char buf[80];
		if(read(port->fd, buf, 79) < 0) {
			perror("read");
			exit(EXIT_FAILURE);
		}
	}
	port->data_len = port->data_start = 0;
}

bool serial_writec(Serial *port, char c) {
	return (write(port->fd, &c, 1) == 1) ? true : false;
}

void sr_wait(unsigned int millis) {
	usleep(millis * 1000);
}
