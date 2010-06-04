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
#ifdef WIN32
#include <windows.h>
#elif __APPLE__
#include "mac/mac_helper.h"
#endif

#include "reader.h"
#include "error.h"

#include "device.h"
#include "pcsc.h"

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int readerGetListFromDevice(Reader_LIST *readersList)
{
	return deviceGetReaderList(readersList);
}

////////////////////////////////////////////////////////////////////////////////////////////////
int readerGetListFromPcsc(Reader_LIST *readersList)
{
	return pcscGetReaderList(readersList);
}

////////////////////////////////////////////////////////////////////////////////////////////////
int readerGetInfo(Reader_ID reader, Reader_INFO *info)
{
	int iReturnCode = DIAGLIB_OK;

	if(info == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	info->id=reader;	

	return iReturnCode;
}


////////////////////////////////////////////////////////////////////////////////////////////////
int readerGetCardInfo(Card_ID id, Card_INFO *info)
{
	if(info == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	return pcscGetCardInfo(id, info);
} 

