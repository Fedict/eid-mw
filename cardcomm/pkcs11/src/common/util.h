
/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2013 FedICT.
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
#pragma once

#ifndef _CONFIG_H_
#define _CONFIG_H_
#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#include "configuration.h"
#endif
#endif

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <iostream>
#include "mw_util.h"

namespace eIDMW
{

	void LogShitLevel(int x);

//--- string conversion between std::wstring and std::string
	std::wstring utilStringWiden(const std::string & in,
						   const std::locale & locale
						   = std::locale());
	std::string utilStringNarrow(const std::wstring & in,
						   const std::locale & locale
						   = std::locale());

/**
 * Case insensitve search, csSearch should be in lower case.
 * Returns true is csSearch is present in csData.
 */
	bool StartsWithCI(const char *csData, const char *csSearch);

	int Test(void);

	char *bin2AsciiHex(const unsigned char *pData, unsigned long ulLen);

	void GetProcessName(wchar_t * wBuffer, unsigned long ulLen);
}

#ifndef WIN32

//#define _TRUNCATE -1

int sprintf_s(char *buffer, size_t sizeOfBuffer,
			    const char *format, ...);

int strcat_s(char *dest, size_t len, const char *src);

int strcpy_s(char *dest, size_t len, const char *src);

int strncpy_s(char *dest, size_t len, const char *src,
			    long count);

int fopen_s(FILE ** pFile, const char *filename,
			  const char *mode);

int wcscpy_s(wchar_t * dest, size_t len, const wchar_t * src);

int fprintf_s(FILE * stream, const char *format, ...);

int vfprintf_s(FILE * stream, const char *format,
			     va_list argptr);
errno_t freopen_s(FILE ** pFile, const char *filename,
				const char *mode, FILE * stream);
#endif
