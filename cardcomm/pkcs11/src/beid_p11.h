
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
#ifndef __beid_p11_h__
#define __beid_p11_h__

#ifndef _WIN32
#include "unix.h"
#include "pkcs11.h"
#else
#include "win32.h"
#pragma pack(push, cryptoki, 1)
#include "pkcs11.h"
#pragma pack(pop, cryptoki)
#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL	0
#else
#define NULL	((void *) 0)
#endif
#endif


#define BEID_FILE_ID					"3F00DF014031"
#define BEID_FILE_ID_SIGN				"3F00DF014032"
#define BEID_FILE_ADDRESS				"3F00DF014033"
#define BEID_FILE_ADDRESS_SIGN			"3F00DF014034"
#define BEID_FILE_PHOTO					"3F00DF014035"
#define BEID_FILE_CERT_RRN				"3F00DF00503C"
#define BEID_FILE_CERT_RRNCA			"3F00DF00503D"
#define BEID_FILE_BASIC_KEY				"3F00DF014040"
#define BEID_FILE_TOKENINFO				"3F00DF005032"

#define BEID_FIELD_TAG_ID_Version				0x00
#define BEID_FIELD_TAG_ID_CardNr				0x01
#define BEID_FIELD_TAG_ID_ChipNr				0x02
#define BEID_FIELD_TAG_ID_ValidityBeginDate		0x03
#define BEID_FIELD_TAG_ID_ValidityEndDate		0x04
#define BEID_FIELD_TAG_ID_IssuingMunicipality	0x05
#define BEID_FIELD_TAG_ID_NationalNr			0x06
#define BEID_FIELD_TAG_ID_Surname				0x07
#define BEID_FIELD_TAG_ID_FirstNames			0x08
#define BEID_FIELD_TAG_ID_FirstLetterOfThirdGivenName	0x09
#define BEID_FIELD_TAG_ID_Nationality			0x0A
#define BEID_FIELD_TAG_ID_LocationOfBirth		0x0B
#define BEID_FIELD_TAG_ID_DateOfBirth			0x0C
#define BEID_FIELD_TAG_ID_Gender				0x0D
#define BEID_FIELD_TAG_ID_Nobility				0x0E
#define BEID_FIELD_TAG_ID_DocumentType			0x0F
#define BEID_FIELD_TAG_ID_SpecialStatus			0x10
#define BEID_FIELD_TAG_ID_PhotoHash				0x11
#define BEID_FIELD_TAG_ID_Duplicata				0x12
#define BEID_FIELD_TAG_ID_SpecialOrganization	0x13
#define BEID_FIELD_TAG_ID_MemberOfFamily		0x14
#define BEID_FIELD_TAG_ID_DateAndCountryOfProtection	0x15
#define BEID_FIELD_TAG_ID_WorkPermitType		0x16
#define BEID_FIELD_TAG_ID_Vat1					0x17
#define BEID_FIELD_TAG_ID_Vat2					0x18
#define BEID_FIELD_TAG_ID_RegionalFileNumber	0x19
#define BEID_FIELD_TAG_ID_BasicKeyHash			0x1A
#define BEID_FIELD_TAG_ID_BrexitMention1		0x1B
#define BEID_FIELD_TAG_ID_BrexitMention2		0x1C
#define BEID_FIELD_TAG_ID_CardAMention1			0x1D
#define BEID_FIELD_TAG_ID_CardAMention2			0x1E
#define BEID_FIELD_TAG_ID_CardEUStartDate		0x1F

#define BEID_FIELD_TAG_ADDR_Version				0x00
#define BEID_FIELD_TAG_ADDR_Street				0x01
#define BEID_FIELD_TAG_ADDR_ZipCode             0x02
#define BEID_FIELD_TAG_ADDR_Municipality        0x03

#define BEID_FIELD_TAG_TOKINF_graph				0x00
#define BEID_FIELD_TAG_TOKINF_elec				0x01
#define BEID_FIELD_TAG_TOKINF_elecint           0x02

/************************/

/* differences with 2.20*/

/************************/

/*#define CK_TRUE   TRUE
#define CK_FALSE  FALSE
#define CKM_SHA256_RSA_PKCS            0x00000040
#define CKM_SHA384_RSA_PKCS            0x00000041
#define CKM_SHA512_RSA_PKCS            0x00000042
*/

#endif
