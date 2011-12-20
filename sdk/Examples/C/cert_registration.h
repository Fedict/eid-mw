/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2009-2010 FedICT.
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
#ifndef cert_registration_h
#define cert_registration_h

#include <windows.h>
#include <stdio.h>
#include "base.h"

DWORD CheckCertificateExist(char * pCardSerialNumber, BOOL* pbSignatureContainerFound, BOOL* pbAuthenticationContainerFound);
DWORD ImportCertificate(BYTE* certData, DWORD certSize, CK_BYTE* cardSerialNumber, CK_ULONG SerialNumberValueLen);

#endif
