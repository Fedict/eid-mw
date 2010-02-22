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
#ifndef _ANALYSECARDDETECTINFO_H_
#define _ANALYSECARDDETECTINFO_H_

#include "analysis.h"
#include "report.h"
#include "card.h"

//******************************************
// Check the list of crds inserted using PCSC
// 
// Pass:
//		cards inserted
// Fail:
//		no cards inserted or PCSC not available
//******************************************
class AnalyseCardDetectInfo : public Analysis
{
public:
	AnalyseCardDetectInfo()
	{
		m_testName = "card_detect";
		m_friendlyName = "PCSC card list";
	}
	virtual ~AnalyseCardDetectInfo()
	{
	}
	virtual int run()
	{
		m_bPassed = false;
		int retVal = DIAGLIB_OK;
		Report_TYPE reportType = REPORT_TYPE_RESULT;
		setProgress(0);
		setStartTime();

		try
		{
			//------------------------------------------
			// write to the report what we're doing
			//------------------------------------------
			wchar_t		sepa		= L'~';
			reportPrintHeader2(reportType, L"Detecting card inserted using PCSC", sepa);

			//------------------------------------------
			// get the cardlist using PCSC
			//------------------------------------------
			resultToReport(reportType,L"[Info ] Retrieving card list");
			Card_LIST cardList;
			retVal = getPCSCCardList(reportType,&cardList);

			if (DIAGLIB_OK!=retVal)
			{
				switch(retVal)
				{
				case DIAGLIB_ERR_BAD_CALL:
					std::wstring(L"[Error] Bad function call to getPCSCCardList()");
					break;
				case DIAGLIB_ERR_LIBRARY_NOT_FOUND:
					std::wstring(L"[Error] Could not load PCSC");
					break;
				case DIAGLIB_ERR_INTERNAL:
					std::wstring(L"[Error] Internal error calling getPCSCCardList()");
					break;
				default:
					std::wstring(L"[Error] Unknown error: getPCSCCardList()");
					break;
				}
				processParamsToStop();
				resultToReport(reportType,m_bPassed);
				return retVal;
			}

			//------------------------------------------
			// card list can be generated, check if a card is inserted (size>0)
			//------------------------------------------
			if (0==cardList.size())
			{
				processParamsToStop();
				resultToReport(reportType,L"[Error] No eID card found");
				resultToReport(reportType,m_bPassed);
				return retVal;
			}

			std::wstringstream text;
			text << L"[Info ] Nr of cardreaders with eID card inserted: " << cardList.size();
			resultToReport(reportType,text);

			bool bPassed = true;

			for (size_t cardIdx=0; cardIdx<cardList.size();cardIdx++)
			{
				Reader_ID reader = cardList.at(cardIdx).Reader;
				Reader_INFO info;
				retVal = readerGetInfo(reader, &info);

				if (DIAGLIB_OK!=retVal)
				{
					resultToReport(reportType,L"[Error] Error getting reader information");
					std::wstringstream txt;
					txt << L"[Error] Reader:" << reader.Name;
					resultToReport(reportType,txt);
					bPassed = false;
				}

				retVal = readerReportInfo(reportType, info);

				if (DIAGLIB_OK!=retVal)
				{
					resultToReport(reportType,L"[Error] Error reporting info of reader");
					std::wstringstream txt;
					txt << L"[Error] Reader:" << reader.Name;
					resultToReport(reportType,txt);
					bPassed = false;
				}
			}
			if (1<cardList.size())
			{
				bPassed = false;
				processParamsToStop();
				resultToReport(reportType,L"[Error] More than 1 (one) card detected");
				resultToReport(reportType,bPassed);
				return retVal;
			}

			m_bPassed = bPassed;
		}
		//------------------------------------------
		// exception from writing to the report
		//------------------------------------------
		catch (ExcReport& exc) 
		{
			processParamsToStop();
			retVal = exc.getErr();
		}

		processParamsToStop();
		return retVal;
	}
private:
	int getPCSCCardList(Report_TYPE reportType, Card_LIST* cardList)
	{
		resultToReport(reportType,L"[Info ] Generating card list using PCSC");

		int retVal = cardGetListFromPcsc(cardList);

		std::wstringstream text;
		if (retVal != DIAGLIB_OK)
		{
			resultToReport(reportType,L"[Error] Card list could not be retrieved");

			switch(retVal)
			{
			case DIAGLIB_ERR_BAD_CALL:
				text << L"[Error] Bad function call";
				break;
			case DIAGLIB_ERR_LIBRARY_NOT_FOUND:
				text << L"[Error] Library not found";
				break;
			case DIAGLIB_ERR_READER_NOT_FOUND:
				text << L"[Error] No Reader found";
				break;
			case DIAGLIB_ERR_INTERNAL:
				text << L"[Error] Internal error";
				break;
			default:
				text << L"[Error] Unknown error";
				break;
			}
		}
		else
		{
			text << L"[Info ] List of cards generated" ;
		}
		resultToReport(reportType,text);
		return retVal;
	}
};

#endif
