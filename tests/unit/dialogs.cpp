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
extern "C" {
#include <testlib.h>
}
#include <string>
#include <dialogs/dialogs.h>


TEST_FUNC(sdialogs) {
	
	if (!have_pin()){ 
		fprintf(stderr, "cannot test dialogs without the ability to enter a pin code\n");
		return TEST_RV_SKIP;
	}
	
	//bad pin dialog diolog with 2 attempts and 1 attempt left
	eIDMW::DlgPinUsage usage = eIDMW::DLG_PIN_AUTH;
	const wchar_t * pin = L"name";
	unsigned long attempts = 2;
	eIDMW::DlgRet ret = eIDMW::DLG_OK;
	printf("badpin ok button test, please select the ok button.\n");
	ret = eIDMW::DlgBadPin(usage,pin,attempts);
	if (ret != eIDMW::DLG_OK){ return TEST_RV_FAIL;}
	
	attempts = 1;
	printf("badpin cancel button test, please select the cancel button.");
	ret = eIDMW::DlgBadPin(usage,pin,attempts);
	if (ret == eIDMW::DLG_ERR||ret == eIDMW::DLG_BAD_PARAM){ return TEST_RV_FAIL;}
	
	//askpin dialog (operation, usage, pinname, pinInfo, pin, bufferlen) to check the pin
 	ret = eIDMW::DLG_OK; 
 	eIDMW::DlgPinOperation operation = eIDMW::DLG_PIN_OP_VERIFY;
	wchar_t * pinname ;
 	eIDMW::DlgPinInfo pin1Info = { 4, 12, 0};
 	eIDMW::DlgPinInfo pin2Info = { 4, 12, 0};
 	wchar_t pin1[20];
 	wchar_t pin2[20];
 	unsigned long bufferlen1 = sizeof(pin1);
 	unsigned long bufferlen2 = sizeof(pin2);
 	
 	printf("pincheck ok button test, please select the ok button.\n");
 	ret = eIDMW::DlgAskPin(operation, usage, pinname, pin1Info, pin1, bufferlen1);
 	if (ret != eIDMW::DLG_OK){ return TEST_RV_FAIL;}
 	
 	printf("pincheck cancel button test, please select the cancel button.\n");
 	ret = eIDMW::DlgAskPin(operation, usage, pinname, pin1Info, pin1, bufferlen1);
 	if (ret != eIDMW::DLG_CANCEL){ return TEST_RV_FAIL;}
	
        //askpins dialog (operation, usage, pinname, pinInfo1, pin1, bufferlen1, pinInfo2, pin2, bufferlen2) to change the pin
        operation = eIDMW::DLG_PIN_OP_CHANGE;
	printf("pinchange ok button test, please select the ok button twice.\n");
 	ret = eIDMW::DlgAskPins(operation, usage, pinname, pin1Info, pin1, bufferlen1, pin2Info, pin2, bufferlen2);
	if (ret != eIDMW::DLG_OK){ return TEST_RV_FAIL;}
 	
 	printf("pinchange cancel button test, please select the ok button and then the cancel button.\n");
 	ret = eIDMW::DlgAskPins(operation, usage, pinname, pin1Info, pin1, bufferlen1, pin2Info, pin2, bufferlen2);
	if (ret != eIDMW::DLG_CANCEL){ return TEST_RV_FAIL;}
  	
	//display pinpad info and close
	printf("pinpad info starting\n");
	wchar_t * reader;
	wchar_t * message;
	unsigned long handle;

	ret = eIDMW::DlgDisplayPinpadInfo( operation, reader, usage, pinname, message, &handle);
	if (ret != eIDMW::DLG_OK){ return TEST_RV_FAIL;}
	eIDMW::DlgClosePinpadInfo(handle);
	return TEST_RV_OK;
}

/*
closepinpadinfo()

*/	

	
	
