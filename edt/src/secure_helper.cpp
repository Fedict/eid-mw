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
#define _POSIX_SOURCE
#include "secure_helper.h"
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include "util.h"
#include "error.h"

#define VASPRINTF_INITIAL_SIZE	128
#define VASPRINTF_FAILED	-1

errno_t _localtime_s(struct tm* _tm, const time_t *time)
{
	struct tm* tm=localtime(time);
	if(tm==NULL)
		return errno;
	memcpy((void*)_tm,(void*)tm,sizeof(struct tm));
	return errno;
}

int vasprintf(char **strp, const char *fmt, va_list ap)
{
	size_t size=VASPRINTF_INITIAL_SIZE;

	char* buf=(char*)malloc(size);			// initial attempt for a "reasonable" string length
	if(buf==NULL)
		return VASPRINTF_FAILED;

	size_t written=vsnprintf(buf, size, fmt, ap);	// try and vsnprintf to it

	if(written<0)					// if failed, clean up buf and return error
	{	
		free(buf);
		return VASPRINTF_FAILED;
	}

	if(written>size)				// if written, but buffer was too small
	{
		size=written;				// we now know exactly which size we need
		char* newbuf=(char*)realloc(buf,size);	// try and grow buffer
                if(newbuf==NULL)
                {
                        free(buf);			// if we can't grow buffer,
			return VASPRINTF_FAILED;	// return error.
                }
                else
                        buf=newbuf;			// buffer grown, write to it again

                written=vsnprintf(buf, size, fmt, ap);
	}	

	if(written<0)
	{
		free(buf);				// if we couldn't write (=Threading issue ; va_list or fmt changed) clean up buf and return error
		return VASPRINTF_FAILED;
	}

	if(written==size)				// if we did write and filled the buffer, set strp and return number of bytes written
	{
		*strp=buf;
		return written;
			
	}

	return VASPRINTF_FAILED;			// if we fall through here, return error.
}

////////////////////////////////////////////////////////////////////////////////////////////////
int _wfopen_s(FILE** pFile, const wchar_t *filename, const wchar_t *mode)
{
	int r = 0;

	if (pFile == NULL)
		return -1;

	std::wstring wFileName=filename;
	std::wstring wMode=mode;
	FILE *f = fopen(string_From_wstring(wFileName).c_str(), string_From_wstring(wMode).c_str());

	if (f != NULL)
		*pFile = f;
	else
		r = errno;

	return r;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int fwprintf_s(FILE *stream, const wchar_t *format, ...)
{
	va_list args;
	char *csTmp = NULL;
	int r = -1;

	va_start(args, format);
	std::wstring wFormat=format;
	r = vasprintf(&csTmp, string_From_wstring(wFormat).c_str(), args);
	va_end(args);

	if (r != -1 && csTmp != NULL)
	{
		r = fprintf(stream, csTmp);
		free(csTmp);
	}

	return r;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int _vfwprintf_s(FILE *stream, const wchar_t *format, va_list argptr)
{
	char *csTmp = NULL;
	int r = -1;

	std::wstring wFormat=format;
	r = vasprintf(&csTmp, string_From_wstring(wFormat).c_str(), argptr);

	if (r != -1 && csTmp != NULL)
	{
		r = fprintf(stream, csTmp);
		free(csTmp);
	}

	return r;
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int strcpy_s(char *dest, size_t len, const char *src)
{
	if (dest == NULL)
		return -1;

	for ( ; len > 1 && *src != '\0'; dest++, src++, len--)
		*dest = *src;

	*dest = '\0';

	return *src == '\0' ? 0 : -1; // 0: OK, -1: NOK
}

////////////////////////////////////////////////////////////////////////////////////////////////
int wcscpy_s(wchar_t *dest, size_t len, const wchar_t *src)
{
	if (dest == NULL)
		return -1;

	for ( ; len > 1 && *src != L'\0'; dest++, src++, len--)
		*dest = *src;

	*dest = L'\0';

	return *src == L'\0' ? 0 : -1; // 0: OK, -1: NOK
}

////////////////////////////////////////////////////////////////////////////////////////////////
int _swprintf_s(wchar_t *buffer, size_t sizeOfBuffer, const wchar_t *format, ...)
{
	va_list args;
	char *csTmp = NULL;
	int r = -1;

	va_start(args, format);
	std::wstring wFormat=format;
	r = vasprintf(&csTmp, string_From_wstring(wFormat).c_str(), args);
	va_end(args);

	if (r != -1 && csTmp != NULL)
	{
		std::string sTmp=csTmp;
		r = wcscpy_s(buffer, sizeOfBuffer, wstring_From_string(sTmp).c_str());
		free(csTmp);
	}

	return r;
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int wcscat_s(wchar_t* dst, size_t elem, const wchar_t* src)
{
    wchar_t* ptr = dst;

    if (!dst || elem == 0) return EINVAL;
    if (!src)
    {
        dst[0] = '\0';
        return EINVAL;
    }

    /* seek to end of dst string (or elem if no end of string is found */
    while (ptr < dst + elem && *ptr != '\0') ptr++;
    while (ptr < dst + elem)
    {
        if ((*ptr++ = *src++) == '\0') return 0;
    }
    /* not enough space */
    dst[0] = '\0';
    return ERANGE;
}
