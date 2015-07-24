#include <curl/curl.h>
#include <libxml/xmlstring.h>
#include <backend.h>

/* Send a SOAP ValidateRequest request out to the trust server */
char* eid_vwr_send_soap(xmlChar* data) {
	static CURL* handle;
	char* retval = NULL;
	struct curl_slist *headers = NULL;
	CURLcode res;
	long httprv;

	if(!handle) {
		handle = curl_easy_init();
		curl_easy_setopt(handle, CURLOPT_URL, "https:///trust-ws.services.belgium.be/eid-trust-service-ws/xkms2");
	}
	curl_easy_setopt(handle, CURLOPT_POSTFIELDS, data);
	headers = curl_slist_append(headers, "SOAPAction: ValidateRequest");
	headers = curl_slist_append(headers, "Content-Type: application/soap+xml; charset=utf-8");
	curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
	if((res = curl_easy_perform(handle)) != CURLE_OK) {
		be_log(EID_VWR_LOG_ERROR, "Could not check trust: libcurl returned error %d", res);
		return retval;
	}
	curl_slist_free_all(headers);

	curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &httprv);
	be_log(EID_VWR_LOG_DETAIL, "Trust request returned %ld\n", httprv);

	return retval;
}
