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
#ifndef _ANALYSESIGNCSPINFO_H_
#define _ANALYSESIGNCSPINFO_H_

#include "analysis.h"
#include "csp.h"

//******************************************
// Perform a signing operation using CSP (windows only)
// To do this, the certificates will be registered in the store
// 
// Pass:
//		signing successful
// Fail:
//		signing failed
//******************************************
class AnalyseSignCSPInfo : public Analysis
{
public:
	AnalyseSignCSPInfo()
	{
		m_testName = "sign_csp";
		m_friendlyName = "CSP signing";
	}
	virtual ~AnalyseSignCSPInfo()
	{
	}
	virtual int run()
	{
		int retVal = DIAGLIB_OK;
		m_bPassed = false;
		setProgress(0);
		setStartTime();
		bool bAvailable = false;
		Report_TYPE reportType = REPORT_TYPE_RESULT;

		try
		{

			progressInit(2);

			wchar_t		sepa		= L'~';

			reportPrintHeader2(reportType, L"CSP signing", sepa);
			resultToReport(reportType,L"[Info ] CSP Checking availability");
			retVal = cspIsAvailable(&bAvailable);

			if (DIAGLIB_OK!=retVal)
			{
				processParamsToStop();
				resultToReport(reportType,L"[Error] Error calling cspIsAvailable()");
				resultToReport(reportType,m_bPassed);
				progressRelease();
				return retVal;
			}

			if (!bAvailable)
			{
				processParamsToStop();
				resultToReport(reportType,L"[Error] CSP not available, could not acquire context.");
				resultToReport(reportType,m_bPassed);
				progressRelease();
				return retVal;
			}
			//------------------------------------------
			// Get the card list using PCSC
			//------------------------------------------
			Card_LIST cardList;
			retVal = cardGetListFromPcsc(&cardList);
			if (DIAGLIB_OK!=retVal)
			{
				processParamsToStop();
				resultToReport(reportType,L"[Error] Could not get card list using PCSC.");
				resultToReport(reportType,m_bPassed);
				progressRelease();
				return retVal;
			}

			progressIncrement();

			if (0==cardList.size())
			{
				processParamsToStop();
				resultToReport(reportType,L"[Error] No eID card present in cardreader(s).");
				resultToReport(reportType,m_bPassed);
				progressRelease();
				return retVal;
			}

			if (1<cardList.size())
			{
				processParamsToStop();
				resultToReport(reportType,L"[Error] More than one eID card inserted");
				resultToReport(reportType,m_bPassed);
				progressRelease();
				return retVal;
			}

			for (size_t idx=0; idx<cardList.size(); idx++)
			{
				bool bSucceed = false;

				resultToReport(reportType,L"[Info ] Test signing");
				retVal = cspTestSign(cardList.at(idx), AUTH_CERT_TYPE, &bSucceed);
				if (DIAGLIB_OK!=retVal)
				{
					std::wstringstream msg;
					switch(retVal)
					{
					//------------------------------------------
					// general errors
					//------------------------------------------
					case DIAGLIB_ERR_INTERNAL:
						msg << L"[Error] CSP internal error";
						break;
					//------------------------------------------
					// CSP errors
					//------------------------------------------
					case DIAGLIB_ERR_CSP_REGISTRATION_FAILED:
						msg << L"[Error] CSP certificate registration failed";
						break;
					case DIAGLIB_ERR_CSP_CONTEXT_FAILED:
						msg << L"[Error] CSP acquire context failed";
						break;
					case DIAGLIB_ERR_CSP_FAILED:
						msg << L"[Error] CSP operation hashing/signing failed";
					    break;
					//------------------------------------------
					// PIN errors
					//------------------------------------------
					case DIAGLIB_ERR_PIN_CANCEL:
						msg << L"[Error] CSP PIN code entry canceled";
						break;
					case DIAGLIB_ERR_PIN_BLOCKED:
						msg << L"[Error] CSP PIN code blocked";
						break;
					case DIAGLIB_ERR_PIN_WRONG:
						msg << L"[Error] CSP PIN code wrong";
						break;
					case DIAGLIB_ERR_PIN_FAILED:
						msg << L"[Error] CSP PIN code entry failed";
						break;
					default:
						msg << L"[Error] CSP unknown error";
					    break;
					}
					processParamsToStop();
					resultToReport(reportType,msg);
					resultToReport(reportType,m_bPassed);
					progressRelease();
					return retVal;
				}
				if (!bSucceed)
				{
					processParamsToStop();
					resultToReport(reportType,L"[Error] CSP did not succeed");
					resultToReport(reportType,m_bPassed);
					progressRelease();
					return retVal;
				}
			}
			progressIncrement();

			processParamsToStop();
			m_bPassed = true;
			resultToReport(reportType,L"[Info ] CSP verification succeeded");
			resultToReport(reportType,m_bPassed);
			progressRelease();
			return retVal;
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
