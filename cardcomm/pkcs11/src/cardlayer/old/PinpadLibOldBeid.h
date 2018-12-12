
/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2017 FedICT.
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

/**
 * Class that takes care of the older BE eID pinpad libs
 * (xipass, cashmouse, ..).
 * This class is needed on Windows only because the old
 * pinpad libs were only for Windows.
 */
#ifndef __PINPADLIBOLDBEID_H__
#define __PINPADLIBOLDBEID_H__

#include "../common/DynamicLib.h"
#include "../common/ByteArray.h"

#include <io.h>
#include <Windows.h>
#include "scr.h"		// the old API
#include "../pinpad2.h"		// the new API

typedef long (*BEID_OLD_PP_INIT) (const char *szReader, DWORD version, SCR_SupportConstants * supported);
typedef long (*BEID_OLD_VERIFY_PIN) (const SCR_Card *, BYTE pinID, const SCR_PinUsage *, const SCR_Application *, BYTE * cardStatus);
typedef long (*BEID_OLD_CHANGE_PIN) (const SCR_Card *, BYTE pinID, const SCR_Application *, BYTE * cardStatus);

namespace eIDMW
{
	class CPinpadLibOldBeid
	{
public:
		CPinpadLibOldBeid();
		~CPinpadLibOldBeid();

		bool Load(unsigned long hContext, SCARDHANDLE hCard,
			  const std::string & csReader, const std::string & csPinpadPrefix, unsigned long ulLanguage);

		void UnLoad();

		CByteArray PinCmd(SCARDHANDLE hCard, unsigned long ulControl, CByteArray oCmd, unsigned char ucPintype,
				  unsigned char ucOperation, unsigned long ulLangCode);

		bool UseOldLib();

private:
		bool CheckLib(const std::string & csPinpadDir,
				   const char *csFileName,
				   unsigned long ulLanguage,
				   unsigned long hContext, SCARDHANDLE hCard,
				   const char *csReader);

		CDynamicLib m_oPinpadLib;
		int m_iLangIdx;

		BEID_OLD_VERIFY_PIN m_pVerifyPin;
		BEID_OLD_CHANGE_PIN m_pChangePin;
	};

}

#endif
