
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
#ifndef __PKCS15PARSER_H__
#define __PKCS15PARSER_H__

#include "common/bytearray.h"
#include <vector>
#include "p15objects.h"

namespace eIDMW
{

	typedef struct commonObject
	{
		std::string csLabel;
		unsigned long ulFlags;
		unsigned long ulAuthID;
		unsigned long ulUserConsent;

		int lastPosition;

		void Reset()
		{
			csLabel = "";
			ulFlags = 0x0;
			ulAuthID = 0x0;
			ulUserConsent = 0x0;
			lastPosition = 0;
		}
	} tCommonObjAttr;

	class PKCS15Parser
	{
public:
		//tDirInfo ParseDir(const CByteArray & contents);

		//tTokenInfo ParseTokenInfo(const CByteArray & contents);

		//tOdfInfo ParseOdf(const CByteArray & contents);

		std::vector < tCert > ParseCdf(const CByteArray & contents);

private:

		void FillCOACert(tCert * cert, tCommonObjAttr * coa);
	};

}

#endif
