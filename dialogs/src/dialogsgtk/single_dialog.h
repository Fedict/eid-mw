/* ****************************************************************************
 * eID Middleware Project.
 * Copyright (C) 2008-2010 FedICT.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.
**************************************************************************** */

#include <limits.h>
#include <unistd.h>

#define	MIN_CMDLINE_PATH_BYTES 14
#define	MAX_UID_LENGTH		   64

typedef struct
{
	char pid_path[PATH_MAX];
} SingleDialog;


void 	sdialog_init(SingleDialog* sdialog);
int 	sdialog_write_pid(SingleDialog* sdialog);
pid_t 	sdialog_read_pid(SingleDialog* sdialog);
int 	sdialog_terminate(SingleDialog* sdialog, pid_t target);
int 	sdialog_terminate_active(SingleDialog* sdialog);
int 	sdialog_lock(SingleDialog* sdialog);
int 	sdialog_unlock(SingleDialog* sdialog);

#define POSIX_SOURCE 1

void 	sdialog_call(char* path, char* msg);
char* 	sdialog_call_modal(char* path,const char* msg);
