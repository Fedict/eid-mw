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
#ifndef __DIAGLIB_REPOSITORY_H__
#define __DIAGLIB_REPOSITORY_H__

#include <stdio.h>
#include <map>
#include <vector>
#include <string>
using namespace std;

typedef std::map<std::wstring,std::wstring>			ContributionMap;	
typedef std::pair<std::wstring,std::wstring>		ContributionEntry;
typedef std::vector<std::wstring>					PrefixList;

class Repository
{
public:
	void prefix		(const std::wstring format, ...);
	void prefix		(const std::string newPrefix);
	void unprefix	(void);
	void available	(bool avail);

	void				contribute (const std::wstring key, const std::wstring format, ...);
	const std::wstring	retrieve(const std::wstring fullKey) const;

	const ContributionMap& results(void) const;

	// Meyers Singleton
	static Repository& instance()	
	{
		static Repository theRepository;
		return theRepository;
	}

private:
	Repository();
	void prefixChanged(void);
	std::wstring		m_current_prefix;
	PrefixList			m_prefixes;
	ContributionMap		m_contributions;
	static Repository*	m_singleton;
};

// readability convenience macros
#define REP_CONTRIBUTE	Repository::instance().contribute
#define REP_PREFIX		Repository::instance().prefix
#define REP_UNPREFIX	Repository::instance().unprefix
#define REP_AVAILABLE	Repository::instance().available
#define REP_RESULTS		Repository::instance().results

#endif //__DIAGLIB_REPOSITORY_H__
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
#ifndef __DIAGLIB_REPOSITORY_H__
#define __DIAGLIB_REPOSITORY_H__

#include <stdio.h>
#include <map>
#include <vector>
#include <string>
using namespace std;

typedef std::map<std::wstring,std::wstring>			ContributionMap;	
typedef std::pair<std::wstring,std::wstring>		ContributionEntry;
typedef std::vector<std::wstring>					PrefixList;

class Repository
{
public:
	void prefix		(const std::wstring format, ...);
	void prefix		(const std::string newPrefix);
	void unprefix	(void);
	void available	(bool avail);

	void				contribute (const std::wstring key, const std::wstring format, ...);
	const std::wstring	retrieve(const std::wstring fullKey) const;

	const ContributionMap& results(void) const;

	// Meyers Singleton
	static Repository& instance()	
	{
		static Repository theRepository;
		return theRepository;
	}

private:
	Repository();
	void prefixChanged(void);
	std::wstring		m_current_prefix;
	PrefixList			m_prefixes;
	ContributionMap		m_contributions;
	static Repository*	m_singleton;
};

// readability convenience macros
#define REP_CONTRIBUTE	Repository::instance().contribute
#define REP_PREFIX		Repository::instance().prefix
#define REP_UNPREFIX	Repository::instance().unprefix
#define REP_AVAILABLE	Repository::instance().available
#define REP_RESULTS		Repository::instance().results

#endif //__DIAGLIB_REPOSITORY_H__
