#ifndef EID_VWR_DATAVERIFY_H
#define EID_VWR_DATAVERIFY_H

/**
  * \brief Check that the data on the card is correctly signed.
  *
  * This method will:
  * - Take a hash of the photo, and compare it against the passed photo
  *   hash
  * - Verify the identity signature (which contains the photo hash)
  *   against the passed identity signature file
  * - Concatenate the passed identity file to the passed address file in
  *   the correct manner, and verify the passed address signature against the
  *   result
  *
  * \warning the validity of the RRN certificate is not checked by this
  * function.
  * \see eid_vwr_verify_rrncert() verifies the RRN certificate
  *
  * \param photo the contents of the binary data with label "PHOTO_FILE"
  * \param plen the length in bytes of the preceding parameter
  * \param photohash the contents of the binary data with label
  * "photo_hash"
  * \param hashlen the length in bytes of the preceding parameter
  * \param datafile the contents of the binary data with label
  * "DATA_FILE"
  * \param datfilelen the length in bytes of the preceding parameter
  * \param datasig the contents of the binary data with label
  * "SIGN_DATA_FILE"
  * \param datsiglen the length in bytes of the preceding parameter
  * \param addrfile the contents of the binary data with label
  * "ADDRESS_FILE"
  * \param addfilelen the length in bytes of the preceding parameter
  * \param addrsig the contents of the binary data with label
  * "SIGN_ADDRESS_FILE"
  * \param addsiglen the length in bytes of the preceding parameter
  * \param rrncert the contents of the binary data with label
  * CERT_RN_FILE (i.e., the RRN certificate)
  * \param certlen the length in bytes of the preceding parameter
  * \return 0 if one of the above-described tests indicates invalid
  * data, or 1 if all tests were successful.
  */
int eid_vwr_check_data_validity(const void *photo, int plen,
				const void *photohash, int hashlen,
				const void *datafile, int datfilelen,
				const void *datasig, int datsiglen,
				const void *addrfile, int addfilelen,
				const void *addrsig, int addsiglen,
				const void *rrncert, int certlen);
int eid_vwr_verify_card(void *data);
#endif
