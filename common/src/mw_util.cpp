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

#include "mw_util.h"
#include <string>
#include <algorithm>

#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE
#endif

#include <string.h>
#include <iostream>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include "util.h"

#include <errno.h>

#ifndef WIN32

#ifndef HAVE_VASPRINTF
#define VASPRINTF_INITIAL_SIZE	128
#define VASPRINTF_FAILED	-1
#endif

////////////////////////////////////////////////////////////////////////////////////////////////
std::wstring wstring_From_string(std::string const& in)
{
	//----------------------------------------------------------
	// just put every char to a wchar and prevent sign extension
	//----------------------------------------------------------
	std::wstring wzString;
	for(size_t i=0;i<in.length();i++)
	{
		wchar_t c = in.at(i);
		wzString += c & 0x00ff;
	}
	return wzString;
}

std::string string_From_wstring(std::wstring const& in)
{
  int length = (int)(in.length());
  char* pc = new char [length+1];
  use_facet< ctype<wchar_t> >(std::locale()).narrow (in.c_str(),in.c_str()+length+1,'?',pc);
  return std::string(pc);

}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////
/*void wstring_TrimR(std::wstring *in)
{
	if(in == NULL)
		return;
		
	for(size_t i=in->length();i>0;)
	{
		i--;
		if(in->at(i) == L' ')
		{
			in->at(i) = L'\0';
		}
		else
		{
			break;
		}
	}
}*/

////////////////////////////////////////////////////////////////////////////////////////////////
/*void vector_From_bytes(std::vector<unsigned char> *out, const unsigned char *in, size_t len)
{
	out->clear();
	out->reserve(len);

    for(size_t i = 0; i<len; i++)
		out->push_back(in[i]);

}*/
/*
////////////////////////////////////////////////////////////////////////////////////////////////
void TokenizeS (const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters, unsigned long maxToken)
{
	tokens.clear();
	
	// Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

	size_t count = 0;
    while ((maxToken == 0 || count < maxToken) && (std::string::npos != pos || std::string::npos != lastPos))
    {
        // Found a token, add it to the vector.
		// For the last token accepted we take up to the end of string
		if (pos != (size_t) -1 && (maxToken == 0 || count < (maxToken-1)))
			tokens.push_back(str.substr(lastPos, pos - lastPos));
        else 
			tokens.push_back(str.substr(lastPos, str.length() - lastPos));
		// Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
		
		count++;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
void TokenizeW (const std::wstring& str, std::vector<std::wstring>& tokens, const std::wstring& delimiters, unsigned long maxToken)
{
	tokens.clear();
	
    // Skip delimiters at beginning.
    std::wstring::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::wstring::size_type pos     = str.find_first_of(delimiters, lastPos);

	size_t count = 0;
    while ((maxToken == 0 || count < maxToken) && (std::string::npos != pos || std::string::npos != lastPos))
    {
        // Found a token, add it to the vector.
		// For the last token accepted we take up to the end of string
		if (pos != (size_t) -1 && (maxToken == 0 || count < (maxToken-1)))
			tokens.push_back(str.substr(lastPos, pos - lastPos));
        else 
			tokens.push_back(str.substr(lastPos, str.length() - lastPos));
		// Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
		
		count++;
    }
}
*/
////////////////////////////////////////////////////////////////////////////////////////////////

/*void wstring_to_lower(std::wstring& str)
{
       std::transform(str.begin(),str.end(),str.begin(),(int(*)(int))tolower);
}*/
/*
wchar_t clean_char(wchar_t in)
{
       switch(in)
       {
               case L'.':
                       return '_';
               case L' ':
                       return '_';
               default:
                       return in;
       }
}
*/

/*void wstring_clean(std::wstring& str)
{
       std::transform(str.begin(),str.end(),str.begin(),clean_char);
}*/

/*
errno_t _localtime_s(struct tm* _tm, const time_t *time)
{
	struct tm* tm=localtime(time);
	if(tm==NULL)
		return errno;
	memcpy((void*)_tm,(void*)tm,sizeof(struct tm));
	return errno;
}*/
#ifndef WIN32
#ifndef HAVE_VASPRINTF
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
		free(buf);
		char* buf=(char*)malloc(size);	// try and grow buffer
		if(buf==NULL)
		{
			return VASPRINTF_FAILED;	// return error.
		}

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
		return (int)written;
			
	}
	free(buf);
	return VASPRINTF_FAILED;			// if we fall through here, return error.
}
#endif
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
int _vfprintf_s(FILE *stream, const char* format, va_list argptr)
{
	char *csTmp = NULL;
	int r = -1;

	r = vasprintf(&csTmp,format, argptr);

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

	*dest = *src;

	return *src == '\0' ? 0 : -1; // 0: OK, -1: NOK
}

////////////////////////////////////////////////////////////////////////////////////////////////
int wcscpy_s(wchar_t *dest, size_t len, const wchar_t *src)
{
	if (dest == NULL)
		return -1;

	for ( ; len > 1 && *src != L'\0'; dest++, src++, len--)
		*dest = *src;

	*dest = *src;

	return *src == L'\0' ? 0 : -1; // 0: OK, -1: NOK
}

int wcscpy_s(wchar_t *dest, const wchar_t *src)
{
	size_t len=wcslen(src);

	if (dest == NULL)
		return -1;

	for ( ; len > 1 && *src != L'\0'; dest++, src++, len--)
		*dest = *src;

	*dest = *src;

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

// after https://www.securecoding.cert.org/confluence/pages/viewpage.action?pageId=2981930
/*#else //WIN32
int dupenv_s(char **buffer, size_t *numberOfElements, const char *varname)
{
//	size_t required;
	char* data=getenv(varname);
	if(!data)
		return EINVAL;
	*numberOfElements=strlen(data);
#ifdef WIN32
	(*buffer)=_strdup(data);
#else
	(*buffer)=strdup(data);
#endif
	if (!*buffer)
		return ENOMEM;
	return 0;
}*/
#endif
