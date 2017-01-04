
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
#include "context.h"
#include "common/configuration.h"

namespace eIDMW
{
	CContext::CContext()
	{
		m_bSSO = CConfig::GetLong(CConfig::
					  EIDMW_CONFIG_PARAM_SECURITY_SINGLESIGNON)
			!= 0;

		m_ulConnectionDelay =
			CConfig::GetLong(CConfig::
					 EIDMW_CONFIG_PARAM_GENERAL_CARDCONNDELAY);
	}

	CContext::~CContext()
	{
		m_oPCSC.ReleaseContext();
	}
}
