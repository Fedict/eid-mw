#ifndef EID_LABELS_H
#define EID_LABELS_H

#ifdef __cplusplus
extern "C"
{
#endif

/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2014 FedICT.
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
#ifdef WIN32
#include <win32.h>
#else
#include <unix.h>
#endif
#include <pkcs11.h>

struct labelnames {
	EID_CHAR ** label;
	int len;
};

CK_BBOOL is_string(const EID_CHAR * const label);
CK_BBOOL on_foreigner(const EID_CHAR * const label);
CK_BBOOL on_eid(const EID_CHAR * const label);
const EID_CHAR * min_version(const EID_CHAR * const label);

struct labelnames* get_foreigner_labels(void);

#ifdef __cplusplus
}
#endif

#endif
