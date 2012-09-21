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
#ifndef __UPDATEDRIVER__
#define __UPDATEDRIVER__


#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0500	// Change this to the appropriate value to target other versions of Windows.
#endif	
#include <windows.h>

#define _SETUP_APIVER 0x0500
#include <SetupApi.h>
#include <newdev.h>
#include <cfgmgr32.h>

typedef DWORD (WINAPI *CMP_WAITNOPENDINGINSTALLEVENTS_PROC)(DWORD time_out); 
BOOL eIDUpdateDriver(HWND hwndParent, char* hardwareID, char* infFilePath, DWORD flags, BOOL* bootRequired,DWORD* pLastErrorCode);
#endif __UPDATEDRIVER__
