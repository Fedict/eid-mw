//We do the verification in the viewerUI, so no dependencies for openSSL are needed

//return 0 for succes
int eid_vwr_verify_card(void* d) {
	//const struct eid_vwr_cache_item *photo, *phash, *data, *datsig, *address, *adsig, *cert;

	return 0; // default to success so UI reaches TOKEN_WAIT and performs validation
}

//return 1 for succes
int eid_vwr_check_data_validity(const void *photo, int plen,
	const void *photohash, int hashlen,
	const void *datafile, int datfilelen,
	const void *datasig, int datsiglen,
	const void *addrfile, int addfilelen,
	const void *addrsig, int addsiglen,
	const void *rrncert, int certlen)
{

	return 1; // default to success; validation failure will be simulated in C# layer
}