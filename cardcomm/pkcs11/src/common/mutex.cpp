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
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&m_Mutex, &attr);
#endif
}

CMutex::~CMutex()
{
#ifdef WIN32
	EnterCriticalSection(&m_Mutex);
	LeaveCriticalSection(&m_Mutex);
	DeleteCriticalSection(&m_Mutex);
#else
	pthread_mutex_destroy(&m_Mutex);
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
