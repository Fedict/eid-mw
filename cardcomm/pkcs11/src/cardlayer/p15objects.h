
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
#ifndef P15OBJECTS_H
#define P15OBJECTS_H

#include <string>
#include "cardlayerconst.h"

namespace eIDMW
{
	// common object flags
	const unsigned long COFLAG_PRIVATE = 0x00000001;
	const unsigned long COFLAG_MODIFIABLE = 0x00000002;

	// A card-independent way to indentify cards
	typedef enum
	{
		PIN_USG_UNKNOWN,
		DLG_USG_PIN_AUTH,
		DLG_USG_PIN_SIGN,
		DLG_USG_PIN_ADDRESS,
	} PinUsage;

	// key usage flags
	const unsigned long KEYUFLAG_ENCRYPT = 0x00000001;
	const unsigned long KEYUFLAG_DECRYPT = 0x00000002;
	const unsigned long KEYUFLAG_SIGN = 0x00000004;
	const unsigned long KEYUFLAG_SIGNRECOVER = 0x00000008;
	const unsigned long KEYUFLAG_WRAP = 0x00000010;
	const unsigned long KEYUFLAG_UNWRAP = 0x00000020;
	const unsigned long KEYUFLAG_VERIFY = 0x00000040;
	const unsigned long KEYUFLAG_VERIFYRECOVER = 0x00000080;
	const unsigned long KEYUFLAG_DERIVE = 0x00000100;
	const unsigned long KEYUFLAG_NONREPUDIATION = 0x00000200;

	// key access flags 
	const unsigned long KEYAFLAG_SENSITIVE = 0x00000001;
	const unsigned long KEYAFLAG_EXTRACTABLE = 0x00000002;
	const unsigned long KEYAFLAG_ALWAYSSENSITIVE = 0x00000004;
	const unsigned long KEYAFLAG_NEVEREXTRACTABLE = 0x00000008;
	const unsigned long KEYAFLAG_LOCAL = 0x00000010;

	typedef struct
	{
		bool bValid;
		// common object attributes
		std::string csLabel;
		unsigned long ulAuthID;
		unsigned long ulIndex;
		unsigned long ulMinLen;
		unsigned long ulMaxLen;
		unsigned long ulStoredLen;
	} tPin;

	typedef enum {
		RSA,
		EC,
	} tKeyType;
	typedef struct
	{
		bool bValid;
		std::string csLabel;
		unsigned long ulUserConsent;

		unsigned long ulID;
		unsigned long ulKeyUsageFlags;
		unsigned long ulKeyRef;

		unsigned long ulKeyLenBytes;
		tKeyType keyType;
	} tPrivKey;

	typedef struct
	{
		bool bValid;
		// common object attributes
		std::string csLabel;
		unsigned long ulFlags;
		unsigned long ulAuthID;
		unsigned long ulUserConsent;

		// common certificate attributes
		unsigned long ulID;
		bool bAuthority;
		bool bImplicitTrust;

		// X509 certificate attributes
		std::string csPath;

	} tCert;

}
#endif
