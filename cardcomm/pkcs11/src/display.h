/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2013 FedICT.
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
#ifndef __display_h__
#define __display_h__
#include "beid_p11.h"

#define P11_DISPLAY_YES				0
#define P11_DISPLAY_NO				1
#define P11_DISPLAY_CANCEL		2
#define P11_DISPLAY_ALWAYS		3

#ifdef __cplusplus
extern "C" {
#endif

CK_BYTE AllowCardReading(void);

#ifdef __cplusplus
   }
#endif

#endif

