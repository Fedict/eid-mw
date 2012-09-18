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
#include "common.h"
#include "getreadercount.h"

CK_RV GetTheReaderCount(int* nrofCardReaders, int cardsInserted){

	CK_RV retval = CKR_OK;				//return value of last pkcs11 function called
	CK_FUNCTION_LIST_PTR functions=NULL;		// list of the pkcs11 function pointers

	CK_ULONG slot_count = 0;
	CK_BBOOL tokenPresent = CK_FALSE;

	int err = GetLastError();
	printf("err = %d\n",err);

	SetLastError(0);

	retval=loadpkcs11(&functions);
	if( retval != CKR_OK )
		return retval;

	if( (retval=(functions->C_Initialize) (NULL)) == CKR_OK)
	{		
		if(cardsInserted > 0)
		{
			tokenPresent = CK_TRUE;
		}

		retval = (functions->C_GetSlotList) (tokenPresent, 0, &slot_count);
		*nrofCardReaders = slot_count;

		(functions->C_Finalize) (NULL_PTR);
	}

	unloadpkcs11();

	return retval;
} 
