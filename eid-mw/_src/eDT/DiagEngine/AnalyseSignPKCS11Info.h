/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
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
#ifndef _ANALYSESIGNPKCS11INFO_H_
#define _ANALYSESIGNPKCS11INFO_H_

#include "analysis.h"
#include "pkcs.h"

//******************************************
// Perform a signing operation using the PKCS11
// 
// Pass:
//		Signing sucessful
// Fail:
//		signing failed
//******************************************
class AnalyseSignPKCS11Info : public Analysis
{
public:
	AnalyseSignPKCS11Info()
	{
		m_testName = "sign_pkcs11";
		m_friendlyName = "PKCS signing";
	}
	virtual ~AnalyseSignPKCS11Info()
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

			reportPrintHeader2(reportType, L"PKCS signing", sepa);
			resultToReport(reportType,L"[Info ] PKCS Checking availability");
			retVal = pkcsIsAvailable(&bAvailable);

			if (DIAGLIB_OK!=retVal)
			{
				processParamsToStop();
				resultToReport(reportType,L"[Error] Error calling pkcsIsAvailable()");
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

			progressIncrement();

			if (DIAGLIB_OK!=retVal)
			{
				processParamsToStop();
				resultToReport(reportType,L"[Error] Could not get card list using PCSC.");
				resultToReport(reportType,m_bPassed);
				progressRelease();
				return retVal;
			}

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

				resultToReport(reportType,L"[Info ] Test signing.");
				retVal = pkcsTestSign(cardList.at(idx), AUTH_CERT_TYPE, &bSucceed);
				if (DIAGLIB_OK!=retVal)
				{
					std::wstringstream msg;
					switch(retVal)
					{
					//------------------------------------------
					// general errors
					//------------------------------------------
					case DIAGLIB_ERR_INTERNAL:
						msg << L"[Error] PKCS internal error.";
						break;
					case DIAGLIB_ERR_LIBRARY_NOT_FOUND:
						msg << L"[Error] PKCS library not found.";
						break;
					//------------------------------------------
					// PKCS errors
					//------------------------------------------
					case DIAGLIB_ERR_PKCS_INIT_FAILED:
						msg << L"[Error] PKCS init failed";
						break;
					case DIAGLIB_ERR_PKCS_FAILED:
						msg << L"[Error] PKCS failed";
						break;
					case DIAGLIB_ERR_PKCS_SIGNING_FAILED:
						msg << L"[Error] PKCS signing failed";
						break;
					case DIAGLIB_ERR_PKCS_KEY_NOT_FOUND:
						msg << L"[Error] PKCS key not found";
						break;
					//------------------------------------------
					// PIN errors
					//------------------------------------------
					case DIAGLIB_ERR_PIN_CANCEL:
						msg << L"[Error] PKCS PIN code entry canceled";
						break;
					case DIAGLIB_ERR_PIN_BLOCKED:
						msg << L"[Error] PKCS PIN code blocked";
						break;
					case DIAGLIB_ERR_PIN_WRONG:
						msg << L"[Error] PKCS PIN code wrong";
						break;
					case DIAGLIB_ERR_PIN_FAILED:
						msg << L"[Error] PKCS PIN code entry failed";
						break;
					//------------------------------------------
					// Card errors
					//------------------------------------------
					case DIAGLIB_ERR_CARD_NOT_FOUND:
						msg << L"[Error] Card not found";
						break;
					default:
						msg << L"[Error] PKCS unknown error";
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
					resultToReport(reportType,L"[Error] PKCS did not succeed");
					resultToReport(reportType,m_bPassed);
					progressRelease();
					return retVal;
				}
			}
			progressIncrement();

			processParamsToStop();
			m_bPassed = true;
			resultToReport(reportType,L"[Info ] PKCS verification succeeded");
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
