
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
#pragma once

#ifndef __PKCS15_H__
#define __PKCS15_H__

#include <vector>

#include "common/eiderrors.h"
#include "common/mwexception.h"
#include "p15objects.h"
#include "pkcs15parser.h"

namespace eIDMW
{
	class CCard;
	class CContext;

	// token flags
//	const unsigned long TOKENFLAG_READONLY = 0x00000001;
//	const unsigned long TOKENFLAG_LOGINREQUIRED = 0x00000002;
//	const unsigned long TOKENFLAG_PRNGENERATION = 0x00000004;
//	const unsigned long TOKENFLAG_EIDCOMPLIANT = 0x00000008;

	typedef struct
	{
		bool isRead;
		std::string path;
		CByteArray byteArray;

		void setDefault(std::string defaultPath = "")
		{
			isRead = false;
			path = defaultPath;
			byteArray.ClearContents();
		}
	} tPKCSFile;

	typedef enum
	{
//		EFDIR,
//		ODF,
//		TOKENINFO,
		CDF,
		PRKDF,
	} tPKCSFileName;

	class CPKCS15
	{
public:
		CPKCS15(void);
		~CPKCS15(void);

		void Clear(CCard * poCard = NULL);
		void SetCard(CCard * poCard);

		unsigned long CertCount();
		tCert GetCert(unsigned long ulIndex);
		tCert GetCertByID(unsigned long ulID);

		unsigned long PrivKeyCount();
		tPrivKey GetPrivKey(unsigned long ulIndex);
		tPrivKey GetPrivKeyByID(unsigned long ulID);

private:
		CCard * m_poCard;
		PKCS15Parser *m_poParser;

#ifdef WIN32
// Get rid of warnings like "warning C4251: 'eIDMW::CPKCS15::m_oPins' : class 'std::vector<_Ty>'
// needs to have dll-interface to be used by clients of class 'eIDMW::CPKCS15'"
// See http://groups.google.com/group/microsoft.public.vc.stl/msg/c4dfeb8987d7b8f0
#pragma warning(push)
#pragma warning(disable:4251)
#endif
		std::vector < tPin > m_oPins;
		std::vector < tCert > m_oCertificates;
#ifdef WIN32
#pragma warning(pop)
#endif

		// files on the card

		tPKCSFile m_xAODF;
		tPKCSFile m_xCDF;
		tPKCSFile m_xPrKDF;

		tPKCSFile m_xPin;	// there could be more than one
		tPKCSFile m_xPrKey;
		tPKCSFile m_xCert;

		// read method for level 3 (aodf,cdf and prkdf)
		void ReadLevel3(tPKCSFileName name);

		void ReadFile(tPKCSFile * pFile, int upperLevel);
	};

}
#endif
