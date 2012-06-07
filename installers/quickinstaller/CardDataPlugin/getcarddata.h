/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2012 FedICT.
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
#ifndef plugin_getcarddata_h
#define plugin_getcarddata_h

#include "common.h"

CK_RV ReadTheCardData(void);
CK_RV FindAndStore(CK_FUNCTION_LIST_PTR functions, CK_SESSION_HANDLE session_handle, const char* label, char** data_storage);

#endif
