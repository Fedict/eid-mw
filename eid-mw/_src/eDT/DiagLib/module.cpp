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
#include "diaglib.h"
#include "module.h"
#include "process.h"
#include "error.h"
#include "log.h"
#include "progress.h"
#include "Repository.h"


int moduleReportInfo(Report_TYPE type,ModuleName name,ProcNameSet names)
{
	int							iReturnCode = DIAGLIB_OK;
	unsigned int				count;
	ModuleSet::const_iterator	iter;

	reportPrint(type,L"     Name = %ls\n", name.c_str());
	for(iter=names.begin(),count=0;iter!=names.end();iter++,count++)
		reportPrint(REPORT_TYPE_COMPLEMENT,	L"Locked By = [%ls]\n",iter->c_str());
	reportPrintSeparator(type, REPORT_PROCESS_SEPARATOR);
	return iReturnCode;
}

void moduleContributeInfo(ModuleName name,ProcNameSet names)
{
	unsigned int				count;
	ProcNameSet::const_iterator	iter;
	wchar_t						usesKey[32];

	REP_PREFIX(name.c_str());
	for(iter=names.begin(),count=0;iter!=names.end();iter++,count++)
	{
		wsprintf(usesKey,L"LockedBy[%08ld]",count);
		REP_CONTRIBUTE(usesKey,iter->c_str());
	}
	REP_UNPREFIX();
}


////////////////////////////////////////////////////////////////////////////////////////////////
int moduleReportList(Report_TYPE type, const Proc_LIST &processList, const wchar_t *TitleIn)
{
	int				iReturnCode=DIAGLIB_OK;
	std::wstring	Title;
	ModuleMap		modules;

	if(TitleIn!=NULL)
		Title=TitleIn;
	else
		Title=L"Module list";

	reportPrintHeader2(type, Title.c_str(), REPORT_PROCESS_SEPARATOR);

	progressInit(processList.size());

	Proc_INFO info;
	Proc_LIST::const_iterator itr;
	for(itr=processList.begin();itr!=processList.end();itr++)
	{
		if(DIAGLIB_OK == processGetInfo(*itr,&info))
		{
			for(ModuleSet::const_iterator i=info.modulesLoaded.begin();i!=info.modulesLoaded.end();i++)
				modules[*i].insert(info.Name);
		}
		progressIncrement();
	}

	for(ModuleMap::const_iterator i=modules.begin();i!=modules.end();i++)
	{
		moduleReportInfo(type,i->first,i->second);
		moduleContributeInfo(i->first,i->second);
	}

	progressRelease();

	return iReturnCode;
}

#endif