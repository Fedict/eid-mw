#include <win32.h>
#include <pkcs11.h>

#include <stdio.h>
#include <malloc.h>
#include "serial_io.h"

#include <Windows.h>

char* default_card_port = "\\.\COM1";
char* default_usb_port = "\\.\COM2";

struct tstserial {
	HANDLE port;
	char buf[200];
	int data_start;
	int data_len;
};

Serial* serial_open(char *portname) {
	Serial *rv = calloc(sizeof(rv), 1);
	DCB dcb;
	COMMTIMEOUTS timeouts;
	int len;

	rv->port = CreateFile(portname, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (rv->port == INVALID_HANDLE_VALUE) {
		free(rv);
		return NULL;
	}
	dcb.BaudRate = 9600;
	dcb.ByteSize = 8;
	dcb.fBinary = TRUE;
	dcb.fDtrControl = DTR_CONTROL_ENABLE;
	dcb.fDsrSensitivity = FALSE;
	dcb.fOutX = FALSE;
	dcb.fInX = FALSE;
	dcb.fRtsControl = RTS_CONTROL_ENABLE;
	dcb.fAbortOnError = FALSE;
	dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = FALSE;
	dcb.StopBits = ONESTOPBIT;
	dcb.Parity = NOPARITY;
	if (!SetCommState(rv->port, &dcb)) {
		free(rv);
		return NULL;
	}
	timeouts.ReadIntervalTimeout = 1;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	if (!SetCommTimeouts(rv->port, &timeouts)) {
		free(rv);
		return NULL;
	}
	if (!SetCommMask(rv->port, EV_RXCHAR)) {
		free(rv);
		return NULL;
	}

	return rv;
}

void serial_close(Serial *port) {
	CloseHandle(port->port);
	free(port);
}

char *serial_read_line(Serial *port) {
	if (!port->data_len) {
		port->data_start = 0;
	} else {
		size_t len = strcspn(port->buf + port->data_start, "\r\n");
		if (len == 0) {
			port->data_len--;
			port->data_start++;
			return serial_read_line(port);
		}
		if (len < port->data_len) {
			char *ptr = port->buf + port->data_start;
			port->data_len -= len + 1;
			port->data_start += len + 1;
			return strndup(ptr, len);
		}
	}
	DWORD performed;
	if (!(ReadFile(port->port, port->buf + port->data_start, sizeof(port->buf) - port->data_start, &performed, NULL))) {
		fprintf(stderr, "could not read from serial port; error = %d\n", GetLastError());
		return NULL;
	}
	port->data_len += performed;
	port->buf[port->data_start + port->data_len] = '\0';
	return serial_read_line(port);
}

void serial_free_line(char *line) {
	free(line);
}

void serial_clear(Serial *port) {
	while (serial_has_data(port)) {
		char buf[80];
		DWORD performed;
		if (!ReadFile(port->port, buf, sizeof buf, &performed, NULL)) {
			perror("read");
			exit(EXIT_FAILURE);
		}
		if (!performed) {
			return;
		}
	}
	port->data_len = port->data_start = 0;
}

bool serial_writec(Serial *port, char c) {
	DWORD performed;
	if (!WriteFile(port->port, &c, 1, &performed, NULL) || performed != 1) {
		return false;
	}
	return true;
}

bool serial_has_data(Serial *port) {
	COMSTAT status;
	DWORD errors;
	if (!ClearCommError(port->port, &errors, &status)) {
		return false;
	}
	if (status.cbInQue > 0) {
		return true;
	}
	return false;
}

void sr_wait(unsigned int millis) {
	Sleep(millis);
}