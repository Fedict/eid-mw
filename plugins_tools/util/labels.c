/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2016 FedICT.
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
#include "unix.h"
#endif
#include "pkcs11.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <eid-util/utftranslate.h>

#define ADD_LABEL(l, is_string) if(!EID_STRCMP(label, l)) return is_string

CK_BBOOL is_string(const EID_CHAR* label) {
	ADD_LABEL(TEXT("CARD_DATA"), CK_FALSE);
	ADD_LABEL(TEXT("ATR"), CK_FALSE);
	ADD_LABEL(TEXT("carddata_serialnumber"), CK_FALSE);
	ADD_LABEL(TEXT("carddata_comp_code"), CK_FALSE);
	ADD_LABEL(TEXT("carddata_os_number"), CK_FALSE);
	ADD_LABEL(TEXT("carddata_os_version"), CK_FALSE);
	ADD_LABEL(TEXT("carddata_glob_os_version"), CK_FALSE);
	ADD_LABEL(TEXT("carddata_soft_mask_number"), CK_FALSE);
	ADD_LABEL(TEXT("carddata_soft_mask_version"), CK_FALSE);
	ADD_LABEL(TEXT("carddata_appl_version"), CK_FALSE);
	ADD_LABEL(TEXT("carddata_appl_int_version"), CK_FALSE);
	ADD_LABEL(TEXT("carddata_pkcs15_version"), CK_FALSE);
	ADD_LABEL(TEXT("carddata_appl_lifecycle"), CK_FALSE);
	ADD_LABEL(TEXT("carddata_pkcs1_support"), CK_FALSE);
	ADD_LABEL(TEXT("carddata_key_exchange_version"), CK_FALSE);
	ADD_LABEL(TEXT("DATA_FILE"), CK_FALSE);
	ADD_LABEL(TEXT("card_number"), CK_TRUE);
	ADD_LABEL(TEXT("chip_number"), CK_FALSE);
	ADD_LABEL(TEXT("validity_begin_date"), CK_TRUE);
	ADD_LABEL(TEXT("validity_end_date"), CK_TRUE);
	ADD_LABEL(TEXT("issuing_municipality"), CK_TRUE);
	ADD_LABEL(TEXT("national_number"), CK_TRUE);
	ADD_LABEL(TEXT("surname"), CK_TRUE);
	ADD_LABEL(TEXT("firstnames"), CK_TRUE);
	ADD_LABEL(TEXT("first_letter_of_third_given_name"), CK_TRUE);
	ADD_LABEL(TEXT("nationality"), CK_TRUE);
	ADD_LABEL(TEXT("location_of_birth"), CK_TRUE);
	ADD_LABEL(TEXT("date_of_birth"), CK_TRUE);
	ADD_LABEL(TEXT("gender"), CK_TRUE);
	ADD_LABEL(TEXT("nobility"), CK_TRUE);
	ADD_LABEL(TEXT("document_type"), CK_FALSE);
	ADD_LABEL(TEXT("special_status"), CK_TRUE);
	ADD_LABEL(TEXT("photo_hash"), CK_FALSE);
	ADD_LABEL(TEXT("duplicata"), CK_TRUE);//there is no converter for this field, and according to card spec, there are ascii values in this field
	ADD_LABEL(TEXT("special_organization"), CK_FALSE);
	ADD_LABEL(TEXT("member_of_family"), CK_FALSE);
	ADD_LABEL(TEXT("date_and_country_of_protection"), CK_TRUE);
	ADD_LABEL(TEXT("ADDRESS_FILE"), CK_FALSE);
	ADD_LABEL(TEXT("address_street_and_number"), CK_TRUE);
	ADD_LABEL(TEXT("address_zip"), CK_TRUE);
	ADD_LABEL(TEXT("address_municipality"), CK_TRUE);
	ADD_LABEL(TEXT("PHOTO_FILE"), CK_FALSE);
	ADD_LABEL(TEXT("CERT_RN_FILE"), CK_FALSE);
	ADD_LABEL(TEXT("SIGN_DATA_FILE"), CK_FALSE);
	ADD_LABEL(TEXT("SIGN_ADDRESS_FILE"), CK_FALSE);
	ADD_LABEL(TEXT("Authentication"), CK_FALSE);
	ADD_LABEL(TEXT("Signature"), CK_FALSE);
	ADD_LABEL(TEXT("Root"), CK_FALSE);
	ADD_LABEL(TEXT("CA"), CK_FALSE);

#ifndef WIN32
	fprintf(stderr, "E: unkown label: %s", label);
#else
	fwprintf(stderr, TEXT("E: unkown label: %s"), label);
#endif
	exit(EXIT_FAILURE);
}
