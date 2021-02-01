#include "cache.h"
#include "backend.h"

#include "xml.h"
#include "xmlmap.h"

#include <string.h>
#include "p11.h"

#ifdef WIN32
#include <stdio.h>
#include <tchar.h>
//#include <objbase.h>
//#include <msxml6.h>
#include "eidxmllite.h"
#endif

#include <assert.h>



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
	/*xmlTextWriterPtr writer = NULL;
	int rc;
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
	return rc;*/
	return 0;

	/*
	//Open writeable output stream
	if (FAILED(hr = SHCreateStreamOnFile(output_file_name, STGM_CREATE | STGM_WRITE, &pOutFileStream)))
	{
		wprintf(L"Error creating file writer, error is %08.8lx", hr);
		HR(hr);
	}

	if (FAILED(hr = CreateXmlWriter(__uuidof(IXmlWriter), (void**)&pWriter, NULL)))
	{
		wprintf(L"Error creating xml writer, error is %08.8lx", hr);
		HR(hr);
	}

	if (FAILED(CreateXmlWriterOutputWithEncodingName(pOutFileStream, NULL, L"UTF-8", &pWriterOutput))) {
		wprintf(L"Error setting xml encoding, error is %08.8lx", hr);
		HR(hr);
	}

	if (FAILED(hr = pWriter->SetOutput(pWriterOutput)))
	{
		wprintf(L"Error, Method: SetOutput, error is %08.8lx", hr);
		HR(hr);
	}

	if (FAILED(hr = pWriter->SetProperty(XmlWriterProperty_Indent, 4)))
	{
		wprintf(L"Error, Method: SetProperty XmlWriterProperty_Indent, error is %08.8lx", hr);
		HR(hr);
	}

	if (FAILED(hr = pWriter->WriteStartDocument(XmlStandalone_Omit)))
	{
		wprintf(L"Error, Method: WriteStartDocument, error is %08.8lx", hr);
		HR(hr);
	}*/
}

/* Read data from the cache and store it to the file whose name we get
* in the *data argument */
int eid_vwr_serialize(void* data) {
	const struct eid_vwr_cache_item* item = cache_get_data(TEXT("xml"));
	FILE* f = fopen((const char*)data, "w");
	fwrite(item->data, item->len, 1, f);
	return fclose(f);
}

int eid_vwr_deserialize(EID_CHAR* data) {
	return eid_vwr_do_deserialize(data);
}
