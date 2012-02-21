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

#include "certprop.h"
#include "runcheck.h"

HANDLE                  ghSvcStopEvent = NULL;

int WINAPI WinMain(HINSTANCE inst,HINSTANCE prev,LPSTR cmd,int show) {

	WCHAR* AppName=TEXT("beidsccertprop.exe");
	DWORD dwAppsRunning = 0; 
	char* cmdptr = cmd;

	while( *cmdptr != 0)
	{
		if(*cmdptr == '-')
		{
			cmdptr++;
			switch(*cmdptr)
			{
			case 'c':
				if( CheckAppRunning(AppName, &dwAppsRunning) == CHECK_OK)
				{
					if(dwAppsRunning > 1)
					{
						//we exist already more then once, so we exit
						return 0;
					}
				}
				break;
			default:
				break;
			}
			if(*cmdptr == 0)
			{
				break;
			}
		}
		cmdptr++;
	}

	return CertProp();
}
