/*****************************************************************************
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
 *****************************************************************************/
//basic certificate decoder
#include <stdio.h>

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "asn1.h"
#include "cert.h"


int cert_get_info(const unsigned char *pcert, unsigned int lcert, T_CERT_INFO *info)
{
	int ret = 0;
	ASN1_ITEM item;

	//should be done already
	memset(info, 0, sizeof(T_CERT_INFO));

	//check size of cert
	ret = asn1_get_item(pcert, lcert, "\1", &item, 1);
	if (ret)
		return(ret);
	if (item.l_raw <= lcert)
		lcert = info->lcert = item.l_raw;
	else
		return (E_X509_INCOMPLETE);

	ret = asn1_get_item(pcert, lcert, X509_SUBJECT, &item, 1);
	if (ret)
		return(ret);

	info->subject = (char*) malloc(item.l_raw);
	if (info->subject == NULL)
		return(E_X509_ALLOC);
	memcpy(info->subject, item.p_raw, item.l_raw);
	info->l_subject = item.l_raw;

	ret = asn1_get_item(pcert, lcert, X509_ISSUER, &item, 1);
	if (ret)
		return(ret);

	info->issuer = (char*) malloc(item.l_raw);
	if (info->issuer == NULL)
		return(E_X509_ALLOC);
	memcpy(info->issuer, item.p_raw, item.l_raw);
	info->l_issuer = item.l_raw;

	ret = asn1_get_item(pcert, lcert, X509_SERIAL, &item, 1);
	if (ret)
		return(ret);
	else if (item.tag != ASN_INTEGER)
		return(E_X509_DECODE);

	info->serial = (char*) malloc (item.l_raw);
	if (info->serial == NULL)
		return(E_X509_ALLOC);
	memcpy(info->serial, item.p_raw, item.l_raw);
	info->l_serial = item.l_raw;

	ret = asn1_get_item(pcert, lcert, X509_VALID_FROM, &item, 1);
	if (ret)
		return(ret);

	info->validfrom = (char*) malloc(item.l_data);
	if (info->validfrom == NULL)
		return(E_X509_ALLOC);
	memcpy(info->validfrom, item.p_data, item.l_data);
	info->l_validfrom = item.l_data;

	ret = asn1_get_item(pcert, lcert, X509_VALID_UNTIL, &item, 1);
	if (ret)
		return(ret);

	info->validto = (char*) malloc(item.l_data);
	if (info->validto == NULL)
		return(E_X509_ALLOC);
	memcpy(info->validto, item.p_data, item.l_data);
	info->l_validto = item.l_data;

	/* check for key type */
	ret = asn1_get_item(pcert, lcert, X509_KEYTYPE, &item, 1);
	if (ret)
		return(ret);
	else if (item.tag != ASN_OID)
		return (E_X509_DECODE);
	else {
		if (item.l_data == sizeof(OID_RSA_ENCRYPTION)-1 && memcmp(item.p_data, OID_RSA_ENCRYPTION, item.l_data) == 0) {
			/* RSA key; extract modulus and exponent */
			ret = asn1_get_item(pcert, lcert, X509_RSA_MOD, &item, 1);
			if (ret)
				return(ret);
			if (*(item.p_data) == 0)
			{
				/* first byte could be zero in ASN_INTEGER, skip this */
				item.p_data++;
				item.l_data--;
			}

			info->mod = (char*) malloc(item.l_data);
			if (info->mod == NULL)
				return(E_X509_ALLOC);

			memcpy(info->mod, item.p_data, item.l_data);
			info->l_mod = item.l_data;

			ret = asn1_get_item(pcert, lcert, X509_RSA_EXP, &item, 1);
			if (ret)
				return(ret);
			if (*(item.p_data) == 0)
			{
				/* first byte could be zero in ASN_INTEGER, skip this */
				item.p_data++;
				item.l_data--;
			}
			info->exp = (char*) malloc(item.l_data);
			if (info->exp == NULL)
				return(E_X509_ALLOC);
			memcpy(info->exp, item.p_data, item.l_data);
			info->l_exp = item.l_data;

			/* PKINFO */
			ret = asn1_get_item(pcert, lcert, X509_PKINFO, &item, 1);
			if (ret)
				return(ret);
			info->pkinfo = malloc(item.l_raw);
			if (info->pkinfo == NULL)
				return(E_X509_ALLOC);
			memcpy(info->pkinfo, item.p_raw, item.l_raw);
			info->l_pkinfo = item.l_raw;

		}
		else if (item.l_data == sizeof(OID_EC_PUBLIC_KEY) - 1 && memcmp(item.p_data, OID_EC_PUBLIC_KEY, item.l_data) == 0){
			ret = asn1_get_item(pcert, lcert, X509_EC_CURVE, &item, 1);
			if (ret)
				return(ret);

			info->curve = malloc(item.l_raw);
			if (info->curve == NULL)
				return(E_X509_ALLOC);
			memcpy(info->curve, item.p_raw, item.l_raw);
			info->l_curve = item.l_raw;

			/* EC public key; */
			ret = asn1_get_item(pcert, lcert, X509_PKINFO, &item, 0);
			if (ret)
				return(ret);
			info->pkinfo = malloc(item.l_data);
			if (info->pkinfo == NULL)
				return(E_X509_ALLOC);
			memcpy(info->pkinfo, item.p_data, item.l_data);
			info->l_pkinfo = item.l_data;
		}
		else {
			return (E_X509_UNKNOWN_KEYTYPE);
		}		
	}

	return (0);
}

void free_info_member(char* pcInfo_member)
{
	if (pcInfo_member != NULL)
	{
		free(pcInfo_member);
		pcInfo_member = NULL;
	}
}

void cert_free_info(T_CERT_INFO *info)
{
	if(info != NULL)
	{
		free_info_member(info->issuer);
		free_info_member(info->mod);
		free_info_member(info->pkinfo);
		free_info_member(info->serial);
		free_info_member(info->validfrom);
		free_info_member(info->validto);
		free_info_member(info->curve);
	}
}


//The card public key is stored on applet 1.8 cards in the following format:
/* Offset       ENCODING                                            ASN.1 Syntax
 *
 *  00          30 76                                               -- SEQUENCE LENGTH
	02                  30 10                                       -- SEQUENCE LENGTH
																	Label
	04                          06 07                               -- OBJECT_ID LENGTH
								2A 86 48 CE 3D 02 01                EcPublicKey (1 2 840 10045 2 1)

	0D                          06 05                               -- OBJECT_ID LENGTH
	0F                          2B 81 04 00 22                      Secp384r1 (1 3 132 0 34)

	14                  03 62                                       -- BIT_STRING (98 bytes) LENGTH
	16                          00                                  -- no bits unused in the final byte
	17                          04                                  compression byte
	18                          {48 bytes}                          -- X coordinate
	48                          {48 bytes}                          -- Y coordinate
 * */


int key_get_info(const unsigned char *pkey, unsigned int lkey, T_KEY_INFO * info)
{
	int ret = 0;
	ASN1_ITEM item;

	//should be done already
	memset(info, 0, sizeof(T_KEY_INFO));

	//verify size of public key file
	ret = asn1_get_item(pkey, lkey, "\1", &item, 1);
	if (ret)
		return(ret);
	if (item.l_raw <= lkey)
		lkey = info->lkey = item.l_raw;
	else
		return (E_X509_INCOMPLETE);

	// check for key type
	ret = asn1_get_item(pkey, lkey, "\1\1\1", &item, 1);


	//verify if the key is an EcPublicKey
	if (item.l_data == sizeof(OID_EC_PUBLIC_KEY) - 1 && memcmp(item.p_data, OID_EC_PUBLIC_KEY, item.l_data) == 0) {
		//get the curve
		ret = asn1_get_item(pkey, lkey, "\1\1\2", &item, 1);
		if (ret)
			return(ret);

		info->curve = malloc(item.l_raw);
		if (info->curve == NULL)
			return(E_X509_ALLOC);
		memcpy(info->curve, item.p_raw, item.l_raw);
		info->l_curve = item.l_raw;

		/* EC public key; */
		ret = asn1_get_item(pkey, lkey, "\1\2", &item, 0);
		if (ret)
			return(ret);
		info->pkinfo = malloc(item.l_data);
		if (info->pkinfo == NULL)
			return(E_X509_ALLOC);
		memcpy(info->pkinfo, item.p_data, item.l_data);
		info->l_pkinfo = item.l_data;
	}
	else
	{
		//we currently only support EC card keys
		return(E_X509_UNKNOWN_KEYTYPE);
	}

	return (0);
}
void key_free_info(T_KEY_INFO * info)
{
	if (info != NULL)
	{
		free_info_member(info->pkinfo);
		free_info_member(info->curve);
	}
}
