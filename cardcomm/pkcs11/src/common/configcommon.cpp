
/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2011 FedICT.
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
#include "configuration.h"

// Common declaration to File and Registry Config
namespace eIDMW
{

//GENERAL
	const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_GENERAL_INSTALLDIR =
		{ EIDMW_CNF_SECTION_GENERAL, EIDMW_CNF_GENERAL_INSTALLDIR, L"$home" };
	const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_GENERAL_LANGUAGE =
		{ EIDMW_CNF_SECTION_GENERAL, EIDMW_CNF_GENERAL_LANGUAGE, L"en" };
	const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GENERAL_CARDTXDELAY =
		{ EIDMW_CNF_SECTION_GENERAL, EIDMW_CNF_GENERAL_CARDTXDELAY, 3 };
	const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GENERAL_CARDCONNDELAY =
		{ EIDMW_CNF_SECTION_GENERAL, EIDMW_CNF_GENERAL_CARDCONNDELAY, 0 };

//LOGGING
	const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_LOGGING_DIRNAME =
		{ EIDMW_CNF_SECTION_LOGGING, EIDMW_CNF_LOGGING_DIRNAME, L"$home" };
	const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_LOGGING_PREFIX =
		{ EIDMW_CNF_SECTION_LOGGING, EIDMW_CNF_LOGGING_PREFIX, L".BEID" };
	const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_LOGGING_FILENUMBER =
		{ EIDMW_CNF_SECTION_LOGGING, EIDMW_CNF_LOGGING_FILENUMBER, 2 };
	const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_LOGGING_FILESIZE =
		{ EIDMW_CNF_SECTION_LOGGING, EIDMW_CNF_LOGGING_FILESIZE, 100000 };
	const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_LOGGING_LEVEL =
		{ EIDMW_CNF_SECTION_LOGGING, EIDMW_CNF_LOGGING_LEVEL, L"error" };
	const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_LOGGING_GROUP =
		{ EIDMW_CNF_SECTION_LOGGING, EIDMW_CNF_LOGGING_GROUP, 0 };

//SECURITY
	const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_SECURITY_SINGLESIGNON =
		{ EIDMW_CNF_SECTION_SECURITY, EIDMW_CNF_SECURITY_SINGLESIGNON, 1 };

//GUI
	const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GUITOOL_VIRTUALKBD =
		{ EIDMW_CNF_SECTION_GUITOOL, EIDMW_CNF_GUITOOL_VIRTUALKBD, 0 };

}				// namespace eidMW
