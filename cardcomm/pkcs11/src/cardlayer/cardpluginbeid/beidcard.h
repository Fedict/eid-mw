
/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2015 FedICT.
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
#ifndef __BEIDCARD_H__
#define __BEIDCARD_H__

#include "../pkicard.h"
#include "beidp15correction.h"

using namespace eIDMW;

// If we want to 'hardcode' this plugin internally in the CAL, this function
// can't be present because it's the same for all plugins
#ifndef CARDPLUGIN_IN_CAL
EIDMW_CAL_API CCard *GetCardInstance(unsigned long ulVersion,
				     const char *csReader, SCARDHANDLE hCard,
				     CContext * poContext,
				     CPinpad * poPinpad);
#endif

CCard *BeidCardGetInstance(unsigned long ulVersion, const char *csReader,
			   SCARDHANDLE hCard, CContext * poContext,
			   CPinpad * poPinpad);

namespace eIDMW
{

	typedef enum
	{
		BELPIC_DF,
		ID_DF,
		UNKNOWN_DF,
	} tBelpicDF;

	class CBeidCard:public CPkiCard
	{
public:
		CBeidCard(SCARDHANDLE hCard, CContext * poContext,
			  CPinpad * poPinpad, const CByteArray & oData,
			  tSelectAppletMode selectAppletMode);
		~CBeidCard(void);

		virtual std::string GetPinpadPrefix();

		virtual CByteArray GetSerialNrBytes();

		/** Returns 3 bytes:
		 *   - the appletversion (1 byte): 0x10, 0x11, 0x20
		 *   - the global OS version (2 bytes) */
		virtual CByteArray GetInfo();

		virtual DlgPinUsage PinUsage2Dlg(const tPin & Pin,
						 const tPrivKey * pKey);
		virtual bool PinCmd(tPinOperation operation, const tPin & Pin,
				    const std::string & csPin1,
				    const std::string & csPin2,
				    unsigned long &ulRemaining,
				    const tPrivKey * pKey = NULL);
		virtual unsigned long PinStatus(const tPin & Pin);
		virtual bool LogOff(const tPin & Pin);

		virtual unsigned long GetSupportedAlgorithms();

		virtual CP15Correction *GetP15Correction();

		virtual unsigned int GetRSAKeySize();
		virtual unsigned char GetAppletVersion();

protected:
		virtual bool ShouldSelectApplet(unsigned char ins,
						     unsigned long ulSW12);
		virtual bool SelectApplet();

		virtual tBelpicDF getDF(const std::string & csPath,
					unsigned long &ulOffset);
		virtual tFileInfo SelectFile(const std::string & csPath,
					     bool bReturnFileInfo = false);
		virtual tFileInfo ParseFileInfo(CByteArray & oFCI);
		virtual CByteArray SelectByPath(const std::string & csPath,
						bool bReturnFileInfo = false);

		virtual unsigned long Get6CDelay();

		virtual void showPinDialog(tPinOperation operation,
					   const tPin & Pin,
					   std::string & csPin1,
					   std::string & csPin2,
					   const tPrivKey * pKey);

		virtual void SetSecurityEnv(const tPrivKey & key,
					    unsigned long algo,
					    unsigned long ulInputLen);
		virtual CByteArray SignInternal(const tPrivKey & key,
						unsigned long algo,
						const CByteArray & oData,
						const tPin * pPin = NULL);

		virtual tCacheInfo GetCacheInfo(const std::string & csPath);

		CByteArray m_oCardData;
		CByteArray m_oSerialNr;
		unsigned char m_ucAppletVersion;
		unsigned long m_ul6CDelay;

		CBeidP15Correction p15correction;
	};

}

#endif
