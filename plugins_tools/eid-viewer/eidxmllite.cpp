#include <ole2.h> 
#include <xmllite.h> 
#include <stdio.h> 
#include <shlwapi.h> 
#include "eidxmllite.h"
#include "xmlmap.h"
#include "backend.h"
#include "p11.h"

const UINT MAX_ELEMENT_DEPTH = 8;

#pragma warning(disable : 4127)  // conditional expression is constant 
//#define CHKHR(stmt)             do { hr = (stmt); if (FAILED(hr)) goto CleanUp; } while(0) 
//#define HR(stmt)                do { hr = (stmt); goto CleanUp; } while(0) 
#define SAFE_RELEASE(I)         if (I){ I->Release();  I = NULL; }
#define SAFE_FREE(val)			if (val != NULL){ free(val); val = NULL;}
#define FAILED_OUT(retVal)		if (retVal != 0){goto out;}

#define check_xml(call) if((rc = call) < 0) { \
	be_log(EID_VWR_LOG_DETAIL, "Error while dealing with file (calling '%s'): %d", #call, rc); \
	goto out; \
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
	char* nodeName = NULL;
	char* value = NULL;
	void* val = NULL;

	if ((ConvertWCharToMultiByte(wcsNodeName, &nodeName) == 0) &&
		(ConvertWCharToMultiByte(pwszValue, &value) == 0))
	{
		struct element_desc *desc = get_elemdesc((const char*)nodeName);
		/* If we recognize this element, parse it */
		if (desc != NULL) {
			int len = 0; {
				val = convert_from_xml(desc->label, value, &len);
				cache_add(desc->label, val, len);
				eid_vwr_p11_to_ui((const char*)(desc->label), (const void*)val, len);
				be_log(EID_VWR_LOG_DETAIL, "found data for label %s", desc->label);
				val = NULL;
			}
		}
	}
	SAFE_FREE(nodeName);
	SAFE_FREE(value);

	return S_OK;
}

HRESULT ParseAttributes(IXmlReader* pReader, const WCHAR* pwszLocalName)
{
	const WCHAR* pwszValue;
	char* nodeName = NULL;
	char* value = NULL;
	void* val = NULL;
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

				if ((ConvertWCharToMultiByte(pwszLocalName, &nodeName) == 0) &&
					(ConvertWCharToMultiByte(pwszValue, &value) == 0))
				{

					struct attribute_desc *desc = get_attdesc((const char*)nodeName);
					/* If we recognize this element, parse it */
					if (desc != NULL) {
						int len = 0; {
							val = convert_from_xml(desc->label, value, &len);
							cache_add(desc->label, val, len);
							eid_vwr_p11_to_ui((const char*)(desc->label), (const void*)val, len);
							be_log(EID_VWR_LOG_DETAIL, "found data for label %s", desc->label);
							val = NULL;
						}
					}
				}
			}
		}
	}
out:
	SAFE_FREE(nodeName);
	SAFE_FREE(value);

	return retVal;
}

int eid_vwr_do_deserialize (char* filename)
{
	HRESULT retVal = S_OK;
	IStream *pFileStream = NULL;
	IXmlReader *pReader = NULL;
	XmlNodeType nodeType;
	const WCHAR* pwszLocalName;
	char* nodeName = NULL;
	char* value = NULL;
	void* val = NULL;
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

/*// This is a sample of how one might handle E_PENDING
if (PENDING(hr = pReader->GetValue(&pwszValue, NULL))){
    // Alert user to the pending notification
    wprintf(L"Error pending, error is %08.8lx", hr);

        // As long as E_PENDING is returned keep trying to get value
        while (PENDING(hr){
            ::sleep(1000);
            hr = pReader->GetValue(&pwszValue, NULL);
        }
        continue;
    }
if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))){
    wprintf(L"Error getting value, error is %08.8lx", hr);
    return -1;
}
wprintf(L"Text: %s\n", pwszValue);*/