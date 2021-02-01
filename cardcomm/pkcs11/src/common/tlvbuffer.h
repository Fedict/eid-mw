
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
// TLVBuffer.h: interface for the CTLVBuffer class.
//
//////////////////////////////////////////////////////////////////////

#ifndef TLVBUFFER_H
#define TLVBUFFER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdlib.h>
#include <string.h>
#include <map>
#include "tlv.h"

namespace eIDMW
{

	class CTLVBuffer
	{
public:
		CTLVBuffer();
		virtual ~ CTLVBuffer();

		void SetTagData(unsigned char ucTag,
					      const unsigned char *pucData,
					      unsigned long ulLen);
		unsigned long Extract(unsigned char *pucData,
						    unsigned long ulLen);
		int ParseTLV(const unsigned char *pucData,
					   unsigned long ulLen);
		bool ParseFileTLV(const unsigned char *pucData,
						unsigned long ulLen);
		CTLV *GetTagData(unsigned char ucTag);
		void FillASCIIData(unsigned char ucTag,
						 char *pData,
						 unsigned long *pulLen);
		bool FillUTF8Data(unsigned char ucTag,
						char *pData,
						unsigned long *pulLen);
		void FillBinaryStringData(unsigned char ucTag,
							char *pData,
							unsigned long
							*pulLen);
		void FillLongData(unsigned char ucTag,
						long *piData);
		void FillBinaryData(unsigned char ucTag,
						  unsigned char *pData,
						  unsigned long *pulLen);
		bool FillBinaryDataCheck(unsigned char ucTag,
						       unsigned char *pData,
						       unsigned long
						       *pulMaxLen);
		unsigned long GetLengthNeeded();

private:
		static char *Hexify(unsigned char *pData,
				    unsigned long ulLen);

		bool TlvEncodeLen(unsigned long ulLenVal,
				  unsigned char *pucBufDest, long *piBufLen);
		bool TlvDecodeLen(const unsigned char *pucBufSrc,
				  long *piBufLen, unsigned long *pulLenVal);

		static char hexChars[];

		typedef std::map < unsigned char, CTLV * >MapTLV;
		typedef MapTLV::iterator ITMap;

		MapTLV m_oMapTLV;
	};

}			     // namespace eIDMW

#endif			     // TLVBUFFER_H
