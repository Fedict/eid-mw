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

#include <algorithm>
#include "repository.h"
#include "WinSCardDllLocksRule.h"

WinSCardDllLocksRule::WinSCardDllLocksRule()
:MetaRule(L"winscard.dll.lockers",L"Are Unexpected Processes Locking winscard.dll?"),
	m_key(L"module_info.winscard_dll.lockedby"),
	m_expected()
{
	m_expected.insert	(L"winlogon.exe");
	m_expected.insert	(L"Explorer.EXE");
	m_expected.insert	(L"svchost.exe");
	m_expected.insert	(L"Firefox.exe");
}

MetaRuleVerdict WinSCardDllLocksRule::verdict(Repository evidence) const
{
	ContributionSet found=evidence.values(m_key);
	ContributionSet un_expected;
	std::set_difference(found.begin(),found.end(),m_expected.begin(),m_expected.end(),std::inserter(un_expected,un_expected.end()));
	if(un_expected.empty())
	{
		return MetaRuleVerdict(false);
	}
	else
	{
		std::wstring guilties(L"[");
		for(ContributionSet::const_iterator i=un_expected.begin();i!=un_expected.end();)
		{
			guilties.append(*i++);
			if(i!=un_expected.end())
				guilties.append(L",");
		}
		guilties.append(L"]");

		return MetaRuleVerdict(true,L"The following processes were unexpectedly locking winscard.dll : " + guilties,L"Find out if these are necessary and/or try stopping them");
	}
}