/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2020 FedICT.
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
#ifdef WIN32
#include <win32.h>
#pragma pack(push, cryptoki, 1)
#include "pkcs11.h"
#pragma pack(pop, cryptoki)
#else
#include <unix.h>
#include <pkcs11.h>
#endif
#include <stdio.h>
#include "testlib.h"
#include <string>
#include <dialogs/dialog.h>


TEST_FUNC(sdialogs) {
	
	//bad pin dialog
	DlgPinUsage usage = DLG_PIN_AUTH;
	const wchar_t * pin = L"name";
	unsigned long attempts = 2;
	if (DlgBadPin(usage,pin,attempts) != DLG_OK){return TEST_RV_OK;}
	

        //askpin dialog (operation, usage, pinname, pin1, buffer1, pin2, buffer2) 
 	DlgPinOperation operation = DLG_PIN_OP_VERIFY;
 	wchar_t * pinname = L"test";
 	DlgPinInfo pin1Info = new DLGPinInfo( 4, 12, 0) ;
 	DlgPinInfo pin2Info = new DLGPinInfo( 4, 12, 0)  ;
 	wchar_t * pin1 = "1234";
 	wchar_t * pin2 = "5678";
 	unsigned long buffer1 = 1024;
 	unsigned long buffer2 = 1024;
  	if (DlgAskPins(operation, usage, pinname, pin1Info, pin1, buffer1, pin2Info, pin2, buffer2)!=){return TEST_RV_OK;}
  	 
	return TEST_RV_FAIL;

}
/*
not:
displaypinpadinfo()
closepinpadinfo()

??
askaccess()
*/	

	
	
