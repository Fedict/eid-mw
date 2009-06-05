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
#ifndef _ANALYSEPCSCCARDINFO_H_
#define _ANALYSEPCSCCARDINFO_H_

#include <exception>
#include <sstream>

#include "analysis.h"
#include "pcsc.h"
#include "AnalysisError.h"


//******************************************
// PCSC card access verification
// 
// Pass:
//
// Fail:
//
//******************************************
class AnalysePCSCCardInfo : public Analysis
{
public:
	AnalysePCSCCardInfo()
	{
		m_testName = "pcsc_access";
		m_friendlyName = "PCSC card access";
	}
	virtual ~AnalysePCSCCardInfo()
	{
	}

	virtual int run()
	{
		m_bPassed = false;
		setProgress(0);
		setStartTime();
		int retVal = DIAGLIB_OK;
		Report_TYPE reportType = REPORT_TYPE_RESULT;

		try
		{
			wchar_t		sepa		= L'~';
			reportPrintHeader2(reportType, L"PCSC card access", sepa);

			//------------------------------------------
			// get the list of readers we found using PCSC
			//------------------------------------------
			Card_LIST cardList;
			int retVal = pcscGetCardList(&cardList);

			//------------------------------------------
			// card list can be generated, check if a card is inserted (size>0)
			//------------------------------------------

			if (DIAGLIB_OK!=retVal)
			{
				std::wstringstream text;
				switch(retVal)
				{
				case DIAGLIB_ERR_BAD_CALL:
					text << L"[Error] Bad function call to pcscGetCardList()";
					break;
				case DIAGLIB_ERR_LIBRARY_NOT_FOUND:
					text << L"[Error] Could not load PCSC";
					break;
				case DIAGLIB_ERR_INTERNAL:
					text << L"[Error] Internal error calling pcscGetCardList()";
					break;
				case DIAGLIB_ERR_READER_NOT_FOUND:
					text << L"[Error] No Reader found ";
					break;
				default:
					text << L"[Error] Unknown error: pcscGetCardList()";
					break;
				}
				resultToReport(reportType,text);
				processParamsToStop();
				resultToReport(reportType,m_bPassed);
				return retVal;
			}
			else
			{
				std::wstringstream text;
				text << L"[Info] Nr of cards detected by PCSC: " << cardList.size();
				resultToReport(reportType,text);

				if (0==cardList.size())
				{
					resultToReport(reportType,L"[Error] No cards detected by PCSC");
					return retVal;
				}

				for (size_t idx=0;idx<cardList.size();idx++)
				{
					Card_INFO info;
					retVal = cardGetInfo(cardList.at(idx), &info);
					switch(retVal)
					{
					case DIAGLIB_ERR_INTERNAL:
						//------------------------------------------------
						// Card_INFO* == NULL
						//------------------------------------------------
					case DIAGLIB_ERR_BAD_CALL:
						//------------------------------------------------
						// cardinfo can not be read from this list (== software error)
						//------------------------------------------------
					case DIAGLIB_ERR_NOT_AVAILABLE:
						text << L"[Error] Internal error retrieving info from card";
						break;
						//------------------------------------------------
						// user clicked 'NO" when asking to grant access to cards
						//------------------------------------------------
					case DIAGLIB_ERR_NOT_ALLOWED_BY_USER:
						text << L"[Warn ] Access to cards refused by user";
						break;
						//------------------------------------------------
						// eidlib could not be loaded
						//------------------------------------------------
					case DIAGLIB_ERR_LIBRARY_NOT_FOUND:
						text << L"[Error] Middleware library could not be loaded.";
						resultToReport(reportType,text);
						processParamsToStop();
						return retVal;
						break;
						//------------------------------------------------
						// no card in reader
						//------------------------------------------------
					case DIAGLIB_ERR_CARD_NOT_FOUND:
						text << L"[Error] Card could not be found.";
						break;
						//------------------------------------------------
						// card type not recognized
						//------------------------------------------------
					case DIAGLIB_ERR_CARD_BAD_TYPE:
						text << L"[Warn ] Card not recognized.";
						break;
						//------------------------------------------------
						// internal error
						//------------------------------------------------
					default:	// DIAGLIB_OK
						break;
					}

					if (retVal != DIAGLIB_OK)
					{
						processParamsToStop();
						resultToReport(reportType,m_bPassed);
						return retVal;
					}

					retVal = cardReportInfo(reportType, info);
				}
				m_bPassed = true;
			}
			processParamsToStop();
			resultToReport(reportType,m_bPassed);
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
			text << L"[Error] Card list could not be retrieved: ";
			switch(retVal)
			{
			case DIAGLIB_ERR_BAD_CALL:
				text << L"Bad function call ";
				break;
			case DIAGLIB_ERR_LIBRARY_NOT_FOUND:
				text << L"Library not found ";
				break;
			case DIAGLIB_ERR_INTERNAL:
				text << L"Internal error ";
				break;
			case DIAGLIB_ERR_READER_NOT_FOUND:
				text << L"No Reader found ";
				break;
			default:
				text << L"Unknown error ";
				break;
			}
		}
		resultToReport(reportType,text);
		return retVal;
	}

};

#endif
