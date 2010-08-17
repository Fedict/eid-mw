/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
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
#ifndef __SEC_HELPER_H__
#define __SEC_HELPER_H__

#include <string.h>
#include <iostream>
#include "util.h"
#include "error.h"

errno_t _localtime_s(struct tm* _tm, const time_t *time);
int 	_swprintf_s(wchar_t *buffer, size_t sizeOfBuffer, const wchar_t *format, ...);
int 	_vfwprintf_s(FILE *stream, const wchar_t *format, va_list argptr);

int vasprintf(char **strp, const char *fmt, va_list ap);
int _wfopen_s(FILE** pFile, const wchar_t *filename, const wchar_t *mode);
int fwprintf_s(FILE *stream, const wchar_t *format, ...);
int strcpy_s(char *dest, size_t len, const char *src);
int wcscpy_s(wchar_t *dest, size_t len, const wchar_t *src);
int wcscat_s(wchar_t* dst, size_t elem, const wchar_t* src);

#endif //__SEC_HELPER_H__
