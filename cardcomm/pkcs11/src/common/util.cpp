
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
#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#include "configuration.h"
#endif
#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <locale>

#include <stdlib.h>

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "util.h"
#include "mw_util.h"

char a_cHexChars[] =
	{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C',
   'D', 'E', 'F' };

namespace eIDMW
{

	std::wstring utilStringWiden(const std::string & in,
				     const std::locale & locale)
	{
		std::wstring out(in.size(), 0);

		for (std::string::size_type i = 0; in.size() > i; ++i)
			out[i] = std::use_facet < std::ctype < wchar_t >
				>(locale).widen(in[i]);
		return out;

	}

	std::     string utilStringNarrow(const std::wstring & in,
					  const std::locale & locale)
	{
		std::string out(in.size(), 0);

		for (std::wstring::size_type i = 0; in.size() > i; ++i)
            // in the gcc implementation of std::locale narrow needs 2 arguments
            // (the second is a default char, here the choice is random)
#ifdef WIN32
#ifdef _GLIBCXX_RELEASE
            out[i] = std::use_facet < std::ctype < wchar_t >
                >(locale).narrow(in[i], 'x');
#else
			out[i] = std::use_facet < std::ctype < wchar_t >
				>(locale).narrow(in[i]);
#endif
#else
			out[i] = std::use_facet < std::ctype < wchar_t >
				>(locale).narrow(in[i], 'x');
#endif
		return out;
	}

/**
 * Case insensitve search, csSearch should be in lower case.
 * Returns true is csSearch is present in csData.
 */
	bool StartsWithCI(const char *csData, const char *csSearch)
	{
		for (const char *pc1 = csData, *pc2 = csSearch; *pc2 != '\0';
		     pc1++, pc2++)
		{
			if (*pc1 != tolower(*pc2))
			{
				return false;
			}
		}

		return true;
	}

/* convert binary to ascii-hexadecimal, terminate with a 00-byte
   You have to free the returned buffer yourself !!
 */
	char *bin2AsciiHex(const unsigned char *pData, unsigned long ulLen)
	{
		char *pszHex = new char[ulLen * 2 + 1];

		if (pData != NULL)
		{
			int j = 0;

			for (unsigned long i = 0; i < ulLen; i++)
			{
				pszHex[j++] =
					a_cHexChars[pData[i] >> 4 & 0x0F];
				pszHex[j++] = a_cHexChars[pData[i] & 0x0F];
			}
			pszHex[j] = 0;
		}
		return pszHex;
	}

/* Return the name of the process that calls the library
 */
#ifdef WIN32
#include <windows.h>
	//typedef DWORD (WINAPI *DYN_GETPROCESSNAME)(HANDLE,LPWSTR,DWORD); 
	typedef DWORD(WINAPI * DYN_GETPROCESSNAME) (HANDLE, HMODULE, LPWSTR,
						    DWORD);
#endif
//#ifdef __APPLE__
//#include <Carbon/Carbon.h>
//#endif

	void GetProcessName(wchar_t * wBuffer, unsigned long ulLen)
	{
		try
		{
#ifdef WIN32
			HMODULE hLibrary = LoadLibrary(L"psapi.dll");

			if (!hLibrary)
				return;

			DYN_GETPROCESSNAME getProcessName;

			HANDLE hProcess =
				OpenProcess(PROCESS_ALL_ACCESS, FALSE,
					    GetCurrentProcessId());

			//getProcessName = (DYN_GETPROCESSNAME) GetProcAddress(hLibrary, "GetProcessImageFileNameW"); 
			getProcessName =
				(DYN_GETPROCESSNAME) GetProcAddress(hLibrary,
								    "GetModuleFileNameExW");

			// If the function address is valid, call the function.

			if (NULL != getProcessName)
			{
				//if(getProcessName(hProcess,wBuffer,ulLen)==0)
				if (getProcessName
				    (hProcess, NULL, wBuffer, ulLen) == 0)
					wcscpy_s(wBuffer, ulLen, L"");
			}

			CloseHandle(hProcess);
			FreeLibrary(hLibrary);
#elif defined __APPLE__
/*
			CFBundleRef thisBundle = CFBundleGetMainBundle();

			if (thisBundle != NULL)
			{
				CFURLRef thisurl =
					CFBundleCopyExecutableURL(thisBundle);
				if (thisurl != NULL)
				{
					UInt8 csPath[250];
					UInt8 csPathLen = sizeof(csPath);
					CFIndex index =
						CFURLGetBytes(thisurl, csPath,
							      csPathLen);
					if (index != -1)
					{
						if (index < csPathLen)
						{
							csPath[index] = '\0';
						} else
						{
							csPath[csPathLen -
							       1] = '\0';
						}
						std::string strPath =
							(char *) csPath;
						wcscpy_s(wBuffer, ulLen,
							 utilStringWiden
							 (strPath).c_str());

					}
					CFRelease(thisurl);
				}
			}
*/
			/*       ProcessSerialNumber xPSN = {kNoProcess, kCurrentProcess};
			   FSRef location;
			   OSStatus ret = GetProcessBundleLocation (&xPSN, &location);
			   if (noErr == ret)
			   {
			   unsigned char csPath[250];
			   ret = FSRefMakePath(&location, csPath, sizeof(csPath));
			   if (noErr == ret)
			   {
			   std::string strPath = (char *) csPath;
			   wcscpy_s(wBuffer, ulLen, utilStringWiden(strPath).c_str());
			   }
			   } */
#else
			char linkname[64];	/* /proc/<pid>/exe */
			pid_t pid;
			int ret;

			/* Get our PID and build the name of the link in /proc */
			pid = getpid();

			if (snprintf
			    (linkname, sizeof(linkname), "/proc/%i/exe",
			     pid) < 0)
			{
				/* This should only happen on large word systems. I'm not sure
				   what the proper response is here.
				   Since it really is an assert-like condition, aborting the
				   program seems to be in order. */
				//abort();
				return;
			}
			/* Now read the symbolic link */
			char *buf = new char[ulLen];

			ret = readlink(linkname, buf, (size_t) ulLen);

			/* In case of an error, leave the handling up to the caller */
			/* Report insufficient buffer size */
			if (ret == -1 || (unsigned long) ret >= ulLen)
			{
				delete[]buf;
				return;
			}

			buf[ret] = 0;	//The string coming from readlink is not null-terminated

			wcscpy_s(wBuffer, ulLen,
				 utilStringWiden(buf).c_str());

			delete[]buf;

#endif
		}
		catch( ...)
		{
		}
	}
}

#ifndef WIN32

#include "assert.h"

int sprintf_s(char *buffer, size_t sizeOfBuffer, const char *format, ...)
{
	va_list args;
	char *csTmp = NULL;
	int r = -1;

	va_start(args, format);
	r = vasprintf(&csTmp, format, args);
	va_end(args);

	if (r != -1 && csTmp != NULL)
	{
		r = strcpy_s(buffer, sizeOfBuffer, csTmp);
		free(csTmp);
	}

	return r;
}

int strcat_s(char *dest, size_t len, const char *src)
{
	if (dest == NULL)
		return -1;

	for (; *dest != '\0' && len > 1; dest++, len--)
		;

	for (; len > 1 && *src != '\0'; dest++, src++, len--)
		*dest = *src;

	*dest = '\0';

	return *src == '\0' ? 0 : -1;	// 0: OK, -1: NOK
}

int strncpy_s(char *dest, size_t len, const char *src, long count)
{

	if (dest == NULL)
		return -1;

	char *dest_start = dest;
	size_t orig_len = len;

	for (; len > 1 && *src != '\0' && count > 0;
	     dest++, src++, len--, count--)
		*dest = *src;

	*dest = '\0';

	if (*src == '\0' || count == 0)
		return 0;	// OK

	if (orig_len > 0)
		*dest_start = '\0';

	return -1;
}

int fopen_s(FILE ** pFile, const char *filename, const char *mode)
{
	int r = 0;

	if (pFile == NULL)
		return -1;

	FILE *f = fopen(filename, mode);

	if (f != NULL)
		*pFile = f;
	else
		r = -1;

	return r;
}

int fprintf_s(FILE * stream, const char *format, ...)
{
	va_list args;
	char *csTmp = NULL;
	int r = -1;

	va_start(args, format);
	r = vasprintf(&csTmp, format, args);
	va_end(args);

	if (r != -1 && csTmp != NULL)
	{
		r = fputs(csTmp, stream);
		free(csTmp);
	}

	return r;
}

int vfprintf_s(FILE * stream, const char *format,
			     va_list argptr)
{
	char *csTmp = NULL;
	int r = -1;

	r = vasprintf(&csTmp, format, argptr);

	if (r != -1 && csTmp != NULL)
	{
		r = fputs(csTmp, stream);
		free(csTmp);
	}

	return r;
}

errno_t freopen_s(FILE ** pFile, const char *filename,
				const char *mode, FILE * stream)
{
	FILE *fd;

	fd = freopen(filename, mode, stream);
	if (!fd)
	{
		return 2;	/* No such file or directory (dummy) */
	}
	*pFile = fd;
	return 0;
}

#endif
