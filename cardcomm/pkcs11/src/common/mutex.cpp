
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
#include "mutex.h"
#include "log.h"
#include <errno.h>

#ifndef TRUE
#define TRUE true
#endif
#ifndef FALSE
#define FALSE false
#endif
#ifndef WIN32
#include "thread.h"
#endif

namespace eIDMW
{

	CMutex::CMutex()
	{
#ifdef WIN32
		InitializeCriticalSection(&m_Mutex);
#else
		pthread_mutexattr_t attr;
		                    pthread_mutexattr_init(&attr);
		         
			 
			 
			 
			 
			 
			pthread_mutexattr_settype(&attr,
						  PTHREAD_MUTEX_RECURSIVE);
		                    pthread_mutex_init(&m_Mutex, &attr);
#endif
	}

	CMutex:: ~CMutex()
	{
#ifdef WIN32
		EnterCriticalSection(&m_Mutex);
		LeaveCriticalSection(&m_Mutex);
		DeleteCriticalSection(&m_Mutex);
#else
		// We should maybe use a conditional macro to do the
		// __builtin_expect below so that it only gets used when we're
		// using a compiler which supports that. However, since we're
		// already in the non-win32 section here, we're either compiling
		// on OSX (clang) or Linux (gcc or clang), which means we do
		// have support for that. So ignore.
		if (__builtin_expect(pthread_mutex_destroy(&m_Mutex), 0))
		{
			try {
				switch (errno)
				{
					case EBUSY:
						MWLOG(LEV_CRIT, MOD_LIB,
						      L"trying to destroy a mutex which is still in use!");
						break;
					case EINVAL:
						MWLOG(LEV_CRIT, MOD_LIB,
						      L"trying to destroy an invalid mutex!");
						break;
				}
			} catch(...) {
			}
		}
#endif
	}

	void CMutex::Lock()
	{
#ifdef WIN32
		EnterCriticalSection(&m_Mutex);
#else
		pthread_mutex_lock(&m_Mutex);
#endif
	}

	void CMutex::Unlock()
	{
#ifdef WIN32
		LeaveCriticalSection(&m_Mutex);
#else
		pthread_mutex_unlock(&m_Mutex);
#endif
	}

}
