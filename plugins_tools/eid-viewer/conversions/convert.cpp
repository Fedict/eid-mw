#include <conversions.h>
#include "convertor.h"
#include "convworker.h"
#include "cache.h"
#include "backend.h"
#include <eid-util/utftranslate.h>

#include <cstring>
#include <map>

/* Return the display-converted string for the card representation
 * pointed to by the "normal" argument */
EID_CHAR *converted_string(const EID_CHAR * label, const EID_CHAR * normal)
{
	Convertor conv;

	return conv.convert(label, normal);
}

/* Return the XML-converted string for the card representation pointed
 * to by the "normal" argument */
EID_CHAR *convert_to_xml(const EID_CHAR * label, const EID_CHAR * normal)
{
	Convertor conv;

	return conv.convert_to_xml(label, normal);
}

/* Return the card representation of the XML-converted string pointed to
 * by the "value" argument */
void *convert_from_xml(const EID_CHAR * name, const EID_CHAR * value,
		       int *len_return)
{
	Convertor conv;

	return conv.convert_from_xml(name, value, len_return);
}

/* Returns 1 if we have a conversion method for this label */
int can_convert(const EID_CHAR * label)
{
	Convertor conv;

	return conv.can_convert(label);
}

/* sets the graphical version number of this card */
void convert_set_graphvers(enum eid_vwr_graphvers gv)
{
	ConversionWorker::set_graphvers(gv);
}

/* Set the display language for conversions */
void eid_vwr_convert_set_lang(enum eid_vwr_langs which)
{
	if (ConversionWorker::get_lang() != which)
	{
		void *iterator;
		const EID_CHAR *label;

		ConversionWorker::set_lang(which);
		for (iterator = cache_label_iterator(), label =
		     cache_next_label(iterator); label != NULL;
		     label = cache_next_label(iterator))
		{
			if (can_convert(label))
			{
				const eid_vwr_cache_item *item =
					cache_get_data(label);
				EID_CHAR *str =
					converted_string(label,
							 (const EID_CHAR *)
							 item->data);
				be_newstringdata(label, str);
				free(str);
			}
		}
		cache_label_iterator_free(iterator);
	}
}

/* Get the current display language for conversions */
enum eid_vwr_langs eid_vwr_convert_get_lang()
{
	return ConversionWorker::get_lang();
}
