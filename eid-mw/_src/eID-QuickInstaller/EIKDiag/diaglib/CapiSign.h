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

#ifndef __CAPISIGN__
#define __CAPISIGN__

#include "stdafx.h"

#include <wincrypt.h>
#include "EikFiles.h"
#include "ErrorFmt.h"
#include "base64.h"


class CCapiSign {

public:
    CCapiSign();
    virtual ~CCapiSign(void);
    static string SignMessage(const Buffer& cert, const string& msg);
    static bool ImportCert(const Buffer& cert, const Buffer& tokenInfo);

protected:

private:
    static std::wstring _ToUnicode(const string& utf8string);
	static bool _StoreUserCerts(PCCERT_CONTEXT pCertContext, unsigned char KeyUsageBits, const std::wstring& containerserial);
};

#endif __CAPISIGN__
