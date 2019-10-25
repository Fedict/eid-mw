
/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2012 FedICT.
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
//new P15 implementation
/*
- TokenInfo aplication id=30 (iAppVersion)is decoded but not specified in the standard !!!
*/

#include <iostream>
#include <stdio.h>

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "common/eiderrors.h"
#include "common/mwexception.h"
#include "common/util.h"
#include "pkcs15parser.h"

#include "asn1.h"

//extern "C" {
//#include "asn1.c"
//}

namespace eIDMW {

#define CASN1Object int     //not needed for non-cyrptlib implementation, just to eliminate temporary compile errors


	/* convert variable length binary bit-stream into int-type */
	static unsigned int bin2int(const unsigned char *p_ucDat, unsigned int iLen)
	{
		unsigned int uiResult = 0;

		//parameter check
		if (iLen > 4)
			throw CMWEXCEPTION(EIDMW_WRONG_ASN1_FORMAT);

		//add all bytes
		while (iLen--)
		{
			uiResult = uiResult << 8 | *(p_ucDat++);
		}
		return uiResult;
	}


	/* convert variable length binary bit-stream into unsigned long-type */
	static unsigned long bin2ulong(const unsigned char *p_ucDat, unsigned int iLen)
	{
		unsigned long ulResult = 0;

		//parameter check
		if (iLen > 8)
			throw CMWEXCEPTION(EIDMW_WRONG_ASN1_FORMAT);

		//add all bytes
		while (iLen--)
		{
			ulResult = ulResult << 8 | *(p_ucDat++);
		}
		return ulResult;
	}



	static unsigned int p15_bitstring2ul(unsigned char *in, unsigned long l_in)
	{
		unsigned long   lBits = 0L;
		unsigned char   *p = in + 1;
		int             i, j;

		//put first bit (most left bit of first asn1 byte) in LSB of flags (unsinged int)
		//so asn1 bytes are swapped before keeping in unsigned int
		if (l_in == 0)
			return 0;
		l_in--;
		for (i = 0; i < (int)l_in; p++, i++)
		{
			for (j = 7; j >= 0; j--)
			{
				lBits |= ((*p >> j) & 0x01) << ((7 - j) + (i * 8));
			}
		}

		return(lBits);
	}



	//----------------------------------------------------------------------------------------

	tCommonObjAttr ParseCommonObjectAttributes(ASN1_ITEM* p_xLev0Item)
	{
		tCommonObjAttr coAttr;

		ASN1_ITEM           xLev1Item;  // CommonObjectAttributes sequence
		ASN1_ITEM           xLev2Item;  // Optional elements


		coAttr.Reset();

		//--- get sequence
		if ((p_xLev0Item->l_data < 2) || (asn1_next_item(p_xLev0Item, &xLev1Item) != 0) || (xLev1Item.tag != ASN_SEQUENCE))
			throw CMWEXCEPTION(EIDMW_WRONG_ASN1_FORMAT);

		//--- scan the common object attributes (all optional)
		while (xLev1Item.l_data > 0)
		{
			//get next object
			if ((xLev1Item.l_data < 2) || (asn1_next_item(&xLev1Item, &xLev2Item) != 0))
				throw CMWEXCEPTION(EIDMW_WRONG_ASN1_FORMAT);

			switch (xLev2Item.tag)
			{
			case ASN_UTF8STRING:  //label
			{
				std::string csLabel((char *)xLev2Item.p_data, 0, xLev2Item.l_data);
				coAttr.csLabel = csLabel;
			}
			break;

			case ASN_BIT_STRING:   // common object flags
				coAttr.ulFlags = p15_bitstring2ul((unsigned char*)xLev2Item.p_data, xLev2Item.l_data);
				break;

			case ASN_OCTET_STRING: // authority id
				coAttr.ulAuthID = bin2ulong(xLev2Item.p_data, xLev2Item.l_data);
				break;

			case ASN_INTEGER:     // user consent
				coAttr.ulUserConsent = bin2ulong(xLev2Item.p_data, xLev2Item.l_data);
				break;

			case ASN_SEQUENCE:    // access control rules
			default:  // do not yet know this item
				break;
			}
		}

		return coAttr;
	}

	//----------------------------------------------------------------------------------------

	// parse a path object
	std::string ParsePath(ASN1_ITEM* p_xLev0Item      // in: Item with single nested sequence of Octet-string
	)
	{
		std::string     csPath = "";
		ASN1_ITEM       xLev1Item;

		//check input
		if ((p_xLev0Item->tag != ASN_SEQUENCE))
			throw CMWEXCEPTION(EIDMW_WRONG_ASN1_FORMAT);

		//get path
		if ((p_xLev0Item->l_data < 2) || (asn1_next_item(p_xLev0Item, &xLev1Item) != 0) || ((xLev1Item.tag) != ASN_OCTET_STRING))
			throw CMWEXCEPTION(EIDMW_WRONG_ASN1_FORMAT);

		//convert to ascii-hex string
		char* p_cBuf = bin2AsciiHex(xLev1Item.p_data, xLev1Item.l_data);
		std::string csData(p_cBuf, xLev1Item.l_data * 2);
		csPath = csData;
		delete[] p_cBuf;

		return csPath;
	}

	std::string ParsePath2(ASN1_ITEM* p_xLev0Item      // in: Item with double nested sequence of Octet-string
	)
	{
		std::string     csPath = "";
		ASN1_ITEM       xLev1Item;
		ASN1_ITEM       xLev2Item;

		//check input
		if ((p_xLev0Item->tag != ASN_SEQUENCE))
			throw CMWEXCEPTION(EIDMW_WRONG_ASN1_FORMAT);

		//get path
		if ((p_xLev0Item->l_data < 2) || (asn1_next_item(p_xLev0Item, &xLev1Item) != 0) || ((xLev1Item.tag) != ASN_SEQUENCE))
			throw CMWEXCEPTION(EIDMW_WRONG_ASN1_FORMAT);

		//get path
		if ((xLev1Item.l_data < 2) || (asn1_next_item(&xLev1Item, &xLev2Item) != 0) || ((xLev2Item.tag) != ASN_OCTET_STRING))
			throw CMWEXCEPTION(EIDMW_WRONG_ASN1_FORMAT);

		//convert to ascii-hex string
		char* p_cBuf = bin2AsciiHex(xLev2Item.p_data, xLev2Item.l_data);
		std::string csData(p_cBuf, xLev2Item.l_data * 2);
		csPath = csData;
		delete[] p_cBuf;

		return csPath;
	}

	//----------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------

	void PKCS15Parser::FillCOACert(tCert* cert, tCommonObjAttr *coa) {
		cert->csLabel = coa->csLabel;
		cert->ulFlags = coa->ulFlags;
		cert->ulAuthID = coa->ulAuthID;
		cert->ulUserConsent = coa->ulUserConsent;
	}

	//----------------------------------------------------------------------------------------


  /* parse P15 top structure into AID/LABEL/PATH
  */
/*	tDirInfo PKCS15Parser::ParseDir(const CByteArray & contents)
	{
		tDirInfo    dir;
		ASN1_ITEM           xLev0Item;  // 
		ASN1_ITEM           xLev1Item;  // 
		ASN1_ITEM           xLev2Item;  // 


		//check if top-level = APLI.1
		xLev0Item.p_data = (unsigned char*)contents.GetBytes();
		xLev0Item.l_data = contents.Size();
		if ((asn1_next_item(&xLev0Item, &xLev1Item) != 0) || (xLev1Item.tag != ASNP15_DIRREC))
			throw CMWEXCEPTION(EIDMW_WRONG_ASN1_FORMAT);

		// get second level data of apli 15/16/17
		// the order in which the objects appear is not checked
		while (xLev1Item.l_data > 0)
		{
			if (asn1_next_item(&xLev1Item, &xLev2Item) != 0)
				throw CMWEXCEPTION(EIDMW_WRONG_ASN1_FORMAT);

			switch (xLev2Item.tag)
			{
			case ASNP15_DIRAID:
			{
				char* p_cBuf = bin2AsciiHex(xLev2Item.p_data, xLev2Item.l_data);
				std::string csData(p_cBuf, xLev2Item.l_data * 2);
				dir.oAppID = csData;
				delete[] p_cBuf;
			}
			break;
			case ASNP15_DIRLABEL:
			{
				std::string csData((char *)xLev2Item.p_data, 0, xLev2Item.l_data);
				dir.csLabel = csData;
			}
			break;
			case ASNP15_DIRPATH:
			{
				char* p_cBuf = bin2AsciiHex(xLev2Item.p_data, xLev2Item.l_data);
				std::string csData(p_cBuf, xLev2Item.l_data * 2);
				dir.csAppPath = csData;
				delete[] p_cBuf;
			}
			break;
			case 19: // object ID
			default:
				// ignore these objects
				break;
		}
	}
		return dir;
}*/

	//----------------------------------------------------------------------------------------
/*
	tTokenInfo PKCS15Parser::ParseTokenInfo(const CByteArray & contents) {
#ifdef VERBOSE
		std::cerr << "\n----------- PKCS15Parser::ParseTokenInfo ------------ " << std::endl;
#endif
		tTokenInfo tInfo;
		ASN1_ITEM           xLev0Item;  // 
		ASN1_ITEM           xLev1Item;      //
		ASN1_ITEM           xLev2Item;      //

		//---check top-level sequence
		xLev0Item.p_data = (unsigned char*)contents.GetBytes();
		xLev0Item.l_data = contents.Size();
		if ((asn1_next_item(&xLev0Item, &xLev1Item) != 0) || (xLev1Item.tag != ASN_SEQUENCE))
			throw CMWEXCEPTION(EIDMW_WRONG_ASN1_FORMAT);

		//--- get Version 
		if ((xLev1Item.l_data < 2) || (asn1_next_item(&xLev1Item, &xLev2Item) != 0) || (xLev2Item.tag != ASN_INTEGER))
			throw CMWEXCEPTION(EIDMW_WRONG_ASN1_FORMAT);
		tInfo.iVersion = bin2int(xLev2Item.p_data, xLev2Item.l_data);

		//--- get Serial number
		if ((xLev1Item.l_data < 2) || (asn1_next_item(&xLev1Item, &xLev2Item) != 0) || (xLev2Item.tag != ASN_OCTET_STRING))
			throw CMWEXCEPTION(EIDMW_WRONG_ASN1_FORMAT);

		std::string csSerial((char *)xLev2Item.p_data, 0, xLev2Item.l_data);
		tInfo.csSerial = csSerial;

		if ((xLev1Item.l_data < 2) || (asn1_next_item(&xLev1Item, &xLev2Item) != 0))
			throw CMWEXCEPTION(EIDMW_WRONG_ASN1_FORMAT);


		//--- get optional manufactureID
		tInfo.csManufactID = "";            //default value
		if (xLev2Item.tag == ASN_UTF8STRING)
		{
			std::string csManufactID((char *)xLev2Item.p_data, 0, xLev2Item.l_data);
			tInfo.csManufactID = csManufactID;

			if ((xLev1Item.l_data < 2) || (asn1_next_item(&xLev1Item, &xLev2Item) != 0))
				throw CMWEXCEPTION(EIDMW_WRONG_ASN1_FORMAT);
		}

		//--- get optional label
		tInfo.csLabel = "";                 //default value
		if (xLev2Item.tag == ASN_PRIMITIVE_CONTEXT_N(0))
		{
			std::string csLabel((char *)xLev2Item.p_data, 0, xLev2Item.l_data);
			tInfo.csLabel = csLabel;

			if ((xLev1Item.l_data < 2) || (asn1_next_item(&xLev1Item, &xLev2Item) != 0))
				throw CMWEXCEPTION(EIDMW_WRONG_ASN1_FORMAT);
		}

		//--- get token flags
		if (xLev2Item.tag != ASN_BIT_STRING)
			throw CMWEXCEPTION(EIDMW_WRONG_ASN1_FORMAT);
		tInfo.ulFlags = p15_bitstring2ul((unsigned char*)xLev2Item.p_data, xLev2Item.l_data);

		//default values
		tInfo.iAppVersion = 0;

		tInfo.iSecEnv = 0;
		tInfo.piRecordInfo[0] = 0;

		while (xLev1Item.l_data > 0)
		{
			//get next item
			if ((xLev1Item.l_data < 2) || (asn1_next_item(&xLev1Item, &xLev2Item) != 0))
				throw CMWEXCEPTION(EIDMW_WRONG_ASN1_FORMAT);

			switch (xLev2Item.tag)
			{
			case ASN_CONSTRUCTED_CONTEXT_N(1):    //Record Info
			{
				ASN1_ITEM           xLev3Item;      //
				ASN1_ITEM           xLev4Item;      //

				//get sequence
				if ((xLev2Item.l_data < 2) || (asn1_next_item(&xLev2Item, &xLev3Item) != 0) || (xLev3Item.tag != ASN_SEQUENCE))
					throw CMWEXCEPTION(EIDMW_WRONG_ASN1_FORMAT);

				for (int iIndex = 0; iIndex < 7; ++iIndex)       //clear destination
					tInfo.piRecordInfo[iIndex] = 0;
				// loop to read the integers
				while (xLev3Item.l_data > 0)
				{
					if ((xLev3Item.l_data < 2) || (asn1_next_item(&xLev3Item, &xLev4Item) != 0))
						throw CMWEXCEPTION(EIDMW_WRONG_ASN1_FORMAT);
					int iIndex = xLev4Item.tag >> 3;
					int iMode = xLev4Item.tag & (ASN_CLASS | ASN_TYPE);
					if ((iMode == ASN_CONTEXT) && (iIndex < 7) && (iIndex >= 0))
					{
						tInfo.piRecordInfo[iIndex] = bin2int(xLev4Item.p_data, xLev4Item.l_data);
					}
				}
			}
			break;

			//case ASN_CONSTRUCTED_CONTEXT_N(2):    //Supported Algorithm Info
			case ASN_PRIMITIVE_CONTEXT_N(3):    //Isser ID
			{
				std::string csIssuer((char *)xLev2Item.p_data, 0, xLev2Item.l_data);
				tInfo.csIssuer = csIssuer;
			}
			break;

			case ASN_PRIMITIVE_CONTEXT_N(4):    //Holder ID
			{
				std::string csHolder((char *)xLev2Item.p_data, 0, xLev2Item.l_data);
				tInfo.csHolder = csHolder;
			}
			break;

			case ASN_PRIMITIVE_CONTEXT_N(5):    //Last Update
			{
				std::string csLastUpdate((char *)xLev2Item.p_data, 0, xLev2Item.l_data);
				tInfo.csLastUpdate = csLastUpdate;
			}
			break;

			case ASN_PRIMITIVE_CONTEXT_N(30):    //Application version
				tInfo.iAppVersion = bin2int(xLev2Item.p_data, xLev2Item.l_data);
				break;

				//case ASN_SEQUENCE:          //security Environment Information
				//case ASN_PRINTABLE_STRING:  //preferred Languague
			default:
				break;
			}

			//point to next item
			if (xLev2Item.l_raw > xLev1Item.l_data)  xLev2Item.l_raw = xLev1Item.l_data; //make sure not to pass the end
			xLev1Item.p_data += xLev2Item.l_raw;      //next item
			xLev1Item.l_data -= xLev2Item.l_raw;
		}

		return tInfo;
	}
	*/

/*

	tOdfInfo PKCS15Parser::ParseOdf(const CByteArray & contents) {
#ifdef VERBOSE
		std::cerr << "\n----------- PKCS15Parser::ParseOdf ------------ " << std::endl;
#endif
		tOdfInfo result;
		ASN1_ITEM           xLev0Item;  //
		ASN1_ITEM           xLev1Item;  //
		ASN1_ITEM           xLev2Item;  //

		int                 iItemTag;

		xLev0Item.l_data = contents.Size();
		xLev0Item.p_data = (unsigned char*)contents.GetBytes();

		while (xLev0Item.l_data > 0)
		{
			//check if first level = APLI.x
			if ((xLev0Item.l_data < 2) || (asn1_next_item(&xLev0Item, &xLev1Item) != 0) || ((xLev1Item.tag & ASN_CLASS) != ASN_CONTEXT))
				throw CMWEXCEPTION(EIDMW_WRONG_ASN1_FORMAT);
			iItemTag = xLev1Item.tag;

			//get second level
			if ((xLev1Item.l_data < 2) || (asn1_next_item(&xLev1Item, &xLev2Item) != 0))
				throw CMWEXCEPTION(EIDMW_WRONG_ASN1_FORMAT);

			if (xLev2Item.tag == ASN_SEQUENCE)
			{
				std::string csPath = ParsePath(&xLev2Item);
				switch (iItemTag)
				{
				case ASN_CONSTRUCTED_CONTEXT_N(0):
					result.csPrkdfPath = csPath;
					break;
				case ASN_CONSTRUCTED_CONTEXT_N(1):
//					result.csPukdfPath = csPath;
					break;
				case ASN_CONSTRUCTED_CONTEXT_N(2):
//					result.csTrustedPukdfPath = csPath;
					break;
				case ASN_CONSTRUCTED_CONTEXT_N(3):
//					result.csSecretPukdfPath = csPath;
					break;
				case ASN_CONSTRUCTED_CONTEXT_N(4):
					result.csCdfPath = csPath;
					break;
				case ASN_CONSTRUCTED_CONTEXT_N(5):
					result.csTrustedCdfPath = csPath;
					break;
				case ASN_CONSTRUCTED_CONTEXT_N(6):
					result.csUsefulCdfPath = csPath;
					break;
				case ASN_CONSTRUCTED_CONTEXT_N(7):
					result.csDodfPath = csPath;
					break;
				case ASN_CONSTRUCTED_CONTEXT_N(8):
					result.csAodfPath = csPath;
					break;
				}
			}
		}

		return result;
	}
	*/

	//----------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------


	std::vector <tCert> PKCS15Parser::ParseCdf(const CByteArray & contents) {
		std::vector<tCert> oResult;
		ASN1_ITEM           xLev0Item;  // 
		ASN1_ITEM           xLev1Item;  // 
		ASN1_ITEM           xLev2Item;  // 
		ASN1_ITEM           xLev3Item;  // 
		ASN1_ITEM           xLev4Item;  // 
		tCert               cert;

		oResult.clear();

#ifdef VERBOSE
		std::cerr << "\n----------- PKCS15Parser::ParseCdf ------------ " << std::endl;
#endif
		//---  X509 Certificate

		xLev0Item.p_data = (unsigned char*)contents.GetBytes();
		xLev0Item.l_data = contents.Size();

		// loop over the possible certificate
		while (xLev0Item.l_data > 0)
		{
			if ((xLev0Item.l_data < 2) || (asn1_next_item(&xLev0Item, &xLev1Item) != 0))
				throw CMWEXCEPTION(EIDMW_WRONG_ASN1_FORMAT);

			if (xLev1Item.tag == ASN_SEQUENCE)
			{
				cert.bValid = true;
				//----  Common Object Attributes -----
				tCommonObjAttr coAttr = ParseCommonObjectAttributes(&xLev1Item);
				FillCOACert(&cert, &coAttr);


				//----  Common Certificate Attributes ----
				if ((xLev1Item.l_data < 2) || (asn1_next_item(&xLev1Item, &xLev2Item) != 0) || (xLev2Item.tag != ASN_SEQUENCE))
					throw CMWEXCEPTION(EIDMW_WRONG_ASN1_FORMAT);

				if ((xLev2Item.l_data < 2) || (asn1_next_item(&xLev2Item, &xLev3Item) != 0) || (xLev3Item.tag != ASN_OCTET_STRING))
					throw CMWEXCEPTION(EIDMW_WRONG_ASN1_FORMAT);
				cert.ulID = bin2ulong(xLev3Item.p_data, xLev3Item.l_data);


				// look for the optional objects, Authority (boolean default false)
				cert.bAuthority = false;
				cert.bImplicitTrust = false;

				while (xLev2Item.l_data > 0)
				{
					if ((xLev2Item.l_data < 2) || (asn1_next_item(&xLev2Item, &xLev3Item) != 0))
						throw CMWEXCEPTION(EIDMW_WRONG_ASN1_FORMAT);
					if (xLev3Item.tag == ASN_BOOLEAN)
						cert.bAuthority = bin2int(xLev3Item.p_data, xLev3Item.l_data) ? true : false;

					else if (xLev3Item.tag == ASN_CONSTRUCTED_CONTEXT_N(3))  // implicit trust (default false)
					{
						if ((xLev3Item.l_data < 2) || (asn1_next_item(&xLev3Item, &xLev4Item) != 0))
							throw CMWEXCEPTION(EIDMW_WRONG_ASN1_FORMAT);
						cert.bImplicitTrust = bin2int(xLev4Item.p_data, xLev4Item.l_data) ? true : false;
					}
					//else if(xLev3Item.tagClass == ASN_CONSTRUCTED_CONTEXT_N(0)  // OOBCertHash
					//else if(xLev3Item.tagClass == ASN_CONSTRUCTED_CONTEXT_N(1)  // trusted usage
					//else if(xLev3Item.tagClass == ASN_CONSTRUCTED_CONTEXT_N(2)  // sequence of credential identif.
				}

				while (xLev1Item.l_data > 0)
				{
					if ((xLev1Item.l_data < 2) || (asn1_next_item(&xLev1Item, &xLev2Item) != 0))
						throw CMWEXCEPTION(EIDMW_WRONG_ASN1_FORMAT);

					if (xLev2Item.tag == ASN_CONSTRUCTED_CONTEXT_N(1))
						//---- [1]X509 CertificateAttributes ----
					{
						// -------- type attributes: path
						if ((xLev2Item.l_data < 2) || (asn1_next_item(&xLev2Item, &xLev3Item) != 0))
							throw CMWEXCEPTION(EIDMW_WRONG_ASN1_FORMAT);
						cert.csPath = ParsePath2(&xLev3Item);
					}
				}
				/* else if(xLev1Item.tag == ASN_CONSTRUCTED_CONTEXT_N(1)){
				  // tag 0:  X509 Attribute Certificate
				  // tag 1:  SPKI Certificate
				  // tag 2:  PGP Certificate
				  // tag 3:  WTLS Certificate
				  // tag 4:  X9-68 Certificate
				  // tag 5:  CV Certificate
				*/
			}
			if (xLev1Item.tag != 0)	//0 means a removed certificate
			{
				oResult.push_back(cert);
			}
		}

		return oResult;
	}


	}
