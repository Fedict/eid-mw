#include <curl/curl.h>
#include <libxml/xmlstring.h>
#include <backend.h>
#include <string.h>

static char* curldata = NULL;
static size_t curldatasize = 0;

static size_t newdata(char* ptr, size_t size, size_t nmemb, void *userdata) {
	size_t thisblock = size * nmemb;
	curldata = realloc(curldata, curldatasize + thisblock);
	strncpy(curldata+curldatasize, ptr, thisblock);
	curldatasize += thisblock;
	return thisblock;
}

/* Send a SOAP ValidateRequest request out to the trust server */
char* eid_vwr_send_soap(xmlChar* data) {
	static CURL* handle;
	char* retval = NULL;
	struct curl_slist *headers = NULL;
	CURLcode res;
	long httprv;

	if(!handle) {
		handle = curl_easy_init();
		curl_easy_setopt(handle, CURLOPT_URL, "https://trust-ws.services.belgium.be/eid-trust-service-ws/xkms2");
	}
	curl_easy_setopt(handle, CURLOPT_POSTFIELDS, data);
	headers = curl_slist_append(headers, "SOAPAction: ValidateRequest");
	headers = curl_slist_append(headers, "Content-Type: application/soap+xml; charset=utf-8");
	curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, newdata);
	if((res = curl_easy_perform(handle)) != CURLE_OK) {
		be_log(EID_VWR_LOG_ERROR, "Could not check trust: %s", curl_easy_strerror(res));
		return retval;
	}
	curl_slist_free_all(headers);

	curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &httprv);

	be_log(EID_VWR_LOG_NORMAL, "Trust request returned %ld", httprv);
	be_log(EID_VWR_LOG_DETAIL, "Data received: %s", data);

	free(curldata);
	curldata = NULL;
	curldatasize = 0;

	return retval;
}
