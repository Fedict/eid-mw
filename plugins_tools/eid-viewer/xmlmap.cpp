#include "xmlmap.h"
#include <map>
#include <string>
#include "cppeidstring.h"

static std::map < EID_STRING, struct element_desc *>xml_to_elem;
static std::map < EID_STRING, struct attribute_desc *>xml_to_att;
static bool inited = false;

/* This file describes our XML format. It is used by xml.c to map an element
 * label to an element struct. We wrote it in C++ so we can use the STL
 * containers, but other than that C++ isn't really needed... */

/* Attributes of the <identity> element */
struct attribute_desc identity_attributes[] = {
	{TEXT("nationalnumber"), TEXT("national_number"), 1},
	{TEXT("dateofbirth"), TEXT("date_of_birth"), 1},
	{TEXT("gender"), TEXT("gender"), 1},
	{TEXT("noblecondition"), TEXT("nobility"), 0},
	{TEXT("specialstatus"), TEXT("special_status"), 0},
	{TEXT("duplicate"), TEXT("duplicata"), 0},
	{TEXT("memberoffamily"), TEXT("member_of_family"), 0},
	{NULL, NULL, 0},
};

/* Child elements of the <identity> element */
struct element_desc identity_elements[] = {
	{TEXT("name"), TEXT("surname"), 1, 0, NULL, NULL},
	{TEXT("firstname"), TEXT("firstnames"), 1, 0, NULL, NULL},
	{TEXT("middlenames"), TEXT("first_letter_of_third_given_name"), 0, 0, NULL, NULL},
	{TEXT("nationality"), TEXT("nationality"), 1, 0, NULL, NULL},
	{TEXT("placeofbirth"), TEXT("location_of_birth"), 1, 0, NULL, NULL},
	{TEXT("photo"), TEXT("PHOTO_FILE"), 1, 1, NULL, NULL},
	{NULL, NULL, 0, 0, NULL, NULL},
};

/* Attributes of the <workpermit> element */
struct attribute_desc workpermit_attributes[] = {
	{TEXT("mention"), TEXT("work_permit_mention"), 1},
	{TEXT("vat1"), TEXT("employer_vat_1"), 0},
	{TEXT("vat2"), TEXT("employer_vat_2"), 0},
	{TEXT("regionalfile"), TEXT("regional_file_number"), 0},
	{NULL, NULL, 0},
};

/* Attributes of the <brexit> element */
struct attribute_desc brexit_attributes[] = {
	{TEXT("mention1"), TEXT("brexit_mention_1"), 1},
	{TEXT("mention2"), TEXT("brexit_mention_2"), 0},
	{NULL, NULL, 0},
};

/* Attributes of the <cardA> element */
struct attribute_desc cardA_attributes[] = {
	{TEXT("mentions1"), TEXT("cardA_mention_1"), 1},
	{TEXT("mentions2"), TEXT("cardA_mention_2"), 0},
	{NULL, NULL, 0},
};

/* Child elements of the <card> element */
static struct element_desc card_elements[] = {
	{TEXT("deliverymunicipality"), TEXT("issuing_municipality"), 1, 0, NULL, NULL},
	{TEXT("workpermit"), NULL, 0, 0, NULL, workpermit_attributes},
	{TEXT("brexit"), NULL, 0, 0, NULL, brexit_attributes},
	{TEXT("cardA"), NULL, 0, 0, NULL, cardA_attributes},
	{NULL, NULL, 0, 0, NULL, NULL},
};

/* Attributes of the <card> element */
static struct attribute_desc card_attributes[] = {
	{TEXT("documenttype"), TEXT("document_type"), 1},
	{TEXT("carddata_appl_version"), TEXT("carddata_appl_version"), 0},
	{TEXT("cardnumber"), TEXT("card_number"), 1},
	{TEXT("chipnumber"), TEXT("chip_number"), 1},
	{TEXT("validitydatebegin"), TEXT("validity_begin_date"), 1},
	{TEXT("validitydateend"), TEXT("validity_end_date"), 1},
	{NULL, NULL, 0},
};

/* Child elements of the <address> element */
static struct element_desc address_elements[] = {
	{TEXT("streetandnumber"), TEXT("address_street_and_number"), 1, 0, NULL, NULL},
	{TEXT("zip"), TEXT("address_zip"), 1, 0, NULL, NULL},
	{TEXT("municipality"), TEXT("address_municipality"), 1, 0, NULL, NULL},
	{NULL, NULL, 0, 0, NULL, NULL},
};

/* Child elements of the <certificates> element */
static struct element_desc certificate_elements[] = {
	{TEXT("root"), TEXT("Root"), 1, 1, NULL, NULL},
	{TEXT("citizenca"), TEXT("CA"), 1, 1, NULL, NULL},
	{TEXT("authentication"), TEXT("Authentication"), 0, 1, NULL, NULL},
	{TEXT("signing"), TEXT("Signature"), 0, 1, NULL, NULL},
	{TEXT("rrn"), TEXT("CERT_RN_FILE"), 1, 1, NULL, NULL},
	{NULL, NULL, 0, 0, NULL, NULL},
};

/* Child elements of the <eid> element (the toplevel element) */
static struct element_desc eid_elements[] = {
	{TEXT("identity"), NULL, 1, 0, identity_elements, identity_attributes},
	{TEXT("card"), NULL, 1, 0, card_elements, card_attributes},
	{TEXT("address"), NULL, 1, 0, address_elements, NULL},
	{TEXT("certificates"), NULL, 1, 0, certificate_elements, NULL},
	{NULL, NULL, 0, 0, NULL, NULL},
};

/* attributes of the <eid> element (the toplevel element) */
static struct attribute_desc eid_attributes[] = {
	{TEXT("version"), TEXT("xml_file_version"), 0},
	{NULL, NULL, 0},
};

/* The toplevel element, <eid> */
static struct element_desc toplevel_arr[] = {
	{TEXT("eid"), NULL, 1, 0, eid_elements, eid_attributes},
	{NULL, NULL, 0, 0, NULL, NULL},
};

struct element_desc *toplevel = toplevel_arr;

/* Initialize the std::map so we can do quick lookups later on */
static void initmap(struct element_desc *elem)
{
	while (elem->name)
	{
		xml_to_elem[elem->name] = elem;
		if (elem->child_elements != NULL)
		{
			initmap(elem->child_elements);
		}
		if (elem->attributes != NULL)
		{
			for (struct attribute_desc * att = elem->attributes;
			     att->name != NULL; att++)
			{
				xml_to_att[att->name] = att;
			}
		}
		elem++;
	}
}

/* Do a lookup of the struct element_desc* corresponding to the given XML
 * element name. Returns NULL if the element is unknown. */
struct element_desc *get_elemdesc(const EID_CHAR * xmlname)
{
	if (!inited)
	{
		initmap(toplevel);
		inited = true;
	}
	if (xml_to_elem.count(xmlname))
	{
		return xml_to_elem[xmlname];
	} else
	{
		return NULL;
	}
}

/* Do a lookup of the struct attribute_desc* corresponding to the given XML
 * element name. Returns NULL if the attribute is unknown.
 *
 * This works because we don't have two elements with attributes having
 * the same name but different semantics. If that ever changes, we may
 * need to change this function's signature. */
struct attribute_desc *get_attdesc(const EID_CHAR * xmlname)
{
	if (!inited)
	{
		initmap(toplevel);
		inited = true;
	}
	if (xml_to_att.count(xmlname))
	{
		return xml_to_att[xmlname];
	} else
	{
		return NULL;
	}
}
