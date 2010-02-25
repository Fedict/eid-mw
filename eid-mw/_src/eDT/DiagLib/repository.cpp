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
#ifdef WIN32
#include <windows.h>
#elif defined __APPLE__
#include "Mac/mac_helper.h"
#endif

#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

#include "diaglib.h"
#include "report.h"
#include "file.h"

#include "error.h"
#include "log.h"
#include "folder.h"
#include <map>
#include "repository.h"
#include "util.h"

Repository::Repository()
:m_contributions(),m_current_prefix(L"default"),m_prefixes()
{
}

void Repository::prefixChanged(void)
{
	m_current_prefix.clear();
	for(PrefixList::size_type i=0;i<m_prefixes.size();i++)
	{
		m_current_prefix.append(m_prefixes[i]);
		if(i<m_prefixes.size())
			m_current_prefix.append(L".");
	}	
}

void Repository::prefix(const std::wstring format, ...)
{
	wchar_t val[1024];
	va_list args;
	va_start(args,format);
	std::wstring cleanFormat(format);wstring_clean(cleanFormat);
	vswprintf(val,sizeof(val)-2,cleanFormat.c_str(),args);
	va_end(args);
	m_prefixes.push_back(val);
	prefixChanged();
}

void Repository::prefix(const std::string newPrefix)
{
	prefix(wstring_From_string(newPrefix));
}

void Repository::unprefix()
{
	m_prefixes.pop_back();
	prefixChanged();
}

void Repository::available(bool avail)
{
	contribute(L"available",avail?L"true":L"false");
}

void Repository::contribute(const std::wstring key, const std::wstring format, ...)
{
	wchar_t _val[1024];
	va_list args;
	va_start(args,format);
	vswprintf(_val,sizeof(_val)-2,format.c_str(),args);
	va_end(args);
	std::wstring val(_val);

	if(val.length()>0)
	{
		std::wstring	 fullKey(m_current_prefix);
						 fullKey.append(key);
		wstring_to_lower(fullKey);
		m_contributions.insert(ContributionEntry(fullKey,val));
	}
}

ContributionSet	Repository::values(const std::wstring key) const
{
	std::set<std::wstring> vls;
	for(ContributionMap::const_iterator i=m_contributions.lower_bound(key);i!=m_contributions.upper_bound(key);i++)
		vls.insert(i->second);
	return vls;
}

const ContributionMap& Repository::results(void) const
{
	return m_contributions;
}