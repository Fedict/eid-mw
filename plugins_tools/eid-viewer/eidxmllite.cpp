//#include <ole2.h>
#include <xmllite.h>
#include <stdio.h>
#include <shlwapi.h>
#include "xml.h"
#include "xmlmap.h"
#include "backend.h"
#include "p11.h"
#include "cache.h"
#include "conversions.h"
extern "C"
{
#include "state.h"
#include <b64/base64dec.h>
#include <b64/base64enc.h>
}
const UINT MAX_ELEMENT_DEPTH = 8;

#define SAFE_RELEASE(I)         if (I){ I->Release();  I = NULL; }
#define SAFE_FREE(val)			if (val != NULL){ free(val); val = NULL;}
#define FAILED_OUT(retVal)		if ((retVal) != 0){goto out;}

#define check_xml(call) if((rc = call) < 0) { \
	be_log(EID_VWR_LOG_DETAIL, TEXT("Error while dealing with file (calling '%s'): %d"), TEXT(#call), rc); \
	goto out; \
}


static int any_attributes_present(struct attribute_desc *attribute) 
{
	while (attribute != NULL && attribute->name) 
	{
		if (cache_have_label(attribute->label)) 
		{
			return 1;
		}
		attribute++;
	}
	return 0;
}

static int any_elements_present(struct element_desc *element) 
{
	while (element != NULL && element->name) 
	{
		if (element->label == NULL) 
		{
			if (any_attributes_present(element->attributes) || any_elements_present(element->child_elements))
			{
				return 1;
			}
		}
		else 
		{
			if (cache_have_label(element->label))
			{
				return 1;
			}
		}
		element++;
	}
	return 0;
}

/* Write attributes to the description in *attribute */
static int write_attributes(IXmlWriter * pWriter, struct attribute_desc *attribute)
{
	HRESULT retVal = 0;
	EID_CHAR *val = NULL;

	while (attribute->name)
	{
		int have_cache = cache_have_label(attribute->label);

		if (attribute->reqd && !have_cache)
		{
			be_log(EID_VWR_LOG_ERROR, L"Could not generate XML version: no data found for required label %s", attribute->label);
			return -1;
		}
		if (have_cache)
		{
			val = cache_get_xmlform(attribute->label);
			if (EID_STRLEN(val) || attribute->reqd)
			{
				pWriter->WriteAttributeString(NULL, attribute->name, NULL, val);
			}
			free(val);
			val = NULL;
		}
		attribute++;
	}

	if (val != NULL)
	{
		free(val);
	}
	return retVal;
}

/* Write elements to the description in *element */
static int write_elements(IXmlWriter * pWriter, struct element_desc *element)
{
	HRESULT retVal;
	EID_CHAR *val = NULL;

	while (element->name)
	{
		if (element->label == NULL)
		{
			//assert(element->child_elements != NULL);
			//check_xml(xmlTextWriterStartElement(writer, BAD_CAST element->name));
			if (element->reqd == 0)
			{
				//element is not requiered, so might not be needed to write it
				//do some tests here to determine if it has any attributes or sub elements present
				//if none present, skip this element
				if ( (any_attributes_present(element->attributes) || any_elements_present(element->child_elements)) == 0)
				{
					element++;
					continue;
				}
			}

			//if the element is requiered, write it down
			pWriter->WriteStartElement(NULL, element->name, NULL);
			if (element->attributes != NULL)
			{
				retVal = write_attributes(pWriter, element->attributes);
			}
			if ((element->child_elements != NULL))
			{
				FAILED_OUT(retVal = write_elements(pWriter, element->child_elements));
			}

			//check_xml(xmlTextWriterEndElement(writer));
			pWriter->WriteEndElement();

		} else
		{
			int have_cache = cache_have_label(element->label);

			//assert(element->child_elements == NULL);

			if (element->reqd && !have_cache)
			{
				be_log(EID_VWR_LOG_ERROR, TEXT("Could not generate XML version: no data found for required label %s"), element->label);
				return -1;
			}
			if (have_cache)
			{
				if (!element->is_b64)
				{
					pWriter->WriteElementString(NULL, element->name, NULL, cache_get_xmlform(element->label));
					//check_xml(xmlTextWriterWriteElement(writer, BAD_CAST element->name, BAD_CAST cache_get_xmlform(element->label)));
				} else
				{
					const struct eid_vwr_cache_item *item = cache_get_data(element->label);
					base64_encodestate state_in;
					base64_init_encodestate(&state_in);
					char *plaintext_out = (char *) malloc(((item->len) * 134) / 100 + 3);	//size increases with 1/3, and 2 padding bytes '=' are possible, plus a terminating zero
					EID_CHAR *eIDChar_out;
					unsigned long ullength = item->len;
					int plaintextLength = 0;

					if (plaintext_out != NULL)
					{
						plaintextLength = base64_encode_block((const char *) (item->data), item->len, plaintext_out, &state_in);
						//add padding if needed and terminating zero 
						base64_encode_blockend(plaintext_out + plaintextLength, &state_in);
						eIDChar_out = UTF8TOEID(plaintext_out, &ullength);
						if (eIDChar_out != NULL)
						{
							pWriter->WriteElementString(NULL,element->name, NULL, eIDChar_out);
							free(eIDChar_out);
						}
						free(plaintext_out);
					} else
					{
						be_log(EID_VWR_LOG_ERROR, TEXT("Could not allocate memory to store %s"), element->label);
					}
					//check_xml(xmlTextWriterStartElement(writer, BAD_CAST element->name));
					//check_xml(xmlTextWriterWriteBase64(writer, item->data, 0, item->len));
					//check_xml(xmlTextWriterEndElement(writer));
				}
				free(val);
				val = NULL;
			}

		}
		element++;
	}
	retVal = 0;
      out:
	if (val != NULL)
	{
		free(val);
	}
	return retVal;
}

/* Called when we enter the FILE or TOKEN states.
Note: in theory it would be possible to just store the xml data we
read from a file in the deserialize event into the cache as-is.
However, that has a few downsides:
- If the file has invalid XML or superfluous data, we will write that
same data back later on.
- If we would want to modify the XML format at some undefined point
in the future, it is a good idea generally to ensure that we
already generate new XML data */
int eid_vwr_gen_xml(void *data)
{

	HRESULT retVal = S_OK;
	IXmlWriter *pWriter = NULL;
	IXmlWriterOutput *pWriterOutput = NULL;
	IStream *pMemoryStream = NULL;
	BYTE *pwszContent = NULL;
	STATSTG ssStreamData = { 0 };

	// Opens writeable output stream.
	pMemoryStream = SHCreateMemStream(NULL, 0);
	if (pMemoryStream == NULL)
		return E_OUTOFMEMORY;

	// Creates the xml writer and generates the content.
	FAILED_OUT(retVal = CreateXmlWriter(__uuidof(IXmlWriter), (void **) &pWriter, NULL));
	retVal = CreateXmlWriterOutputWithEncodingName(pMemoryStream, NULL, L"utf-8", &pWriterOutput);
	retVal = pWriter->SetOutput(pWriterOutput);
	retVal = pWriter->SetProperty(XmlWriterProperty_Indent, TRUE);

	//don't write these, they were not present in the previous viewer
	//retVal = pWriter->WriteStartDocument(XmlStandalone_Omit);

	retVal = write_elements(pWriter, toplevel);
	retVal = pWriter->WriteEndDocument();
	retVal = pWriter->Flush();

	// Allocate enough memory for the xml content.

	retVal = pMemoryStream->Stat(&ssStreamData, STATFLAG_NONAME);
	ULONG cbSize = ssStreamData.cbSize.LowPart;

	pwszContent = new BYTE[cbSize];
	if (pwszContent == NULL)
	{
		be_log(EID_VWR_LOG_COARSE, TEXT("Could not generate XML format: error creating the xml buffer"));
		return E_OUTOFMEMORY;
	}
	// Copies the content from the stream to the buffer.
	LARGE_INTEGER position;

	position.QuadPart = 0;
	retVal = pMemoryStream->Seek(position, STREAM_SEEK_SET, NULL);
	ULONG cbRead;

	retVal = pMemoryStream->Read(pwszContent, cbSize, &cbRead);

	cache_add_bin(TEXT("xml"), pwszContent, cbSize);

out:
	if (pwszContent != NULL)
	{
		free(pwszContent);
	}

	return retVal;
}


HRESULT ReadNodePending(IXmlReader * pReader, XmlNodeType * pNodeType,
			int retries)
{
	HRESULT retval = pReader->Read(pNodeType);
	int count = 0;

	while ((retval == E_PENDING) && (count < retries))
	{
		Sleep(1000);
		retval = pReader->Read(pNodeType);
		count++;
	}
	return retval;
}


HRESULT StoreLocalName(WCHAR ** nodeNames, const WCHAR * pwszLocalName)
{
	WCHAR *temp = (WCHAR *) realloc((*nodeNames), sizeof(WCHAR) * (wcslen(pwszLocalName) + 1));
	if (temp == NULL)
	{
		return S_FALSE;
	}

	wcscpy(temp, pwszLocalName);
	*nodeNames = temp;

	return S_OK;
}


HRESULT StoreTextElement(const WCHAR * pwszValue, WCHAR * wcsNodeName)
{
	EID_CHAR *val = NULL;

	struct element_desc *desc = get_elemdesc((EID_CHAR *) wcsNodeName);

	/* If we recognize this element, parse it */
	if (desc != NULL)
	{
		int len = 0;

		{
			val = (EID_CHAR *) convert_from_xml(desc->label, pwszValue, &len);
			if (desc->is_b64)
			{
				//utf16toutf8
				unsigned long length = len;
				char *utf8Char_out = EIDTOUTF8(val, &length);

				//base64decode
				base64_decodestate state_in;
				int decodedLength = 0;
				char *binData = (char *) malloc(length);

				if (binData != NULL)
				{
					base64_init_decodestate(&state_in);
					decodedLength = base64_decode_block(utf8Char_out, length, binData, &state_in);

					cache_add(desc->label, val, len);
					eid_vwr_p11_to_ui((const EID_CHAR *) (desc->label), (const void *)binData, decodedLength);
					be_log(EID_VWR_LOG_DETAIL, TEXT("found data for label %s"), desc->label);
					free(binData);
				}
				if (utf8Char_out != NULL)
				{
					free(utf8Char_out);
				}
			} else
			{
				int len = 0;

				val = (EID_CHAR *)convert_from_xml(desc->label, pwszValue, &len);
				cache_add(desc->label, val, len);
				eid_vwr_p11_to_ui((const EID_CHAR*)(desc->label), (const void *)val, len);
				be_log(EID_VWR_LOG_DETAIL, TEXT("found data for label %s"), desc->label);
				val = NULL;
			}
			val = NULL;
		}
	}

	return S_OK;
}

HRESULT ParseAttributes(IXmlReader * pReader, const WCHAR * pwszLocalName)
{
	const WCHAR *pwszValue;

	EID_CHAR *val = NULL;
	HRESULT retVal = pReader->MoveToFirstAttribute();

	if (retVal != S_OK)
	{
		return retVal;
	} 
	else
	{
		do
		{
			if (!pReader->IsDefault())
			{
				FAILED_OUT(retVal = pReader->GetLocalName(&pwszLocalName, NULL));
				FAILED_OUT(retVal = pReader->GetValue(&pwszValue, NULL));

				struct attribute_desc *desc = get_attdesc((const EID_CHAR *)pwszLocalName);
				/* If we recognize this element, parse it */
				if (desc != NULL)
				{
					int len = 0;

					val = (EID_CHAR *)convert_from_xml(desc->label, pwszValue, &len);
					cache_add(desc->label, val, len);
					eid_vwr_p11_to_ui((const EID_CHAR*)(desc->label), (const void*)val, len);
					be_log(EID_VWR_LOG_DETAIL, TEXT("found data for label %s"), desc->label);
					val = NULL;
				}
			}
		} while (pReader->MoveToNextAttribute() == S_OK);
	}
    out:

	return retVal;
}

//parse the xml file
int eid_vwr_deserialize(const EID_CHAR * filename)
{
	HRESULT retVal = S_OK;
	HRESULT moreData = S_OK;
	IStream *pFileStream = NULL;
	IXmlReader *pReader = NULL;
	XmlNodeType nodeType;
	const WCHAR *pwszLocalName;
	EID_CHAR *nodeName = NULL;
	EID_CHAR *value = NULL;
	EID_CHAR *val = NULL;
	const WCHAR *pwszValue;
	UINT attrCount = 0;
	UINT count = 0;
	UINT depth = 0;
	WCHAR *nodeNames[MAX_ELEMENT_DEPTH + 1];	//also storing depth0

	for (count = 0; count <= MAX_ELEMENT_DEPTH; count++)
	{
		nodeNames[count] = NULL;
	}

	FAILED_OUT(retVal = SHCreateStreamOnFile(filename, STGM_READ, &pFileStream));

	FAILED_OUT(retVal = CreateXmlReader(__uuidof(IXmlReader), (void **) &pReader,  NULL));

	FAILED_OUT(retVal = pReader->SetProperty(XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit));

	FAILED_OUT(retVal = pReader->SetProperty(XmlReaderProperty_MaxElementDepth, MAX_ELEMENT_DEPTH));

	FAILED_OUT(retVal = pReader->SetInput(pFileStream));

	moreData = ReadNodePending(pReader, &nodeType, 20);

	be_newsource(EID_VWR_SRC_FILE);

	while (moreData == S_OK)
	{
		switch (nodeType)
		{
			case XmlNodeType_XmlDeclaration:
				break;
			case XmlNodeType_Element:
				FAILED_OUT(retVal = pReader->GetLocalName(&pwszLocalName, NULL));
				FAILED_OUT(retVal = pReader->GetAttributeCount(&attrCount));
				FAILED_OUT(retVal = pReader->GetDepth(&depth));
				if (depth <= MAX_ELEMENT_DEPTH)
				{
					FAILED_OUT(retVal = StoreLocalName(&(nodeNames[depth]), pwszLocalName));
				}
				if (attrCount > 0)
				{
					retVal = ParseAttributes(pReader, pwszLocalName);
					//continue with other nodes, even if this one failed, so don't check return value
				}
				break;
			case XmlNodeType_EndElement:
				break;
			case XmlNodeType_Text:
				FAILED_OUT(retVal = pReader->GetValue(&pwszValue, NULL));
				FAILED_OUT(retVal = pReader->GetDepth(&depth));
				if (depth <= MAX_ELEMENT_DEPTH)
				{
					FAILED_OUT(retVal = StoreTextElement(pwszValue, nodeNames[depth - 1]));
				}
				break;
			case XmlNodeType_Whitespace:
			case XmlNodeType_CDATA:
			case XmlNodeType_ProcessingInstruction:
			case XmlNodeType_Comment:
			case XmlNodeType_DocumentType:
				break;
		}
		moreData = ReadNodePending(pReader, &nodeType, 20);
	}

out:
	SAFE_RELEASE(pFileStream);
	SAFE_RELEASE(pReader);

	for (count = 0; count <= MAX_ELEMENT_DEPTH; count++)
	{
		if (nodeNames[count] != NULL)
		{
			free(nodeNames[count]);
			nodeNames[count] = NULL;
		}
	}
	if (retVal == S_OK)
	{
		sm_handle_event(EVENT_READ_READY, NULL, NULL, NULL);
	} else
	{
		be_log(EID_VWR_LOG_ERROR, TEXT("Error reading file %s"), filename);
	}

	return retVal;
}

//return the xml data
int eid_vwr_serialize(const EID_CHAR * filename)
{
	int retVal = 0;
	const struct eid_vwr_cache_item *item = cache_get_data(TEXT("xml"));
	FILE *f = EID_FOPEN(filename, TEXT("w"));
	if (f == NULL) {
		return 1;
	}
	fwrite(item->data, item->len, 1, f);

	retVal = fclose(f);
	sm_handle_event(EVENT_SERIALIZE_READY, NULL, NULL, NULL);
	return retVal;
}
