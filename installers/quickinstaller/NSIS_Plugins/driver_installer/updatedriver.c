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

#include "updatedriver.h"

BOOL CopyInf(char* infFilePath) {

	DWORD _lastErrorCode;
	const char* inffile = infFilePath;

	if( MAX_PATH < strlen(infFilePath) )
		return FALSE;

	if (SetupCopyOEMInf(
		inffile,				// full path to .inf file
		NULL,					// path of .inf is path of driver files
		SPOST_PATH,				// copy driver path
		SP_COPY_NOOVERWRITE,	// don't repolace if already present
		NULL,					// NA not interested in destination .inf filename
		0,						// NA size of .inf filename returned buffer
		NULL,					// NA not interested in returned size
		NULL)) {				// NA not interested in DestinationInfFileNameComponent 
			return TRUE;
	}
	_lastErrorCode = GetLastError();
	if (_lastErrorCode == ERROR_FILE_EXISTS)
		return TRUE;
	else
		return FALSE;

}


BOOL IsDeviceInstallInprogress(DWORD dwTimeOutMillis) {

	HMODULE hModule;
	CMP_WAITNOPENDINGINSTALLEVENTS_PROC pCMP_WaitNoPendingInstallEvents;

	hModule = GetModuleHandle(TEXT("setupapi.dll"));
	if(!hModule) 
	{
		// Should never happen since we're linked to SetupAPI, but...
		return FALSE;
	}

	pCMP_WaitNoPendingInstallEvents = (CMP_WAITNOPENDINGINSTALLEVENTS_PROC)GetProcAddress(hModule, "CMP_WaitNoPendingInstallEvents");
	if(!pCMP_WaitNoPendingInstallEvents)
	{
		// We're running on a release of the operating system that doesn't supply this function.
		// Trust the operating system to suppress AutoRun when appropriate.
		return FALSE;
	}
	return (pCMP_WaitNoPendingInstallEvents(dwTimeOutMillis) == WAIT_TIMEOUT);

}

BOOL eIDUpdateDriver(HWND hwndParent, char* hardwareID, char* infFilePath, DWORD flags, BOOL* bootRequired,DWORD* pLastErrorCode) {

	// make sure no driver install is in progress
	if (IsDeviceInstallInprogress(5000)) 
	{
		*pLastErrorCode = GetLastError();
		return FALSE;
	}
	// inform Windows & PnP about the new driver, so it can be installed when the device is plugged in
	if (!CopyInf(infFilePath)) 
	{
		*pLastErrorCode = GetLastError();
		return FALSE;
	}

	if (UpdateDriverForPlugAndPlayDevices(hwndParent, hardwareID, infFilePath, flags, bootRequired)) 
	{
		return TRUE;
	} 
	else 
	{
		*pLastErrorCode = GetLastError();
		if ( (*pLastErrorCode == ERROR_NO_SUCH_DEVINST) ||  (*pLastErrorCode == ERROR_NO_MORE_ITEMS) )
		{
			return TRUE;
		}
		return FALSE;
	}

}
