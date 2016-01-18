#include <ole2.h> 
#include <xmllite.h> 
#include <stdio.h> 
#include <shlwapi.h> 
#include "eidxmllite.h"
#include "xmlmap.h"
#include "backend.h"
#include "p11.h"

const UINT MAX_ELEMENT_DEPTH = 8;

#define SAFE_RELEASE(I)         if (I){ I->Release();  I = NULL; }
#define SAFE_FREE(val)			if (val != NULL){ free(val); val = NULL;}
#define FAILED_OUT(retVal)		if ((retVal) != 0){goto out;}

#define check_xml(call) if((rc = call) < 0) { \
	be_log(EID_VWR_LOG_DETAIL, TEXT("Error while dealing with file (calling '%s'): %d"), #call, rc); \
	goto out; \
}

/* Write elements to the description in *element */
/*static int write_elements(IXmlWriter *pWriter, struct element_desc *element) {
	HRESULT retVal;
	char* val = NULL;
	while (element->name) {
		if (element->label == NULL) {
			//assert(element->child_elements != NULL);
			//check_xml(xmlTextWriterStartElement(writer, BAD_CAST element->name));
			pWriter->WriteStartElement(NULL,element->name,NULL);
			if (element->attributes != NULL) {
				write_attributes(pWriter, element->attributes);
				//retVal = pWriter->WriteAttributeString(NULL,
				//	element->name, NULL,
				//	element->attributes);
			}
			check_xml(write_elements(writer, element->child_elements));
			//check_xml(xmlTextWriterEndElement(writer));
			pWriter->WriteEndElement();
		}
		else {
			int have_cache = cache_have_label(element->label);
			assert(element->child_elements == NULL);

			if (element->reqd && !have_cache) {
				be_log(EID_VWR_LOG_ERROR, "Could not write file: no data found for required label %s", element->label);
				return -1;
			}
			if (have_cache) {
				val = cache_get_xmlform(element->label);
				if (!element->is_b64) {
					check_xml(xmlTextWriterWriteElement(writer, BAD_CAST element->name, BAD_CAST cache_get_xmlform(element->label)));
				}
				else {
					const struct eid_vwr_cache_item *item = cache_get_data(element->label);
					check_xml(xmlTextWriterStartElement(writer, BAD_CAST element->name));
					check_xml(xmlTextWriterWriteBase64(writer, item->data, 0, item->len));
					check_xml(xmlTextWriterEndElement(writer));
				}
				free(val);
				val = NULL;
			}
		}
		element++;
	}
	rc = 0;
out:
	if (val != NULL) {
		free(val);
	}
	return rc;
}*/

/* Called when we enter the FILE or TOKEN states.
Note: in theory it would be possible to just store the xml data we
read from a file in the deserialize event into the cache as-is.
However, that has a few downsides:
- If the file has invalid XML or superfluous data, we will write that
same data back later on.
- If we would want to modify the XML format at some undefined point
in the future, it is a good idea generally to ensure that we
already generate new XML data */
int eid_vwr_gen_xml(void* data) {
	/*HRESULT retVal = S_OK;
	IStream *pFileStream = NULL;
	IXmlWriter *pWriter = NULL;

	FAILED_OUT(retVal = CreateXmlWriter(__uuidof(IXmlWriter), (void**)&pWriter, NULL));
	//xmlTextWriterPtr writer = NULL;
	//int rc;
	xmlBufferPtr buf;

	buf = xmlBufferCreate();
	if (buf == NULL) {
	be_log(EID_VWR_LOG_COARSE, "Could not generate XML format: error creating the xml buffer");
	rc = -1;
	goto out;
	}
	writer = xmlNewTextWriterMemory(buf, 0);
	if (writer == NULL) {
	be_log(EID_VWR_LOG_ERROR, "Could not open file");
	rc = -1;
	goto out;
	}

	check_xml(xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL));
	check_xml(write_elements(writer, toplevel));
	check_xml(xmlTextWriterEndDocument(writer));

	cache_add("xml", buf->content, strlen(buf->content));

	rc = 0;
	out:
	if (writer) {
	xmlFreeTextWriter(writer);
	}
	if (buf) {
	xmlBufferFree(buf);
	}
	return retVal;*/
	return 0;
}


HRESULT ReadNodePending(IXmlReader *pReader, XmlNodeType *pNodeType, int retries)
{
	HRESULT retval = pReader->Read(pNodeType);
	int count = 0;
	while ((retval == E_PENDING) && (count < retries)) {
		Sleep(1000);
		retval = pReader->Read(pNodeType);
		count++;
	}
	return retval;
}


HRESULT StoreLocalName(WCHAR** nodeNames, const WCHAR* pwszLocalName)
{
	WCHAR* temp = (WCHAR*)realloc((*nodeNames), sizeof(WCHAR) * (wcslen(pwszLocalName)+1) );
	if (temp == NULL)
		return S_FALSE;

	wcscpy(temp, pwszLocalName);
	*nodeNames = temp;

	return S_OK;
}

int ConvertWCharToMultiByte(const wchar_t* wsIn, char** bsOut)
{
	size_t buffersize = 256;
	char* buffer = NULL;
	size_t numOfCharConverted = 0;

	//retrieve the needed buffersize
	wcstombs_s(&numOfCharConverted, NULL, 0, wsIn, 0);
	buffer = (char*)calloc(numOfCharConverted, 1);
	if (buffer == NULL)
		return -1;
	*bsOut = buffer;

	buffersize = numOfCharConverted;

	return wcstombs_s(&numOfCharConverted, buffer, buffersize, wsIn, buffersize - 1);
	
}

HRESULT StoreTextElement(const WCHAR* pwszValue, WCHAR* wcsNodeName)
{
//	EID_CHAR* nodeName = NULL;
//	EID_CHAR* value = NULL;
	void* val = NULL;

//	if ((ConvertWCharToMultiByte(wcsNodeName, &nodeName) == 0) &&
//		(ConvertWCharToMultiByte(pwszValue, &value) == 0))
//	{
		struct element_desc *desc = get_elemdesc((EID_CHAR*)wcsNodeName);
		/* If we recognize this element, parse it */
		if (desc != NULL) {
			int len = 0; {
				val = convert_from_xml(desc->label, pwszValue, &len);
				cache_add(desc->label, val, len);
				eid_vwr_p11_to_ui((const EID_CHAR*)(desc->label), (const void*)val, len);
				be_log(EID_VWR_LOG_DETAIL, TEXT("found data for label %s"), desc->label);
				val = NULL;
			}
		}
//	}
//	SAFE_FREE(nodeName);
//	SAFE_FREE(value);

	return S_OK;
}

HRESULT ParseAttributes(IXmlReader* pReader, const WCHAR* pwszLocalName)
{
	const WCHAR* pwszValue;
//	EID_CHAR* nodeName = NULL;
//	EID_CHAR* value = NULL;
	EID_CHAR* val = NULL;
	HRESULT retVal = pReader->MoveToFirstAttribute();

	if (retVal != S_OK)
	{
		return retVal;
	}
	else
	{
		while (pReader->MoveToNextAttribute() == S_OK)
		{
			if (!pReader->IsDefault())
			{
				FAILED_OUT(retVal = pReader->GetLocalName(&pwszLocalName, NULL));
				FAILED_OUT(retVal = pReader->GetValue(&pwszValue, NULL));

//				if ((ConvertWCharToMultiByte(pwszLocalName, &nodeName) == 0) &&
//					(ConvertWCharToMultiByte(pwszValue, &value) == 0))
//				{

					struct attribute_desc *desc = get_attdesc((const EID_CHAR*)pwszLocalName);
					/* If we recognize this element, parse it */
					if (desc != NULL) {
						int len = 0; {
							val = (EID_CHAR*)convert_from_xml(desc->label, pwszValue, &len);
							cache_add(desc->label, val, len);
							eid_vwr_p11_to_ui((const EID_CHAR*)(desc->label), (const void*)val, len);
							be_log(EID_VWR_LOG_DETAIL, TEXT("found data for label %s"), desc->label);
							val = NULL;
						}
					}
				}
//			}
		}
	}
out:
//	SAFE_FREE(nodeName);
//	SAFE_FREE(value);

	return retVal;
}

int eid_vwr_do_deserialize (const EID_CHAR* filename)
{
	HRESULT retVal = S_OK;
	IStream *pFileStream = NULL;
	IXmlReader *pReader = NULL;
	XmlNodeType nodeType;
	const WCHAR* pwszLocalName;
	EID_CHAR* nodeName = NULL;
	EID_CHAR* value = NULL;
	EID_CHAR* val = NULL;
	const WCHAR* pwszValue;
	UINT attrCount = 0;
	UINT count = 0;
	UINT depth = 0;
	WCHAR* nodeNames[MAX_ELEMENT_DEPTH+1]; //also storing depth0

	for (count = 0; count <= MAX_ELEMENT_DEPTH; count++)
	{
		nodeNames[count] = NULL;
	}

	FAILED_OUT(retVal = SHCreateStreamOnFile(filename, STGM_READ, &pFileStream));
	FAILED_OUT(retVal = CreateXmlReader(__uuidof(IXmlReader), (void**)&pReader, NULL));
	FAILED_OUT(retVal = pReader->SetProperty(XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit));
	FAILED_OUT(retVal = pReader->SetProperty(XmlReaderProperty_MaxElementDepth, MAX_ELEMENT_DEPTH));
	FAILED_OUT(retVal = pReader->SetInput(pFileStream));

	retVal = ReadNodePending(pReader, &nodeType, 20);

	while (retVal == S_OK )
	{
		switch (nodeType)
		{
		case XmlNodeType_XmlDeclaration:
			break;
		case XmlNodeType_Element:
			FAILED_OUT(retVal = pReader->GetLocalName(&pwszLocalName, NULL));
			FAILED_OUT(retVal = pReader->GetAttributeCount(&attrCount));
			FAILED_OUT(retVal = pReader->GetDepth(&depth));
			if(depth <= MAX_ELEMENT_DEPTH)
				FAILED_OUT(retVal = StoreLocalName(&(nodeNames[depth]), pwszLocalName ));
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
				FAILED_OUT(retVal = StoreTextElement(pwszValue, nodeNames[depth-1]));
			break;
		case XmlNodeType_Whitespace:
		case XmlNodeType_CDATA:
		case XmlNodeType_ProcessingInstruction:
		case XmlNodeType_Comment:
		case XmlNodeType_DocumentType:
			break;
		}
		retVal = ReadNodePending(pReader, &nodeType, 20);
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
	return 0;
}
