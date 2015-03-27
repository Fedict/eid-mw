/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2014 FedICT.
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
#else
#include <unix.h>
#endif
#include <pkcs11.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define ADD_LABEL(l, is_string) if(!strcmp(label, l)) return is_string

CK_BBOOL is_string(const char* label) {
	ADD_LABEL("CARD_DATA", CK_FALSE);
	ADD_LABEL("ATR", CK_FALSE);
	ADD_LABEL("carddata_serialnumber", CK_FALSE);
	ADD_LABEL("carddata_comp_code", CK_FALSE);
	ADD_LABEL("carddata_os_number", CK_FALSE);
	ADD_LABEL("carddata_os_version", CK_FALSE);
	ADD_LABEL("carddata_glob_os_version", CK_FALSE);
	ADD_LABEL("carddata_soft_mask_number", CK_FALSE);
	ADD_LABEL("carddata_soft_mask_version", CK_FALSE);
	ADD_LABEL("carddata_appl_version", CK_FALSE);
	ADD_LABEL("carddata_appl_int_version", CK_FALSE);
	ADD_LABEL("carddata_pkcs15_version", CK_FALSE);
	ADD_LABEL("carddata_appl_lifecycle", CK_FALSE);
	ADD_LABEL("carddata_pkcs1_support", CK_FALSE);
	ADD_LABEL("carddata_key_exchange_version", CK_FALSE);
	ADD_LABEL("DATA_FILE", CK_FALSE);
	ADD_LABEL("card_number", CK_TRUE);
	ADD_LABEL("chip_number", CK_FALSE);
	ADD_LABEL("validity_begin_date", CK_TRUE);
	ADD_LABEL("validity_end_date", CK_TRUE);
	ADD_LABEL("issuing_municipality", CK_TRUE);
	ADD_LABEL("national_number", CK_TRUE);
	ADD_LABEL("surname", CK_TRUE);
	ADD_LABEL("firstnames", CK_TRUE);
	ADD_LABEL("first_letter_of_third_given_name", CK_TRUE);
	ADD_LABEL("nationality", CK_TRUE);
	ADD_LABEL("location_of_birth", CK_TRUE);
	ADD_LABEL("date_of_birth", CK_TRUE);
	ADD_LABEL("gender", CK_TRUE);
	ADD_LABEL("nobility", CK_TRUE);
	ADD_LABEL("document_type", CK_FALSE);
	ADD_LABEL("special_status", CK_TRUE);
	ADD_LABEL("photo_hash", CK_FALSE);
	ADD_LABEL("duplicata", CK_FALSE);
	ADD_LABEL("special_organization", CK_FALSE);
	ADD_LABEL("member_of_family", CK_FALSE);
	ADD_LABEL("date_and_country_of_protection", CK_TRUE);
	ADD_LABEL("ADDRESS_FILE", CK_FALSE);
	ADD_LABEL("address_street_and_number", CK_TRUE);
	ADD_LABEL("address_zip", CK_TRUE);
	ADD_LABEL("address_municipality", CK_TRUE);
	ADD_LABEL("PHOTO_FILE", CK_FALSE);
	ADD_LABEL("CERT_RN_FILE", CK_FALSE);
	ADD_LABEL("SIGN_DATA_FILE", CK_FALSE);
	ADD_LABEL("SIGN_ADDRESS_FILE", CK_FALSE);
	ADD_LABEL("Authentication", CK_FALSE);
	ADD_LABEL("Signature", CK_FALSE);
	ADD_LABEL("Root", CK_FALSE);
	ADD_LABEL("CA", CK_FALSE);

	printf("E: unknown label %s found\n", label);
	exit(EXIT_FAILURE);
}
