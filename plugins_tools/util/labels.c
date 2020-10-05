/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2017 FedICT.
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
#pragma pack(push, cryptoki, 1)
#include "pkcs11.h"
#pragma pack(pop, cryptoki)
#else
#include "unix.h"
#include "pkcs11.h"
#endif
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <eid-util/utftranslate.h>
#include <eid-util/labels.h>

static struct {
	EID_CHAR * label;
	CK_BBOOL is_string;
	CK_BBOOL on_eid;
	CK_BBOOL on_foreigner;
	EID_CHAR * min_version;
} labels[] = {
	{ TEXT("CARD_DATA"),			CK_FALSE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("ATR"),				CK_FALSE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("carddata_serialnumber"),	CK_FALSE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("carddata_comp_code"),		CK_FALSE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("carddata_os_number"),		CK_FALSE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("carddata_os_version"),		CK_FALSE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("carddata_glob_os_version"),	CK_FALSE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("carddata_soft_mask_number"),	CK_FALSE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("carddata_soft_mask_version"),	CK_FALSE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("carddata_appl_version"),	CK_FALSE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("carddata_appl_int_version"),	CK_FALSE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("carddata_pkcs15_version"),	CK_FALSE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("carddata_appl_lifecycle"),	CK_FALSE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("carddata_pkcs1_support"),	CK_FALSE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("carddata_key_exchange_version"),CK_FALSE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("carddata_pin_counter"),         CK_FALSE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("DATA_FILE"),			CK_FALSE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("card_number"),			CK_TRUE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("chip_number"),			CK_FALSE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("validity_begin_date"),		CK_TRUE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("validity_end_date"),		CK_TRUE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("issuing_municipality"),		CK_TRUE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("national_number"),		CK_TRUE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("surname"),			CK_TRUE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("firstnames"),			CK_TRUE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("first_letter_of_third_given_name"), CK_TRUE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("nationality"),			CK_TRUE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("location_of_birth"),		CK_TRUE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("date_of_birth"),		CK_TRUE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("gender"),			CK_TRUE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("nobility"),			CK_TRUE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("document_type"),		CK_TRUE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("special_status"),		CK_TRUE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("photo_hash"),			CK_FALSE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("duplicata"),			CK_TRUE, CK_FALSE, CK_TRUE, NULL },//there is no converter for this field, and according to card spec, there are ascii values in this field
	{ TEXT("special_organization"),		CK_FALSE, CK_FALSE, CK_TRUE, NULL },
	{ TEXT("member_of_family"),		CK_FALSE, CK_FALSE, CK_TRUE, TEXT("4.1") },
	{ TEXT("date_and_country_of_protection"), CK_TRUE, CK_FALSE, CK_TRUE, TEXT("4.1") },
	{ TEXT("work_permit_mention"),		CK_TRUE, CK_FALSE, CK_TRUE, TEXT("4.1") },
	{ TEXT("employer_vat_1"),		CK_TRUE, CK_FALSE, CK_TRUE, TEXT("4.1") },
	{ TEXT("employer_vat_2"),		CK_TRUE, CK_FALSE, CK_TRUE, TEXT("4.1") },
	{ TEXT("regional_file_number"),		CK_TRUE, CK_FALSE, CK_TRUE, TEXT("4.1") },
	{ TEXT("ADDRESS_FILE"),			CK_FALSE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("address_street_and_number"),	CK_TRUE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("address_zip"),			CK_TRUE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("address_municipality"),		CK_TRUE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("PHOTO_FILE"),			CK_FALSE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("CERT_RN_FILE"),			CK_FALSE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("SIGN_DATA_FILE"),		CK_FALSE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("SIGN_ADDRESS_FILE"),		CK_FALSE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("Authentication"),		CK_FALSE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("Signature"),			CK_FALSE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("Root"),				CK_FALSE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("CA"),				CK_FALSE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("xml_file_version"),		CK_TRUE, CK_FALSE, CK_FALSE, NULL }, // virtual label only used when reading an XML file
	{ TEXT("xml"),				CK_TRUE, CK_FALSE, CK_FALSE, NULL }, // virtual label only used when reading an XML file
	{ TEXT("BASIC_KEY_FILE"),		CK_FALSE, CK_TRUE, CK_TRUE, NULL },
	{ TEXT("basic_key_hash"),		CK_FALSE, CK_TRUE, CK_TRUE, NULL },
	{ NULL,					0, 0, 0, NULL },
};

static struct labelnames *foreignerlabels = NULL;

#ifndef WIN32
#define EID_FPRINTF fprintf
#else
#define EID_FPRINTF fwprintf
#endif

#define fun(TYPE, DEFAULT, NAME) TYPE NAME(const EID_CHAR * const label) {\
	int i;\
	for(i=0; labels[i].label != NULL; i++) {\
		if(!EID_STRCMP(labels[i].label, label)) {\
			return labels[i].NAME;\
		}\
	}\
	EID_FPRINTF(stderr, TEXT("E: unknown label: %s"), label);\
	return DEFAULT; \
}

fun(CK_BBOOL, CK_FALSE, on_foreigner)
fun(CK_BBOOL, CK_FALSE, on_eid)
fun(CK_BBOOL, CK_FALSE, is_string)
fun(const EID_CHAR *, NULL, min_version)

struct labelnames* get_foreigner_labels() {
	if(foreignerlabels == NULL) {
		int i;
		foreignerlabels = calloc(sizeof(struct labelnames), 1);
		for(i=0; labels[i].label != NULL; i++) {
			if(!labels[i].on_eid && labels[i].on_foreigner) {
				foreignerlabels->label = realloc(foreignerlabels->label, sizeof(EID_CHAR*) * ++(foreignerlabels->len));
				foreignerlabels->label[foreignerlabels->len - 1] = labels[i].label;
			}
		}
	}

	return foreignerlabels;
}
