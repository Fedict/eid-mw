
/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2014 FedICT.
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

/**
 * Pinpad support based on CCID (which uses the SCardControl()
 * function in PCSC).
 * Also supported is the loading of pinpad libraries for those
 * readers who aren't CCID compliant or to offer extra functionality
 * such as specifying the messages to be displayed.
 * See pinpad2.h
 */

#pragma once

#ifndef PINPAD_H
#define PINPAD_H

#include "p15objects.h"
#include "internalconst.h"
#include "pinpadlib.h"

#define FEATURE_VERIFY_PIN_START         0x01
#define FEATURE_VERIFY_PIN_FINISH        0x02
#define FEATURE_MODIFY_PIN_START         0x03
#define FEATURE_MODIFY_PIN_FINISH        0x04
#define FEATURE_GET_KEY_PRESSED          0x05
#define FEATURE_VERIFY_PIN_DIRECT        0x06
#define FEATURE_MODIFY_PIN_DIRECT        0x07

namespace eIDMW
{
	class CContext;

	class EIDMW_CAL_API CPinpad
	{
public:
		CPinpad(void);

		void Init(CContext * poContext, SCARDHANDLE hCard,
			  const std::string & csReader,
			  const std::string & csPinpadPrefix);
		void Init(CContext * poContext, SCARDHANDLE hCard,
			  const std::string & csReader,
			  const std::string & csPinpadPrefix,
			  CByteArray usReaderFirmVers);

		bool UsePinpad(tPinOperation operation);

		CByteArray PinCmd(tPinOperation operation,
				  const tPin & pin, unsigned char ucPinType,
				  const CByteArray & oAPDU,
				  unsigned long &ulRemaining);

protected:
		     CByteArray PinpadControl(unsigned long ulControl,
					      const CByteArray & oCmd,
					      tPinOperation operation,
					      unsigned char ucPintype,
					      const std::string & csPinLabel,
					      bool bShowDlg);
		CByteArray PinpadPPDU(BYTE cbControl, const CByteArray & oCmd,
				      tPinOperation operation,
				      unsigned char ucPintype,
				      const std::string & csPinLabel,
				      bool bShowDlg);
		void UnloadPinpadLib();
		void GetFeatureList();
		void GetPPDUFeatureList();
		unsigned long GetLanguage();
		unsigned char PinOperation2Lib(tPinOperation operation);

		unsigned char ToFormatString(const tPin & pin);
		unsigned char ToPinBlockString(const tPin & pin);
		unsigned char ToPinLengthFormat(const tPin & pin);
		unsigned char GetMaxPinLen(const tPin & pin);

		// For PIN verify and unblock without PIN change (1 PIN needed)
		CByteArray PinCmd1(tPinOperation operation,
				   const tPin & pin, unsigned char ucPinType,
				   const CByteArray & oAPDU,
				   unsigned long &ulRemaining);
		// For PIN change and unblock with PIN change (2 PINs needed)
		CByteArray PinCmd2(tPinOperation operation,
				   const tPin & pin, unsigned char ucPinType,
				   const CByteArray & oAPDU,
				   unsigned long &ulRemaining);

		CContext *m_poContext;
		SCARDHANDLE m_hCard;
#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4251)	// these strings do not need a dll interface
#endif
		            std::string m_csReader;
		            std::string m_csPinpadPrefix;
#ifdef WIN32
#pragma warning(pop)
#endif

		unsigned short m_usReaderFirmVers;
		bool m_bNewCard;
		bool m_bUsePinpadLib;
		unsigned long m_ulLangCode;

		CPinpadLib m_oPinpadLib;

		bool m_bCanVerifyUnlock;	// Can do operations with 1 PIN
		bool m_bCanChangeUnlock;	// Can do operations with 2 PINs
		unsigned long m_ioctlVerifyStart;
		unsigned long m_ioctlVerifyFinish;
		unsigned long m_ioctlVerifyDirect;
		unsigned long m_ioctlChangeStart;
		unsigned long m_ioctlChangeFinish;
		unsigned long m_ioctlChangeDirect;
		bool m_bCanUsePPDU;
	};

}
#endif
