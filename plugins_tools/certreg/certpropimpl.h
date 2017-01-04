
/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2011-2012 FedICT.
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
 
#ifndef IMPORTCERTIMPL_H_
#define IMPORTCERTIMPL_H_
#include <winscard.h>
#include <string>
#include <wincrypt.h>
	
#define CARDREADER_POLL_INTERVAL 5000
#define MAX_CARD_SERIAL_SIZE     100
	using std::string;
using std::wstring;

#include "../common/log.h"
#include "../common/config.h"
#include "../cardlayer/p15objects.h"
#include "../cardlayer/cardlayer.h"
#include "../cardlayer/context.h"
	DWORD ImportCertificates(const char *readerName, char *pSerialNumber,
				  size_t cSerialNumber);
DWORD RemoveCertificates(char *pSerialNumber);
DWORD StoreAuthorityCerts(PCCERT_CONTEXT pCertContext,
			   unsigned char KeyUsageBits);
DWORD StoreUserCerts(PCCERT_CONTEXT pCertContext, unsigned char KeyUsageBits,
		      const char *cardSerialNumber);
 
#endif	/* IMPORTCERTIMPL_H_ */
