
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
#pragma once
#ifndef MWEXCEPTION_H
#define MWEXCEPTION_H


#include <exception>
#include <iostream>

namespace eIDMW
{

        using namespace std;

	class CMWException:public std::exception
	{
public:
		//CMWException(long lError);
		CMWException(long lError, const char *cpFile, long lLine);
		~CMWException() throw()
		{
		};
		virtual const char *what() const throw();

		long GetError() const
		{
			return m_lError;
		};
		      std::string GetFile() const
		{
			return m_sFile;
		};
		long GetLine() const
		{
			return m_lLine;
		};

protected:
#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4251)	//m_sFile should not be exported by the beidcommon.dll
#endif
		      std::string m_sFile;
#ifdef WIN32
#pragma warning(pop)
#endif

		long m_lError;
		long m_lLine;
	};

	class CNotAuthenticatedException:public CMWException
	{
public:
		CNotAuthenticatedException(long lError);
	};

#define CMWEXCEPTION(i)	CMWException(i, __FILE__, __LINE__)

}
#endif
