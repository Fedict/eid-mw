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
#ifndef _ANALYSEMETA_H_
#define _ANALYSEMETA_H_

#include <map>
#include "analysis.h"
#include "Repository.h"
#include "MetaRule.h"
#include "MetaRuleVerdict.h"
#include "WinSCardDllLocksRule.h"

class AnalyseMeta : public Analysis
{
public:
	AnalyseMeta()
	{
		m_testName = "diagnostics";
		m_friendlyName = "Diagnostics";
	}

	virtual ~AnalyseMeta()
	{
	}

	void judge(MetaRule* rule)
	{
		MetaRuleVerdict verdict=rule->verdict(REPOSITORY);
		if(verdict.guilty())
		{
			commentToReport(REPORT_TYPE_RESULT,rule->name());
			commentToReport(REPORT_TYPE_RESULT,verdict.verdict());
			commentToReport(REPORT_TYPE_RESULT,verdict.corrective());
		}
	}

	virtual int run()
	{
		int retVal = DIAGLIB_OK;
		m_bPassed = true;
		setProgress(0);
		setStartTime();
		Report_TYPE reportType = REPORT_TYPE_RESULT;

		try
		{
			progressInit(1);
			reportPrintHeader2(reportType, L"Meta Diagnostics",L'#');
			
			judge(&WinSCardDllLocksRule());

			ContributionMap contributions=REP_RESULTS();
			for(ContributionMap::const_iterator i=contributions.begin();i!=contributions.end();i++)
			{
				std::wstring	contribStr(i->first);
								contribStr.append(L"=");
								contribStr.append(i->second);
				commentToReport(reportType,contribStr);
			}
				
			progressIncrement();
		}
		//------------------------------------------
		// exception from writing to the report
		//------------------------------------------
		catch (ExcReport& exc) 
		{
			processParamsToStop();
			retVal = exc.getErr();
		}
		resultToReport(reportType,m_bPassed);
		progressRelease();
		return retVal;
	}
};

#endif
