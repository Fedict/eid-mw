#ifndef SERIAL_IO_H
#define SERIAL_IO_H

#include <stdbool.h>

typedef struct tstserial Serial;

extern char* default_card_port;
extern char* default_usb_port;

Serial* serial_open(char *portname);
void serial_close(Serial *port);
char* serial_read_line(Serial *port);
void serial_free_line(char *line);
bool serial_writec(Serial *port, char c);
bool serial_has_data(Serial *port);
void serial_clear(Serial *port);
void sr_wait(unsigned int millis);

#endif
